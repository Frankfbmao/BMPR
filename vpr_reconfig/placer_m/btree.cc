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
#include <cmath>
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

	maplist(w, h);
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

	long  id = Max(grids);
	Congest = grids[id].value;
	return Congest;
}
//to check correctness of the tree
void B_Tree::Final_tree()
{


	FILE *tree = fopen("Final_tree", "a");

	fprintf(tree, "%d  %.3f   %.3f   %.3f  %d   %.3f\n", modules_N, Width, Height, Area, WireLength, getDeadSpace());

	//////////for (int i = 0; i < modules_N; i++)
	///support split module. use map structure.
	for (map<int, Module>::iterator it = modules.begin();
		it != modules.end();
		it++)
	{
		int i = it->first;
		fprintf(tree, "RECT %s   %d   %d   %d   %d \n",
			modules[i].name, modules_info[i].x, modules_info[i].y, modules_info[i].rx, modules_info[i].ry);  //height
	}
	fprintf(tree, "-----\n");

	fprintf(tree, "nodes_root = %d\n", nodes_root);

	////////for (int i = 0; i < modules_N; i++)
	////////{
	///support split module. use map structure.
	for (map<int, Module>::iterator it = modules.begin();
		it != modules.end();
		it++)
	{
		int i = it->first;
		fprintf(tree, "node[%d].id = %d  nodes[%d].parent= %d    nodes[%d].left=%d   nodes[%d].right= %d\n", i, nodes[i].id, i, nodes[i].parent, i, nodes[i].left, i, nodes[i].right);
	}

	fprintf(tree, "--------------------------------------------------------------------\n");
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
	////// initialize contour structure
	////contour.resize(modules_N);
	////  
	////// initialize b*tree by complete binary tree
	////nodes.resize(modules_N);

	vector<int> tmpmodidset;  //module id set
	for (map<int, Module>::iterator ita = modules.begin();
		ita != modules.end();
		ita++)
	{   //record module id.     vector index id start from 
		tmpmodidset.push_back(ita->first);

		//initial nodes
		Node tmpnode;
		nodes.insert(make_pair(ita->first, tmpnode));

		//initial contour
		Contour tmpcontour;
		contour.insert(make_pair(ita->first, tmpcontour));
	}

	nodes_root = tmpmodidset[0];
	for (int i = 0; i < modules_N; i++)   //create initial tree
	{
		nodes[tmpmodidset[i]].id = tmpmodidset[i];

		////nodes[i].parent = (i+1)/2-1;    /// i is 0 root node of the initial tree
		if (NIL == (i + 1) / 2 - 1)
		{
			nodes[tmpmodidset[i]].parent = NIL;
		}
		else
		{
			nodes[tmpmodidset[i]].parent = tmpmodidset[(i + 1) / 2 - 1];
		}
		nodes[tmpmodidset[i]].left = (2 * i + 1 < modules_N ? tmpmodidset[2 * i + 1] : NIL);
		nodes[tmpmodidset[i]].right = (2 * i + 2 < modules_N ? tmpmodidset[2 * i + 2] : NIL);
	}
	nodes[tmpmodidset[0]].parent = NIL;
	best_sol.clear();
	last_sol.clear();
	clear();

	////fbmao debug
	/////	cout << "in initial function, it has bug " << endl;
	normalize_cost(30);
}

//---------------------------------------------------------------------------
//   Testing, Debuging tools
//---------------------------------------------------------------------------

bool B_Tree::legal()
{
	int num = 0;
	return legal_tree(NIL, nodes_root, num);
}

bool B_Tree::legal_tree(int p, int n, int &num)
{
	num++;
	if (nodes[n].parent != p) return false;
	if (nodes[n].left != NIL)
	if (legal_tree(n, nodes[n].left, num) != true) return false;

	if (nodes[n].right != NIL)
	if (legal_tree(n, nodes[n].right, num) != true) return false;

	if (p == NIL) // root
		return (num == modules_N);
	return true;
}

void B_Tree::testing()
{
	int p, n;
	Solution E;

	do
	{

		n = rand() % modules_N;
		p = rand() % modules_N;

		while (n == nodes_root)		// n is not root
			n = rand() % modules_N;

		while (n == p || nodes[n].parent == p || nodes[p].parent == n)	// n != p & n.parent != p
			p = rand() % modules_N;

		Node &node = nodes[n];
		Node &parent = nodes[p];
		get_solution(E);
		swap_node(parent, node);

	} while (legal());

	cout << "p=" << p << ", n=" << n << endl;
	recover(E);
	show_tree();
	cout << "\n  p=" << p << ", n=" << n << endl;
	swap_node(nodes[p], nodes[n]);
	show_tree();
}

void B_Tree::show_tree()
{
	cout << "root: " << nodes_root << endl;
	//////for (int i = 0; i < modules_N; i++)
	//////{

	///support split module. use map structure.
	for (map<int, Module>::iterator it = modules.begin();
		it != modules.end();
		it++)
	{
		int i = it->first;
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

	place_module(p, NIL);
	Node &n = nodes[p];
	if (n.right != NIL)      S.push(n.right);
	if (n.left != NIL)      S.push(n.left);

	// inorder traverse
	while (!S.empty())
	{
		p = S.top();
		S.pop();
		Node &n = nodes[p];

		assert(n.parent != NIL);
		bool is_left = (nodes[n.parent].left == n.id);
		place_module(p, n.parent, is_left);

		if (n.right != NIL)      S.push(n.right);
		if (n.left != NIL)      S.push(n.left);
	}

	// compute Width, Height
	double max_x = -1, max_y = -1;
	for (int p = contour_root; p != NIL; p = contour[p].front)
	{
		max_x = max(max_x, double(modules_info[p].rx));
		max_y = max(max_y, double(modules_info[p].ry));
	}

	Width = max_x;
	Height = max_y;
	Area = Height*Width;       //chip area (include deadspace)

	TotalArea = 0;
	///////	for (int i = 0; i < modules_N; i++)
	///support split module. use map structure.
	for (map<int, Module>::iterator it = modules.begin();
		it != modules.end();
		it++)
	{
		int i = it->first;
		TotalArea += modules[i].area;   //total area for all the modules
	}

	Deadspace = (Area - TotalArea) / double(Area);

	//fbmao debug
	// cout << "xxxxxxxxdebug xxxxxxxxxxxxxxxxxxxxxxx area " <<Area<< " totalarea " <<TotalArea << endl;


	//////	creatNetworkOfModules();  //for consider congestion?????

	FPlan::packing(); 	// for wirelength  
	isRecoverSol = false;   //if packing multiple times, it will have an error in here

	//for congestion
	if (calcongest == true)
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
void B_Tree::place_module(int mod, int abut, bool is_left)
{
	Module_Info &mod_mf = modules_info[mod];
	mod_mf.rotate = nodes[mod].rotate;  //now is closed
	mod_mf.flip = nodes[mod].flip;

	//if the solution is the recover solution or the recover best solution
	//donnot need to choose the ratio, or else the solution cannot recover
	if (!isRecoverSol && !isoneratioflow && !modules[mod].issplitmod && !issplitInCurrentratio)  ////if it is split module, do not change ratio.
	{
		//first choose a shape for module according to the area*dealy 
		//just choose the first three 
		if (modules[mod].aspec.size() == 0)
		{
			cout << "There is an aspect ratio error for module %d, pls check it" << mod << endl;
			exit(1);
		}

		int id = 0;
		//may need to modify later.  consider half size, reduce time complexity
		//random choose a solution during packing
		if (1 == modules[mod].aspec.size())  //for hard module, only has one ratio
		{
			id = 0;
		}
		else
		{
			if (2 == (int)modules[mod].aspec.size())
			{
				id = rand() % ((int)modules[mod].aspec.size());
			}
			else
			{
				id = rand() % (((int)modules[mod].aspec.size() + 1) / 2);
			}
		}
		float asperatio = modules[mod].aspec[id];
		//shoule update the module info at the same time, in order to calculate cost for each one
		modules[mod].ratio = asperatio;
		map<float, pair<pair<int, int>, float> >::iterator it = modules[mod].aspeWidHeightDelay.find(asperatio);
		if (it != modules[mod].aspeWidHeightDelay.end())
		{
			pair<pair<int, int>, float> &widHeightDelay = it->second;
			modules[mod].width = widHeightDelay.first.first;
			modules[mod].height = widHeightDelay.first.second;
			modules[mod].area = modules[mod].width * modules[mod].height;
			modules[mod].delay = widHeightDelay.second;   //use to calculate delay cost (Intra_delayCost)
		}
		else
		{
			cout << "There is not elements in aspecAreaDelay map, pls check." << endl;
		}
	}

	int w = modules[mod].width;
	int h = modules[mod].height;
	if (nodes[mod].rotate)
		swap(w, h);

	if (abut == NIL)			// root node
	{
		contour_root = mod;
		contour[mod].back = NIL;
		contour[mod].front = NIL;
		mod_mf.x = mod_mf.y = 0;
		mod_mf.rx = w, mod_mf.ry = h;

		//fbmao for supporting
		//	to check this module position
		//	1. whether contains heterogeneous block, if contain, whether the resource is enough?  if not reject (Or final step to reject the solution?? Each step must be legal??)
		//	2. if not contain . check whether it occupy memory or dsp column.  so it need give extra column to place it. (Currently use this approach 2)
		//update ry to let the RAM, DSP in legal position  (they have position constraint)
		//update the rx of the module, to let the Region meet the resource requirement  (expand along x axis)
		checkArchResourceAndUpdateModuleCoordinate(mod, mod_mf.x, mod_mf.y, mod_mf.rx, mod_mf.ry);

		return;
	}

	int p;   // trace contour from p

	if (is_left)	// left
	{
		int abut_width = (nodes[abut].rotate ? modules[abut].height :
			modules[abut].width);

		///fbmao. Notice  rx may != x + width  (rx-x  virtual width: region width)	
		// mod_mf.x  = modules_info[abut].x + abut_width;  //original 
		mod_mf.x = modules_info[abut].rx;    //fbmao change for supporting heterogeneous blocks. module has expanded

		mod_mf.rx = mod_mf.x + w;

		p = contour[abut].front;

		contour[abut].front = mod;
		contour[mod].back = abut;

		if (p == NIL)  // no obstacle in X axis
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

		int n = contour[abut].back;

		if (n == NIL)
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
	int bx, by;
	assert(p != NIL);

	for (; p != NIL; p = contour[p].front)
	{
		bx = modules_info[p].rx;
		by = modules_info[p].ry;
		min_y = max(min_y, by);

		if (bx >= mod_mf.rx)			 	// update contour
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
			if (mod_mf.rx > bx)
			{ //this ifsentence cannot be executed
				//redo and skip
				mod_mf.y = originalmod_mfy;
				mod_mf.ry = originalmod_mfry;
				mod_mf.rx = originalmodmfrx; //mod_mf donnot change
				continue;
			}



			if (bx > mod_mf.rx)
			{
				contour[mod].front = p;
				contour[p].back = mod;
			}
			else							// bx==mod_mf.rx
			{
				int n = contour[p].front;
				contour[mod].front = n;
				if (n != NIL)
					contour[n].back = mod;
			}
			break;
		}
	}

	if (p == NIL)
	{
		mod_mf.y = (min_y == INT_MIN ? 0 : min_y);
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
//	need to change the height to ry-> for tracing the contour structure. chang ry since  RAM, DSP strart position need to be changed
void B_Tree::checkArchResourceAndUpdateModuleCoordinate(int modid, int &x, int &y, int &rx, int &ry)
{
	//use for expand  x column.  some ram and dsp used the CLB's positions
	int numcolforraminmodule = 0;  //the number of column for contains "RAM" in module
	int numcolfordspinmodule = 0;  //the "dsp"

	////check whether the module in the top module set or in the split module set
	map<string, map<float, pair<int, int> > >::iterator fileratioramdspcolit
		= filenameRatioramdspcolnum.find(modules[modid].name);           /////?????? check whether the name needed to be changed.

	if (fileratioramdspcolit != filenameRatioramdspcolnum.end())
	{   ///the module is a top module and it is not split
		map<float, pair<int, int> > &tmpratioramdsp = fileratioramdspcolit->second;
		map<float, pair<int, int> >::iterator tmprtaioramdspit = tmpratioramdsp.find(modules[modid].ratio);
		assert(tmprtaioramdspit != tmpratioramdsp.end());
		numcolforraminmodule = tmprtaioramdspit->second.first;
		numcolfordspinmodule = tmprtaioramdspit->second.second;
	}
	else
	{ ///it must in the split module set 
		if (isdynamicpartition)  /////isdynamicpartition == 1    Dynamic partition
		{   ///split module in two or three or n parts. record the number of column (RAM, DSP) in a module 
			//splitfilename,  ratio   partitionRatio     memnum  dspnum  (real vaild memnum, dspnum in original module, Region)
			///can divide the modules into 2 or 3 or 4 or 5 ... parts????
			/////map<string, map<float, map<int, pair<int, int> > > > dynamicsplitfilenameRatioPartnumramdspcolnum;
			map<string, map<float, map<int, pair<int, int> > > >::iterator tmpsplitfilenameratiopartnumramdspit =
				dynamicsplitfilenameRatioPartnumramdspcolnum.find(modules[modid].name);
			assert(tmpsplitfilenameratiopartnumramdspit != dynamicsplitfilenameRatioPartnumramdspcolnum.end());
			map<float, map<int, pair<int, int> > > &tmpratiopartnumramdspcol = tmpsplitfilenameratiopartnumramdspit->second;
			map<float, map<int, pair<int, int> > >::iterator tmpratiopartnumramdspcolit = tmpratiopartnumramdspcol.find(modules[modid].ratio);
			assert(tmpratiopartnumramdspcolit != tmpratiopartnumramdspcol.end());
			map<int, pair<int, int> > &tmppartnumramdspcol = tmpratiopartnumramdspcolit->second;
			map<int, pair<int, int> >::iterator tmppartnumramdspcolit = tmppartnumramdspcol.find(modules[modid].curbelongnpart);
			assert(tmppartnumramdspcolit != tmppartnumramdspcol.end());
			numcolforraminmodule = tmppartnumramdspcolit->second.first;
			numcolfordspinmodule = tmppartnumramdspcolit->second.second;
		}
		else if (!issplitfitfordifferentarch) 
		{
			map<string, map<float, map<float, pair<int, int> > > >::iterator tmpsplitfilenameratiopartratioramdspit =
				splitfilenameRatioPartratioramdspcolnum.find(modules[modid].name);
			assert(tmpsplitfilenameratiopartratioramdspit != splitfilenameRatioPartratioramdspcolnum.end());
			map<float, map<float, pair<int, int> > > &tmpratiopartratioramdspcol = tmpsplitfilenameratiopartratioramdspit->second;
			map<float, map<float, pair<int, int> > >::iterator tmpratiopartratioramdspcolit = tmpratiopartratioramdspcol.find(modules[modid].ratio);
			assert(tmpratiopartratioramdspcolit != tmpratiopartratioramdspcol.end());
			map<float, pair<int, int> > &tmppartratioramdspcol = tmpratiopartratioramdspcolit->second;
			map<float, pair<int, int> >::iterator tmppartratioramdspcolit = tmppartratioramdspcol.find(modules[modid].curvertipartiRatio);
			assert(tmppartratioramdspcolit != tmppartratioramdspcol.end());
			numcolforraminmodule = tmppartratioramdspcolit->second.first;
			numcolfordspinmodule = tmppartratioramdspcolit->second.second;
		}
		else  ////issplitfitfordifferentarch == 1   
		{   ////split fit for different architectures. (partition based on valid RAM and DSP columns)  
			map<string, map<float, pair<int, int> > >::iterator tmpsplitfilenameratioramdspit =
				fordiffarchsplitfilenameRatioramdspcolnum.find(modules[modid].name);
			assert(tmpsplitfilenameratioramdspit != fordiffarchsplitfilenameRatioramdspcolnum.end());
			map<float, pair<int, int> > &tmpratioramdspcol = tmpsplitfilenameratioramdspit->second;
			map<float, pair<int, int> >::iterator tmpratioramdspcolit = tmpratioramdspcol.find(modules[modid].ratio);
			assert(tmpratioramdspcolit != tmpratioramdspcol.end());
			numcolforraminmodule = tmpratioramdspcolit->second.first;
			numcolfordspinmodule = tmpratioramdspcolit->second.second;
		}
	}


	//In y coordinate, RAM position start from 1   
	//In y coordinate, DSP position start from 1 (one ram blk can place in position 1, 1+repeatram, 1+2*repeatram ...
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
	while (tmpmemcol <= rx)
	{
		if (tmpmemcol >= archx && tmpmemcol <= rx)
		{
			memcount++;
		}
		tmpmemcol += fmemrepeat;
	}

	//find the col for multiply (larger than rx)
	while (tmpmultiplycol <= rx)
	{
		if (tmpmultiplycol >= archx && tmpmultiplycol <= rx)
		{
			multiplycount++;
		}
		tmpmultiplycol += fmultirepeat;
	}


	//used for update module' verticalH. since the RAM (or DSP) start point must be legal
	//if the position is not enough. then expand along the x axis
	int ramshiftgap = 0; //move ramshiftgap along y axis to make the RAM start position legal
	int dspshiftgap = 0; //move dspshiftgap along y axis to make the DSP start postion legal

	//first check whether this module is heterogeneous module(containing RAM, DSP,etc.)
	//////set<int>::iterator ita = fheteromodidset.find(modid);       ///////// use this sentence to substitude???? assert(modules[modid].numram > 0 || modules[modid].numdsp > 0);
	if (modules[modid].numram > 0 || modules[modid].numdsp > 0)  //////ita != fheteromodidset.end()) //1.module with heterogeous block.such as RAM, DSP.
	{
		int extramemcol = 0;  //memory and DSP need to add extra column for clb.
		int extramultiplycol = 0;

		//check whether it occupy (memory's) column, if not. expand
		int tmpmemrx = rx;  //the temp position for rx  related to memory
		int tmpmultirx = rx; //the temp postion for rx related to multiply

		int tmpnummem = 0;  //the number of available position for mem
		int tmpnummultiply = 0;  //for multiply(DSP)


		//update veritcalH
		//first judge whether the start point for placing RAM and DSP is legal or not?
		//1> find the gap for shift ram to legal position
		int tmpfirstRamlegalpos = ramlegalstartrow;      //find gap can be better??? 
		while (tmpfirstRamlegalpos < archy) //archy is (module's lowest coordinate + 1)
		{
			tmpfirstRamlegalpos = tmpfirstRamlegalpos + fmemoryheight;
		}

		ramshiftgap = tmpfirstRamlegalpos - archy;

		int tmpfirstDsplegalpos = dsplegalstartrow;
		while (tmpfirstDsplegalpos < archy) //archy is (module's lowest coordinate + 1)
		{
			tmpfirstDsplegalpos = tmpfirstDsplegalpos + fmultiheight;
		}

		dspshiftgap = tmpfirstDsplegalpos - archy;

		int dspramshifgap = 0;
		int ramnumposforeachcol = 0;
		int dspnumposforeachcol = 0;

		assert(modules[modid].numram > 0 || modules[modid].numdsp > 0);
		//update rx and calculate the number of positions in each column for placing RAM and DSP
		if (modules[modid].numram > 0 && modules[modid].numdsp > 0)
		{    //choose the large value
			if (ramshiftgap >= dspshiftgap)
			{
				dspramshifgap = ramshiftgap;
			}
			else
			{
				dspramshifgap = dspshiftgap;
			}
		}
		else if (modules[modid].numram > 0)  //only has ram, so dsp does not have position constraint
		{   //donnot need to consider the DSP position constraint
			dspramshifgap = ramshiftgap;
		}
		else if (modules[modid].numdsp > 0)
		{   //does not consider the ram position constraint
			dspramshifgap = dspshiftgap;
		}

		//update ry
		ry = ry + dspramshifgap; //expand ry to make sure the RAM in the legal position

		int modstartrow = archy;
		int	modendrow = ry;
		while (modstartrow <= modendrow)
		{   //collect the available positions for placing RAM or DSP in each col (within the module region)
			if (modstartrow >= ramlegalstartrow && 0 == (modstartrow - ramlegalstartrow) % fmemoryheight
				&& modstartrow + fmemoryheight - 1 <= modendrow) //cannot beyond the region boudary
			{
				ramnumposforeachcol++; ////cannot use (verticalH / fmemoryheight), since memory start pos constraint
			}

			//dsp
			if (modstartrow >= dsplegalstartrow && 0 == (modstartrow - dsplegalstartrow) % fmultiheight
				&& modstartrow + fmultiheight - 1 <= modendrow)
			{
				dspnumposforeachcol++;
			}

			modstartrow++;
		}

		if (modules[modid].numram > 0 && modules[modid].numdsp > 0) //module contains memory and DSP
		{    //I.xxx
			//compare the number of cols for RAM, DSP. (may do not have solution, during SA process. Finally, become feasible.)
			assert(ramnumposforeachcol >= 1 && dspnumposforeachcol >= 1); //at least can place one RAM and DSP

			if (memcount > 0 && multiplycount > 0) //module occupy memeory column
			{
				tmpnummem = memcount * ramnumposforeachcol; //the available memory (number) 
				if (tmpnummem < modules[modid].numram)  //not enough, need to expand
				{
					assert(ramnumposforeachcol > 0);
                     
					extramemcol = (modules[modid].numram - tmpnummem) / ramnumposforeachcol;
					///possible bug 4/3 = 1. at least need 2 column
					if (extramemcol * ramnumposforeachcol < (modules[modid].numram - tmpnummem))
					{
						extramemcol = extramemcol + 1;
					}

					////////if (0 == extramemcol) //eg. 2/3 = 0. but still need at least one column for memory
					////////	extramemcol = 1;

					tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;
				}

				tmpnummultiply = multiplycount * dspnumposforeachcol; ///////(verticalH / fmultiheight);
				if (tmpnummultiply < modules[modid].numdsp)
				{
					assert(dspnumposforeachcol > 0);
					extramultiplycol = (modules[modid].numdsp - tmpnummultiply) / dspnumposforeachcol;
					//possible bug.  4/3 = 1.   it at least be 2
					if (extramultiplycol * dspnumposforeachcol < (modules[modid].numdsp - tmpnummultiply))
					{
						extramultiplycol = extramultiplycol + 1;
					}

					//////if (0 == extramultiplycol)  //bug 2 /3 = 0
					//////	extramultiplycol = 1;

					tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;
				}


				if (tmpmemrx >= tmpmultirx)
				{
					rx = tmpmemrx;
				}
				else
				{
					rx = tmpmultirx;
				}
			}
			else if (memcount > 0)  //module not occupy dsp col in the arch. Need to expand
			{
				//first check whether memeory is enough.
				tmpnummem = memcount * ramnumposforeachcol;
				if (tmpnummem < modules[modid].numram)
				{
					assert(ramnumposforeachcol > 0);
					extramemcol = (modules[modid].numram - tmpnummem) / ramnumposforeachcol;
					///possible bug   4/3 = 1,  2/3 = 0
					if (extramemcol * ramnumposforeachcol < (modules[modid].numram - tmpnummem))
					{
						extramemcol = extramemcol + 1;
					}

					//////if (0 == extramemcol) //eg. 2/3 = 0. but still need at least one column for memory
					//////	extramemcol = 1;

					tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;
				}

				assert(dspnumposforeachcol > 0);
				extramultiplycol = modules[modid].numdsp / dspnumposforeachcol;
				///possible bug.  4/3 = 1.   2/3 = 0
				if (extramultiplycol * dspnumposforeachcol < modules[modid].numdsp)
				{
					extramultiplycol = extramultiplycol + 1;
				}

				////////if (0 == extramultiplycol)
				////////	extramultiplycol = 1;

				tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;

				if (tmpmemrx >= tmpmultirx)
				{
					rx = tmpmemrx;
				}
				else
				{
					rx = tmpmultirx;
				}
			}
			else if (multiplycount > 0) //module do not occupy mem column. expand to contian memory resource
			{
				//first check whether multiply enough?
				tmpnummultiply = multiplycount * dspnumposforeachcol;
				if (tmpnummultiply < modules[modid].numdsp)
				{
					assert(dspnumposforeachcol > 0);
					extramultiplycol = (modules[modid].numdsp - tmpnummultiply) / dspnumposforeachcol;
					if (extramultiplycol * dspnumposforeachcol < (modules[modid].numdsp - tmpnummultiply))
					{
						extramultiplycol = extramultiplycol + 1;
					}

					//////if (0 == extramultiplycol)
					//////	extramultiplycol = 1;

					tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;
				}

				//to get memory
				assert(ramnumposforeachcol > 0);
				extramemcol = modules[modid].numram / ramnumposforeachcol;
				if (extramemcol * ramnumposforeachcol < modules[modid].numram)
				{
					extramemcol = extramemcol + 1;
				}

				//////if (0 == extramemcol)
				//////	extramemcol = 1;

				tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;
				if (tmpmemrx >= tmpmultirx)
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
				if (extramemcol * ramnumposforeachcol < modules[modid].numram)
				{
					extramemcol = extramemcol + 1;
				}

				//////if (0 == extramemcol)
				//////	extramemcol = 1;

				tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;

				//expand for multiply

				////	cout << "Debug in function  checkArchcoordinate" << endl;
				if (dspnumposforeachcol == 0)
				{
					cout << dspnumposforeachcol << endl;
				}


				assert(dspnumposforeachcol > 0);
				extramultiplycol = modules[modid].numdsp / dspnumposforeachcol;
				if (extramultiplycol * dspnumposforeachcol < modules[modid].numdsp)
				{
					extramultiplycol = extramultiplycol + 1;
				}

				//////if (0 == extramultiplycol)
				//////	extramultiplycol = 1;

				tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;

				if (tmpmemrx >= tmpmultirx)
				{
					rx = tmpmemrx;
				}
				else
				{
					rx = tmpmultirx;
				}
			}
		}  ////module contains memory and DSP

		else if (modules[modid].numram > 0)  //module only has ram, donot have dsp
		{    //II.xxxxx
			//check whether ram is enough
			if (memcount > 0)  //modules' region occupy (memory column) of the architecture
			{
				//first check whether the momory number is enough
				tmpnummem = memcount * ramnumposforeachcol;
				if (tmpnummem < modules[modid].numram)
				{
					assert(ramnumposforeachcol > 0);
					extramemcol = (modules[modid].numram - tmpnummem) / ramnumposforeachcol;
					if (extramemcol * ramnumposforeachcol < (modules[modid].numram - tmpnummem))
					{
						extramemcol = extramemcol + 1;
					}

					//////if (0 == extramemcol)
					//////	extramemcol = 1;

					tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;
				}


				if (0 == multiplycount)
				{
					rx = tmpmemrx;
				}
				else
				{
					if (rx == tmpmemrx) //if ram is enough
					{
						int tmpmultiplycount = multiplycount;
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							int tmpmemendpos = rx + tmpmultiplycount;
							while (tmpmempos <= tmpmemendpos)  //loop endless????bug
							{
								if (tmpmempos > rx && tmpmempos <= tmpmemendpos)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							int tmpmultiplyendpos = rx + tmpmultiplycount;
							while (tmpmultiplypos <= tmpmultiplyendpos)
							{
								if (tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							//if it has enough position for clbs
							//gap: multiplycount
							if (tmpmultiplycount - multiplycount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
							{
								break;
							}
							else
								tmpmultiplycount++;
						} while (1);

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

							while (tmpmempos <= tmpmemrx)
							{
								if (tmpmempos > rx && tmpmempos <= tmpmemrx)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							while (tmpmultiplypos <= tmpmemrx)
							{
								if (tmpmultiplypos > rx && tmpmultiplypos <= tmpmemrx)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							//minus 1 : because current the memory or dsp column cannot be used for placing clb
							if ((tmpmemrx - rx - 1) - tmpmemexpandcount - tmpmultiplyexpandcount - multiplycount >= 0)//if it has enough position for clbs
							{
								break;
							}
							else
								tmpmemrx++;
						} while (1);

						rx = tmpmemrx;
					}//rx != tmpmemrx
				}//	if(0 != multiplycount)
			}
			else //module donot occupy memory column
			{
				//to get memory
				assert(ramnumposforeachcol > 0);
				extramemcol = modules[modid].numram / ramnumposforeachcol;
				if (extramemcol * ramnumposforeachcol < modules[modid].numram)
				{
					extramemcol = extramemcol + 1;
				}

				////////if (0 == extramemcol)
				////////	extramemcol = 1;

				tmpmemrx = tmpmemcol + (extramemcol - 1) * fmemrepeat;

				if (0 == multiplycount)
				{
					rx = tmpmemrx;
				}
				else
				{
					if (rx == tmpmemrx) //if ram is enough
					{
						int tmpmultiplycount = multiplycount;
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							int tmpmemendpos = rx + tmpmultiplycount;
							while (tmpmempos <= tmpmemendpos)
							{
								if (tmpmempos > rx && tmpmempos <= tmpmemendpos)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							int tmpmultiplyendpos = rx + tmpmultiplycount;
							while (tmpmultiplypos <= tmpmultiplyendpos)
							{
								if (tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							//if it has enough position for clbs
							if (tmpmultiplycount - multiplycount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
							{
								break;
							}
							else
								tmpmultiplycount++;
						} while (1);

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

							while (tmpmempos <= tmpmemrx)
							{
								if (tmpmempos > rx && tmpmempos <= tmpmemrx)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							while (tmpmultiplypos <= tmpmemrx)
							{
								if (tmpmultiplypos > rx && tmpmultiplypos <= tmpmemrx)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							if ((tmpmemrx - rx - 1) - tmpmemexpandcount - tmpmultiplyexpandcount - multiplycount >= 0)//if it has enough position for clbs
							{
								break;
							}
							else
								tmpmemrx++;
						} while (1);

						rx = tmpmemrx;
					}
				}
			}
		}

		else   //numdsp > 0	
		{   //III.xxxx.
			if (multiplycount > 0) //occupy multiply column
			{
				//first check whether multiply enough?
				tmpnummultiply = multiplycount * dspnumposforeachcol;
				if (tmpnummultiply < modules[modid].numdsp)
				{
					assert(dspnumposforeachcol > 0);
					extramultiplycol = (modules[modid].numdsp - tmpnummultiply) / dspnumposforeachcol;
					if (extramultiplycol * dspnumposforeachcol < (modules[modid].numdsp - tmpnummultiply))
					{
						extramultiplycol = extramultiplycol + 1;
					}

					//////if (0 == extramultiplycol)
					//////	extramultiplycol = 1;

					tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;
				}


				if (0 == memcount)
				{
					rx = tmpmultirx;
				}
				else
				{
					if (rx == tmpmultirx) //if multiply is enough
					{
						int tmpmemcount = memcount;
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							int tmpmemendpos = rx + tmpmemcount;
							while (tmpmempos <= tmpmemendpos)
							{
								if (tmpmempos > rx && tmpmempos <= tmpmemendpos)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							int tmpmultiplyendpos = rx + tmpmemcount;
							while (tmpmultiplypos <= tmpmultiplyendpos)
							{
								if (tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							//if it has enough position for clbs
							if (tmpmemcount - memcount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
							{
								break;
							}
							else
								tmpmemcount++;
						} while (1);

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

							while (tmpmempos <= tmpmultirx)
							{
								if (tmpmempos > rx && tmpmempos <= tmpmultirx)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							while (tmpmultiplypos <= tmpmultirx)
							{
								if (tmpmultiplypos > rx && tmpmultiplypos <= tmpmultirx)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							if ((tmpmultirx - rx - 1) - tmpmemexpandcount - tmpmultiplyexpandcount - memcount >= 0)//if it has enough position for clbs
							{
								break;
							}
							else
								tmpmultirx++;
						} while (1);

						rx = tmpmultirx;
					}
				}
			}
			else  //module do not occupy dspcolumn
			{
				assert(dspnumposforeachcol > 0);
				extramultiplycol = modules[modid].numdsp / dspnumposforeachcol;
				if (extramultiplycol * dspnumposforeachcol < modules[modid].numdsp)
				{
					extramultiplycol = extramultiplycol + 1;
				}

				////////if (0 == extramultiplycol)
				////////	extramultiplycol = 1;

				tmpmultirx = tmpmultiplycol + (extramultiplycol - 1) * fmultirepeat;

				if (0 == memcount)
				{
					rx = tmpmultirx;
				}
				else
				{
					if (rx == tmpmultirx) //if multiply is enough
					{
						int tmpmemcount = memcount;
						do
						{
							int tmpmempos = fmemstart;
							int tmpmemexpandcount = 0;

							int tmpmultiplypos = fmultistart;
							int tmpmultiplyexpandcount = 0;

							int tmpmemendpos = rx + tmpmemcount; //memory number > 0
							while (tmpmempos <= tmpmemendpos)
							{
								if (tmpmempos > rx && tmpmempos <= tmpmemendpos)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							int tmpmultiplyendpos = rx + tmpmemcount;
							while (tmpmultiplypos <= tmpmultiplyendpos)
							{
								if (tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							//if it has enough position for clbs
							if (tmpmemcount - memcount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
							{
								break;
							}
							else
								tmpmemcount++;
						} while (1);

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

							while (tmpmempos <= tmpmultirx)
							{
								if (tmpmempos > rx && tmpmempos <= tmpmultirx)
								{
									tmpmemexpandcount++;
								}
								tmpmempos += fmemrepeat;
							}

							while (tmpmultiplypos <= tmpmultirx)
							{
								if (tmpmultiplypos > rx && tmpmultiplypos <= tmpmultirx)
								{
									tmpmultiplyexpandcount++;
								}
								tmpmultiplypos += fmultirepeat;
							}

							if ((tmpmultirx - rx - 1) - tmpmemexpandcount - tmpmultiplyexpandcount - memcount >= 0)//if it has enough position for clbs
							{
								break;
							}
							else
								tmpmultirx++;
						} while (1);

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
		if (modules[modid].numram > 0 && modules[modid].numdsp > 0)
		{
			if (memcount > 0 && multiplycount > 0)
			{
				if (memcount > numcolforraminmodule)
				{
					expand1 = memcount - numcolforraminmodule;
				}

				if (multiplycount > numcolfordspinmodule)
				{
					expand2 = multiplycount - numcolfordspinmodule;
				}
			}
			else if (memcount > 0)
			{
				if (memcount > numcolforraminmodule)
				{
					expand1 = memcount - numcolforraminmodule;
				}
			}
			else if (multiplycount > 0)
			{
				if (multiplycount > numcolfordspinmodule)
				{
					expand2 = multiplycount - numcolfordspinmodule;
				}
			}
		}
		else if (modules[modid].numram > 0)
		{
			if (memcount > numcolforraminmodule)
			{
				expand1 = memcount - numcolforraminmodule;
			}
		}
		else if (modules[modid].numdsp > 0)
		{
			expand2 = multiplycount - numcolfordspinmodule;
		}

		///when the number of RAM (or DSP) column in Architecture less than the number of RAM columnn inside module
		if (expand1 < 0)
		{
			expand1 = 0;
		}

		if (expand2 < 0)
		{
			expand2 = 0;
		}

		int extraRamDspCol = expand1 + expand2;
		if (extraRamDspCol > 0)
		{
			do
			{
				int tmpmempos = fmemstart;
				int tmpmemexpandcount = 0;

				int tmpmultiplypos = fmultistart;
				int tmpmultiplyexpandcount = 0;

				int tmpmemendpos = rx + extraRamDspCol; //memory number > 0
				while (tmpmempos <= tmpmemendpos)
				{
					if (tmpmempos > rx && tmpmempos <= tmpmemendpos)
					{
						tmpmemexpandcount++;
					}
					tmpmempos += fmemrepeat;
				}

				int tmpmultiplyendpos = rx + extraRamDspCol;
				while (tmpmultiplypos <= tmpmultiplyendpos)
				{
					if (tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
					{
						tmpmultiplyexpandcount++;
					}
					tmpmultiplypos += fmultirepeat;
				}

				//if it has enough position for clbs
				if (extraRamDspCol - memcount - multiplycount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
				{
					break;
				}
				else
					extraRamDspCol++;
			} while (1);
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
			while (tmpmempos <= tmpmemendpos)
			{
				if (tmpmempos > rx && tmpmempos <= tmpmemendpos)
				{
					tmpmemexpandcount++;
				}
				tmpmempos += fmemrepeat;
			}

			int tmpmultiplyendpos = rx + extraRamDspCol;
			while (tmpmultiplypos <= tmpmultiplyendpos)
			{
				if (tmpmultiplypos > rx && tmpmultiplypos <= tmpmultiplyendpos)
				{
					tmpmultiplyexpandcount++;
				}
				tmpmultiplypos += fmultirepeat;
			}

			//if it has enough position for clbs
			if (extraRamDspCol - memcount - multiplycount - tmpmemexpandcount - tmpmultiplyexpandcount >= 0)
			{
				break;
			}
			else
				extraRamDspCol++;
		} while (1);

		rx = rx + extraRamDspCol;//expand the rx
	}
}





//---------------------------------------------------------------------------
//   Manipulate B*Tree auxilary procedure
//---------------------------------------------------------------------------

// place child in parent's edge
void B_Tree::wire_nodes(int parent, int child, DIR edge)
{
	assert(parent != NIL);
	(edge == LEFT ? nodes[parent].left : nodes[parent].right) = child;
	if (child != NIL)
		nodes[child].parent = nodes[parent].id;
}

// get node's child
inline int B_Tree::child(int node, DIR d)
{
	assert(node != NIL);
	return (d == LEFT ? nodes[node].left : nodes[node].right);
}


//---------------------------------------------------------------------------
//   Simulated Annealing Temporal Solution
//---------------------------------------------------------------------------

void B_Tree::get_solution(Solution &sol)
{
	/////sol.cost = getCost();
	///// Need to check the cost and the flow ??????? multiratio  if (!isoneratioflow)   //if has multi-ratio.   if one ratio and non-split flow. do not need to record the sol
	{
		///keep modules, modules_Info
		//split nodes, node id will be changed. keep original node id unchanged
		sol.nodes_root = nodes_root;
		sol.contour_root = contour_root;
		sol.modules_N = modules_N;

		//need clear resize fist??

		sol.nodes = nodes;
		sol.modules = modules;
		sol.modules_info = modules_info;
		sol.contour = contour;



////////////		isRecoverSol = true;

		//////////for support split
		/////////sol.parentsubmoduleid = parentsubmoduleid;   //split solution. not accept
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
	///one ratio also need to be split
	///////if (!isoneratioflow)   //if has multi-ratio            ????????????check it is right???? bug?? flow??
	{
		///keep modules, modules_Info
		//split nodes, node id will be changed. keep original node id unchanged
		nodes_root = sol.nodes_root;
		contour_root = sol.contour_root;
		modules_N = sol.modules_N;

		//need clear resize fist??

		nodes = sol.nodes;
		modules = sol.modules;
		modules_info = sol.modules_info;
		contour = sol.contour;
		isRecoverSol = true;

		////////for support split
		//////parentsubmoduleid = sol.parentsubmoduleid;
	}



	////////nodes_root = sol.nodes_root;
	////////nodes = sol.nodes;
	////////if (!isoneratioflow)   //if has multi-ratio. 
	////////{
	//////// for (int i = 0; i < modules_N; i++)
	//////// {
	////////  //modules_info[i].x = sol.recoverInfo[i].x;
	////////  //modules_info[i].y = sol.recoverInfo[i].y;
	////////  modules[i].width = sol.recoverInfo[i].width;
	////////  modules[i].height = sol.recoverInfo[i].height;
	////////  modules[i].area = sol.recoverInfo[i].area;
	////////  modules[i].delay = sol.recoverInfo[i].delay;
	////////  modules[i].ratio = sol.recoverInfo[i].ratio;
	//////// }
	//////// isRecoverSol = true;
	////////}
}

void B_Tree::recover_partial()
{
	if (changed_root != NIL)
		nodes_root = changed_root;

	for (int i = 0; i < changed_nodes.size(); i++)
	{
		Node &n = changed_nodes[i];
		nodes[n.id] = n;
	}
}

void B_Tree::add_changed_nodes(int n)
{
	if (n == NIL) return;

	for (int i = 0; i < changed_nodes.size(); i++)
	if (changed_nodes[i].id == n)
		return;
	changed_nodes.push_back(nodes[n]);
}

//---------------------------------------------------------------------------
//   Simulated Annealing Permutation Operations
//---------------------------------------------------------------------------

void B_Tree::perturb()
{
	////now only contains three branches
	if (issplitfitfordifferentarch)
	{    ///split for different architectures.    partition based on RAM, DSP column
		fordiffarchsplimodperturbwithoutstrongconstraint();
	}
	else ///////if (isdynamicpartition)
	{
		dynamicsplimodperturb();    ///use swap rate to control whether to do split operation
	}
	//////else if (!issplitfitfordifferentarch)
	//////{   //split or do not split   (If do split flow. it should partition module in half)
	//////	splimodperturbwithoutstrongconstraint();
	//////}



	return;


	//fbmao for allowing perturb splitted modules
	///////	cout << "Enter perturb function" << endl;
	splimodperturb();     ///New perturb function  support  (allowing splitted module to be swapped)
	return;


	////node constraint. 
	////	1. splitted  submodule cannot be splitted again
	////	2. nodes have position constraint: split module cannot be directly perturb
	//////(add constraint on it. splitted modules from the same parent module must be considered together)
	////	3. recove etc. record some information. E.g.  module is not be splitted .update ma<> parentxxxxxx

	//fbmao add for supporting split module (idnodeidmap is used to map index);
	///////create idnodeidmap
	vector<int>idnodeidmap;  //id-> nodeid may not continuous. eg. 1, 5, 7,  ..  vector index is 0,1,2,3
	for (map<int, Node>::iterator it = nodes.begin();
		it != nodes.end();
		it++)
	{
		idnodeidmap.push_back(it->first); //it->first== modid == nodeid
	}

	///??? Later split module can be changed or moved.  (one of these three operations can work)
	int p, n;
	n = rand() % modules_N;

	////choose a non-split module.    ////???  if all are splitted modules, keep nodes' constraint (find a way to perturb solution)
	int n1 = -1; //temporary initial value
	int tmpcountnum = 0;
	while (modules[idnodeidmap[n]].issplitmod)
	{
		if (tmpcountnum >= modules_N)
		{
			break;
		}
		n1 = rand() % modules_N;
		if (n != n1)
		{
			tmpcountnum++;
			n = n1;
		}
	}

	if (tmpcountnum >= modules_N)
	{
		return;   //currently, all the modules are splitted module. do nothing [Cannot use the split operation].
	}

	double r = rand_01();

	if (r < move_rate)
	{
		// delete & insert
		tmpcountnum = 0;
		// choose different module
		do
		{
			p = rand() % modules_N;                 /////later pick up from  non-splitmodule list

			if (p != n)
			{
				tmpcountnum++;
			}

			if (tmpcountnum >= modules_N)
			{
				break;
			}

		} while (n == p || modules[idnodeidmap[p]].issplitmod);


		if (modules[idnodeidmap[p]].issplitmod || tmpcountnum > modules_N)
		{
			return;   ///do nothing.
		}

		delete_node(nodes[idnodeidmap[n]]);
		insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
	}
	else if (r < swap_rate)
	{														// swap node
		// choose different module
		do
		{
			p = rand() % modules_N;                 /////later pick up from  non-splitmodule list

			if (p != n)
			{
				tmpcountnum++;
			}

			if (tmpcountnum >= modules_N)
			{
				break;
			}

		} while (n == p || modules[idnodeidmap[p]].issplitmod);

		if (modules[idnodeidmap[p]].issplitmod || tmpcountnum > modules_N)
		{
			return;   ///do nothing.
		}

		swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

	}
	else  ///support split operation (fbmao add). split operation.   
	{	//a module is split into two submodules  <--> a node is split into two nodes and has position constraint
		//change structure.   map<int, node>    keep original module id. node id (in order to recover to original state). removed modules also are needed to be recorded.
		//split module n (n is the node id equal to the module id, contour id)

		//also can determine whether it is right child or left child?
		//first get submodules, and can get modules' id. -> then get node id 

		//add by fbmao for supporting splitting module
		//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & partitionRatio & its submodule set & partitioncost   //first level (the topest level)
		////////////map<string, map<float, map<int, map<float, splitmodule> > > > topmodnamesubmodset;	


		///Notice. Currrently module only can be split once.
		///pick up non-split module from the unsplit modules.

		map<string, map<float, map<int, map<float, splitmodule> > > >::iterator tmpmodnameratiotypepartratiosplitmodit; //used these iterator below
		map<float, map<int, map<float, splitmodule> > >::iterator tmpratiotypepartratiosplitmodit;
		map<int, map<float, splitmodule> >::iterator tmptypepartratiosplitmodit;
		map<float, splitmodule>::iterator tmppartratiosplitmodit;

		//Currenlty, the module has been split, cannot return back
		string tmpmodname;    //current module's parent name (the most original name by running the VPR)
		string modnamewithpostfix; //current modules' full name
		//split module idnodeidmap[n]

		////////(module) idnodeidmap[n] will be split 
		float tmpratio = modules[idnodeidmap[n]].ratio;  //Each time need to record the startx, endx. etc??.
		modnamewithpostfix = modules[idnodeidmap[n]].name;  //eg.  A#11#11   A#12#11  (partition 2 levels)

		//'#' is the symbol for get the top module name 
		size_t firstpos = modnamewithpostfix.find("#"); //get the first position of '#'
		if (firstpos != string::npos)
		{
			tmpmodname = modnamewithpostfix.substr(0, firstpos);
		}
		else
		{
			tmpmodname = modules[idnodeidmap[n]].name;
		}

		///get the number of "#"
		int symbolcount = 0;
		string tmpmodnameSymbolnum = modnamewithpostfix;
		while (1)
		{
			size_t tmppos = tmpmodnameSymbolnum.find("#");
			if (tmppos != string::npos)
			{
				tmpmodnameSymbolnum = tmpmodnameSymbolnum.substr(tmppos + 1);
				symbolcount++;
			}
			else
				break;
		}

		//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & partitionRatio & its submodule set & partitioncost   //first level (the topest level)
		////////////map<string, map<float, map<int, map<float, splitmodule> > > > topmodnamesubmodset;
		float tmppartratio = 0.5; ////partition ratio on the module
		bool tmpisvertpartition = 1; ///////????current use vertical partition (Try) rand_bool();  //judge whether it is vertical partition
		tmpmodnameratiotypepartratiosplitmodit = topmodnamesubmodset.find(tmpmodname);

		if (tmpmodnameratiotypepartratiosplitmodit == topmodnamesubmodset.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
			return;    ///this module's width is 1. cannot be split. Do nothing

		assert(tmpmodnameratiotypepartratiosplitmodit != topmodnamesubmodset.end());  //When enter split flow, should have splitted submodules  
		map<float, map<int, map<float, splitmodule> > > &tmpratiotypepartratiosplitmod = tmpmodnameratiotypepartratiosplitmodit->second;
		tmpratiotypepartratiosplitmodit = tmpratiotypepartratiosplitmod.find(tmpratio);
		assert(tmpratiotypepartratiosplitmodit != tmpratiotypepartratiosplitmod.end());   ////?????  if multi-level. should 
		map<int, map<float, splitmodule> > &tmptypepartratiosplitmod = tmpratiotypepartratiosplitmodit->second;
		tmptypepartratiosplitmodit = tmptypepartratiosplitmod.find(tmpisvertpartition);

		if (tmpratiotypepartratiosplitmodit == tmpratiotypepartratiosplitmod.end())  ///////in some specific ratios, it does not have submodules (e.g., width ==1, height == 1)
		{	/////////set a mark note ? ? if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!

			//// Save time. Need to specify a variable   (Later change time)

			return;    //do nothing and do not do packing
		}

		assert(tmptypepartratiosplitmodit != tmptypepartratiosplitmod.end());   //????has this item? if do not split. may be empty . check
		map<float, splitmodule> & tmppartratiosplitmod = tmptypepartratiosplitmodit->second;
		tmppartratiosplitmodit = tmppartratiosplitmod.find(tmppartratio);
		assert(tmppartratiosplitmodit != tmppartratiosplitmod.end());
		splitmodule tmpsplitmod = tmppartratiosplitmodit->second;

		///fbmao add: if a module is split into two modules in the first level, when do recursively partition
		///all the two modules need to be partitioned??  
		///Rule. 1.All the two modules need to do parition. or 2. one of the submodule needs to be paritioned. All are ok. 

		//		check which level it is   the number of #   ??????????????
		//	    use the name before the first #

		map<string, map<int, map<float, splitmodule> > >::iterator ita;
		map <int, map<float, splitmodule> >::iterator itb;
		map <float, splitmodule>::iterator itc;

		//first find the splitmodule specified by the partition level
		while (tmpsplitmod.partitionLevel <= symbolcount)     //if it has several levels (larger than 1)
		{
			////map<parentModulename, partitionType, splitPositionRatio, splitmodule, partitionCost(default 0.0)>
			/////map<string, map<int, map<float, splitmodule> > > parentmodnamePartypePartratioSplitmod;
			ita = tmpsplitmod.parentmodnamePartypePartratioSplitmod.begin();
			assert(ita != tmpsplitmod.parentmodnamePartypePartratioSplitmod.end());
			itb = ita->second.begin();
			assert(itb != ita->second.end());
			itc = itb->second.begin();
			assert(itc != itb->second.end());
			tmpsplitmod = itc->second;
		}


		//////Sometimes only has one submodule  ( when the another submodule is empty)
		int tmpsubmodnum = tmpsplitmod.submodset.size();
		assert(tmpsubmodnum >= 1);
		Module tmpmoda, tmpmodb;

		if (tmpsubmodnum == 2)
		{
			tmpmoda = tmpsplitmod.submodset[0]; //sub modules
			tmpmodb = tmpsplitmod.submodset[1]; //sub modules       //mod id, the number of all modules+1   
		}
		else
		{
			tmpmoda = tmpsplitmod.submodset[0]; //sub modules
		}

		//store id...    ///<parentNode, submodNodea, submodNodeb>
		map<int, pair<int, int> >::iterator parentsubmodidit = parentsubmoduleid.find(idnodeidmap[n]);
		assert(parentsubmodidit == parentsubmoduleid.end());

		if (tmpsubmodnum == 2)
		{
			parentsubmoduleid.insert(make_pair(idnodeidmap[n], make_pair(tmpmoda.id, tmpmodb.id)));
			tmpmoda.issplitmod = 1;
			tmpmodb.issplitmod = 1;
		}
		else
		{
			parentsubmoduleid.insert(make_pair(idnodeidmap[n], make_pair(tmpmoda.id, -1)));
			tmpmoda.issplitmod = 1;
		}

		Node tmpnodea;
		tmpnodea.id = tmpmoda.id;   //module id is from construction or 
		assert(tmpnodea.id >= 0);

		Node tmpnodeb;
		if (tmpsubmodnum == 2)
		{
			tmpnodeb.id = tmpmodb.id;
			assert(tmpnodeb.id >= 0);
		}

		//add the new nodes
		modules.insert(make_pair(tmpmoda.id, tmpmoda));
		if (tmpsubmodnum == 2)
		{
			modules.insert(make_pair(tmpmodb.id, tmpmodb));
		}

		Module_Info tmpmodinfoa, tmpmodinfob;
		modules_info.insert(make_pair(tmpmoda.id, tmpmodinfoa));
		if (tmpsubmodnum == 2)
		{
			modules_info.insert(make_pair(tmpmodb.id, tmpmodinfob));
		}

		nodes.insert(make_pair(tmpmoda.id, tmpnodea));

		if (tmpsubmodnum == 2)
		{
			nodes.insert(make_pair(tmpmodb.id, tmpnodeb));
		}

		Contour tmpcontoura, tmpcontourb;
		contour.insert(make_pair(tmpmoda.id, tmpcontoura));

		if (tmpsubmodnum == 2)
		{
			contour.insert(make_pair(tmpmodb.id, tmpcontourb));
		}

		bool isplaceleft;         ////Notice: currently only consider the one time partition (do not have recursively partition.  complex, constraint set)
		if (tmpisvertpartition)  //1: vertical partition.   0: horizontal partition
		{
			insert_node(nodes[idnodeidmap[n]], nodes[tmpnodea.id]);

			if (tmpsubmodnum == 2)
			{
				isplaceleft = false;
				insert_node(nodes[tmpnodea.id], nodes[tmpnodeb.id], isplaceleft);
			}

			delete_node(nodes[idnodeidmap[n]]);

			//for (map<string, int>::iterator tmpconstTypeIdit = modules[idnodeidmap[n]].constraintmodset.begin();
			//	tmpconstTypeIdit != modules[idnodeidmap[n]].constraintmodset.end();
			//	tmpconstTypeIdit++)
			//{

			//}

			//keep constraint relation
			if (tmpsubmodnum == 2)  ////only when it has two submodules, they have constraint relationship 
			{    ////Notice:    if splitted submodules can be move or swap or re-split. constraintmodset need to be used
				modules[tmpmoda.id].constraintmodset.insert(make_pair("right", tmpmodb.id));
				modules[tmpmodb.id].constraintmodset.insert(make_pair("parent", tmpmoda.id));
			}
		}
		else  //horizontal partition  xxxxx                      
		{
			//how to record relation among modules and nodes;
			////insert_node(nodes[idnodeidmap[n]], submodule1, isplaceleft); ///// ?? Need to judge pareent node and parentparentnode relation to determine
			////	  it is left or right child   ////	nodes[parent.right].parent = node.id;
			////	                                //determine whether it is left child or right child
			insert_node(nodes[idnodeidmap[n]], nodes[tmpnodea.id]);

			if (tmpsubmodnum == 2)
			{
				isplaceleft = true;
				insert_node(nodes[tmpnodea.id], nodes[tmpnodeb.id], isplaceleft);
			}

			delete_node(nodes[idnodeidmap[n]]);

			if (tmpsubmodnum == 2)
			{
				/////Need to update constraint relations
				modules[tmpmoda.id].constraintmodset.insert(make_pair("left", tmpmodb.id));
				modules[tmpmodb.id].constraintmodset.insert(make_pair("parent", tmpmoda.id));
			}
		}

		//delete the parent module, node etc. (the id in the module, module_info, node, contour)
		modules.erase(idnodeidmap[n]);
		modules_info.erase(idnodeidmap[n]);
		nodes.erase(idnodeidmap[n]);
		contour.erase(idnodeidmap[n]);

		////////after split update module_N
		if (tmpsubmodnum == 2)
		{
			modules_N = modules_N + 1;  /////split a module into two modules
		}
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



void B_Tree::splimodperturb()
{
	////node constraint. 
	////	1. splitted  submodule cannot be splitted again
	////	2. nodes have position constraint: split module cannot be directly perturbed
	//////     (add constraint on it. splitted modules from the same parent module must be considered together)
	////	3. recove etc. record some information. E.g.  module is not splitted .update ma<> parentxxxxxx

	//fbmao add for supporting split module (idnodeidmap is used to map index);
	///////create idnodeidmap
	vector<int>idnodeidmap;  //id-> nodeid may not continuous. eg. 1, 5, 7,  ..  vector index is 0,1,2,3
	for (map<int, Node>::iterator it = nodes.begin();
		it != nodes.end();
		it++)
	{
		idnodeidmap.push_back(it->first); //it->first== modid == nodeid
	}

	int p, n;
	n = rand() % modules_N;
	double r = rand_01();

	if (r < move_rate)   ////1. move
	{
		// delete & insert
		////tmpcountnum = 0;
		// choose different module
		do
		{
			p = rand() % modules_N;
		} while (n == p);

		//////1.1
		if (!modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)
		{
			delete_node(nodes[idnodeidmap[n]]);
			insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
		}
		else if (modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod) ///1.2
		{
			////first get another module that related to modules[idnodeidmap[n]
			if (modules[idnodeidmap[n]].constraintmodset.empty())  ///after split, only has one module
			{  ///after split,  only has one module
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else   ////non-empty
			{
				int tmpanothermodid = -1;
				string tmpright = "right";
				string tmpparent = "parent";
				map<string, int> ::iterator tmpstringintita =
					modules[idnodeidmap[n]].constraintmodset.find(tmpright);
				if (tmpstringintita == modules[idnodeidmap[n]].constraintmodset.end())
				{
					map<string, int> ::iterator tmpstringintitb =
						modules[idnodeidmap[n]].constraintmodset.find(tmpparent);
					assert(tmpstringintitb != modules[idnodeidmap[n]].constraintmodset.end());
					////parent mod id
					tmpanothermodid = tmpstringintitb->second;

					////operation
					delete_node(nodes[tmpanothermodid]);
					insert_node(nodes[idnodeidmap[p]], nodes[tmpanothermodid]);

					delete_node(nodes[idnodeidmap[n]]);
					bool tmpisplaceleft = false;   ////it can be random???? Not need to be left or right??
					///acutllay, the split module only has constraint (ajacent to each other)
					insert_node(nodes[tmpanothermodid], nodes[idnodeidmap[n]], tmpisplaceleft);
				}
				else
				{   /////right child id
					tmpanothermodid = tmpstringintita->second;
					delete_node(nodes[idnodeidmap[n]]);
					insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

					delete_node(nodes[tmpanothermodid]);
					bool tmpisplaceleft = false;
					insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodid], tmpisplaceleft);
				}
			}
		}
		else if (!modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod)   /////1.3
		{
			//////cannot impact the splitted modules relationship after moving new module to connect to splitted module
			/////First check whether another module (adjacent to the splitted module) 
			if (modules[idnodeidmap[p]].constraintmodset.empty())
			{
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else   ////non-empty
			{
				int tmpanothermodid = -1;
				string tmpright = "right";
				string tmpparent = "parent";
				map<string, int> ::iterator tmpstringintita =
					modules[idnodeidmap[p]].constraintmodset.find(tmpright);
				if (tmpstringintita == modules[idnodeidmap[p]].constraintmodset.end())
				{
					map<string, int> ::iterator tmpstringintitb =
						modules[idnodeidmap[p]].constraintmodset.find(tmpparent);
					assert(tmpstringintitb != modules[idnodeidmap[p]].constraintmodset.end());
					////parent mod id
					tmpanothermodid = tmpstringintitb->second;

					////operation  (??Whether has potential bug)
					delete_node(nodes[idnodeidmap[n]]);
					insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

					//////delete_node(nodes[idnodeidmap[n]]);
					//////insert_node(nodes[idnodeidmap[tmpanothermodid]], nodes[idnodeidmap[n]]);


					//////delete_node(nodes[idnodeidmap[n]]);
					//////insert_node(nodes[idnodeidmap[tmpanothermodid]], nodes[idnodeidmap[n]]);

					//////delete_node(nodes[idnodeidmap[p]]);
					//////bool tmpisplaceleft = false;
					/////////acutllay, the split module only has constraint (ajacent to each other)
					//////insert_node(nodes[idnodeidmap[tmpanothermodid]], nodes[idnodeidmap[p]], tmpisplaceleft);
				}
				else
				{   /////right child id
					tmpanothermodid = tmpstringintita->second;

					delete_node(nodes[idnodeidmap[n]]);

					////insert from the right child  in order to do not impact relation of splitted modules
					insert_node(nodes[tmpanothermodid], nodes[idnodeidmap[n]]);
				}
			}
		}
		else   /////1.4  all are splitted module. modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod
		{
			if (modules[idnodeidmap[n]].constraintmodset.empty() && modules[idnodeidmap[p]].constraintmodset.empty())
			{
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else if (!modules[idnodeidmap[n]].constraintmodset.empty() && modules[idnodeidmap[p]].constraintmodset.empty())
			{
				int tmpanothermodid = -1;
				string tmpright = "right";
				string tmpparent = "parent";
				map<string, int> ::iterator tmpstringintita =
					modules[idnodeidmap[n]].constraintmodset.find(tmpright);
				if (tmpstringintita == modules[idnodeidmap[n]].constraintmodset.end())
				{
					map<string, int> ::iterator tmpstringintitb =
						modules[idnodeidmap[n]].constraintmodset.find(tmpparent);
					assert(tmpstringintitb != modules[idnodeidmap[n]].constraintmodset.end());
					////parent mod id
					tmpanothermodid = tmpstringintitb->second;

					////operation
					delete_node(nodes[tmpanothermodid]);
					insert_node(nodes[idnodeidmap[p]], nodes[tmpanothermodid]);

					delete_node(nodes[idnodeidmap[n]]);
					bool tmpisplaceleft = false;
					///acutllay, the split module only has constraint (ajacent to each other)
					insert_node(nodes[tmpanothermodid], nodes[idnodeidmap[n]], tmpisplaceleft);
				}
				else
				{   /////right child id
					tmpanothermodid = tmpstringintita->second;
					delete_node(nodes[idnodeidmap[n]]);
					insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

					delete_node(nodes[tmpanothermodid]);
					bool tmpisplaceleft = false;
					insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodid], tmpisplaceleft);
				}
			}
			else if (modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				//////cannot impact the splitted modules relationship after moving new module to connect to splitted module
				/////First check whether another module (adjacent to the splitted module) 
				////non-empty
				int tmpanothermodid = -1;
				string tmpright = "right";
				string tmpparent = "parent";
				map<string, int> ::iterator tmpstringintita =
					modules[idnodeidmap[p]].constraintmodset.find(tmpright);
				if (tmpstringintita == modules[idnodeidmap[p]].constraintmodset.end())
				{
					map<string, int> ::iterator tmpstringintitb =
						modules[idnodeidmap[p]].constraintmodset.find(tmpparent);
					assert(tmpstringintitb != modules[idnodeidmap[p]].constraintmodset.end());
					////parent mod id
					tmpanothermodid = tmpstringintitb->second;

					////operation
					delete_node(nodes[idnodeidmap[n]]);
					insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

					////////operation
					////delete_node(nodes[idnodeidmap[n]]);
					////insert_node(nodes[idnodeidmap[tmpanothermodid]], nodes[idnodeidmap[n]]);

					////delete_node(nodes[idnodeidmap[p]]);
					////bool tmpisplaceleft = false;
					///////acutllay, the split module only has constraint (ajacent to each other)
					////insert_node(nodes[idnodeidmap[tmpanothermodid]], nodes[idnodeidmap[p]], tmpisplaceleft);
				}
				else
				{   /////right child id
					tmpanothermodid = tmpstringintita->second;
					delete_node(nodes[idnodeidmap[n]]);

					////insert from the right child  in order to do not impact relation of splitted modules
					insert_node(nodes[tmpanothermodid], nodes[idnodeidmap[n]]);
				}
			}
			else ///////////(!modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				int tmpanothermodida = -1;
				int tmpanothermodidc = -1;
				string tmpright = "right";
				string tmpparent = "parent";
				map<string, int> ::iterator tmpstringintita =
					modules[idnodeidmap[n]].constraintmodset.find(tmpright);
				if (tmpstringintita == modules[idnodeidmap[n]].constraintmodset.end())
				{
					map<string, int> ::iterator tmpstringintitb =
						modules[idnodeidmap[n]].constraintmodset.find(tmpparent);
					assert(tmpstringintitb != modules[idnodeidmap[n]].constraintmodset.end());
					////parent mod id
					tmpanothermodida = tmpstringintitb->second;

					map<string, int> ::iterator tmpstringintitc =
						modules[idnodeidmap[p]].constraintmodset.find(tmpright);
					if (tmpstringintitc == modules[idnodeidmap[p]].constraintmodset.end())
					{
						map<string, int> ::iterator tmpstringintitd =
							modules[idnodeidmap[p]].constraintmodset.find(tmpparent);
						assert(tmpstringintitd != modules[idnodeidmap[p]].constraintmodset.end());
						////////parent mod id
						tmpanothermodidc = tmpstringintitd->second;

						///move operation
						delete_node(nodes[tmpanothermodida]);
						insert_node(nodes[idnodeidmap[p]], nodes[tmpanothermodida]);

						delete_node(nodes[idnodeidmap[n]]);
						insert_node(nodes[tmpanothermodida], nodes[idnodeidmap[n]], false);
					}
					else
					{   ///right child
						tmpanothermodidc = tmpstringintitc->second;

						///move operation
						delete_node(nodes[tmpanothermodida]);
						insert_node(nodes[tmpanothermodidc], nodes[tmpanothermodida]);

						delete_node(nodes[idnodeidmap[n]]);
						insert_node(nodes[tmpanothermodida], nodes[idnodeidmap[n]], false);
					}
				}
				else
				{   /////right child id
					tmpanothermodida = tmpstringintita->second;

					map<string, int> ::iterator tmpstringintitc =
						modules[idnodeidmap[p]].constraintmodset.find(tmpright);
					if (tmpstringintitc == modules[idnodeidmap[p]].constraintmodset.end())
					{
						map<string, int> ::iterator tmpstringintitd =
							modules[idnodeidmap[p]].constraintmodset.find(tmpparent);
						assert(tmpstringintitd != modules[idnodeidmap[p]].constraintmodset.end());
						////////parent mod id
						tmpanothermodidc = tmpstringintitd->second;

						///move operation
						delete_node(nodes[idnodeidmap[n]]);
						insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

						delete_node(nodes[tmpanothermodida]);
						insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodida], false);
					}
					else
					{   ///right child
						tmpanothermodidc = tmpstringintitc->second;

						///move operation
						delete_node(nodes[idnodeidmap[n]]);
						insert_node(nodes[tmpanothermodidc], nodes[idnodeidmap[n]]);

						delete_node(nodes[tmpanothermodida]);
						insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodida], false);
					}
				}
			}
		}
	}
	else if (r < swap_rate)   //2. swap
	{														// swap node
		// choose different module
		do
		{
			p = rand() % modules_N;                 /////later pick up from non-splitmodule list
		} while (n == p);

		if (!modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)  ///2.1
		{
			swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
		}
		else if (modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)  ////2.2
		{
			if (modules[idnodeidmap[n]].constraintmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else
			{
				int tmpanothermodida = -1;
				string tmpright = "right";
				string tmpparent = "parent";
				map<string, int> ::iterator tmpstringintita =
					modules[idnodeidmap[n]].constraintmodset.find(tmpright);
				if (tmpstringintita == modules[idnodeidmap[n]].constraintmodset.end())
				{
					map<string, int> ::iterator tmpstringintitb =
						modules[idnodeidmap[n]].constraintmodset.find(tmpparent);
					assert(tmpstringintitb != modules[idnodeidmap[n]].constraintmodset.end());
					////parent mod id
					tmpanothermodida = tmpstringintitb->second;

					swap_node2(nodes[idnodeidmap[p]], nodes[tmpanothermodida]);

					delete_node(nodes[idnodeidmap[n]]);
					insert_node(nodes[tmpanothermodida], nodes[idnodeidmap[n]], false);
				}
				else
				{   /////right child id
					tmpanothermodida = tmpstringintita->second;

					swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

					delete_node(nodes[tmpanothermodida]);
					insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodida], false);
				}
			}
		}
		else if (!modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod)  /////2.3
		{
			if (modules[idnodeidmap[p]].constraintmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else
			{
				int tmpanothermodida = -1;
				string tmpright = "right";
				string tmpparent = "parent";
				map<string, int> ::iterator tmpstringintita =
					modules[idnodeidmap[p]].constraintmodset.find(tmpright);
				if (tmpstringintita == modules[idnodeidmap[p]].constraintmodset.end())
				{
					map<string, int> ::iterator tmpstringintitb =
						modules[idnodeidmap[p]].constraintmodset.find(tmpparent);
					assert(tmpstringintitb != modules[idnodeidmap[n]].constraintmodset.end());
					////parent mod id
					tmpanothermodida = tmpstringintitb->second;


					swap_node2(nodes[tmpanothermodida], nodes[idnodeidmap[n]]);

					delete_node(nodes[idnodeidmap[p]]);
					insert_node(nodes[tmpanothermodida], nodes[idnodeidmap[p]], false);
				}
				else
				{   /////right child id
					tmpanothermodida = tmpstringintita->second;

					swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

					delete_node(nodes[tmpanothermodida]);
					insert_node(nodes[idnodeidmap[p]], nodes[tmpanothermodida], false);
				}
			}
		}
		else   /////modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod ///2.4      
		{
			if (modules[idnodeidmap[n]].constraintmodset.empty() && modules[idnodeidmap[p]].constraintmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else if (!modules[idnodeidmap[n]].constraintmodset.empty() && modules[idnodeidmap[p]].constraintmodset.empty())
			{
				int tmpanothermodida = -1;
				string tmpright = "right";
				string tmpparent = "parent";
				map<string, int> ::iterator tmpstringintita =
					modules[idnodeidmap[n]].constraintmodset.find(tmpright);
				if (tmpstringintita == modules[idnodeidmap[n]].constraintmodset.end())
				{
					map<string, int> ::iterator tmpstringintitb =
						modules[idnodeidmap[n]].constraintmodset.find(tmpparent);
					assert(tmpstringintitb != modules[idnodeidmap[n]].constraintmodset.end());
					////parent mod id
					tmpanothermodida = tmpstringintitb->second;

					swap_node2(nodes[idnodeidmap[p]], nodes[tmpanothermodida]);

					delete_node(nodes[idnodeidmap[n]]);
					insert_node(nodes[tmpanothermodida], nodes[idnodeidmap[n]], false);
				}
				else
				{   /////right child id
					tmpanothermodida = tmpstringintita->second;

					swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

					delete_node(nodes[tmpanothermodida]);
					insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodida], false);
				}
			}
			else if (modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				int tmpanothermodida = -1;
				string tmpright = "right";
				string tmpparent = "parent";
				map<string, int> ::iterator tmpstringintita =
					modules[idnodeidmap[p]].constraintmodset.find(tmpright);
				if (tmpstringintita == modules[idnodeidmap[p]].constraintmodset.end())
				{
					map<string, int> ::iterator tmpstringintitb =
						modules[idnodeidmap[p]].constraintmodset.find(tmpparent);
					assert(tmpstringintitb != modules[idnodeidmap[p]].constraintmodset.end());
					////parent mod id
					tmpanothermodida = tmpstringintitb->second;

					swap_node2(nodes[tmpanothermodida], nodes[idnodeidmap[n]]);

					delete_node(nodes[idnodeidmap[p]]);
					insert_node(nodes[tmpanothermodida], nodes[idnodeidmap[p]], false);
				}
				else
				{   /////right child id
					tmpanothermodida = tmpstringintita->second;

					swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

					delete_node(nodes[tmpanothermodida]);
					insert_node(nodes[idnodeidmap[p]], nodes[tmpanothermodida], false);
				}
			}
			else ///////////(!modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				int tmpanothermodida = -1;
				int tmpanothermodidc = -1;
				string tmpright = "right";
				string tmpparent = "parent";
				map<string, int> ::iterator tmpstringintita =
					modules[idnodeidmap[n]].constraintmodset.find(tmpright);
				if (tmpstringintita == modules[idnodeidmap[n]].constraintmodset.end())
				{
					map<string, int> ::iterator tmpstringintitb =
						modules[idnodeidmap[n]].constraintmodset.find(tmpparent);
					assert(tmpstringintitb != modules[idnodeidmap[n]].constraintmodset.end());
					////parent mod id
					tmpanothermodida = tmpstringintitb->second;

					map<string, int> ::iterator tmpstringintitc =
						modules[idnodeidmap[p]].constraintmodset.find(tmpright);
					if (tmpstringintitc == modules[idnodeidmap[p]].constraintmodset.end())
					{
						map<string, int> ::iterator tmpstringintitd =
							modules[idnodeidmap[p]].constraintmodset.find(tmpparent);
						assert(tmpstringintitd != modules[idnodeidmap[p]].constraintmodset.end());
						////////parent mod id
						tmpanothermodidc = tmpstringintitd->second;

						///swap operation
						swap_node2(nodes[tmpanothermodidc], nodes[tmpanothermodida]);
						swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
					}
					else
					{   ///right child
						tmpanothermodidc = tmpstringintitc->second;

						///swap operation
						swap_node2(nodes[idnodeidmap[p]], nodes[tmpanothermodida]);
						swap_node2(nodes[tmpanothermodidc], nodes[idnodeidmap[n]]);
					}
				}
				else
				{   /////right child id
					tmpanothermodida = tmpstringintita->second;

					map<string, int> ::iterator tmpstringintitc =
						modules[idnodeidmap[p]].constraintmodset.find(tmpright);
					if (tmpstringintitc == modules[idnodeidmap[p]].constraintmodset.end())
					{
						map<string, int> ::iterator tmpstringintitd =
							modules[idnodeidmap[p]].constraintmodset.find(tmpparent);
						assert(tmpstringintitd != modules[idnodeidmap[p]].constraintmodset.end());
						////////parent mod id
						tmpanothermodidc = tmpstringintitd->second;

						///swap operation
						swap_node2(nodes[tmpanothermodidc], nodes[idnodeidmap[n]]);
						swap_node2(nodes[idnodeidmap[p]], nodes[tmpanothermodida]);
					}
					else
					{   ///right child
						tmpanothermodidc = tmpstringintitc->second;

						///swap operation
						///swap operation
						swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
						swap_node2(nodes[tmpanothermodidc], nodes[tmpanothermodida]);
					}
				}
			}
		}
	}
	else ///3.  support split operation (fbmao add).  (Only when the module is non-splitted module, we can split the module)
	{	//a module is split into two submodules  <--> a node is split into two nodes and has position constraint
		//change structure.   map<int, node>  keep original module id. node id (in order to recover to original state). removed modules (original) also are needed to be recorded.
		//split module n (n is the node id equal to the module id, contour id)

		//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & partitionRatio & its submodule set & partitioncost   //first level (the topest level)
		////////////map<string, map<float, map<int, map<float, splitmodule> > > > topmodnamesubmodset;	

		////if split module number === original number of modules   means all the original modules are be splitted.  In this case, 
		///// change the move_rate and swap rate  (since do not have split operation).  

		bool hasnonsplitmod = false;
		for (map<int, Module>::iterator it = modules.begin();
			it != modules.end();
			it++)
		{
			int i = it->first;
			if (!modules[i].issplitmod)   ////if has non-split module
			{
				hasnonsplitmod = true;
				break;
			}
		}

		///if all the splitted module are splitted modules.  remove the split operation  (only has move and swap)
		if (false == hasnonsplitmod) ///non-splitmodule does not exist
		{
			move_rate = 0.5;
			swap_rate = 1.0;
			return;    /////do nothing
		}

		///Notice. Currrently module only can be split once.
		///pick up non-split module from the unsplit modules.
		////choose a non-split module.    ////???  if all are splitted modules, keep nodes' constraint (find a way to perturb solution)
		int n1 = -1; //temporary initial value
		int tmpcountnum = 0;
		while (modules[idnodeidmap[n]].issplitmod)    ////use a global count to compare with the original number of top modules.
		{
			if (tmpcountnum >= modules_N)
			{
				break;
			}
			n1 = rand() % modules_N;
			if (n != n1)
			{
				tmpcountnum++;
				n = n1;
			}
		}

		if (tmpcountnum >= modules_N)
		{
			return;   //currently, all the modules are splitted module. do nothing [Cannot use the split operation].
		}

		map<string, map<float, map<int, map<float, splitmodule> > > >::iterator tmpmodnameratiotypepartratiosplitmodit; //used these iterator below
		map<float, map<int, map<float, splitmodule> > >::iterator tmpratiotypepartratiosplitmodit;
		map<int, map<float, splitmodule> >::iterator tmptypepartratiosplitmodit;
		map<float, splitmodule>::iterator tmppartratiosplitmodit;

		//Currenlty, the module has been split, cannot return back
		string tmpmodname;    //current module's parent name (the most original name by running the VPR)
		string modnamewithpostfix; //current modules' full name
		//split module idnodeidmap[n]

		////////(module) idnodeidmap[n] will be split 
		float tmpratio = modules[idnodeidmap[n]].ratio;  //Each time need to record the startx, endx. etc??.
		modnamewithpostfix = modules[idnodeidmap[n]].name;  //eg.  A#11#11   A#12#11  (partition 2 levels)

		//'#' is the symbol for get the top module name 
		size_t firstpos = modnamewithpostfix.find("#"); //get the first position of '#'
		if (firstpos != string::npos)
		{
			tmpmodname = modnamewithpostfix.substr(0, firstpos);
		}
		else
		{
			tmpmodname = modules[idnodeidmap[n]].name;
		}

		///get the number of "#"
		int symbolcount = 0;
		string tmpmodnameSymbolnum = modnamewithpostfix;
		while (1)
		{
			size_t tmppos = tmpmodnameSymbolnum.find("#");
			if (tmppos != string::npos)
			{
				tmpmodnameSymbolnum = tmpmodnameSymbolnum.substr(tmppos + 1);
				symbolcount++;
			}
			else
				break;
		}

		//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & partitionRatio & its submodule set & partitioncost   //first level (the topest level)
		////////////map<string, map<float, map<int, map<float, splitmodule> > > > topmodnamesubmodset;
		float tmppartratio = 0.5; ////partition ratio on the module
		bool tmpisvertpartition = 1; ///////????current use vertical partition (Try) rand_bool();  //judge whether it is vertical partition
		tmpmodnameratiotypepartratiosplitmodit = topmodnamesubmodset.find(tmpmodname);

		if (tmpmodnameratiotypepartratiosplitmodit == topmodnamesubmodset.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
		{
			//////set a mark note??  if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!
			//////	Need to update the code in allow perturbsplitmod

			//// Save time. Need to specify a variable   (Later change time)
			return;    ///this module's width is 1. cannot be split. Do nothing
		}

		assert(tmpmodnameratiotypepartratiosplitmodit != topmodnamesubmodset.end());  //When enter split flow, should have splitted submodules  
		map<float, map<int, map<float, splitmodule> > > &tmpratiotypepartratiosplitmod = tmpmodnameratiotypepartratiosplitmodit->second;
		tmpratiotypepartratiosplitmodit = tmpratiotypepartratiosplitmod.find(tmpratio);

		if (tmpmodnameratiotypepartratiosplitmodit == topmodnamesubmodset.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
		{
			/////////set a mark note ? ? if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!

			//// Save time. Need to specify a variable   (Later change time)
			return;    //do nothing and do not do packing
		}

		assert(tmpratiotypepartratiosplitmodit != tmpratiotypepartratiosplitmod.end());   ////?????  if multi-level. should 
		map<int, map<float, splitmodule> > &tmptypepartratiosplitmod = tmpratiotypepartratiosplitmodit->second;
		tmptypepartratiosplitmodit = tmptypepartratiosplitmod.find(tmpisvertpartition);
		assert(tmptypepartratiosplitmodit != tmptypepartratiosplitmod.end());   //????has this item? if do not split. may be empty . check
		map<float, splitmodule> & tmppartratiosplitmod = tmptypepartratiosplitmodit->second;
		tmppartratiosplitmodit = tmppartratiosplitmod.find(tmppartratio);
		assert(tmppartratiosplitmodit != tmppartratiosplitmod.end());
		splitmodule tmpsplitmod = tmppartratiosplitmodit->second;

		///fbmao add: if a module is split into two modules in the first level, when do recursively partition
		///all the two modules need to be partitioned??  
		///Rule. 1.All the two modules need to do parition. or 2. one of the submodule needs to be paritioned. All are ok. 

		//		check which level it is   the number of #   ??????????????
		//	    use the name before the first #

		map<string, map<int, map<float, splitmodule> > >::iterator ita;
		map <int, map<float, splitmodule> >::iterator itb;
		map <float, splitmodule>::iterator itc;

		//first find the splitmodule specified by the partition level
		while (tmpsplitmod.partitionLevel <= symbolcount)     //if it has several levels (larger than 1)
		{
			////map<parentModulename, partitionType, splitPositionRatio, splitmodule, partitionCost(default 0.0)>
			/////map<string, map<int, map<float, splitmodule> > > parentmodnamePartypePartratioSplitmod;
			ita = tmpsplitmod.parentmodnamePartypePartratioSplitmod.begin();
			assert(ita != tmpsplitmod.parentmodnamePartypePartratioSplitmod.end());
			itb = ita->second.begin();
			assert(itb != ita->second.end());
			itc = itb->second.begin();
			assert(itc != itb->second.end());
			tmpsplitmod = itc->second;
		}


		//////Sometimes only has one submodule  ( when the another submodule is empty)
		int tmpsubmodnum = tmpsplitmod.submodset.size();
		assert(tmpsubmodnum >= 1);
		Module tmpmoda, tmpmodb;

		if (tmpsubmodnum == 2)
		{
			tmpmoda = tmpsplitmod.submodset[0]; //sub modules
			tmpmodb = tmpsplitmod.submodset[1]; //sub modules       //mod id, the number of all modules+1   
		}
		else
		{
			tmpmoda = tmpsplitmod.submodset[0]; //sub modules
		}

		//store id...    ///<parentNode, submodNodea, submodNodeb>
		map<int, pair<int, int> >::iterator parentsubmodidit = parentsubmoduleid.find(idnodeidmap[n]);
		assert(parentsubmodidit == parentsubmoduleid.end());

		if (tmpsubmodnum == 2)
		{
			parentsubmoduleid.insert(make_pair(idnodeidmap[n], make_pair(tmpmoda.id, tmpmodb.id)));
			tmpmoda.issplitmod = 1;
			tmpmodb.issplitmod = 1;
		}
		else
		{
			parentsubmoduleid.insert(make_pair(idnodeidmap[n], make_pair(tmpmoda.id, -1)));
			tmpmoda.issplitmod = 1;
		}

		Node tmpnodea;
		tmpnodea.id = tmpmoda.id;   //module id is from construction or 
		assert(tmpnodea.id >= 0);

		Node tmpnodeb;
		if (tmpsubmodnum == 2)
		{
			tmpnodeb.id = tmpmodb.id;
			assert(tmpnodeb.id >= 0);
		}

		//add the new nodes
		modules.insert(make_pair(tmpmoda.id, tmpmoda));
		if (tmpsubmodnum == 2)
		{
			modules.insert(make_pair(tmpmodb.id, tmpmodb));
		}

		Module_Info tmpmodinfoa, tmpmodinfob;
		modules_info.insert(make_pair(tmpmoda.id, tmpmodinfoa));
		if (tmpsubmodnum == 2)
		{
			modules_info.insert(make_pair(tmpmodb.id, tmpmodinfob));
		}

		nodes.insert(make_pair(tmpmoda.id, tmpnodea));

		if (tmpsubmodnum == 2)
		{
			nodes.insert(make_pair(tmpmodb.id, tmpnodeb));
		}

		Contour tmpcontoura, tmpcontourb;
		contour.insert(make_pair(tmpmoda.id, tmpcontoura));

		if (tmpsubmodnum == 2)
		{
			contour.insert(make_pair(tmpmodb.id, tmpcontourb));
		}

		bool isplaceleft;         ////Notice: currently only consider the one time partition (do not have recursively partition.  complex, constraint set)
		if (tmpisvertpartition)  //1: vertical partition.   0: horizontal partition
		{
			insert_node(nodes[idnodeidmap[n]], nodes[tmpnodea.id]);

			if (tmpsubmodnum == 2)
			{
				isplaceleft = false;
				insert_node(nodes[tmpnodea.id], nodes[tmpnodeb.id], isplaceleft);
			}

			delete_node(nodes[idnodeidmap[n]]);

			//for (map<string, int>::iterator tmpconstTypeIdit = modules[idnodeidmap[n]].constraintmodset.begin();
			//	tmpconstTypeIdit != modules[idnodeidmap[n]].constraintmodset.end();
			//	tmpconstTypeIdit++)
			//{

			//}

			//keep constraint relation
			if (tmpsubmodnum == 2)  ////only when it has two submodules, they have constraint relationship 
			{    ////Notice:    if splitted submodules can be move or swap or re-split. constraintmodset need to be used
				modules[tmpmoda.id].constraintmodset.insert(make_pair("right", tmpmodb.id));
				modules[tmpmodb.id].constraintmodset.insert(make_pair("parent", tmpmoda.id));
			}
		}
		else  //horizontal partition  xxxxx                      
		{
			//how to record relation among modules and nodes;
			////insert_node(nodes[idnodeidmap[n]], submodule1, isplaceleft); ///// ?? Need to judge pareent node and parentparentnode relation to determine
			////	  it is left or right child   ////	nodes[parent.right].parent = node.id;
			////	                                //determine whether it is left child or right child
			insert_node(nodes[idnodeidmap[n]], nodes[tmpnodea.id]);

			if (tmpsubmodnum == 2)
			{
				isplaceleft = true;
				insert_node(nodes[tmpnodea.id], nodes[tmpnodeb.id], isplaceleft);
			}

			delete_node(nodes[idnodeidmap[n]]);

			if (tmpsubmodnum == 2)
			{
				/////Need to update constraint relations
				modules[tmpmoda.id].constraintmodset.insert(make_pair("left", tmpmodb.id));
				modules[tmpmodb.id].constraintmodset.insert(make_pair("parent", tmpmoda.id));
			}
		}

		//delete the parent module, node etc. (the id in the module, module_info, node, contour)
		modules.erase(idnodeidmap[n]);
		modules_info.erase(idnodeidmap[n]);
		nodes.erase(idnodeidmap[n]);
		contour.erase(idnodeidmap[n]);

		////////after split update module_N
		if (tmpsubmodnum == 2)
		{
			modules_N = modules_N + 1;  /////split a module into two modules
		}
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







/////Dynamic partition module during runtime.  <3> for the same architecture.
void B_Tree::dynamicsplimodperturb()
{    ///Function: Dynamic partition.  (partition module during placement (runtime). Also need to decide. split module into two parts or three parts? four parts??)
	////node constraint. 
	////	1. splitted submodule cannot be splitted again
	////	2. nodes have position constraint: split module cannot be directly perturbed
	//////     (add constraint on it. splitted modules from the same parent module must be considered together(put together). They have some combinations)
	////	3. recove etc. record some information. E.g.  module is not splitted .update ma<> parentxxxxxx

	//fbmao add for supporting split module (idnodeidmap is used to map index);
	///////create idnodeidmap
	vector<int>idnodeidmap;  //id-> nodeid may not continuous. eg. 1, 5, 7,  ..  vector index Id is 0,1,2,3
	map<int, int> tmpmodidindexmap;     ////////[modid == nodeid]
	int tmpindex = 0;
	for (map<int, Node>::iterator it = nodes.begin();
		it != nodes.end();
		it++)
	{
		idnodeidmap.push_back(it->first); //it->first== modid == nodeid
		tmpmodidindexmap.insert(make_pair(it->first, tmpindex));
		tmpindex++;
	}

	int p, n;
	n = rand() % modules_N;
	double r = rand_01();

	int tmpanothermodid = -1; ///global variable in this function
	int tmporinode = -1;
	bool tmpisplaceleft, tmpisplacelefta, tmpisplaceleftb;

	int tmpanothermodida = -1;
	int tmporinodea = -1;

	int tmpanothermodidb = -1;
	int tmporinodeb = -1;

	if (r < move_rate)   ////1. move
	{
		// delete & insert
		////tmpcountnum = 0;
		// choose different module
		do
		{
			p = rand() % modules_N;
		} while (n == p);

		//////1.1
		if (!modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)
		{
			delete_node(nodes[idnodeidmap[n]]);
			insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
		}
		else if (modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod) ///1.2
		{
			////first get another module that related to modules[idnodeidmap[n]
			if (modules[idnodeidmap[n]].adjacentmodset.empty())  ///after split, only has one module
			{  ///after split,  only has one module
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else   ////non-empty
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module

				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[n];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);

					////if a module is split module. need to get the first submodule. (submodule is the parenent submodule. may more easier for routing?????)
					tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;  //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}
			}
		}
		else if (!modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod)   /////1.3
		{
			//////cannot impact the splitted modules relationship after moving new module to connect to splitted module
			/////First check whether another module (adjacent to the splitted module) 
			if (modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else   ////non-empty
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);


				///deal with other submodules of nodes[idnodeidmap[p]]
				tmpanothermodid = -1;
				tmporinode = idnodeidmap[p];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[p]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;     //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}
			}
		}
		else   /////1.4  all are splitted module. modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod
		{
			if (modules[idnodeidmap[n]].adjacentmodset.empty() && modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else if (!modules[idnodeidmap[n]].adjacentmodset.empty() && modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module

				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[n];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;  //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}

			}
			else if (modules[idnodeidmap[n]].adjacentmodset.empty() && !modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				//////cannot impact the splitted modules relationship (after moving new module to connect to splitted module)
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				///deal with other submodules of nodes[idnodeidmap[p]]
				tmpanothermodid = -1;
				tmporinode = idnodeidmap[p];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[p]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;     //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}
			}
			else ///////////(!modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodida = -1;
				tmporinodea = idnodeidmap[n];
				for (int tmpida = 0; tmpida < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpida++)
				{
					tmpanothermodida = modules[idnodeidmap[n]].adjacentmodset[tmpida];
					delete_node(nodes[tmpanothermodida]);
					tmpisplacelefta = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinodea], nodes[tmpanothermodida], tmpisplacelefta);

					tmporinodea = tmpanothermodida;  //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}

				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				///deal with other submodules of nodes[idnodeidmap[p]]
				tmpanothermodidb = -1;  /////////////?????????????? tmpanothermodid tmporinode can use a variable.
				tmporinodeb = idnodeidmap[p];
				for (int tmpidb = 0; tmpidb < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpidb++)
				{
					tmpanothermodidb = modules[idnodeidmap[p]].adjacentmodset[tmpidb];
					delete_node(nodes[tmpanothermodidb]);
					tmpisplaceleftb = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinodeb], nodes[tmpanothermodidb], tmpisplaceleftb);

					tmporinodeb = tmpanothermodidb;     //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}
			}
		}
	}
	else if (r < swap_rate)   //2. swap   
	{														// swap node
		// choose different module
		do
		{
			p = rand() % modules_N;                 /////later pick up from non-splitmodule list
		} while (n == p);

		if (!modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)  ///2.1
		{
			swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
		}
		else if (modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)  ////2.2
		{
			if (modules[idnodeidmap[n]].constraintmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[n];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;
				}
			}
		}
		else if (!modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod)  /////2.3
		{
			if (modules[idnodeidmap[p]].constraintmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module	
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[p];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[p]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;
				}
			}
		}
		else   /////modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod ///2.4      
		{
			if (modules[idnodeidmap[n]].adjacentmodset.empty() && modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else if (!modules[idnodeidmap[n]].adjacentmodset.empty() && modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[n];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;
				}
			}
			else if (modules[idnodeidmap[n]].adjacentmodset.empty() && !modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module	
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[p];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[p]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;
				}
			}
			else ///////////(!modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodida = -1;
				tmporinodea = idnodeidmap[n];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodida = modules[idnodeidmap[n]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodida]);
					tmpisplacelefta = rand_bool();
					insert_node(nodes[tmporinodea], nodes[tmpanothermodida], tmpisplacelefta);

					tmporinodea = tmpanothermodida;
				}

				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module	
				int tmpanothermodidb = -1;
				int tmporinodeb = idnodeidmap[p];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodidb = modules[idnodeidmap[p]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodidb]);
					tmpisplaceleftb = rand_bool();
					insert_node(nodes[tmporinodeb], nodes[tmpanothermodidb], tmpisplaceleftb);

					tmporinodeb = tmpanothermodidb;
				}
			}
		}
	}
	else ///3. split operation (fbmao add).  (Only when the module is non-splitted module, we can split the module)
	{	//a module is split into several submodules  <--> a node is split into several nodes and has position constraint
		//change structure.   map<int, node>  keep original module id. node id (in order to recover to original state). removed modules (original) also are needed to be recorded.
		//split module n (n is the node id equal to the module id, contour id)

		//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & partitionRatio & its submodule set & partitioncost   //first level (the topest level)
		////////////map<string, map<float, map<int, map<float, splitmodule> > > > topmodnamesubmodset;	

		////if split module number === original number of modules   means all the original modules are be splitted.  In this case, 
		///// change the move_rate and swap rate  (since do not need to have split operation). 


		////need to check whether this module can be split. it is splitted into two parts or three parts????????????????????????
		bool hasnonsplitmod = false;
		for (map<int, Module>::iterator it = modules.begin();
			it != modules.end();
			it++)
		{
			int i = it->first;
			if (!modules[i].issplitmod)   ////if has non-split module
			{
				hasnonsplitmod = true;
				break;
			}
		}

		///if all the splitted module are splitted modules.  remove the split operation  (only has move and swap)
		if (false == hasnonsplitmod) ///non-splitmodule does not exist
		{
			move_rate = 0.5;  ///change the probability for swap operations
			swap_rate = 1.0;   ///when all the modules are splitted module, will do not have split. 

			perturb();  //split do nothing, so do perturb() again
			return;    /////do nothing
		}


		///Notice. Currrently module only can be split once.
		///pick up non-split module from the unsplit modules.
		////choose a non-split module.   
		////////int n1 = -1; //temporary initial value
		////////int tmpcountnum = 0;
		///////////find the non-split module
		////////while (modules[idnodeidmap[n]].issplitmod)    ////use a global count to compare with the original number of top modules.
		////////{
		////////	n1 = rand() % modules_N;
		////////	if (n != n1)
		////////	{
		////////		tmpcountnum++;
		////////		n = n1;
		////////	}
		////////}
        ////////    assert(!modules[idnodeidmap[n]].issplitmod);

		////find the module need to be split


		///1.1 Module selection
		///Pick up a non-split module for partition
		double tmpoverallwastearea = Area - TotalArea;
		int tmpemptywidth = sqrt(tmpoverallwastearea);
		int tmpemptyheight = tmpemptywidth;
		vector<int> largeareamodvect;  //module area larger than current overall empty space
		largeareamodvect.clear();
		///collect the modules which area larger than the tmpoverallwastearea
		for (map<int, Module>::iterator it = modules.begin();
			it != modules.end();
			it++)
		{
			int i = it->first;
			if (!modules[i].issplitmod && modules[i].area > tmpoverallwastearea)   ////if has non-split module
			{
				largeareamodvect.push_back(i);
			}
		}

		map<int, int> largewidthmodidmapt; ///map<width, modid> 
		largewidthmodidmapt.clear();

		///compare the width  (we do vertical partition)
		int tmpusewidth = 0; //tmp use for get the module id with the largest width
		if (largeareamodvect.empty())
		{  ///all the modules' area less than the empty space, choose the module with the largest width for partition
			for (map<int, Module>::iterator it = modules.begin();
				it != modules.end();
				it++)
			{
				int i = it->first;
				if (!modules[i].issplitmod)   ///pick up the unsplit modules
				{
					largewidthmodidmapt.insert(make_pair(modules[i].width, i));
				}
			}
		}
		else
		{
			int largeareamodnum = (int)largeareamodvect.size();
			for (int tmpi = 0; tmpi < largeareamodnum; tmpi++)
			{
				int tmpmodid = largeareamodvect[tmpi];
				largewidthmodidmapt.insert(make_pair(modules[tmpmodid].width, tmpmodid));
			}
		}

		int forsplitmodid = -1; ////at least >= 0.   initial value.
		assert(largewidthmodidmapt.size() > 0);
		map<int, int>::reverse_iterator tmpwdithmodidit = largewidthmodidmapt.rbegin();
		assert(tmpwdithmodidit != largewidthmodidmapt.rend());

		////choose the module with the largest width for splitting.
		forsplitmodid = tmpwdithmodidit->second;  

		/////   change n to forsplitmodid     forsplitmodid must be a non-split module
		/////n = forsplitmodid;  //do this .  just do not need to convert the modid to n.  (use the same structure)
		map<int, int>::iterator tmpmodidindexidit = tmpmodidindexmap.find(forsplitmodid);
		assert(tmpmodidindexidit != tmpmodidindexmap.end());
		n = tmpmodidindexidit->second;


		////Flow 2: In splitperturb function.    <2>  split modules into two or three parts or four parts (choose which one is best)
		/////////////////map<string, map<float, map<int, map<int, splitmodule > > > > fordynamictopmodnamesubmodset;

		map<string, map<float, map<int, map<int, splitmodule > > > >::iterator tmpmodnameratiotypepartnumsplitmodit; //used these iterator 
		map<float, map<int, map<int, splitmodule> > >::iterator tmpratiotypepartnumsplitmodit; 
		map<int, map<int, splitmodule> >::iterator tmptypepartnumsplitmodit;
		map<int, splitmodule>::iterator tmppartnumsplitmodit;

		//Currenlty, the module has been split, cannot return back
		string tmpmodname;    //current module's parent name (the most original name by running the VPR)
		string modnamewithpostfix; //current modules' full name
		//split module idnodeidmap[n]

		///A module is split for different segments, then it will marked as issplit. 
		////Notice if bad result. recover
		////////(module) idnodeidmap[n] will be split .     

		//////pick up a ratio for split   (Each module has several ratios)
		float tmpratio = modules[idnodeidmap[n]].ratio;  //Each time need to record the startx, endx. etc??.
		modnamewithpostfix = modules[idnodeidmap[n]].name;  //eg.  A#11#11   A#12#11  (partition 2 levels)

		//'#' is the symbol for get the top module name 
		size_t firstpos = modnamewithpostfix.find("#"); //get the first position of '#'
		if (firstpos != string::npos)
		{
			tmpmodname = modnamewithpostfix.substr(0, firstpos);
		}
		else
		{
			tmpmodname = modules[idnodeidmap[n]].name;
		}

		///get the number of "#"
		int symbolcount = 0;
		string tmpmodnameSymbolnum = modnamewithpostfix;
		while (1)
		{
			size_t tmppos = tmpmodnameSymbolnum.find("#");
			if (tmppos != string::npos)
			{
				tmpmodnameSymbolnum = tmpmodnameSymbolnum.substr(tmppos + 1);
				symbolcount++;
			}
			else
				break;
		}

		//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & its submodule set & partitioncost   //first level (the topest level)
		////////map<string, map<float, map<int, splitmodule > > > fitfordiffarchtopmodnamesubmodset;
		float tmppartratio = -1;  ///partition based on the RAM, DSP columns. do not need the partition ratio. give a default value. -1   
		////record the partition ratio
		modules[idnodeidmap[n]].curvertipartiRatio = tmppartratio; ///set the parition ratio !!!!  (Do not use these two sentences.)

		bool tmpisvertpartition = 1; ///////????current use vertical partition (Try) rand_bool();  //judge whether it is vertical partition
		tmpmodnameratiotypepartnumsplitmodit = fordynamictopmodnamesubmodset.find(tmpmodname);
		if (tmpmodnameratiotypepartnumsplitmodit == fordynamictopmodnamesubmodset.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
		{
			//////set a mark note??  if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!
			//// Save time. Need to specify a variable   (Later change time)
			return;    ///this module's width is 1. cannot be split. Do nothing
		}

		assert(tmpmodnameratiotypepartnumsplitmodit != fordynamictopmodnamesubmodset.end());  //When enter split flow, should have splitted submodules  
		map<float, map<int, map<int, splitmodule> > > &tmpratiotypepartnumsplitmod = tmpmodnameratiotypepartnumsplitmodit->second;
		tmpratiotypepartnumsplitmodit = tmpratiotypepartnumsplitmod.find(tmpratio);

		if (tmpratiotypepartnumsplitmodit == tmpratiotypepartnumsplitmod.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
		{
			/////////set a mark note ? ? if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!
			//// Save time. Need to specify a variable   (Later change time)
			return;    //do nothing and do not do packing
		}

		assert(tmpratiotypepartnumsplitmodit != tmpratiotypepartnumsplitmod.end());   ////?????  if multi-level. should 
		map<int, map<int, splitmodule> > &tmptypepartnumsplitmod = tmpratiotypepartnumsplitmodit->second;
		tmptypepartnumsplitmodit = tmptypepartnumsplitmod.find(tmpisvertpartition);
		assert(tmptypepartnumsplitmodit != tmptypepartnumsplitmod.end());
		map<int, splitmodule> &tmppartnumsplitmod = tmptypepartnumsplitmodit->second;
		if (tmppartnumsplitmod.size() == 0)
		{
			return;  ///cannot partition. only has one part (original module)
		}



	    ////compare 2 parts and 3 parts more precisely.   
		////1.2 Partition a module
		///How many parts we need when we partition a module?  Partition into n parts (n is 2 or 3 or 4 or 5, which one is the best)
		///currently only consider 2 or 3 parts.
		////Judement criterion: utilize the empty space

		//////1.2.1    partition a module into 2 parts.
		////int twopartnum = 2;
		////vector<Module> twopartsubmods;
		////twopartsubmods.clear();
		////tmppartnumsplitmodit = tmppartnumsplitmod.find(twopartnum);
		////if (tmppartnumsplitmodit != tmppartnumsplitmod.end())
		////{
		////	//// when divide into n parts. it is empty (In other words, cannot split module into n parts)
		////	splitmodule tmpsplitmod = tmppartnumsplitmodit->second;

		////	///fbmao add: if a module is split into several modules in the first level, when do recursively partition
		////	///all the submodules need to be partitioned??  
		////	///Rule. 1.All the two modules need to do parition. or 2. one of the submodule needs to be paritioned. All are ok. 

		////	//		check which level it is   the number of #   ??????????????
		////	//	    use the name before the first #

		////	//////Sometimes only has one submodule  ( when the other submodules is empty)
		////	int tmpsubmodnum = tmpsplitmod.submodset.size();  ////get the number of submodules for this top module
		////	///// map<int, vector<int> > diffarchparentsubmoduleidset; 
		////	assert(tmpsubmodnum >= 1);
		////	vector<int> tmpsubmodidset;
		////	tmpsubmodidset.clear();
		////	for (int tmpsubmodid = 0; tmpsubmodid < tmpsubmodnum; tmpsubmodid++)
		////	{
		////		Module tmpmoda = tmpsplitmod.submodset[tmpsubmodid];
		////		twopartsubmods.push_back(tmpmoda);
		////	}
		////}


		///////1.2.2 Partiion into 3 parts
		////int threepartnum = 3;
		////vector<Module> threepartsubmods;
		////threepartsubmods.clear();
		////tmppartnumsplitmodit = tmppartnumsplitmod.find(threepartnum);
		////if (tmppartnumsplitmodit != tmppartnumsplitmod.end())
		////{
		////	//// when divide into n parts. it is empty (In other words, cannot split module into n parts)
		////	splitmodule tmpsplitmod = tmppartnumsplitmodit->second;

		////	///fbmao add: if a module is split into several modules in the first level, when do recursively partition
		////	///all the submodules need to be partitioned??  
		////	///Rule. 1.All the two modules need to do parition. or 2. one of the submodule needs to be paritioned. All are ok. 

		////	//		check which level it is   the number of #   ??????????????
		////	//	    use the name before the first #

		////	//////Sometimes only has one submodule  ( when the other submodules is empty)
		////	int tmpsubmodnum = tmpsplitmod.submodset.size();  ////get the number of submodules for this top module
		////	///// map<int, vector<int> > diffarchparentsubmoduleidset; 
		////	assert(tmpsubmodnum >= 1);
		////	vector<int> tmpsubmodidset;
		////	tmpsubmodidset.clear();
		////	for (int tmpsubmodid = 0; tmpsubmodid < tmpsubmodnum; tmpsubmodid++)
		////	{
		////		Module tmpmoda = tmpsplitmod.submodset[tmpsubmodid];
		////		threepartsubmods.push_back(tmpmoda);
		////	}
		////}


        ////2. Partition Rule
		////1.2.3  compare 2 part with 3 part, which one is better
		int tmppartn = 1; ////default. do not partition.  will record which partition way is better (E.g. 2 parts or 3 parts)

		//Each subpart for two partition
		int twopartwidth = modules[forsplitmodid].width / 2;   //if width is 3
		int twopartheight = modules[forsplitmodid].height;
		double twopartarea = twopartwidth * twopartheight;

		//Each subpart for three partition
		int threepartwidth = modules[forsplitmodid].width / 3;
		int threepartheight = modules[forsplitmodid].height;
		double threepartarea = threepartwidth * threepartheight;

		///1.2.3.1  
		if (modules[forsplitmodid].area <= tmpoverallwastearea)
		{  ////waste area is enough to accomodate the module (modules[forsplitmodid])
			tmppartn = 2;  //choose 2 partition. since it is enough for placing empty space. Not need to partition into 3 parts. due to complexity and runntime
		}
		else ////// (modules[forsplitmodid].area > tmpoverallwastearea)
		{   //we judge which partition way can use more empty space.
			if (twopartarea > tmpoverallwastearea)
			{
				tmppartn = 3; /////more easy for 3 parts to use the empty space
			}
			else if (twopartarea == tmpoverallwastearea)
			{
				tmppartn = 2;
			}
			else
			{  //twopartarea < tmpoverallwastearea
				if (threepartwidth * 2 > tmpoverallwastearea)
				{
					tmppartn = 2; ///can use larger empty space
				}
				else //////// (threepartwidth * 2 <= tmpoverallwastearea)
				{
					tmppartn = 3; 
				}
			}		
		}


        ///we partition the module into N parts  (N equals to tmppartn)
		tmppartnumsplitmodit = tmppartnumsplitmod.find(tmppartn);
		if (tmppartnumsplitmodit == tmppartnumsplitmod.end())
		{
			return; //// when divide into n parts. it is empty (In other words, cannot split module into n parts)
		}

		splitmodule tmpsplitmod = tmppartnumsplitmodit->second;

		///fbmao add: if a module is split into several modules in the first level, when do recursively partition
		///all the submodules need to be partitioned??  
		///Rule. 1.All the two modules need to do parition. or 2. one of the submodule needs to be paritioned. All are ok. 

		//		check which level it is   the number of #   ??????????????
		//	    use the name before the first #

		//////Sometimes only has one submodule  ( when the other submodules is empty)
		int tmpsubmodnum = tmpsplitmod.submodset.size();  ////get the number of submodules for this top module
		///// map<int, vector<int> > diffarchparentsubmoduleidset; 
		assert(tmpsubmodnum >= 1);
		vector<int> tmpsubmodidset;
		tmpsubmodidset.clear();
		for (int tmpsubmodid = 0; tmpsubmodid < tmpsubmodnum; tmpsubmodid++)
		{
			Module tmpmoda = tmpsplitmod.submodset[tmpsubmodid];
			tmpmoda.issplitmod = 1;
			tmpsubmodidset.push_back(tmpmoda.id);

			/////nodes.
			Node tmpnodea;
			tmpnodea.id = tmpmoda.id;   //module id is from construction or 
			assert(tmpnodea.id >= 0);

			//add the modules
			modules.insert(make_pair(tmpmoda.id, tmpmoda));

			////add modules_info
			Module_Info tmpmodinfoa;
			modules_info.insert(make_pair(tmpmoda.id, tmpmodinfoa));

			//add nodes
			nodes.insert(make_pair(tmpmoda.id, tmpnodea));

			//add contour
			Contour tmpcontoura;
			contour.insert(make_pair(tmpmoda.id, tmpcontoura));
		}

		////record the connected submodules (these submodules derived by splitting a top module)
		for (int i = 0; i < tmpsubmodnum; i++)
		{
			for (int j = 0; i != j && j < tmpsubmodnum; j++)
			{
				////if do not use the strong constraint:  Only need to put them adjacent together
				modules[tmpsubmodidset[i]].adjacentmodset.push_back(tmpsubmodidset[j]);
			}
		}

		////1.2.3how to place the submodules for a module
		////In B*-tree. change the nodes information
		bool preisplaceleft;
		bool tmpisplaceleft;         ////Notice: currently only consider the one time partition (do not have recursively partition.  complex, constraint set)
		if (tmpisvertpartition)  //1: vertical partition.   0: horizontal partition
		{
			int tmporinodeid = idnodeidmap[n];
			int tmpanothermodida = -1;
			for (int tmpid = 0; tmpid < (int)tmpsubmodidset.size(); tmpid++)
			{
				tmpanothermodida = tmpsubmodidset[tmpid];
				if (2 == tmppartn)      //how to place the submodules
				{
					tmpisplaceleft = rand_bool();
				}
				else
				{
					int tmpcombinaheight = 3 * threepartheight;
					if (tmpcombinaheight <= tmpemptyheight)     ////can use a parameter to control it ???? since the emptyheigt is a estimatation value.
					{
						tmpisplaceleft = 0; //place as a right child.
					}
					else
					{
						if (tmpid == 1)
						{
							tmpisplaceleft = rand_bool();;
							preisplaceleft = tmpisplaceleft;
						}
						else if (tmpid == 2)  //current at most has three submodules. the tmpid equals to 2
						{
							tmpisplaceleft = !preisplaceleft;
						}
						else
						{
							tmpisplaceleft = rand_bool(); ///the first submodule is put as a right child.
							assert(tmpid <= 3); //or else it has bug  (currently. dynamic split only consider 2 or 3 parts.)
						}
					}
				}
					
				insert_node(nodes[tmporinodeid], nodes[tmpanothermodida], tmpisplaceleft);

				///re-update the node id.  for submodules: submod1, submod2, submod3, submod4, submod5, submod6 ...
				tmporinodeid = tmpanothermodida;
			}

			delete_node(nodes[idnodeidmap[n]]);
		}    ///////else  //horizontal partition  do nothingxxxxx   

		///The same as that in the flow (split for different architectures). since only partition once and do not return back.
		diffarchparentsubmoduleidset.insert(make_pair(idnodeidmap[n], tmpsubmodidset)); 

		//delete the parent module, node etc. (the id in the module, module_info, node, contour)
		modules.erase(idnodeidmap[n]);
		modules_info.erase(idnodeidmap[n]);
		nodes.erase(idnodeidmap[n]);
		contour.erase(idnodeidmap[n]);

		////////after split, update module_N
		modules_N = modules_N + tmpsubmodnum - 1;  /////split a module into several modules
	}

	return;
}




void B_Tree::splimodperturbwithoutstrongconstraint()
{
	////node constraint. 
	////	1. splitted  submodule cannot be splitted again
	////	2. nodes have position constraint: split module cannot be directly perturbed
	//////     (add constraint on it. splitted modules from the same parent module must be considered together)
	////	3. recove etc. record some information. E.g.  module is not splitted .update ma<> parentxxxxxx

	//fbmao add for supporting split module (idnodeidmap is used to map index);
	///////create idnodeidmap
	vector<int>idnodeidmap;  //id-> nodeid may not continuous. eg. 1, 5, 7,  ..  vector index is 0,1,2,3
	for (map<int, Node>::iterator it = nodes.begin();
		it != nodes.end();
		it++)
	{
		idnodeidmap.push_back(it->first); //it->first== modid == nodeid
	}

	int p, n;
	n = rand() % modules_N;
	double r = rand_01();

	if (r < move_rate)   ////1. move
	{
		// delete & insert
		////tmpcountnum = 0;
		// choose different module
		do
		{
			p = rand() % modules_N;
		} while (n == p);

		//////1.1
		if (!modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)
		{
			delete_node(nodes[idnodeidmap[n]]);
			insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
		}
		else if (modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod) ///1.2
		{
			////first get another module that related to modules[idnodeidmap[n]
			if (modules[idnodeidmap[n]].constraintmodset.empty())  ///after split, only has one module
			{  ///after split,  only has one module
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else   ////non-empty
			{
				int tmpanothermodid = -1;
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module
				tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[0];

				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				delete_node(nodes[tmpanothermodid]);
				bool tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
				insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodid], tmpisplaceleft);
			}
		}
		else if (!modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod)   /////1.3
		{
			//////cannot impact the splitted modules relationship after moving new module to connect to splitted module
			/////First check whether another module (adjacent to the splitted module) 
			if (modules[idnodeidmap[p]].constraintmodset.empty())
			{
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else   ////non-empty
			{
				int tmpanothermodid = -1;
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				tmpanothermodid = modules[idnodeidmap[p]].adjacentmodset[0];

				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				delete_node(nodes[tmpanothermodid]);
				bool tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
				insert_node(nodes[idnodeidmap[p]], nodes[tmpanothermodid], tmpisplaceleft);
			}
		}
		else   /////1.4  all are splitted module. modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod
		{
			if (modules[idnodeidmap[n]].constraintmodset.empty() && modules[idnodeidmap[p]].constraintmodset.empty())
			{
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else if (!modules[idnodeidmap[n]].constraintmodset.empty() && modules[idnodeidmap[p]].constraintmodset.empty())
			{
				int tmpanothermodid = -1;
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[0];

				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				delete_node(nodes[tmpanothermodid]);
				bool tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
				insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodid], tmpisplaceleft);
			}
			else if (modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				//////cannot impact the splitted modules relationship after moving new module to connect to splitted module
				/////First check whether another module (adjacent to the splitted module) 
				////non-empty
				int tmpanothermodid = -1;
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				tmpanothermodid = modules[idnodeidmap[p]].adjacentmodset[0];

				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				delete_node(nodes[tmpanothermodid]);
				bool tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
				insert_node(nodes[idnodeidmap[p]], nodes[tmpanothermodid], tmpisplaceleft);
			}
			else ///////////(!modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				int tmpanothermodida = -1;
				int tmpanothermodidc = -1;

				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				tmpanothermodida = modules[idnodeidmap[n]].adjacentmodset[0];

				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				tmpanothermodidc = modules[idnodeidmap[p]].adjacentmodset[0];

				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				delete_node(nodes[tmpanothermodida]);
				bool tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
				insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodida], tmpisplaceleft);

				delete_node(nodes[tmpanothermodidc]);
				tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
				insert_node(nodes[idnodeidmap[p]], nodes[tmpanothermodidc], tmpisplaceleft);
			}
		}
	}
	else if (r < swap_rate)   //2. swap   
	{														// swap node
		// choose different module
		do
		{
			p = rand() % modules_N;                 /////later pick up from non-splitmodule list
		} while (n == p);

		if (!modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)  ///2.1
		{
			swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
		}
		else if (modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)  ////2.2
		{
			if (modules[idnodeidmap[n]].constraintmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else
			{
				int tmpanothermodid = -1;
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[0];

				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				delete_node(nodes[tmpanothermodid]);
				bool tmpisplaceleft = rand_bool();
				insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodid], tmpisplaceleft);
			}
		}
		else if (!modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod)  /////2.3
		{
			if (modules[idnodeidmap[p]].constraintmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else
			{
				int tmpanothermodida = -1;
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				tmpanothermodida = modules[idnodeidmap[p]].adjacentmodset[0];

				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				delete_node(nodes[tmpanothermodida]);
				bool tmpisplaceleft = rand_bool();
				insert_node(nodes[idnodeidmap[p]], nodes[tmpanothermodida], tmpisplaceleft);
			}
		}
		else   /////modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod ///2.4      
		{
			if (modules[idnodeidmap[n]].constraintmodset.empty() && modules[idnodeidmap[p]].constraintmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else if (!modules[idnodeidmap[n]].constraintmodset.empty() && modules[idnodeidmap[p]].constraintmodset.empty())
			{
				int tmpanothermodida = -1;
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				tmpanothermodida = modules[idnodeidmap[n]].adjacentmodset[0];

				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				delete_node(nodes[tmpanothermodida]);
				bool tmpisplaceleft = rand_bool();
				insert_node(nodes[idnodeidmap[n]], nodes[tmpanothermodida], tmpisplaceleft);
			}
			else if (modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				int tmpanothermodida = -1;
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				tmpanothermodida = modules[idnodeidmap[p]].adjacentmodset[0];

				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				delete_node(nodes[tmpanothermodida]);
				bool tmpisplaceleft = rand_bool();
				insert_node(nodes[idnodeidmap[p]], nodes[tmpanothermodida], tmpisplaceleft);
			}
			else ///////////(!modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				int tmpanothermodida = -1;
				int tmpanothermodidc = -1;

				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				tmpanothermodida = modules[idnodeidmap[n]].adjacentmodset[0];

				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				tmpanothermodidc = modules[idnodeidmap[p]].adjacentmodset[0];

				///swap operation
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
				swap_node2(nodes[tmpanothermodidc], nodes[tmpanothermodida]);
			}
		}
	}
	else ///3. split operation (fbmao add).  (Only when the module is non-splitted module, we can split the module)
	{	//a module is split into two submodules  <--> a node is split into two nodes and has position constraint
		//change structure.   map<int, node>  keep original module id. node id (in order to recover to original state). removed modules (original) also are needed to be recorded.
		//split module n (n is the node id equal to the module id, contour id)

		//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & partitionRatio & its submodule set & partitioncost   //first level (the topest level)
		////////////map<string, map<float, map<int, map<float, splitmodule> > > > topmodnamesubmodset;	

		////if split module number === original number of modules   means all the original modules are be splitted.  In this case, 
		///// change the move_rate and swap rate  (since do not have split operation).  

		bool hasnonsplitmod = false;
		for (map<int, Module>::iterator it = modules.begin();
			it != modules.end();
			it++)
		{
			int i = it->first;
			if (!modules[i].issplitmod)   ////if has non-split module
			{
				hasnonsplitmod = true;
				break;
			}
		}

		///if all the splitted module are splitted modules.  remove the split operation  (only has move and swap)
		if (false == hasnonsplitmod) ///non-splitmodule does not exist
		{
			move_rate = 0.5;  ///change the probability for swap operations
			swap_rate = 1.0;

			perturb();  //split do nothing, so do perturb() again
			return;    /////do nothing
		}

		///Notice. Currrently module only can be split once.
		///pick up non-split module from the unsplit modules.
		////choose a non-split module.    ////???  if all are splitted modules, keep nodes' constraint (find a way to perturb solution)
		int n1 = -1; //temporary initial value
		int tmpcountnum = 0;
		///find the non-split module
		while (modules[idnodeidmap[n]].issplitmod)    ////use a global count to compare with the original number of top modules.
		{
			n1 = rand() % modules_N;
			if (n != n1)
			{
				tmpcountnum++;
				n = n1;
			}
		}

		assert(!modules[idnodeidmap[n]].issplitmod);

		///fbmao revised
		//////if (tmpcountnum >= modules_N)
		//////{
		//////	return;   //currently, all the modules are splitted module. do nothing [Cannot use the split operation].
		//////}

		map<string, map<float, map<int, map<float, splitmodule> > > >::iterator tmpmodnameratiotypepartratiosplitmodit; //used these iterator below
		map<float, map<int, map<float, splitmodule> > >::iterator tmpratiotypepartratiosplitmodit;
		map<int, map<float, splitmodule> >::iterator tmptypepartratiosplitmodit;
		map<float, splitmodule>::iterator tmppartratiosplitmodit;

		//Currenlty, the module has been split, cannot return back
		string tmpmodname;    //current module's parent name (the most original name by running the VPR)
		string modnamewithpostfix; //current modules' full name
		//split module idnodeidmap[n]

		///A module is split for different ratios, then it will marked as issplit. Also need to add another parameter to check whether all the ratios encountered already (Notice if bad result. recover)
		////////(module) idnodeidmap[n] will be split .     

		//////???? Need to revise, pick up a ratio for split   (Each module has several ratios)

		float tmpratio = modules[idnodeidmap[n]].ratio;  //Each time need to record the startx, endx. etc??.
		modnamewithpostfix = modules[idnodeidmap[n]].name;  //eg.  A#11#11   A#12#11  (partition 2 levels)

		//'#' is the symbol for get the top module name 
		size_t firstpos = modnamewithpostfix.find("#"); //get the first position of '#'
		if (firstpos != string::npos)
		{
			tmpmodname = modnamewithpostfix.substr(0, firstpos);
		}
		else
		{
			tmpmodname = modules[idnodeidmap[n]].name;
		}

		///get the number of "#"
		int symbolcount = 0;
		string tmpmodnameSymbolnum = modnamewithpostfix;
		while (1)
		{
			size_t tmppos = tmpmodnameSymbolnum.find("#");
			if (tmppos != string::npos)
			{
				tmpmodnameSymbolnum = tmpmodnameSymbolnum.substr(tmppos + 1);
				symbolcount++;
			}
			else
				break;
		}

		//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & partitionRatio & its submodule set & partitioncost   //first level (the topest level)
		////////////map<string, map<float, map<int, map<float, splitmodule> > > > topmodnamesubmodset;
		float tmppartratio = 0.5; ////partition ratio on the module     ///choose a ratio
		////record the partition ratio
		modules[idnodeidmap[n]].curvertipartiRatio = tmppartratio; ///set the parition ratio !!!!

		bool tmpisvertpartition = 1; ///////????current use vertical partition (Try) rand_bool();  //judge whether it is vertical partition
		tmpmodnameratiotypepartratiosplitmodit = topmodnamesubmodset.find(tmpmodname);

		if (tmpmodnameratiotypepartratiosplitmodit == topmodnamesubmodset.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
		{
			//////set a mark note??  if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!
			//////	Need to update the code in allow perturbsplitmod

			//// Save time. Need to specify a variable   (Later change time)
			return;    ///this module's width is 1. cannot be split. Do nothing
		}

		assert(tmpmodnameratiotypepartratiosplitmodit != topmodnamesubmodset.end());  //When enter split flow, should have splitted submodules  
		map<float, map<int, map<float, splitmodule> > > &tmpratiotypepartratiosplitmod = tmpmodnameratiotypepartratiosplitmodit->second;
		tmpratiotypepartratiosplitmodit = tmpratiotypepartratiosplitmod.find(tmpratio);

		if (tmpratiotypepartratiosplitmodit == tmpratiotypepartratiosplitmod.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
		{
			/////////set a mark note ? ? if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!

			//// Save time. Need to specify a variable   (Later change time)
			return;    //do nothing and do not do packing
		}

		assert(tmpratiotypepartratiosplitmodit != tmpratiotypepartratiosplitmod.end());   ////?????  if multi-level. should 
		map<int, map<float, splitmodule> > &tmptypepartratiosplitmod = tmpratiotypepartratiosplitmodit->second;
		tmptypepartratiosplitmodit = tmptypepartratiosplitmod.find(tmpisvertpartition);
		assert(tmptypepartratiosplitmodit != tmptypepartratiosplitmod.end());   //????has this item? if do not split. may be empty . check
		map<float, splitmodule> & tmppartratiosplitmod = tmptypepartratiosplitmodit->second;
		tmppartratiosplitmodit = tmppartratiosplitmod.find(tmppartratio);
		assert(tmppartratiosplitmodit != tmppartratiosplitmod.end());
		splitmodule tmpsplitmod = tmppartratiosplitmodit->second;

		///fbmao add: if a module is split into two modules in the first level, when do recursively partition
		///all the two modules need to be partitioned??  
		///Rule. 1.All the two modules need to do parition. or 2. one of the submodule needs to be paritioned. All are ok. 

		//		check which level it is   the number of #   ??????????????
		//	    use the name before the first #

		map<string, map<int, map<float, splitmodule> > >::iterator ita;
		map <int, map<float, splitmodule> >::iterator itb;
		map <float, splitmodule>::iterator itc;

		//first find the splitmodule specified by the partition level
		while (tmpsplitmod.partitionLevel <= symbolcount)     //if it has several levels (larger than 1)
		{
			////map<parentModulename, partitionType, splitPositionRatio, splitmodule, partitionCost(default 0.0)>
			/////map<string, map<int, map<float, splitmodule> > > parentmodnamePartypePartratioSplitmod;
			ita = tmpsplitmod.parentmodnamePartypePartratioSplitmod.begin();
			assert(ita != tmpsplitmod.parentmodnamePartypePartratioSplitmod.end());
			itb = ita->second.begin();
			assert(itb != ita->second.end());
			itc = itb->second.begin();
			assert(itc != itb->second.end());
			tmpsplitmod = itc->second;
		}

		//////Sometimes only has one submodule  ( when the another submodule is empty)
		int tmpsubmodnum = tmpsplitmod.submodset.size();
		assert(tmpsubmodnum >= 1);
		Module tmpmoda, tmpmodb;

		if (tmpsubmodnum == 2)
		{
			tmpmoda = tmpsplitmod.submodset[0]; //sub modules
			tmpmodb = tmpsplitmod.submodset[1]; //sub modules       //mod id, the number of all modules+1   
		}
		else
		{
			tmpmoda = tmpsplitmod.submodset[0]; //sub modules
		}

		//store id...    ///<parentNode, submodNodea, submodNodeb>
		map<int, pair<int, int> >::iterator parentsubmodidit = parentsubmoduleid.find(idnodeidmap[n]);

		//////////debug  test
		//////for (map<int, pair<int, int> >::iterator kit = parentsubmoduleid.begin();
		//////	kit != parentsubmoduleid.end();
		//////	kit++)
		//////{
		//////	cout << "topid " << kit->first << " subid1 " << kit->second.first << " subid2 " << kit->second.second << endl;
		//////}

		//////for (vector<int>::iterator kbb = idnodeidmap.begin();
		//////	kbb != idnodeidmap.end();
		//////	kbb++)
		//////{
		//////	cout << " modid " << *kbb << " issplit " << modules[*kbb].issplitmod << endl;
		//////}


		//Reason for comment this sentence is that:  e.g., if module 0 is splitted in the sbmod 01 02.  
		////but it not take this operation. it will be splitted later. parenetsubmoduleid is not clear. so it can be !=. 
		///////assert(parentsubmodidit == parentsubmoduleid.end());   

		if (tmpsubmodnum == 2)
		{
			parentsubmoduleid.insert(make_pair(idnodeidmap[n], make_pair(tmpmoda.id, tmpmodb.id)));
			tmpmoda.issplitmod = 1;
			tmpmodb.issplitmod = 1;
		}
		else
		{
			parentsubmoduleid.insert(make_pair(idnodeidmap[n], make_pair(tmpmoda.id, -1)));
			tmpmoda.issplitmod = 1;
		}

		Node tmpnodea;
		tmpnodea.id = tmpmoda.id;   //module id is from construction or 
		assert(tmpnodea.id >= 0);

		Node tmpnodeb;
		if (tmpsubmodnum == 2)
		{
			tmpnodeb.id = tmpmodb.id;
			assert(tmpnodeb.id >= 0);
		}

		//add the new nodes
		modules.insert(make_pair(tmpmoda.id, tmpmoda));
		if (tmpsubmodnum == 2)
		{
			modules.insert(make_pair(tmpmodb.id, tmpmodb));
		}

		Module_Info tmpmodinfoa, tmpmodinfob;
		modules_info.insert(make_pair(tmpmoda.id, tmpmodinfoa));
		if (tmpsubmodnum == 2)
		{
			modules_info.insert(make_pair(tmpmodb.id, tmpmodinfob));
		}

		nodes.insert(make_pair(tmpmoda.id, tmpnodea));

		if (tmpsubmodnum == 2)
		{
			nodes.insert(make_pair(tmpmodb.id, tmpnodeb));
		}

		Contour tmpcontoura, tmpcontourb;
		contour.insert(make_pair(tmpmoda.id, tmpcontoura));

		if (tmpsubmodnum == 2)
		{
			contour.insert(make_pair(tmpmodb.id, tmpcontourb));
		}

		bool isplaceleft;         ////Notice: currently only consider the one time partition (do not have recursively partition.  complex, constraint set)
		if (tmpisvertpartition)  //1: vertical partition.   0: horizontal partition
		{
			insert_node(nodes[idnodeidmap[n]], nodes[tmpnodea.id]);

			if (tmpsubmodnum == 2)
			{
				isplaceleft = false;
				insert_node(nodes[tmpnodea.id], nodes[tmpnodeb.id], isplaceleft);
			}

			delete_node(nodes[idnodeidmap[n]]);

			//for (map<string, int>::iterator tmpconstTypeIdit = modules[idnodeidmap[n]].constraintmodset.begin();
			//	tmpconstTypeIdit != modules[idnodeidmap[n]].constraintmodset.end();
			//	tmpconstTypeIdit++)
			//{

			//}

			//keep constraint relation
			if (tmpsubmodnum == 2)  ////only when it has two submodules, they have constraint relationship 
			{    ////Notice:    if splitted submodules can be move or swap or re-split. constraintmodset need to be used
				modules[tmpmoda.id].constraintmodset.insert(make_pair("right", tmpmodb.id));
				modules[tmpmodb.id].constraintmodset.insert(make_pair("parent", tmpmoda.id));

				////if do not use the strong constraint:  Only need to put them adjacent together
				modules[tmpmoda.id].adjacentmodset.push_back(tmpmodb.id);
				modules[tmpmodb.id].adjacentmodset.push_back(tmpmoda.id);
			}
		}
		else  //horizontal partition  xxxxx                      
		{
			//how to record relation among modules and nodes;
			////insert_node(nodes[idnodeidmap[n]], submodule1, isplaceleft); ///// ?? Need to judge pareent node and parentparentnode relation to determine
			////	  it is left or right child   ////	nodes[parent.right].parent = node.id;
			////	                                //determine whether it is left child or right child
			insert_node(nodes[idnodeidmap[n]], nodes[tmpnodea.id]);

			if (tmpsubmodnum == 2)
			{
				isplaceleft = true;
				insert_node(nodes[tmpnodea.id], nodes[tmpnodeb.id], isplaceleft);
			}

			delete_node(nodes[idnodeidmap[n]]);

			if (tmpsubmodnum == 2)
			{
				/////Need to update constraint relations
				modules[tmpmoda.id].constraintmodset.insert(make_pair("left", tmpmodb.id));
				modules[tmpmodb.id].constraintmodset.insert(make_pair("parent", tmpmoda.id));

				////if do not use the strong constraint:  Only need to put them adjacent together
				modules[tmpmoda.id].adjacentmodset.push_back(tmpmodb.id);
				modules[tmpmodb.id].adjacentmodset.push_back(tmpmoda.id);
			}
		}

		//delete the parent module, node etc. (the id in the module, module_info, node, contour)
		modules.erase(idnodeidmap[n]);
		modules_info.erase(idnodeidmap[n]);
		nodes.erase(idnodeidmap[n]);
		contour.erase(idnodeidmap[n]);

		////////after split update module_N
		if (tmpsubmodnum == 2)
		{
			modules_N = modules_N + 1;  /////split a module into two modules
		}
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



void B_Tree::fordiffarchsplimodperturbwithoutstrongconstraint()
{    ///Function: fit for different architectures.
	////node constraint. 
	////	1. splitted  submodule cannot be splitted again
	////	2. nodes have position constraint: split module cannot be directly perturbed
	//////     (add constraint on it. splitted modules from the same parent module must be considered together)
	////	3. recove etc. record some information. E.g.  module is not splitted .update ma<> parentxxxxxx

	//fbmao add for supporting split module (idnodeidmap is used to map index);
	///////create idnodeidmap
	vector<int>idnodeidmap;  //id-> nodeid may not continuous. eg. 1, 5, 7,  ..  vector index is 0,1,2,3
	for (map<int, Node>::iterator it = nodes.begin();
		it != nodes.end();
		it++)
	{
		idnodeidmap.push_back(it->first); //it->first== modid == nodeid
	}

	int p, n;
	n = rand() % modules_N;
	double r = rand_01();

	int tmpanothermodid = -1; ///global variable in this function
	int tmporinode = -1;
	bool tmpisplaceleft, tmpisplacelefta, tmpisplaceleftb;

	int tmpanothermodida = -1;
	int tmporinodea = -1;

	int tmpanothermodidb = -1;
	int tmporinodeb = -1;

	if (r < move_rate)   ////1. move
	{
		// delete & insert
		////tmpcountnum = 0;
		// choose different module
		do
		{
			p = rand() % modules_N;
		} while (n == p);

		//////1.1
		if (!modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)
		{
			delete_node(nodes[idnodeidmap[n]]);
			insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
		}
		else if (modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod) ///1.2
		{
			////first get another module that related to modules[idnodeidmap[n]
			if (modules[idnodeidmap[n]].adjacentmodset.empty())  ///after split, only has one module
			{  ///after split,  only has one module
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else   ////non-empty
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module

				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[n];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;  //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}
			}
		}
		else if (!modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod)   /////1.3
		{
			//////cannot impact the splitted modules relationship after moving new module to connect to splitted module
			/////First check whether another module (adjacent to the splitted module) 
			if (modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else   ////non-empty
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);


				///deal with other submodules of nodes[idnodeidmap[p]]
				tmpanothermodid = -1;
				tmporinode = idnodeidmap[p];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[p]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;     //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}
			}
		}
		else   /////1.4  all are splitted module. modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod
		{
			if (modules[idnodeidmap[n]].adjacentmodset.empty() && modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else if (!modules[idnodeidmap[n]].adjacentmodset.empty() && modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module

				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[n];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;  //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}

			}
			else if (modules[idnodeidmap[n]].adjacentmodset.empty() && !modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				//////cannot impact the splitted modules relationship (after moving new module to connect to splitted module)
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				///deal with other submodules of nodes[idnodeidmap[p]]
				tmpanothermodid = -1;
				tmporinode = idnodeidmap[p];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[p]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;     //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}
			}
			else ///////////(!modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module
				delete_node(nodes[idnodeidmap[n]]);
				insert_node(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodida = -1;
				tmporinodea = idnodeidmap[n];
				for (int tmpida = 0; tmpida < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpida++)
				{
					tmpanothermodida = modules[idnodeidmap[n]].adjacentmodset[tmpida];
					delete_node(nodes[tmpanothermodida]);
					tmpisplacelefta = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinodea], nodes[tmpanothermodida], tmpisplacelefta);

					tmporinodea = tmpanothermodida;  //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}

				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module
				///deal with other submodules of nodes[idnodeidmap[p]]
				tmpanothermodidb = -1;  /////////////?????????????? tmpanothermodid tmporinode can use a variable.
				tmporinodeb = idnodeidmap[p];
				for (int tmpidb = 0; tmpidb < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpidb++)
				{
					tmpanothermodidb = modules[idnodeidmap[p]].adjacentmodset[tmpidb];
					delete_node(nodes[tmpanothermodidb]);
					tmpisplaceleftb = rand_bool();  ///use rand_bool. they have some options (only need to meet they put adjacent together)
					insert_node(nodes[tmporinodeb], nodes[tmpanothermodidb], tmpisplaceleftb);

					tmporinodeb = tmpanothermodidb;     //update submodules.  for inserting.  submodule1, submodule2, submodule 3 ...
				}
			}
		}
	}
	else if (r < swap_rate)   //2. swap   
	{														// swap node
		// choose different module
		do
		{
			p = rand() % modules_N;                 /////later pick up from non-splitmodule list
		} while (n == p);

		if (!modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)  ///2.1
		{
			swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
		}
		else if (modules[idnodeidmap[n]].issplitmod && !modules[idnodeidmap[p]].issplitmod)  ////2.2
		{
			if (modules[idnodeidmap[n]].constraintmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[n];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;
				}
			}
		}
		else if (!modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod)  /////2.3
		{
			if (modules[idnodeidmap[p]].constraintmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module	
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[p];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[p]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;
				}
			}
		}
		else   /////modules[idnodeidmap[n]].issplitmod && modules[idnodeidmap[p]].issplitmod ///2.4      
		{
			if (modules[idnodeidmap[n]].adjacentmodset.empty() && modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);
			}
			else if (!modules[idnodeidmap[n]].adjacentmodset.empty() && modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[n];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[n]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;
				}
			}
			else if (modules[idnodeidmap[n]].adjacentmodset.empty() && !modules[idnodeidmap[p]].adjacentmodset.empty())
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module	
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodid = -1;
				tmporinode = idnodeidmap[p];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodid = modules[idnodeidmap[p]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodid]);
					tmpisplaceleft = rand_bool();
					insert_node(nodes[tmporinode], nodes[tmpanothermodid], tmpisplaceleft);

					tmporinode = tmpanothermodid;
				}
			}
			else ///////////(!modules[idnodeidmap[n]].constraintmodset.empty() && !modules[idnodeidmap[p]].constraintmodset.empty())
			{
				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[n]].adjacentmodset.size() > 0); ////has sibling module	
				swap_node2(nodes[idnodeidmap[p]], nodes[idnodeidmap[n]]);

				tmpanothermodida = -1;
				tmporinodea = idnodeidmap[n];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[n]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodida = modules[idnodeidmap[n]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodida]);
					tmpisplacelefta = rand_bool();
					insert_node(nodes[tmporinodea], nodes[tmpanothermodida], tmpisplacelefta);

					tmporinodea = tmpanothermodida;
				}

				//////Only need to record the binding module's id
				assert(modules[idnodeidmap[p]].adjacentmodset.size() > 0); ////has sibling module	
				int tmpanothermodidb = -1;
				int tmporinodeb = idnodeidmap[p];
				for (int tmpid = 0; tmpid < (int)modules[idnodeidmap[p]].adjacentmodset.size(); tmpid++)
				{
					tmpanothermodidb = modules[idnodeidmap[p]].adjacentmodset[tmpid];
					delete_node(nodes[tmpanothermodidb]);
					tmpisplaceleftb = rand_bool();
					insert_node(nodes[tmporinodeb], nodes[tmpanothermodidb], tmpisplaceleftb);

					tmporinodeb = tmpanothermodidb;
				}
			}
		}
	}
	else ///3. split operation (fbmao add).  (Only when the module is non-splitted module, we can split the module)
	{	//a module is split into two submodules  <--> a node is split into two nodes and has position constraint
		//change structure.   map<int, node>  keep original module id. node id (in order to recover to original state). removed modules (original) also are needed to be recorded.
		//split module n (n is the node id equal to the module id, contour id)

		//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & partitionRatio & its submodule set & partitioncost   //first level (the topest level)
		////////////map<string, map<float, map<int, map<float, splitmodule> > > > topmodnamesubmodset;	

		////if split module number === original number of modules   means all the original modules are be splitted.  In this case, 
		///// change the move_rate and swap rate  (since do not have split operation).  

		bool hasnonsplitmod = false;
		for (map<int, Module>::iterator it = modules.begin();
			it != modules.end();
			it++)
		{
			int i = it->first;
			if (!modules[i].issplitmod)   ////if has non-split module
			{
				hasnonsplitmod = true;
				break;
			}
		}

		///if all the splitted module are splitted modules.  remove the split operation  (only has move and swap)
		if (false == hasnonsplitmod) ///non-splitmodule does not exist
		{
			move_rate = 0.5;  ///change the probability for swap operations
			swap_rate = 1.0;   ///when all the modules are splitted module, will do not have split. 

			perturb();  //split do nothing, so do perturb() again
			return;    /////do nothing
		}

		///Notice. Currrently module only can be split once.
		///pick up non-split module from the unsplit modules.
		////choose a non-split module.    ////???  if all are splitted modules, keep nodes' constraint (find a way to perturb solution)
		int n1 = -1; //temporary initial value
		int tmpcountnum = 0;
		///find the non-split module
		while (modules[idnodeidmap[n]].issplitmod)    ////use a global count to compare with the original number of top modules.
		{
			n1 = rand() % modules_N;
			if (n != n1)
			{
				tmpcountnum++;
				n = n1;
			}
		}

		assert(!modules[idnodeidmap[n]].issplitmod);
		if (!issplitfitfordifferentarch)
		{  ////Flow <1>  split module based on ratio.    At most has two submodules.
			/////and split module for the same architecture that the libraray uses. 

			map<string, map<float, map<int, map<float, splitmodule> > > >::iterator tmpmodnameratiotypepartratiosplitmodit; //used these iterator below
			map<float, map<int, map<float, splitmodule> > >::iterator tmpratiotypepartratiosplitmodit;
			map<int, map<float, splitmodule> >::iterator tmptypepartratiosplitmodit;
			map<float, splitmodule>::iterator tmppartratiosplitmodit;

			//Currenlty, the module has been split, cannot return back
			string tmpmodname;    //current module's parent name (the most original name by running the VPR)
			string modnamewithpostfix; //current modules' full name
			//split module idnodeidmap[n]

			///A module is split for different ratios, then it will marked as issplit. Also need to add another parameter to check whether all the ratios encountered already (Notice if bad result. recover)
			////////(module) idnodeidmap[n] will be split .     

			//////???? Need to revise, pick up a ratio for split   (Each module has several ratios)

			float tmpratio = modules[idnodeidmap[n]].ratio;  //Each time need to record the startx, endx. etc??.
			modnamewithpostfix = modules[idnodeidmap[n]].name;  //eg.  A#11#11   A#12#11  (partition 2 levels)

			//'#' is the symbol for get the top module name 
			size_t firstpos = modnamewithpostfix.find("#"); //get the first position of '#'
			if (firstpos != string::npos)
			{
				tmpmodname = modnamewithpostfix.substr(0, firstpos);
			}
			else
			{
				tmpmodname = modules[idnodeidmap[n]].name;
			}

			///get the number of "#"
			int symbolcount = 0;
			string tmpmodnameSymbolnum = modnamewithpostfix;
			while (1)
			{
				size_t tmppos = tmpmodnameSymbolnum.find("#");
				if (tmppos != string::npos)
				{
					tmpmodnameSymbolnum = tmpmodnameSymbolnum.substr(tmppos + 1);
					symbolcount++;
				}
				else
					break;
			}

			//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & partitionRatio & its submodule set & partitioncost   //first level (the topest level)
			////////////map<string, map<float, map<int, map<float, splitmodule> > > > topmodnamesubmodset;
			float tmppartratio = 0.5; ////partition ratio on the module     ///choose a ratio
			////record the partition ratio
			modules[idnodeidmap[n]].curvertipartiRatio = tmppartratio; ///set the parition ratio !!!!

			bool tmpisvertpartition = 1; ///////????current use vertical partition (Try) rand_bool();  //judge whether it is vertical partition
			tmpmodnameratiotypepartratiosplitmodit = topmodnamesubmodset.find(tmpmodname);

			if (tmpmodnameratiotypepartratiosplitmodit == topmodnamesubmodset.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
			{
				//////set a mark note??  if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!
				//////	Need to update the code in allow perturbsplitmod

				//// Save time. Need to specify a variable   (Later change time)
				return;    ///this module's width is 1. cannot be split. Do nothing
			}

			assert(tmpmodnameratiotypepartratiosplitmodit != topmodnamesubmodset.end());  //When enter split flow, should have splitted submodules  
			map<float, map<int, map<float, splitmodule> > > &tmpratiotypepartratiosplitmod = tmpmodnameratiotypepartratiosplitmodit->second;
			tmpratiotypepartratiosplitmodit = tmpratiotypepartratiosplitmod.find(tmpratio);

			if (tmpratiotypepartratiosplitmodit == tmpratiotypepartratiosplitmod.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
			{
				/////////set a mark note ? ? if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!

				//// Save time. Need to specify a variable   (Later change time)
				return;    //do nothing and do not do packing
			}

			assert(tmpratiotypepartratiosplitmodit != tmpratiotypepartratiosplitmod.end());   ////?????  if multi-level. should 
			map<int, map<float, splitmodule> > &tmptypepartratiosplitmod = tmpratiotypepartratiosplitmodit->second;
			tmptypepartratiosplitmodit = tmptypepartratiosplitmod.find(tmpisvertpartition);
			assert(tmptypepartratiosplitmodit != tmptypepartratiosplitmod.end());   //????has this item? if do not split. may be empty . check
			map<float, splitmodule> & tmppartratiosplitmod = tmptypepartratiosplitmodit->second;
			tmppartratiosplitmodit = tmppartratiosplitmod.find(tmppartratio);
			assert(tmppartratiosplitmodit != tmppartratiosplitmod.end());
			splitmodule tmpsplitmod = tmppartratiosplitmodit->second;

			///fbmao add: if a module is split into two modules in the first level, when do recursively partition
			///all the two modules need to be partitioned??  
			///Rule. 1.All the two modules need to do parition. or 2. one of the submodule needs to be paritioned. All are ok. 

			//		check which level it is   the number of #   ??????????????
			//	    use the name before the first #

			map<string, map<int, map<float, splitmodule> > >::iterator ita;
			map <int, map<float, splitmodule> >::iterator itb;
			map <float, splitmodule>::iterator itc;

			//first find the splitmodule specified by the partition level
			while (tmpsplitmod.partitionLevel <= symbolcount)     //if it has several levels (larger than 1)
			{
				////map<parentModulename, partitionType, splitPositionRatio, splitmodule, partitionCost(default 0.0)>
				/////map<string, map<int, map<float, splitmodule> > > parentmodnamePartypePartratioSplitmod;
				ita = tmpsplitmod.parentmodnamePartypePartratioSplitmod.begin();
				assert(ita != tmpsplitmod.parentmodnamePartypePartratioSplitmod.end());
				itb = ita->second.begin();
				assert(itb != ita->second.end());
				itc = itb->second.begin();
				assert(itc != itb->second.end());
				tmpsplitmod = itc->second;
			}


			//////Sometimes only has one submodule  ( when the another submodule is empty)
			int tmpsubmodnum = tmpsplitmod.submodset.size();
			assert(tmpsubmodnum >= 1);
			Module tmpmoda, tmpmodb;

			if (tmpsubmodnum == 2)
			{
				tmpmoda = tmpsplitmod.submodset[0]; //sub modules
				tmpmodb = tmpsplitmod.submodset[1]; //sub modules       //mod id, the number of all modules+1   
			}
			else
			{
				tmpmoda = tmpsplitmod.submodset[0]; //sub modules
			}

			//store id...    ///<parentNode, submodNodea, submodNodeb>
			map<int, pair<int, int> >::iterator parentsubmodidit = parentsubmoduleid.find(idnodeidmap[n]);


			//////////debug  test
			//////for (map<int, pair<int, int> >::iterator kit = parentsubmoduleid.begin();
			//////	kit != parentsubmoduleid.end();
			//////	kit++)
			//////{
			//////	cout << "topid " << kit->first << " subid1 " << kit->second.first << " subid2 " << kit->second.second << endl;
			//////}

			//////for (vector<int>::iterator kbb = idnodeidmap.begin();
			//////	kbb != idnodeidmap.end();
			//////	kbb++)
			//////{
			//////	cout << " modid " << *kbb << " issplit " << modules[*kbb].issplitmod << endl;
			//////}


			//Reason for comment this sentence is that:  e.g., if module 0 is splitted in the sbmod 01 02.  
			////but it not take this operation. it will be splitted later. parenetsubmoduleid is not clear. so it can be !=. 
			///////assert(parentsubmodidit == parentsubmoduleid.end());   

			if (tmpsubmodnum == 2)
			{
				parentsubmoduleid.insert(make_pair(idnodeidmap[n], make_pair(tmpmoda.id, tmpmodb.id)));
				tmpmoda.issplitmod = 1;
				tmpmodb.issplitmod = 1;
			}
			else
			{
				parentsubmoduleid.insert(make_pair(idnodeidmap[n], make_pair(tmpmoda.id, -1)));
				tmpmoda.issplitmod = 1;
			}

			Node tmpnodea;
			tmpnodea.id = tmpmoda.id;   //module id is from construction or 
			assert(tmpnodea.id >= 0);

			Node tmpnodeb;
			if (tmpsubmodnum == 2)
			{
				tmpnodeb.id = tmpmodb.id;
				assert(tmpnodeb.id >= 0);
			}

			//add the new nodes
			modules.insert(make_pair(tmpmoda.id, tmpmoda));
			if (tmpsubmodnum == 2)
			{
				modules.insert(make_pair(tmpmodb.id, tmpmodb));
			}

			Module_Info tmpmodinfoa, tmpmodinfob;
			modules_info.insert(make_pair(tmpmoda.id, tmpmodinfoa));
			if (tmpsubmodnum == 2)
			{
				modules_info.insert(make_pair(tmpmodb.id, tmpmodinfob));
			}

			nodes.insert(make_pair(tmpmoda.id, tmpnodea));

			if (tmpsubmodnum == 2)
			{
				nodes.insert(make_pair(tmpmodb.id, tmpnodeb));
			}

			Contour tmpcontoura, tmpcontourb;
			contour.insert(make_pair(tmpmoda.id, tmpcontoura));

			if (tmpsubmodnum == 2)
			{
				contour.insert(make_pair(tmpmodb.id, tmpcontourb));
			}

			bool isplaceleft;         ////Notice: currently only consider the one time partition (do not have recursively partition.  complex, constraint set)
			if (tmpisvertpartition)  //1: vertical partition.   0: horizontal partition
			{
				insert_node(nodes[idnodeidmap[n]], nodes[tmpnodea.id]);

				if (tmpsubmodnum == 2)
				{
					isplaceleft = false;
					insert_node(nodes[tmpnodea.id], nodes[tmpnodeb.id], isplaceleft);
				}

				delete_node(nodes[idnodeidmap[n]]);

				//for (map<string, int>::iterator tmpconstTypeIdit = modules[idnodeidmap[n]].constraintmodset.begin();
				//	tmpconstTypeIdit != modules[idnodeidmap[n]].constraintmodset.end();
				//	tmpconstTypeIdit++)
				//{

				//}

				//keep constraint relation
				if (tmpsubmodnum == 2)  ////only when it has two submodules, they have constraint relationship 
				{    ////Notice:    if splitted submodules can be move or swap or re-split. constraintmodset need to be used
					modules[tmpmoda.id].constraintmodset.insert(make_pair("right", tmpmodb.id));
					modules[tmpmodb.id].constraintmodset.insert(make_pair("parent", tmpmoda.id));

					////if do not use the strong constraint:  Only need to put them adjacent together
					modules[tmpmoda.id].adjacentmodset.push_back(tmpmodb.id);
					modules[tmpmodb.id].adjacentmodset.push_back(tmpmoda.id);
				}
			}
			else  //horizontal partition  xxxxx                      
			{
				//how to record relation among modules and nodes;
				////insert_node(nodes[idnodeidmap[n]], submodule1, isplaceleft); ///// ?? Need to judge pareent node and parentparentnode relation to determine
				////	  it is left or right child   ////	nodes[parent.right].parent = node.id;
				////	                                //determine whether it is left child or right child
				insert_node(nodes[idnodeidmap[n]], nodes[tmpnodea.id]);

				if (tmpsubmodnum == 2)
				{
					isplaceleft = true;
					insert_node(nodes[tmpnodea.id], nodes[tmpnodeb.id], isplaceleft);
				}

				delete_node(nodes[idnodeidmap[n]]);

				if (tmpsubmodnum == 2)
				{
					/////Need to update constraint relations
					modules[tmpmoda.id].constraintmodset.insert(make_pair("left", tmpmodb.id));
					modules[tmpmodb.id].constraintmodset.insert(make_pair("parent", tmpmoda.id));

					////if do not use the strong constraint:  Only need to put them adjacent together
					modules[tmpmoda.id].adjacentmodset.push_back(tmpmodb.id);
					modules[tmpmodb.id].adjacentmodset.push_back(tmpmoda.id);
				}
			}

			//delete the parent module, node etc. (the id in the module, module_info, node, contour)
			modules.erase(idnodeidmap[n]);
			modules_info.erase(idnodeidmap[n]);
			nodes.erase(idnodeidmap[n]);
			contour.erase(idnodeidmap[n]);

			////////after split update module_N
			if (tmpsubmodnum == 2)
			{
				modules_N = modules_N + 1;  /////split a module into two modules
			}
		}
		else
		{   ////Flow 2: In splitperturb function.    <2>  split modules for fitting for different architectures

			map<string, map<float, map<int, splitmodule> > >::iterator tmpmodnameratiotypesplitmodit; //used these iterator below
			map<float, map<int, splitmodule> >::iterator tmpratiotypesplitmodit;
			map<int, splitmodule>::iterator tmptypesplitmodit;

			//Currenlty, the module has been split, cannot return back
			string tmpmodname;    //current module's parent name (the most original name by running the VPR)
			string modnamewithpostfix; //current modules' full name
			//split module idnodeidmap[n]

			///A module is split for different segments, then it will marked as issplit. 
			////Notice if bad result. recover
			////////(module) idnodeidmap[n] will be split .     

			//////pick up a ratio for split   (Each module has several ratios)
			float tmpratio = modules[idnodeidmap[n]].ratio;  //Each time need to record the startx, endx. etc??.
			modnamewithpostfix = modules[idnodeidmap[n]].name;  //eg.  A#11#11   A#12#11  (partition 2 levels)

			//'#' is the symbol for get the top module name 
			size_t firstpos = modnamewithpostfix.find("#"); //get the first position of '#'
			if (firstpos != string::npos)
			{
				tmpmodname = modnamewithpostfix.substr(0, firstpos);
			}
			else
			{
				tmpmodname = modules[idnodeidmap[n]].name;
			}

			///get the number of "#"
			int symbolcount = 0;
			string tmpmodnameSymbolnum = modnamewithpostfix;
			while (1)
			{
				size_t tmppos = tmpmodnameSymbolnum.find("#");
				if (tmppos != string::npos)
				{
					tmpmodnameSymbolnum = tmpmodnameSymbolnum.substr(tmppos + 1);
					symbolcount++;
				}
				else
					break;
			}

			//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & its submodule set & partitioncost   //first level (the topest level)
			////////map<string, map<float, map<int, splitmodule > > > fitfordiffarchtopmodnamesubmodset;
			float tmppartratio = -1;  ///partition based on the RAM, DSP columns. do not need the partition ratio. give a default value. -1   
			////record the partition ratio
			modules[idnodeidmap[n]].curvertipartiRatio = tmppartratio; ///set the parition ratio !!!!  (Do not use these two sentences.)

			bool tmpisvertpartition = 1; ///////????current use vertical partition (Try) rand_bool();  //judge whether it is vertical partition
			tmpmodnameratiotypesplitmodit = fitfordiffarchtopmodnamesubmodset.find(tmpmodname);
			if (tmpmodnameratiotypesplitmodit == fitfordiffarchtopmodnamesubmodset.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
			{
				//////set a mark note??  if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!
				//////	Need to update the code in allow perturbsplitmod

				//// Save time. Need to specify a variable   (Later change time)
				return;    ///this module's width is 1. cannot be split. Do nothing
			}

			assert(tmpmodnameratiotypesplitmodit != fitfordiffarchtopmodnamesubmodset.end());  //When enter split flow, should have splitted submodules  
			map<float, map<int, splitmodule> > &tmpratiotypesplitmod = tmpmodnameratiotypesplitmodit->second;
			tmpratiotypesplitmodit = tmpratiotypesplitmod.find(tmpratio);

			if (tmpratiotypesplitmodit == tmpratiotypesplitmod.end())  ////if the modules with different ratios, widths all are equal to 1. Cannot be split
			{
				/////////set a mark note ? ? if do not implement perturb operation, then do not do packing to save time !!!!!!!!!!!!!!!!

				//// Save time. Need to specify a variable   (Later change time)
				return;    //do nothing and do not do packing
			}

			assert(tmpratiotypesplitmodit != tmpratiotypesplitmod.end());   ////?????  if multi-level. should 
			map<int, splitmodule> &tmptypepartratiosplitmod = tmpratiotypesplitmodit->second;
			tmptypesplitmodit = tmptypepartratiosplitmod.find(tmpisvertpartition);
			assert(tmptypesplitmodit != tmptypepartratiosplitmod.end());   //????has this item? if do not split. may be empty . check
			splitmodule tmpsplitmod = tmptypesplitmodit->second;

			///fbmao add: if a module is split into several modules in the first level, when do recursively partition
			///all the submodules need to be partitioned??  
			///Rule. 1.All the two modules need to do parition. or 2. one of the submodule needs to be paritioned. All are ok. 

			//		check which level it is   the number of #   ??????????????
			//	    use the name before the first #

			//////Sometimes only has one submodule  ( when the other submodules is empty)
			int tmpsubmodnum = tmpsplitmod.submodset.size();  ////get the number of submodules for this top module
			///// map<int, vector<int> > diffarchparentsubmoduleidset; 
			assert(tmpsubmodnum >= 1);
			vector<int> tmpsubmodidset;
			tmpsubmodidset.clear();
			for (int tmpsubmodid = 0; tmpsubmodid < tmpsubmodnum; tmpsubmodid++)
			{
				Module tmpmoda = tmpsplitmod.submodset[tmpsubmodid];
				tmpmoda.issplitmod = 1;
				tmpsubmodidset.push_back(tmpmoda.id);

				/////nodes.
				Node tmpnodea;
				tmpnodea.id = tmpmoda.id;   //module id is from construction or 
				assert(tmpnodea.id >= 0);

				//add the modules
				modules.insert(make_pair(tmpmoda.id, tmpmoda));

				////add modules_info
				Module_Info tmpmodinfoa;
				modules_info.insert(make_pair(tmpmoda.id, tmpmodinfoa));

				//add nodes
				nodes.insert(make_pair(tmpmoda.id, tmpnodea));

				//add contour
				Contour tmpcontoura;
				contour.insert(make_pair(tmpmoda.id, tmpcontoura));
			}

			////record the connected submodules (these submodules derived by splitting a top module)
			for (int i = 0; i < tmpsubmodnum; i++)
			{
				for (int j = 0; i != j && j < tmpsubmodnum; j++)
				{
					////if do not use the strong constraint:  Only need to put them adjacent together
					modules[tmpsubmodidset[i]].adjacentmodset.push_back(tmpsubmodidset[j]);
				}
			}

			////In B*-tree. change the nodes information
			bool tmpisplaceleft;         ////Notice: currently only consider the one time partition (do not have recursively partition.  complex, constraint set)
			if (tmpisvertpartition)  //1: vertical partition.   0: horizontal partition
			{
				int tmporinodeid = idnodeidmap[n];
				int tmpanothermodida = -1;
				for (int tmpid = 0; tmpid < (int)tmpsubmodidset.size(); tmpid++)
				{
					tmpanothermodida = tmpsubmodidset[tmpid];
					tmpisplaceleft = rand_bool();
					insert_node(nodes[tmporinodeid], nodes[tmpanothermodida], tmpisplaceleft);

					///re-update the node id.  for submodules: submod1, submod2, submod3, submod4, submod5, submod6 ...
					tmporinodeid = tmpanothermodida;
				}

				delete_node(nodes[idnodeidmap[n]]);
			}    ///////else  //horizontal partition  do nothingxxxxx   

			diffarchparentsubmoduleidset.insert(make_pair(idnodeidmap[n], tmpsubmodidset));

			//delete the parent module, node etc. (the id in the module, module_info, node, contour)
			modules.erase(idnodeidmap[n]);
			modules_info.erase(idnodeidmap[n]);
			nodes.erase(idnodeidmap[n]);
			contour.erase(idnodeidmap[n]);

			////////after split, update module_N
			modules_N = modules_N + tmpsubmodnum - 1;  /////split a module into several modules
		}
	}

	return;
}



void B_Tree::swap_node(Node &n1, Node &n2)
{

	if (n1.left != NIL)
	{
		nodes[n1.left].parent = n2.id;
	}
	if (n1.right != NIL)
	{
		nodes[n1.right].parent = n2.id;
	}
	if (n2.left != NIL)
	{
		nodes[n2.left].parent = n1.id;
	}
	if (n2.right != NIL)
	{
		nodes[n2.right].parent = n1.id;
	}

	if (n1.parent != n1.id)
	{
		if (n1.parent != n1.id && n1.parent != NIL)
		{
			//add_changed_nodes(n1.parent);
			if (nodes[n1.parent].left == n1.id)
				nodes[n1.parent].left = n2.id;
			else
				nodes[n1.parent].right = n2.id;
		}
		else
		{
			changed_root = n1.id;
			nodes_root = n2.id;
		}
	}

	if (n2.parent != n2.id)
	{
		if (n2.parent != NIL)
		{
			if (nodes[n2.parent].left == n2.id)
				nodes[n2.parent].left = n1.id;
			else
				nodes[n2.parent].right = n1.id;
		}
		else
		{
			nodes_root = n1.id;
		}
	}

	swap(n1.left, n2.left);
	swap(n1.right, n2.right);
	swap(n1.parent, n2.parent);
}

void B_Tree::swap_node2(Node &n1, Node &n2)
{
	if (n1.parent != n2.id && n2.parent != n1.id)
		swap_node(n1, n2);
	else
	{
		bool leftChild;
		bool parentN1 = (n1.id == n2.parent);
		if (parentN1)
		{
			if (n1.left == n2.id)
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
			if (n2.left == n1.id)
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

		swap_node(n1, n2);
		if (parentN1)
		{
			n1.parent = n2.id;
			if (leftChild)
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
			if (leftChild)
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

	if (edge)
	{
		// place left

		//add_changed_nodes(parent.left);
		node.left = parent.left;
		node.right = NIL;
		if (parent.left != NIL)
			nodes[parent.left].parent = node.id;

		parent.left = node.id;
	}
	else
	{
		// place right

		//add_changed_nodes(parent.right);
		node.left = NIL;
		node.right = parent.right;
		if (parent.right != NIL)
			nodes[parent.right].parent = node.id;

		parent.right = node.id;
	}
}



//fbmao add for support split module. nodes have position constraint
void B_Tree::insert_node(Node &parent, Node &node, bool isplaceleft)
{    //use isplaceleft to control the position constraint

	node.parent = parent.id;
	bool edge = isplaceleft;

	if (edge)
	{
		// place left

		//add_changed_nodes(parent.left);
		node.left = parent.left;
		node.right = NIL;
		if (parent.left != NIL)
			nodes[parent.left].parent = node.id;

		parent.left = node.id;
	}
	else
	{
		// place right

		//add_changed_nodes(parent.right);
		node.left = NIL;
		node.right = parent.right;
		if (parent.right != NIL)
			nodes[parent.right].parent = node.id;

		parent.right = node.id;
	}
}


void B_Tree::delete_node(Node &node)
{
	int child = NIL;		// pull which child
	int subchild = NIL;		// child's subtree
	int subparent = NIL;

	if (!node.isleaf())
	{
		bool left = rand_bool();			// choose a child to pull up
		if (node.left == NIL) left = false;
		if (node.right == NIL) left = true;

		//add_changed_nodes(node.left);
		//add_changed_nodes(node.right);

		if (left)
		{
			child = node.left;			// child will never be NIL
			if (node.right != NIL)
			{
				subchild = nodes[child].right;
				subparent = node.right;
				nodes[node.right].parent = child;
				nodes[child].right = node.right;	// abut with node's another child
			}
		}
		else
		{
			child = node.right;
			if (node.left != NIL)
			{
				subchild = nodes[child].left;
				subparent = node.left;
				nodes[node.left].parent = child;
				nodes[child].left = node.left;
			}
		}
		//add_changed_nodes(subchild);
		nodes[child].parent = node.parent;
	}

	if (node.parent == NIL)			// root
	{
		// changed_root = nodes_root;
		nodes_root = child;
	}
	else								// let parent connect to child
	{
		//add_changed_nodes(node.parent);
		if (node.id == nodes[node.parent].left)
			nodes[node.parent].left = child;
		else
			nodes[node.parent].right = child;
	}

	// place subtree
	if (subchild != NIL)
	{
		Node &sc = nodes[subchild];
		assert(subparent != NIL);

		while (1)
		{
			Node &p = nodes[subparent];

			// 2003/11/19
			// if both left and right child NIL, place with equal probability
			if (p.left == NIL && p.right == NIL)
			{
				sc.parent = p.id;
				if (rand_bool())
				{
					p.left = sc.id;
				}
				else
				{
					p.right = sc.id;
				}
				break;
			}
			else if (p.left == NIL)		// place left 
			{
				sc.parent = p.id;
				p.left = sc.id;
				break;
			}
			else if (p.right == NIL)		// place right
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


bool B_Tree::delete_node2(Node &node, DIR pull)
{
	DIR npull = !pull;

	int p = node.parent;
	int n = node.id;
	int c = child(n, pull);
	int cn = child(n, npull);

	assert(n != nodes_root); // not root;

	DIR p2c = (nodes[p].left == n ? LEFT : RIGHT);	// current DIR, LEFT or RIGHT

	if (c == NIL)
	{
		// Pull child, but NIL, so pull the other child
		wire_nodes(p, cn, p2c);
		return (cn != NIL);   // folding
	}
	else
	{
		wire_nodes(p, c, p2c);
	}

	while (c != NIL)
	{
		int k = child(c, npull);
		wire_nodes(c, cn, npull);
		cn = k;
		n = c;
		c = child(c, pull);
	}

	if (cn != NIL)
	{
		wire_nodes(n, cn, pull);
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

void B_Tree::insert_node2(Node &parent, Node &node,
	DIR edge = LEFT, DIR push = LEFT, bool fold/*=false*/){
	DIR npush = !push;
	int p = parent.id;
	int n = node.id;
	int c = child(p, edge);

	wire_nodes(p, n, edge);
	wire_nodes(n, c, push);

	while (c != NIL)
	{
		wire_nodes(n, child(c, npush), npush);
		n = c;
		c = child(c, push);
	}
	wire_nodes(n, NIL, npush);

	if (fold)
	{
		wire_nodes(nodes[n].parent, NIL, push);
		wire_nodes(nodes[n].parent, n, npush);
	}
}

void B_Tree::setOrientation()
{
	if (outline_ratio <= 0)
		return;


	////for( int i=0; i<modules_N; i++ )
	////{
	for (map<int, Module>::iterator it = modules.begin();
		it != modules.end();
		it++)
	{
		int i = it->first;
		if (modules[i].width > outline_width || modules[i].height > outline_height)
		{
			// rotate it
			nodes[i].rotate = true;
			modules[i].no_rotate = true;
			printf("fix module %d\n", i);

			if (modules[i].height > outline_width || modules[i].width > outline_height)
			{
				printf("impossible to legal module %d\n", i);
			}

		}
		if (modules[i].height > outline_width || modules[i].width > outline_height)
		{
			modules[i].no_rotate = true;
			printf("fix module %d\n", i);
		}



	}

}

