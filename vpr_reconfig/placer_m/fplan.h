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

//add by fbmao
//for routing, if route fail, expand space
const int expandcol = 2;
const int expandrow = 2;
const bool isrouteExpand = false; //default should be false


const long  FFlag = 500000;  //标志变量，用于判断grids[id].m_id是否为空

const int row_number = 100;
const int col_number = 100;
//const bool calcongest=false;   //mao add 用于判断是否计算拥塞  主要用于Btree.cc中的packing函数里
const bool calcongest= false; //false;   //if calcongest is true, the pin should have values

//add fbmao for cost function
extern float cost_beta;

//add fbmao give probability to choose different opreations Op1, Op2, Op3, Op4
extern double move_rate;

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

//for exploration
//percentage for prregion number  (random  percentage*totalmodulenum (fp.size())
extern float percentprnum;



//for support heterogeneous architecture.
const string blkRamType = "memory";
const string blkDspType = "mult_36";
const string blkClkType = "clk";


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

float x1,y1;  //左底角坐标
float x2,y2;   //右上角坐标

int row, col ;//    row col 
};                                       //把结构改为指针

typedef vector<Grid> Grids;
typedef  vector<long> Gids;

extern Grids grids;
struct Pin{
  int mod;
  int net;
  int x,y;    // relative position
  int ax,ay;  // absolute position

  int row;     //add mao
  int col;    //add mao
  int vertex;  //add mao      
  int  prop;       //  source(0), sink (1) or others(-1)    add mao
  Pin(int x_=-1,int y_=-1){ x=x_,y=y_; }
};

struct virtualPin{
  int mod;
  int net;
  int x,y;    // relative position
  int ax,ay;  // absolute position

  int row;     //add mao
  int col;    //add mao
  int vertex;  //add mao      
  int  prop;       //  source(0), sink (1) or others(-1)    add mao
  virtualPin(int x_=-1,int y_=-1){ x=x_,y=y_; }
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
  int width,height;
  int x,y;
  int area;
  float ratio;   //add by fbmao
  float delay; 
};

struct Module{
  int id;
  char name[300];   ///char name[70];
  int width,height;
  int x,y;   //module x, y doesnot update, but the modulInfo is updated 
  int area;

  int deltax;  //deltax, deltay used to map from Btree's coordinate to 
  int deltay;  // FPGA architecture coordinate

  Pins pins;
  Module_Type type;
  bool no_rotate;
  float ratio;   //add by fbmao
  //aspectRatio, pair<width, height>, Delay   
  //donnot use area since the real width / height may be not equal to aspec
  //aspeAreaDelay;   //notice type may need to modify
  map<float, pair< pair<int, int>, float> > aspeWidHeightDelay; 

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
  vector<map<string,vector<int> > > portconnectModuleIds; //used for calculate wirelenth bundingbox
 

   //for congestion
  Gids gids; //  add mao 记录模块覆盖的网格号
  int number;//  add mao  module 中pin number


  //fbmao add for support heterogeneous architecture
  int numram; //record the RAM and DSP number. Later to check whether the placement is legal or not
  int numdsp;

  //In order to move convenietly.  move one column one time.  column by column 
  //map<col, row blkname>

 
  
  //record io coordinate repeat???? 
  //First do not consider module's  the most left-bottom coordinate


     //function : readplacefileAndstoreIOcoordinates    // the two function. can combine together
  //consider the storage space.  assign module to module
    //map<ratio, map<x, map<pair<y,z> blkname> > >   <-->  map<col, map<row, blkname> >
  map<float, map<int, map<pair<int, int>, string> > > ratioxyzblkname; //in each module

  //store final coordinate of blks in arch
  map<int, map<pair<int, int>, string> > finalxyzblkname;


};
typedef vector<Module> Modules;

struct Module_Info{
  bool rotate, flip;
  int x,y;
  int rx,ry;
};



typedef vector<Module_Info> Modules_Info;

class FPlan{
  public:
    FPlan(double calpha);
    void read(char*);
    virtual void init()		=0;
    virtual void packing();
    virtual void perturb()	=0;    
    virtual void keep_sol()	=0;
    virtual void keep_best()	=0;
    virtual void recover()	=0;
    virtual void recover_best() =0;
    virtual double getCost();    
	void read_simple(char *file);

    int    size()         { return modules_N; }
	//for congestion
	double getcongvalue()   { return Congest; }; //mao add 

    double getTotalArea() { return TotalArea; }
    double getArea()      { return Area;      }
    int    getWireLength(){ return WireLength;}
    double getWidth()     { return Width;     }
    double getHeight()    { return Height;    }
	//double getAR()        { return Width/Height; }
	double getAR()        { return Height/Width; }
    double  getDeadSpace();
	double getIntraDelay() { return IntraDelay;}

    // outline
    double k3;
    double outline_width;
    double outline_height;
    double outline_ratio;
    bool isFit()
    {
      return ( Height <= outline_height && Width <= outline_width );
    }

    // information
    void list_information();
    void show_modules();    
    void normalize_cost(int);
	void outDraw( const char* outfilename );		// output drawing file
	void gnuplotDraw( const char* filename ); //output gnuplot drawing file

	//no update width and height .. Means the original draw figure.
	//However, outDraw()   support hetero arch.  rx may != x +width
	void noWidthUpdateOutDraw( const char* outfilename );   

	bool checkoverlap();
	
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



	void storeClbBlocksFromLoadableRegionFiles(string regionname); //from substitute region  clbfile 




	//submodulefilename is a place file path
	void readplacefileAndstoreIOcoordinates(string submodulefilename, string subfilename, float submoduleratio,
		                                   map<float, map<int, map<pair<int, int>, string> > > &ratioxyzblkname);


	//for support heterogenoeous architecture
	void updateSubModuleWidthAndHeight(string modname, float ratio, int modwidth, 
		const int modheight, int &blankRamNum, int &blankDspNum,
		Module &mod);
		//map<float, map<int, map<pair<int, int>, string> > > &ratioxyzblkname);
	void setFilenametoFp(char* file) { filename = file; };


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


    double calcWireLengthConsidermoduleioposAndFinaliopos();

	
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
	void GetRowColId( int x, int y, int & row, int & col, int & id );
	int  Max(Grids  &chain);
	void getcongmartrix();
	void freePtr();
    void freeglobal();
	virtual float congestimate() =0;



//for supporting heterogeneous architecture and modules with hetero blocks
    //memory 
	int fmemstart; 
	int fmemrepeat; 
	int fmemoryheight;

	//multiply
	int fmultistart; 
	int fmultirepeat; 
	int fmultiheight;
	
	//store modules with heterogenous block (Eg. RAM, DSP), in fplan file
	set<int> fheteromodidset;

	//total number of RAM, DSP
	int fmemorynum;
	int fmultiplynum;



  protected:
    void clear();
    double calcWireLength();
	double calIntraDelay();


	 //calculate wirelength and consider the original module port's position
	 double calcWireLengthConsidermoduleiopos(); 



    void scaleIOPad(); 
	
    double Area;
    double Width,Height;
    int WireLength;
    double TotalArea;
    double IntraDelay;
	double Congest;

    int modules_N;    
    Modules modules;
    Module  root_module;
    Modules_Info modules_info;    
    Nets network;
    double norm_area, norm_wire, norm_delay;
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
   
   //filename,  ratio        memnum  dspnum  (real vaild memnum, dspnum in original module, Region)
   map<string, map<float, pair<int, int> > > filenameRatioramdspcolnum;


   map<string, map<float, map<string, Finaliopos> > > filenameRatioioBlknamecoord;


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
    void read_IO_list(Module&,bool parent);
    void read_network();
    void create_network();   

    map<string,int> net_table;
    string filename;  //file name
	//map<clbname, clbtype> clbnametype
	map<string, string> clbnametype;
};


void error(char *msg,char *msg2="");
bool rand_bool();
double rand_01();
double seconds();
      
//---------------------------------------------------------------------------
#endif