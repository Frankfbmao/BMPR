// Project: B*-tree based placement/floorplanning
// Advisor: Yao-Wen Chang  <ywchang@cis.nctu.edu.tw>
// Authors: Jer-Ming Hsu   <barz@cis.nctu.edu.tw>
// 	    Hsun-Cheng Lee <gis88526@cis.nctu.edu.tw>
// Sponsors: NSC, Taiwan; Arcadia, Inc.; UMC.
// Version 1.0
// Date:    7/19/2000

//---------------------------------------------------------------------------
#include <cstdlib>
#include <iostream>             
#include <cstring>
#include <ctime>
#include <cmath>
using namespace std;

#include "btree.h"
#include "sa.h"
//---------------------------------------------------------------------------
//for calculating congestion 
//global variable
Grids grids;
vector<vector<virtualPin> >netsets;
vector<int> numbersets;  //numberset contains the number of terminals of each nets

float cost_beta;  //global variable define in fplan.h
double move_rate; //global variable define in fplan.h
double swap_rate; //global variable define in fplan.h

string tmpoutfile;  //used for output data to file
string archname; // arch name
int rd; //parameter for module ratio driven flow 0: don't consider ratio driven, 1 (other value): consider ratio-driven
//means if the module size less than a predefined value, it will have a fixed ratio
int timesformsize;  //parameter for changing iteration times in each temperature accorinding to the (total module number * rationumber)
//0: sa para don't consider the modulesize   1: sa consider the module size sa
int sabigmodulesize, sasmallmodulesize; //parameter for design exploration
float sacoefficienta, sacoefficientb; //used to change the iteration time in each temperature
float smallarea; //small area criterion,  for ratio driven flow (consider relation between the module size and the ratio)
int io3; // 0: don't consider the original module port's position (module's center-to-center wirelength estimation)
//1 : consider the orginal module port's position (get the coordinate from the place file)
int isloadable; //for loadable flow,  used to remove IO and replace the blk acoording to the module-base place coordinates
int isheteroarch; //0: general arch without memory, multiply etc.
//1: heterogenous arch (support memory, multiply etc.)
//default is 0 (general arch)

int isoneratioflow; //0: multiratioflwo,  1: one ratio flow.  Default: 0 multi-ratio flow
int isvprarch1; //1: vpr arch. k6_N10_memDepth16384_memData64_40nm_timing (in vpr arch dir).

int issplitaftersometime; //for split after some iteration

////isinternosplitoneratio  1: do not split    0: split for inter solution
////run for some time has a intersolution (best so far).  default :  0. 
int isinternosplitoneratio; 

int issplitInCurrentratio;   //control the place_module() function. only choose one ratio. default 0 (do not split)

////1: split module to fit for different architectures   
////0: (do not support the flow that spliting for fitting for different architecture)
int issplitfitfordifferentarch; 

//architecture type. 0, 1, 2 etc. (It can be set by user in advance)
int garchtype;   ///default 0

///global variable. remove the clb blocks
int grmclb;   //In split flow. 1: execute the flow. remove the empty CLB columns   0: Do not remove. 
//Default value is  0 

////1:  traditional flow to fit for different architectures
////0:  traditional flow does not execute to be fit for different architectures
int istradfitfordiffarch;  //default 0

///1: dymaic partition a module into submodules
///0: do not do dynamic partition during runtime
int isdynamicpartition; //default 0

string tmponeratiodir;   ///direcotry for storing one ratio placement result
string tmpmultiratiodir; ///directory for storing multi-ratio placement result

//for PR-aware route flow exploration
//percentage for the number of prregions (random  percentage*totalmodulenum (fp.size())
///Specify how many regions are PR regions. 
float percentprnum;    //default 1.0      range (0.1 - 1.0)

///0: use roughly netlist (verilog port to reflect the connectsion betwen them).
////1: use real netlist
bool isrealnetlist = 0;  


//-ownarch1 modified based on vprarch1 
//-vprarch1

int main(int argc, char **argv)
{
	//firstly, need to create two directory.  one ratio. multi-ratio directory
	//store results for one ratio flow and multi-ratio flow

	///The code does split default.  If do not do split. Must add a parameter (-nosplit)  

	char filename[80], outfile[80] = "";
	int times = 50; //// 50;  /// 50; /// 200; // 200; ///// 50; //// 50; // 100; // 400; /////100; //original 400, 
	//check split the cases have problem????
	int local = 7;

	double init_temp = 0.9, term_temp = 0.1;
	//  double alpha=1;  // 20130105
	double alpha = 0.55; // before 0.5;  Now change to 0.55 /// 1.0; /// 0.5; // 1.0; /// 0.5; /// 1; // 0.5;
	double k3 = 1.0;

	int rand_seed = -1;

	// fixed-outline parameters
	double deadSpace = 0.15;
	double ar = -1;
	int format = 0;

	//add by fbmao
	cost_beta = 0.5; //global variable default 0.5

////	move_rate = 0.5; //default 0.5 global varaible 

	move_rate = 0.33;  ///fbmao change latest

	swap_rate = 0.7; //for split. add another operation: split operation
	                 //default: has split operation.  so if do not do split, must change the swap_rate to 1 or larger

//// swap_rate = 1.0; //no split operation

	//add arch parameter
	archname = "6k6_n8_10.xml";  //default arch name
	rd = 0; //default 0 (don't consider the module size, module with small size has one ratio)
	timesformsize = 0; //(time for module size) Default 0    
	//change times of sa para accorinding to the (total module number * rationumber)
	//0: sa para don't consider the number of modules    1: sa consider the number of modules in sa

	//below give the default value
	///for design exploration,  used to control the iteration time of sa
	sabigmodulesize = 80;  //The number of modules. used to control iteration time 
	sasmallmodulesize = 50; //The number of modules. used to control iteration time
	sacoefficienta = 2; //coefficient used to calculate iteration time
	sacoefficientb = 0.5; //coefficient used to calculate iteration time

	//default, used in ratio driven flow, when rd == 1 
	///when area less than smallarea, do not consider different ratio since it is too small
	smallarea = 8; 

	io3 = 1;  //default 1, 0: means don't consider the original module port's position, use center-to-center wirelength estimation
	//1: port-to-port wirelenght estimation

	isloadable = 0; //for loadable flow    1: loadable flow 0: not loadable flow
	isheteroarch = 1; //0 :for support general arch or 1 : heterogenous arch 
	percentprnum = 1.0; // range (0.1-1.0) default 1.0, all the modules are PR regions

	isoneratioflow = 0; //0: multiratio flow. 1: one ratio flow. default. multi-ratio flow

///	isvprarch1 = 1; //Default 1:
	//1: arch k6_N10_memDepth16384_memData64_40nm_timing.xml from vpr arch dir.  0: own created arch based on the 
	// k6_N10_memDepth16384_memData64_40nm_timing.xml


	/////Arch2  
////	cout << "*****Notice  it may be generate bug, check the isvprarch1 parameter" << endl;
	isvprarch1 = 0;     /////1: vpr architectuture    0: architecture modified based on vpr architecture.

	isrealnetlist = 0; //default 0: use the rougly netlist.  1: use the real netlist 

	//has two directory: one for store result for one ratio
	tmponeratiodir = "oneratio";  //can use command line to change the directory
	//store result for multi-ratio
	tmpmultiratiodir = "multiratio";

	//for support split after running for some time. (module has multiratio.  choose one ratio,
	//////the solution best so far.) then split the module.  in a step
	issplitaftersometime = 0; ///

	////isinternosplitoneratio  1: do not split inter solution   0: split for inter solution
	isinternosplitoneratio = 0; ////default 0. split one ratio flow at some stage. if split in intermediate solution, default split at one ratio

	//when the ratio is chosen, then do not change ratio any more. 0: can choose multi-ratio. 1: only one ratio
	issplitInCurrentratio = 0;  ///Default 0: moule can have multiratio when find a ratio to split. it will be 1

	///0:  do not execute the flow that spliting module to fit for different architectures. 
	///partition based on (partition ratio)
	
	///1:  execute the flow that splitting module to fit for different architecture (partition based on RAM, DSP column. etc.)
    issplitfitfordifferentarch = 0; 

	///specially for setting parameters for different architecture.
	/// Under the flow: split module for different rachitectures or traditional BMP for different architectures
	garchtype = 0;   ///default 0

	grmclb = 0;   //In split flow. 1: execute the flow. remove the empty CLB columns   0: Do not remove. 

	////1:  traditional flow to fit for different architectures
	////0:  traditional flow does not execute to be fit for different architectures
	istradfitfordiffarch = 0;  //default 0

	///1: dymaic partition a module into submodules
	///0: do not do dynamic partition during runtime
	isdynamicpartition = 0; //default 0            will change the default value to 1. 

	if (argc <= 1)
	{
		printf("Usage: btree <filename> [option]\n");
		printf("Option: \n");
		printf("   -simple      use simple format instead of MCNC format\n");
		printf("   -seed d      rand seed \n");
		printf("   -ar f        fixed-outline aspect ratio (default= %f)\n", ar);
		printf("   -maxWS f     fixed-outline maximum white space (default= %f)\n", deadSpace);
		printf("   -alpha f     area weight (wire weight = 1 - alpha) (default= %f)\n", alpha);
		printf("   -k3 f        AR weight (default= %.2f)\n", k3);
		printf("   -P f         intial acceptance rate \n");
		//printf( "   -lamda f     simulated annealing lamda \n" );
		printf("   -t d         # pertubations for each temperature (default= %d)\n", times);
		printf("   -local d     # local searches (default= %d)\n", local);
		printf("   -beta     #  weight beta (default= %f)\n", cost_beta);
		printf("   -moverate     #  probability for operations(op1-op4) (default= %f)\n", move_rate);
		printf("   -arch     #  the arch name %s  (for routing)\n", archname.c_str());
		printf("   -rd     #  the ratiodriven  rd %d  \n", rd);
		printf("   -timesformsize     #  the timesformsizeYorN    %d  \n", timesformsize);   //time of para in sa for module size
		// printf("   -io3     #  1: use original module port's position when calculating wirelength 0: otherwise  io3 %d  \n", io3);

		return 0;
	}
	else
	{
		int argi = 1;
		if (argi < argc) strcpy(filename, argv[argi++]);

		while (argi < argc)
		{
			string s = argv[argi++];
			if (s == "-n")	// times
			{
				times = atoi(argv[argi++]);
			}
			else if (s == "-P")
			{
				P = atof(argv[argi++]);
			}
			//else if( s == "-lamda" ) 
			//{
			//	lamda = atof(argv[argi++]);
			//} 
			else if (s == "-seed")
			{
				rand_seed = atoi(argv[argi++]);
				srand(rand_seed);  //add by fbmao,  use the rand_seed
			}
			else if (s == "-ar")
			{
				ar = atof(argv[argi++]);
			}
			else if (s == "-maxWS")
			{
				deadSpace = atof(argv[argi++]);
			}
			else if (s == "-simple")
			{
				format = 1;
			}
			else if (s == "-local")
			{
				local = atoi(argv[argi++]);
			}
			else if (s == "-t")
			{
				times = atoi(argv[argi++]);
			}
			else if (s == "-alpha")
			{
				alpha = atof(argv[argi++]);
			}
			else if (s == "-k3")
			{
				k3 = atof(argv[argi++]);
			}
			else if (s == "-beta")
			{
				cost_beta = atof(argv[argi++]);
			}
			else if (s == "-moverate")  //for choose operation op1, op2, op3, op4 give different probability
			{
				move_rate = atof(argv[argi++]);
			}
			else if (s == "-arch")  //for using specific arch
			{
				archname = argv[argi++];
			}
			else if (s == "-rd")  //for ratio consider the module size, small area module will used only one ratio
			{
				rd = atoi(argv[argi++]);
			}
			else if (s == "-timesformsize")  //for adaptive, change the sa para (iteration times) for each temperature
			{
				timesformsize = atoi(argv[argi++]);
			}
			else if (s == "-sabigmodulesize")  //for exploration, give the criterion of big size module
			{
				sabigmodulesize = atoi(argv[argi++]);
			}
			else if (s == "-sasmallmodulesize")  //for exploration, give the criterion of small size module
			{
				sasmallmodulesize = atoi(argv[argi++]);
			}
			else if (s == "-sacoefficienta")  //for exploration, control the iterative times for each temperature
			{
				sacoefficienta = atof(argv[argi++]);
			}
			else if (s == "-sacoefficientb")  //for exploration, control the iterative times for each temperature
			{
				sacoefficientb = atof(argv[argi++]);
			}
			else if (s == "-smallarea")  //for exploration, control the iterative times for each temperature
			{
				smallarea = atof(argv[argi++]);
			}
			else if (s == "-io3")  //1: port-to-port bounding box wirelength estimation, 
			{                      //0 : center-to-center bounding box wirelength estimation
				io3 = atoi(argv[argi++]);
			}
			else if (s == "-isloadable")  //for loadable flow,  should replace the reconfig region(subfunction, module)
			{
				isloadable = atoi(argv[argi++]);
			}
			else if (s == "-isheteroarch")  //for support heterogenous arch
			{
				isheteroarch = atoi(argv[argi++]);
			}
			else if (s == "-prpercent")  //for exploration, control the iterative times for each temperature
			{
				percentprnum = atof(argv[argi++]);
			}
			else if (s == "-isoneratioflow")  //for support heterogenous arch
			{
				isoneratioflow = 1;  //1: one ratio flow.   0: multi-ratio flow
			}
			else if (s == "-isownarch")  //for support heterogenous arch
			{
				isvprarch1 = 0;  //1: vpr arch.   0: arch modified based on vpr arch
			}
			else if (s == "-isrealnet")  //for support heterogenous arch
			{
				isrealnetlist = 1;  //1: one ratio flow.   0: multi-ratio flow
			}
			else if (s == "-nosplit")   //original BMP flow. Do not split
			{
				swap_rate = 1.0;   ///do not run split flow. the split operation cannot be executed.
			}
			else if (s == "-issplitafter")  //for split after some time
			{
				issplitaftersometime = 1;  //1: one ratio flow.   0: multi-ratio flow
			}
			else if (s == "-isinternosplit")  ///inter solution do not split. but based on one ratio run BMP 
			{
				isinternosplitoneratio = 1; ///1: do not split
			}
			else if (s == "-issplitdiffarch")  ///  split modules for fitting for different architectures
			{
				issplitfitfordifferentarch = 1; ///1: Yes. do
			}
			else if (s == "-istradfordiffarch")  ///traditional flow for fitting for different architectures
			{
				istradfitfordiffarch = 1; ///Yes. traditional flow for different architectures
			}
			else if (s == "-archtype")  ///only for the flow: fit for different architectures. currenlty only has 0, 1, 2. 3
			{                           ////The default arch is not (0, 1, 2, 3)
				garchtype = atoi(argv[argi++]);
			}
			else if (s == "-rmclb")  //In split flow. 1: execute the flow. remove the empty CLB columns   0: Do not remove. 
			{                          
				grmclb = 1;
			}
			else if (s == "-dodynamic")  //In split flow. 1: execute the flow. remove the empty CLB columns   0: Do not remove. 
			{
				isdynamicpartition = 1;
			}
			else
			{
				cout << "Wrong argument: " << s << endl;
				return 0;
			}
		}

		//if(argi < argc) times=atoi(argv[argi++]);
		//if(argi < argc) local=atoi(argv[argi++]);
		//if(argi < argc) avg_ratio=atof(argv[argi++]);
		//if(argi < argc) alpha=atof(argv[argi++]);
		//if(argi < argc) lamda=atof(argv[argi++]);
		//if(argi < argc) term_temp=atof(argv[argi++]);
		//if(argi < argc) strcpy(outfile, argv[argi++]);
	}

	if (rand_seed == -1)
	{
		rand_seed = (unsigned)time(NULL);
		srand(rand_seed);
	}

	cout << "\n circuit = " << filename
		<< "\n format  = " << format
		<< "\n seed    = " << rand_seed
		<< "\n AR      = " << ar
		<< "\n maxWS   = " << deadSpace
		<< "\n alpha   = " << alpha
		<< "\n k3      = " << k3
		<< "\n P       = " << P
		<< "\n lamda   = " << lamda
		<< "\n local   = " << local
		<< "\n times   = " << times
		<< "\n archname = " << archname.c_str()
		<< "\n rd = " << rd
		<< "\n timesforsize(module) = " << timesformsize
		<< "\n percentprnum = " << percentprnum
		<< endl;



	//try
	//{
	{  //fbmao add
		string outfilename = filename;
		if (isoneratioflow)   //isoneratioflow default: 0
		{
			tmpoutfile = outfilename + "/" + tmponeratiodir + "/" + outfilename;
		}
		else
		{
			tmpoutfile = outfilename + "/" + tmpmultiratiodir + "/" + outfilename;
		}
	}

	double time = seconds();
	B_Tree fp(alpha);

	//set filename to fp class
	fp.setFilenametoFp(filename);

	//record architecture. eg. RAM start column,gap, DSP start column gap.
	fp.recordDataofArchitecture(archname);   //used for checking the solution is feasible or not
	fp.storeClbAndHeteroBlocksFromvprFiles(); //from vpr.out file
	fp.getfinalioblocks();
	//for change original blkname, when move blocks to the final placement file
	fp.getblksnamemaplist();

	switch (format)
	{
	case 0:
		fp.read(filename);    //default: use this function to read file 
		break;
	case 1:
		fp.read_simple(filename);
		break;
	}


	fp.k3 = k3;
	if (ar >= 0)
	{
		fp.outline_height = sqrt((1.0 + deadSpace) * ar * fp.getTotalArea());
		fp.outline_width = sqrt((1.0 + deadSpace) / ar * fp.getTotalArea());
		fp.outline_ratio = ar;
		printf("outline: H %.2f   W %.2f\n", fp.outline_height, fp.outline_width);
	}
	else
	{
		fp.outline_ratio = -1;
	}


	//fbmao add for loadable flow
	{   //this part only work for loadable flow
		if (isloadable)  //default: 0  //need to read the configregion file to check the 
		{                              //loadable region

			//int archwidth = 10;  //for case ch_intrinsics, the width and heigh can be read from place file
			//need to modify the interface later
			//int archheight = 10;
			int archwidth = 24;   //for case boundtop, 24 just a test value 
			int archheight = 24;
			fp.getloadableRegionAndReplace(archwidth, archheight);

			//for change original blkname, when move blocks to the final placement file
			fp.getblksnamemaplist();   //should change later

			//for loadable flow, getting place file after doing loadable work for one or more regions
			//read the place file already stored before and change the lodable region place result by using 
			//the replace file, it will output to a directory, name case.place  
			fp.combineplacefileAfterloadable();
			return 1;
		}
	}

	fp.setOrientation();
	//fp.show_modules();

	/////for new wirelength calculation (except IO block)
	fp.getmodulenameRatioBlknameCoordinateFromOrignalVPRfile();

	//create network for calculating wirelength  (connections based on the input/output name of the original modules' verilog file
	fp.traceSoftModuleConnections();
	fp.traceSoftModuleConnectionsKeepportname();

	//fbmao add for supporting spliting module, firstly preparing the splitting module for later use
	////partition based on certain percentage (only two parts) [eg. 0.4, 0.6 etc.]
	fp.splitmoduleAndRecordSubmodule();          

	///fbmao added
	///Partition for fitting for different architectures
	//[partition based on RAM and DSP column, may have several parts].  share the same structure : fordiffarchsplitfilenameRatioramdspcolnum
	if (issplitfitfordifferentarch)
	{
		fp.splitmodulefitfordifferentarch();
	}
	else if (isdynamicpartition)
	{
		///Partition for fitting for different architectures
		//[partition based on RAM and DSP column, may have several parts]
		fp.splitmodulefordynamicpartition();
	}


	///set a specific ratio for all modules. and based on it to run flow
	if (isoneratioflow) 
	{
		fp.specifyoneratioformodules();
	}

	//record the total number of DSP, RAM 
	fp.recordMemDspnumforEachModule();  
	
	////Function: Set different architecture file.
	/////fit for different architectures. 
	///traditional flow for different architectures.  Key point: how to get right rx, ry for a module
	if (istradfitfordiffarch || issplitfitfordifferentarch)
	{  
		garchtype = 1;   ///archtype is a global variable.  Now only has [three different types. 0, 1, 2, 3]
		////////change the paramter for fitting for different architectures
		fp.setParameterForDifferentArch(garchtype);
	}

	fp.init();

	//////fbmao debug
   //////cout << "In main.cpp comments the normalize_cost(1000)" << endl;
 ///   fp.normalize_cost(1000);

	fp.normalize_cost(700);

	if (1 == io3)  //port-to-port takes much time
	{
		///times = 50; //50;
		// fp.size();
		//	 fp.getmaxnumratio()
	//	cout << "test " << endl;

		/// times = pow(fp.size() , 1.3333);       ///split flow,  only perturb on one ratio. may do not consider the numratio

		//for support split flow. fbmao revised
	///	times = pow(fp.modNumBeforeSplit(), 1.3333);


		//int testnumb = pow((fp.getmaxnumratio() + 1) / 2, 1.3333);

		////times = pow(fp.size() * (fp.getmaxnumratio() + 1) / 2, 1.3333);

		//int testtimesddd = pow(90, 1.3333);

		//int testtimes = pow(fp.size() * (fp.getmaxnumratio() + 1) / 2, 1.3333);
		//int kkkkk = 2;
	}

	double last_time = SA_Floorplan(fp, times, local, term_temp);
	//Random_Floorplan(fp,times);

	//fbmao revised. Should have enough positions to place IO blocks (extend Width or height of whole chip)
	fp.checkIOPositionEnoughAndExpandwidthAndHeight();

	fp.list_information();
	if (true == fp.checkoverlap())
	{
		cout << "the modules are overlap, pls check";
		exit(1);
	}

	//before combining the place file together, need to 
	//place the blocks inside the module well (for each submodule)
	//creating a 1 <-> 1 mapping system.   that means a coordinate generated by B*-tree can directly find a position in the FPGA architecure

	////when final results be determined, then start to do local placement for each module. 
	//Here it is based on the original blk's position in the prepared module
	//map the blks' coordinate to board
	fp.doLocalplaceForEachmoduleMapToArch();
	//combine blks into one file
	fp.getFinalPlaceFiles(filename);   //bug  position not right

	//original combine file. do not support heterogeneous architecture  (Use BMP for homogeneous architecture and homogeneous module)
	////	 fp.combinePlaceFiles(filename);

	//for the congestion
	//fp.Final_tree();   //mao add 
	//fp.Output_positions(filename);  //mao add 

	{ // log performance and quality
		string tmpfilename = filename;

		if (strlen(outfile) == 0)
		{
			////tmpfilename = tmpfilename + "/" + tmpfilename + ".res";
			if (isoneratioflow)
			{
				tmpfilename = tmpfilename + "/" + tmponeratiodir + "/" + tmpfilename + ".res";
			}
			else
			{
				tmpfilename = tmpfilename + "/" + tmpmultiratiodir + "/" + tmpfilename + ".res";
			}
		}
		//         strcpy(outfile,strcat(filename,".res"));

		last_time = last_time - time;
		double total_cpu = seconds() - time;
		printf("start time = %.2f\n", time);
		printf("CPU time       = %.2f\n", total_cpu);
		printf("Last CPU time  = %.2f\n", last_time);


		//for congestion
		float MaxCongvalue = 0;
		double mfbaverage = 0;
		double mfbstd = 0;
		if (true == calcongest)
		{
			MaxCongvalue = fp.getcongvalue();
			mfbaverage = fp.mfbmean(grids);
			mfbstd = fp.mfbvar(grids);
		}

		grids.clear();

		//    FILE *fs= fopen(outfile,"a+");
		FILE *fs = fopen(tmpfilename.c_str(), "a+");
		if (!fs)
		{
			cout << "Cannot open the file " << tmpfilename.c_str() << endl;
			exit(1);
		}

		if (true == calcongest)
		{
			fprintf(fs, "MaxCongvalue= %5.2f, mfbaverage = %8.5f, mfbstd = %8.5f, CPU1= %5.2f, CPU2= %5.2f, Cost= %.6f, Area= %.0f, Dead= %.4f, Wire= %.0f, Seed= %d ",
				MaxCongvalue,
				mfbaverage,
				mfbstd,
				total_cpu,
				last_time,
				double(fp.getCost()),
				double(fp.getArea()),
				double(fp.getDeadSpace()),
				double(fp.getWireLength()),
				rand_seed);
			fprintf(fs, " : %d %.2f %.4f \n", times, P, avg_ratio);
		}
		else
		{
			//fprintf(fs,"CPU1= %5.2f, CPU2= %5.2f, Cost= %.6f, Area= %.0f, Dead= %.4f, Wire= %.0f, Seed= %d ",
			//         total_cpu,
			//   last_time,
			//double(fp.getCost()),  
			//double(fp.getArea()),
			//double(fp.getDeadSpace()), 
			//         double(fp.getWireLength()), 
			//rand_seed );


			fprintf(fs, "casename %s blknum %d alpha %.2f beta %.2f Wire= %.0f delay %.2f Area %.0f CPU1 %5.2f CPU2 %5.2f Cost %.6f Deadspace %.4f%% Seed %d times %d p %.2f avg_ratio %.4f width %.0f height %.0f moverate %.1f sabsize %d sassize %d sacoa %.1f sacob %.1f smallarea %.1f percentprnum %.1f\n",
				filename,
				fp.size(),
				alpha,
				cost_beta,
				double(fp.getWireLength()),
				fp.getIntraDelay()*1e14,  //delay 1e-9
				double(fp.getArea()),
				total_cpu,
				last_time,
				double(fp.getCost()),
				double(fp.getDeadSpace()),
				rand_seed,
				times,
				P,
				avg_ratio,
				fp.getWidth(),
				fp.getHeight(),
				move_rate,
				sabigmodulesize,
				sasmallmodulesize,
				sacoefficienta,
				sacoefficientb,
				smallarea,
				percentprnum
				);
		}

		// PS:
		// avg_ratio <== avg delta cost

		fclose(fs);

		//need remove topclock
		//fp.combinePlaceFiles(filename);

		//for checking the nets in route
		//set<string> netnameSet;
		//fp.collectNetNames(filename, netnameSet);
	}

	//}
	//catch(...)
	//{
	// printf( "Exception occured\n" );
	//}
	return 1;
}
