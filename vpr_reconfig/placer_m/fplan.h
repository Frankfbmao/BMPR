// Project: B*-trees floorplanning
// Advisor: Yao-Wen Chang  <ywchang@cis.nctu.edu.tw>
// Authors: Jer-Ming Hsu   <barz@cis.nctu.edu.tw>
// 	    Hsun-Cheng Lee <gis88526@cis.nctu.edu.tw>
// Sponsor: Arcadia Inc.
// Date:    7/19/2000 ~

//---------------------------------------------------------------------------
#ifndef fplanH
#define fplanH
//---------------------------------------------------------------------------
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <cstdio>
//---------------------------------------------------------------------------

using namespace std;



//use the roughly 
extern bool isrealnetlist; ///default 0: it is roughly netlist.     1: realnetlist

//add by fbmao
//for routing, if route fail, expand space
const int expandcol = 2;
const int expandrow = 2;
const bool isrouteExpand = false; //default should be false


const long  FFlag = 500000;  //标志变量，用于判断grids[id].m_id是否为空

const int row_number = 100;
const int col_number = 100;
//const bool calcongest=false;   //mao add 用于判断是否计算拥塞  主要用于Btree.cc中的packing函数里
const bool calcongest = false; //false;   //if calcongest is true, the pin should have values

//add fbmao for cost function
extern float cost_beta;

//add fbmao give probability to choose different opreations Op1, Op2, Op3, Op4
extern double move_rate;
extern double swap_rate;

//arch name
extern string archname;
extern int rd;
extern int timesformsize;

extern int sabigmodulesize;  //for design exploration
extern int sasmallmodulesize;
extern float sacoefficienta;
extern float sacoefficientb;
extern float smallarea;
extern int io3;

extern int isheteroarch; // the targeted arch is heterogenous arch or general arch without memory,mulply etc.
extern int isvprarch1; //1: vpr arch. k6_N10_memDepth16384_memData64_40nm_timing (in vpr arch dir).

//for exploration
//percentage for prregion number  (random  percentage*totalmodulenum (fp.size())
extern float percentprnum;

extern int isoneratioflow; //1: one ratio flow.  0: multi-ratio flow
extern string tmponeratiodir;
extern string tmpmultiratiodir;

//for support heterogeneous architecture.
const string blkRamType = "memory";
const string blkDspType = "mult_36";
const string blkClkType = "clk";

//for support split after some iteration (not at the beginning or )
extern int issplitaftersometime;   //1: split it after some time   0: split at the begging. start from a random initial solution.

extern int isinternosplitoneratio; ///1: do not 

extern int issplitInCurrentratio; //1: split based on one ratio.  0:  can choose multi-ratio

///1: split module to fit for different architectures  
///0: do not execute the flow that split module to fit for different architectures 
extern int issplitfitfordifferentarch;


////1:  traditional flow to fit for different architectures
////0:  traditional flow does not execute to be fit for different architectures
extern int istradfitfordiffarch;  //default 0

/////architecture type :  for fitting for different architectures.   
////has some type. 0, 1, 2
extern int garchtype;   ///default 0

///global variable. remove the clb blocks
extern int grmclb;   //In split flow. 1: execute the flow. remove the empty CLB columns   0: Do not remove. 
                     //Default value is  0  

///1: dymaic partition a module into submodules
///0: do not do dynamic partition during runtime
extern int isdynamicpartition; //default 0

//
struct IOposition{
	int left1;
	int left2;
	int bottom1;
	int bottom2;
	int right1;
	int right2;
	int top1;
	int top2;
};

struct Finaliopos{
	int x;
	int y;
	int z;
};



//for output.draw and .plt file
extern string tmpoutfile;

struct Grid_Net{

	//long net;      //net ID
	long from;    // GRG中Node ID    通过此网格ID中所相交的模块所覆盖的几个网格区域向外扩
	long to;
};
typedef Grid_Net grid_net;

struct Grid{                        //add mao
	long id;     //grid id
	float value; //congestion value
	vector<grid_net> net; // net id passed the grid
	vector<int> m_id; //与此网格相交的模块的id

	//long x1,y1;  //左底角坐标
	//long x2,y2;   //右上角坐标

	float x1, y1;  //左底角坐标
	float x2, y2;   //右上角坐标

	int row, col;//    row col 
};                                       //把结构改为指针

typedef vector<Grid> Grids;
typedef  vector<long> Gids;

extern Grids grids;
struct Pin{
	int mod;
	int net;
	int x, y;    // relative position
	int ax, ay;  // absolute position

	int row;     //add mao
	int col;    //add mao
	int vertex;  //add mao      
	int  prop;       //  source(0), sink (1) or others(-1)    add mao
	Pin(int x_ = -1, int y_ = -1){ x = x_, y = y_; }
};

struct virtualPin{
	int mod;
	int net;
	int x, y;    // relative position
	int ax, ay;  // absolute position

	int row;     //add mao
	int col;    //add mao
	int vertex;  //add mao      
	int  prop;       //  source(0), sink (1) or others(-1)    add mao
	virtualPin(int x_ = -1, int y_ = -1){ x = x_, y = y_; }
};

//for calculating congestion 
extern  vector<vector<virtualPin> >netsets;
extern  vector<int> numbersets;

typedef vector<Pin> Pins;
typedef Pin* Pin_p;
typedef vector<Pin_p> Net;
typedef vector<Net > Nets;

enum Module_Type { MT_HARD, MT_SOFT, MT_RECLINEAR };

//module info for recover
struct ModInfoForRecSol{
	int width, height;
	int x, y;
	int area;
	float ratio;   //add by fbmao
	float delay;
};

struct Module{
	int id;
	char name[300];   ///char name[70];
	int width, height;
	int x, y;   //module x, y doesnot update, but the modulInfo is updated 
	int area;

	int deltax;  //deltax, deltay used to map from Btree's coordinate to 
	int deltay;  // FPGA architecture coordinate

	int issplitmod;  ///only has two values (0 and 1)
	float curvertipartiRatio; ////current partition ratio, if it is a split module . //default 1  Do not do horizontal parition

	float curHoripartiRatio; ////currently do not have horizontal partition. (e.g., carry chain cannot be paritioned)


	Pins pins;
	Module_Type type;
	bool no_rotate;
	float ratio;   //add by fbmao
	//aspectRatio, pair<width, height>, Delay   
	//donnot use area since the real width / height may be not equal to aspec
	//aspeAreaDelay;   //notice type may need to modify
	map<float, pair< pair<int, int>, float> > aspeWidHeightDelay;


	//fbmao add for supporting splitting module
	//bake original width and height of module information
	map<float, pair< pair<int, int>, float> > bakeoriaspeWidHeightDelay;    ////non -split module has this property



	// sort them according to the importance  
	//(eg. area * delay with less value is most important) ascending order
	vector<float> aspec;
	float delay;
	string clkname;
	set<string> inputports;
	set<string> outputports;

	//1.the connectModuleId doesn't contain the port name
	//one port corresponding to 
	vector<vector<int> > connectModuleIds; //used for calculate wirelenth bundingbox


	//2.the connectModuleId contain the port name
	vector<map<string, vector<int> > > portconnectModuleIds; //used for calculate wirelenth bundingbox


	//fbmao add for supporting split module.  the modules has position constraints
	////map<left, or right, parentid, modid>
	map<string, int> constraintmodset;   ///default is empty set   (record modules' position constraint)
	/// this set also can be used to judge whether the module is splitted or not

	//support for fitting for different architectures
	vector<int> adjacentmodset; ///////after split, it records the another module id.  May do not have sibling module  (eg. another module is empty)

	//for congestion
	Gids gids; //  add mao 记录模块覆盖的网格号
	int number;//  add mao  module 中pin number


	//fbmao add for support heterogeneous architecture
	int numram; //record the RAM and DSP number. Later to check whether the placement is legal or not
	int numdsp;

	int curbelongnpart; ///if do dynamic partition. it is belong to (2 partition or 3 partition???. The result is from 2 parts or 3 parts???)
	                    ///currently, it is 2 parts? 3 parts?

	//In order to move convenietly.  move one column one time.  column by column 
	//map<col, row blkname>

	//record io coordinate repeat???? 
	//First do not consider module's  the most left-bottom coordinate

	//function : readplacefileAndstoreIOcoordinates    // the two function. can combine together
	//consider the storage space.  assign module to module
	//map<ratio, map<x, map<pair<y,z> blkname> > >   <-->  map<col, map<row, blkname> >
	map<float, map<int, map<pair<int, int>, string> > > ratioxyzblkname; //in each module   for top module

	//for supporting splitting module. parittion based on ratio or (RAM, DSP column)
	map<float, map<int, map<pair<int, int>, string> > > bakeoriginalratioxyzblkname; //in each module

	//store final coordinate of blks in arch
	map<int, map<pair<int, int>, string> > finalxyzblkname;


	///Notice <1>  <2> are splitting flow.  <1> partition based on ratio    <2> partition based on RAM, DSP.
	////can choose different positions for slitting a specific module  (partitionratio. eg. 0.2, 0.4, 0.5, 0.6, 0.8)
	map<float, map<float, map<int, map<pair<int, int>, string> > > > ratiopartratioxyzblkname; //for each split module   <1>

	map<float, map<float, map<int, map<pair<int, int>, string> > > > bakeoriginalratiopartratioxyzblkname; //<1>  for each split module. consider parition ratio


	////split for fitting for different architectures flow. partition based on RAM or DSP column
	map<float, map<int, map<pair<int, int>, string> > > diffarchratioxyzblkname; //for each split module       <2>

	////split for fitting for different architectures flow
	map<float, map<int, map<pair<int, int>, string> > > bakediffarchratioxyzblkname; //for each split module    <2>  partition based on RAM, DSP


	////split for fitting for the same architectures (can divide the module into 2 parts or 3 parts or 4 parts ...)
	map<float, map<int, map<int, map<pair<int, int>, string> > > > fordynamicratiopartnumxyzblkname; //for each split module       <3>

	////split for fitting for different architectures flow
	map<float, map<int, map<int, map<pair<int, int>, string> > > > bakefordynamicratiopartnumxyzblkname; //for each split module    <3>  partition the module into 2 or 3 or 4 parts.


	///initial construct function
	Module()
	{
		issplitmod = 0; //default. 1: represent split mod.   0: represent split mod
		curvertipartiRatio = 1.0; ////default
		numram = 0;
		numdsp = 0;
		curbelongnpart = 0; //at least has two submodule, if do dynamic partition. Or else it is the original module.
		connectModuleIds.clear();  //store the connected module's id 
		portconnectModuleIds.clear();
		inputports.clear();
		outputports.clear();
		aspeWidHeightDelay.clear();
		aspec.clear();
		ratioxyzblkname.clear();
		bakeoriginalratioxyzblkname.clear();
		finalxyzblkname.clear();
		constraintmodset.clear();
		adjacentmodset.clear();

		///partition based on ratio. 
		bakeoriginalratiopartratioxyzblkname.clear();
		ratiopartratioxyzblkname.clear();

		///fit for different architecture. partition based on RAM, DSP column
		diffarchratioxyzblkname.clear();
		bakediffarchratioxyzblkname.clear();
	}

};

//fbmao add for supporting split module
////typedef vector<Module> Modules;
typedef map<int, Module> Modules;

struct Module_Info{
	bool rotate, flip;
	int x, y;
	int rx, ry;
};


//fbmao add for supporting split module
////typedef vector<Module_Info> Modules_Info;
typedef map<int, Module_Info> Modules_Info;


//The rule for defining submodule's name is like below.
//A  partition level: 1    -> submodule  A#11 A#12
// recursive parititon.  partition level:2 -> (A#11) submodule A#11#11  A#11#12    
//partition level: 3     
//
struct splitmodule
{
	int partitionLevel; //eg. default = 0.  do not split module
	//1: partition 1 time(level)	                   //2: partition 2 times(level)
	float oriparentratio;  //the most original module ratio of the module
	float partitioncost;

	///////1. split module based on ratio and has only each parent module only has two submodules.  (currently)
	///////2.  split module based on the RAM, DSP column
	vector<Module> submodset;

	/////Notation Description: 
	///partitionType [1: verical partition.   0:horizontal partition. Default 1]

	///splitPositionRatio [split (partition) ratio (left-part / (left-part+right-part) ]
	//eg. balanced partition is left/right = 1:1, ratio = 1/(1+1) = 0.5
	//default 0.5 [left, median] and [median, right] can be got from module coordinate and width, height 

	///struct splitmodule  [use to record submodule information]

	//parent module name (before partition)

	///partitioncost  [area, delay, wirelength form an estimation function???]

	////map<parentModulename, partitionType, splitPositionRatio, splitmodule, partitionCost(default 0.0)>
	////	map<string, map<int, map<float, map<splitmodule, float> > > > parentmodnamePartypePartratioSplitmod;  /////non-empty  only when more than two levels
	map<string, map<int, map<float, splitmodule> > > parentmodnamePartypePartratioSplitmod;  /////non-empty  only when more than two levels


	///[partitioncost, sort them according to the split cost].
	//Need to map to board (based on origianl module), need to record the values below
	map <string, vector < map<splitmodule, pair<int, float> > > > parentmodnameSplitmodPartypePartratio; /////non-empty only when more than two levels

	splitmodule()
	{   //default
		partitionLevel = 0; //partition level
		oriparentratio = 1.0;  //default
		partitioncost = 0.0;
		submodset.clear();
		parentmodnamePartypePartratioSplitmod.clear();
		parentmodnameSplitmodPartypePartratio.clear();
	}
};


typedef enum archtypevalue
{
	ARCH_0, ARCH_1, ARCH_2, ARCH_3
} ARCH_TYPE_VALUE;


class FPlan{
public:
	FPlan(double calpha);
	void read(char*);
	virtual void init() = 0;
	virtual void packing();
	virtual void perturb() = 0;
	virtual void keep_sol() = 0;
	virtual void keep_best() = 0;
	virtual void recover() = 0;
	virtual void recover_best() = 0;
	virtual double getCost();
	void read_simple(char *file);

	int    size()         { return modules_N; }
	int modNumBeforeSplit() { return originalmodnamemodid.size(); }

	//for congestion
	double getcongvalue()   { return Congest; }; //mao add 

	double getTotalArea() { return TotalArea; }
	double getArea()      { return Area; }
	int    getWireLength(){ return WireLength; }
	double getWidth()     { return Width; }
	double getHeight()    { return Height; }
	//double getAR()        { return Width/Height; }
	double getAR()        { return Height / Width; }
	double  getDeadSpace();
	double getIntraDelay() { return IntraDelay; }


	void specifyoneratioformodules();
	void splitmoduleAndRecordSubmodule();

	////split modules for fit for different architecture.
	void splitmodulefitfordifferentarch();

	////dynamic partition (split modules) (partition into two parts, three parts or four parts??)
	////more than three or four parts needs much time. in a tradeoff.  consider two or three parts.
	void splitmodulefordynamicpartition();

	////revise the wirelength function, for support split operation
	void readcombinednetlist(string netname, string archname);

	///fbmao support split flow
	///New Wirelength function for supporting calculate wirelength
	void getmodulenameRatioBlknameCoordinateFromOrignalVPRfile();
	void readplacefileAndstoreClbRamDspCoordinates(string submoduleplacefile, string subfilename, float submoduleratio);


	//add by fbmao for supporting splitting module
	//top module name & ratio & (horizontal Split 0 or Vertical Split 1) & partitionRatio & its submodule set & partitioncost   //first level (the topest level)
	map<string, map<float, map<int, map<float, splitmodule> > > > topmodnamesubmodset;


	//add by fbmao. splitting module for fitting for different architectures
	//top module name & ratio & (horizontal Split 0 or Vertical Split 1)  & its submodule set & partitioncost   //first level (the topest level)
	map<string, map<float, map<int, splitmodule > > > fitfordiffarchtopmodnamesubmodset;


	//dynamic splitting module for the same architecture.  split a module into 2 or 3 or 4 or 5 parts.
	//top module name & ratio & (horizontal Split 0 or Vertical Split 1)  & partnum(how many parts) & its submodule set & partitioncost   //first level (the topest level)
	map<string, map<float, map<int, map<int, splitmodule > > > > fordynamictopmodnamesubmodset;


	// outline
	double k3;
	double outline_width;
	double outline_height;
	double outline_ratio;
	bool isFit()
	{
		return (Height <= outline_height && Width <= outline_width);
	}

	// information
	void list_information();
	void show_modules();
	void normalize_cost(int);
	void outDraw(const char* outfilename);		// output drawing file
	void gnuplotDraw(const char* filename); //output gnuplot drawing file

	//no update width and height .. Means the original draw figure.
	//However, outDraw()   support hetero arch.  rx may != x +width
	void noWidthUpdateOutDraw(const char* outfilename);

	bool checkoverlap();

	///check the IO positions are enough or not for placing the IO blocks
	void checkIOPositionEnoughAndExpandwidthAndHeight();

	//for congestion

	void Output_positions(char * positionFile);


	//1.for soft module based placement
	void traceSoftModuleConnections();
	void creatNetworkOfModules();
	void storeClbAndHeteroBlocksFromvprFiles(); //from vpr.out file

	//	void storeIoBlocksFromplaceFiles(); //store io block from the vpr file

	//for supporting heterogeneous architecture and modules with heterogeneous block
	void recordMemDspnumforEachModule();  //for later checking the 
	void recordDataofArchitecture(string archname);

	void setParameterForDifferentArch(int archtype); ///choose a architecture type and then set the parameters

	void storeClbBlocksFromLoadableRegionFiles(string regionname); //from substitute region  clbfile 

	//submodulefilename is a place file path
	void readplacefileAndstoreIOcoordinates(string submodulefilename, string subfilename, float submoduleratio,
		map<float, map<int, map<pair<int, int>, string> > > &ratioxyzblkname);


	//for support heterogenoeous architecture
	void updateSubModuleWidthAndHeight(string modname, float ratio, int modwidth,
		const int modheight, int &blankRamNum, int &blankDspNum, int &blankClbNum,
		Module &mod);


	//spliting a module into several submodules based on RAM, DSP column
	//store blks' coordinate
	void fordiffarchupdateSplitModuleWidthAndHeight(string modname, float ratio, const int modwidth,
		const int modheight, int &blankRamNum, int &blankDspNum, int &blankClbNum, Module &mod,
		int isVertical, Module &parentmod, int tmpbegin, int tmpend);

	void fordynamicflowupdateSplitModuleWidthAndHeight(string modname, float ratio, const int modwidth,
		const int modheight, int &blankRamNum, int &blankDspNum, int &blankClbNum, Module &mod,
		int isVertical, Module &parentmod, int tmpbegin, int tmpend);

	//for support spliting a module into two submodules.
	//store blks' coordinate
	void updateSplitModuleWidthAndHeight(string modname, float ratio, const int modwidth,
		const int modheight, int &blankRamNum, int &blankDspNum, int &blankClbNum, Module &mod,
		int isleftbelowmod, int isVertical, Module &parentmod);


	//submodulefilename is a place file path
	//isleftbelowmod only has two values (0 and 1). 1 : this submodule is leftOrBelow mod after splitting
	void getsplitmodblkscoordinate(Module &parentmod, Module &submod, int isleftbelowmod,
		int isVertical, float submoduleratio,
		map<float, map<float, map<int, map<pair<int, int>, string> > > > &ratiopartratioxyzblkname);


	///split modules for fitting for different architectures
	void fordiffarchgetsplitmodblkscoordinate(Module &parentmod,
		int tmpwidthbegin, int tmpwidthend, int isVertical, float submoduleratio,
		map<float, map<int, map<pair<int, int>, string> > > &ratioxyzblkname);

	///split modules into 2 or 3 or 4 or 5 or ... n parts.
	void fordynamicgetsplitmodblkscoordinate(Module &parentmod,
		int tmpwidthbegin, int tmpwidthend, int isVertical, float submoduleratio,
		map<float, map<int, map<int, map<pair<int, int>, string> > > > &ratiopartnumxyzblkname);


	//map<float, map<int, map<pair<int, int>, string> > > &ratioxyzblkname);
	void setFilenametoFp(char* file) { filename = file; };
	string getFilenamefromFP() { return filename; };


	void placeMemoryAndMultiply();

	void combinePlaceFiles(char *filename);

	//for support hetero
	void getFinalPlaceFiles(char *file);

	//for support heterogeneous architecture
	void doLocalplaceForEachmoduleMapToArch();

	//for route to set occ (full capacity) and set inode
	void collectReconfigRegion(char *filename, vector<struct coord> &regionSets);
	void collectNetNames(char *filename, set<string> &netnameSet);


	//add later for randomly placing IO and also considering the IO position (final IO and module ports coordinate)
	void traceSoftModuleConnectionsKeepportname();  // portname connectIds,  vector<map<portname, vector<int>connectmoduleIds>>


	//2.for congestion
	double mfbmean(Grids  &chain);
	double mfbvar(Grids  &chain);

	//get allIO blocks
	void getfinalioblocks();

	void getblksnamemaplist();

	int getmaxnumratio() { return maxnumratio; }


	//add for loadable flow
	void getloadableRegionAndReplace(int archwidth, int archheight);

	//combine subplace files into a complete place file after loadable work
	void combineplacefileAfterloadable();

protected:

	//fbmao add for loadable flow
	void  readAndRewriteplace(int coordx, int coordy, string subplacefile,
		string newplacefile, string regionname, int archwidth, int archheight);


	void *my_malloc(size_t size);
	char **ReadLineTokens(FILE * InFile, int *LineNum);
	void readAndWrite_place(const char *subplacename,
		string finalplacefilename,
		string finalroutenetfilename,
		string virtualpinfilename,
		const char *subplace_file,
		const char *subroute_file,
		int nx, int ny,
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
		int &flagindex,
		int &zflag,
		vector<string> &clockSet);


	void assignIOAccordOriginCoordinate(
		int x, //iox
		int y, //ioy
		int z, //ioz
		string tempa, //ioname
		Module &mod,
		Module_Info &modInfo);


	void readplacefileAndReturnXYcoordinates(string portname,
		const char *subplace_file, int &xcoord, int &ycoord);

	//1. IO blocks place in oder, left, right, lower, top.  Place io blocks one by one and
	//don't consider the IO coordinate in the original placement file
	void moveIOblksToFinalplacefile(string finalplacefilename,
		int &blkcount,
		int &ioblkcount);

	//2. IO blocks place in oder, left, right, lower, top.  Place io blocks one by one and
	//consider the IO coordinate in the original placement file
	void moveIOblksToFinalplacefileConsiderOrigiposition(string finalplacefilename,
		int &blkcount,
		int &ioblkcount);

	//3. Place io blocks and consider the IO coordinate in the original placement file
	//random get Io block's coordinate
	void moveRandomIOblksToFinalPlacefile(string finalplacefilename,
		int &blkcount,
		int &ioblkcount);

	//4. move memory and multiply .etc. to final place file
	void moveMemoryAndMultiToFinalPlacefile(string finalplacefilename,
		int &blkcount);

	void getInitialIOblksposition();

	void randomFinalIoblks();


	void getioresource();  //for simple io optimization


	//sidename: left1, left2, top1, top2, right2, right1, bottom2, bottom1
	int getIoPos(int &x, int &y, int &z, string sidename);   //0  wrong data    1:right data


	//2.for congestion driven placement
	//add mao
	int getgridPosition();
	//	void getNetwork(); //bug
	//	void subnetworksize(char);
	void creatNetworkRCol();

	void creatPtr();                            //add mao
	void maplist(float &w, float &h);      //  mao add 
	void GetRowColId(int x, int y, int & row, int & col, int & id);
	int  Max(Grids  &chain);
	void getcongmartrix();
	void freePtr();
	void freeglobal();
	virtual float congestimate() = 0;

	//for supporting heterogeneous architecture and modules with hetero blocks
	//memory 
	int fmemstart;
	int fmemrepeat;
	int fmemoryheight;

	//multiply
	int fmultistart;
	int fmultirepeat;
	int fmultiheight;

	///IO capacity   for calculating how many positions for placing IO blocks.
	int fiocapacity;

	//store modules with heterogenous block (Eg. RAM, DSP), in fplan file
	/////	set<int> fheteromodidset;   ///use mod.numdsp, mod.numram instead of fheteromodidset

	//total number of RAM, DSP
	int fmemorynum;
	int fmultiplynum;

	///for split flow. record the original top module
	Modules bakeoriginaltopmodules;

	///modname, modid     (original module name and mod id) it is unique
	map<string, int> originalmodnamemodid;

	set<int> ignoreNetid;

protected:
	void clear();
	double calcWireLength();
	double calIntraDelay();

	double calcWireLengthConsidermoduleioposAndFinaliopos();

	//calculate wirelength and consider the original module port's position
	double calcWireLengthConsidermoduleiopos();


	//calculate wirelength and consider the original module port's position
	double calcWireRealnetlist();


	//fit for different architectures
	//calculate wirelength and consider the original module port's position
	double diffarchcalcWireRealnetlist();

	//calculate wirelength and consider the original module port's position
	double diffarchcalcWireLengthConsidermoduleiopos();

	void scaleIOPad();

	double Area;
	double Width, Height;
	int WireLength;
	double TotalArea;
	double IntraDelay;
	double Congest;

	//fbmao new add
	double Deadspace;

	////fbmao add for supporting spliting modules
	////
	double normwidthheight;

	int oritopmoduleNum; ///the most original number of top modules

	int modules_N;
	Modules modules;
	Module  root_module;
	Modules_Info modules_info;
	Nets network;
	double norm_area, norm_wire, norm_delay;

	//fbmao new add
	double norm_deadspace;


	double norm_congest;

	double cost_alpha;


	vector<vector<int> > connection;

	//add by fbmao 
	//filename, ratio, blkname
	map<string, map<float, set<string> > > filenameRatioBlkname;

	//fbmao add for supporting heterogeneous architecture
	//filename, blktype, blkname
	map<string, map<string, set<string> > > filenameblktypeblkname;
	//filename, ramdspblknames
	map<string, set<string> > filenameRamDspBlks;

	///only for top module
	//filename,  ratio        memnum  dspnum  (real vaild memnum, dspnum in original module, Region)
	map<string, map<float, pair<int, int> > > filenameRatioramdspcolnum;


	///only for split module
	///for support split flow. record the number of column (RAM, DSP) in a module 
	//splitfilename,  ratio   partitionRatio     memnum  dspnum  (real vaild memnum, dspnum in original module, Region)
	map<string, map<float, map<float, pair<int, int> > > > splitfilenameRatioPartratioramdspcolnum;

	///splitting for fitting for different architectures
	map<string, map<float, pair<int, int> > > fordiffarchsplitfilenameRatioramdspcolnum;

	///split module in two or three or n parts. record the number of column (RAM, DSP) in a module 
	//splitfilename,  ratio   partitionRatio     memnum  dspnum  (real vaild memnum, dspnum in original module, Region)
	///can divide the modules into 2 or 3 or 4 or 5 ... parts????
	map<string, map<float, map<int, pair<int, int> > > > dynamicsplitfilenameRatioPartnumramdspcolnum;

	map<string, map<float, map<string, Finaliopos> > > filenameRatioioBlknamecoord;

	//support new wirelength calculation and split module calculation
	////map<filename, ratio, blkname, xyz>;
	///record blk information in top module
	//////map<topmodulename, map<ratio, map<blkname, xyzcoordinate> > >
	map<string, map<float, map<string, Finaliopos> > > topmodnameRatioBlknameCoord;

	///1. record blk information in split module
	////map<splitmodname, map<parentmodratio, map<partitionratio, map<blkname, xyzcoordinate> > > >
	map<string, map<float, map<float, map<string, Finaliopos> > > > splitmodnameRatioPartratioBlknameCoord;

	////2. fit for different architectures
	///record blk information
	////map<splitmodname, map<parentmodratio, map<blkname, xyzcoordinate> > >
	map<string, map<float, map<string, Finaliopos> > > diffarchsplitmodnameRatioBlknameCoord;

	////fit for the same arch, but do dynamic partition
	///record blk information
	////map<splitmodname, map<parentmodratio, map<partnum, map<blkname, xyzcoordinate> > > >
	map<string, map<float, map<int, map<string, Finaliopos> > > > dynsplitmodnameRatioPartnumBlknameCoord;

	///partition based on the ratio and only has two submodules.
	//parent id.  submod-1  submod-2 .  if submodule is empty, set (NIL == -1) to this module
	//////Wirelength Calculation:  Used to get the top module ID from the submodule, then get module connections
	map<int, pair<int, int> > parentsubmoduleid;     /////if a module has several ratio. each ratio can be partitioned. 
	/////a parent may have several groups of submodule. consider ratio
	///need to change to  map<int, map<float, pair<int, int> > >

	//for fitting for different architectures.
	//parent id.  submodules id.
	//////Wirelength Calculation:  Used to get the top module ID from the submodule, then get module connections
	map<int, vector<int> > diffarchparentsubmoduleidset;     /////if a module has several ratio. each ratio can be partitioned. 

	//for loadable flow, record the clb block in the loadable region
	set<string> loadableregionblknames;


	//add by fbmao 
	//for IO match (name and number should be the same as in the netlist file)
	set<string> finaliosets; //IOs get from the netlist
	set<string> clkset;

	//memory and multiply
	set<string> memoryset;
	set<string> multiplyset;

	//map<memoryMultiplyname, coordinates>
	map<string, Finaliopos> memoryMultiplyCoords;

	//map floorplan X cooridnate to fpga X coordinate
	map<int, int> floorplanXtofpgaX;

	//original blkname in place file -> pre-vpr blkname in pre-vpr place file
	map<string, string>blksnamemap;  //for move the clb, memory, io to the final place file

	map<string, int> ionameindex;

	//for congestion assign different weights
	//get the max terminals of all the nets
	int maxNumTerminal;


	//for choose IO position
	map<string, struct IOposition> ionameIopos;

	//collect IO resource
	//left1, left2, top1, top2, right2, right1, bottom2, bottom1 --left1
	map<string, vector<Finaliopos> > ioresource;

	int maxnumratio;  //show how many ratios are be considered

	vector<Finaliopos> finaliocoordinates;
	vector<Finaliopos> allIoresource;


private:
	void read_dimension(Module&);
	void read_IO_list(Module&, bool parent);
	void read_network();
	void create_network();

	map<string, int> net_table;
	string filename;  //file name
	//map<clbname, clbtype> clbnametype
	map<string, string> clbnametype;
};


void error(char *msg, char *msg2 = "");
bool rand_bool();
double rand_01();
double seconds();

//---------------------------------------------------------------------------
#endif