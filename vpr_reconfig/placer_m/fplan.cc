// Project: B*-trees floorplanning
// Advisor: Yao-Wen Chang  <ywchang@cis.nctu.edu.tw>
// Authors: Jer-Ming Hsu   <barz@cis.nctu.edu.tw>
// 	    Hsun-Cheng Lee <gis88526@cis.nctu.edu.tw>
// Sponsor: Arcadia Inc.
// Date:    7/19/2000 ~

//---------------------------------------------------------------------------
#include <fstream>
#include <cstdio>
#include <cstring>
#include <climits>
#include <cassert>
#include <iostream>
#include <ctime>
#include <sstream>
#include <math.h>
#include <time.h>
#include <algorithm>

using namespace std;
//#include <sys/time.h>
//#include <sys/resource.h>

#include "fplan.h"

#include "RouteFrame.h"
//#include "SCTypeDef.h"
#include "SCDesign.h"
#include "SCglobals.h"
#include "scgroute.h"
double congestCoefficient=1.0;    //拥塞的权系数     cost_alpha是FPlan的输入参数

//---------------------------------------------------------------------------
char line[100],t1[80],t2[80], t3[80], t4[80];
char tnum1[80],tnum2[80], tnum3[80], tnum4[80];
ifstream fs;

//add by fbmao
//sometimes hard module and soft modules are in the same file
ifstream fsnumber; //get the number (how many different ratio for each blocks)

FPlan::FPlan(double calpha=1){
  norm_area= 1;
  norm_wire= 1;
  norm_delay = 1;
  norm_congest = 1;
  cost_alpha=calpha;

  //for congestion evaluation  congestion_weight = terminal/maxnumterminal
  maxNumTerminal = 1; 
  maxnumratio = 1; //at least have one ratio

  outline_width = 0; //for isFit function. Do not consider fixed outline
  outline_height = 0;  //in this case: make isFIt always false

//support heterogeneous architecture and modules with hetero blocks
  //memory
  fmemstart = 0;
  fmemrepeat = 0;
  fmemoryheight = 0;

  //multiply
  fmultistart = 0;
  fmultirepeat = 0;
  fmultiheight = 0;

  //total number of RAM, DSP
  fmemorynum = 0;
  fmultiplynum = 0;
}

void FPlan::packing(){
  if(cost_alpha!=1)
  {

	 if(1 == io3)   //io3 default 0
	      calcWireLengthConsidermoduleiopos();
	 else
	 calcWireLength();

     calIntraDelay();	 
  }
}

void FPlan::clear(){
  Area = 0; 
  WireLength = 0;
  IntraDelay = 0;
  Congest = 0;
}

double FPlan::getCost()
{
  //if(cost_alpha==1)
  //   return cost_alpha*(Area/norm_area);
  //else if(cost_alpha < 1e-4)
  //   return (WireLength/norm_wire);
  //else
  //   return cost_alpha*(Area/norm_area)+(1-cost_alpha)*(WireLength/norm_wire);
 
  // outline cost

  double ar = getHeight()/getWidth();
  double ar_cost = ar - outline_ratio;
  ar_cost = ar_cost * ar_cost;
  if( outline_ratio <= 0 )
      ar_cost = 0;

  //for test 
 // cout<<"cost_alpha "<<cost_alpha<<" cost_beta "<<cost_beta<<endl;


  //modified by fbmao
  if( cost_alpha == 1 )
      return cost_alpha*(Area/norm_area) + ar_cost;

  if(false == calcongest)
	   return cost_alpha*(Area/norm_area) + (1-cost_alpha)*((WireLength/norm_wire)*cost_beta + (IntraDelay/norm_delay)*(1-cost_beta)) + k3*ar_cost;
  else 
	  return cost_alpha*(Area/norm_area) + cost_alpha*(WireLength/norm_wire) +(1-cost_alpha-cost_alpha)*(Congest/norm_congest)+ k3*ar_cost; 
  

  /*
  if( cost_alpha == 1 )
      return cost_alpha*(Area/norm_area) + ar_cost;


 // return cost_alpha*(Area/norm_area) + (1-cost_alpha)*(WireLength/norm_wire) + k3*ar_cost;
  if(false == calcongest)
	  return cost_alpha*(Area/norm_area) + (1-cost_alpha)*((WireLength/norm_wire)*0.5 + (IntraDelay/norm_delay)*0.5) + k3*ar_cost;
  else 
	  return cost_alpha*(Area/norm_area) + cost_alpha*(WireLength/norm_wire) +(1-cost_alpha-cost_alpha)*(Congest/norm_congest)+ k3*ar_cost; 
*/
}

double FPlan::getDeadSpace()
{
  return 100*(Area-TotalArea)/double(Area);
}

void FPlan::normalize_cost(int t)
{
  norm_area = norm_wire = norm_delay = norm_congest = 0;

  for(int i=0; i < t; i++)
  {
    perturb();
    packing();


    norm_area += Area;
    norm_wire += WireLength;
	norm_delay += IntraDelay;
    norm_congest += Congest; //mao add
  }
  
  norm_area /= t;
  norm_wire /= t;
  norm_delay /= t;
  norm_congest /= t;
  printf("normalize area=%.6f, wire=%.5f, intradelay=%.5f, congest=%.3f\n", norm_area*1e-6, norm_wire, norm_delay*1e9, norm_congest);  //need to confirm 1e-6
}

//---------------------------------------------------------------------------
//   Read
//---------------------------------------------------------------------------

char* tail(char *str){
    str[strlen(str)-1]=0;
    return str;
}

void FPlan::read(char *file)
{
  filename = file; 
  //read 
  string tempfilename = filename + "/" + filename + ".txt";
//  fs.open(file);
  fs.open(tempfilename.c_str());
  fsnumber.open(tempfilename.c_str()); //fbmao for different ratio
  if(!fs || !fsnumber)
  {
    error("unable to open file: %s",file);
	exit(1);
  }

  bool final=false;
  Module dummy_mod;

  //fsnumber should use before fs in order to avoid affect the value used for module
  fsnumber >> t1 >> t2 >> t3 >> t4; // the first comment line, at the beginning of the file
  fs >> t1 >> t2 >> t3 >> t4; // the first comment line, at the beginning of the file
  for(int i=0; !fs.eof(); i++){
    // modules
    modules.push_back(dummy_mod);	// new module
    Module &mod = modules.back();
    mod.id = i;
    mod.pins.clear();
    mod.no_rotate = false;
	
	fsnumber >> t1 >> t2;
    fs >> t1 >> t2;
    tail(t2);
    strcpy(mod.name, t2);
    //dimension aspec area delay
    read_dimension(mod);
  
 //   fs >> t1 >> t2;
 //   tail(t2);			// remove ";"
 //   strcpy(mod.name,t2);

 //   fs >> t1 >> t2;
 //   if(!strcmp(t2,"PARENT;"))
	//final= true;
 //   
 //   // dimension
 //   read_dimension(mod);    
 //   read_IO_list(mod,final);

 //   // network
 //   if(final){
 //     read_network();
 //     break;
 //   }
  } //for

  //root_module = modules.back();
  //modules.pop_back();		// exclude the parent module
  //modules_N = (int)modules.size();  
  //modules_info.resize(modules_N);
  //modules.resize(modules_N);

  //create_network();

  //TotalArea = 0;
  //for(int i=0; i < modules_N; i++)
  //  TotalArea += modules[i].area;

  fs.close();
  fsnumber.close();
  root_module = modules.back();
//the module file format is changed, so now it doesnot contain parent module
//  modules.pop_back();		// exclude the parent module
  modules_N = (int)modules.size();  
  modules_info.resize(modules_N);
  modules.resize(modules_N);

//  create_network();

  TotalArea = 0;
  for(int i=0; i < modules_N; i++)
    TotalArea += modules[i].area;

}

void FPlan::read_dimension(Module &mod)
{

	//MODULE A;
	//DIMENSIONS 
	//Aspec area delay
	//0.2    5    6
	//0.4    3    4
	//   0.6    7	8
	//0.8	   10   12
	//1.0    5    6
	//ENDMODULE;
	//MODULE B;
	//DIMENSIONS 
	//Aspec area delay
	//0.2    5    6
	//0.4    3    4
	//   0.6    7	8
	//0.8	   10   12
	//1.0    5    6
	//ENDMODULE;
	//MODULE C;
	//DIMENSIONS 
	//Aspec area delay
	//0.2    5    6
	//0.4    3    4
	//   0.6    7	8
	//0.8	   10   12
	//1.0    5    6
	//ENDMODULE;

	//fbmao add for supporting heterogeneous arch
	mod.ratioxyzblkname.clear(); //store blks' coordinate
	mod.finalxyzblkname.clear();

	//aspec = width/height;   nx/ny
	mod.aspeWidHeightDelay.clear();
	mod.aspec.clear();
	map<float, float> tempAreaDelayAspec;  //used for sort the five groups
	tempAreaDelayAspec.clear();
	float modaspec;
	int modwidth;
	string tempstr;
	int modheight;
	float moddelay;

	//First: get the ratio number (the number of ratio), then used it for storing module information
	int numratio = 0; //should at least >=1, at least have one ratio
	char tnum[80];
	float tmodaspec;
	int tmodwidth;
	string ttempstr;
	int tmodheight;
	float tmoddelay;
	fsnumber >> t1; //DIMENSIONS
	fsnumber >> t1 >> t2 >> t3;  //aspec area delay 
	//(eg format) 1.0   3 x 3   8.11621e-10
	while(true)
	{
		fsnumber >> tnum;
		if(0 == strcmp(tnum, "ENDMODULE;"))  //for each module fsnum will stop when find "ENDMODULE;"
		{
			break;
		}
		else 
		{  //if not equal to "ENDMODULE;", means its ratio value, only need to read the next number except first one
			//before: fsnumber >> modaspec >> modwidth >> tempstr >> modheight >> moddelay;  //give a initial value to module
			//now:  fsnumber >> modwidth >> tempstr >> modheight >> moddelay;  //give a initial value to module
			fsnumber >> tmodwidth >> ttempstr >> tmodheight >> tmoddelay;  //give a initial value to module
			numratio++;
		}
	}
	assert(numratio >= 1); //add by fbmao

	//add for exploration, sa parameter according to the module size
	if(numratio > maxnumratio)
		maxnumratio = numratio;

	//Then read the module information using another fs
	fs >> t1; //DIMENSIONS
	fs >> t1 >> t2 >> t3;  //aspec area delay 
	fs >> modaspec >> modwidth >> tempstr >> modheight >> moddelay;  //give a initial value to module

	//how many blank column,  RAM and DSP. and then remove blank column
	int tmpblankRamNum = 0;
	int tmpblankDspNum = 0;
	//remove blank RAM, DSP
	updateSubModuleWidthAndHeight(mod.name, modaspec, modwidth, modheight, tmpblankRamNum, tmpblankDspNum, mod);
	//update modwidth 
	modwidth = modwidth - tmpblankRamNum - tmpblankDspNum;

	//No matter whether the module contains the RAM or DSP or not. record all blk's coordinate for moving to FPGA architecture conveniently
	//mod.name  mod.ratio find the placefile  memory,RAM number . update the blocks coordinates..  when remove RAM DSP.

	//update modwidth and blks' coordinate after removing blank memory and multiply
	////for support heterogenous arch, here should change the width (eg,arch contain memory, multiply etc.)

	tempAreaDelayAspec.insert(make_pair((modwidth * modheight * moddelay), modaspec));
	mod.x = 0;
	mod.y = 0;
	mod.width =   modwidth; //int(sqrt(taspec * tarea));
	mod.height =  modheight; //int(sqrt(tarea / taspec));
	mod.area = modwidth * modheight;
	mod.delay = moddelay;  //approximately,  since remove the blank RAM, DSP.
	//use area*delay as a weight to judge whether the importance of the modules 
	mod.aspeWidHeightDelay.insert(make_pair(modaspec, make_pair(make_pair(modwidth, modheight),moddelay)));
	for(int i = 0; i < numratio - 1; i++)
	{
		fs >> modaspec >> modwidth >> tempstr >> modheight >> moddelay;
		//for support heterogenous arch, here should change the width (eg,arch contain memory, multiply etc.)
		//remove blank RAM, DSP
		tmpblankRamNum = 0;
		tmpblankDspNum = 0;
		//remove the blank RAM, DSP
		updateSubModuleWidthAndHeight(mod.name, modaspec, modwidth, modheight, tmpblankRamNum, tmpblankDspNum, mod);
		//update modwidth 
		modwidth = modwidth - tmpblankRamNum - tmpblankDspNum;

		//b = (int)(modaspec * 10);
		//   modaspec = b / 10.0;
		//		  modaspec = (float)((int)(modaspec * 10))/10;   //keep one position  eg 21.111,   get the valule 21.1
		if(0 == rd || (1 == rd && modwidth*modheight > smallarea)) //8  //smallarea default 8 smallareacriterion   //rd == 1 consider the ratio driven (that small module only have one aspect ratios)
		{
			mod.aspeWidHeightDelay.insert(make_pair(modaspec, make_pair(make_pair(modwidth, modheight),moddelay)));
			tempAreaDelayAspec.insert(make_pair((modwidth * modheight * moddelay), modaspec));
		}
	}

	for(map<float, float>::iterator it = tempAreaDelayAspec.begin(); it != tempAreaDelayAspec.end(); it++)
	{
		mod.aspec.push_back((*it).second);
	}

	fs >> t1;  //"ENDMODULE;"
}

/*  //it works only for the module with 5 aspect ratio
void FPlan::read_dimension(Module &mod){

	//MODULE A;
	//DIMENSIONS 
	//Aspec area delay
	//0.2    5    6
	//0.4    3    4
 //   0.6    7	8
	//0.8	   10   12
	//1.0    5    6
	//ENDMODULE;
	//MODULE B;
	//DIMENSIONS 
	//Aspec area delay
	//0.2    5    6
	//0.4    3    4
 //   0.6    7	8
	//0.8	   10   12
	//1.0    5    6
	//ENDMODULE;
	//MODULE C;
	//DIMENSIONS 
	//Aspec area delay
	//0.2    5    6
	//0.4    3    4
 //   0.6    7	8
	//0.8	   10   12
	//1.0    5    6
	//ENDMODULE;

	//aspec = width/height;   nx/ny
	mod.aspeWidHeightDelay.clear();
	mod.aspec.clear();
	map<float, float> tempAreaDelayAspec;  //used for sort the five groups
	tempAreaDelayAspec.clear();
    float modaspec;
	int modwidth;
	string tempstr;
	int modheight;
	float moddelay;
	
	fs >> t1; //DIMENSIONS
	fs >> t1 >> t2 >> t3;  //aspec area delay 
	fs >> modaspec >> modwidth >> tempstr >> modheight >> moddelay;  //give a initial value to module
	//string tmpRatio = string.Format(modaspec, "0.00");
	//modaspec = atof(tmpRatio.c_str());
	//int b = (int)(modaspec * 10);
	//modaspec = b / 10.0;
//	modaspec = (float)(((int)(modaspec * 10))/10);   //keep one position  eg 21.111,   get the valule 21.1
	tempAreaDelayAspec.insert(make_pair((modwidth * modheight * moddelay), modaspec));
	mod.x = 0;
	mod.y = 0;
	mod.width =   modwidth; //int(sqrt(taspec * tarea));
	mod.height =  modheight; //int(sqrt(tarea / taspec));
	mod.area = modwidth * modheight;
	mod.delay = moddelay;
    //use area*delay as a weight to judge whether the importance of the modules 
    mod.aspeWidHeightDelay.insert(make_pair(modaspec, make_pair(make_pair(modwidth, modheight),moddelay)));
	for(int i = 0; i < 4; i++){
	      fs >> modaspec >> modwidth >> tempstr >> modheight >> moddelay;
		  //b = (int)(modaspec * 10);
	   //   modaspec = b / 10.0;
//		  modaspec = (float)((int)(modaspec * 10))/10;   //keep one position  eg 21.111,   get the valule 21.1
		  mod.aspeWidHeightDelay.insert(make_pair(modaspec, make_pair(make_pair(modwidth, modheight),moddelay)));
		  tempAreaDelayAspec.insert(make_pair((modwidth * modheight * moddelay), modaspec));
	}
	for(map<float, float>::iterator it = tempAreaDelayAspec.begin(); it != tempAreaDelayAspec.end(); it++){
		mod.aspec.push_back((*it).second);
	}

   fs >> t1;

    //fs >> t1;
    //int min_x=INT_MAX,min_y=INT_MAX,max_x=INT_MIN,max_y=INT_MIN;
    //int tx,ty;
    //for(int i=0; i < 4;i++){
    //  fs >> tx >> ty; 
    //  min_x=min(min_x,tx); max_x=max(max_x,tx);
    //  min_y=min(min_y,ty); max_y=max(max_y,ty);
    //}

    //mod.x      = min_x;
    //mod.y      = min_y;
    //mod.width  = max_x - min_x;
    //mod.height = max_y - min_y;
    //mod.area   = mod.width * mod.height;
    //fs >> t1 >> t2;
}
*/



void FPlan::read_IO_list(Module &mod,bool parent=false){
    // IO list
    while(!fs.eof()){
      Pin p;
      fs.getline(line,100);
      if(strlen(line)==0) continue;
      sscanf(line,"%s %*s %d %d",t1,&p.x,&p.y);

      if(!strcmp(t1,"ENDIOLIST;"))
	break;

      if(parent){ // IO pad is network
       // make unique net id
        net_table.insert(make_pair(string(t1),net_table.size()));
        p.net = net_table[t1];
      }

      p.mod = mod.id;
      p.x -= mod.x;  p.y -= mod.y;	// shift to origin

      mod.pins.push_back(p);
    }
    fs.getline(line,100);
}

void FPlan::read_network(){
    while(!fs.eof()){
      bool end=false;
      int n=0;
      fs >> t1 >> t2;
      if(!strcmp(t1,"ENDNETWORK;"))
        break;
      // determine which module interconnection by name
      int m_id;
      for(m_id=0; m_id < modules.size(); m_id++)
        if(!strcmp(modules[m_id].name,t2))
   	  break;
      if(m_id== modules.size())
 	error("can't find suitable module name!");
        
      while(!fs.eof()){
        fs >> t1;
        if(t1[strlen(t1)-1]==';'){
 	  tail(t1);
          end=true;
        }

        // make unique net id   //pad net is also unique
        net_table.insert(make_pair(string(t1),net_table.size()));
        modules[m_id].pins[n++].net = net_table[t1];
        if(end) break;
      }
    }
}

void FPlan::read_simple(char *file)
{
  filename = file; 
  string tempfile = filename + "/" + filename;
  fs.open(tempfile.c_str());
  if(!fs)
    error("unable to open file: %s",file);

  Module dummy_mod;
  for(int i=0; !fs.eof(); i++)
  {
    // modules
    modules.push_back(dummy_mod);	// new module
    Module &mod = modules.back();
    mod.id = i;
    mod.pins.clear();
    mod.no_rotate = false;

    fs >> t2;
    strcpy(mod.name, t2);

    fs >> t1 >> t2;
	mod.width = atoi( t1 );
	mod.height = atoi( t2 );
	mod.area = mod.width * mod.height;
  }

  root_module = modules.back();
  modules.pop_back();		// exclude the parent module
  modules_N = modules.size();  
  modules_info.resize(modules_N);
  modules.resize(modules_N);

  //create_network();

  TotalArea = 0;
  for(int i=0; i < modules_N; i++)
    TotalArea += modules[i].area;


}




//---------------------------------------------------------------------------
//   Wire Length Estimate
//---------------------------------------------------------------------------

void FPlan::create_network(){
  network.resize(net_table.size());

  for(int i=0; i < modules_N; i++){
    for(int j=0; j < modules[i].pins.size(); j++){
      Pin &p = modules[i].pins[j];
      network[p.net].push_back(&p);
    }
  }

  for(int j=0; j < root_module.pins.size(); j++){
    Pin &p = root_module.pins[j];
    network[p.net].push_back(&p);
  }

  connection.resize(modules_N+1);
  for(int i=0; i < modules_N+1; i++){
    connection[i].resize(modules_N+1);
    fill(connection[i].begin(), connection[i].end(), 0);
  }

  for(int i=0; i < network.size(); i++){
    for(int j=0; j < network[i].size()-1; j++){
      int p= network[i][j]->mod;
      for(int k=j+1 ; k < network[i].size(); k++){
        int q= network[i][k]->mod;
        connection[p][q]++;
        connection[q][p]++;   
      }
    }
  }
}


void FPlan::scaleIOPad(){
  double px = Width/double(root_module.width);
  double py = Height/double(root_module.height);
    
  for(int i=0; i < root_module.pins.size(); i++){
    Pin &p = root_module.pins[i];
    p.ax = int(px * p.x);
    p.ay = int(py * p.y);
    
  }      
}

double FPlan::calIntraDelay()
{
	IntraDelay = 0;
   for(int i=0; i < modules_N; i++)
	   IntraDelay += modules[i].delay;
   return IntraDelay;
}

double FPlan::calcWireLength()
{
//modified by fbmao    
//??need to add Inter delay between soft macro according the Nets??
// net->pin->clb->coordinate->checkLookupTable(delaytable)
	//since the floorplan will update each iteration
//	creatNetworkOfModules();
	WireLength = 0;
    for(int i=0; i < modules_N; i++)
	{ 
		//used for calculate wirelenth bundingbox
		for(int j = 0; j < modules[i].connectModuleIds.size(); j++)
		{
			float weight = 1.0;
			//for congestion.  assign different weights
			//weight = modules[i].connectModuleIds.size() / maxNumTerminal;
		    int max_x= INT_MIN, max_y= INT_MIN;      
            int min_x= INT_MAX, min_y= INT_MAX;  
		    max_x= max(max_x, (int)(modules_info[i].x + modules_info[i].rx) /2.0); max_y= max(max_y, (int)(modules_info[i].y + modules_info[i].ry)/2.0);
            min_x= min(min_x, (int)(modules_info[i].x + modules_info[i].rx) /2.0); min_y= min(min_y, (int)(modules_info[i].y + modules_info[i].ry)/2.0);
			for(int k = 0; k < modules[i].connectModuleIds[j].size(); k++)
			{
				int id = (modules[i].connectModuleIds[j])[k];
				max_x= max(max_x, int(modules_info[id].x + modules_info[id].rx) /2.0), max_y= max(max_y, int(modules_info[id].y + modules_info[id].ry)/2.0);
                min_x= min(min_x, int(modules_info[id].x + modules_info[id].rx) /2.0), min_y= min(min_y, int(modules_info[id].y + modules_info[id].ry)/2.0);
			}
			WireLength += (int)(((max_x-min_x)+(max_y-min_y)) * weight);
		}
	}

	return WireLength;


//////////////////////////////
//  scaleIOPad();
// 
//  WireLength=0;
//  // compute absolute position
//  for(int i=0; i < modules_N; i++){   
//    int mx= modules_info[i].x, my= modules_info[i].y;
//    bool rotate= modules_info[i].rotate;
//    int w= modules[i].width;
//
//    for(int j=0; j < modules[i].pins.size(); j++){
//      Pin &p = modules[i].pins[j];
//
//      if(!rotate){      
//        p.ax= p.x+mx, p.ay= p.y+my;
//      }
//      else{ // Y' = W - X, X' = Y
//	p.ax= p.y+mx, p.ay= (w-p.x)+my;
//      } 
//    }
//  }
//
//  for(int i=0; i < network.size(); i++){     
//    int max_x= INT_MIN, max_y= INT_MIN;      
//    int min_x= INT_MAX, min_y= INT_MAX;      
//
//    assert(network[i].size() > 0);
//    for(int j=0; j < network[i].size(); j++){
//      Pin &p = *network[i][j];
//      max_x= max(max_x, p.ax), max_y= max(max_y, p.ay);
//      min_x= min(min_x, p.ax), min_y= min(min_y, p.ay);
//    }
////    printf("%d %d %d %d\n",max_x,min_x,max_y,min_y);
//    WireLength += (max_x-min_x)+(max_y-min_y);
//  }
//  return WireLength;
}


double FPlan::calcWireLengthConsidermoduleiopos()
{
	//vector<map<string,vector<int> > > portconnectModuleIds; //used for calculate wirelenth bun

	//modified by fbmao    
	//??need to add Inter delay between soft macro according the Nets??
	// net->pin->clb->coordinate->checkLookupTable(delaytable)
	//since the floorplan will update each iteration
	//	creatNetworkOfModules();
	string subplacename, subplacenamea;   
	float ratio;
	WireLength = 0;
	for(int i=0; i < modules_N; i++)
	{ 
		//used for calculate wirelenth bundingbox
		for(int j = 0; j < modules[i].portconnectModuleIds.size(); j++)
		{
			map<string, vector<int> > & tmpportconnectid = modules[i].portconnectModuleIds[j];
			for(map<string, vector<int> >::iterator it = tmpportconnectid.begin();
				it != tmpportconnectid.end();
				it++)
			{		
				int xcoord = -1, ycoord = -1;
				string tmpportname = it->first;
				vector<int> &tmpconnectIds = it->second;  //modules[i].portconnectModuleIds[j
				float weight = 1.0;
				//for congestion.  assign different weights
				//weight = modules[i].connectModuleIds.size() / maxNumTerminal;
				int max_x= INT_MIN, max_y= INT_MIN;      
				int min_x= INT_MAX, min_y= INT_MAX;  

				//for each module, it corresponds to a place file
				string tmpratio, sratio;
				Module &mod = modules[i];
				subplacename = mod.name;
				ratio = mod.ratio;

/*
				stringstream ss;
				ss.str(""); //release temp memory
				ss<<mod.ratio;
				tmpratio = ss.str();
				size_t pos = tmpratio.find(".");
				if(pos != string::npos)
					sratio = tmpratio.substr(0,pos) + tmpratio.substr(pos+1, pos+2);
				else
					sratio = tmpratio + "0";
         
				string submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".v/" + subplacename + ".place";
				fs.open(submoduleplacefile.c_str());
				if(!fs)
				{
					//notice the path
					printf("cannot open the file %s\n", const_cast<char*>(submoduleplacefile.c_str()));
					submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".blif/" + subplacename + ".place";
					fs.close();
					fs.open(submoduleplacefile.c_str());
					if(!fs)
					{
						cout<<"cannot open the file: "<<submoduleplacefile<<endl;
						exit(1);
					}
				}

				fs.close();
*/

				//map<string, map<float, map<string, Finaliopos> > > filenameRatioioBlknamecoord;
				map<string, map<float, map<string, Finaliopos> > >::iterator tmpita = filenameRatioioBlknamecoord.find(subplacename);
				if(tmpita != filenameRatioioBlknamecoord.end())
				{
					map<float, map<string, Finaliopos> > & tmpratioioblknamecoord = tmpita->second;
					map<float, map<string, Finaliopos> >::iterator tmpitb = tmpratioioblknamecoord.find(ratio);
					if(tmpitb != tmpratioioblknamecoord.end())
					{
						map<string, Finaliopos> & tmpioblknamecoord = tmpitb->second;

						//the port name should be in the subfile or else donnot consider the port, it is useless or remove
						//during net combining procedure
						map<string, Finaliopos>::iterator tmpitc = tmpioblknamecoord.find(tmpportname);
						if(tmpitc != tmpioblknamecoord.end())
						{
							Finaliopos& tmpiopos = tmpitc->second;
							xcoord = tmpiopos.x;
							ycoord = tmpiopos.y;
						}
						else
						{
							string prefixtmpportname = "out:" + tmpportname;
							tmpitc = tmpioblknamecoord.find(prefixtmpportname);
							if(tmpitc != tmpioblknamecoord.end())
							{
								Finaliopos& pretmpiopos = tmpitc->second;
								xcoord = pretmpiopos.x;
								ycoord = pretmpiopos.y;
							}
							//else  //comment on 20130909
							//{
							  // cout<<"Warning1: the io block  " << prefixtmpportname <<"don't exist in the place file, pls check"<<endl;
							  //  readplacefileAndReturnXYcoordinates(tmpportname, submoduleplacefile.c_str(), xcoord, ycoord);
							//}
						}
					}
				}
				else
				{
					cout<<"the subplacefile "<<subplacename<<" is lost in the filenameRatioioblknamecoord, pls check"<<endl;
					exit(1);
				}


				// comment on 20130909
				//if(-1  == xcoord || -1 == ycoord)
				//{
				//	string prefixportname = "out:" + tmpportname;
				//	readplacefileAndReturnXYcoordinates(prefixportname, submoduleplacefile.c_str(), xcoord, ycoord);
				//}

			//	assert(xcoord >= 0 && ycoord >= 0);  //the io port should be in the place file


				if(-1 == xcoord || -1 == ycoord)   //if port not in the place file
				{
					max_x = min_x; 
					max_y = min_y;
					continue; //if the source port doesn't exist in the original place file, it donnot need to be considered(pass it)
				   //max_x= max(max_x, (int)(modules_info[i].x + modules[i].width /2.0)); max_y= max(max_y, (int)(modules_info[i].y + modules[i].height/2.0));
       //            min_x= min(min_x, (int)(modules_info[i].x + modules[i].width /2.0)); min_y= min(min_y, (int)(modules_info[i].y + modules[i].height/2.0));
				}
				else 
				{  //exist in the original sub place file
                   assert(xcoord >= 0 && ycoord >= 0);
				   max_x= max(max_x, xcoord + mod.x); max_y= max(max_y, ycoord + mod.y);
				   min_x= min(min_x, xcoord + mod.x); min_y= min(min_y, ycoord + mod.y);
				}

				//some sink ports
				for(int k = 0; k < tmpconnectIds.size(); k++)
				{
					int id = tmpconnectIds[k];
					//for each module, it corresponds to a place file
					string tmpratioa, sratioa;
					Module &moda = modules[id];
					subplacenamea = moda.name;
					ratio = moda.ratio;

/*   //this part can be used for debug (can know while subplace file are currently used)
					stringstream ssa;
                    ssa.str(""); //release temp memory
					ssa<<moda.ratio;
					tmpratioa = ssa.str();
					size_t posa = tmpratioa.find(".");
					if(posa != string::npos)
						sratioa = tmpratioa.substr(0,posa) + tmpratioa.substr(posa+1, posa+2);
					else
						sratioa = tmpratioa + "0";

					string submoduleplacefilea = filename + "/run1/6k6_n8_" + sratioa + ".xml/" + subplacenamea + ".v/" + subplacenamea + ".place";
					fs.open(submoduleplacefilea.c_str());
					if(!fs)
					{
						//notice the path
						printf("cannot open the file %s\n", const_cast<char*>(submoduleplacefilea.c_str()));
						submoduleplacefilea = filename + "/run1/6k6_n8_" + sratioa + ".xml/" + subplacenamea + ".blif/" + subplacenamea + ".place";
						fs.close();
						fs.open(submoduleplacefilea.c_str());
						if(!fs)
						{
							cout<<"cannot open the file: "<<submoduleplacefilea<<endl;
							exit(1);
						}
					}
					fs.close();
*/


				//map<string, map<float, map<string, Finaliopos> > > filenameRatioioBlknamecoord;
				map<string, map<float, map<string, Finaliopos> > >::iterator tmpita = filenameRatioioBlknamecoord.find(subplacenamea);
				if(tmpita != filenameRatioioBlknamecoord.end())
				{
					map<float, map<string, Finaliopos> > & tmpratioioblknamecoord = tmpita->second;
					map<float, map<string, Finaliopos> >::iterator tmpitb = tmpratioioblknamecoord.find(ratio);
					if(tmpitb != tmpratioioblknamecoord.end())
					{
						map<string, Finaliopos> & tmpioblknamecoord = tmpitb->second;
						map<string, Finaliopos>::iterator tmpitc = tmpioblknamecoord.find(tmpportname);
						if(tmpitc != tmpioblknamecoord.end())
						{
							Finaliopos& tmpiopos = tmpitc->second;
							xcoord = tmpiopos.x;
							ycoord = tmpiopos.y;
						}
						else
						{
							string prefixtmpportname = "out:" + tmpportname;
							tmpitc = tmpioblknamecoord.find(prefixtmpportname);
							if(tmpitc != tmpioblknamecoord.end())
							{
								Finaliopos& pretmpiopos = tmpitc->second;
								xcoord = pretmpiopos.x;
								ycoord = pretmpiopos.y;
							}
							//else  //comment 20130909
							//{
							//	cout<<"Warning2: the io block  " << prefixtmpportname <<"don't exist in the place file, pls check"<<endl;
					         //   readplacefileAndReturnXYcoordinates(tmpportname, submoduleplacefilea.c_str(), xcoord, ycoord);
							//}
						}
					}
				}
				else
				{
					cout<<"the subplacefile "<<subplacename<<" is lost in the filenameRatioioblknamecoord, pls check"<<endl;
					exit(1);
				}


				  // if(-1  == xcoord || -1 == ycoord)
				  // {
					 //string prefixportname = "out:" + tmpportname;
					 //readplacefileAndReturnXYcoordinates(prefixportname, submoduleplacefile.c_str(), xcoord, ycoord);
				  // }					

				//	assert(xcoord >= 0 && ycoord >= 0);  //the io port should be in the place file

					if(-1 == xcoord || -1 == ycoord)   //if port not in the place file
					{
						//if the port doesn't exist in the sub place file, it doesn't impact the wirelength
						max_x = min_x;  //let the wirelength be 0
						max_y = min_y;

						continue;
						//max_x= max(max_x, (int)(modules_info[id].x + modules[id].width /2.0)); max_y= max(max_y, (int)(modules_info[id].y + modules[id].height/2.0));
						//min_x= min(min_x, (int)(modules_info[id].x + modules[id].width /2.0)); min_y= min(min_y, (int)(modules_info[id].y + modules[id].height/2.0));
					}
					else
					{
						assert(xcoord >= 0 && ycoord >= 0);  //the io port should be in the place file
						max_x= max(max_x, xcoord + moda.x), max_y= max(max_y, ycoord + moda.y);
						min_x= min(min_x, xcoord + moda.x), min_y= min(min_y, ycoord + moda.y);
					}
				}

				WireLength += (int)(((max_x-min_x)+(max_y-min_y)) * weight);   //one port, one net can get one wirelength
			}
		}
	}

	return WireLength;
}



double FPlan::calcWireLengthConsidermoduleioposAndFinaliopos()
{

	//vector<map<string,vector<int> > > portconnectModuleIds; //used for calculate wirelenth bun

	//modified by fbmao    
	//??need to add Inter delay between soft macro according the Nets??
	// net->pin->clb->coordinate->checkLookupTable(delaytable)
	//since the floorplan will update each iteration
	//	creatNetworkOfModules();
	string subplacename, subplacenamea;   
	float ratio;
	WireLength = 0;
	for(int i = 0; i < modules_N; i++)
	{ 
		//used for calculate wirelenth bundingbox
		for(int j = 0; j < modules[i].portconnectModuleIds.size(); j++)
		{
			map<string, vector<int> > & tmpportconnectid = modules[i].portconnectModuleIds[j];
			for(map<string, vector<int> >::iterator it = tmpportconnectid.begin();
				it != tmpportconnectid.end();
				it++)
			{		
				int xcoord = -1, ycoord = -1;
				string tmpportname = it->first;
				vector<int> &tmpconnectIds = it->second;  //modules[i].portconnectModuleIds[j
				float weight = 1.0;
				//for congestion.  assign different weights
				//weight = modules[i].connectModuleIds.size() / maxNumTerminal;
				int max_x= INT_MIN, max_y= INT_MIN;      
				int min_x= INT_MAX, min_y= INT_MAX;  

				//for each module, it corresponds to a place file
				string tmpratio, sratio;
				Module &mod = modules[i];
				subplacename = mod.name;
				ratio = mod.ratio;
				stringstream ss;
                ss.str(""); //release temp memory
				ss<<mod.ratio;
				tmpratio = ss.str();
				size_t pos = tmpratio.find(".");
				if(pos != string::npos)
					sratio = tmpratio.substr(0,pos) + tmpratio.substr(pos+1, pos+2);
				else
					sratio = tmpratio + "0";
				string submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".v/" + subplacename + ".place";
				fs.open(submoduleplacefile.c_str());
				if(!fs)
				{
					//notice the path
					printf("cannot open the file %s\n", const_cast<char*>(submoduleplacefile.c_str()));
					submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".blif/" + subplacename + ".place";
					fs.close();
					fs.open(submoduleplacefile.c_str());
					if(!fs)
					{
						cout<<"cannot open the file: "<<submoduleplacefile<<endl;
						exit(1);
					}
				}

				fs.close();


				{
				//map<string, map<float, map<string, Finaliopos> > > filenameRatioioBlknamecoord;
				map<string, map<float, map<string, Finaliopos> > >::iterator tmpita = filenameRatioioBlknamecoord.find(subplacename);
				if(tmpita != filenameRatioioBlknamecoord.end())
				{
					map<float, map<string, Finaliopos> > & tmpratioioblknamecoord = tmpita->second;
					map<float, map<string, Finaliopos> >::iterator tmpitb = tmpratioioblknamecoord.find(ratio);
					if(tmpitb != tmpratioioblknamecoord.end())
					{
						map<string, Finaliopos> & tmpioblknamecoord = tmpitb->second;
						map<string, Finaliopos>::iterator tmpitc = tmpioblknamecoord.find(tmpportname);
						if(tmpitc != tmpioblknamecoord.end())
						{
							Finaliopos& tmpiopos = tmpitc->second;
							xcoord = tmpiopos.x;
							ycoord = tmpiopos.y;
						}
						else
						{
							string prefixtmpportname = "out:" + tmpportname;
							tmpitc = tmpioblknamecoord.find(prefixtmpportname);
							if(tmpitc != tmpioblknamecoord.end())
							{
								Finaliopos& pretmpiopos = tmpitc->second;
								xcoord = pretmpiopos.x;
								ycoord = pretmpiopos.y;
							}
							//else    //comment 20130909
							//{
								//cout<<"Warning3: the io block  " << prefixtmpportname <<"don't exist in the place file, pls check"<<endl;
					           // readplacefileAndReturnXYcoordinates(tmpportname, submoduleplacefile.c_str(), xcoord, ycoord);
							//}
						}
					}
				}
				else
				{
					cout<<"the subplacefile "<<subplacename<<" is lost in the filenameRatioioblknamecoord, pls check"<<endl;
					exit(1);
				}

				}

				//if(-1  == xcoord || -1 == ycoord)
				//{
				//	string prefixportname = "out:" + tmpportname;
				//	readplacefileAndReturnXYcoordinates(prefixportname, submoduleplacefile.c_str(), xcoord, ycoord);
				//}

				//assert(xcoord >= 0 && ycoord >= 0);  //the io port should be in the place file
			    if(-1 == xcoord || -1 == ycoord)   //if port not in the place file
				{
				    max_x = min_x; 
					max_y = min_y;
					continue;
					//max_x= max(max_x, (int)(modules_info[i].x + modules[i].width /2.0)); max_y= max(max_y, (int)(modules_info[i].y + modules[i].height/2.0));
					//min_x= min(min_x, (int)(modules_info[i].x + modules[i].width /2.0)); min_y= min(min_y, (int)(modules_info[i].y + modules[i].height/2.0));
				}
				else
				{
                   assert(xcoord >= 0 && ycoord >= 0);  //the io port should be in the place file
				   max_x= max(max_x, xcoord + mod.x); max_y= max(max_y, ycoord + mod.y);
				   min_x= min(min_x, xcoord + mod.x); min_y= min(min_y, ycoord + mod.y);
				}

	            //if moduleio in the finalio,then consider its coordinate during calculating wirelength
				set<string>::iterator ioita = finaliosets.find(tmpportname);
				if(ioita != finaliosets.end())  
				{
					int index =  ionameindex[tmpportname]; 
					int ioposx = finaliocoordinates[index].x;
					int ioposy = finaliocoordinates[index].y;

					max_x= max(max_x, xcoord +  ioposx); max_y= max(max_y, ioposy);
					min_x= min(min_x, xcoord + ioposx); min_y= min(min_y, ioposy);
				}

				for(int k = 0; k < tmpconnectIds.size(); k++)
				{
					int id = tmpconnectIds[k];
					//for each module, it corresponds to a place file
					string tmpratioa, sratioa;
					stringstream ssa;
                    ssa.str(""); //release temp memory
					Module &moda = modules[id];
					subplacenamea = moda.name;
					ratio = moda.ratio;
					ssa<<moda.ratio;
					tmpratioa = ssa.str();
					size_t posa = tmpratioa.find(".");
					if(posa != string::npos)
						sratioa = tmpratioa.substr(0,posa) + tmpratioa.substr(posa+1, posa+2);
					else
						sratioa = tmpratioa + "0";
					string submoduleplacefilea = filename + "/run1/6k6_n8_" + sratioa + ".xml/" + subplacenamea + ".v/" + subplacenamea + ".place";
					fs.open(submoduleplacefilea.c_str());
					if(!fs)
					{
						//notice the path
						printf("cannot open the file %s\n", const_cast<char*>(submoduleplacefilea.c_str()));
						submoduleplacefilea = filename + "/run1/6k6_n8_" + sratioa + ".xml/" + subplacenamea + ".blif/" + subplacenamea + ".place";
						fs.close();
						fs.open(submoduleplacefilea.c_str());
						if(!fs)
						{
							cout<<"cannot open the file: "<<submoduleplacefilea<<endl;
							exit(1);
						}
					}
					fs.close();

              
				{
				//map<string, map<float, map<string, Finaliopos> > > filenameRatioioBlknamecoord;
				map<string, map<float, map<string, Finaliopos> > >::iterator tmpita = filenameRatioioBlknamecoord.find(subplacenamea);
				if(tmpita != filenameRatioioBlknamecoord.end())
				{
					map<float, map<string, Finaliopos> > & tmpratioioblknamecoord = tmpita->second;
					map<float, map<string, Finaliopos> >::iterator tmpitb = tmpratioioblknamecoord.find(ratio);
					if(tmpitb != tmpratioioblknamecoord.end())
					{
						map<string, Finaliopos> & tmpioblknamecoord = tmpitb->second;
						map<string, Finaliopos>::iterator tmpitc = tmpioblknamecoord.find(tmpportname);
						if(tmpitc != tmpioblknamecoord.end())
						{
							Finaliopos& tmpiopos = tmpitc->second;
							xcoord = tmpiopos.x;
							ycoord = tmpiopos.y;
						}
						else
						{
							string prefixtmpportname = "out:" + tmpportname;
							tmpitc = tmpioblknamecoord.find(prefixtmpportname);
							if(tmpitc != tmpioblknamecoord.end())
							{
								Finaliopos& pretmpiopos = tmpitc->second;
								xcoord = pretmpiopos.x;
								ycoord = pretmpiopos.y;
							}
							//else          //comment on 20130909
							//{
							//	cout<<"Warning4: the io block  " << prefixtmpportname <<"don't exist in the place file, pls check"<<endl;
					         //   readplacefileAndReturnXYcoordinates(tmpportname, submoduleplacefilea.c_str(), xcoord, ycoord);
							//}
						}
					}
				}
				else
				{
					cout<<"the subplacefile "<<subplacename<<" is lost in the filenameRatioioblknamecoord, pls check"<<endl;
					exit(1);
				}

				}

				
				    //if(-1  == xcoord || -1 == ycoord)
				    //{
					   // string prefixportname = "out:" + tmpportname;
					   // readplacefileAndReturnXYcoordinates(prefixportname, submoduleplacefile.c_str(), xcoord, ycoord);
				    //}

					//assert(xcoord >= 0 && ycoord >= 0);  //the io port should be in the place file


			        if(-1 == xcoord || -1 == ycoord)   //if port not in the place file
				    {
						max_x = min_x; 
						max_y = min_y;
						continue;
					   //max_x= max(max_x, (int)(modules_info[id].x + modules[id].width /2.0)); max_y= max(max_y, (int)(modules_info[id].y + modules[id].height/2.0));
					   //min_x= min(min_x, (int)(modules_info[id].x + modules[id].width /2.0)); min_y= min(min_y, (int)(modules_info[id].y + modules[id].height/2.0));
				    }
					else
					{
                       assert(xcoord >= 0 && ycoord >= 0);  //the io port should be in the place file
					   max_x= max(max_x, xcoord + moda.x), max_y= max(max_y, ycoord + moda.y);
					   min_x= min(min_x, xcoord + moda.x), min_y= min(min_y, ycoord + moda.y);
					}
				}

				WireLength += (int)(((max_x-min_x)+(max_y-min_y)) * weight);
			}
		}
	}

	return WireLength;
}



//---------------------------------------------------------------------------
//   Modules Information
//---------------------------------------------------------------------------

string query_map(map<string,int> M,int value){
  for(map<string,int>::iterator p=M.begin(); p != M.end(); p++){
    if(p->second == value)
      return p->first;
  }
  return "";
}

void FPlan::show_modules()
{
  for(int i=0; i < modules.size();i++){
    cout << "Module: " << modules[i].name << endl;
    cout << "  Width = " << modules[i].width;
    cout << "  Height= " << modules[i].height << endl;
    cout << "  Area  = " << modules[i].area << endl;
//    cout << modules[i].pins.size() << " Pins:\n";
//    for(int j=0; j < modules[i].pins.size(); j++){
//      cout << query_map(net_table,modules[i].pins[j].net) << " ";
//      cout << modules[i].pins[j].x << " " << modules[i].pins[j].y << endl;
//    }
  }
}

void FPlan::outDraw( const char* outfilename )
{
//	string tempfile = filename + "/" + outfilename;
//	FILE *fs = fopen( tempfile.c_str(), "w" ); 
    //fbmao modified
//	FILE *fs = fopen(outfilename, "a+" );
	FILE *fs = fopen(outfilename, "w" );
	for( int i=0; i<modules_N; i++ )
	{
		fprintf( fs,"RECT %s %d %d %d %d 0\n",
			modules[i].name,
			modules_info[i].x, 
			modules_info[i].y,
			modules_info[i].rx - modules_info[i].x,
			modules_info[i].ry - modules_info[i].y );
	}
	fprintf( fs,"---------------\n");
    fclose(fs);
}


void FPlan::noWidthUpdateOutDraw( const char* outfilename )
{
//	string tempfile = filename + "/" + outfilename;
//	FILE *fs = fopen( tempfile.c_str(), "w" ); 
    //fbmao modified
//	FILE *fs = fopen(outfilename, "a+" );
	FILE *fs = fopen(outfilename, "w" );
	for( int i=0; i<modules_N; i++ )
	{
		fprintf( fs,"RECT %s %d %d %d %d 0\n",
			modules[i].name,
			modules_info[i].x, 
			modules_info[i].y,
			modules[i].width,   //the only different thing for supporting heterogeneous arch
			modules_info[i].ry - modules_info[i].y);
	}
	fprintf( fs,"---------------\n");
    fclose(fs);
}


void FPlan::gnuplotDraw( const char* filename )
{

  //  FILE *fs = fopen( filename, "w" );

	//fbmao modified
//	FILE *fs = fopen( filename, "a+" );
	FILE *fs = fopen( filename, "w" );
    fprintf(fs,"set nokey\nset size ratio -1\nset title ' Area= %.0f HPWL= %.0f Deadspace= %.4f%% width= %.0f height= %.0f\nplot '-' w l\n",
		double(getArea()), double(getWireLength()), double(getDeadSpace()), getWidth(), getHeight());
	for( int i=0; i<modules_N; i++ )
	{   
		fprintf( fs,"%d %d\n%d %d\n%d %d\n%d %d\n%d %d\n",
		modules_info[i].x, // 模块x坐标
		modules_info[i].y, // 模块y坐标

        modules_info[i].rx,
        modules_info[i].y,


        modules_info[i].rx,
        modules_info[i].ry,

        modules_info[i].x,
        modules_info[i].ry,

        modules_info[i].x,    // 模块x坐标
		modules_info[i].y ); // 模块y坐标
	
		fprintf(fs,"\n");
	}
	fprintf(fs,"EOF\npause -1 'Press any key' ");
    fclose(fs);
}



void FPlan::Output_positions(char * positionFile) {

    char outfile[80];

    strcpy(outfile, positionFile);
    strcat(outfile, ".fixed");

//    cout << " Write the positions of blocks to " << outfile << endl;

    ofstream fout(outfile);
    fout<<modules_N<<endl;
    for (int i=0; i<modules_N; ++i ){
        fout<<modules[i].name<<" ";
        
        fout<<(double) modules_info[i].x<<" ";
        fout<<(double) modules_info[i].y<<" ";
        fout<<(double) modules_info[i].rx<<" ";
        //out<<(double) modules_info[i].ry<<endl;
        fout<<(double) modules_info[i].ry<<" ";
        // print out the candidate ID
//       fout<< modules_info[i].implementation_ID << endl;
		fout<<endl;
    }
    fout<<endl<<endl;
    fout.close();
   
}
void FPlan::list_information(){

  string info = filename + "/" + filename+ ".info";   
  ofstream of(info.c_str());
  
  of << modules_N << " " << Width << " " << Height << endl;
  for(int i=0; i < modules_N; i++){
    of << modules_info[i].x  << " " << modules_info[i].rx  << " ";
    of << modules_info[i].y << " " << modules_info[i].ry << endl;
  }
  of << endl;

  calcWireLength(); 
  calIntraDelay();

  #if 0
	FILE *fa=fopen("CongMatrix.txt","a");
int s=0;
for(int i=0;i<grids.size();i++)
	 {   
	     fprintf( fa,"%.3f     ",grids[i].value );
		     s++;
		 if(s%col_number==0)
		   fprintf(fa,"\n");
	 }
	fclose(fa);	 
#endif


  //fbmao comment 2012/12/14
  //int x,y,rx,ry;
  //for(int i=0; i < network.size(); i++){
  //  assert(network[i].size()>0);
  //  x = network[i][0]->ax;
  //  y = network[i][0]->ay;
  //  
  //  for(int j=1; j < network[i].size(); j++){
  //    rx = network[i][j]->ax;
  //    ry = network[i][j]->ay;
  //    of << x << " " << y << " " << rx << " " << ry << endl;
  //    x = rx, y = ry;
  //  }
  //}

  cout << "Num of Module  = " << modules_N << endl;
  cout << "Total Area     = " << TotalArea*1e-6 << endl;
  cout << "Height         = " << Height*1e-3 << endl;
  cout << "Width          = " << Width*1e-3 << endl;
  cout << "Aspect Ratio   = " << getAR() << endl; 

  if( outline_ratio > 0 )
  {
      if( Height <= outline_height && Width <= outline_width )
      {
          printf( "Fit into the outline.\n" );
      }
  }

  cout << "\nArea           = " << Area*1e-6 << endl;
  cout << "Wire Length    = " << calcWireLength()*1e-3 << endl;
  cout<<"intradelay   = "<< calIntraDelay() *1e-6 <<endl;
  printf( "Dead Space     = %.2f\n\n", getDeadSpace());

}

void FPlan::storeClbAndHeteroBlocksFromvprFiles()
{
	//used for move the (soft module) clbs.   IO donnot need to change coordinates

	//string filename, float ratio, string blockname, string blocktype
	//need to specify a file to read the block information
	string clbfile = filename +"/"+ filename + "clb.txt";

	//#### Comments
	//filename, ratio, blkname
	//map<string, map<float, set<string> > > filenameRatioBlkname;
	//filename, blktype, blkname
	//map<string, map<string, set<string> > > filenameblktypeblkname;
	//filename, ramdspblknames
	//map<string, set<string> > filenameRamDspBlks;

	//the file only stores the clb' blkname 
	filenameRatioBlkname.clear();
	filenameblktypeblkname.clear();
	filenameRamDspBlks.clear();

	map<string, map<float, set<string> > >::iterator ita;
	map<float, set<string> >::iterator itb;
	set<string>::iterator itc;
	string subfilename;
	float ratio;
	string tmpblkname, blkname, blktype;

	//for support heterogeneous architecture
	//filename, blktype (memory, multiply), blknameSet
	map<string, map<string, set<string> > > ::iterator itd;
	map<string, set<string> >::iterator ite; 

	fs.open(clbfile.c_str());
	if(!fs)
	{
		error("unable to open the file %s", const_cast<char*>(clbfile.c_str()));
		exit(1);
	}
	fs >> t1 >> t2 >> t3 >> t4;
	for(int i = 0; !fs.eof(); i++)
	{
		fs >> subfilename >> ratio >> tmpblkname >> blktype;
		size_t pos = tmpblkname.find(".");
		if(pos != string::npos)  
		{
			blkname = tmpblkname.substr(pos+1);
		}
		else
		{
			error("the block name %s is not right\n");
		}

		if(0 != strcmp(blktype.c_str(), "clb"))
		{  //do not consider heterogeneous architecture and modules with hetero blocks
			if(!isheteroarch)  
				continue;

			//only store memory, multiply block
			itd = filenameblktypeblkname.find(subfilename);
			if(itd != filenameblktypeblkname.end())
			{
				map<string, set<string> > & typeBlknames = itd->second;
				ite = typeBlknames.find(blktype);  //check blktype
				if(ite != typeBlknames.end())
				{
					set<string> &blknames = ite->second;
					blknames.insert(blkname);
				}
				else
				{
					set<string> tempblknames;
					tempblknames.insert(blkname);
					typeBlknames.insert(make_pair(blktype, tempblknames));
				}
			}
			else
			{
				set<string> blocknames;
				map<string, set<string> > temptypeBlknames;
				blocknames.insert(blkname);
				temptypeBlknames.insert(make_pair(blktype, blocknames));
				filenameblktypeblkname.insert(make_pair(subfilename, temptypeBlknames));
			}


			map<string, set<string> >::iterator itf = filenameRamDspBlks.find(subfilename);
			if(itf != filenameRamDspBlks.end())
			{
				itf->second.insert(blkname);
			}
			else
			{
				set<string> tmpramdspblkset;
				tmpramdspblkset.insert(blkname);
				filenameRamDspBlks.insert(make_pair(subfilename, tmpramdspblkset));
			}


			continue;
		}


		////test bug fbmao
		//if(0 == strcmp("load_data_translator", subfilename.c_str())  || 0 == strcmp("lo_reg", subfilename.c_str()) )
		//	cout<<"bug about filename lost comes out"<<endl;

		ita = filenameRatioBlkname.find(subfilename);
		if(ita != filenameRatioBlkname.end())
		{
			map<float, set<string> > &ratioBlknames = ita->second;
			itb = ratioBlknames.find(ratio);
			if(itb != ratioBlknames.end())
			{
				set<string> &blknames = itb->second;
				blknames.insert(blkname);
			}
			else
			{
				set<string> tempblknames;
				tempblknames.insert(blkname);
				ratioBlknames.insert(make_pair(ratio, tempblknames));
			}
		}
		else
		{
			set<string> blocknames;
			map<float, set<string> > tempRatioBlknames;
			blocknames.insert(blkname);
			tempRatioBlknames.insert(make_pair(ratio, blocknames));
			filenameRatioBlkname.insert(make_pair(subfilename, tempRatioBlknames));
		}
	}
	fs.close();
}




void FPlan::storeClbBlocksFromLoadableRegionFiles(string regionname)
{  //now only support one reconfigurable region be replaced
	string clbfile = filename + "/loadable/" + regionname + ".v/" + regionname + "clb.txt";
	set<string>::iterator ita;
    string subfilename;
	float ratio;
	string tmpblkname, blkname, blktype;
	fs.open(clbfile.c_str());
	if(!fs)
	{
		error("unable to open the file %s", const_cast<char*>(clbfile.c_str()));
		exit(1);
	}
	fs >> t1 >> t2 >> t3 >> t4;
	for(int i = 0; !fs.eof(); i++)
	{
		fs >> subfilename >> ratio >> tmpblkname >> blktype;
		if(0 != strcmp(blktype.c_str(), "clb"))
		{  //only store the clb block. does not contain memory
			continue;
		}

		size_t pos = tmpblkname.find(".");
		if(pos != string::npos)  
		{
			blkname = tmpblkname.substr(pos+1);
		}
		else
		{
			error("the block name %s is not right\n");
			exit(1);
		}

		ita = loadableregionblknames.find(blkname);
		if(ita == loadableregionblknames.end())
		{
			loadableregionblknames.insert(blkname);
		}
	}
	fs.close();
}



//after lodable flow, combine lodable place and orginal subplace file into whole place file
void FPlan::combineplacefileAfterloadable()
{
	//1. get the loadable region
	FILE *fpconfig;
	char **coordtokens;
	int rline;
	string loadableregionname;
	vector<string> loadregionames;
	loadregionames.clear();
	string reconfigregionfile = filename + "/" + filename + ".configregion";
	fpconfig = fopen(reconfigregionfile.c_str(), "r");
	if(!fpconfig)
	{
		printf("cannot open file: %s\n", reconfigregionfile.c_str());
		exit(1);
	}

	coordtokens = ReadLineTokens(fpconfig, &rline);
	if(NULL == coordtokens)
	{
		printf("bad line specified in the reconfigregionfile  %s\n", reconfigregionfile.c_str());
	}

	//reorder the net Id
	while(coordtokens)
	{
		int isloadable = atoi(coordtokens[7]);
		if(isloadable) //find the loadable region, and then store its name
		{
			loadableregionname = coordtokens[5];
			loadregionames.push_back(loadableregionname);
		}

		free(*coordtokens);
		free(coordtokens);
		coordtokens = ReadLineTokens(fpconfig, &rline);
	}
	fclose(fpconfig);



	//2. first read the Io blocks and 3.read the clb blocks, especially deal with the loadable region
	//add for loadable flow, for getting the whole place file
	char **tokens;
	int line;
	int blkcount = 0; //mark blk number
	//final place file
	string finalplacefile = filename + "/loadable/" + loadableregionname + ".v/" + loadableregionname + ".ld.place";
	FILE * ffinalplace = fopen(finalplacefile.c_str(), "w+");

	//read ioplacefile
	string ioforfinalplacefile = filename + "/placefiles/" + filename + ".ioplace";
	FILE *infile = fopen(ioforfinalplacefile.c_str(), "r");
	if(!infile || !ffinalplace)
	{
		cout<<"cannot open the file "<<ioforfinalplacefile.c_str()<<"  "<<finalplacefile.c_str()<<endl;
		exit(1);
	}

	//deal with the first five lines
	int linenum = 0;
	string tmpa, tmpb, tmpc, tmpd, tmpe, tmpf, tmpg;
	fs.open(ioforfinalplacefile.c_str());
	if(!fs)
	{
		error("unable to open the file %s",
			const_cast<char*>(ioforfinalplacefile.c_str()));
		exit(1);
	}
	for(int i = 0; !fs.eof(); i++)
	{
		if(0 == linenum)
		{
			fs>>tmpa >>tmpb>>tmpc>>tmpd>>tmpe>>tmpf;
			fprintf(ffinalplace, "Netlist file: %s   Architecture file: %s\n",
				tmpc.c_str(), tmpf.c_str());
			linenum++;
		}

		if(1 == linenum)
		{
			fs>>tmpa>>tmpb>>tmpc>>tmpd>>tmpe>>tmpf>>tmpg;
			fprintf(ffinalplace, "Array size: %d x %d logic blocks\n\n", atoi(tmpc.c_str()), atoi(tmpe.c_str()));
			fprintf(ffinalplace, "#block name\tx\ty\tsubblk\tblock number\n");
			fprintf(ffinalplace, "#----------\t--\t--\t------\t------------\n");
		} 
		fs.close();
		break;
	}

	//read other lines in the .ioplace file
	tokens = ReadLineTokens(infile, &line);
	tokens = ReadLineTokens(infile, &line);
	tokens = ReadLineTokens(infile, &line);
	while(tokens)
	{
		tmpa = tokens[0];
		tmpb = tokens[1];
		tmpc = tokens[2];
		tmpd = tokens[3];

		fprintf(ffinalplace, "%s\t", tmpa.c_str());
		if(strlen(tmpa.c_str()) < 8)
			fprintf(ffinalplace, "\t");
		//move clb block's coordinate //subblk donnot need to move, for clb's subblk is (0,0)
		//here doesn't consider the memory coordinate
		fprintf(ffinalplace, "%s\t%s\t%s", tmpb.c_str(), tmpc.c_str(), tmpd.c_str()); 
		fprintf(ffinalplace, "\t#%d\n", blkcount);
		blkcount++;
		free(*tokens);
		free(tokens);
		tokens = ReadLineTokens(infile, &line);
	}
	fclose(infile);


	//then read subregion place file, deal with loadable region especially
	//each module corresponding to a place file
	for(int i = 0; i < modules_N; i++)
	{
		Module &mod = modules[i];
		//need to check whether module's id is right
		assert(mod.id == i);
		Module_Info &modInfo = modules_info[mod.id];
		const char* subplacename = mod.name;
		string fforfinalplacefile;
		FILE *tmpsubplacefile;
		if(0 != strcmp(subplacename, loadableregionname.c_str()))
		{  //the module is not the loable region
		   //Notice the clbplace file format, it needs to read directly, not the same as original place format
			fforfinalplacefile = filename + "/" + subplacename + ".clbplace";
			tmpsubplacefile = fopen(fforfinalplacefile.c_str(), "r");
			if(!tmpsubplacefile)
			{
				cout<<"cannot open the file "<<fforfinalplacefile.c_str()<<endl;
				exit(1);
			}

			tokens = ReadLineTokens(tmpsubplacefile, &line);
			while(tokens)
			{
				tmpa = tokens[0];
				tmpb = tokens[1];
				tmpc = tokens[2];
				tmpd = tokens[3];

				fprintf(ffinalplace, "%s\t", tmpa.c_str());
				if(strlen(tmpa.c_str()) < 8)
					fprintf(ffinalplace, "\t");
				//move clb block's coordinate //subblk donnot need to move, for clb's subblk is (0,0)
				//here doesn't consider the memory coordinate
				fprintf(ffinalplace, "%s\t%s\t%s", tmpb.c_str(), tmpc.c_str(), tmpd.c_str()); 
				fprintf(ffinalplace, "\t#%d\n", blkcount);
				blkcount++;
				free(*tokens);
				free(tokens);
				tokens = ReadLineTokens(tmpsubplacefile, &line);
			}
			fclose(tmpsubplacefile);
		}
		else
		{
			//need to change the clb blk name in the replace file
			fforfinalplacefile = filename + "/loadable/" + loadableregionname +".v/" + loadableregionname + ".replace";
			tmpsubplacefile = fopen(fforfinalplacefile.c_str(), "r");
			if(!tmpsubplacefile)
			{
				cout<<"cannot open the file "<<fforfinalplacefile.c_str()<<endl;
				exit(1);
			}

			map<string, string>::iterator itmapc;
			string tmpnewa;
			tokens = ReadLineTokens(tmpsubplacefile, &line);
			tokens = ReadLineTokens(tmpsubplacefile, &line);
			tokens = ReadLineTokens(tmpsubplacefile, &line);
			while(tokens)
			{
				tmpa = tokens[0];
				tmpb = tokens[1];
				tmpc = tokens[2];
				tmpd = tokens[3];

				//Notice: change name
				string ttname = tmpa + "*" + loadableregionname;
				itmapc = blksnamemap.find(ttname); //blksnamempa should contain this clb blk
				assert(itmapc != blksnamemap.end());
				if(itmapc != blksnamemap.end())
				{   //use a new name subsitude the original blk name
					tmpnewa = itmapc->second;
				}

				fprintf(ffinalplace, "%s\t", tmpnewa.c_str());
				if(strlen(tmpnewa.c_str()) < 8)
					fprintf(ffinalplace, "\t");
				//move clb block's coordinate //subblk donnot need to move, for clb's subblk is (0,0)
				//here doesn't consider the memory coordinate
				fprintf(ffinalplace, "%s\t%s\t%s", tmpb.c_str(), tmpc.c_str(), tmpd.c_str()); 
				fprintf(ffinalplace, "\t#%d\n", blkcount);
				blkcount++;
				free(*tokens);
				free(tokens);
				tokens = ReadLineTokens(tmpsubplacefile, &line);
			}

		}
		fclose(tmpsubplacefile);
	}
	fclose(ffinalplace);
}



//fbmao
void FPlan::getloadableRegionAndReplace(int archwidth, int archheight)
{  //notice should release, the replace file will need to do route in subregion, 
	//so donnot change name (use namemap)
	FILE *fpconfig;
	char **coordtokens;
	int rline;
	vector<string> loadregionames;
	loadregionames.clear();
	string reconfigregionfile = filename + "/" + filename + ".configregion";
	fpconfig = fopen(reconfigregionfile.c_str(), "r");
	if(!fpconfig)
	{
		printf("cannot open file: %s\n", reconfigregionfile.c_str());
		exit(1);
	}

	coordtokens = ReadLineTokens(fpconfig, &rline);
	if(NULL == coordtokens)
	{
		printf("bad line specified in the reconfigregionfile  %s\n", reconfigregionfile.c_str());
	}

    loadableregionblknames.clear();

	//reorder the net Id
	while(coordtokens)
	{
		int isloadable = atoi(coordtokens[7]);
		if(isloadable) //find the loadable region, and then store its name
		{
			 string regionname = coordtokens[5];
			 loadregionames.push_back(regionname);
		     int coordx = atoi(coordtokens[0]);
		     int coordrx = atoi(coordtokens[1]);
		     int coordy = atoi(coordtokens[2]);
		     int coordry = atoi(coordtokens[3]);
			 string subplacefile = filename + "/loadable/" + regionname + ".v/" + regionname + ".place";
			 string newplacefile = filename + "/loadable/" + regionname +".v/" + regionname + ".replace";
			 storeClbBlocksFromLoadableRegionFiles(regionname);
			 readAndRewriteplace(coordx, coordy, subplacefile, newplacefile, regionname, archwidth, archheight);//default 1 1		 
	 	}

		free(*coordtokens);
	    free(coordtokens);
	    coordtokens = ReadLineTokens(fpconfig, &rline);
	}

	fclose(fpconfig);
 
	return;
}



void  FPlan::readAndRewriteplace(int coordx, int coordy, string subplacefile, 
	                  string newplacefile, string regionname, int archwidth, int archheight)
{
    //replace the clbs of the reconfig region, based on orginal place result
	//here coordx, coordy is the bottom-left corner coordinate of this module in the floorplan result
	FILE *infile;
	char **tokens;
	int line;
	string archfile = "6k6_n8.xml";
	string newnetfilename = regionname + ".net";
	//first clean the file
	FILE *ff = fopen(newplacefile.c_str(), "w");
	fclose(ff);
	ff = fopen(newplacefile.c_str(), "a+");
	if(!ff)
	{
		cout<<"cannot open the file "<<newplacefile.c_str()<<endl;
		exit(1);
	}
	
	int blkcount = 0;
	if(0 == blkcount)
	{    //filename like bgm
		fprintf(ff, "Netlist file: %s   Architecture file: %s\n", newnetfilename.c_str(),
			archname.c_str());

		 //width and height will use the arc size
		fprintf(ff, "Array size: %d x %d logic blocks\n\n", archwidth, archheight); 
		fprintf(ff, "#block name\tx\ty\tsubblk\tblock number\n");
		fprintf(ff, "#----------\t--\t--\t------\t------------\n");
	}

	infile = fopen(subplacefile.c_str(), "r");
	if(!infile)
	{
		cout<<"it cannot open the file "<<subplacefile.c_str()<<endl;
		exit(1);
	}
	// Check filenames in first line match 
	tokens = ReadLineTokens(infile, &line);
	int error = 0;
	if(NULL == tokens)
	{
		error = 1;
	}
	for(int i = 0; i < 6; ++i)
	{
		if(!error)
		{
			if(NULL == tokens[i])
			{
				error = 1;
			}
		}
	}
	if(!error)
	{
		if((0 != strcmp(tokens[0], "Netlist")) ||
			(0 != strcmp(tokens[1], "file:")) ||
			(0 != strcmp(tokens[3], "Architecture")) ||
			(0 != strcmp(tokens[4], "file:")))
		{
			error = 1;
		};
	}
	if(error)
	{
		printf("error:\t"
			"'%s' - Bad filename specification line in placement file\n",
			subplacefile.c_str());
		exit(1);
	}


	// Check array size in second line matches 
	tokens = ReadLineTokens(infile, &line);
	error = 0;
	if(NULL == tokens)
	{
		error = 1;
	}
	for(int i = 0; i < 7; ++i)
	{
		if(!error)
		{
			if(NULL == tokens[i])
			{
				error = 1;
			}
		}
	}
	if(!error)
	{
		if((0 != strcmp(tokens[0], "Array")) ||
			(0 != strcmp(tokens[1], "size:")) ||
			(0 != strcmp(tokens[3], "x")) ||
			(0 != strcmp(tokens[5], "logic")) ||
			(0 != strcmp(tokens[6], "blocks")))
		{
			error = 1;
		};
	}
	if(error)
	{
		printf("error1 :\t"
			"'%s' - Bad fpga size specification line in placement file\n",
			subplacefile.c_str());
		exit(1);
	}


	tokens = ReadLineTokens(infile, &line);
	while(tokens)
	{
		string tempblkname = tokens[0];
		int x = atoi(tokens[1]);
		int y = atoi(tokens[2]);
		int z = atoi(tokens[3]);

		set<string>::iterator itc = loadableregionblknames.find(tempblkname);
		if(itc != loadableregionblknames.end()) //the block  is a clb block
		{   //need to modified the blk name and coordinate, based on coordinat(detaX, detaY)
			//tempblkname = tempa + "*" + subpostfix;		
			fprintf(ff, "%s\t", tempblkname.c_str());
			if(strlen(tempblkname.c_str()) < 8)
				fprintf(ff, "\t");
			//move clb block's coordinate //subblk donnot need to move, for clb's subblk is (0,0)
			fprintf(ff, "%d\t%d\t%d", x+coordx, y+coordy, z);  //here doesn't consider the memory coordinate
			fprintf(ff, "\t#%d\n", blkcount);
			blkcount++;
		}   

		free(*tokens);
		free(tokens);
		tokens = ReadLineTokens(infile, &line);
	}

	fclose(infile);
	fclose(ff);
}


//void FPlan::storeIoBlocksFromplaceFiles()
//{
//  //first store the io block and then use the io blk's coordinate(in original place file)
//  //to calculate wirelength
//	
//	//map<string, map<float, set<string> > > filenameRatioBlkname;
//	
//	//store filename
//	vector<string> filenamearray;
//	//store ratio
//	vector<float> ratioarray;
//	filenamearray.clear();
//	ratioarray.clear();
//
//	for(map<string, map<float, set<string> > >::iterator it = filenameRatioBlkname.begin();
//		 it != filenameRatioBlkname.end();
//		 it++)
//	{
//		string tmpfilename = it->first;
//		filenamearray.push_back(tmpfilename);
//		map<float, set<string> > & tmpratioblkname = it->second;
//		for(map<float, set<string> >::iterator ita = tmpratioblkname.begin();
//			 ita != tmpratioblkname.end();
//			 ita++)
//		{
//			ratioarray.push_back(ita->first);
//		}
//	}
//
//	filenameRatioioBlknamecoord.clear();
//	// map<string, map<float, map<string, Finaliopos> > > filenameRatioioBlknamecoord;
//
//	for(int i = 0; i < (int)filenamearray.size(); i++)
//	{
//		string subplacename = filenamearray[i];
//		for(int j = 0; j < (int)ratioarray.size(); j++)
//		{
//			//for each module, it corresponds to a place file
//			string tmpratio, sratio;
//			float ratio = ratioarray[j];
//			stringstream ss;
//            ss.str(""); //release temp memory
//			ss<<ratio;
//			tmpratio = ss.str();
//			size_t pos = tmpratio.find(".");
//			if(pos != string::npos)
//				sratio = tmpratio.substr(0,pos) + tmpratio.substr(pos+1, pos+2);
//			else
//				sratio = tmpratio + "0";
//			string submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".v/" + subplacename + ".place";
//			fs.open(submoduleplacefile.c_str());
//			if(!fs)
//			{
//				//notice the path
//				printf("cannot open the file %s\n", const_cast<char*>(submoduleplacefile.c_str()));
//				submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".blif/" + subplacename + ".place";
//				fs.close();
//				fs.open(submoduleplacefile.c_str());
//				if(!fs)
//				{
//					cout<<"cannot open the file: "<<submoduleplacefile<<endl;
//					exit(1);
//				}
//			}
//
//			fs.close();
//
//			//get the blk coord, then store it into filenameRatioioBlknamecoord
//           // map<string, map<float, map<string, Finaliopos> > > filenameRatioioBlknamecoord;
//           //read place file and store the coordinate
//			//if a file only have io, it may have problem, now assume each module has at least one clb
//			//if(0 == strcmp(submoduleplacefile.c_str(),"branchresolve"))
//			//	cout<<"check the bug "<<endl; 
//
//			readplacefileAndstoreIOcoordinates(submoduleplacefile, subplacename, ratio);
//		}
//	}
//}


//record the total number of RAM, DSP
void FPlan::recordMemDspnumforEachModule()
{   
	fheteromodidset.clear();
	fmemorynum = 0;
	fmultiplynum = 0;

	map<string, int> nameId;
	nameId.clear();
	for(int i = 0; i < modules_N; i++)
	{
		nameId.insert(make_pair(modules[i].name, modules[i].id));
	}

	for(map<string, map<string, set<string> > >::iterator ita = filenameblktypeblkname.begin();
		ita != filenameblktypeblkname.end();
		ita++)
	{
		int tmpramnum = 0; 
		int tmpdspnum = 0;
		string submodulename = ita->first;
		//map<string, set<string> >& tmpblktypeblknameset = ita->second;
		//map<string, set<string> >::iterator itb = tmpblktypeblknameset.find("memory");
		//map<string, set<string> >::iterator itc = tmpblktypeblknameset.find("multiply");	
		//if(itb != tmpblktypeblknameset.end())	
		//{
		//	set<string> &memblkset = itb->second;
		//	tmpramnum = memblkset.size();
		//}

		//if(itc != tmpblktypeblknameset.end())	
		//{
		//	set<string> &multiplyblkset = itc->second;
		//	tmpdspnum = multiplyblkset.size();
		//}

		int id  = nameId[submodulename];
		Module &tempmod = modules[id];
		//	tempmod.numram = tmpramnum;
		//	tempmod.numdsp = tmpdspnum;
		if(tempmod.numram > 0 || tempmod.numdsp > 0)
		{
			fheteromodidset.insert(id);  //store module id or mod's name. (check later)
		}

		fmemorynum += tempmod.numram;
		fmultiplynum += tempmod.numdsp;
	}
}


void FPlan::recordDataofArchitecture(string archname)
{
	if(archname == "k6_N10_memDepth16384_memData64_40nm_timing.xml")
	{	//memory
		fmemstart = 2;
		fmemrepeat = 8;
		fmemoryheight = 6;

		//multiply     //mult_36
		fmultistart = 4;
		fmultirepeat = 8;
		fmultiheight = 4;
	}
	else if(archname == "6k6_n8_10.xml")  //k6_N10_memDepth16384_memData64_40nm_timing.xml
	{	
		//1. vpr arch
		//memory
		fmemstart = 2;
		fmemrepeat = 8;
		fmemoryheight = 6;

		//multiply
		fmultistart = 4;
		fmultirepeat = 8;
		fmultiheight = 4;

/*
	    //2. own arch
		//memory
		fmemstart = 3;
		fmemrepeat = 5;
		fmemoryheight = 2;

		//multiply
		fmultistart = 5;
		fmultirepeat = 5;
		fmultiheight = 4;
*/

	}
	else if(archname == "hard_fpu_arch_timing.xml") //check later??? cannot use this arch. donot have regular memory and multiply
	{	//memory
		fmemstart = 7;
		fmemrepeat = 10;
		fmemoryheight = 2;

		//multiply
		fmultistart = 6;
		fmultirepeat = 8;
		fmultiheight = 4;
	}
	else if(archname == "soft_fpu_arch_timing.xml")//cannot use this arch. donot have regular memory and multiply
	{	//memory
		fmemstart = 7;
		fmemrepeat = 10;
		fmemoryheight = 2;

		//multiply
		fmultistart = 6;
		fmultirepeat = 8;
		fmultiheight = 4;
	}	
	else
	{
		cout<<"Error: Cannot find the specific Architecture, need to set the arch parameter "<<archname.c_str()<<endl;
		exit(0);
	}
}


void *FPlan::my_malloc(size_t size)
{
    void *ret;
	if(size == 0) {
		return NULL;
	}

    if((ret = malloc(size)) == NULL)
	{
	    fprintf(stderr, "Error:  Unable to malloc memory.  Aborting.\n");
	    abort();
	    exit(1);
	}
    return (ret);
}

/* Reads in a single line from file, splits into tokens and allocates
 * a list of tokens. Returns the an array of character arrays with the
 * final item being marked by an empty string.
 * Returns NULL on EOF
 * NB: Token list is does as two allocations, one for pointer list
 * and one for character array. Free what pointer points to and then
 * free the pointer itself */
char **
FPlan::ReadLineTokens(FILE * InFile,
	       int *LineNum)
{

    enum
    { BUFFSIZE = 65536 };	/* This is much more than enough */
    char Buffer[BUFFSIZE];	/* Must match BUFFSIZE */
    char *Res;
    char *Last;
    char *Cur;
    char *Dst;
    char **Tokens;
    int TokenCount;
    int Len;
    int CurToken;
    bool InToken;

    do
	{
	    /* Read the string */
	    Res = fgets(Buffer, BUFFSIZE, InFile);
	    if(NULL == Res)
		{
		    if(feof(InFile))
			{
			    return NULL;	/* Return NULL on EOF */
			}
		    else
			{
			    printf("ERROR:\t" "Unexpected error reading file\n");
			    exit(1);
			}
		}
	    ++(*LineNum);

	    /* Strip newline if any */
	    Last = Buffer + strlen(Buffer);
	    if((Last > Buffer) && ('\n' == Last[-1]))
		{
		    --Last;
		}
	    if((Last > Buffer) && ('\r' == Last[-1]))
		{
		    --Last;
		}

	    /* Handle continued lines */
	    while((Last > Buffer) && ('\\' == Last[-1]))
		{
		    /* Strip off the backslash */
		    --Last;

		    /* Read next line by giving pointer to null-char as start for next */
		    Res = fgets(Last, (BUFFSIZE - (Last - Buffer)), InFile);
		    if(NULL == Res)
			{
			    if(feof(InFile))
				{
				    return NULL;	/* Return NULL on EOF */
				}
			    else
				{
				    printf("ERROR:\t"
					   "Unexpected error reading file\n");
				    exit(1);
				}
			}
		    ++(*LineNum);

		    /* Strip newline */
		    Last = Buffer + strlen(Buffer);
		    if((Last > Buffer) && ('\n' == Last[-1]))
			{
			    --Last;
			}
		    if((Last > Buffer) && ('\r' == Last[-1]))
			{
			    --Last;
			}
		}

	    /* Strip comment if any */
	    Cur = Buffer;
	    while(Cur < Last)
		{
		    if('#' == *Cur)
			{
			    Last = Cur;
			    break;
			}
		    ++Cur;
		}

	    /* Count tokens and find size */
	    assert(Last < (Buffer + BUFFSIZE));
	    Len = 0;
	    TokenCount = 0;
	    Cur = Buffer;
	    InToken = false;
	    while(Cur < Last)
		{
		    if(InToken)
			{
			    if((' ' == *Cur) || ('\t' == *Cur))
				{
				    InToken = false;
				}
			    else
				{
				    ++Len;
				}
			}
		    else
			{
			    if((' ' != *Cur) && ('\t' != *Cur))
				{
				    ++TokenCount;
				    ++Len;
				    InToken = true;
				}
			}
		    ++Cur;	/* Advance pointer */
		}
	}
    while(0 == TokenCount);

    /* Find the size of mem to alloc. Use a contiguous block so is 
     * easy to deallocate */
    Len = (sizeof(char) * Len) +	/* Length of actual data */
	(sizeof(char) * TokenCount);	/* Null terminators */

    /* Alloc the pointer list and data list. Count the final 
     * empty string we will use as list terminator */
    Tokens = (char **)my_malloc(sizeof(char *) * (TokenCount + 1));
    *Tokens = (char *)my_malloc(sizeof(char) * Len);

    /* Copy tokens to result */
    Cur = Buffer;
    Dst = *Tokens;
    InToken = false;
    CurToken = 0;
    while(Cur < Last)
	{
	    if(InToken)
		{
		    if((' ' == *Cur) || ('\t' == *Cur))
			{
			    InToken = false;
			    *Dst = '\0';	/* Null term token */
			    ++Dst;
			    ++CurToken;
			}
		    else
			{
			    *Dst = *Cur;	/* Copy char */
			    ++Dst;
			}
		}
	    else
		{
		    if((' ' != *Cur) && ('\t' != *Cur))
			{
			    Tokens[CurToken] = Dst;	/* Set token start pointer */
			    *Dst = *Cur;	/* Copy char */
			    ++Dst;
			    InToken = true;
			}
		}
	    ++Cur;		/* Advance pointer */
	}
    if(InToken)
	{
	    *Dst = '\0';	/* Null term final token */
	    ++Dst;
	    ++CurToken;
	}
    assert(CurToken == TokenCount);

    /* Set the final empty string entry */
    Tokens[CurToken] = NULL;

    /* Return the string list */
    return Tokens;
}


void FPlan::getfinalioblocks()
{
	//collect io block from the file xxx.module.io
	//the name already changed
	//first reset
	finaliosets.clear();
	clkset.clear();
	memoryset.clear();
    multiplyset.clear();

	int countnum = 0;  //record the number of the io
	//the io name in this file has aleready changed. (Final name)
	string moduleiofile = filename + "/" + filename + ".module.io";

	//1> store the clbNetName  will be used later during routing
	FILE *routInfile;
	char **routetokens;
	int rline = 0;
	routInfile = fopen(moduleiofile.c_str(), "r");
	if(!routInfile)
	{
		cout<<"cannot open file: "<<moduleiofile<<endl;
		exit(1);
	}

	routetokens = ReadLineTokens(routInfile, &rline);
	
	if(NULL == routetokens)
	{
		cout<<"bad line specified in the routefile "<<moduleiofile<<endl;
	}
	
	//reorder the net Id
	while(routetokens)   //can support that the last line is a blank line
	{
		if(NULL != routetokens[0]) 
		{
			string blkname = routetokens[0];
			string blktype = routetokens[1];
			if(0 == strcmp(routetokens[1], "io") || 0 == strcmp(routetokens[1], "clk"))
			{
			      finaliosets.insert(blkname);
				  countnum++;
		//		  cout<<"finalIOnum"<<"  "<<finaliosets.size()<<" "<<blkname.c_str()<<"  countnum "<<countnum<<endl;

				  if(0 == strcmp(routetokens[1], blkClkType.c_str())) ////"clk"))
				       clkset.insert(blkname);
			}
			else if(0 == strcmp(routetokens[1], blkRamType.c_str())) ////"memory"))
				memoryset.insert(blkname);
			else if(0 == strcmp(routetokens[1], blkDspType.c_str())) ////"mult_36")) ///"multiplier"))// mulitplier type may need to change later
			    multiplyset.insert(blkname);
			else 
			{
				printf("Error: The block type %s is unknown, pls check\n", blktype.c_str());
				exit(1);
			}
		}
		free(*routetokens);
	    free(routetokens);
		routetokens = ReadLineTokens(routInfile, &rline);
	}
	fclose(routInfile);
	cout<<"The IO number (include clk and IO) in the netlist is : "<<(int)finaliosets.size()<<endl;;
}


void FPlan::getblksnamemaplist()
{
	//first reset
    blksnamemap.clear();
	string moduleiofile = filename + "/" + filename + ".blknamemap";

	//1> store the blkname(in original sub placement file) and the blkname (in pre-vpr placement file), map<blkname, blkname> 
	//using for moving the blks in the final placement file 
	FILE *routInfile;
	char **routetokens;
	int rline = 0;
	routInfile = fopen(moduleiofile.c_str(), "r");
	if(!routInfile)
	{
		cout<<"cannot open file: "<<moduleiofile<<endl;
		exit(1);
	}

	 //first line, is the commence line, the readlinetokne will overlook the commment line with #
	routetokens = ReadLineTokens(routInfile, &rline); 
	//for(int i = 0; !feof(routInfile); i++)
	//{
	if(NULL == routetokens)
	{
		cout<<"bad line specified in the routefile "<<moduleiofile<<endl;
	}
	
	//reorder the net Id
	while(routetokens)   //can support that the last line is a blank line
	{
		if(NULL != routetokens[0]) 
		{
			string originalblkname = routetokens[0];
			string prevprblkname = routetokens[1];
            blksnamemap.insert(make_pair(originalblkname, prevprblkname));
		}
		free(*routetokens);
	    free(routetokens);
		routetokens = ReadLineTokens(routInfile, &rline);
	}
	fclose(routInfile);
	cout<<"The number of blk in blknamemap is : "<<(int)blksnamemap.size()<<endl;
}


/*
//deal with each macro
void FPlan::readAndWrite_place(const char *subplacename,   
	string finalplacefilename,
	string finalroutenetfilename,
	string virtualpinfilename, //for virtual pin
	const char *subplace_file,
	const char *subroute_file,
	int nx,
	int ny,
	float ratio,
	int &blkcount,
	int &ioblkcount,
	int &netcount,
	Module_Info &modInfo,
	string subpostfix,
	vector<bool> &left,
	vector<bool> &right,
	vector<bool> &lower,
	vector<bool> &top,
	bool &leftflag,
	bool &rightflag,
	bool &lowerflag,
	bool &topflag,
	int &flagindex, //(IO capacity (should decided by defined archfile)
	int &zflag,   //from 0, to 7) when big than 7,reset it to 0
	vector<string> &clockSet)
{

	//1> store the clbNetName and clbNetId (virtualpin);  will be used later during routing
	FILE *routInfile;
	char **routetokens;
	int rline;
	routInfile = fopen(subroute_file, "r");
	if(!routInfile)
	{
		cout<<"cannot open file: "<<subroute_file<<endl;
		exit(1);
	}

	routetokens = ReadLineTokens(routInfile, &rline);
	if(NULL == routetokens)
	{
		cout<<"bad line specified in the routefile "<<subroute_file<<endl;
	}
	
	FILE *ffnet = fopen(finalroutenetfilename.c_str(), "a+");
	FILE *ffvirtualnet = fopen(virtualpinfilename.c_str(), "a+");
	if(!ffnet)
	{
		cout<<"cannot open the final routenet file "<<finalroutenetfilename<<endl;
		exit(1);
	}

	if(!ffvirtualnet)
	{
		cout<<"cannot open the final routenet file "<<virtualpinfilename<<endl;
		exit(1);
	}


	//for dealing with reconfigurable region
    //store for dealing with reconfigurable regions
	string glnetname; //global netname
	string tmpcol, tmprow;

	 //used to get inode in route
	bool findsourcepad = false; 
	string channel;//use to get the chan and track
	int tracknum;


	//reorder the net Id
	while(routetokens)
	{
		if(0 == strcmp(routetokens[0], "Net"))
		{
			int netId = atoi(routetokens[1]);
			string tmpnetname = routetokens[2];
			size_t posa = tmpnetname.find("(");
			size_t posb = tmpnetname.find(")");
			string netname;
			if(posa != string::npos && posb != string::npos)
			{
				netname = tmpnetname.substr(posa+1, posb-1);  //need to check the code??
			}
			else
			{
				cout<<"Error: The net is wrong, pls check"<<endl;
				exit(1);
			}

			fprintf(ffnet, "net %d  %s\n", netcount, netname.c_str());
			netcount++;  
		}


		//read route file (for each submodule)
		//---for dealing with virtual pins in later route(for routing)
		//virtual port (netname) path property    virtual pin property   sink(inpin): out   source(opin): in
		//for virtual pin move
		//get netId and netName
		if(0 == strcmp(routetokens[0], "Net"))
		{
			int netId = atoi(routetokens[1]);
			string tmpnetname = routetokens[2];
			size_t posa = tmpnetname.find("(");
			size_t posb = tmpnetname.find(")");
			if(posa != string::npos && posb != string::npos)
			{
				glnetname = tmpnetname.substr(posa+1, posb-1);  //need to check the code??
			}
			else
			{
				cout<<"Error: The net is wrong, pls check"<<endl;
				exit(1);
			}
			////		fprintf(virtualNet, "net %d  %s\n", netcount, glnetname.c_str());
		}


		//get track number  //Track format has two types
		//eg. CHANX (1,0) Track 27   sink pad and source  can be both two types
		//eg. CHANX (1,11) to (4, 11) Track 31 so should differentiate them
		//track type 1  
	   if(NULL != routetokens[0] && NULL != routetokens[1] &&
			NULL != routetokens[2] && 0 == strcmp(routetokens[2], "Track:"))
		{
			tracknum = atoi(routetokens[3]);
			channel = routetokens[0];

			if(findsourcepad)  //1: find sourcepad means encouter the soure of this net
			{
				//modified 
				int testnx = atoi(tmpcol.c_str());
				int testny = atoi(tmprow.c_str());
				//should consider whether it beyonds the bounding box
				if(testnx > nx)   //nx width , ny height  
					testnx = nx;
				if(testny > ny)
					testny = ny;
				

				fprintf(ffvirtualnet, "virtual_port(netname) %s col %d row %d direct %s channel %s track %d path %s\n",\
					glnetname.c_str(), (testnx + modInfo.x), (testny + modInfo.y),\
					"VSINK", channel.c_str(), tracknum, subroute_file);  

				//reset the value to false
				findsourcepad = false;
			}
		}


		//track type 2
		if(NULL != routetokens[0] && NULL != routetokens[1] &&
			NULL != routetokens[2] && NULL != routetokens[3] &&
			NULL != routetokens[4] && 0 == strcmp(routetokens[4], "Track:")) 
		{
			tracknum = atoi(routetokens[5]);
			channel = routetokens[0];

			if(findsourcepad)  //1: find sourcepad means encouter the soure of this net
			{
				//modified 
				int testnx = atoi(tmpcol.c_str());
				int testny = atoi(tmprow.c_str());
				//should consider whether it beyonds the bounding box
				if(testnx > nx)   //nx width , ny height  
					testnx = nx;
				if(testny > ny)
					testny = ny;
				

				fprintf(ffvirtualnet, "virtual_port(netname) %s col %d row %d direct %s channel %s track %d path %s\n",\
					glnetname.c_str(), (testnx + modInfo.x), (testny + modInfo.y),\
					"VSINK", channel.c_str(), tracknum, subroute_file);  


				//reset the value to false
				findsourcepad = false;
			}
		}

		if(NULL != routetokens[0] && NULL != routetokens[1] &&
			NULL != routetokens[2] && 0 == strcmp(routetokens[2], "Pad:"))  //pad format is fixed
		{
			size_t posa, posb;
			string strcolrow; //get the virtual pins col and row
			//only store the source PAD   sink PAD   ??  the track number may not unique, check later
			if(0 == strcmp(routetokens[0], "SOURCE"))
			{
				findsourcepad = true;
				//format eg  SOURCE (2,0)  Pad: 13  
				strcolrow = routetokens[1];
				posa = strcolrow.find(",");
				posb = strcolrow.find(")");
				if(posa != string::npos && posb != string::npos)
				{
					//move the position according to the placement result
					tmpcol = strcolrow.substr(1, posa-1);  //need to check the code??
					tmprow = strcolrow.substr(posa+1, posb-1);
				}
				else
				{
					cout<<"Error: The source format is wrong, pls check"<<endl;
					exit(1);
				}
			}
			else if(0 == strcmp(routetokens[0], "SINK"))
			{
				//sink : out      source : in 
				strcolrow = routetokens[1];
				posa = strcolrow.find(",");
				posb = strcolrow.find(")");
				if(posa != string::npos && posb != string::npos)
				{
					//modified
					//move the position according to the placement result
					tmpcol = strcolrow.substr(1, posa-1);  //need to check the code??
					tmprow = strcolrow.substr(posa+1, posb-1);
														//modified 
				    int testnx = atoi(tmpcol.c_str());
				    int testny = atoi(tmprow.c_str());
				    //should consider whether it beyonds the bounding box
				    if(testnx > nx)   //nx width , ny height  
					   testnx = nx;
				    if(testny > ny)
					   testny = ny;

					fprintf(ffvirtualnet, "virtual_port(netname) %s col %d row %d direct %s channel %s track %d path %s\n",\
						glnetname.c_str(),(testnx + modInfo.x), (testny + modInfo.y),
						"VSOURCE", channel.c_str(), tracknum, subroute_file);  

				}
				else
				{
					cout<<"Error: The source format is wrong, pls check"<<endl;
					exit(1);
				}
			}
		} //if

		free(*routetokens);
	    free(routetokens);
		routetokens = ReadLineTokens(routInfile, &rline);
	}//while

	fclose(ffnet);
	fclose(ffvirtualnet);
	fclose(routInfile);


      //2> store place file, move the blk and IO   according to the netlist file
	//Notice: 
	//??record x ++, y++.. since cannot place in the same row or column
	FILE *infile;
	char **tokens;
	int line;
	string archfile = "6k6_n8.xml";
	string newnetfilename = filename + ".net";
	FILE *ff = fopen(finalplacefilename.c_str(), "a+");
	if(!ff)
	{
		cout<<"cannot open the file "<<finalplacefilename<<endl;
		exit(1);
	}

	if(0 == blkcount)
	{    //filename like bgm
		fprintf(ff, "Netlist file: %s   Architecture file: %s\n", newnetfilename.c_str(),
			archfile.c_str());
		fprintf(ff, "Array size: %d x %d logic blocks\n\n", (int)Width, (int)Height);
		fprintf(ff, "#block name\tx\ty\tsubblk\tblock number\n");
		fprintf(ff, "#----------\t--\t--\t------\t------------\n");
	}

	infile = fopen(subplace_file, "r");
	if(!infile)
	{
		cout<<"it cannot open the file "<<subplace_file<<endl;
		exit(1);
	}
	// Check filenames in first line match 
	tokens = ReadLineTokens(infile, &line);
	int error = 0;
	if(NULL == tokens)
	{
		error = 1;
	}
	for(int i = 0; i < 6; ++i)
	{
		if(!error)
		{
			if(NULL == tokens[i])
			{
				error = 1;
			}
		}
	}
	if(!error)
	{
		if((0 != strcmp(tokens[0], "Netlist")) ||
			(0 != strcmp(tokens[1], "file:")) ||
			(0 != strcmp(tokens[3], "Architecture")) ||
			(0 != strcmp(tokens[4], "file:")))
		{
			error = 1;
		};
	}
	if(error)
	{
		printf("error:\t"
			"'%s' - Bad filename specification line in placement file\n",
			subplace_file);
		exit(1);
	}


	// Check array size in second line matches 
	tokens = ReadLineTokens(infile, &line);
	error = 0;
	if(NULL == tokens)
	{
		error = 1;
	}
	for(int i = 0; i < 7; ++i)
	{
		if(!error)
		{
			if(NULL == tokens[i])
			{
				error = 1;
			}
		}
	}
	if(!error)
	{
		if((0 != strcmp(tokens[0], "Array")) ||
			(0 != strcmp(tokens[1], "size:")) ||
			(0 != strcmp(tokens[3], "x")) ||
			(0 != strcmp(tokens[5], "logic")) ||
			(0 != strcmp(tokens[6], "blocks")))
		{
			error = 1;
		};
	}
	if(error)
	{
		printf("error1 :\t"
			"'%s' - Bad fpga size specification line in placement file\n",
			subplace_file);
		exit(1);
	}
	if((atoi(tokens[2]) != nx) || (atoi(tokens[4]) != ny))
	{
		printf("error1 :\t"
			"'%s' - Current FPGA size (%d x %d) is different from "
			"size when placement generated (%d x %d)\n", subplace_file,
			nx, ny, atoi(tokens[2]), atoi(tokens[4]));
		exit(1);
	}

	tokens = ReadLineTokens(infile, &line);
	//first judge whether it is clb blocks and then move x, y ???????
	map<string, map<float, set<string> > >::iterator ita;
	map<float, set<string> >::iterator itb;
	set<string> blknameset;
	map<string, string>::iterator itmapc;

	//clkflag   blkcount donnot change
	bool clkflag = false;

	//filename, ratio, blkname    //in module.io file, memory has a postfix, but io doesn't have. deal with them different
	ita = filenameRatioBlkname.find(subplacename);
	if(ita != filenameRatioBlkname.end())
	{
		itb = ita->second.find(ratio);
		if(itb != ita->second.end())
		{
			blknameset = itb->second;
			while(tokens)
			{
				//clkflag only keep one clock in the final place file
				clkflag = false;

				//// Set clb coords
				//char subpostfix[80];
				////need to check whether it converts right
				//itoa(count,subpostfix, 10); 
				string tempa = tokens[0];
				int x = atoi(tokens[1]);
				int y = atoi(tokens[2]);
				int z = atoi(tokens[3]);
				set<string>::iterator itc = blknameset.find(tempa);
				set<string>::iterator ioit = finaliosets.find(tempa);

				string tempblkname;
				//tempblkname = tempa + "*" + subpostfix;
				tempblkname = tempa + "*" + subplacename;

				string tempmemoryblkname;
				itmapc = blksnamemap.find(tempblkname);
				if(itmapc != blksnamemap.end())
				{   //use a new name subsitude the original blk name
						tempmemoryblkname = itmapc->second;
				}
				else
				{
						cout<<"Error: blk name "<<tempblkname<<" doesn't exist in blknamemap list, pls check!"<<endl;
						exit(1);
				}


				set<string>::iterator memoryit = memoryset.find(tempmemoryblkname); //since memory get from module.io file, it has postfix
				if(itc != blknameset.end()) //the block  is a clb block
				{   //need to modified the blk name and coordinate, based on coordinat(detaX, detaY)
					//tempblkname = tempa + "*" + subpostfix;		
					itmapc = blksnamemap.find(tempblkname);
					if(itmapc != blksnamemap.end())
					{   //use a new name subsitude the original blk name
						tempblkname = itmapc->second;
					}
					else
					{
						cout<<"Error: the clb blk name "<<tempblkname<<" is not exist in blknamemap list, pls check!"<<endl;
						exit(1);
					}
					
					fprintf(ff, "%s\t", tempblkname.c_str());

					if(strlen(tempblkname.c_str()) < 8)
						fprintf(ff, "\t");
					//move clb block's coordinate //subblk donnot need to move, for clb's subblk is (0,0)
					fprintf(ff, "%d\t%d\t%d", x+modInfo.x, y+modInfo.y, z);
					fprintf(ff, "\t#%d\n", blkcount);
					blkcount++;
				}      
				else if(memoryit != memoryset.end())  //if it has two or more memory, the codes should be changed
				{   //set memoryblock coordinate,  first give a fixed coordinate
					int memoryx = 14;
					int memoryy = 1;
					int memoryz = 0;
					fprintf(ff, "%s\t", tempmemoryblkname.c_str());
					if(strlen(tempmemoryblkname.c_str()) < 8)
						fprintf(ff, "\t");
					//move clb block's coordinate //subblk donnot need to move, for clb's subblk is (0,0)
				//	fprintf(ff, "%d\t%d\t%d", 14, 1, 0);
				    fprintf(ff, "%d\t%d\t%d", x+modInfo.x, y+modInfo.y, z);
					fprintf(ff, "\t#%d\n", blkcount);
					blkcount++;
				}
					//the code below should changed later
				else if(ioit != finaliosets.end())   // || 0 == strcmp(tempa.c_str(), "top.single_port_ram+str^out~6")) //the block is IO block, donnot need to move IO (need to check)
				{    //only io in the netlist should need to store in the place file
					//only keep one topclock
					if(clockSet.size() > 0 && tempa == clockSet[0])
					{
						if(clockSet.size() < 2)
						     clockSet.push_back(tempa);
						else
						{
							clkflag = true;
						}
					}

						//true: has occupied    false: donnot be used now
	                    //vector<bool> left; //x = 0 col , y from 1 to ny row
	                    //vector<bool> right; //x = nx+1, y from 1 to ny row
	                    //vector<bool> lower; //x from 1 to nx,    y = 0
	                    //vector<bool> top;    //x from 1 to nx,y = ny+1

					if(!clkflag)
					{
						ioblkcount++;  //to check whether the final io number match or not
						//IO x, y, z should change
						//choose a legal position for the IO
						if(false == leftflag)
						{
							if(zflag > 7)
								zflag = 0;

							x = 0;
							y = flagindex; //y begin from 1
							z = zflag;
							zflag++;
							if(zflag > 7)  //7 should get from the architecture file
							{
								flagindex++;
							}

							if(flagindex > (int)Height)
							{
								leftflag = true;
								flagindex = 1;
							}
						}
						else if(false == rightflag)
						{
							if(zflag > 7)
								zflag = 0;

							x = (int)Width + 1;
							y = flagindex;
							z = zflag;
							zflag++;
							if(zflag > 7)
								flagindex++;

							if(flagindex > (int)Height)
							{
								rightflag = true;
								flagindex = 1;
							}
						}
						else if(false == lowerflag)
						{
							if(zflag > 7)
								zflag = 0;

							x = flagindex;
							y = 0;
							z = zflag;
							zflag++;

							if(zflag > 7)
								flagindex++;

							if(flagindex > (int)Width)
							{
								lowerflag = true;
								flagindex = 1;
							}
						}
						else if(false == topflag)
						{
							if(zflag > 7)
								zflag = 0;

							x = flagindex;
							y = (int)Height + 1;
							z = zflag;
							zflag++;

							if(zflag > 7)
								flagindex++;

							if(flagindex > (int)Width)
							{
								topflag = true;
								flagindex = 1;
							}
						}
						else 
						{ 
							cout<<"Error:  IO positions are not enough, Pls check with it"<<endl;
							exit(1);
						}

						//string tempblkname = tempa + "_" + subpostfix;
						tempblkname = tokens[0];  //keep the original name
						fprintf(ff, "%s\t", tempblkname.c_str());
						if(strlen(tempblkname.c_str()) < 8)
							fprintf(ff, "\t");
						fprintf(ff, "%d\t%d\t%d", x, y, z);
						fprintf(ff, "\t#%d\n", blkcount);
						blkcount++;
					}
				}

//				if(!clkflag)
//				    blkcount++;			
				free(*tokens);
				free(tokens);
				tokens = ReadLineTokens(infile, &line);
			}
		}
		else
		{
			cout<<"filename1 "<<subplacename<<"'s ratio "<<ratio<<" is wrong, pls check"<<endl;
			exit(1);
		}
	}
	else
	{
		cout<<"filename "<<subplacename<<" is wrong, pls check"<<endl;
		exit(1);
	}

	fclose(infile);
	fclose(ff);
}
*/



//fbmao modify  for moving IO according the module.io file directly 06/12/2013
//deal with each macro
void FPlan::readAndWrite_place(const char *subplacename,   
	string finalplacefilename,
	string finalroutenetfilename,
	string virtualpinfilename, //for virtual pin
	const char *subplace_file,
	const char *subroute_file,
	int nx,
	int ny,
	float ratio,
	int &blkcount,
	int &ioblkcount,
	int &netcount,
	Module_Info &modInfo,
	string subpostfix,
	vector<bool> &left,
	vector<bool> &right,
	vector<bool> &lower,
	vector<bool> &top,
	bool &leftflag,
	bool &rightflag,
	bool &lowerflag,
	bool &topflag,
	int &flagindex, //(IO capacity (should decided by defined archfile)
	int &zflag,   //from 0, to 7) when big than 7,reset it to 0
	vector<string> &clockSet)
{
    //1> store place file, move the blk and IO   according to the netlist file
	//Notice: 
	//??record x ++, y++.. since cannot place in the same row or column
	FILE *infile;
	char **tokens;
	int line;
	string archfile = "6k6_n8.xml";
	string newnetfilename = filename + ".net";
	FILE *ff = fopen(finalplacefilename.c_str(), "a+");
	if(!ff)
	{
		cout<<"cannot open the file "<<finalplacefilename<<endl;
		exit(1);
	}

    //add for loadable flow, for getting the whole place file
	string ioforfinalplacefile = filename + "/placefiles/" + filename + ".ioplace";
	FILE *ioforfinalplace = fopen(ioforfinalplacefile.c_str(), "a+");
	if(!ioforfinalplace)
	{
		cout<<"cannot open the file "<<ioforfinalplacefile.c_str()<<endl;
		exit(1);
	}

	string fforfinalplacefile = filename + "/" + subplacename + ".clbplace";
	FILE *forfinalplace = fopen(fforfinalplacefile.c_str(), "w+");
	if(!forfinalplace)
	{
		cout<<"cannot open the file "<<fforfinalplacefile.c_str()<<endl;
		exit(1);
	}


	if(0 == blkcount)
	{    //filename like bgm
		//fprintf(ff, "Netlist file: %s   Architecture file: %s\n", newnetfilename.c_str(),
		//	archfile.c_str());
		fprintf(ff, "Netlist file: %s   Architecture file: %s\n", newnetfilename.c_str(),
			archname.c_str());
		fprintf(ff, "Array size: %d x %d logic blocks\n\n", (int)Width, (int)Height);
		fprintf(ff, "#block name\tx\ty\tsubblk\tblock number\n");
		fprintf(ff, "#----------\t--\t--\t------\t------------\n");



		//add for loadable flow, can be used to combine subfunction place file into a 
		//whole net after loadable flow
		fprintf(ioforfinalplace, "Netlist file: %s   Architecture file: %s\n", newnetfilename.c_str(),
			archname.c_str());
		fprintf(ioforfinalplace, "Array size: %d x %d logic blocks\n\n", (int)Width, (int)Height);
		fprintf(ioforfinalplace, "#block name\tx\ty\tsubblk\tblock number\n");
		fprintf(ioforfinalplace, "#----------\t--\t--\t------\t------------\n");

	}

	fclose(ioforfinalplace);


	infile = fopen(subplace_file, "r");
	if(!infile)
	{
		cout<<"it cannot open the file "<<subplace_file<<endl;
		exit(1);
	}
	// Check filenames in first line match 
	tokens = ReadLineTokens(infile, &line);
	int error = 0;
	if(NULL == tokens)
	{
		error = 1;
	}
	for(int i = 0; i < 6; ++i)
	{
		if(!error)
		{
			if(NULL == tokens[i])
			{
				error = 1;
			}
		}
	}
	if(!error)
	{
		if((0 != strcmp(tokens[0], "Netlist")) ||
			(0 != strcmp(tokens[1], "file:")) ||
			(0 != strcmp(tokens[3], "Architecture")) ||
			(0 != strcmp(tokens[4], "file:")))
		{
			error = 1;
		};
	}
	if(error)
	{
		printf("error:\t"
			"'%s' - Bad filename specification line in placement file\n",
			subplace_file);
		exit(1);
	}


	// Check array size in second line matches 
	tokens = ReadLineTokens(infile, &line);
	error = 0;
	if(NULL == tokens)
	{
		error = 1;
	}
	for(int i = 0; i < 7; ++i)
	{
		if(!error)
		{
			if(NULL == tokens[i])
			{
				error = 1;
			}
		}
	}
	if(!error)
	{
		if((0 != strcmp(tokens[0], "Array")) ||
			(0 != strcmp(tokens[1], "size:")) ||
			(0 != strcmp(tokens[3], "x")) ||
			(0 != strcmp(tokens[5], "logic")) ||
			(0 != strcmp(tokens[6], "blocks")))
		{
			error = 1;
		};
	}
	if(error)
	{
		printf("error1 :\t"
			"'%s' - Bad fpga size specification line in placement file\n",
			subplace_file);
		exit(1);
	}


	// the variable below is used for dealing with heterogenous arch
	//read from arch file
	//memory
	int memstart = 7;
	int memrepeat = 10;
	int memoryheight = 2;

	int multistart = 6;
	int multirepeat = 8;
	int multiheight = 4;
	int memoryextracol = 0, multiplyextracol = 0;
	int tmpnx = atoi(tokens[2]);
	int tmpmodwidth = atoi(tokens[2]);// original module width, but if it uses the memory arch, then the nx is changed 
	                                  //whem move the coordinate in the place file, should use orginal width as reference,not the nx
	//nx should be changed for supporting heterogenous arch
	if(isheteroarch && tmpnx >= memstart)  //??if arch is not heterogenous, then set a parameter that memstart == 0?? should change later
	{
		memoryextracol = (tmpnx - memstart) / memrepeat + 1;
		tmpnx = tmpnx - memoryextracol;
		//reset 
		memoryextracol = 0;
	}

	if(tmpnx != nx || (atoi(tokens[4]) != ny))
	{
		printf("error1 :\t"
			"'%s' - Current FPGA size (%d x %d) is different from "
			"size when placement generated (%d x %d)\n", subplace_file,
			nx, ny, tmpnx, atoi(tokens[4]));
		exit(1);
	}

	tokens = ReadLineTokens(infile, &line);
	//first judge whether it is clb blocks and then move x, y ???????
	map<string, map<float, set<string> > >::iterator ita;
	map<float, set<string> >::iterator itb;
	set<string> blknameset;
	map<string, string>::iterator itmapc;
	//clkflag   blkcount donnot change
	bool clkflag = false;

	//filename, ratio, blkname    //in module.io file, memory has a postfix, but io doesn't have. deal with them different
	ita = filenameRatioBlkname.find(subplacename);
	if(ita != filenameRatioBlkname.end())   //if ita == filenameRatioBlkname.end()   means that the subfile only contains IO blocks
	{
		itb = ita->second.find(ratio);
		if(itb != ita->second.end())
		{
			blknameset = itb->second;
			while(tokens)
			{
				//clkflag only keep one clock in the final place file
				clkflag = false;
				//// Set clb coords
				//char subpostfix[80];
				////need to check whether it converts right
				//itoa(count,subpostfix, 10); 
				string tempa = tokens[0];
				int x = atoi(tokens[1]);
				int y = atoi(tokens[2]);
				int z = atoi(tokens[3]);

				//for support heterogenous architecture
				//deal with io especially, io on the boudary and in 
			    if(isheteroarch && x >= memstart && tmpmodwidth >= memstart)  //nx >= memstart means it contain at least one column memory etc.
	            {
				   if(tmpmodwidth + 1 == x)
				   {   //if x in the io boundary
					   memoryextracol = (x -1 - memstart) / memrepeat + 1; 
				   }
				   else
				   {
                       memoryextracol = (x - memstart) / memrepeat + 1;
				   } 
	               x = x - memoryextracol;    //delete the memory column number first
	            }

				set<string>::iterator itc = blknameset.find(tempa);
				set<string>::iterator ioit = finaliosets.find(tempa);
				string tempblkname;
				//tempblkname = tempa + "*" + subpostfix;
				tempblkname = tempa + "*" + subplacename;
				string tempmemoryblkname;
				itmapc = blksnamemap.find(tempblkname);
				if(itmapc != blksnamemap.end())
				{   //use a new name subsitude the original blk name
				    tempmemoryblkname = itmapc->second;
				}
				else
				{
				    cout<<"Error: blk name "<<tempblkname<<" doesn't exist in blknamemap list, pls check!"<<endl;
					exit(1);
				}

				set<string>::iterator memoryit = memoryset.find(tempmemoryblkname); //since memory get from module.io file, it has postfix
				if(itc != blknameset.end()) //the block  is a clb block
				{   //need to modified the blk name and coordinate, based on coordinat(detaX, detaY)
					//tempblkname = tempa + "*" + subpostfix;		
					itmapc = blksnamemap.find(tempblkname);
					if(itmapc != blksnamemap.end())
					{   //use a new name subsitude the original blk name
						tempblkname = itmapc->second;
					}
					else
					{
						cout<<"Error: the clb blk name "<<tempblkname<<" is not exist in blknamemap list, pls check!"<<endl;
						exit(1);
					}
					fprintf(ff, "%s\t", tempblkname.c_str());
					if(strlen(tempblkname.c_str()) < 8)
						fprintf(ff, "\t");


				 //store clb blks coordinates, that can be used to combine subfunction place file to 
				 // a complete place file after the loadable flow (when some regions needed to be changed)
					fprintf(forfinalplace, "%s\t", tempblkname.c_str());
					if(strlen(tempblkname.c_str()) < 8)
						fprintf(forfinalplace, "\t");



                  //support heterogenous block (arch file contain memory, multiply, etc.)
					//move clb block's coordinate //subblk donnot need to move, for clb's subblk is (0,0)
					//convert <x_modInfo.x, y+modinfo.y, z>  ->  
					int tmpfpgaX = x + modInfo.x;
					map<int, int>::iterator itxtox = floorplanXtofpgaX.find(x+modInfo.x);
					if(itxtox != floorplanXtofpgaX.end())
					{
						tmpfpgaX = itxtox->second;
					}
					else
					{
						cout<<"Error: floorplanXtofpgaX has error, the floorplanX can't be found"<<endl;
						exit(1);
					}
					fprintf(ff, "%d\t%d\t%d", tmpfpgaX, y+modInfo.y, z);

					//fprintf(ff, "%d\t%d\t%d", x+modInfo.x, y+modInfo.y, z);
					fprintf(ff, "\t#%d\n", blkcount);

					//can be used to combine subfunction place file into a whole place file
					//after loadable work
					fprintf(forfinalplace, "%d\t%d\t%d", tmpfpgaX, y+modInfo.y, z);
					fprintf(forfinalplace, "\t#%d\n", blkcount);

					blkcount++;
				}      
				else if(memoryit != memoryset.end())  //if it has two or more memory, the codes should be changed
				{   //set memoryblock coordinate,  first give a fixed coordinate
					
					//for memory, map<memoryname, coordinate<x,y,z> >
					//use the memory name to get coordinate
					
/*					
					int memoryx = 14;
					int memoryy = 1;
					int memoryz = 0;
					fprintf(ff, "%s\t", tempmemoryblkname.c_str());
					if(strlen(tempmemoryblkname.c_str()) < 8)
						fprintf(ff, "\t");
					//move clb block's coordinate //subblk donnot need to move, for clb's subblk is (0,0)
				//	fprintf(ff, "%d\t%d\t%d", 14, 1, 0);
				    fprintf(ff, "%d\t%d\t%d", x+modInfo.x, y+modInfo.y, z);
					fprintf(ff, "\t#%d\n", blkcount);
					blkcount++;
*/
				}
				else //for choose io block's side (it belongs to which side) 
				{    //using IO coordinate and the height, width of the floorplan result
					//it is io
					struct IOposition tempiopos;
					map<string, struct IOposition>::iterator ioposit;
					// map<string, struct IOposition> ionameIopos;  it will have 8 regions
					int tempiox = x + modInfo.x;
					int tempioy = y + modInfo.y;
					//each IO in original place, have four sides
					if(0 == x)
					{
						if(tempiox <= (int)(Width/2))
						{
							if(tempioy <= (int)(Height/2))
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.left1++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 1;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
							else
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.left2++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 1;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
						}
						else //(tempiox > (int)(Width/2))
						{
							if(tempioy <= (int)(Height/2))
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.bottom2++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 1;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
							else
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.top2++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 1;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
						}
					}
					else if(0 == y)
					{
						if(tempiox <= (int)(Width/2))
						{
							if(tempioy <= (int)(Height/2))
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.bottom1++;
								}
								else
								{
									tempiopos.bottom1 = 1;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
							else
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.left2++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 1;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
						}
						else //(tempiox > (int)(Width/2))
						{
							if(tempioy <= (int)(Height/2))
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.bottom2++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 1;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
							else
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.right2++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 1;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
						}
					}else if(nx+1 == x)
					{
						if(tempiox <= (int)(Width/2))
						{
							if(tempioy <= (int)(Height/2))
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.bottom1++;
								}
								else
								{
									tempiopos.bottom1 = 1;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
							else
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.top1++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 1;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
						}
						else //(tempiox > (int)(Width/2))
						{
							if(tempioy <= (int)(Height/2))
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.right1++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 1;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
							else
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.right2++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 1;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
						}
					}else if(ny +1 == y)
					{
						if(tempiox <= (int)(Width/2))
						{
							if(tempioy <= (int)(Height/2))
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.left1++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 1;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
							else
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.top1++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 1;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
						}
						else //(tempiox > (int)(Width/2))
						{
							if(tempioy <= (int)(Height/2))
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.right1++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 1;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 0;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
							else
							{
								ioposit = ionameIopos.find(tempa);
								if(ioposit != ionameIopos.end())
								{
									struct IOposition& temppos = ioposit->second;
									temppos.top2++;
								}
								else
								{
									tempiopos.bottom1 = 0;
									tempiopos.bottom2 = 0;
									tempiopos.left1 = 0;
									tempiopos.left2 = 0;
									tempiopos.right1 = 0;
									tempiopos.right2 = 0;
									tempiopos.top1 = 0;
									tempiopos.top2 = 1;
									ionameIopos.insert(make_pair(tempa, tempiopos));
								}
							}
						}
					}else
					{
						cout<<"IO position in the placement file is wrong,  x  "<<x<<" y "<<y<<endl;
						exit(1);
					}
				}
	
				free(*tokens);
				free(tokens);
				tokens = ReadLineTokens(infile, &line);
			}
		}
		else
		{
			cout<<"filename1 "<<subplacename<<"'s ratio "<<ratio<<" is wrong, pls check"<<endl;
			exit(1);
		}
	}
	else
	{
		cout<<"filename "<<subplacename<<" is wrong(lost), pls check"<<endl;
		exit(1);
	}

	fclose(infile);
	fclose(ff);
	fclose(forfinalplace);
}


//return x and y coordinate according to the portname and subplacename
void FPlan::readplacefileAndReturnXYcoordinates(string portname, 
	           const char *subplace_file, int &xcoord, int &ycoord)
{
    //used to check, whether this io blk in the place file, it must should be in
	xcoord = -1, ycoord = -1;  
	//2> store place file, move the blk and IO   according to the netlist file
	//Notice: 
	//??record x ++, y++.. since cannot place in the same row or column
	FILE *infile;
	char **tokens;
	int line;
	infile = fopen(subplace_file, "r");
	if(!infile)
	{
		cout<<"it cannot open the file "<<subplace_file<<endl;
		exit(1);
	}
	// Check filenames in first line match 
	tokens = ReadLineTokens(infile, &line);
	int error = 0;
	if(NULL == tokens)
	{
		error = 1;
	}
	for(int i = 0; i < 6; ++i)
	{
		if(!error)
		{
			if(NULL == tokens[i])
			{
				error = 1;
			}
		}
	}
	if(!error)
	{
		if((0 != strcmp(tokens[0], "Netlist")) ||
			(0 != strcmp(tokens[1], "file:")) ||
			(0 != strcmp(tokens[3], "Architecture")) ||
			(0 != strcmp(tokens[4], "file:")))
		{
			error = 1;
		};
	}
	if(error)
	{
		printf("error:\t"
			"'%s' - Bad filename specification line in placement file\n",
			subplace_file);
		exit(1);
	}


	// Check array size in second line matches 
	tokens = ReadLineTokens(infile, &line);
	error = 0;
	if(NULL == tokens)
	{
		error = 1;
	}
	for(int i = 0; i < 7; ++i)
	{
		if(!error)
		{
			if(NULL == tokens[i])
			{
				error = 1;
			}
		}
	}
	if(!error)
	{
		if((0 != strcmp(tokens[0], "Array")) ||
			(0 != strcmp(tokens[1], "size:")) ||
			(0 != strcmp(tokens[3], "x")) ||
			(0 != strcmp(tokens[5], "logic")) ||
			(0 != strcmp(tokens[6], "blocks")))
		{
			error = 1;
		};
	}
	if(error)
	{
		printf("error1 :\t"
			"'%s' - Bad fpga size specification line in placement file\n",
			subplace_file);
		exit(1);
	}

	tokens = ReadLineTokens(infile, &line);
	while(tokens)
	{
		//// Set clb coords
		//char subpostfix[80];
		////need to check whether it converts right
		//itoa(count,subpostfix, 10); 
		string tempa = tokens[0];
		int x = atoi(tokens[1]);
		int y = atoi(tokens[2]);
		int z = atoi(tokens[3]);

		if(0  == strcmp(portname.c_str(), tempa.c_str()))
		{
			xcoord = x;
			ycoord = y;
			break;
		}
	    free(*tokens);
	    free(tokens);
		tokens = ReadLineTokens(infile, &line);
	}

	fclose(infile);
}


//for support heterogenoeous architecture
//store blks' coordinate
void FPlan::updateSubModuleWidthAndHeight(string modname, float ratio, const int modwidth, 
	const int modheight, int &blankRamNum, int &blankDspNum,
	Module &mod)
	//	 map<float, map<int, map<pair<int, int>, string> > > &ratioxyzblkname)
{
	//for each module, it corresponds to a place file
	ifstream tmpupdatemodsizefs;
	string tmpratio, sratio;
	stringstream ss;
	ss.str(""); //release temp memory
	ss<<ratio;
	tmpratio = ss.str();
	size_t pos = tmpratio.find(".");
	if(pos != string::npos)
		sratio = tmpratio.substr(0,pos) + tmpratio.substr(pos+1, pos+2);
	else
		sratio = tmpratio + "0";
	string submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + modname + ".v/" + modname + ".place";
	tmpupdatemodsizefs.open(submoduleplacefile.c_str());
	if(!tmpupdatemodsizefs)
	{   //judge whether file exist or not??
		//notice the path
		printf("cannot open the file %s\n", const_cast<char*>(submoduleplacefile.c_str()));
		submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + modname + ".blif/" + modname + ".place";
		tmpupdatemodsizefs.close();
		tmpupdatemodsizefs.open(submoduleplacefile.c_str());
		if(!tmpupdatemodsizefs)
		{
			cout<<"cannot open the file: "<<submoduleplacefile<<endl;
			exit(1);
		}
	}

	tmpupdatemodsizefs.close();

	//get the blk coord, then store it into filenameRatioioBlknamecoord
	// map<string, map<float, map<string, Finaliopos> > > filenameRatioioBlknamecoord;
	//read place file and store the coordinate
	//if a file only have io, it may have problem, now assume each module has at least one clb
	//if(0 == strcmp(submoduleplacefile.c_str(),"branchresolve"))
	//	cout<<"check the bug "<<endl; 

	// recording all the blk's coordinate
	map<float, map<int, map<pair<int, int>, string> > > &ratioxyzblkname = mod.ratioxyzblkname;

	//1>. record all the blks' coordinate from VPR file of each submodule
	readplacefileAndstoreIOcoordinates(submoduleplacefile, modname, ratio, ratioxyzblkname);

	// 2> after recording all the blk's coordinate
	//  then remove the blank ram and dsp. and move the blks' coordinate. Keep height unchanged
	// x+1 ->col??

	//2.1> record the ram, dsp number acoording to the architecture
	//map<x, map<pair(y,z), blkname> >
	map<float, map<int, map<pair<int, int>, string> > >::iterator ita = ratioxyzblkname.find(ratio);
	assert(ita != ratioxyzblkname.end());
	map<int, map<pair<int, int>, string> > &tmpxyzblk = ita->second;

	//bake for later moving blks
	map<int, map<pair<int, int>, string> >  bakexyzblkname = ita->second;
	map<int, map<pair<int, int>, string> >::iterator itb;

	set<int> tmpemptyramcolid;
	set<int> tmpemptydspcolid;
	tmpemptyramcolid.clear();
	tmpemptydspcolid.clear();

	set<int>tempemptyramdspcolid;
	tempemptyramdspcolid.clear();

	//arch info 
	int tmpcola = fmemstart; 
	int tmpramnum = 0;

	int tmpcolb = fmultistart;
	int tmpdspnum = 0;

	int ramtotalcolnum = 0; //in each region
	int dsptotalcolnum = 0;

	//collect blank RAM
	while(tmpcola <= modwidth)
	{
		itb = tmpxyzblk.find(tmpcola);
		if(itb != tmpxyzblk.end()) //this block with column has alread used (means has memory)
		{
			ramtotalcolnum++; //this col is for placing ram
			map<pair<int, int>, string> &tmpyzblk = itb->second;
			tmpramnum += tmpyzblk.size();
		}
		else
		{
			blankRamNum++;
			tmpemptyramcolid.insert(tmpcola);
			tempemptyramdspcolid.insert(tmpcola);
		}
		tmpcola += fmemrepeat;
	}

	//collect blank DSP
	while(tmpcolb <= modwidth)
	{
		itb = tmpxyzblk.find(tmpcolb);
		if(itb != tmpxyzblk.end())
		{
			dsptotalcolnum++; ///this column for placing dsp
			map<pair<int, int>, string> &tmpyzblk = itb->second;
			tmpdspnum += tmpyzblk.size();
		}
		else
		{
			blankDspNum++;
			tmpemptydspcolid.insert(tmpcolb);
			tempemptyramdspcolid.insert(tmpcolb);
		}
		tmpcolb += fmultirepeat;
	}

	//assing the total number dsp,ram for each module
	mod.numram = tmpramnum;
	mod.numdsp = tmpdspnum;

	//record the number of columns that containg RAM and DSP in the module (Region)
	map<string, map<float, pair<int, int> > >::iterator fileratioramdspcolit = filenameRatioramdspcolnum.find(modname);
	if(fileratioramdspcolit != filenameRatioramdspcolnum.end())
	{
		map<float, pair<int, int> > & tmpratioramdspcol = fileratioramdspcolit->second;
		map<float, pair<int, int> > ::iterator ratioramdspcolit = tmpratioramdspcol.find(ratio);
		assert(ratioramdspcolit == tmpratioramdspcol.end());
		tmpratioramdspcol.insert(make_pair(ratio, make_pair(ramtotalcolnum, dsptotalcolnum)));
	}
	else
	{
		map<float, pair<int, int> > ratioramdspcoltemp;
		ratioramdspcoltemp.insert(make_pair(ratio, make_pair(ramtotalcolnum, dsptotalcolnum)));
		filenameRatioramdspcolnum.insert(make_pair(modname, ratioramdspcoltemp));
	}


	//2.2> remove the blank RAM, DSP. And then change its X coordinate (update x)
	if(0 != blankRamNum || 0 != blankDspNum)  //if has blank column, then need to remove them
	{
		//first check the x in which region [x1, x2]
		//then then judge how many columns are deleted. then move the value.
		//remove blank ram,dsp

		tmpxyzblk.clear(); //first clear coordinates and then insert new value
		set<int>::iterator ramdspit;

		//module only has clb. remove ram, dsp column
		for(map<int, map<pair<int, int>, string> >::iterator itc = bakexyzblkname.begin();
			itc != bakexyzblkname.end();
			itc++)
		{
			int tmpx = itc->first;
			map<pair<int, int>, string> tmpyzblk = itc->second;
			//record the times the blk needs to move  after removing memory and dsp columns  
			//count = 0  correspond to firstblank column. the x less than it. do not need change
			int count = 0; 
			//int firstblankcol = -1;
			for(set<int>::iterator itd = tempemptyramdspcolid.begin();
				itd != tempemptyramdspcolid.end();
				itd++, count++)
			{
				//find the first blank col which is larger than tmpx
				if(tmpx < *itd)
				{
					//firstblankcol = *itd;
					break;
				}
			}

			//find the blank col that larger than current x.
			//update x, (the coordinate system is the same as the original VPR result)
			int newx = tmpx - count;
			tmpxyzblk.insert(make_pair(newx, tmpyzblk));
		}
	}
}



//submoduleplacefile is the place file path, subfilename is the sub module file 
//store all the blks's coordinate.  CLB, RAM, DSP
void FPlan::readplacefileAndstoreIOcoordinates(string submoduleplacefile, string subfilename, float submoduleratio,
	                                       map<float, map<int, map<pair<int, int>, string> > > &ratioxyzblkname)
{
	//this function works to store Io blk coordinates.  CLB, RAM, DSP coordinates
	int xcoord = -1, ycoord = -1, zcoord = -1;  //initial x, y,z  
	FILE *infile;
	char **tokens;
	int line;
	infile = fopen(submoduleplacefile.c_str(), "r");
	if(!infile)
	{
		cout<<"it cannot open the file "<<submoduleplacefile.c_str()<<endl;
		exit(1);
	}
	// Check filenames in first line match 
	tokens = ReadLineTokens(infile, &line);
	int error = 0;
	if(NULL == tokens)
	{
		error = 1;
	}
	for(int i = 0; i < 6; ++i)
	{
		if(!error)
		{
			if(NULL == tokens[i])
			{
				error = 1;
			}
		}
	}
	if(!error)
	{
		if((0 != strcmp(tokens[0], "Netlist")) ||
			(0 != strcmp(tokens[1], "file:")) ||
			(0 != strcmp(tokens[3], "Architecture")) ||
			(0 != strcmp(tokens[4], "file:")))
		{
			error = 1;
		}
	}
	if(error)
	{
		printf("error:\t"
			"'%s' - Bad filename specification line in placement file\n",
			submoduleplacefile.c_str());
		exit(1);
	}


	// Check array size in second line matches 
	tokens = ReadLineTokens(infile, &line);
	error = 0;
	if(NULL == tokens)
	{
		error = 1;
	}
	for(int i = 0; i < 7; ++i)
	{
		if(!error)
		{
			if(NULL == tokens[i])
			{
				error = 1;
			}
		}
	}
	if(!error)
	{
		if((0 != strcmp(tokens[0], "Array")) ||
			(0 != strcmp(tokens[1], "size:")) ||
			(0 != strcmp(tokens[3], "x")) ||
			(0 != strcmp(tokens[5], "logic")) ||
			(0 != strcmp(tokens[6], "blocks")))
		{
			error = 1;
		};
	}
	if(error)
	{
		printf("error1 :\t"
			"'%s' - Bad fpga size specification line in placement file\n",
			submoduleplacefile.c_str());
		exit(1);
	}

	map<string, map<float, set<string> > >::iterator ita;
	map<float, set<string> >::iterator itb;

	map<string, map<float, map<string, Finaliopos> > >::iterator itd;
	map<float, map<string, Finaliopos> >::iterator ite;

	//fbmao for supporting heterogeneous architecture.
	//map<ratio,  map<x, map<pair<y,z), blkname> >
	map<float, map<int, map<pair<int, int>, string> > >::iterator itf;
	map<int, map<pair<int, int>, string> >::iterator itg;

	//map<string, set<string> >  //filenameblkset;
	map<string, set<string> > ::iterator itj;

	tokens = ReadLineTokens(infile, &line);
	while(tokens)
	{
		//// Set clb coords
		//char subpostfix[80];
		////need to check whether it converts right
		//itoa(count,subpostfix, 10); 
		Finaliopos tmpiopos;

		string tempa = tokens[0];
		tmpiopos.x = atoi(tokens[1]);
		tmpiopos.y = atoi(tokens[2]);
		tmpiopos.z = atoi(tokens[3]);

		itj = filenameRamDspBlks.find(subfilename); //only store the RAM, DSP

		//filename, ratio, blkname    //in module.io file, memory has a postfix, but io doesn't have. deal with them different
		ita = filenameRatioBlkname.find(subfilename);
		if(ita != filenameRatioBlkname.end())   //if ita == filenameRatioBlkname.end()   means that the subfile only contains IO blocks
		{                                       //currently, each place file contains at least one clb
			itb = ita->second.find(submoduleratio);
			if(itb != ita->second.end())
			{
				set<string>& blknameset = itb->second;  //blknameset is a clb set
				set<string>::iterator itc = blknameset.find(tempa);

				bool isRamdspflag = 0;  //0: represent the block is ram or dsp. 1: represent it is IO block
				if(itj != filenameRamDspBlks.end())
				{
					set<string>& tmpramdspblks = itj->second;
					set<string>::iterator itm = tmpramdspblks.find(tempa);
					if(itm != tmpramdspblks.end())
					{
						isRamdspflag = 1;
					}
				}

				//it is io block
				if(itc == blknameset.end() && !isRamdspflag)  //not clb, and not dsp,ram.
				{
					//store io coordinate.
					//   map<string, map<float, map<string, Finaliopos> > > filenameRatioioBlknamecoord;
					itd = filenameRatioioBlknamecoord.find(subfilename);
					if(itd != filenameRatioioBlknamecoord.end())
					{
						map<float, map<string, Finaliopos> > &ratioioBlknamescoord = itd->second;
						ite = ratioioBlknamescoord.find(submoduleratio);
						if(ite != ratioioBlknamescoord.end())
						{
							map<string, Finaliopos> &ioblknamescoord = ite->second;
							ioblknamescoord.insert(make_pair(tempa, tmpiopos));
						}
						else
						{
							map<string, Finaliopos> tmpblknamecoord;
							tmpblknamecoord.insert(make_pair(tempa, tmpiopos));
							ratioioBlknamescoord.insert(make_pair(submoduleratio, tmpblknamecoord));
						}
					}
					else
					{
						map<string, Finaliopos> tmpblknamecoorda;
						map<float, map<string, Finaliopos> > tmpRatioblknamecoorda;
						tmpblknamecoorda.insert(make_pair(tempa, tmpiopos));
						tmpRatioblknamecoorda.insert(make_pair(submoduleratio, tmpblknamecoorda));
						filenameRatioioBlknamecoord.insert(make_pair(subfilename, tmpRatioblknamecoorda));
					}
				}
				else  //it is RAM, DSP or clb
				{
					itf = ratioxyzblkname.find(submoduleratio);
					if(itf != ratioxyzblkname.end()) //ratio exist
					{
						map<int, map<pair<int, int>, string> > &tmpxyzblkname = itf->second;
						itg = tmpxyzblkname.find(tmpiopos.x);
						if(itg != tmpxyzblkname.end()) //x exist
						{
							map<pair<int, int>, string> & tmpyzblkname = itg->second;
							//<y,z> unique
							tmpyzblkname.insert(make_pair(make_pair(tmpiopos.y, tmpiopos.z), tempa));
						}
						else  //new x
						{
							map<pair<int, int>, string> atmpyzblk;
							atmpyzblk.insert(make_pair(make_pair(tmpiopos.y, tmpiopos.z), tempa));
							tmpxyzblkname.insert(make_pair(tmpiopos.x, atmpyzblk));
						}
					}
					else 
					{
						map<int, map<pair<int, int>, string> > btmpxyzblk;
						map<pair<int, int>, string> btmpyzblk;
						btmpyzblk.insert(make_pair(make_pair(tmpiopos.y, tmpiopos.z), tempa));
						btmpxyzblk.insert(make_pair(tmpiopos.x, btmpyzblk));
						ratioxyzblkname.insert(make_pair(submoduleratio, btmpxyzblk));
					}
				}
			}
		}
		else
		{
			cout<<"Warning: the module only contains IO, pls check"<<endl;
			exit(1);
		}

		free(*tokens);
		free(tokens);
		tokens = ReadLineTokens(infile, &line);
	}

	fclose(infile);
}



void FPlan::moveIOblksToFinalplacefile(string finalplacefilename,
	                                   int &blkcount,
	                                   int &ioblkcount)
{
	FILE *ff = fopen(finalplacefilename.c_str(), "a+");
	if(!ff)
	{
		cout<<"cannot open the file "<<finalplacefilename<<endl;
		exit(1);
	}

	//for IO optimization   maybe used later for optimization
	//true: has occupied    false: donnot be used now
	vector<bool> left; //x = 0 col , y from 1 to ny row
	vector<bool> right; //x = nx+1, y from 1 to ny row
	vector<bool> lower; //x from 0 to nx,    y = 0
	vector<bool> top;    //x from 0 to nx,y = ny+1

	int flagindex = 1;
	int zflag = 0; //zflag [0..7]   now ch_intrinsic Io capacity is 7 should get from arch file
	bool leftflag = false;  // to record whether it  order left, right, low, top to place IO
	bool rightflag = false;
	bool lowerflag = false;
	bool topflag = false;
	int x, y, z;

	for(set<string>::iterator ioit = finaliosets.begin(); ioit != finaliosets.end(); ioit++)
	{
		//the code below should changed later
		// || 0 == strcmp(tempa.c_str(), "top.single_port_ram+str^out~6")) //the block is IO block, donnot need to move IO (need to check)
		//only io in the netlist should need to store in the place file
		//only keep one topclock

		//true: has occupied    false: donnot be used now
		//vector<bool> left; //x = 0 col , y from 1 to ny row
		//vector<bool> right; //x = nx+1, y from 1 to ny row
		//vector<bool> lower; //x from 1 to nx,    y = 0
		//vector<bool> top;    //x from 1 to nx,y = ny+1

		ioblkcount++;  //to check whether the final io number match or not
		//IO x, y, z should change
		//choose a legal position for the IO
		if(false == leftflag)
		{
			if(zflag > 7)
				zflag = 0;

			x = 0;
			y = flagindex; //y begin from 1
			z = zflag;
			zflag++;
			if(zflag > 7)  //7 should get from the architecture file
			{
				flagindex++;
			}

			if(flagindex > (int)Height)
			{
				leftflag = true;
				flagindex = 1;
			}
		}
		else if(false == rightflag)
		{
			if(zflag > 7)
				zflag = 0;

			x = (int)Width + 1;
			y = flagindex;
			z = zflag;
			zflag++;
			if(zflag > 7)
				flagindex++;

			if(flagindex > (int)Height)
			{
				rightflag = true;
				flagindex = 1;
			}
		}
		else if(false == lowerflag)
		{
			if(zflag > 7)
				zflag = 0;

			x = flagindex;
			y = 0;
			z = zflag;
			zflag++;

			if(zflag > 7)
				flagindex++;

			if(flagindex > (int)Width)
			{
				lowerflag = true;
				flagindex = 1;
			}
		}
		else if(false == topflag)
		{
			if(zflag > 7)
				zflag = 0;

			x = flagindex;
			y = (int)Height + 1;
			z = zflag;
			zflag++;

			if(zflag > 7)
				flagindex++;

			if(flagindex > (int)Width)
			{
				topflag = true;
				flagindex = 1;
			}
		}
		else 
		{ 
			cout<<"Error:  IO positions are not enough, Pls check with it"<<endl;
			exit(1);
		}

		//string tempblkname = tempa + "_" + subpostfix;
		string tempblkname = *ioit;  //keep the original name
		fprintf(ff, "%s\t", tempblkname.c_str());
		if(strlen(tempblkname.c_str()) < 8)
			fprintf(ff, "\t");
		fprintf(ff, "%d\t%d\t%d", x, y, z);
		fprintf(ff, "\t#%d\n", blkcount);
		blkcount++;
	}
	fclose(ff);
}



void FPlan::moveRandomIOblksToFinalPlacefile(string finalplacefilename,
	int &blkcount, int &ioblkcount)
{
	FILE *ff = fopen(finalplacefilename.c_str(), "a+");
	if(!ff)
	{
		cout<<"cannot open the file "<<finalplacefilename<<endl;
		exit(1);
	}

	//add for loadable flow, for getting the whole place file
	////////string ioforfinalplacefile = filename + "/placefiles/" + filename + ".ioplace";
	////////FILE *ioforfinalplace = fopen(ioforfinalplacefile.c_str(), "a+");
	////////if(!ioforfinalplace)
 ////////   {
	////////	 cout<<"cannot open the file "<<ioforfinalplacefile.c_str()<<endl;
	////////	 exit(1);
	//////// }

	int i = 0;
	for(set<string>::iterator ioit = finaliosets.begin(); ioit != finaliosets.end(); ioit++, i++)
	{
		string iosname;
    	int x = -1, y = -1, z = -1; //io position
		ioblkcount++;  //to check whether the final io number match or not
		//IO x, y, z should change
		//choose a legal position for the IO
		iosname = *ioit;	
		x = finaliocoordinates[i].x;
		y = finaliocoordinates[i].y;
		z = finaliocoordinates[i].z;

		if(x == -1 || y == -1 || z == -1)
		{
			cout<<"Error: It is a wrong coordinate for the bock "<<iosname.c_str()<<" ("<<x<<","<<y<<","<<z<<"), pls check it!"<<endl;
			exit(1);
		}

		fprintf(ff, "%s\t", iosname.c_str());  //io, keep the original name
		if(strlen(iosname.c_str()) < 8)
			fprintf(ff, "\t");
		fprintf(ff, "%d\t%d\t%d", x, y, z);
		fprintf(ff, "\t#%d\n", blkcount);


		//////add for loadable flow
		////fprintf(ioforfinalplace, "%s\t", iosname.c_str());  //io, keep the original name
		////if(strlen(iosname.c_str()) < 8)
		////	fprintf(ioforfinalplace, "\t");
		////fprintf(ioforfinalplace, "%d\t%d\t%d", x, y, z);
		////fprintf(ioforfinalplace, "\t#%d\n", blkcount);

		blkcount++;
	}
	fclose(ff);
	////fclose(ioforfinalplace);
}


void FPlan::moveIOblksToFinalplacefileConsiderOrigiposition(string finalplacefilename,
	int &blkcount, int &ioblkcount)
{
	//Height, Width are used for getioresource() function
	getioresource();  //after assigned, remove the available pos from the map
	FILE *ff = fopen(finalplacefilename.c_str(), "a+");
	if(!ff)
	{
		cout<<"cannot open the file "<<finalplacefilename<<endl;
		exit(1);
	}

	//add for loadable flow, for getting the whole place file
	string ioforfinalplacefile = filename + "/placefiles/" + filename + ".ioplace";
	FILE *ioforfinalplace = fopen(ioforfinalplacefile.c_str(), "a+");
	if(!ioforfinalplace)
    {
		 cout<<"cannot open the file "<<ioforfinalplacefile.c_str()<<endl;
		 exit(1);
	 }

	map<string, vector<Finaliopos> >::iterator ioresourceit;
	for(set<string>::iterator ioit = finaliosets.begin(); ioit != finaliosets.end(); ioit++)
	{
		//for choose IO position
		// map<string, struct IOposition> ionameIopos;
		//collect IO resource
		//left1, left2, top1, top2, right2, right1, bottom2, bottom1 --left1
		//  map<string, vector<Finaliopos> > ioresource;
		int flag = 0; //reset flag value used for check whether there is a available pos for the IO
		string iosname;
    	int x = -1, y = -1, z = -1; //io position
		int id;
		ioblkcount++;  //to check whether the final io number match or not
		//IO x, y, z should change
		//choose a legal position for the IO
		iosname = *ioit;
	

		//assign the position from the resoure to io blk
		map<string, struct IOposition>::iterator tmpioposit = ionameIopos.find(iosname);
		if(tmpioposit != ionameIopos.end())
		{
			//first get the side of this io blk
			string sidenameofio;
			struct IOposition& tmpiopos = tmpioposit->second;
			sidenameofio = "left1";
			int tempvalue = tmpiopos.left1;

			//compare the value and check this port belongs to which side seems to be better
			if(tempvalue < tmpiopos.left2)
			{
				sidenameofio = "left2";
				tempvalue = tmpiopos.left2;
			}
			if(tempvalue < tmpiopos.top2)
			{
				sidenameofio = "top2";
				tempvalue = tmpiopos.top2;
			}
			if(tempvalue < tmpiopos.top1)
			{
				sidenameofio = "top1";
				tempvalue = tmpiopos.top1;
			}
			if(tempvalue < tmpiopos.right2)
			{
				sidenameofio = "right2";
				tempvalue = tmpiopos.right2;
			}			
			if(tempvalue < tmpiopos.right1)
			{
				sidenameofio = "right1";
				tempvalue = tmpiopos.right1;
			}			
			if(tempvalue < tmpiopos.bottom2)
			{
				sidenameofio = "bottom2";
				tempvalue = tmpiopos.bottom2;
			}			
			if(tempvalue < tmpiopos.bottom1)
			{
				sidenameofio = "bottom1";
				tempvalue = tmpiopos.bottom1;
			}

			ioresourceit = ioresource.find(sidenameofio);
			if(ioresourceit != ioresource.end())
			{
				vector<Finaliopos> & tmpfinaliopos = ioresourceit->second;
				if(0 == (int)tmpfinaliopos.size())
				{
					flag = getIoPos(x, y, z, "left1");
					if(0 == flag)
					{
						flag = getIoPos(x, y, z, "left2");
						if(0 == flag)
						{
							flag = getIoPos(x, y, z, "top1");
							if(0 == flag)
							{
								flag = getIoPos(x, y, z, "top2");
								if(0 == flag)
								{
									flag = getIoPos(x, y, z, "right2");
									if(0 == flag)
									{
										flag = getIoPos(x, y, z, "right1");
										if(0 == flag)
										{
											flag = getIoPos(x, y, z, "bottom2");
											if(0 == flag)
											{
												flag = getIoPos(x, y, z, "bottom1");
												if(0 == flag)
												{
													cout<<"Error1: can't find name in map ioresource! (too many IOs)"<<endl;
													exit(1);
												}
											}
										}
									}
								}
							}
						}
					}
				}else if(1 == (int)tmpfinaliopos.size())
				{
					x = tmpfinaliopos[0].x;
					y = tmpfinaliopos[0].y;
					z = tmpfinaliopos[0].z;
					tmpfinaliopos.erase(tmpfinaliopos.begin());
					flag = 1;
				}
				else
				{
					id = rand() % ((int)tmpfinaliopos.size());
					x = tmpfinaliopos[id].x;
					y = tmpfinaliopos[id].y;
					z = tmpfinaliopos[id].z;
					tmpfinaliopos.erase(tmpfinaliopos.begin()+id);
					flag = 1;
				}
			}
			else
			{
				cout<<"Error: The side name "<<sidenameofio<<" is not correct"<<endl;
				exit(1);
			}
		} //io not in the original place and then random choose a position
		else{
			flag = getIoPos(x, y, z, "left1");
			if(0 == flag)
			{
				flag = getIoPos(x, y, z, "left2");
				if(0 == flag)
				{
					flag = getIoPos(x, y, z, "top1");
					if(0 == flag)
					{
						flag = getIoPos(x, y, z, "top2");
						if(0 == flag)
						{
							flag = getIoPos(x, y, z, "right2");
							if(0 == flag)
							{
								flag = getIoPos(x, y, z, "right1");
								if(0 == flag)
								{
									flag = getIoPos(x, y, z, "bottom2");
									if(0 == flag)
									{
										flag = getIoPos(x, y, z, "bottom1");
										if(0 == flag)
										{
											cout<<"Error2: can't find name in map ioresource! (maybe too many IOs)"<<endl;
											exit(1);
										}
									}
								}
							}
						}
					}
				}
			}
		}

		assert(1 == flag);
		if(x == -1 || y == -1 || z == -1)
		{
			cout<<"Error: It is a wrong coordinate for the bock "<<iosname.c_str()<<" ("<<x<<","<<y<<","<<z<<"), pls check it!"<<endl;
			exit(1);
		}

		fprintf(ff, "%s\t", iosname.c_str());  //io, keep the original name
		if(strlen(iosname.c_str()) < 8)
			fprintf(ff, "\t");
		fprintf(ff, "%d\t%d\t%d", x, y, z);
		fprintf(ff, "\t#%d\n", blkcount);

	   //add for loadable flow
		fprintf(ioforfinalplace, "%s\t", iosname.c_str());  //io, keep the original name
		if(strlen(iosname.c_str()) < 8)
			fprintf(ioforfinalplace, "\t");
		fprintf(ioforfinalplace, "%d\t%d\t%d", x, y, z);
		fprintf(ioforfinalplace, "\t#%d\n", blkcount);


		blkcount++;
	}
	fclose(ff);
	fclose(ioforfinalplace);
}



void FPlan::getInitialIOblksposition()
{    //divide the chip boundary into eight part and move io of each module to the nearest position(place) 
	finaliocoordinates.clear();
    finaliocoordinates.resize((int)(finaliosets.size()));
	getioresource();  //after assigned, remove the available pos from the map

	map<string, vector<Finaliopos> >::iterator ioresourceit;
	int i = 0;
	for(set<string>::iterator ioit = finaliosets.begin(); ioit != finaliosets.end(); ioit++, i++)
	{
		//for choose IO position
		// map<string, struct IOposition> ionameIopos;
		//collect IO resource
		//left1, left2, top1, top2, right2, right1, bottom2, bottom1 --left1
		//  map<string, vector<Finaliopos> > ioresource;
		int flag = 0; //reset flag value used for check whether there is a available pos for the IO
		string iosname;
    	int x = -1, y = -1, z = -1; //io position
		int id;
//		ioblkcount++;  //to check whether the final io number match or not
		//IO x, y, z should change
		//choose a legal position for the IO
		iosname = *ioit;
		
		//if(0 == strcmp("out:top^d_writedatamem~25", iosname.c_str()))   //top^tm3_sram_data_in~0  top^rgCont~0  0 == strcmp("out:top^tm3_sram_oe~0", iosname.c_str()) || 0 == strcmp("top^texinfo~0", iosname.c_str()))
		//{
		//	int tmpk = 0;
		//}


		//assign the position from the resoure to io blk
		map<string, struct IOposition>::iterator tmpioposit = ionameIopos.find(iosname);
		if(tmpioposit != ionameIopos.end())
		{
			//first get the side of this io blk
			string sidenameofio;
			struct IOposition& tmpiopos = tmpioposit->second;
			sidenameofio = "left1";
			int tempvalue = tmpiopos.left1;

			//compare the value
			if(tempvalue < tmpiopos.left2)
			{
				sidenameofio = "left2";
				tempvalue = tmpiopos.left2;
			}
			if(tempvalue < tmpiopos.top2)
			{
				sidenameofio = "top2";
				tempvalue = tmpiopos.top2;
			}
			if(tempvalue < tmpiopos.top1)
			{
				sidenameofio = "top1";
				tempvalue = tmpiopos.top1;
			}
			if(tempvalue < tmpiopos.right2)
			{
				sidenameofio = "right2";
				tempvalue = tmpiopos.right2;
			}			
			if(tempvalue < tmpiopos.right1)
			{
				sidenameofio = "right1";
				tempvalue = tmpiopos.right1;
			}			
			if(tempvalue < tmpiopos.bottom2)
			{
				sidenameofio = "bottom2";
				tempvalue = tmpiopos.bottom2;
			}			
			if(tempvalue < tmpiopos.bottom1)
			{
				sidenameofio = "bottom1";
				tempvalue = tmpiopos.bottom1;
			}

			ioresourceit = ioresource.find(sidenameofio);
			if(ioresourceit != ioresource.end())
			{
				vector<Finaliopos> & tmpfinaliopos = ioresourceit->second;
				if(0 == (int)tmpfinaliopos.size())
				{
					flag = getIoPos(x, y, z, "left1");
					if(0 == flag)
					{
						flag = getIoPos(x, y, z, "left2");
						if(0 == flag)
						{
							flag = getIoPos(x, y, z, "top1");
							if(0 == flag)
							{
								flag = getIoPos(x, y, z, "top2");
								if(0 == flag)
								{
									flag = getIoPos(x, y, z, "right2");
									if(0 == flag)
									{
										flag = getIoPos(x, y, z, "right1");
										if(0 == flag)
										{
											flag = getIoPos(x, y, z, "bottom2");
											if(0 == flag)
											{
												flag = getIoPos(x, y, z, "bottom1");
												if(0 == flag)
												{
													cout<<"Error1: can't find name in map ioresource! (too many IOs)"<<endl;
													exit(1);
												}
											}
										}
									}
								}
							}
						}
					}
				}else if(1 == (int)tmpfinaliopos.size())
				{
					x = tmpfinaliopos[0].x;
					y = tmpfinaliopos[0].y;
					z = tmpfinaliopos[0].z;
					tmpfinaliopos.erase(tmpfinaliopos.begin());
					flag = 1;
				}
				else
				{
					id = rand() % ((int)tmpfinaliopos.size());
					x = tmpfinaliopos[id].x;
					y = tmpfinaliopos[id].y;
					z = tmpfinaliopos[id].z;
					tmpfinaliopos.erase(tmpfinaliopos.begin()+id);
					flag = 1;
				}
			}
			else
			{
				cout<<"Error: The side name "<<sidenameofio<<" is not correct"<<endl;
				exit(1);
			}
		} //io not in the original place and then random choose a position
		else{
			flag = getIoPos(x, y, z, "left1");
			if(0 == flag)
			{
				flag = getIoPos(x, y, z, "left2");
				if(0 == flag)
				{
					flag = getIoPos(x, y, z, "top1");
					if(0 == flag)
					{
						flag = getIoPos(x, y, z, "top2");
						if(0 == flag)
						{
							flag = getIoPos(x, y, z, "right2");
							if(0 == flag)
							{
								flag = getIoPos(x, y, z, "right1");
								if(0 == flag)
								{
									flag = getIoPos(x, y, z, "bottom2");
									if(0 == flag)
									{
										flag = getIoPos(x, y, z, "bottom1");
										if(0 == flag)
										{
											cout<<"Error2: can't find name in map ioresource! (maybe too many IOs)"<<endl;
											exit(1);
										}
									}
								}
							}
						}
					}
				}
			}
		}

		assert(1 == flag);
		if(x == -1 || y == -1 || z == -1)
		{
			cout<<"Error: It is a wrong coordinate for the bock "<<iosname.c_str()<<" ("<<x<<","<<y<<","<<z<<"), pls check it!"<<endl;
			exit(1);
		}

		finaliocoordinates[i].x = x;
		finaliocoordinates[i].y = y;
		finaliocoordinates[i].z = z;
	}
	assert((int)(finaliosets.size()) == i);  //just for check the io number should be same as the io number
}


int FPlan::getIoPos(int &x, int &y, int &z, string sidename)
{   //0 : wrong data    1: right data
	int id;
	map<string, vector<Finaliopos> >::iterator ioresourceit;
	ioresourceit = ioresource.find(sidename);
	if(ioresourceit != ioresource.end() && 0 != (int)ioresourceit->second.size())
	{
		vector<Finaliopos> & tmpfinaliopos = ioresourceit->second;
		if(1 == (int)tmpfinaliopos.size())
		{
			x = tmpfinaliopos[0].x;
			y = tmpfinaliopos[0].y;
			z = tmpfinaliopos[0].z;
			tmpfinaliopos.erase(tmpfinaliopos.begin());
		}
		else
		{
			id = rand() % ((int)tmpfinaliopos.size());
			x = tmpfinaliopos[id].x;
			y = tmpfinaliopos[id].y;
			z = tmpfinaliopos[id].z;
			tmpfinaliopos.erase(tmpfinaliopos.begin()+id);
		}
		return 1;
	}
	else
	{
		return 0;
	}
}


void FPlan::randomFinalIoblks()
{
	getInitialIOblksposition();

	vector<bool>usedflag; 
	vector<int>recordIoblkid;
	usedflag.resize((int)(allIoresource.size()), false);  //default: false

	//default: -1  means not be used, record the finalio's index
	recordIoblkid.resize((int)(allIoresource.size()), -1);  

	for(int i = 0; i < (int)(finaliocoordinates.size()); i++)
	{
		Finaliopos tmpiopos = finaliocoordinates[i];
		for(int j = 0; j < int(allIoresource.size()); j++)
		{
			Finaliopos &tmpioposb = allIoresource[j];
			if(tmpiopos.x == tmpioposb.x &&
			   tmpiopos.y == tmpioposb.y &&
			   tmpiopos.z == tmpioposb.z)
			{
				recordIoblkid[j] = i;
				usedflag[j] = true;  //true means there is a block,use this position
			}
		}
	}


	//get finalioblks     finalioblk
	ionameindex.clear();   //ionameindex will used in calculating the wirelength
	int index = 0;
	for(set<string>::iterator ioit = finaliosets.begin(); ioit != finaliosets.end(); ioit++, index++)
	{
		string iosname;
		//choose a legal position for the IO
		iosname = *ioit;	
		ionameindex.insert(make_pair(iosname, index));
	}


	int minwirelength = calcWireLengthConsidermoduleioposAndFinaliopos();

	int itertimes = 0;
	while(itertimes < 20) //50) //1000)
	{
		bool accept = true; //default: true
	    //then random choose two and swap between them
	    int rd1 = rand() % int(allIoresource.size());
	    int rd2 = rand() % int(allIoresource.size());
	    if(!(usedflag[rd1] == false &&
			usedflag[rd2] == false))
		{
	        int indexa = -1, indexb = -1; 
			itertimes++;
			if(usedflag[rd1] == true &&
			   usedflag[rd2] == true)
			{
	             indexa = recordIoblkid[rd1];  //get the finalIO blk's index  whiich in the finalio
				 indexb = recordIoblkid[rd2]; //get the finalio blk's index
				 Finaliopos tmpiopos = finaliocoordinates[indexa];
				 finaliocoordinates[indexa] = finaliocoordinates[indexb];
				 finaliocoordinates[indexb] = tmpiopos;
			
				 recordIoblkid[rd1] = indexb;
                 recordIoblkid[rd2] = indexa;

				 int wirelengtha = calcWireLengthConsidermoduleioposAndFinaliopos();
				 if(minwirelength < wirelengtha)
				       accept = false;

				 if(false == accept)
				 { //rollback
				    recordIoblkid[rd1] = indexa;
                    recordIoblkid[rd2] = indexb;
			        tmpiopos = finaliocoordinates[indexa];
				    finaliocoordinates[indexa] = finaliocoordinates[indexb];
				    finaliocoordinates[indexb] = tmpiopos;
				 }
			}else if(usedflag[rd1] == true && usedflag[rd2] == false)
			{
				indexa = recordIoblkid[rd1];

				 recordIoblkid[rd1] = -1; //-1 means it isn't used
                 recordIoblkid[rd2] = indexa;
				 finaliocoordinates[indexa] = allIoresource[rd2];
				 usedflag[rd1] = false;
				 usedflag[rd2] = true;

				 int wirelengthb = calcWireLengthConsidermoduleioposAndFinaliopos();
				 if(minwirelength < wirelengthb)
				       accept = false;

				 if(false == accept)
				 { //rollback
				    usedflag[rd1] = true;
				    usedflag[rd2] = false;
				    recordIoblkid[rd1] = indexa;
                    recordIoblkid[rd2] = -1;
				    finaliocoordinates[indexa] = allIoresource[rd1];				   
				 }
			}else  //rd1 false    rd2 true
			{
				indexb = recordIoblkid[rd2];

				 recordIoblkid[rd2] = -1; //-1 means it isn't used
                 recordIoblkid[rd1] = indexb;
				 finaliocoordinates[indexb] = allIoresource[rd1];
				 usedflag[rd1] = true;
				 usedflag[rd2] = false;

				 int wirelengthc = calcWireLengthConsidermoduleioposAndFinaliopos();
				 if(minwirelength < wirelengthc)
				       accept = false;

				 if(false == accept)
				 { //rollback
				    usedflag[rd1] = false;
				    usedflag[rd2] = true;
				    recordIoblkid[rd1] = -1;
                    recordIoblkid[rd2] = indexb;
				    finaliocoordinates[indexb] = allIoresource[rd2];	   
				 }
			}
		}
	}

}



void FPlan::getioresource()
{
	//the side of the chip is in the order below
	//left1, left2, top1, top2, right2, right1, bottom2, bottom1

	int iocap = 8; //7; //later will pass a parameter to here;   io capacity
	ioresource.clear();

	int col = 0;
	int row = 0;   
	string sidename;
	int k;
	vector<Finaliopos> templeft1iopos;
	vector<Finaliopos> templeft2iopos;
	vector<Finaliopos> tempright1iopos;
	vector<Finaliopos> tempright2iopos;
	vector<Finaliopos> temptop1iopos;
	vector<Finaliopos> temptop2iopos;
	vector<Finaliopos> tempbottom1iopos;
	vector<Finaliopos> tempbottom2iopos;
	templeft1iopos.clear();
	templeft2iopos.clear();
	tempright1iopos.clear();
	tempright2iopos.clear();
	temptop1iopos.clear();
	temptop2iopos.clear();
	tempbottom1iopos.clear();
	tempbottom2iopos.clear();

	allIoresource.clear();

	//1.left1
	sidename = "left1";
	for(row = 1; row <= (int)Height; row++)
	{  	
		if(row <= (int)(Height/2))
		{
			for(k = 0; k < iocap; k++)
			{
				struct Finaliopos tempos;
				tempos.x = 0;
				tempos.y = row;
				tempos.z = k;
				templeft1iopos.push_back(tempos);
				allIoresource.push_back(tempos);
			}
		}
	}
	ioresource.insert(make_pair(sidename, templeft1iopos));
	

	//2. left2
	sidename = "left2";
	for(row = 1; row <= (int)Height; row++)
	{
		if(row > (int)(Height/2))
		{
			for(k = 0; k < iocap; k++)
			{
			    struct Finaliopos tempos;
				tempos.x = 0;
				tempos.y = row;
				tempos.z = k;
				templeft2iopos.push_back(tempos);
				allIoresource.push_back(tempos);
			}
		}
	}
	ioresource.insert(make_pair(sidename, templeft2iopos));

	//3. right1
    sidename = "right1";
	for(row = 1; row <= (int)Height; row++)
	{
		if(row <= (int)(Height/2))
		{
			for(k = 0; k < iocap; k++)
			{
				struct Finaliopos tempos;
				tempos.x = int(Width) + 1;
				tempos.y = row;
				tempos.z = k;
				tempright1iopos.push_back(tempos);
				allIoresource.push_back(tempos);
			}
		}
	}
	ioresource.insert(make_pair(sidename, tempright1iopos));

	//right2
	sidename = "right2";
	for(row = 1; row <= (int)Height; row++)
	{
		if(row > (int)(Height/2))
		{
			for(k = 0; k < iocap; k++)
			{
				struct Finaliopos tempos;
				tempos.x = int(Width) + 1;
				tempos.y = row;
				tempos.z = k;
				tempright2iopos.push_back(tempos);
				allIoresource.push_back(tempos);
			}
		}
	}
	ioresource.insert(make_pair(sidename, tempright2iopos));

	//top1
	sidename = "top1";
	for(col = 1; col <= (int)Width; col++)
	{  
		if(col <= (int)(Width/2))
		{
			for(k = 0; k < iocap; k++)
			{
				struct Finaliopos tempos;
				tempos.x = col;
				tempos.y = (int)Height + 1;
				tempos.z = k;
				temptop1iopos.push_back(tempos);
				allIoresource.push_back(tempos);
			}
		}
	}
	ioresource.insert(make_pair(sidename, temptop1iopos));

	//top2
	sidename = "top2";
	for(col = 1; col <= (int)Width; col++)
	{
		if(col > (int)(Width/2))
		{
			for(k = 0; k < iocap; k++)
			{
				struct Finaliopos tempos;
				tempos.x = col;
				tempos.y = (int)Height + 1;
				tempos.z = k;
				temptop2iopos.push_back(tempos);
				allIoresource.push_back(tempos);
			}
		}
	}
	ioresource.insert(make_pair(sidename, temptop2iopos));

	//bottom1
	sidename = "bottom1";
	for(col = 1; col <= (int)Width; col++)
	{
		if(col <= (int)(Width/2))
		{
			for(k = 0; k < iocap; k++)
			{
				struct Finaliopos tempos;
				tempos.x = col;
				tempos.y = 0;
				tempos.z = k;
				tempbottom1iopos.push_back(tempos);
				allIoresource.push_back(tempos);
			}
		}
	}
	ioresource.insert(make_pair(sidename, tempbottom1iopos));

	sidename = "bottom2";
	for(col = 1; col <= (int)Width; col++)
	{
		if(col > (int)(Width/2))
		{
			for(k = 0; k < iocap; k++)
			{
			    struct Finaliopos tempos;
				tempos.x = col;
				tempos.y = 0;
				tempos.z = k;
				tempbottom2iopos.push_back(tempos);
				allIoresource.push_back(tempos);
			}
		}
	}
	ioresource.insert(make_pair(sidename, tempbottom2iopos));
}


void FPlan::traceSoftModuleConnections()
{    
	//collect mod.connectModuleIds
	//ReadLineToken doesnot consider the comment line and blank line(a line is empty)
	//bgm.trace.txt
	//first map modulename to moduleId
	map<string, int> nameId;
	nameId.clear();
	for(int i = 0; i < modules_N; i++)
	{
		nameId.insert(make_pair(modules[i].name, modules[i].id));
		modules[modules[i].id].connectModuleIds.clear();  //store the connected module's id 
		modules[modules[i].id].inputports.clear();
		modules[modules[i].id].outputports.clear();
	}
    //collect clk, inputport,outputport for soft modules
	string tracefilename = filename + "/" + filename + ".trace.txt";
	FILE *infile;
	char **tokens;
	int line;
	infile = fopen(tracefilename.c_str(), "r");
	if(!infile)
	{
		cout<<"error: cannot open the trace file" <<tracefilename<<endl;
		exit(1);
	}

	if(feof(infile))
	{
		cout<<"the trace file donnot contains enough submodules"<<endl;
		exit(1);
	}

	//begin to read submodule
	tokens = ReadLineTokens(infile, &line);
	for(int i = 0; !feof(infile) && NULL != tokens; i++)
	{
		string submodulename;
	    if(NULL == tokens)
	    {
		   cout<<"error1: the file is empty, pls check"<<endl;
	    }
	    submodulename = tokens[0];
	    int id  = nameId[submodulename];
	    Module &tempmod = modules[id];
	    tokens = ReadLineTokens(infile, &line);
	    if(NULL == tokens)
	    {
		   cout<<"error2: the file is empty, pls check"<<endl;
	    }
	    int j = 0;
		while(tokens[j])
		{
		    tempmod.inputports.insert(tokens[j]);
			j++;
		}
	    tokens = ReadLineTokens(infile, &line);
	    if(NULL == tokens)
	    {
		   cout<<"error3: the file is empty, pls check"<<endl;
	    }
	    j = 0;
		while(tokens[j])
		{
			tempmod.outputports.insert(tokens[j]);
			j++;
		}

	    /* Get next line */
	    assert(*tokens);
	    free(*tokens);
	    free(tokens);
		tokens = ReadLineTokens(infile, &line);
	}
	fclose(infile);

	//2. then create connections among modules
	set<string>::iterator it;
	for(int i = 0; i < modules_N; i++)
	{
		Module& mod = modules[i];
		for(set<string>::iterator ita = mod.outputports.begin(); 
			ita != mod.outputports.end(); 
			ita++)
		{
			string tmpname = (*ita);
			vector<int> connectIds;
			for(int k = 0; k < modules_N; k++)
			{
				it = modules[k].inputports.find(tmpname);
				if(it != modules[k].inputports.end())
					connectIds.push_back(k);
			}
			if(connectIds.size() != 0)
				mod.connectModuleIds.push_back(connectIds);
		}
	}

//	creatNetworkOfModules();
}


void FPlan::traceSoftModuleConnectionsKeepportname()
{    
	//collect portconnectIds (//vector<map<portname, vector<int> > >)
	//ReadLineToken doesnot consider the comment line and blank line(a line is empty)
	//bgm.trace.txt
	//first map modulename to moduleId
	map<string, int> nameId;
	nameId.clear();
	for(int i = 0; i < modules_N; i++)
	{
		nameId.insert(make_pair(modules[i].name, modules[i].id));
		//vector<map<portname, vector<int> > >
		 //vector<map<string,vector<int> > > portconnectModuleIds; //used for calculate wirelenth bundingbox
		modules[modules[i].id].portconnectModuleIds.clear(); ////store the connected module's id
		modules[modules[i].id].inputports.clear();
		modules[modules[i].id].outputports.clear();
	}
    //collect clk, inputport,outputport for soft modules
	string tracefilename = filename + "/" + filename + ".trace.txt";
	FILE *infile;
	char **tokens;
	int line;
	infile = fopen(tracefilename.c_str(), "r");
	if(!infile)
	{
		cout<<"error: cannot open the trace file" <<tracefilename<<endl;
		exit(1);
	}


	if(feof(infile))
	{
		cout<<"the trace file donnot contains enough submodules"<<endl;
		exit(1);
	}

	//begin to read submodule
	tokens = ReadLineTokens(infile, &line);
	for(int i = 0; !feof(infile) && NULL != tokens; i++)
	{
		string submodulename;
	    if(NULL == tokens)
	    {
		   cout<<"error1: the file is empty, pls check"<<endl;
	    }
	    submodulename = tokens[0];
	    int id  = nameId[submodulename];
	    Module &tempmod = modules[id];
	    tokens = ReadLineTokens(infile, &line);
	    if(NULL == tokens)
	    {
		   cout<<"error2: the file is empty, pls check"<<endl;
	    }
	    int j = 0;
		while(tokens[j])
		{
		    tempmod.inputports.insert(tokens[j]);
			j++;
		}
	    tokens = ReadLineTokens(infile, &line);
	    if(NULL == tokens)
	    {
		   cout<<"error3: the file is empty, pls check"<<endl;
	    }
	    j = 0;
		while(tokens[j])
		{
			tempmod.outputports.insert(tokens[j]);
			j++;
		}

	    /* Get next line */
	    assert(*tokens);
	    free(*tokens);
	    free(tokens);
		tokens = ReadLineTokens(infile, &line);
	}
	fclose(infile);

	//2. then create connections among modules
	set<string>::iterator it;
	for(int i = 0; i < modules_N; i++)
	{
		Module& mod = modules[i];
		for(set<string>::iterator ita = mod.outputports.begin(); 
			ita != mod.outputports.end(); 
			ita++)
		{
			string tmpname = (*ita);
			vector<int> connectIds;
			map<string, vector<int> > portconnectIds;
			connectIds.clear();
			portconnectIds.clear();

			for(int k = 0; k < modules_N; k++)
			{
				it = modules[k].inputports.find(tmpname);
				if(it != modules[k].inputports.end())
					connectIds.push_back(k);
			}
			if(connectIds.size() != 0)
			{
				portconnectIds.insert(make_pair(tmpname, connectIds));
			    mod.portconnectModuleIds.push_back(portconnectIds);
			}
		}
	}

//	creatNetworkOfModules();
}



void FPlan::creatNetworkOfModules()
{
		// vector<vector<int> > connectModuleIds;
	netsets.clear();
	numbersets.clear();
	//3  create net sets and numberset to store the number
	for(int i = 0; i < modules_N; i++)
	{
		Module &mod = modules[i];
		if(mod.connectModuleIds.size() >0)  //it is a net
		{
			for(int j = 0; j < mod.connectModuleIds.size(); j++)
			{
				//need to verify whether two ids are the same
				vector<int> tmpconnectId = mod.connectModuleIds[j];
				vector<struct virtualPin> virtualNet;
					struct virtualPin vrpina;
					vrpina.mod = mod.id;
					vrpina.net = netsets.size();  //net id 
					vrpina.x = modules_info[mod.id].x;
					vrpina.y = modules_info[mod.id].y;
					vrpina.ax = (int)((modules_info[mod.id].x + modules_info[mod.id].rx)/2.0);
					vrpina.ay = (int)((modules_info[mod.id].y + modules_info[mod.id].ry)/2.0);
					virtualNet.push_back(vrpina);
				for(int k = 0; k < tmpconnectId.size(); k++)
				{
					int id = tmpconnectId[k];
					struct virtualPin vrpin;
					vrpin.mod = mod.id;
					vrpin.net = netsets.size();
					vrpin.x = modules_info[id].x;
					vrpin.y = modules_info[id].y;
					vrpin.ax = (int)((modules_info[id].x + modules_info[id].rx)/2.0);
					vrpin.ay = (int)((modules_info[id].y + modules_info[id].ry)/2.0);
					virtualNet.push_back(vrpin);
				}

				netsets.push_back(virtualNet); //netsets contains all the nets
				if(maxNumTerminal < virtualNet.size())
				{
					maxNumTerminal = virtualNet.size();
				}
				numbersets.push_back(virtualNet.size()); //numberset contains the number of terminals of each nets
			}
		}
	}
}



//for each submodule. find the corresponding coordinates for each block inside the modules.
//	insert into floorplanXtofpgaX
//	when read the clbfile. record all the coordinates??  
//	(different ratio, different coordinates. it is better to get the final results. cost much for storing all the blk's coordinates??  check later.)

void FPlan::placeMemoryAndMultiply()
{
	////read from arch file
	////memory
	//fmemstart = 7;
	//fmemrepeat = 10;
	//fmemoryheight = 2;

	////multiply
 //   fmultistart = 6;
	//fmultirepeat = 8;
	//fmultiheight = 4;

// map<name, coordinate> memoryMultiplyCoords;
	fmemorynum = memoryset.size();
	fmultiplynum = multiplyset.size();

	//bug!!!?? arch has memory, so that column is used for memory, clb can't place there??
//	int memoryextracol = memorynum / (Height/memoryheight);
//	int multiplyextracol = multiplynum / (Height/ multiheight);
	 int memoryextracol = 0, multiplyextracol = 0;
	 if(isheteroarch && Width >= fmemstart)
	 {
		 memoryextracol = (Width - fmemstart) / (fmemrepeat - 1) + 1;
	 }

	//need to modify the width..it need to place memory
	Width = Width + memoryextracol + multiplyextracol;  
   //need to check width that whether there are feasible position for place memory and multiply
	//because they are not adjacent. eg. two column has the gap

	//first collect memory and multiply resource,
	//second random choose position for place them
	vector<Finaliopos> memorypos;
	vector<Finaliopos> multiplypos;
	memorypos.clear();
	multiplypos.clear();
	//collect memory resource  position
	for(int tmpmemstart = fmemstart; tmpmemstart <= Width; tmpmemstart = tmpmemstart + fmemrepeat)
	{
		for(int ycoord = 1; (ycoord + fmemoryheight - 1) <= Height; ycoord = ycoord + fmemoryheight)
		{
			Finaliopos tmpblkpos;
			tmpblkpos.x = tmpmemstart;
			tmpblkpos.y =  ycoord;
			tmpblkpos.z = 0;
			memorypos.push_back(tmpblkpos);
		}
	}


	//collect multiply resource
	for(int tmpmultistart = fmultistart; tmpmultistart <= Width; tmpmultistart = tmpmultistart + fmultirepeat)
	{
		for(int ycoord = 1; (ycoord + fmultiheight - 1) <= Height; ycoord = ycoord + fmultiheight)
		{
			Finaliopos tmpblkpos;
			tmpblkpos.x = tmpmultistart;
			tmpblkpos.y =  ycoord;
			tmpblkpos.z = 0;
			multiplypos.push_back(tmpblkpos);
		}
	}

	//srand(time(0));
	//random shuffle the values of v  
    random_shuffle(memorypos.begin(), memorypos.end());  
	random_shuffle(multiplypos.begin(), multiplypos.end());  

	memoryMultiplyCoords.clear();
   //randomly pick position for the memory and multiply
	//vector can have randomly order, eg. use shuffle function
	assert(memorypos.size() >= fmemorynum && multiplypos.size() >= fmultiplynum);  //has enough positions for placing memory and multiplier
	int i = 0;
	for(set<string>::iterator itm = memoryset.begin(); 
		 itm != memoryset.end();
		 itm++, i++)
	{
		memoryMultiplyCoords.insert(make_pair(*itm, memorypos[i]));
	}

	i = 0;
    for(set<string>::iterator itb = multiplyset.begin(); 
		 itb != multiplyset.end();
		 itb++, i++)
	{
		memoryMultiplyCoords.insert(make_pair(*itb, multiplypos[i]));
	}

	//the code below are used for clbs block
	floorplanXtofpgaX.clear();
	//based on the memory and muliply position, and Width and Height of, store the information for clb coordinat
	//collect the clbs' mapping relationship
	//map x -> x of fpga
	 //used to support the heterogenous blocks
	 int xcoord;
     if(0 == isheteroarch)
	 {
		 for(xcoord = 1; xcoord <= Width; xcoord++)
		 {
		     floorplanXtofpgaX.insert(make_pair(xcoord, xcoord));
		 }
	 }
	 else if(fmemorynum == 0 && 0 == fmultiplynum)  //.v only has clb blocks
	 {  //arch doesn't have multiplier, only has memory
         for(xcoord = 1; xcoord <= Width; xcoord++)
		 {
			 if(xcoord < fmemstart) // && xcoord < multistart)
			 {
				 floorplanXtofpgaX.insert(make_pair(xcoord, xcoord));
			 }
			 else
			 {
				 int memdetaX = 0;
				 memdetaX = (xcoord - fmemstart) / (fmemrepeat - 1);
				 floorplanXtofpgaX.insert(make_pair(xcoord, xcoord + memdetaX + 1));  
			 }
		 }
	 }
	 else if(fmemorynum > 0 && 0 == fmultiplynum)
	 {  //arch doesn't have multiplier, only has memory
         for(xcoord = 1; xcoord <= Width; xcoord++)
		 {
			 if(xcoord < fmemstart) // && xcoord < multistart)
			 {
				 floorplanXtofpgaX.insert(make_pair(xcoord, xcoord));
			 }
			 else
			 {
				 int memdetaX = 0;
				 memdetaX = (xcoord - fmemstart) / (fmemrepeat - 1);
				 floorplanXtofpgaX.insert(make_pair(xcoord, xcoord + memdetaX + 1));  
			 }
		 }
	 }
	 else if(0 == fmemorynum && fmultiplynum > 0)
	 {   //only has multiplier
	    int multidetaX = 0;
        if(xcoord < fmultistart)
		{
		   floorplanXtofpgaX.insert(make_pair(xcoord, xcoord));
		}
		else //if(xcoord >= multistart)
		{
		   multidetaX = (xcoord - fmultistart) / (fmultirepeat - 1);
		   floorplanXtofpgaX.insert(make_pair(xcoord, xcoord + multidetaX + 1));  
		}
	 }
	 else 
	 { // memorynum > 0 && multiplynum > 0
		//arch contain memory and multiply
	 }
}


void FPlan::moveMemoryAndMultiToFinalPlacefile(string finalplacefilename, int &blkcount)
{
	FILE *ff = fopen(finalplacefilename.c_str(), "a+");
	if(!ff)
	{
		cout<<"cannot open the file "<<finalplacefilename<<endl;
		exit(1);
	}

	for(map<string, Finaliopos>::iterator it = memoryMultiplyCoords.begin();
		it != memoryMultiplyCoords.end();
		it++)
	{
		string tmpblkname = it->first;
		Finaliopos &blkcoord = it->second;
		fprintf(ff, "%s\t", tmpblkname.c_str());
		if(strlen(tmpblkname.c_str()) < 8)
			fprintf(ff, "\t");

		fprintf(ff, "%d\t%d\t%d", blkcoord.x, blkcoord.y, blkcoord.z);
		fprintf(ff, "\t#%d\n", blkcount);
		blkcount++;
	}
	fclose(ff);
}


void FPlan::combinePlaceFiles(char *file)
{  
	//x and y dimensions of the FPGA itself, the core of the FPGA is from
	//[1..nx][1..ny], the I/Os form a perimeter surrounding the core  (btree module(0,0)<->(1,1)architecture file)
     //FPGA complex blocks grid [0..nx+1][0..ny+1] 
	//root module map to FPGA architecture and x + 1 y +1  
	//different soft modules cannot place same row or column 
	//should see the parse file to know emb and mac place which column
	//totally how many clbs are in the FPGA architecture
	//whether pad can placed in location (0, 0)
	ionameIopos.clear();
	filename = file;
	//finalplacefilename used to store the final result (the whole result)
	string finalplacefilename = filename + "/" + filename + ".place";
	string finalrouteNetfilename = filename + "/" + filename + ".routenet";
	string virtualpinfilename = filename + "/" + filename + ".virtualpins";

	//first clear the content in the file
    FILE *ff = fopen(finalplacefilename.c_str(), "w+");
	if(!ff)
	{
		cout<<"cannot open the final place file "<<finalplacefilename<<endl;
		exit(1);
	}
	fclose(ff);

	string subplacename;
	float ratio;
	int blkcount = 0;
	int ioblkcount = 0;
	int netcount = 0;
	int filenum = 0;
	string reconfigRegionfilename = filename + "/" + filename + ".configregion";
	FILE *ffp = fopen(reconfigRegionfilename.c_str(), "w+");
	if(!ffp) //remove the data stored before 
	{
		cout<<"cannot open the file "<<reconfigRegionfilename<<endl;
		exit(1);
	}
	fclose(ffp);

	ofstream reconfp(reconfigRegionfilename.c_str(), ios::app);
	if (!reconfp)
	{
		cout << "Unable to open " << reconfigRegionfilename << " for appending.\n";
		exit(1);
	}

	//Notice!!! add by fbmao  for routing
	//if routing faile expand expandcol, expandrow to support new resoure
	if(isrouteExpand)
	{
		for(int i = 0; i < modules_N; i++)
		{
			Module &mod = modules[i];
			//need to check whether module's id is right
			assert(mod.id == i);
			Module_Info &modInfo = modules_info[mod.id];
			modInfo.x += expandcol;
			modInfo.y += expandrow;
			modInfo.rx += expandcol;
			modInfo.ry += expandrow;
		}
		
		Width += expandcol*2;
        Height += expandrow*2;
	}

	//nx =(int)Width, ny(int)Height    //width height may be need to add 1 resepctively for reserve enough resource 
	//should consider the memory block column

	//for IO optimization   maybe used later for optimization
	//true: has occupied    false: donnot be used now
	vector<bool> left; //x = 0 col , y from 1 to ny row
	vector<bool> right; //x = nx+1, y from 1 to ny row
	vector<bool> lower; //x from 0 to nx,    y = 0
	vector<bool> top;    //x from 0 to nx,y = ny+1

	int topnx =(int)Width;  //top level
	int topny =(int)Height; 
	for(int i = 1; i <= topny; i++)
	{
		left.push_back(false);
		right.push_back(false);
	}

	for(int j = 1; j <= topnx; j++)
	{
		lower.push_back(false);
		top.push_back(false);
	}

	int flagindex = 1;
	int zflag = 0; //zflag [0..7]   now ch_intrinsic Io capacity is 8 should get from arch file
	bool leftflag = false;  // record its order: left, right, low, top and then to place IO
	bool rightflag = false;
	bool lowerflag = false;
	bool topflag = false;

	//only keep one clock in the place file,  now only support one single global clock
	vector<string> clockSet;
	//if clkset.size is empty, the circuit is useless 
	if(clkset.size() > 0)
	{
		assert(1 == clkset.size());
		set<string>::iterator clkiter = clkset.begin();
		string clkname = (*clkiter);   //should accept or read the clk
		//	clockSet.push_back("top^clk");  //before  clock name also should be changed
		clockSet.push_back(clkname); 
	}
	else
	{
		cout<<"Warning: the circuit doesn't have clk, pls check!"<<endl;
	}


	string finalplaceinfoname = filename + "/" + filename + ".moduleplaceinfo";
	if(1)
	{
		FILE* fmoduleinfo = NULL;
		fmoduleinfo = fopen(finalplaceinfoname.c_str(),"w+");   //used to store new data,not a+
		if(NULL == fmoduleinfo)
		{
			printf("canot open testbug.txt,pls check\n");
			exit(1);
		}
		fclose(fmoduleinfo);
	}


    //add for loadable flow, for getting the whole place file
	string ioforfinalplacefile = filename + "/placefiles/" + filename + ".ioplace";
	FILE *ioforfinalplace = fopen(ioforfinalplacefile.c_str(), "w+");
	if(!ioforfinalplace)
	{
		cout<<"cannot open the file "<<ioforfinalplacefile.c_str()<<endl;
		exit(1);
	}


	//for support heterogenous blocks  eg. DSP, Memory
     //memory resource,  multiplier resource
	//after this function, the Width will be changed, if it has memory, multiply
	if(isheteroarch && (0 != memoryset.size() || 0 != multiplyset.size()))
	{  //isheteroarch  is true, means currently, the arch contains memory and muliply
		FILE *infile;
		char **tokens;
		int line;
		string archfile = "6k6_n8.xml";
		string newnetfilename = filename + ".net";
		FILE *ff = fopen(finalplacefilename.c_str(), "a+");
		if(!ff)
		{
			cout<<"cannot open the file "<<finalplacefilename<<endl;
			exit(1);
		}

		if(0 == blkcount)
		{    //filename like bgm
			//fprintf(ff, "Netlist file: %s   Architecture file: %s\n", newnetfilename.c_str(),
			//	archfile.c_str());

			fprintf(ioforfinalplace, "Netlist file: %s   Architecture file: %s\n", newnetfilename.c_str(),
				archname.c_str());
			fprintf(ioforfinalplace, "Array size: %d x %d logic blocks\n\n", (int)Width, (int)Height);
			fprintf(ioforfinalplace, "#block name\tx\ty\tsubblk\tblock number\n");
			fprintf(ioforfinalplace, "#----------\t--\t--\t------\t------------\n");

			//add for loadable flow
			fprintf(ioforfinalplace, "Netlist file: %s   Architecture file: %s\n", newnetfilename.c_str(),
				archname.c_str());
			fprintf(ioforfinalplace, "Array size: %d x %d logic blocks\n\n", (int)Width, (int)Height);
			fprintf(ioforfinalplace, "#block name\tx\ty\tsubblk\tblock number\n");
			fprintf(ioforfinalplace, "#----------\t--\t--\t------\t------------\n");

		}	
		fclose(ff);

		moveMemoryAndMultiToFinalPlacefile(finalplacefilename, blkcount);
	}

	fclose(ioforfinalplace);

	//each module corresponding to a place file
	for(int i = 0; i < modules_N; i++)
	{
		Module &mod = modules[i];
		//need to check whether module's id is right
		assert(mod.id == i);
	    Module_Info &modInfo = modules_info[mod.id];
		subplacename = mod.name;
		//define a relationship between ration and path subscript can give a direct path
		ratio = mod.ratio;
		string tmpratio, sratio;  
		stringstream ss;
        ss.str(""); //release temp memory
		ss<<mod.ratio;
		tmpratio = ss.str();
		size_t pos = tmpratio.find(".");

		if(pos != string::npos)
		{
			sratio = tmpratio.substr(0, pos) + tmpratio.substr(pos+1, pos+2);  //need to check the code??
		}
		else
		{
			sratio = tmpratio + "0";
		}

		//first find the place file eg.
		//path
		string submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".v/"+ subplacename + ".place";
		string submoduleroutefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".v/" + subplacename + ".route";
		fs.open(submoduleplacefile.c_str()); //judge whether it exist and assign the name to the submoduleplacefile
		if(!fs)   //.v, .blif
		{   //notice the path
			printf("canot open the file %s", const_cast<char*>(submoduleplacefile.c_str()));
		    submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".blif/"+ subplacename + ".place";
			submoduleroutefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".blif/"+ subplacename + ".route";
			fs.close();
			fs.open(submoduleplacefile.c_str());
			if(!fs)
			{
				cout<<"cannot open the file:  "<<submoduleplacefile<<endl;
				exit(1);
			}
		}
		fs.close();
	    char subpostfix[80];
	    //need to check whether it converts right
	//	itoa(filenum,subpostfix, 10); 
		sprintf(subpostfix, "%d", filenum);  //
 
		cout<<i<<" "<<submoduleplacefile<<"   "<<sratio<<" "<<" "<<mod.width<<" "<<mod.height<<endl;

	    //fbmao debug test
#if 1
		if(1)
		{
			FILE* fmoduleinfo = fopen(finalplaceinfoname.c_str(),"a+");
			if(NULL != fmoduleinfo)
			{
				fprintf(fmoduleinfo,"submodule %d submoduleplacefile %s sratio %s mod.width %d mod.height %d\n",
					i,submoduleplacefile.c_str(),sratio.c_str(),(int)mod.width,(int)mod.height);
			}
			else
			{
				printf("canot open testbug.txt,pls check\n");
				exit(1);
			}
			fclose(fmoduleinfo);
		}
#endif

		//add for control the pr region number
		//store reconfigurable region
		int tempprnum =  (int)(percentprnum * size());
		if(tempprnum == 0)
			tempprnum = 1;
		if((percentprnum * size()) > (tempprnum + 0.5))
		{
			tempprnum = tempprnum + 1;
		}

		if(1 == (int)size() || filenum < tempprnum)
		{
		    reconfp<<modInfo.x<<"\t"<<modInfo.rx<<"\t"<<modInfo.y<<"\t"<<modInfo.ry<<"\t"<<filenum<<"\t"
			    <<subplacename.c_str()<<"\t"<<1<<"\t"<<0<<endl; //should add one later and modified route code
		                                                     //add reconfigurablereion loadableregion at the end
		}
		else
		{
			reconfp<<modInfo.x<<"\t"<<modInfo.rx<<"\t"<<modInfo.y<<"\t"<<modInfo.ry<<"\t"<<filenum<<"\t"
			    <<subplacename.c_str()<<"\t"<<0<<"\t"<<0<<endl;
		}
					
		//need to convert module based placement coordinate to tile based FPGA coordinate
		readAndWrite_place(subplacename.c_str(), finalplacefilename, finalrouteNetfilename,
		   virtualpinfilename, submoduleplacefile.c_str(), submoduleroutefile.c_str(), 
		   mod.width, mod.height, mod.ratio, blkcount, ioblkcount, netcount, modInfo, subpostfix, 
		   left, right, lower, top, leftflag, rightflag, lowerflag, topflag, flagindex, 
		   zflag, clockSet); 
		filenum++;
	}
	reconfp.close();

	//move IO to final place file
	//Approach 1:  
	//play io block one by one along the boundary, add IO blocks in the final placement file
	//moveIOblksToFinalplacefile(finalplacefilename, blkcount, ioblkcount);


   if(0 == io3)   //default io3 : 0   //use the module center coordinate instead of using the module port's coordinate
   {
	 //Approach2:
     moveIOblksToFinalplacefileConsiderOrigiposition(finalplacefilename, blkcount, ioblkcount);
   }
   else  //consider the module port's coordinate, when calculating wirelength
   {
	 //approach3:   use the result generated by randomly perturbing IOs
	 randomFinalIoblks();
	 moveRandomIOblksToFinalPlacefile(finalplacefilename, blkcount, ioblkcount);
   }

    assert((int)finaliosets.size() == ioblkcount); //should change later delete the memory number
	//for debug
	if((int)finaliosets.size() != ioblkcount)
	{
		cout<<(int)finaliosets.size() <<"  "<<ioblkcount<<endl;
	    exit(1);
	}
}


void FPlan::doLocalplaceForEachmoduleMapToArch()
{
	
	//arch info
	//fmemstart = 2;
	//fmemrepeat = 8;
	//fmemoryheight = 6;

	////multiply     //mult_36
	//fmultistart = 4;
	//fmultirepeat = 8;
	//fmultiheight = 4;

	//each module has a region. and then need to assign the block inside the module to a 
	//feasible postion in the architecture.
	//Notice:  if the number of columns for placing RAM or DSP larger than (RAM colums (or DSP columns)) 
	          //1. Yes.  just map them to final FPGA architecture one by one . need to consider position constraint
	          //2. NO.  2.1 frist place the memory in the column one by one
	                   //2.2 (simultaneous collect the unused RAM position)
	         //2.3 if the avialable column cannot exist. place left unplaced memory in the unused positions one by one in order   
	//visit module one by one
	string subplacename;
	float ratio;
	//each module corresponding to a place file
	for(int i = 0; i < modules_N; i++)
	{
		Module &mod = modules[i];
		//need to check whether module's id is right
		assert(mod.id == i);
		Module_Info &modInfo = modules_info[mod.id];
		subplacename = mod.name;

		//define a relationship between ration and path subscript can give a direct path
		ratio = mod.ratio;
		mod.finalxyzblkname.clear();

		//for a region  (a module)
		//the coordinate in the final architecture
		int startcol = modInfo.x + 1;  //module's start column
		int endcol = modInfo.rx;

		int startrowy = modInfo.y + 1;
		int endrowy = modInfo.ry;

		//Notice:  modInfo.rx (may not equal to) (modInfo.x + mod.width)

		//Ram cols in Region(corresponds to Architecture)
		vector<int> colCoordRamInArch;
		vector<int> colCoordDspInArch;
		vector<int> colCoordClbInArch;
		colCoordRamInArch.clear();
		colCoordDspInArch.clear();
		colCoordClbInArch.clear();

		//Position list for placing RAM and DSP. should consider the start position and height
		//map<x, set<pair<y,z> > > xyz
		map<int, set<pair<int, int> > > ramPoslist;
		map<int, set<pair<int, int> > > dspPoslist;
		ramPoslist.clear();
		dspPoslist.clear();

		set<pair<int, int> > ramyzset;
		set<pair<int, int> > dspyzset;
		ramyzset.clear();
		dspyzset.clear();

		//collect RAM position list
		//the minimum coordinate of RAM, DSP
		int ramminy = 1;
		int dspminy = 1;
		//RAM and DSP  Y coordinates start from 1
		for(int tmprow = startrowy; tmprow <= endrowy; tmprow++)
		{
			if(tmprow >= ramminy && 0 == (tmprow - ramminy) % fmemoryheight
				&& tmprow + fmemoryheight <= endrowy) //cannot beyong the 
			{
				ramyzset.insert(make_pair(tmprow, 0)); //assume all the z is 0.
			}

			if(tmprow >= dspminy && 0 == (tmprow - dspminy) % fmultiheight
				&& tmprow + fmultiheight <= endrowy)
			{
				dspyzset.insert(make_pair(tmprow, 0));
			}
		}

		//collect the clb position and then assign them
		//Assuming only has three types:  CLB, RAM, DSP
		//remove the blank column only for ram and memory
		for(int tmpcol = startcol; tmpcol <= endcol; tmpcol++)
		{	
			if(tmpcol >= fmemstart && 0 == (tmpcol - fmemstart) % fmemrepeat)
			{  //the column only for placing ram
				colCoordRamInArch.push_back(tmpcol);
			}
			else if(tmpcol >= fmultistart && 0 == (tmpcol - fmultistart) % fmultirepeat)
			{  //the col only for placing dsp 
				colCoordDspInArch.push_back(tmpcol);
			}
			else   // the col only for placing clb.   
			{
				colCoordClbInArch.push_back(tmpcol);
			}
		}

		//get the position list for placing RAM and DSP
		//later use the poslist for placing unplaced RAM and DSP
		for(int tmpi = 0; tmpi < colCoordRamInArch.size(); tmpi++)
		{
			int tmpramcolx = colCoordRamInArch[tmpi];
			ramPoslist.insert(make_pair(tmpramcolx, ramyzset));
		}

		for(int tmpj = 0; tmpj < colCoordDspInArch.size(); tmpj++)
		{
			int tmpdspcolx = colCoordDspInArch[tmpj];
			dspPoslist.insert(make_pair(tmpdspcolx, dspyzset));
		}


		//info get from architecture
		int ramlegalstartrow = 1;   //the minimum start row for placing RAM
		int dsplegalstartrow = 1;  //the minimum start row for placing DSP

		int ramshiftgap = 0; //shift to a legal position
		int dspshiftgap = 0;

		//first judge whether the start point for placing RAM and DSP is legal or not?
		//1> find the gap for shift ram to legal position
		int tmpfirstRamlegalpos = ramlegalstartrow;      //find gap can be better??? 
		while(tmpfirstRamlegalpos < startrowy) //archy is (module's lowest coordinate + 1)
		{
			tmpfirstRamlegalpos = tmpfirstRamlegalpos + fmemoryheight;
		}

		ramshiftgap = tmpfirstRamlegalpos - startrowy;

		int tmpfirstDsplegalpos = dsplegalstartrow;
		while(tmpfirstDsplegalpos < startrowy) //archy is (module's lowest coordinate + 1)
		{
			tmpfirstDsplegalpos = tmpfirstDsplegalpos + fmultiheight;
		}

		dspshiftgap = tmpfirstDsplegalpos - startrowy;

		//collect RAM, DSP
		//the ratioxyzblkname :  coordinate in original submodule
		set<int> tmpramcolset;  //ram col
		set<int> tmpdspcolset; //dsp col
		tmpramcolset.clear();
		tmpdspcolset.clear();

		map<float, map<int, map<pair<int,int>, string> > >::iterator ita = mod.ratioxyzblkname.find(ratio);
		assert(ita != mod.ratioxyzblkname.end());
		//the coordinate in submodule
		map<int, map<pair<int,int>, string> >& tmpxyzblks = ita->second;

		//the block inside module has already placed in order. and collected in the mod.ratioxyzblk
		if(0 == mod.numram && 0 == mod.numdsp)
		{ // 1 >>>>>>>    module only contains clb 
			assert(colCoordClbInArch.size() >= tmpxyzblks.size());  //comapre the number of column for clb

			//convert the coordinate in submodule to the arch coordinate
			int j = 0;
			for(map<int, map<pair<int,int>, string> >::iterator itb = tmpxyzblks.begin();
				itb != tmpxyzblks.end();
				itb++)
			{
				map<pair<int,int>, string> tmpfinalyzblks;
				tmpfinalyzblks.clear();

				//update the y value (add the bottom-left coordinate of this module
				map<pair<int,int>, string> & tmpyzblks = itb->second;
				for(map<pair<int,int>, string>::iterator itc = tmpyzblks.begin();
					itc != tmpyzblks.end();
					itc++)
				{   //y
					int tmpfinaly = itc->first.first;
					//update y 
					tmpfinaly = tmpfinaly + modInfo.y;
					///z 
					int tmpfinalz = itc->first.second;
					string tmpblkname = itc->second;
					tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
				}

				//update x
				int finalx = colCoordClbInArch[j];
				map<int, map<pair<int,int>, string> >::iterator itd = mod.finalxyzblkname.find(finalx);
				assert(itd == mod.finalxyzblkname.end()); //should not exist before
				mod.finalxyzblkname.insert(make_pair(finalx, tmpfinalyzblks));
				j++;
			}
		}
		else if(mod.numram > 0 && mod.numdsp > 0)
		{   ////2 >>>>  module contains both RAM and DSP
			map<string, map<string, set<string> > >::iterator typeblkit = filenameblktypeblkname.find(subplacename);
			assert(typeblkit != filenameblktypeblkname.end()); //has ram or dsp
			map<string, set<string> >& tmpblktypeblknameset = typeblkit->second;
			map<string, set<string> >::iterator ramtypeit = tmpblktypeblknameset.find(blkRamType); ////"memory");
			map<string, set<string> >::iterator dsptypeit = tmpblktypeblknameset.find(blkDspType); //// "multiply");

			//RAM set
			assert(ramtypeit != tmpblktypeblknameset.end());
			set<string> &memblkset = ramtypeit->second;

			//DSP set
			assert(dsptypeit != tmpblktypeblknameset.end());	
			set<string> &dspblkset = dsptypeit->second;

			//only collect RAM, DSP
			for(map<int, map<pair<int,int>, string> >::iterator itf = tmpxyzblks.begin();
				itf != tmpxyzblks.end();
				itf++)
			{
				int tmpx = itf->first;
				int flag = 0; //0: it is clb blk,   1: RAM or DSP
				map<pair<int,int>, string> & tmpyzblks = itf->second;
				for(map<pair<int,int>, string>::iterator itg = tmpyzblks.begin();
					itg != tmpyzblks.end();
					itg++)
				{
					string tmpblk = itg->second;
					set<string>::iterator tmpramit = memblkset.find(tmpblk);
					set<string>::iterator tmpdspit = dspblkset.find(tmpblk);

					if(tmpramit != memblkset.end())
					{
						tmpramcolset.insert(tmpx);
					}
					else if(tmpdspit != dspblkset.end())
					{
						tmpdspcolset.insert(tmpx);
					}
					break;  
				}
			}

			//place RAM
			int j = 0;
			//if(colCoordRamInArch.size() >= tmpramcolset.size())
			int colramsize = colCoordRamInArch.size();
			set<int>::iterator tmpramit;
			for(tmpramit = tmpramcolset.begin();  //ram x in original module (region)
				tmpramit != tmpramcolset.end() && j <= colramsize -1;
				tmpramit++)
			{
				map<pair<int,int>, string> tmpfinalyzblks;
				tmpfinalyzblks.clear();

				int tmpx = *tmpramit;
				map<int, map<pair<int,int>, string> >::iterator ith = tmpxyzblks.find(tmpx);
				assert(ith != tmpxyzblks.end());
				//update the y value (add the bottom-left coordinate of this module
				map<pair<int,int>, string> & tmpyzblks = ith->second;
				for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
					itk != tmpyzblks.end();
					itk++)
				{  
					string tmpblkname = itk->second;
					//y
					int tmpfinaly = itk->first.first;

					//update y 
					tmpfinaly = tmpfinaly + modInfo.y + ramshiftgap;  //add the ram shift gap (to be a legal position)
					//z
					int tmpfinalz = itk->first.second;

					//remove the resource in the architecture
					map<int, set<pair<int, int> > > ::iterator ramposit = ramPoslist.find(colCoordRamInArch[j]);
					assert(ramposit != ramPoslist.end());
					set<pair<int, int> > & tmpramposlist = ramposit->second;
					pair<int, int> usedyz = make_pair(tmpfinaly, tmpfinalz);
					////////delete y z here  
					tmpramposlist.erase(usedyz);

					tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
				}

				//update x
				int finalx = colCoordRamInArch[j];
				map<int, map<pair<int,int>, string> >::iterator itd = mod.finalxyzblkname.find(finalx);
				assert(itd == mod.finalxyzblkname.end()); //should not exist before
				mod.finalxyzblkname.insert(make_pair(finalx, tmpfinalyzblks));
				j++;
			}

			//the number of ram column in final architecture is less than the number of ram
			//column in the region (module). we need to use the unused position for placing ram block 
			//deal with the left unplaced ram
			//if(colCoordRamInArch.size() < tmpramcolset.size())
			if(tmpramit != tmpramcolset.end() && colCoordRamInArch.size() == j) 
			{  //place the unplaced RAMs in the unused positions of RAM
				//unplace ram   <--->   //unused place position
				for(; tmpramit != tmpramcolset.end(); tmpramit++)
				{
					int tmpx = *tmpramit;						
					map<int, map<pair<int,int>, string> >::iterator ith = tmpxyzblks.find(tmpx);
					assert(ith != tmpxyzblks.end());
					//update the y value (add the bottom-left coordinate of this module
					map<pair<int,int>, string> & tmpyzblks = ith->second;
					for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
						itk != tmpyzblks.end();
						itk++)
					{  
						string tmpblkname = itk->second;
						int tmpfinalx = -1;
						int tmpfinaly = -1;
						int tmpfinalz = -1;

						int findpos = 0; //0: do not find.   1: find a position for the RAM

						for(map<int, set<pair<int, int> > >::iterator ita = ramPoslist.begin();
							ita != ramPoslist.end();
							ita++)
						{
							tmpfinalx = ita->first; //tmpx  in the final arch (coordinate)
							set<pair<int, int> > & leftramyz = ita->second;
							for(set<pair<int, int> >::iterator itb = leftramyz.begin();
								itb != leftramyz.end();
								itb++)
							{
								tmpfinaly = itb->first;
								tmpfinalz = itb->second;
								findpos = 1;
								leftramyz.erase(itb);
							}

							if(1 == findpos)
							{
								break;
							}
							else
							{
								ramPoslist.erase(ita);
							}			
						}

						assert(1 == findpos); //at least find a position
						map<int, map<pair<int,int>, string> >::iterator itc = mod.finalxyzblkname.find(tmpfinalx);
						if(itc != mod.finalxyzblkname.end()) //should not exist before
						{
							itc->second.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
						}
						else
						{
							map<pair<int,int>, string> tmpfinalyzblks;
							tmpfinalyzblks.clear();
							tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
							mod.finalxyzblkname.insert(make_pair(tmpfinalx, tmpfinalyzblks));
						}
					}
				}
			}

			//place DSP
			//firstly place the ram and dsp block
			j = 0;
			set<int>::iterator tmpdspit;
			//if(colCoordDspInArch.size() >= tmpdspcolset.size());
			for(tmpdspit = tmpdspcolset.begin();
				tmpdspit != tmpdspcolset.end() && j <= colCoordDspInArch.size() - 1;
				tmpdspit++)
			{
				map<pair<int,int>, string> tmpfinalyzblks;
				tmpfinalyzblks.clear();

				int tmpx = *tmpdspit;
				map<int, map<pair<int,int>, string> >::iterator ith = tmpxyzblks.find(tmpx);
				assert(ith != tmpxyzblks.end());
				//update the y value (add the bottom-left coordinate of this module
				map<pair<int,int>, string> & tmpyzblks = ith->second;
				for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
					itk != tmpyzblks.end();
					itk++)
				{   
					string tmpblkname = itk->second;
					//y
					int tmpfinaly = itk->first.first;
					//update y 
					tmpfinaly = tmpfinaly + modInfo.y + dspshiftgap;
					//z
					int tmpfinalz = itk->first.second;

					//remove the resource in the architecture
					map<int, set<pair<int, int> > > ::iterator dspposit = dspPoslist.find(colCoordDspInArch[j]);
					assert(dspposit != dspPoslist.end());
					set<pair<int, int> > & tmpdspposlist = dspposit->second;
					pair<int, int> usedyz = make_pair(tmpfinaly, tmpfinalz);
					////////delete y z here  
					tmpdspposlist.erase(usedyz);	

					tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
				}

				//update x
				int finalx = colCoordDspInArch[j];
				map<int, map<pair<int,int>, string> >::iterator itd = mod.finalxyzblkname.find(finalx);
				assert(itd == mod.finalxyzblkname.end()); //should not exist before
				mod.finalxyzblkname.insert(make_pair(finalx, tmpfinalyzblks));
				j++;
			}

			//Dsp column not enough
			//if(colCoordDspInArch.size() < tmpdspcolset.size());
			if(tmpdspit != tmpdspcolset.end() && colCoordDspInArch.size() == j) 
			{  //place the unplaced DSPs in the unused positions of DSP
				//unplace DSP   <--->   //unused place position
				for(; tmpdspit != tmpdspcolset.end(); tmpdspit++)
				{
					int tmpx = *tmpdspit;						
					map<int, map<pair<int,int>, string> >::iterator ith = tmpxyzblks.find(tmpx);
					assert(ith != tmpxyzblks.end());
					//update the y value (add the bottom-left coordinate of this module
					map<pair<int,int>, string> & tmpyzblks = ith->second;
					for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
						itk != tmpyzblks.end();
						itk++)
					{  
						string tmpblkname = itk->second;
						int tmpfinalx = -1;
						int tmpfinaly = -1;
						int tmpfinalz = -1;

						int findpos = 0; //0: do not find.   1: find a position for the RAM

						for(map<int, set<pair<int, int> > >::iterator ita = dspPoslist.begin();
							ita != dspPoslist.end();
							ita++)
						{
							tmpfinalx = ita->first; //tmpx  in the final arch (coordinate)
							set<pair<int, int> > & leftdspyz = ita->second;
							for(set<pair<int, int> >::iterator itb = leftdspyz.begin();
								itb != leftdspyz.end();
								itb++)
							{
								tmpfinaly = itb->first;
								tmpfinalz = itb->second;
								findpos = 1;
								leftdspyz.erase(itb);
							}

							if(1 == findpos)
							{
								break;
							}
							else
							{
								dspPoslist.erase(ita);
							}			
						}

						assert(1 == findpos); //at least find a position
						map<int, map<pair<int,int>, string> >::iterator itc = mod.finalxyzblkname.find(tmpfinalx);
						if(itc != mod.finalxyzblkname.end()) //should not exist before
						{
							itc->second.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
						}
						else
						{
							map<pair<int,int>, string> tmpfinalyzblks;
							tmpfinalyzblks.clear();
							tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
							mod.finalxyzblkname.insert(make_pair(tmpfinalx, tmpfinalyzblks));
						}
					}
				}
			}


			//place CLB
			j = 0;
			for(map<int, map<pair<int,int>, string> >::iterator itb = tmpxyzblks.begin();
				itb != tmpxyzblks.end();
				itb++)
			{
				int tmpx = itb->first;
				set<int>::iterator ramit = tmpramcolset.find(tmpx);
				set<int>::iterator dspit= tmpdspcolset.find(tmpx);
				if(ramit != tmpramcolset.end() || dspit != tmpdspcolset.end()) //it is ram, already solved
					continue;

				//deal with clb's column
				map<pair<int,int>, string> tmpfinalyzblks;
				tmpfinalyzblks.clear();

				//update the y value (add the bottom-left coordinate of this module
				map<pair<int,int>, string> & tmpyzblks = itb->second;
				for(map<pair<int,int>, string>::iterator itc = tmpyzblks.begin();
					itc != tmpyzblks.end();
					itc++)
				{   //y
					int tmpfinaly = itc->first.first;
					//update y 
					tmpfinaly = tmpfinaly + modInfo.y;
					//z
					int tmpfinalz = itc->first.second;
					string tmpblkname = itc->second;
					tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
				}

				//update x
				assert(j < colCoordClbInArch.size());
				int finalx = colCoordClbInArch[j];
				map<int, map<pair<int,int>, string> >::iterator itd = mod.finalxyzblkname.find(finalx);
				assert(itd == mod.finalxyzblkname.end()); //should not exist before
				mod.finalxyzblkname.insert(make_pair(finalx, tmpfinalyzblks));
				j++;
			} //dspnum > 0 && ramnum >0
		}
		else //3>>>> has only Ram   or has only dsp (mod.numram > 0 && mod.numdsp == 0) || (mod.numram == 0 && mod.numdsp > 0)     
		{ 
			map<string, map<string, set<string> > >::iterator typeblkit = filenameblktypeblkname.find(subplacename);
			assert(typeblkit != filenameblktypeblkname.end()); //has ram or dsp
			map<string, set<string> >& tmpblktypeblknameset = typeblkit->second;
			map<string, set<string> >::iterator ramtypeit = tmpblktypeblknameset.find(blkRamType); ////"memory");
			map<string, set<string> >::iterator dsptypeit = tmpblktypeblknameset.find(blkDspType); //// "multiply");

			//RAM set
			set<string> memblkset;
			memblkset.clear();
			if(ramtypeit != tmpblktypeblknameset.end())
				memblkset = ramtypeit->second;

			//DSP set
			set<string> dspblkset;
			dspblkset.clear();
			if(dsptypeit != tmpblktypeblknameset.end())	
				dspblkset = dsptypeit->second;

			//collect RAM, DSP
			for(map<int, map<pair<int,int>, string> >::iterator itf = tmpxyzblks.begin();
				itf != tmpxyzblks.end();
				itf++)
			{
				int tmpx = itf->first;
				map<pair<int,int>, string> & tmpyzblks = itf->second;
				for(map<pair<int,int>, string>::iterator itg = tmpyzblks.begin();
					itg != tmpyzblks.end();
					itg++)
				{
					string tmpblk = itg->second;

					if(mod.numram > 0) //only contains ram
					{
						set<string>::iterator tmpramit = memblkset.find(tmpblk);

						if(tmpramit != memblkset.end())
						{
							tmpramcolset.insert(tmpx);
						}
					}
					else if(mod.numdsp > 0)
					{
						set<string>::iterator tmpdspit = dspblkset.find(tmpblk);
						if(tmpdspit != dspblkset.end())
						{
							tmpdspcolset.insert(tmpx);
						}
					}
				}
			}

			//firstly place the ram and dsp block
			//secondly place the clb
			//blk's col start from 1  in submodule
			if(mod.numram > 0) //3.1  >>>  only contains RAM
			{
				//firstly place the ram and dsp block
				int j = 0;
				set<int>::iterator tmpramit;
				//if(colCoordRamInArch.size() >= tmpramcolset.size());
				for(tmpramit = tmpramcolset.begin();
					tmpramit != tmpramcolset.end() && j <= colCoordRamInArch.size();
					tmpramit++)
				{
					map<pair<int,int>, string> tmpfinalyzblks;
					tmpfinalyzblks.clear();

					int tmpx = *tmpramit;
					map<int, map<pair<int,int>, string> >::iterator ith = tmpxyzblks.find(tmpx);
					assert(ith != tmpxyzblks.end());
					//update the y value (add the bottom-left coordinate of this module
					map<pair<int,int>, string> & tmpyzblks = ith->second;
					for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
						itk != tmpyzblks.end();
						itk++)
					{  
						string tmpblkname = itk->second;
						//y
						int tmpfinaly = itk->first.first;
						//update y 
						tmpfinaly = tmpfinaly + modInfo.y + ramshiftgap;
						//z
						int tmpfinalz = itk->first.second;

						//remove the resource in the architecture
						map<int, set<pair<int, int> > > ::iterator ramposit = ramPoslist.find(colCoordRamInArch[j]);
						assert(ramposit != ramPoslist.end());
						set<pair<int, int> > & tmpramposlist = ramposit->second;
						pair<int, int> usedyz = make_pair(tmpfinaly, tmpfinalz);
						////////delete y z here  
						tmpramposlist.erase(usedyz);

						tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
					}

					//update x
					int finalx = colCoordRamInArch[j];
					map<int, map<pair<int,int>, string> >::iterator itd = mod.finalxyzblkname.find(finalx);
					assert(itd == mod.finalxyzblkname.end()); //should not exist before
					mod.finalxyzblkname.insert(make_pair(finalx, tmpfinalyzblks));
					j++;
				}



				//the number of ram column in final architecture is less than the number of ram
				//column in the region (module). we need to use the unused position for placing ram block 
				//deal with the left unplaced ram
				//if(colCoordRamInArch.size() < tmpramcolset.size())
				if(tmpramit != tmpramcolset.end() && colCoordRamInArch.size() == j) 
				{  //place the unplaced RAMs in the unused positions of RAM
					//unplace ram   <--->   //unused place position
					for(; tmpramit != tmpramcolset.end(); tmpramit++)
					{
						int tmpx = *tmpramit;						
						map<int, map<pair<int,int>, string> >::iterator ith = tmpxyzblks.find(tmpx);
						assert(ith != tmpxyzblks.end());
						//update the y value (add the bottom-left coordinate of this module
						map<pair<int,int>, string> & tmpyzblks = ith->second;
						for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
							itk != tmpyzblks.end();
							itk++)
						{  
							string tmpblkname = itk->second;
							int tmpfinalx = -1;
							int tmpfinaly = -1;
							int tmpfinalz = -1;

							int findpos = 0; //0: do not find.   1: find a position for the RAM

							for(map<int, set<pair<int, int> > >::iterator ita = ramPoslist.begin();
								ita != ramPoslist.end();
								ita++)
							{
								tmpfinalx = ita->first; //tmpx  in the final arch (coordinate)
								set<pair<int, int> > & leftramyz = ita->second;
								for(set<pair<int, int> >::iterator itb = leftramyz.begin();
									itb != leftramyz.end();
									itb++)
								{
									tmpfinaly = itb->first;
									tmpfinalz = itb->second;
									findpos = 1;
									leftramyz.erase(itb);
								}

								if(1 == findpos)
								{
									break;
								}
								else
								{
									ramPoslist.erase(ita);
								}			
							}

							assert(1 == findpos); //at least find a position
							map<int, map<pair<int,int>, string> >::iterator itc = mod.finalxyzblkname.find(tmpfinalx);
							if(itc != mod.finalxyzblkname.end()) //should not exist before
							{
								itc->second.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
							}
							else
							{
								map<pair<int,int>, string> tmpfinalyzblks;
								tmpfinalyzblks.clear();
								tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
								mod.finalxyzblkname.insert(make_pair(tmpfinalx, tmpfinalyzblks));
							}
						}
					}
				}


				//place clb.   segment length more close to the original segment
				int leftgapinmodule = *(tmpramcolset.begin()) - 1;	
				int leftgapinArch = *(colCoordRamInArch.begin())- startcol;
				if(leftgapinmodule >= leftgapinArch) //only need map clb one by one
				{
					int j = 0;
					assert(colCoordClbInArch.size() >= (tmpxyzblks.size() - tmpramcolset.size()));
					for(map<int, map<pair<int,int>, string> >::iterator itb = tmpxyzblks.begin();
						itb != tmpxyzblks.end();
						itb++)
					{
						int tmpx = itb->first;
						set<int>::iterator itc = tmpramcolset.find(tmpx);
						if(itc != tmpramcolset.end()) //it is ram, already solved
							continue;


						map<pair<int,int>, string> tmpfinalyzblks;
						tmpfinalyzblks.clear();

						//update the y value (add the bottom-left coordinate of this module
						map<pair<int,int>, string> & tmpyzblks = itb->second;
						for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
							itk != tmpyzblks.end();
							itk++)
						{   //y
							int tmpfinaly = itk->first.first;
							//update y 
							tmpfinaly = tmpfinaly + modInfo.y;
							//z
							int tmpfinalz = itk->first.second;
							string tmpblkname = itk->second;
							tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
						}

						//update x
						assert(j < colCoordClbInArch.size());
						int finalx = colCoordClbInArch[j];
						map<int, map<pair<int,int>, string> >::iterator itd = mod.finalxyzblkname.find(finalx);
						assert(itd == mod.finalxyzblkname.end()); //should not exist before
						mod.finalxyzblkname.insert(make_pair(finalx, tmpfinalyzblks));
						j++;
					}
				}
				else  //leftgapinmodule < leftgapinArch
				{
					//find the start pos in vector
					//if the segment contains all the memory, it is ok.    
					int numclbcol = tmpxyzblks.size() - tmpramcolset.size();
					int startramcol = colCoordRamInArch[0];
					int tmpramnum = colCoordRamInArch.size();
					int endramcol = colCoordRamInArch[tmpramnum -1];
					int startposclb = 0; //colCoordRamInArch[startposclb].  make the orignal segment match better in final architecture
					while(startposclb + numclbcol < colCoordClbInArch.size() && !(startramcol > colCoordClbInArch[startposclb]  
					&& startramcol < colCoordClbInArch[startposclb + numclbcol - 1]))
					{
						startposclb++;
					}

					for(map<int, map<pair<int,int>, string> >::iterator itb = tmpxyzblks.begin();
						itb != tmpxyzblks.end();
						itb++)
					{
						int tmpx = itb->first;
						set<int>::iterator itc = tmpramcolset.find(tmpx);
						if(itc != tmpramcolset.end()) //it is ram, already solved
							continue;

						map<pair<int,int>, string> tmpfinalyzblks;
						tmpfinalyzblks.clear();

						//update the y value (add the bottom-left coordinate of this module
						map<pair<int,int>, string> & tmpyzblks = itb->second;
						for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
							itk != tmpyzblks.end();
							itk++)
						{   //y
							int tmpfinaly = itk->first.first;
							//update y 
							tmpfinaly = tmpfinaly + modInfo.y;
							//z
							int tmpfinalz = itk->first.second;
							string tmpblkname = itk->second;
							tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
						}

						//update x
						assert(startposclb < colCoordClbInArch.size());
						int finalx = colCoordClbInArch[startposclb];
						map<int, map<pair<int,int>, string> >::iterator itd = mod.finalxyzblkname.find(finalx);
						assert(itd == mod.finalxyzblkname.end()); //should not exist before
						mod.finalxyzblkname.insert(make_pair(finalx, tmpfinalyzblks));
						startposclb++;
					}                 

				}
			} //if numram > 0

			else //mod.numdsp > 0    3.2>>>>   only has dsp
			{
				//firstly place the ram and dsp block
				int j = 0;
				set<int>::iterator tmpdspit;
				assert(colCoordDspInArch.size() >= tmpdspcolset.size());
				for(tmpdspit = tmpdspcolset.begin();
					tmpdspit != tmpdspcolset.end() && j <= colCoordDspInArch.size()-1;
					tmpdspit++)
				{
					map<pair<int,int>, string> tmpfinalyzblks;
					tmpfinalyzblks.clear();

					int tmpx = *tmpdspit;
					map<int, map<pair<int,int>, string> >::iterator ith = tmpxyzblks.find(tmpx);
					assert(ith != tmpxyzblks.end());
					//update the y value (add the bottom-left coordinate of this module
					map<pair<int,int>, string> & tmpyzblks = ith->second;
					for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
						itk != tmpyzblks.end();
						itk++)
					{   
						string tmpblkname = itk->second;
						//y
						int tmpfinaly = itk->first.first;
						//update y 
						tmpfinaly = tmpfinaly + modInfo.y + dspshiftgap;
						//z
						int tmpfinalz = itk->first.second;

						//remove the resource in the architecture
						map<int, set<pair<int, int> > > ::iterator dspposit = dspPoslist.find(colCoordDspInArch[j]);
						assert(dspposit != dspPoslist.end());
						set<pair<int, int> > & tmpdspposlist = dspposit->second;
						pair<int, int> usedyz = make_pair(tmpfinaly, tmpfinalz);
						////////delete y z here  
						tmpdspposlist.erase(usedyz);	

						tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
					}

					//update x
					int finalx = colCoordDspInArch[j];
					map<int, map<pair<int,int>, string> >::iterator itd = mod.finalxyzblkname.find(finalx);
					assert(itd == mod.finalxyzblkname.end()); //should not exist before
					mod.finalxyzblkname.insert(make_pair(finalx, tmpfinalyzblks));
					j++;
				}

				//Dsp column not enough
				//if(colCoordDspInArch.size() < tmpdspcolset.size());
				if(tmpdspit != tmpdspcolset.end() && colCoordDspInArch.size() == j) 
				{  //place the unplaced DSP in the unused positions of dsp
					//unplace dsp   <--->   //unused place position
					for(; tmpdspit != tmpdspcolset.end(); tmpdspit++)
					{
						int tmpx = *tmpdspit;						
						map<int, map<pair<int,int>, string> >::iterator ith = tmpxyzblks.find(tmpx);
						assert(ith != tmpxyzblks.end());
						//update the y value (add the bottom-left coordinate of this module
						map<pair<int,int>, string> & tmpyzblks = ith->second;
						for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
							itk != tmpyzblks.end();
							itk++)
						{  
							string tmpblkname = itk->second;
							int tmpfinalx = -1;
							int tmpfinaly = -1;
							int tmpfinalz = -1;
							int findpos = 0; //0: do not find.   1: find a position for the RAM

							for(map<int, set<pair<int, int> > >::iterator ita = dspPoslist.begin();
								ita != dspPoslist.end();
								ita++)
							{
								tmpfinalx = ita->first; //tmpx  in the final arch (coordinate)
								set<pair<int, int> > & leftdspyz = ita->second;
								for(set<pair<int, int> >::iterator itb = leftdspyz.begin();
									itb != leftdspyz.end();
									itb++)
								{
									tmpfinaly = itb->first;
									tmpfinalz = itb->second;
									findpos = 1;
									leftdspyz.erase(itb);
								}

								if(1 == findpos)
								{
									break;
								}
								else
								{
									dspPoslist.erase(ita);
								}			
							}

							assert(1 == findpos); //at least find a position
							map<int, map<pair<int,int>, string> >::iterator itc = mod.finalxyzblkname.find(tmpfinalx);
							if(itc != mod.finalxyzblkname.end()) //should not exist before
							{
								itc->second.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
							}
							else
							{
								map<pair<int,int>, string> tmpfinalyzblks;
								tmpfinalyzblks.clear();
								tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
								mod.finalxyzblkname.insert(make_pair(tmpfinalx, tmpfinalyzblks));
							}
						}
					}
				}




				//place clb
				int leftgapinmodule = *(tmpdspcolset.begin()) - 1;	
				int leftgapinArch = *(colCoordDspInArch.begin())- startcol;
				if(leftgapinmodule >= leftgapinArch) //only need map clb one by one
				{
					int j = 0;
					assert(colCoordClbInArch.size() >= (tmpxyzblks.size() - tmpdspcolset.size()));
					for(map<int, map<pair<int,int>, string> >::iterator itb = tmpxyzblks.begin();
						itb != tmpxyzblks.end();
						itb++)
					{
						map<pair<int,int>, string> tmpfinalyzblks;
						tmpfinalyzblks.clear();

						int tmpx = itb->first;
						set<int>::iterator itc = tmpdspcolset.find(tmpx);
						if(itc != tmpdspcolset.end()) //it is ram, already solved
							continue;


						//update the y value (add the bottom-left coordinate of this module
						map<pair<int,int>, string> & tmpyzblks = itb->second;
						for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
							itk != tmpyzblks.end();
							itk++)
						{   //y
							int tmpfinaly = itk->first.first;
							//update y 
							tmpfinaly = tmpfinaly + modInfo.y;
							//z
							int tmpfinalz = itk->first.second;
							string tmpblkname = itk->second;
							tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
						}

						//update x
						assert(j < colCoordClbInArch.size());
						int finalx = colCoordClbInArch[j];
						map<int, map<pair<int,int>, string> >::iterator itd = mod.finalxyzblkname.find(finalx);
						assert(itd == mod.finalxyzblkname.end()); //must not exist before
						mod.finalxyzblkname.insert(make_pair(finalx, tmpfinalyzblks));
						j++;
					}
				}
				else  //leftgapinmodule < leftgapinArch
				{
					//find the start pos in vector
					//if the segment contains all the memory, it is ok.
					int numclbcol = tmpxyzblks.size() - tmpdspcolset.size();
					int startdspcol = colCoordDspInArch[0];
					int tmpdspnum = colCoordDspInArch.size();
					int enddspcol = colCoordDspInArch[tmpdspnum -1];
					int startposclb = 0; //colCoordRamInArch[startposclb]
					while(startposclb + numclbcol < colCoordClbInArch.size() && !(startdspcol > colCoordClbInArch[startposclb]  
					&& startdspcol < colCoordClbInArch[startposclb + numclbcol - 1]))
					{
						startposclb++;
					}

					for(map<int, map<pair<int,int>, string> >::iterator itb = tmpxyzblks.begin();
						itb != tmpxyzblks.end();
						itb++)
					{
						map<pair<int,int>, string> tmpfinalyzblks;
						tmpfinalyzblks.clear();
						int tmpx = itb->first;
						set<int>::iterator itc = tmpdspcolset.find(tmpx);
						if(itc != tmpdspcolset.end()) //it is dsp, already solved
							continue;

						//update the y value (add the bottom-left coordinate of this module
						map<pair<int,int>, string> & tmpyzblks = itb->second;
						for(map<pair<int,int>, string>::iterator itk = tmpyzblks.begin();
							itk != tmpyzblks.end();
							itk++)
						{   //y
							int tmpfinaly = itk->first.first;
							//update y 
							tmpfinaly = tmpfinaly + modInfo.y;
							//z
							int tmpfinalz = itk->first.second;
							string tmpblkname = itk->second;
							tmpfinalyzblks.insert(make_pair(make_pair(tmpfinaly, tmpfinalz), tmpblkname));
						}

						//update x
						assert(startposclb < colCoordClbInArch.size());
						int finalx = colCoordClbInArch[startposclb];
						map<int, map<pair<int,int>, string> >::iterator itd = mod.finalxyzblkname.find(finalx);
						assert(itd == mod.finalxyzblkname.end()); //should not exist before
						mod.finalxyzblkname.insert(make_pair(finalx, tmpfinalyzblks));
						startposclb++;
					}                 

				}
			} //if numdsp > 0

		}//only has ram or only has dsp
	} //for i = 0; i<=n
}


//for support hetero
void FPlan::getFinalPlaceFiles(char *file)
{
	ionameIopos.clear();
	filename = file;
	//finalplacefilename used to store the final result (the whole result)
	string finalplacefilename = filename + "/" + filename + ".place";
	string finalrouteNetfilename = filename + "/" + filename + ".routenet";
	string virtualpinfilename = filename + "/" + filename + ".virtualpins";

	//first clear the content in the file
	FILE *ff = fopen(finalplacefilename.c_str(), "w+");
	if(!ff)
	{
		cout<<"cannot open the final place file "<<finalplacefilename<<endl;
		exit(1);
	}
	fclose(ff);

	string subplacename;
	float ratio;
	int blkcount = 0;
	int ioblkcount = 0;
	int netcount = 0;
	int filenum = 0;
	string reconfigRegionfilename = filename + "/" + filename + ".configregion";
	FILE *ffp = fopen(reconfigRegionfilename.c_str(), "w+");
	if(!ffp) //remove the data stored before 
	{
		cout<<"cannot open the file "<<reconfigRegionfilename<<endl;
		exit(1);
	}
	fclose(ffp);

	ofstream reconfp(reconfigRegionfilename.c_str(), ios::app);
	if (!reconfp)
	{
		cout << "Unable to open " << reconfigRegionfilename << " for appending.\n";
		exit(1);
	}

	//nx =(int)Width, ny(int)Height    //width height may be need to add 1 resepctively for reserve enough resource 
	//should consider the memory block column

	int flagindex = 1;
	//int zflag = 0; //zflag [0..7]   now ch_intrinsic Io capacity is 8 should get from arch file

	//only keep one clock in the place file,  now only support one single global clock
	vector<string> clockSet;
	//if clkset.size is empty, the circuit is useless 
	if(clkset.size() > 0)
	{
		assert(1 == clkset.size());
		set<string>::iterator clkiter = clkset.begin();
		string clkname = (*clkiter);   //should accept or read the clk
		//	clockSet.push_back("top^clk");  //before  clock name also should be changed
		clockSet.push_back(clkname); 
	}
	else
	{
		cout<<"Warning: the circuit doesn't have clk, pls check!"<<endl;
	}

	string finalplaceinfoname = filename + "/" + filename + ".moduleplaceinfo";
	if(1)
	{
		FILE* fmoduleinfo = NULL;
		fmoduleinfo = fopen(finalplaceinfoname.c_str(),"w+");   //used to store new data,not a+
		if(NULL == fmoduleinfo)
		{
			printf("canot open testbug.txt,pls check\n");
			exit(1);
		}
		fclose(fmoduleinfo);
	}


	//add for loadable flow, for getting the whole place file
	//////string ioforfinalplacefile = filename + "/placefiles/" + filename + ".ioplace";
	//////FILE *ioforfinalplace = fopen(ioforfinalplacefile.c_str(), "w+");
	//////if(!ioforfinalplace)
	//////{
	//////	cout<<"cannot open the file "<<ioforfinalplacefile.c_str()<<endl;
	//////	exit(1);
	//////}
	//////fclose(ioforfinalplace);

	//each module corresponding to a place file
	for(int i = 0; i < modules_N; i++)
	{
		Module &mod = modules[i];
		//need to check whether module's id is right
		assert(mod.id == i);
		Module_Info &modInfo = modules_info[mod.id];
		subplacename = mod.name;
		//define a relationship between ration and path subscript can give a direct path
		ratio = mod.ratio;
		string tmpratio, sratio;  
		stringstream ss;
		ss.str(""); //release temp memory
		ss<<mod.ratio;
		tmpratio = ss.str();
		size_t pos = tmpratio.find(".");

		if(pos != string::npos)
		{
			sratio = tmpratio.substr(0, pos) + tmpratio.substr(pos+1, pos+2);  //need to check the code??
		}
		else
		{
			sratio = tmpratio + "0";
		}

		//first find the place file eg.
		//path
		string submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".v/"+ subplacename + ".place";
		string submoduleroutefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".v/" + subplacename + ".route";
		fs.open(submoduleplacefile.c_str()); //judge whether it exist and assign the name to the submoduleplacefile
		if(!fs)   //.v, .blif
		{   //notice the path
			printf("canot open the file %s", const_cast<char*>(submoduleplacefile.c_str()));
			submoduleplacefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".blif/"+ subplacename + ".place";
			submoduleroutefile = filename + "/run1/6k6_n8_" + sratio + ".xml/" + subplacename + ".blif/"+ subplacename + ".route";
			fs.close();
			fs.open(submoduleplacefile.c_str());
			if(!fs)
			{
				cout<<"cannot open the file:  "<<submoduleplacefile<<endl;
				exit(1);
			}
		}
		fs.close();
		char subpostfix[80];
		//need to check whether it converts right
		//	itoa(filenum,subpostfix, 10); 
		sprintf(subpostfix, "%d", filenum);  //

		cout<<i<<" "<<submoduleplacefile<<"   "<<sratio<<" "<<" "<<mod.width<<" "<<mod.height<<endl;

		//fbmao debug test
#if 1
		if(1)
		{
			FILE* fmoduleinfo = fopen(finalplaceinfoname.c_str(),"a+");
			if(NULL != fmoduleinfo)
			{
				fprintf(fmoduleinfo,"submodule %d submoduleplacefile %s sratio %s mod.width %d mod.height %d\n",
					i,submoduleplacefile.c_str(),sratio.c_str(),(int)mod.width,(int)mod.height);
			}
			else
			{
				printf("canot open testbug.txt,pls check\n");
				exit(1);
			}
			fclose(fmoduleinfo);
		}
#endif

		//add for control the pr region number
		//store reconfigurable region
		int tempprnum =  (int)(percentprnum * size());
		if(tempprnum == 0)
			tempprnum = 1;
		if((percentprnum * size()) > (tempprnum + 0.5))
		{
			tempprnum = tempprnum + 1;
		}

		if(1 == (int)size() || filenum < tempprnum)
		{
			reconfp<<modInfo.x<<"\t"<<modInfo.rx<<"\t"<<modInfo.y<<"\t"<<modInfo.ry<<"\t"<<filenum<<"\t"
				<<subplacename.c_str()<<"\t"<<1<<"\t"<<0<<endl; //should add one later and modified route code
			//add reconfigurablereion loadableregion at the end
		}
		else
		{
			reconfp<<modInfo.x<<"\t"<<modInfo.rx<<"\t"<<modInfo.y<<"\t"<<modInfo.ry<<"\t"<<filenum<<"\t"
				<<subplacename.c_str()<<"\t"<<0<<"\t"<<0<<endl;
		}




		//write blk info to file
		//also need to change the blkname according to the 1 <-> 1 blknamemap

		string archfile = "6k6_n8.xml";
		string newnetfilename = filename + ".net";
		FILE *ff = fopen(finalplacefilename.c_str(), "a+");
		if(!ff)
		{
			cout<<"cannot open the file "<<finalplacefilename<<endl;
			exit(1);
		}

		if(0 == blkcount)
		{    //filename like bgm
			//fprintf(ff, "Netlist file: %s   Architecture file: %s\n", newnetfilename.c_str(),
			//	archfile.c_str());
			fprintf(ff, "Netlist file: %s   Architecture file: %s\n", newnetfilename.c_str(),
				archname.c_str());
			fprintf(ff, "Array size: %d x %d logic blocks\n\n", (int)Width, (int)Height);
			fprintf(ff, "#block name\tx\ty\tsubblk\tblock number\n");
			fprintf(ff, "#----------\t--\t--\t------\t------------\n");

		}

		map<string, string>::iterator blknamemapit;
		for(map<int, map<pair<int, int>, string> >::iterator ita = mod.finalxyzblkname.begin();
			ita != mod.finalxyzblkname.end();
			ita++)
		{
			//col x coordinate
			int xcol = ita->first;

			map<pair<int, int>, string> &tmpyzblkname = ita->second;
			for(map<pair<int, int>, string>::iterator itb = tmpyzblkname.begin();
				itb != tmpyzblkname.end();
				itb++)
			{
				int yrow = itb->first.first;
				int zcoord = itb->first.second;
				string tempa = itb->second;

				string tempblkname;
				//tempblkname = tempa + "*" + subpostfix;
				tempblkname = tempa + "*" + subplacename;
				string tempmemoryblkname;
				blknamemapit = blksnamemap.find(tempblkname);
				if(blknamemapit != blksnamemap.end())
				{   //use a new name subsitude the original blk name
					tempmemoryblkname = blknamemapit->second;
				}
				else
				{
					cout<<"Error: blk name "<<tempmemoryblkname<<" doesn't exist in blknamemap list, pls check!"<<endl;
					exit(1);
				}

				fprintf(ff, "%s\t", tempmemoryblkname.c_str());
				if(strlen(tempmemoryblkname.c_str()) < 8)
					fprintf(ff, "\t");

				fprintf(ff, "%d\t%d\t%d", xcol, yrow, zcoord);
				fprintf(ff, "\t#%d\n", blkcount);
				blkcount++;
			}
		}
		fclose(ff);

		//IO initial coordinate
		map<string, map<float, map<string, Finaliopos> > >::iterator 
			itd = filenameRatioioBlknamecoord.find(subplacename);

		if(itd != filenameRatioioBlknamecoord.end())
		{
			map<float, map<string, Finaliopos> > &ratioioBlknamescoord = itd->second;
			map<float, map<string, Finaliopos> >::iterator ite = ratioioBlknamescoord.find(ratio);
			assert(ite != ratioioBlknamescoord.end());
			map<string, Finaliopos> &ioblknamescoord = ite->second;
			for(map<string, Finaliopos>::iterator itf = ioblknamescoord.begin();
				itf != ioblknamescoord.end();
				itf++)
			{
				string tmpioname = itf->first;
				int iox = itf->second.x;
				int ioy = itf->second.y;
				int ioz = itf->second.z;
				assignIOAccordOriginCoordinate(iox, ioy, ioz, tmpioname, mod, modInfo);
			}
		}
		filenum++;
	}
	reconfp.close();

	//move IO to final place file
	//Approach 1:  
	//play io block one by one along the boundary, add IO blocks in the final placement file
	//moveIOblksToFinalplacefile(finalplacefilename, blkcount, ioblkcount);


	if(0 == io3)   //default io3 : 0   //use the module center coordinate instead of using the module port's coordinate
	{
		//Approach2:
		moveIOblksToFinalplacefileConsiderOrigiposition(finalplacefilename, blkcount, ioblkcount);
	}
	else  //consider the module port's coordinate, when calculating wirelength
	{
		//approach3:   use the result generated by randomly perturbing IOs
		randomFinalIoblks();
		moveRandomIOblksToFinalPlacefile(finalplacefilename, blkcount, ioblkcount);
	}

	assert((int)finaliosets.size() == ioblkcount); //should change later delete the memory number
	//for debug
	if((int)finaliosets.size() != ioblkcount)
	{
		cout<<(int)finaliosets.size() <<"  "<<ioblkcount<<endl;
		exit(1);
	}
}


//Initially, Assign IO to different region according to their
void FPlan::assignIOAccordOriginCoordinate(
	int x, //iox
	int y, //ioy
	int z, //ioz
	string tempa, //ioname
	Module &mod,
	Module_Info &modInfo)
{    
	int nx = mod.width;
	int ny = mod.height;
	//using IO coordinate and the height, width of the floorplan result
	//it is io
	struct IOposition tempiopos;
	map<string, struct IOposition>::iterator ioposit;
	// map<string, struct IOposition> ionameIopos;  it will have 8 regions
	int tempiox = x + modInfo.x;
	int tempioy = y + modInfo.y;
	//each IO in original place, have four sides
	if(0 == x)
	{
		if(tempiox <= (int)(Width/2))
		{
			if(tempioy <= (int)(Height/2))
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.left1++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 1;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
			else
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.left2++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 1;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
		}
		else //(tempiox > (int)(Width/2))
		{
			if(tempioy <= (int)(Height/2))
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.bottom2++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 1;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
			else
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.top2++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 1;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
		}
	}
	else if(0 == y)
	{
		if(tempiox <= (int)(Width/2))
		{
			if(tempioy <= (int)(Height/2))
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.bottom1++;
				}
				else
				{
					tempiopos.bottom1 = 1;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
			else
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.left2++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 1;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
		}
		else //(tempiox > (int)(Width/2))
		{
			if(tempioy <= (int)(Height/2))
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.bottom2++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 1;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
			else
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.right2++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 1;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
		}
	}else if(nx+1 <= x)
	{
		if(tempiox <= (int)(Width/2))
		{
			if(tempioy <= (int)(Height/2))
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.bottom1++;
				}
				else
				{
					tempiopos.bottom1 = 1;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
			else
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.top1++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 1;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
		}
		else //(tempiox > (int)(Width/2))
		{
			if(tempioy <= (int)(Height/2))
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.right1++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 1;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
			else
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.right2++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 1;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
		}
	}else if(ny +1 <= y)  //since remove blank RAM and DSP
	{
		if(tempiox <= (int)(Width/2))
		{
			if(tempioy <= (int)(Height/2))
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.left1++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 1;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
			else
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.top1++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 1;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
		}
		else //(tempiox > (int)(Width/2))
		{
			if(tempioy <= (int)(Height/2))
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.right1++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 1;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 0;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
			else
			{
				ioposit = ionameIopos.find(tempa);
				if(ioposit != ionameIopos.end())
				{
					struct IOposition& temppos = ioposit->second;
					temppos.top2++;
				}
				else
				{
					tempiopos.bottom1 = 0;
					tempiopos.bottom2 = 0;
					tempiopos.left1 = 0;
					tempiopos.left2 = 0;
					tempiopos.right1 = 0;
					tempiopos.right2 = 0;
					tempiopos.top1 = 0;
					tempiopos.top2 = 1;
					ionameIopos.insert(make_pair(tempa, tempiopos));
				}
			}
		}
	}else
	{
		cout<<"IO position in the placement file is wrong,  x  "<<x<<" y "<<y<<endl;
		exit(1);
	}
}




struct coord
{  //((x,y), (rx,ry))
	int x;   
	int rx;
	int y;
	int ry;
};


void FPlan::collectReconfigRegion(char *filename, vector<struct coord> &regionSets)
{
	string tmpfilename = filename;
	string reconfigRegionfilename = tmpfilename + "/" + tmpfilename + ".configregion";
	ifstream fpconfig(reconfigRegionfilename.c_str());
	if(!fpconfig)
	{
		cout<<"unable to open file: "<<reconfigRegionfilename<<endl;
		exit(1);
	}

	int x, rx, y, ry;
	for(int i=0; !fpconfig.eof(); i++)
	{
		struct coord coords;
		fpconfig >> x >> rx >> y >> ry;
		coords.x = x; 
		coords.rx = rx;
		coords.y = y;
		coords.ry = ry;
		regionSets.push_back(coords);
	}
	return;
}

void FPlan::collectNetNames(char *filename, set<string> &netnameSet)
{
	string tmpfilename = filename;
	string  routnetfilename = tmpfilename + "/" + tmpfilename + ".routenet";

	FILE *routInfile;
	char **routetokens;
	int rline;
	routInfile = fopen(routnetfilename.c_str(), "r");
	if(!routInfile)
	{
		cout<<"cannot open file: "<<routnetfilename<<endl;
		exit(1);
	}

	routetokens = ReadLineTokens(routInfile, &rline);
	if(NULL == routetokens)
	{
		cout<<"bad line specified in the routefile "<<routnetfilename<<endl;
	}
	
	//reorder the net Id
	while(routetokens)
	{	
		int netid = atoi(routetokens[1]);
		string netname = routetokens[2];
		netnameSet.insert(netname);
		free(*routetokens);
	    free(routetokens);
		routetokens = ReadLineTokens(routInfile, &rline);
	}
	fclose(routInfile);
}


bool FPlan::checkoverlap()
{
	cout<<"check overlap begin"<<endl;
	cout<<"Total number of Blocks: "<<modules_N<<endl;
	for (int i = 0; i < modules_N; i++ ) {
		for (int j = i +1; j < modules_N; j++ ){
			//cout<<i<<" "<<j<<endl;
			if (i == j) 
				continue;
			else
			{
				double MinX, MaxX, MinY, MaxY;
				double x1_i, x2_i,y1_i,y2_i;
				double x1_j, x2_j,y1_j, y2_j;
				x1_i = modules_info[i].x;
				x2_i = modules_info[i].rx;
				y1_i = modules_info[i].y;
				y2_i = modules_info[i].ry;
				x1_j = modules_info[j].x;
				x2_j = modules_info[j].rx;
				y1_j = modules_info[j].y;
				y2_j = modules_info[j].ry;

				// (MinX,MinY) is the lower left point of the potential overlapped region
				//锟斤拷MaxX, MaxY锟斤拷is the upper right point of the potential overlapped region
				MinX = (x1_i > x1_j)?(x1_i):(x1_j);
				MinY = (y1_i > y1_j)?(y1_i):(y1_j);
				MaxX = (x2_i < x2_j)?(x2_i):(x2_j);
				MaxY = (y2_i < y2_j)?(y2_i):(y2_j);

				// if the lower left point is lower and be left to the upper right point, then overlep detected
				if ( (MinX -  MaxX < -0.01) && (MinY - MaxY < -0.01 )){
					cout << "Overlap detected between blocks " << i <<" and "<<j<<"!!"<<endl;
					cout<<"Check End : here are overlaps, pls check the coordinates"<<endl;
					return true;
				}
			}
		}
	}
	//if(getWidth() > outline_width || getHeight() > outline_height){
	//	cout<<"The floorplan is beyond the fix-outline boundary !"<<endl;
	//	cout<<"check overlap end"<<endl;
	//	return true;
	//}
	cout<<"check End: There is no overlap"<<endl;
	return false;
}



//---------------------------------------------------------------------------
//   Auxilliary Functions
//---------------------------------------------------------------------------

void error(char *msg,char *msg2/*=""*/)
{
  printf(msg,msg2);
  cout << endl;
  throw 1;
}

bool rand_bool()
{
	return bool( rand()%2 == 0 );
}

double rand_01()
{
  return double(rand()%10000)/10000;
}

double seconds()
{
	//rusage time; 
	//getrusage(RUSAGE_SELF,&time);
	//return (double)(1.0*time.ru_utime.tv_sec+0.000001*time.ru_utime.tv_usec);
	return (double)clock()/CLOCKS_PER_SEC;  
}


//*******************************************************************************************
//mao add 

void FPlan::creatPtr()
{
	//   designPtr->libPtr=SCMakeLib();
	//   designPtr->techPtr=SCMakeTech();
	//	designPtr->constrPtr = SCMakeConstr();
	//	designPtr->constrPtr->floorConstr = SCMakeFloor();

	designPtr=(MDesign *) malloc(sizeof(MDesign));
	if (designPtr == NULL)
	{
		printf("Memory overflow");
		exit(-1);
	}

	designPtr->cellPtr = NULL;
	designPtr->netNum = 0;
	designPtr->cellNum = 0;
	designPtr->cellPtr= (MCell *) malloc(sizeof(MCell));

	if (designPtr->cellPtr == NULL)
	{
		printf("Memory overflow");
		exit(-1);
	}
	designPtr->cellPtr->id = 0;
	designPtr->cellPtr->grg = NULL;

	//	designPtr=mfbMakeDesign();
	//	designPtr->cellPtr= mfbMakeCell();
	cellNum=size();
	designPtr->cellNum=cellNum;
	return;

}

void FPlan::freeglobal()
{

	for(int i=0;i< netsets.size();i++)
	{
		if(glbNets[i].required!=NULL)
		{
			free(glbNets[i].required);
		    glbNets[i].required=NULL;
		}
	}

	if (glbSegments!=NULL)
		free(glbSegments);
	glbSegments=NULL;

	if (glbNodes!=NULL)
		free(glbNodes);
	glbNodes=NULL;

	if (glbNets!=NULL)
		free(glbNets);
	glbNets=NULL;

	return;

}

void FPlan::freePtr()
{


	for( int row = 0; row < row_number; row++ )
		for( int col = 0; col < col_number - 1; col++ )
		{
			if(designPtr->cellPtr->grg->hSegMatrix[row][col]!=NULL)
			{
				free(designPtr->cellPtr->grg->hSegMatrix[row][col]);
				designPtr->cellPtr->grg->hSegMatrix[row][col]=NULL;
			}
		}


		for( int row = 0; row < row_number; row++ )
		{
			if(designPtr->cellPtr->grg->hSegMatrix[row]!=NULL)
			{

				free(designPtr->cellPtr->grg->hSegMatrix[row] );
				designPtr->cellPtr->grg->hSegMatrix[row] =NULL;
			}
		}


		if(designPtr->cellPtr->grg->hSegList!=NULL)
		{

			free(designPtr->cellPtr->grg->hSegList);
			designPtr->cellPtr->grg->hSegList=NULL;

		}




		if(designPtr->cellPtr->grg->hSegMatrix!=NULL)
		{
			free(designPtr->cellPtr->grg->hSegMatrix);
			designPtr->cellPtr->grg->hSegMatrix=NULL;
		}




		for( int row = 0; row < row_number - 1; row++ )
			for( int col = 0; col < col_number; col++ )
			{

				if(designPtr->cellPtr->grg->vSegMatrix[row][col]!=NULL)
				{
					free(designPtr->cellPtr->grg->vSegMatrix[row][col]);
					designPtr->cellPtr->grg->vSegMatrix[row][col]=NULL;
				}
			}



			for( int row = 0; row < row_number - 1; row++ )
			{

				if(designPtr->cellPtr->grg->vSegMatrix[row]!=NULL)
				{
					free(designPtr->cellPtr->grg->vSegMatrix[row]);
					designPtr->cellPtr->grg->vSegMatrix[row]=NULL;
				}
			}



			if(designPtr->cellPtr->grg->vSegList!=NULL)
			{
				free(designPtr->cellPtr->grg->vSegList);
				designPtr->cellPtr->grg->vSegList=NULL;

			}

			if(designPtr->cellPtr->grg->segList!=NULL)
			{
				free(designPtr->cellPtr->grg->segList);
				designPtr->cellPtr->grg->segList =NULL;
			}


			if(designPtr->cellPtr->grg->vSegMatrix!=NULL)
			{
				free(designPtr->cellPtr->grg->vSegMatrix);
				designPtr->cellPtr->grg->vSegMatrix=NULL;

			}

/*
			for( int row = 0; row < row_number; row++ )
				for( int col = 0; col < col_number; col++ )
				{
					if(designPtr->cellPtr->grg->nodeList[row*col_number + col]!=NULL)
					{
						designPtr->cellPtr->grg->nodeList[row*col_number + col]=NULL;
					}
				}
*/

				if(designPtr->cellPtr->grg->nodeList)
				{
					free(designPtr->cellPtr->grg->nodeList);
					designPtr->cellPtr->grg->nodeList=NULL;

				}

				for( int row = 0; row < row_number; row++ )
					for( int col = 0; col < col_number; col++ )
					{

						if(designPtr->cellPtr->grg->nodeMatrix[row][col]!=NULL)
						{
							free (designPtr->cellPtr->grg->nodeMatrix[row][col] );
							designPtr->cellPtr->grg->nodeMatrix[row][col] =NULL;
						}
					}


					for( int row = 0; row < row_number; row++ )
					{
						if(designPtr->cellPtr->grg->nodeMatrix[row]!=NULL)
						{
							free(designPtr->cellPtr->grg->nodeMatrix[row]);
							designPtr->cellPtr->grg->nodeMatrix[row]=NULL;
						}
					}

					if(designPtr->cellPtr->grg->nodeMatrix!=NULL)
					{
						free(designPtr->cellPtr->grg->nodeMatrix);
						designPtr->cellPtr->grg->nodeMatrix=NULL;
					}


					if(designPtr->cellPtr->grg!=NULL)
					{
						delete designPtr->cellPtr->grg;
						designPtr->cellPtr->grg=NULL;
					}

					if(designPtr->cellPtr!=NULL)
					{
						free(designPtr->cellPtr);
						designPtr->cellPtr=NULL;
					}

					if(designPtr->xMapList!=NULL)
					{
						free(designPtr->xMapList);
						designPtr->xMapList=NULL;
					}

					if(designPtr->yMapList!=NULL)
					{
						free(designPtr->yMapList);
						designPtr->yMapList=NULL;
					}



					if(designPtr!=NULL)
					{
						free(designPtr);
						designPtr=NULL;
					}

					return;
}

//void FPlan::getNetwork()
//{   
//	for(int i=0;i<netsets.size();i++)
//		netsets[i].clear();
//	Network.clear();
//	calcWireLength(); 
//	Network=network;  //得到网的信息 mao add     在calcWireLength前面加creatnetwork会出现问题，造成perturb
//	                  //后，得到的网一样。 实质perturb后，网上pin的绝对坐标会发生变化
//	return;
//}



//void subnetworksize()          //用于后面记算网中实际转化为GRG时有多少pin
//{
//	number.clear();
//   for(int i=0;i<Network.size();i++)
//	   number.push_back(Network[i].size());   
//   return;
//}

void FPlan::maplist(float &w, float &h)
{
	netnum=netsets.size();
	designPtr->netNum=netnum;
    
    r_number = row_number;
    c_number = col_number;

    designPtr->xMapList=(long *)malloc(sizeof(long)* (col_number+1));   //maplist以前从哪得到 8 *8 行列数
    designPtr->yMapList=(long *)malloc(sizeof(long)* (row_number+1));

    for(int j=0;j<col_number+1;j++)
       designPtr->xMapList[j]=j*(w/col_number);
    for(int i=0;i<row_number+1;i++)
       designPtr->yMapList[i]=i*(h/row_number);

	return;
}

void FPlan::GetRowColId( int x, int y, int & row, int & col, int & id )
{
	if( y <  designPtr->yMapList[0] )
	{
		cerr<<"Warning: pin y position is less than chip bottom!"<<endl;
		row = 0;
	}

	if( y >=  designPtr->yMapList[row_number] )
	{
	//	cerr<<"Warning: pin y position is larger than chip top!"<<endl;
		row = row_number - 1;
	}

	for( int i = 0; i < row_number; i++ )
	{
		if( (y >=  designPtr->yMapList[i]) && (y <  designPtr->yMapList[i+1]) )
		{
			row = i;
			break;
		}
	}

	if( x <  designPtr->xMapList[0] )
	{
		cerr<<"Warning: pin x position is less than chip left!"<<endl;
		col = 0;
	}

	if( x >= designPtr->xMapList[col_number] )
	{
	//	cerr<<"Warning: pin x position is larger than chip right!"<<endl;
		col = col_number - 1;
	}

	for( int i = 0; i < col_number; i++ )
	{
		if( (x >=  designPtr->xMapList[i]) && (x <  designPtr->xMapList[i+1]) )
		{
			col = i;
			break;
		}
	}

	id = 1 + row*col_number + col;
	return ;
}

void FPlan::creatNetworkRCol()
{
	vector<int> flag;
	for(int i=0;i<netsets.size();i++)
		flag.push_back(0);

	for(int i=0;i< netsets.size();i++)
		for(int j=0;j< netsets[i].size();j++)
		{
			GetRowColId( netsets[i][j].ax  , netsets[i][j].ay, netsets[i][j].row,netsets[i][j].col, netsets[i][j].vertex );
		}
		
		for(int i=0;i< netsets.size();i++)
		{ 
		   	int p=0;
			// 用于判断是否找到sink flag==0表明还没有找到sink       
             netsets[i][0].prop=0;
         for(int j=netsets[i].size()-1;(j>=1)&&(flag[i]==0);)  //标记其它点的prop flag[i]==0表明还没有找到线网i的sink
             if(netsets[i][j].vertex!=netsets[i][0].vertex)  
			 {
				 netsets[i][j].prop=1;
				 flag[i]=1;
				 p=j;

			 }
			 else 
			 {
			    netsets[i][j].prop=0;
		        numbersets[i]= numbersets[i]-1;
				j--;
			 }

//找到sink后标记其它的点的prop 
          if((flag[i]==1)&&(p>1))       //j是数组下标从0开始
             for(int q=1;q<p;q++)
			 {
			     if(netsets[i][q].vertex==netsets[i][0].vertex)			
				     netsets[i][q].prop=0;
				 else
					if(netsets[i][q].vertex==netsets[i][p].vertex)
					    netsets[i][q].prop=1;
					else
					   netsets[i][q].prop=-1;


                   if((netsets[i][q].vertex==netsets[i][0].vertex)||(netsets[i][q].vertex==netsets[i][p].vertex))
                       numbersets[i]= numbersets[i]-1;
				   else
                     for(int k=1; k<q;k++)
					    if(netsets[i][q].vertex==netsets[i][k].vertex)
                       numbersets[i]= numbersets[i]-1;                      
			  }
	      }

  flag.clear();

#if 0
FILE *fmao=fopen("networkflag.txt","a");

for(int i=0;i<Network.size();i++)
{
	 fprintf(fmao,"%d \n",number[i]);
	for(int j=0;j<Network[i].size();j++)
{
	fprintf( fmao,"%d %d %d  %d  %d  %d  %d  %d", Network[i][j].x , Network[i][j].y,Network[i][j].ax  , Network[i][j].ay, Network[i][j].row,Network[i][j].col, Network[i][j].vertex,Network[i][j].prop );
		  
	 fprintf(fmao,"\n");
}
 
}

fprintf(fmao,"-------------------------------------------------------\n");
fclose(fmao);	

#endif

   return;
}


int FPlan::Max(Grids  &chain)
{ 
  int p;
  float max=0;
  
  for(int i=0; i < chain.size();i++)
   { 
     if(chain[i].value>max)
        {
		max=chain[i].value; 
         p=i; 
        }
    }
   return p;
}


double FPlan::mfbmean(Grids  &chain)
{
  double sum=0;
  for(int i=0; i < chain.size();i++)
	  sum+= chain[i].value;
  return sum/chain.size();
}


double FPlan::mfbvar(Grids  &chain)
{
  double m = mfbmean(chain);
  double sum=0;
  double var;
  int N= chain.size();

  for(int i=0; i < N;i++)
	  sum += (chain[i].value-m)*(chain[i].value-m);

  var = sum/(N);
//  printf("  m=%.4f ,v=%.4f\n",m,var);

  return var;
}

int FPlan::getgridPosition()
{ 
   float W=getWidth();
   float H=getHeight();
  grids.resize(row_number*col_number);

//找出i与所在的行i列j的关系，从而确定坐标.i,j从0开始  确定矩形W*H划分后，每个单元格区域的坐标
for (int i =0;i< row_number*col_number; i++)        //（可由i确定行列关系，即可找到行，列与单元格的编号  (i行,j列单元格对应的网格编号i*col+j)，行、列都是从0开始计算）
 {
    grids[i].x1 = (W/col_number) * (i%col_number);
    grids[i].y1 = (H/row_number) * (i/col_number);
	grids[i].x2 = (W/col_number) * (i%col_number + 1);
    grids[i].y2 = (H/row_number) * (i/col_number + 1);
	grids[i].row = i/col_number;
	grids[i].col = i%col_number;
	grids[i].id = i;
	grids[i].value=0;
 }

return 1;
}


//=============================================================================================================
// congEsti.c中estiCongAllNets()函数里pEstiCongestionInfoOfGRG()之后计算congMatrix

// 用grids[i].value记录各拥塞值 ，并用max函数 求最大值标号 返回最大值标号及最大值 
// for(int i=0;i<grids.size();i++)
	//  grids[i].value=  以这个点为端点的segment相加  ，由i可得到网格的行列，从而可以计算出经过这个点的线段及相应的根据（行、列坐标）可求出segment对 应的坐标
//已知 glbsegment[i]->weight 求
 //   grids[i].value=glbsegment->weight;

//==========================================================================================================

void FPlan::getcongmartrix()  //coordinates2SegmentID列从1开始，grids列从0开始
{

   int    row=row_number;
   int    col=col_number;
   int i;
 for( i=0;i<grids.size();i++)
   if((i+1)%col==0)
   {
		 if(i==col-1)
		 {  
			 grids[i].value=glbSegments[coordinates2SegmentID(grids[i].col,grids[i].row,grids[i].col+1,grids[i].row)].segment->weight/2
				           +glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+1,grids[i].row+1)].segment->weight/2;
	         grids[i].value = grids[i].value/2;

		 }
		 else
			 if(i==row*col-1)
			 {
				 grids[i].value=glbSegments[coordinates2SegmentID(grids[i].col,grids[i].row,grids[i].col+1,grids[i].row)].segment->weight/2
					 +glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row-1,grids[i].col+1,grids[i].row)].segment->weight/2;
                 grids[i].value = grids[i].value/2;
			 }
			 else
			 {
				 grids[i].value=glbSegments[coordinates2SegmentID(grids[i].col,grids[i].row,grids[i].col+1,grids[i].row)].segment->weight/2
					 +glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row-1,grids[i].col+1,grids[i].row)].segment->weight/2
					 +glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+1,grids[i].row+1)].segment->weight/2;
	             grids[i].value = grids[i].value/2;
			 }
         
   }
	else
		if(i%col==0)
		{
		
	     	if(i==0)
			{
				grids[i].value=glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+2,grids[i].row)].segment->weight/2
					          +glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+1,grids[i].row+1)].segment->weight/2;
	            grids[i].value = grids[i].value/2;
			}
			else 
				if(i==row*col-col)
		    	{
					grids[i].value=glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row-1,grids[i].col+1,grids[i].row)].segment->weight/2
						+glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+2,grids[i].row)].segment->weight/2;
			        grids[i].value = grids[i].value/2;
		    	}
				else
				{
					grids[i].value=glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row-1,grids[i].col+1,grids[i].row)].segment->weight/2
						+glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+2,grids[i].row)].segment->weight/2
						+glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+1,grids[i].row+1)].segment->weight/2;
			        grids[i].value = grids[i].value/2;
				}
		}
		else 
			if(i>0&&i<col-1)
			{
				grids[i].value=glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+2,grids[i].row)].segment->weight/2
					+glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+1,grids[i].row+1)].segment->weight/2
					+glbSegments[coordinates2SegmentID(grids[i].col,grids[i].row,grids[i].col+1,grids[i].row)].segment->weight/2;
	            grids[i].value = grids[i].value/2;
			}
			else 
				if((i>row*col-col)&&i<row*col-1)
				{
					grids[i].value=glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row-1,grids[i].col+1,grids[i].row)].segment->weight/2
						+glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+2,grids[i].row)].segment->weight/2
						+glbSegments[coordinates2SegmentID(grids[i].col,grids[i].row,grids[i].col+1,grids[i].row)].segment->weight/2;
		  
                    grids[i].value = grids[i].value/2;
				}
				else
				{
					grids[i].value=glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row-1,grids[i].col+1,grids[i].row)].segment->weight/2
						+glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+2,grids[i].row)].segment->weight/2
						+glbSegments[coordinates2SegmentID(grids[i].col,grids[i].row,grids[i].col+1,grids[i].row)].segment->weight/2
						+glbSegments[coordinates2SegmentID(grids[i].col+1,grids[i].row,grids[i].col+1,grids[i].row+1)].segment->weight/2;
		  		    grids[i].value = grids[i].value/2;
				}


          return;
}