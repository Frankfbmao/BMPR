// Project: B*-trees floorplanning
// Advisor: Yao-Wen Chang  <ywchang@cis.nctu.edu.tw>
// Authors: Jer-Ming Hsu   <barz@cis.nctu.edu.tw>
// 	    Hsun-Cheng Lee <gis88526@cis.nctu.edu.tw>
// Sponsor: Arcadia Inc.
// Date:    7/19/2000 ~

// 2003/11/19	Modify perturb(), use one random number
//              Modify swap_node() and add swap_node2(), can swap parent and child
//              Modify delete_node(), place subtree with equal probability for left/right child

//---------------------------------------------------------------------------
#include<climits>
#include <stack>
#include <algorithm>
#include <iostream>

using namespace std;



#include "btree.h"
#include "RouteFrame.h"


//---------------------------------------------------------------------------
//double move_rate = 0.33;
//double swap_rate = 0.67;

//1. fbmao modified first
//double move_rate = 0.5;  //modified by fbmao close rotate in order to shift the .place file rightly

//2. then take it as a global variable and can use command to pass the parameter, see the fplan.h and main.cc for
//this variable

//double swap_rate = 1.0;
//-------------------------------------------------------------------------------------------------
//mao add 

float B_Tree::congestimate()
{

	grids.clear();

    float w = getWidth();
	float h = getHeight();

	maplist(w,h);
	getgridPosition();
//	cout<<"Network.size="<< Network.size(); 
//  cout<<"\ngrids.size="<<grids.size();

//	getNetwork(); 
//	subnetworksize();
	creatNetworkRCol();  

//    grid_moduleID();       //   进行增量操作才用此函数

	mfbRENewMain();
	RoutEstiMain(); //add mao
    getcongmartrix();
	freeglobal();
    

   
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

fprintf(fa,"--------------------------------------------------------------\n");
	fclose(fa);	 
#endif

   long  id=Max(grids);
   Congest= grids[id].value;
   return Congest;
}
//to check correctness of the tree
void B_Tree::Final_tree()
{
 

  FILE *tree = fopen( "Final_tree", "a" );

  fprintf( tree,"%d  %.3f   %.3f   %.3f  %d   %.3f\n",modules_N, Width, Height, Area,WireLength,getDeadSpace()); 

	for( int i=0; i<modules_N; i++ )
	{
		fprintf( tree,"RECT %s   %d   %d   %d   %d \n",
			modules[i].name,modules_info[i].x,modules_info[i].y,modules_info[i].rx,modules_info[i].ry );  //height
	}
	fprintf(tree,"-----\n");
    
	 fprintf(tree,"nodes_root = %d\n",nodes_root);

     for(int i=0; i < modules_N; i++)
	 {
	     fprintf(tree,"node[%d].id = %d  nodes[%d].parent= %d    nodes[%d].left=%d   nodes[%d].right= %d\n",i, nodes[i].id,i,nodes[i].parent,i,nodes[i].left,i,nodes[i].right);	 
	 }
    
    fprintf(tree,"--------------------------------------------------------------------\n");
	fclose(tree);

}



// above add by mao 

//---------------------------------------------------------------------------
//   Initialization
//---------------------------------------------------------------------------

void B_Tree::clear()
{
  // initial contour value
  contour_root = NIL;
  FPlan::clear();
}

void B_Tree::init()
{
  // initialize contour structure
  contour.resize(modules_N);
    
  // initialize b*tree by complete binary tree
  nodes.resize(modules_N);
  nodes_root=0;
  for(int i=0; i < modules_N; i++)
  {
    nodes[i].id = i;
    nodes[i].parent = (i+1)/2-1;
    nodes[i].left   = (2*i+1 < modules_N ? 2*i+1 : NIL);
    nodes[i].right  = (2*i+2 < modules_N ? 2*i+2 : NIL);
  }
  nodes[0].parent = NIL;
  best_sol.clear();
  last_sol.clear();
  clear();
  normalize_cost(30);
} 

//---------------------------------------------------------------------------
//   Testing, Debuging tools
//---------------------------------------------------------------------------

bool B_Tree::legal()
{
  int num=0;
  return legal_tree(NIL,nodes_root,num);
}

bool B_Tree::legal_tree(int p,int n,int &num)
{
  num++;
  if(nodes[n].parent!=p) return false;
  if(nodes[n].left != NIL)
    if(legal_tree(n,nodes[n].left,num) != true) return false;

  if(nodes[n].right != NIL)
    if(legal_tree(n,nodes[n].right,num) != true) return false;

  if(p==NIL) // root
    return (num==modules_N);
  return true;
}

void B_Tree::testing()
{
  int p,n;
  Solution E;

  do
  {

    n = rand()%modules_N;
    p = rand()%modules_N;

    while(n==nodes_root)		// n is not root
      n = rand()%modules_N;

    while(n==p||nodes[n].parent==p||nodes[p].parent==n)	// n != p & n.parent != p
      p = rand()%modules_N;
   
    Node &node = nodes[n];
    Node &parent = nodes[p];
    get_solution(E);
    swap_node(parent,node);

  }while(legal());

  cout << "p=" << p << ", n=" << n << endl;
  recover(E);
  show_tree();
  cout << "\n  p=" << p << ", n=" << n << endl;
  swap_node(nodes[p],nodes[n]);
  show_tree();
}

void B_Tree::show_tree()
{
  cout << "root: " << nodes_root << endl;
  for(int i=0; i < modules_N; i++)
  {
    cout << nodes[i].id << ": ";
    cout << nodes[i].left << " ";
    cout << nodes[i].parent << " ";
    cout << nodes[i].right << endl;
  }
}

//---------------------------------------------------------------------------
//   Placement modules
//---------------------------------------------------------------------------

void B_Tree::packing()
{
  stack<int> S;

  clear();
  int p = nodes_root;

  place_module(p,NIL);
  Node &n = nodes[p];
  if(n.right != NIL)      S.push(n.right);
  if(n.left  != NIL)      S.push(n.left);

  // inorder traverse
  while(!S.empty())
  {
    p = S.top();
    S.pop();
    Node &n = nodes[p];

    assert(n.parent != NIL);
    bool is_left = (nodes[n.parent].left == n.id);
    place_module(p,n.parent,is_left);

    if(n.right != NIL)      S.push(n.right);
    if(n.left  != NIL)      S.push(n.left);
  }

  // compute Width, Height
  double max_x=-1,max_y=-1;
  for(int p= contour_root; p != NIL; p=contour[p].front)
  {
    max_x = max(max_x,double(modules_info[p].rx));  
    max_y = max(max_y,double(modules_info[p].ry));
  }

  Width  = max_x;
  Height = max_y;
  Area   = Height*Width;       //chip area (include deadspace)

  TotalArea = 0;
  for(int i=0; i < modules_N; i++)
    TotalArea += modules[i].area;   //total area for all the modules

  creatNetworkOfModules();
  FPlan::packing(); 	// for wirelength  
  isRecoverSol = false;   //if packing multiple times, it will have an error in here

  //for congestion
  if(calcongest==true)
  {  
	 creatPtr();
     congestimate();
     freePtr();
  }


  //fbmao testbut
/*  if(true ==checkoverlap())
  {
	  cout<<"the modules are overlap, pls check";
	  exit(1);
  }
*/

}

// is_left: default is true  //modified 2012/12/14
void B_Tree::place_module(int mod,int abut,bool is_left)
{
  Module_Info &mod_mf = modules_info[mod];
  mod_mf.rotate       = nodes[mod].rotate;  //now is closed
  mod_mf.flip         = nodes[mod].flip;

  //if the solution is the recover solution or the recover best solution
  //donnot need to choose the ratio, or else the solution cannot recover
if(!isRecoverSol)
{
  //first choose a shape for module according to the area*dealy 
  //just choose the first three 
  if(modules[mod].aspec.size() == 0)
  {
	  cout<<"There is an aspect ratio error for module %d, pls check it"<<mod<<endl;
	  exit(1);
  }

  int id = 0;
  //may need to modify later.  consider half size, reduce time complexity
  //random choose a solution during packing
  if(1 == modules[mod].aspec.size())  //for hard module, only has one ratio
  {
	  id = 0;
  }
  else
  {
	  if(2 == (int)modules[mod].aspec.size())
	  {
		  id = rand()% ((int)modules[mod].aspec.size());
	  }
	  else
	  {
          id = rand() % (((int)modules[mod].aspec.size() + 1)/ 2) ;
	  }
  }
  float asperatio =  modules[mod].aspec[id];
  //shoule update the module info at the same time, in order to calculate cost for each one
  modules[mod].ratio = asperatio;
  map<float, pair<pair<int, int>, float> >::iterator it = modules[mod].aspeWidHeightDelay.find(asperatio);
  if(it != modules[mod].aspeWidHeightDelay.end())
  {
	  pair<pair<int, int>, float> &widHeightDelay = it->second;  
	  modules[mod].width = widHeightDelay.first.first;
	  modules[mod].height = widHeightDelay.first.second;
	  modules[mod].area = modules[mod].width * modules[mod].height;
	  modules[mod].delay = widHeightDelay.second;   //use to calculate delay cost (Intra_delayCost)
  }
  else
  {
	  cout<<"There is not elements in aspecAreaDelay map, pls check."<<endl;
  }
}

  int w =  modules[mod].width;
  int h =  modules[mod].height;
  if(nodes[mod].rotate)
    swap(w,h);
  
  if(abut==NIL)			// root node
  {
    contour_root = mod;
    contour[mod].back = NIL;
    contour[mod].front = NIL;
    mod_mf.x  = mod_mf.y = 0;
	mod_mf.rx = w, mod_mf.ry = h;

	//fbmao for supporting
	//	to check this module position
	//	1. whether contains heterogeneous block, if contain, whether the resource is enough?  if not reject (Or final step to reject the solution?? Each step must be legal??)
	//	2. if not contain . check whether it occupy memory or dsp column.  so it need give extra column to place it.
	//update ry to let the RAM, DSP in legal position  (they have position constraint)
	//update the rx of the module, to let the Region meet the resource requirement  (expand along x axis)
	checkArchResourceAndUpdateModuleCoordinate(mod, mod_mf.x, mod_mf.y, mod_mf.rx, mod_mf.ry);

    return;
  }
  
  int p;   // trace contour from p

  if(is_left)	// left
  {
    int abut_width = (nodes[abut].rotate ? modules[abut].height : 
                                           modules[abut].width);

  ///fbmao. Notice  rx may != x + width  (rx-x  virtual width: region width)	
 // mod_mf.x  = modules_info[abut].x + abut_width;  //original 
    mod_mf.x  = modules_info[abut].rx;    //fbmao change for supporting heterogeneous blocks. module has expanded

	mod_mf.rx = mod_mf.x + w;

	p = contour[abut].front;

    contour[abut].front = mod;
    contour[mod].back = abut;

    if(p==NIL)  // no obstacle in X axis
	{
      mod_mf.y = 0;
      mod_mf.ry = h;
	  contour[mod].front = NIL;

	  //left child update
	  //fbmao for supporting 
	  //update the rx of the module, to let the Region meet the resource requirement  (expand along x axis)
	  checkArchResourceAndUpdateModuleCoordinate(mod, mod_mf.x, mod_mf.y, mod_mf.rx, mod_mf.ry);

      return;
    }
  }
  else
  {
	// upper
    mod_mf.x = modules_info[abut].x;
    mod_mf.rx = mod_mf.x + w;

    p = abut;
     
    int n=contour[abut].back;

    if(n==NIL)
	{ // i.e, mod_mf.x==0
      contour_root = mod;
      contour[mod].back = NIL;
    }
    else
	{
      contour[n].front = mod;
      contour[mod].back = n;
    }
  }
  
  int min_y = INT_MIN;
  int bx,by;
  assert(p!=NIL);
    
  for(; p!=NIL ; p=contour[p].front)
  {
    bx = modules_info[p].rx;
    by = modules_info[p].ry;
    min_y = max(min_y, by);
      
    if(bx >= mod_mf.rx)			 	// update contour
	{
      //fbmao add for support expand x, y
      int originalmod_mfy = mod_mf.y;
	  int originalmod_mfry = mod_mf.ry;
	  int originalmodmfrx = mod_mf.rx;

      mod_mf.y = min_y;
	  mod_mf.ry = mod_mf.y + h;

	  //right child update
	  //fbmao for supporting 
	  //update the rx of the module, to let the Region meet the resource requirement  (expand along x axis)
	  checkArchResourceAndUpdateModuleCoordinate(mod, mod_mf.x, mod_mf.y, mod_mf.rx, mod_mf.ry);
	  
	  //judge whether it is ok
	  if(mod_mf.rx > bx)
	  { //this ifsentence cannot be executed
        //redo and skip
         mod_mf.y = originalmod_mfy;
		 mod_mf.ry = originalmod_mfry;
		 mod_mf.rx = originalmodmfrx; //mod_mf donnot change
		 continue;
	  }



      if(bx > mod_mf.rx)
	  {
        contour[mod].front = p;
        contour[p].back = mod;
      }
	  else							// bx==mod_mf.rx
	  { 
        int n= contour[p].front;
        contour[mod].front = n;
        if(n!=NIL)
          contour[n].back = mod;
      }
      break;     
    }
  }

  if(p==NIL)
  {
    mod_mf.y  = (min_y==INT_MIN? 0 : min_y);
	mod_mf.ry = mod_mf.y + h;

	//right child update
	//fbmao for supporting 
	//update the rx of the module, to let the Region meet the resource requirement  (expand along x axis)
	checkArchResourceAndUpdateModuleCoordinate(mod, mod_mf.x, mod_mf.y, mod_mf.rx, mod_mf.ry);

	contour[mod].front = NIL;
  }
}




//fbmao for supporting 
//update the rx of the module, to let the Region meet the resource requirement  (expand along x axis)
//	need to change the height to ry-> for tracing the contour structure
void B_Tree::checkArchResourceAndUpdateModuleCoordinate(int modid, int &x, int &y, int &rx, int &ry)
{
		//use for expand  x column.  some ram and dsp used the CLB's positions
		int numcolforraminmodule = 0;  //the number of column for contains "RAM" in module
		int numcolfordspinmodule = 0;  //the "dsp"
		map<string, map<float, pair<int, int> > >::iterator fileratioramdspcolit 
			= filenameRatioramdspcolnum.find(modules[modid].name);
		assert(fileratioramdspcolit != filenameRatioramdspcolnum.end());
		map<float, pair<int, int> > &tmpratioramdsp = fileratioramdspcolit->second;
		map<float, pair<int, int> >::iterator tmprtaioramdspit = tmpratioramdsp.find(modules[modid].ratio);
		assert(tmprtaioramdspit != tmpratioramdsp.end());
		numcolforraminmodule = tmprtaioramdspit->second.first;
		numcolfordspinmodule = tmprtaioramdspit->second.second;

	//In y coordinate, RAM position start from 1   
	//In y coordinate, DSP position start from 1 (one can place in position 1, 1+repeatram, 1+2*repeatram ...
	//need to check whether RAM, DSP in legal position (RAM, DSP): start position and their height
	//Note: need to change ry (ry may != y+height) accroding to the RAM y position 
	//and Their y coordinate. finally (y becomes "y+1", if RAM position is wrong)

	//Notice: ry need to change. since the RAM, DSP has specified postion. means,only specific position is ok.
	//vertical height of the module
	//int verticalH = modules[modid].height; // does not equal to ry - y; initial value. may update later

	//first convert the region coordinates of modules to the column and row coordinate of the archiecture
	int archx = x + 1;  //col -> correspond to the fpga arch coordinate
	int archy = y + 1; //row.  module (lowest_y_coordinate + 1)

	//info get from architecture
	int memcount = 0;   //how many mem col that the module occupy
	int tmpmemcol = fmemstart;  //col position of memory
	int ramlegalstartrow = 1;   //the minimum start row for placing RAM

	int multiplycount = 0;   //how many multiply col that the module occupy
	int tmpmultiplycol = fmultistart; //col position of multiply
	int dsplegalstartrow = 1;  //the minimum start row for placing DSP


	//find the col for memory (larger than rx)
	while(tmpmemcol <= rx)
	{
		if(tmpmemcol >= archx && tmpmemcol <= rx)
		{
			memcount++;
		}
		tmpmemcol += fmemrepeat;
	}

	//find the col for multiply (larger than rx)
	while(tmpmultiplycol <= rx)
	{
		if(tmpmultiplycol >= archx && tmpmultiplycol <= rx)
		{
			multiplycount++;
		}
		tmpmultiplycol += fmultirepeat;
	}


	//used for update module' verticalH. since the RAM (or DSP) start point must be legal
	//if the position is not enough. then expand along the x axis
	int ramshiftgap = 0; //move ramshiftgap along y axis to make the RAM start position legal
	int dspshiftgap = 0; //move dspshiftgap along y axis to make the DSP start postion legal

	//first check whether this module is heterogeneous module(containing RAM,DSP,etc.)
	set<int>::iterator ita = fheteromodidset.find(modid);
	if(ita != fheteromodidset.end()) //1.module with heterogeous block.such as RAM, DSP.
	{
		int extramemcol = 0;  //memory and DSP need to add extra column for clb.
		int extramultiplycol = 0;

		//check whether it occupy (memory's) column, if not. expand
		int tmpmemrx = rx;  //the temp position for rx  related to memory
		int tmpmultirx = rx; //the temp postion for rx related to multiply

		int tmpnummem = 0;  //the number of available position for mem
		int tmpnummultiply = 0;  //for multiply


		//update veritcalH
		//first judge whether the start point for placing RAM and DSP is legal or not?
		//1> find the gap for shift ram to legal position
		int tmpfirstRamlegalpos = ramlegalstartrow;      //find gap can be better??? 
		while(tmpfirstRamlegalpos < archy) //archy is (module's lowest coordinate + 1)
		{
			tmpfirstRamlegalpos = tmpfirstRamlegalpos + fmemoryheight;
		}

		ramshiftgap = tmpfirstRamlegalpos - archy;

		int tmpfirstDsplegalpos = dsplegalstartrow;
		while(tmpfirstDsplegalpos < archy) //archy is (module's lowest coordinate + 1)
		{
			tmpfirstDsplegalpos = tmpfirstDsplegalpos + fmultiheight;
		}

		dspshiftgap = tmpfirstDsplegalpos - archy;

		int dspramshifgap = 0;
		int ramnumposforeachcol = 0;
		int dspnumposforeachcol = 0;

		assert(modules[modid].numram > 0 || modules[modid].numdsp > 0);
		//update rx and calculate the number of positions in each column for placing RAM and DSP
		if(modules[modid].numram > 0 && modules[modid].numdsp > 0)
		{    //choose the large value
			if(ramshiftgap >= dspshiftgap)
			{
				dspramshifgap = ramshiftgap;
			}
			else 
			{
				dspramshifgap = dspshiftgap;
			}
		}
		else if(modules[modid].numram > 0)  //only has ram, so dsp does not have position constraint
		{   //donnot need to consider the DSP position constraint
			dspramshifgap = ramshiftgap;
		} 
		else if(modules[modid].numdsp > 0)
		{   //does not consider the ram position constraint
			dspramshifgap = dspshiftgap;
		}

		//update ry
		ry = ry + dspramshifgap; //expand ry to make sure the RAM in the legal position

		int modstartrow = archy;
		int	modendrow  = ry; 
		while(modstartrow <= modendrow)
		{   //collect the available positions for placing RAM or DSP in each col (within the module region)
			if(modstartrow >= ramlegalstartrow && 0 == (modstartrow - ramlegalstartrow) % fmemoryheight
				&& modstartrow + fmemoryheight - 1 <= modendrow) //cannot beyond the region boudary
			{
				ramnumposforeachcol++; ////cannot use (verticalH / fmemoryheight), since memory start pos constraint
			}

			//dsp
			if(modstartrow >= dsplegalstartrow && 0 == (modstartrow - dsplegalstartrow) % fmultiheight
				&& modstartrow + fmultiheight - 1 <= modendrow)
			{
				dspnumposforeachcol++;
			}

			modstartrow++;
		}

		if(modules[modid].numram > 0 && modules[modid].numdsp > 0) //module contains memory and DSP
		{    //I.xxx
			//compare the number of cols for RAM, DSP. (may do not have solution, during SA process. Finally, become feasible.)
			assert(ramnumposforeachcol >= 0 && dspnumposforeachcol >= 0); //at least can place one RAM and DSP

			if(memcount > 0 && multiplycount >0) //module occupy memeory column
			{
				tmpnummem = memcount * ramnumposforeachcol; //the available memory (number) 
				if(tmpnummem < modules[modid].numram)  //not enough, need to expand
				{
					assert(ramnumposforeachcol > 0);
					extramemcol = (modules[modid].numram - tmpnummem) / ramnumposforeachcol;
					if(0 == extramemcol) //eg. 2/3 = 0. but still need at least one column for memory
						extramemcol = 1; 

					tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;
				}

				tmpnummultiply = multiplycount * dspnumposforeachcol; ///////(verticalH / fmultiheight);
				if(tmpnummultiply < modules[modid].numdsp)
				{
					assert(dspnumposforeachcol > 0);
					extramultiplycol = (modules[modid].numdsp - tmpnummultiply) / dspnumposforeachcol;
					if(0 == extramultiplycol)
						extramultiplycol = 1;

					tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;
				}


				if(tmpmemrx >= tmpmultirx)
				{
					rx = tmpmemrx;
				}
				else
				{
					rx = tmpmultirx;
				}
			}
			else if(memcount > 0)  //module not occupy dsp col in the arch. Need to expand
			{  
				//first check whether memeory is enough.
				tmpnummem = memcount * ramnumposforeachcol;
				if(tmpnummem < modules[modid].numram)
				{
					assert(ramnumposforeachcol > 0);
					extramemcol = (modules[modid].numram - tmpnummem) / ramnumposforeachcol;
					if(0 == extramemcol) //eg. 2/3 = 0. but still need at least one column for memory
						extramemcol = 1; 

					tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;
				}

				assert(dspnumposforeachcol > 0);
				extramultiplycol = modules[modid].numdsp / dspnumposforeachcol;
				if(0 == extramultiplycol)
					extramultiplycol = 1;

				tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;

				if(tmpmemrx >= tmpmultirx)
				{
					rx = tmpmemrx;
				}
				else
				{
					rx = tmpmultirx;
				}
			}
			else if(multiplycount > 0) //module do not occupy mem column. expand to contian memory resource
			{
				//first check whether multiply enough?
				tmpnummultiply = multiplycount * dspnumposforeachcol;
				if(tmpnummultiply < modules[modid].numdsp)
				{
					assert(dspnumposforeachcol > 0);
					extramultiplycol = (modules[modid].numdsp - tmpnummultiply) / dspnumposforeachcol;

					if(0 == extramultiplycol)
						extramultiplycol = 1;

					tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;
				}

				//to get memory
				assert(ramnumposforeachcol > 0);
				extramemcol = modules[modid].numram / ramnumposforeachcol;
				if(0 == extramemcol)
					extramemcol = 1;

				tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;
				if(tmpmemrx >= tmpmultirx)
				{
					rx = tmpmemrx;
				}
				else
				{
					rx = tmpmultirx;
				}
			}
			else    //this module donot occupy any columns of RAM or DSP.
			{
				//expand for memory
				assert(ramnumposforeachcol > 0);
				extramemcol = modules[modid].numram / ramnumposforeachcol;
				if(0 == extramemcol)
					extramemcol = 1;

				tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;

				//expand for multiply
				assert(dspnumposforeachcol > 0);
				extramultiplycol = modules[modid].numdsp / dspnumposforeachcol;
				if(0 == extramultiplycol)
					extramultiplycol = 1;

				tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;

				if(tmpmemrx >= tmpmultirx)
				{
					rx = tmpmemrx;
				}
				else
				{
					rx = tmpmultirx;
				}
			}
		}  ////module contains memory and DSP

		else if(modules[modid].numram > 0)  //module only has ram, donot have dsp
		{    //II.xxxxx
			//check whether ram is enough
			if(memcount > 0)  //modules' region occupy (memory column) of the architecture
			{
				//first check whether the momory number is enough
				tmpnummem = memcount * ramnumposforeachcol;
				if(tmpnummem < modules[modid].numram)
				{
					assert(ramnumposforeachcol > 0);
					extramemcol = (modules[modid].numram - tmpnummem) / ramnumposforeachcol;

					if(0 == extramemcol)
						extramemcol = 1;

					tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;
				}


				if(0 == multiplycount)
				{
					rx = tmpmemrx;
				}
				else
				{
					if(rx == tmpmemrx) //if ram is enough
					{
						int tmpmultiplycount = multiplycount;
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							int tmpmemendpos = rx + tmpmultiplycount;
							while(tmpmempos <= tmpmemendpos)  //loop endless????bug
							{
								if(tmpmempos > rx && tmpmempos <= tmpmemendpos)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							int tmpmultiplyendpos = rx + tmpmultiplycount;
							while(tmpmultiplypos <= tmpmultiplyendpos)
							{
								if(tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							//if it has enough position for clbs
							//gap: multiplycount
							if(tmpmultiplycount - multiplycount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
							{
								break;
							}
							else
								tmpmultiplycount++;
						}while(1);

						//here, we add multiplycount, since, the multiply column will use the column that for CLB. We need to expand
						rx = tmpmemrx + tmpmultiplycount;   //multiply coloum used the clb, then add an extra column
					}
					else //tmpmemrx > rx.  has alread expand. now check whether the gap. tmpmemrx - rx - 1 has enough position for clbs. 
					{    //may no need to add the multiplycount
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							while(tmpmempos <= tmpmemrx)
							{
								if(tmpmempos > rx && tmpmempos <= tmpmemrx)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							while(tmpmultiplypos <= tmpmemrx)
							{
								if(tmpmultiplypos > rx && tmpmultiplypos <= tmpmemrx)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							//minus 1 : because current the memory or dsp column cannot be used for placing clb
							if((tmpmemrx - rx - 1) - tmpmemexpandcount - tmpmultiplyexpandcount - multiplycount >= 0)//if it has enough position for clbs
							{
								break;
							}
							else
								tmpmemrx++;
						}while(1);

						rx = tmpmemrx;
					}//rx != tmpmemrx
				}//	if(0 != multiplycount)
			}
			else //module donot occupy memory column
			{
				//to get memory
				assert(ramnumposforeachcol > 0);
				extramemcol = modules[modid].numram / ramnumposforeachcol;
				if(0 == extramemcol)
					extramemcol = 1;

				tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;

				if(0 == multiplycount)
				{
					rx = tmpmemrx;
				}
				else
				{
					if(rx == tmpmemrx) //if ram is enough
					{
						int tmpmultiplycount = multiplycount;
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							int tmpmemendpos = rx + tmpmultiplycount;
							while(tmpmempos <= tmpmemendpos)
							{
								if(tmpmempos > rx && tmpmempos <= tmpmemendpos)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							int tmpmultiplyendpos = rx + tmpmultiplycount;
							while(tmpmultiplypos <= tmpmultiplyendpos)
							{
								if(tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							//if it has enough position for clbs
							if(tmpmultiplycount - multiplycount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
							{
								break;
							}
							else
								tmpmultiplycount++;
						}while(1);

						//here, we add multiplycount, since, the multiply column will use the column that for CLB. We need to expand
						rx = tmpmemrx + tmpmultiplycount;   //multiply coloum used the clb, then add an extra column
					}
					else //tmpmemrx > rx.  has alread expand. now check whether the gap. tmpmemrx - rx - 1 has enough position for clbs. 
					{    //may no need to add the multiplycount
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							while(tmpmempos <= tmpmemrx)
							{
								if(tmpmempos > rx && tmpmempos <= tmpmemrx)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							while(tmpmultiplypos <= tmpmemrx)
							{
								if(tmpmultiplypos > rx && tmpmultiplypos <= tmpmemrx)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							if((tmpmemrx - rx - 1) - tmpmemexpandcount - tmpmultiplyexpandcount - multiplycount >= 0)//if it has enough position for clbs
							{
								break;
							}
							else
								tmpmemrx++;
						}while(1);

						rx = tmpmemrx;
					}
				}
			}
		}

		else   //numdsp > 0	
		{   //III.xxxx.
			if(multiplycount > 0) //occupy multiply column
			{
				//first check whether multiply enough?
				tmpnummultiply = multiplycount * dspnumposforeachcol;
				if(tmpnummultiply < modules[modid].numdsp)
				{
					assert(dspnumposforeachcol > 0);
					extramultiplycol = (modules[modid].numdsp - tmpnummultiply) / dspnumposforeachcol;
					if(0 == extramultiplycol)
						extramultiplycol = 1;

					tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;
				}


				if(0 == memcount)
				{
					rx = tmpmultirx;
				}
				else
				{
					if(rx == tmpmultirx) //if multiply is enough
					{
						int tmpmemcount = memcount;
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							int tmpmemendpos = rx + tmpmemcount;
							while(tmpmempos <= tmpmemendpos)
							{
								if(tmpmempos > rx && tmpmempos <= tmpmemendpos)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							int tmpmultiplyendpos = rx + tmpmemcount;
							while(tmpmultiplypos <= tmpmultiplyendpos)
							{
								if(tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							//if it has enough position for clbs
							if(tmpmemcount - memcount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
							{
								break;
							}
							else
								tmpmemcount++;
						}while(1);

						//here, we add multiplycount, since, the multiply column will use the column that for CLB. We need to expand
						rx = tmpmultirx + tmpmemcount;   //multiply coloum used the clb, then add an extra column
					}
					else //tmpmultirx > rx.  has alread expand. now check whether the gap. tmpmultirx - rx - 1 has enough position for clbs. 
					{    //may no need to add the multiplycount
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							while(tmpmempos <= tmpmultirx)
							{
								if(tmpmempos > rx && tmpmempos <= tmpmultirx)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							while(tmpmultiplypos <= tmpmultirx)
							{
								if(tmpmultiplypos > rx && tmpmultiplypos <= tmpmultirx)
								{
									tmpmultiplyexpandcount++;							
								}
								tmpmultiplypos += fmultirepeat;
							}

							if((tmpmultirx - rx - 1) - tmpmemexpandcount - tmpmultiplyexpandcount - memcount >= 0)//if it has enough position for clbs
							{
								break;
							}
							else
								tmpmultirx++;
						}while(1);

						rx = tmpmultirx;
					}
				}
			}
			else  //module do not occupy dspcolumn
			{
				assert(dspnumposforeachcol > 0);
				extramultiplycol = modules[modid].numdsp / dspnumposforeachcol;
				if(0 == extramultiplycol)
					extramultiplycol = 1;

				tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;

				if(0 == memcount)
				{
					rx = tmpmultirx;
				}
				else
				{
					if(rx == tmpmultirx) //if multiply is enough
					{
						int tmpmemcount = memcount;
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							int tmpmemendpos = rx + tmpmemcount; //memory number > 0
							while(tmpmempos <= tmpmemendpos)
							{
								if(tmpmempos > rx && tmpmempos <= tmpmemendpos)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							int tmpmultiplyendpos = rx + tmpmemcount;
							while(tmpmultiplypos <= tmpmultiplyendpos)
							{
								if(tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							//if it has enough position for clbs
							if(tmpmemcount - memcount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
							{
								break;
							}
							else
								tmpmemcount++;
						}while(1);

						//here, we add multiplycount, since, the multiply column will use the column that for CLB. We need to expand
						rx = tmpmultirx + tmpmemcount;   //multiply coloum used the clb, then add an extra column
					}
					else //tmpmemrx > rx.  has alread expand. now check whether the gap. tmpmemrx - rx - 1 has enough position for clbs. 
					{    //may no need to add the multiplycount
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							while(tmpmempos <= tmpmultirx)
							{
								if(tmpmempos > rx && tmpmempos <= tmpmultirx)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							while(tmpmultiplypos <= tmpmultirx)
							{
								if(tmpmultiplypos > rx && tmpmultiplypos <= tmpmultirx)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							if((tmpmultirx - rx - 1) - tmpmemexpandcount - tmpmultiplyexpandcount - memcount>= 0)//if it has enough position for clbs
							{
								break;
							}
							else
								tmpmultirx++;
						}while(1);

						rx = tmpmultirx;
					}
				}
			}
		}


		//expand the rx for placing CLB.   (numcolforraminmodule: the col already contain RAM within module)
		//because sometimes numcolforraminmodule > memcount or numcolfordspinmodule > dspcount
		//expand
		int expand1 = 0;  //ramexpand
		int expand2 = 0;  //dspexpand
		if(modules[modid].numram > 0 && modules[modid].numdsp > 0)
		{
			if(memcount > 0 && multiplycount > 0)
			{
				if(memcount > numcolforraminmodule)
				{
					expand1 = memcount - numcolforraminmodule;
				}

				if(multiplycount > numcolfordspinmodule)
				{
					expand2 = multiplycount - numcolfordspinmodule;
				}
			}
			else if(memcount > 0)
			{
				if(memcount > numcolforraminmodule)
				{
					expand1 = memcount - numcolforraminmodule;
				}
			}
			else if(multiplycount > 0)
			{
				if(multiplycount > numcolfordspinmodule)
				{
					expand2 = multiplycount - numcolfordspinmodule;
				}
			}
		}
		else if(modules[modid].numram > 0)
		{
			if(memcount > numcolforraminmodule)
			{
				expand1 = memcount - numcolforraminmodule;
			}
		}
		else if( modules[modid].numdsp > 0)
		{
			expand2 = multiplycount - numcolfordspinmodule;
		}

		int extraRamDspCol = expand1 + expand2;
		if(extraRamDspCol > 0)
		{
			do
			{
				int tmpmempos = fmemstart;
				int tmpmemexpandcount = 0;

				int tmpmultiplypos = fmultistart;
				int tmpmultiplyexpandcount = 0;

				int tmpmemendpos = rx + extraRamDspCol; //memory number > 0
				while(tmpmempos <= tmpmemendpos)
				{
					if(tmpmempos > rx && tmpmempos <= tmpmemendpos)
					{
						tmpmemexpandcount++;
					}
					tmpmempos += fmemrepeat;
				}

				int tmpmultiplyendpos = rx + extraRamDspCol;
				while(tmpmultiplypos <= tmpmultiplyendpos)
				{
					if(tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
					{
						tmpmultiplyexpandcount++;
					}
					tmpmultiplypos += fmultirepeat;
				}

				//if it has enough position for clbs
				if(extraRamDspCol - memcount - multiplycount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
				{
					break;
				}
				else
					extraRamDspCol++;
			}while(1);
			rx = rx + extraRamDspCol;//expand the rx
		}

	}
	else  //2.module does not contain any RAM or DSP
	{
		//if module occupy the column of RAM or DSP. add an extra column for this module. Means Expand module. rx become large
		//update rx  

		//(Possible bug(do not have enough place to place).  If bug generate, using the code above can solve this problem )

		//need change the code to make it corret.
		//cout<<"Possible bug.  need change the code to make it corret."<<endl;

		int extraRamDspCol = memcount + multiplycount;
		do
		{
			int tmpmempos = fmemstart;
			int tmpmemexpandcount = 0;

			int tmpmultiplypos = fmultistart;
			int tmpmultiplyexpandcount = 0;

			int tmpmemendpos = rx + extraRamDspCol; //memory number > 0
			while(tmpmempos <= tmpmemendpos)
			{
				if(tmpmempos > rx && tmpmempos <= tmpmemendpos)
				{
					tmpmemexpandcount++;
				}
				tmpmempos += fmemrepeat;
			}

			int tmpmultiplyendpos = rx + extraRamDspCol;
			while(tmpmultiplypos <= tmpmultiplyendpos)
			{
				if(tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
				{
					tmpmultiplyexpandcount++;
				}
				tmpmultiplypos += fmultirepeat;
			}

			//if it has enough position for clbs
			if(extraRamDspCol - memcount - multiplycount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
			{
				break;
			}
			else
				extraRamDspCol++;
		}while(1);

		rx = rx + extraRamDspCol;//expand the rx
	}
}





//---------------------------------------------------------------------------
//   Manipulate B*Tree auxilary procedure
//---------------------------------------------------------------------------

// place child in parent's edge
void B_Tree::wire_nodes( int parent, int child, DIR edge )		
{
	assert(parent!=NIL);
	(edge == LEFT ? nodes[parent].left : nodes[parent].right) = child;
	if(child!=NIL) 
		nodes[child].parent = nodes[parent].id;
}

// get node's child
inline int B_Tree::child( int node, DIR d )		
{
	assert( node!=NIL );
	return ( d==LEFT ? nodes[node].left : nodes[node].right);  
}


//---------------------------------------------------------------------------
//   Simulated Annealing Temporal Solution
//---------------------------------------------------------------------------

void B_Tree::get_solution(Solution &sol)
{
  sol.nodes_root = nodes_root;
  sol.nodes = nodes;
  sol.cost = getCost();
  sol.recoverInfo.resize(modules_N);
  for(int i = 0; i < modules_N; i++)
  {
	  sol.recoverInfo[i].x = modules_info[i].x;
	  sol.recoverInfo[i].y = modules_info[i].y;
	  sol.recoverInfo[i].width = modules[i].width;
	  sol.recoverInfo[i].height = modules[i].height;
	  sol.recoverInfo[i].area = modules[i].area;
	  sol.recoverInfo[i].delay = modules[i].delay;
	  sol.recoverInfo[i].ratio = modules[i].ratio;
  }
}

void B_Tree::keep_sol()
{
  get_solution(last_sol);
}

void B_Tree::keep_best()
{
  get_solution(best_sol);
}

void B_Tree::recover()
{
  recover(last_sol);
  // recover_partial();
}

void B_Tree::recover_best()
{
  recover(best_sol);
}

void B_Tree::recover(Solution &sol)
{
  nodes_root = sol.nodes_root;
  nodes = sol.nodes;
  for(int i = 0; i < modules_N; i++)
  {
	  modules_info[i].x = sol.recoverInfo[i].x;
	  modules_info[i].y = sol.recoverInfo[i].y;
	  modules[i].width = sol.recoverInfo[i].width;
	  modules[i].height = sol.recoverInfo[i].height;
	  modules[i].area = sol.recoverInfo[i].area;
	  modules[i].delay = sol.recoverInfo[i].delay;
	  modules[i].ratio = sol.recoverInfo[i].ratio;
  }
  isRecoverSol = true;
}

void B_Tree::recover_partial()
{
  if(changed_root != NIL)
    nodes_root = changed_root;
  
  for(int i=0; i < changed_nodes.size(); i++)
  {
    Node &n = changed_nodes[i];
    nodes[n.id] = n;
  }
}

void B_Tree::add_changed_nodes(int n)
{
  if(n==NIL) return;

  for(int i=0; i < changed_nodes.size(); i++)
    if(changed_nodes[i].id == n)
	  return;
  changed_nodes.push_back(nodes[n]);
}

//---------------------------------------------------------------------------
//   Simulated Annealing Permutation Operations
//---------------------------------------------------------------------------

void B_Tree::perturb()
{

	int p,n;
	n = rand()%modules_N;

	double r = rand_01();

	if( r < move_rate )
	{														// delete & insert
		// choose different module
		do
		{
			p = rand()%modules_N;	
		} while( n==p );

		delete_node(nodes[n]);
		insert_node(nodes[p], nodes[n]);
	}
	else// if( r < swap_rate )
	{														// swap node
		// choose different module
		do
		{
			p = rand()%modules_N;
		}while( n==p );

		swap_node2( nodes[p], nodes[n] );

	}
	//else //if( r < rotate_rate )
	//{							
 //       // rotate node
 //       int count =0;
	//	while( modules[n].no_rotate == true )
 //       {
 //           count++;
 //           n = rand()%modules_N;
 //           if( count > 10 )
 //           {
 //               printf( "WARN: perturb() rotation\n" );
 //               break;
 //           }
 //       }
 //       nodes[n].rotate = !nodes[n].rotate;
	//}
	//else
	//{														// resize soft module
	//	if( modules_soft_N != 0 )
	//	{
	//		n = modules_soft[ rand()%modules_soft_N ];
	//		B_Tree::soft_resize( n );
	//	}
	//	else
	//		nodes[n].rotate = !nodes[n].rotate;
	//}

	return;



//  int p,n;
//  n = rand()%modules_N;
//
////  changed_nodes.clear();
////  changed_root = NIL;
//
//
//  if(rotate_rate > rand_01()){
////    changed_nodes.push_back(nodes[n]);
//    nodes[n].rotate = !nodes[n].rotate;
//    if(rand_bool()) nodes[n].flip = !nodes[n].flip;
//  }
//  else{ 	
//
//    if(swap_rate >rand_01()){
//      do{
//        p = rand()%modules_N;
//      }while(n==p||nodes[n].parent==p||nodes[p].parent==n);
//
////      changed_nodes.push_back(nodes[p]);
////      changed_nodes.push_back(nodes[n]);
//
//      swap_node(nodes[p],nodes[n]);
//
//    }else{
//      do{
//        p = rand()%modules_N;
//      }while(n==p);
//
////      changed_nodes.push_back(nodes[p]);
////      changed_nodes.push_back(nodes[n]);
//
//      delete_node(nodes[n]);
//      insert_node(nodes[p],nodes[n]);
//    }
//  }
}

void B_Tree::swap_node(Node &n1, Node &n2)
{

	if(n1.left!=NIL)
	{  
		nodes[n1.left].parent  = n2.id;
	}
	if(n1.right!=NIL)
	{
		nodes[n1.right].parent = n2.id;  
	}
	if(n2.left!=NIL)
	{
		nodes[n2.left].parent  = n1.id;
	}
	if(n2.right!=NIL)
	{
		nodes[n2.right].parent = n1.id;  
	}

	if( n1.parent != n1.id )
	{
		if( n1.parent != n1.id && n1.parent != NIL)
		{
			//add_changed_nodes(n1.parent);
			if(nodes[n1.parent].left==n1.id)
				nodes[n1.parent].left  = n2.id;
			else
				nodes[n1.parent].right = n2.id; 
		}
		else
		{
			changed_root = n1.id;
			nodes_root = n2.id;
		}
	}

	if( n2.parent != n2.id )
	{
		if( n2.parent != NIL )
		{
			if(nodes[n2.parent].left==n2.id)
				nodes[n2.parent].left  = n1.id;
			else
				nodes[n2.parent].right = n1.id; 
		}
		else
		{
			nodes_root = n1.id;
		}
	}

	swap(n1.left,n2.left);
	swap(n1.right,n2.right);
	swap(n1.parent,n2.parent);
}

void B_Tree::swap_node2( Node &n1, Node &n2)
{
	if( n1.parent != n2.id && n2.parent != n1.id )
		swap_node( n1, n2 );
	else
	{
		bool leftChild;
		bool parentN1 = ( n1.id == n2.parent );
		if( parentN1 )
		{
			if( n1.left == n2.id )
			{
				n1.left = NIL;
				leftChild = true;
			}
			else
			{
				n1.right = NIL;
				leftChild = false;
			}
			n2.parent = n2.id;
		}
		else
		{
			if( n2.left == n1.id )
			{
				n2.left = NIL;
				leftChild = true;
			}
			else
			{
				n2.right = NIL;
				leftChild = false;
			}
			n1.parent = n1.id;
		}

		swap_node( n1, n2 );
		if( parentN1 )
		{
			n1.parent = n2.id;
			if( leftChild )
			{
				n2.left = n1.id;
			}
			else
			{
				n2.right = n1.id;
			}
		}
		else
		{
			n2.parent = n1.id;
			if( leftChild )
			{
				n1.left = n2.id;
			}
			else
			{
				n1.right = n2.id;
			}
		}
	}
}

void B_Tree::insert_node(Node &parent, Node &node)
{
	node.parent = parent.id;
	bool edge = rand_bool();

	if(edge)
	{
		// place left

		//add_changed_nodes(parent.left);
		node.left  = parent.left;
		node.right = NIL;
		if(parent.left!=NIL)
			nodes[parent.left].parent = node.id;

		parent.left = node.id;
	}
	else
	{
		// place right

		//add_changed_nodes(parent.right);
		node.left  = NIL;
		node.right = parent.right;
		if(parent.right!=NIL)
			nodes[parent.right].parent = node.id;

		parent.right = node.id;
	}
}

void B_Tree::delete_node(Node &node)
{
	int child    = NIL;		// pull which child
	int subchild = NIL;		// child's subtree
	int subparent= NIL; 

	if(!node.isleaf())
	{
		bool left = rand_bool();			// choose a child to pull up
		if(node.left ==NIL) left=false;
		if(node.right==NIL) left=true;

		//add_changed_nodes(node.left);
		//add_changed_nodes(node.right);

		if(left)
		{
			child = node.left;			// child will never be NIL
			if(node.right!=NIL)
			{
				subchild  = nodes[child].right;
				subparent = node.right;
				nodes[node.right].parent = child; 
				nodes[child].right = node.right;	// abut with node's another child
			}
		}
		else
		{
			child = node.right;
			if(node.left!=NIL)
			{
				subchild  = nodes[child].left;
				subparent = node.left;
				nodes[node.left].parent = child;
				nodes[child].left = node.left;
			}
		}
		//add_changed_nodes(subchild);
		nodes[child].parent = node.parent;
	}

	if(node.parent == NIL)			// root
	{			
		// changed_root = nodes_root;
		nodes_root = child;
	}
	else								// let parent connect to child
	{					
		//add_changed_nodes(node.parent);
		if(node.id == nodes[node.parent].left)
			nodes[node.parent].left  = child;
		else
			nodes[node.parent].right = child;
	}

	// place subtree
	if(subchild != NIL)
	{
		Node &sc = nodes[subchild];
		assert(subparent != NIL);

		while(1)
		{
			Node &p = nodes[subparent];

			// 2003/11/19
			// if both left and right child NIL, place with equal probability
			if( p.left == NIL && p.right == NIL )
			{
				sc.parent = p.id;
				if( rand_bool() )
				{
					p.left = sc.id;
				}
				else
				{
					p.right = sc.id;
				}
				break;
			}
			else if( p.left == NIL )		// place left 
			{
				sc.parent = p.id;
				p.left = sc.id;
				break;
			}
			else if( p.right == NIL )		// place right
			{
				sc.parent = p.id;
				p.right = sc.id;
				break;
			}
			else
			{
				subparent = (rand_bool() ? p.left : p.right);
			}
		}
	}

}


bool B_Tree::delete_node2( Node &node, DIR pull )
{
	DIR npull = !pull;

	int p  = node.parent;
	int n  = node.id;
	int c  = child( n, pull );
	int cn = child( n, npull );

	assert( n!= nodes_root ); // not root;

	DIR p2c = ( nodes[p].left == n ? LEFT : RIGHT );	// current DIR, LEFT or RIGHT

	if(c == NIL)
	{
		// Pull child, but NIL, so pull the other child
		wire_nodes( p, cn, p2c );
		return ( cn!=NIL );   // folding
	}
	else
	{
		wire_nodes( p, c, p2c );
	}

	while( c != NIL )
	{
		int k = child( c, npull );
		wire_nodes( c, cn, npull );
		cn = k;
		n = c;
		c = child(c,pull);
	}

	if(cn != NIL)
	{
		wire_nodes(n,cn,pull);
		return true;
	}
	else 
		return false;

}

/*
   Insert node into parent's left or right subtree according by "edge".
   Push node into parent's subtree in  "push" direction.

   if "fold" is true, then fold the leaf.
   (for the boundary condition of "delete" operation)

   delete <==> insert are permutating operations that can be recoved.
*/

void B_Tree::insert_node2(Node &parent,Node &node,
                        DIR edge=LEFT,DIR push=LEFT,bool fold/*=false*/){
  DIR npush = !push;
  int p= parent.id;
  int n= node.id;
  int c= child(p,edge);

  wire_nodes(p,n,edge);
  wire_nodes(n,c,push);
    
  while(c!=NIL)
  {
    wire_nodes(n,child(c,npush) ,npush);
    n= c;
    c= child(c,push);
  }
  wire_nodes(n,NIL,npush);

  if(fold)
  {
    wire_nodes(nodes[n].parent,NIL,push);
    wire_nodes(nodes[n].parent,n,npush); 
  }
}

void B_Tree::setOrientation()
{
    if( outline_ratio <= 0 )
        return;


    for( int i=0; i<modules_N; i++ )
    {
        if( modules[i].width > outline_width || modules[i].height > outline_height )
        {
            // rotate it
            nodes[i].rotate = true;
            modules[i].no_rotate = true;
            printf( "fix module %d\n", i );
    
            if( modules[i].height > outline_width || modules[i].width > outline_height )
            {
                printf( "impossible to legal module %d\n", i );
            }

        }
        if( modules[i].height > outline_width || modules[i].width > outline_height )
        {
            modules[i].no_rotate = true;
            printf( "fix module %d\n", i );
        }



    }

}

