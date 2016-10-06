// Project: B*-trees floorplanning
// Advisor: Yao-Wen Chang  <ywchang@cis.nctu.edu.tw>
// Authors: Jer-Ming Hsu   <barz@cis.nctu.edu.tw>
// 	    Hsun-Cheng Lee <gis88526@cis.nctu.edu.tw>
// Sponsor: Arcadia Inc.
// Date:    7/19/2000 ~

//---------------------------------------------------------------------------
#ifndef btreeH
#define btreeH
//---------------------------------------------------------------------------
#include <vector>
#include <cassert>
#include "fplan.h"
//---------------------------------------------------------------------------
const int NIL = -1;
typedef bool DIR;
const bool LEFT=0, RIGHT=1;

struct Node
{
  int id,parent,left,right;
  bool rotate,flip;
  bool isleaf(){ return (left==NIL && right==NIL); }
  Node() { id = NIL; parent = NIL; left = NIL; right = NIL; rotate = false; flip = false; }
};
 
struct Contour
{
  int front,back;
  Contour() { front = NIL; back = NIL; }
};

class B_Tree : public FPlan
{
  public:
    B_Tree(double calpha=1) 
		:FPlan(calpha),
	    isRecoverSol(false){}
    virtual void init();
    virtual void packing();
    virtual void perturb();
    virtual void keep_sol();
    virtual void keep_best();    
    virtual void recover();
    virtual void recover_best();


	////fbmao add for support split module perturb
	void splimodperturb();

	////fbmao add for support split module perturb <1>. for the architecture which is the same as the architecture in library.
	void splimodperturbwithoutstrongconstraint();

	///split modulefor fitting for different architectures  <2>  split for different architecture.
	void fordiffarchsplimodperturbwithoutstrongconstraint();

	/////Dynamic partition module during runtime.  <3> for the same architecture.
	void dynamicsplimodperturb();


    // debuging
    void testing();
    
    // set orientation according to the outline
    void setOrientation();

   //for congestion
	void Final_tree();
	virtual float congestimate();

protected:
    
	void show_tree();  
	void place_module(int mod,int abut,bool is_left=true);

	//fbmao for supporting 
	//update the rx of the module, to let the Region meet the resource requirement  (expand along x axis)
	void checkArchResourceAndUpdateModuleCoordinate(int modid, int &x, int &y, int &rx, int &ry);



    bool legal();
    void clear();
    
    // Auxilary function
    void wire_nodes(int parent,int child,DIR edge);
    int child(int node,DIR d);
    bool legal_tree(int p,int n,int &num);
    void add_changed_nodes(int n);
  
    // SA permutating operation
    void swap_node(Node &n1, Node &n2);
    void swap_node2(Node &n1, Node &n2);
    void insert_node(Node &parent,Node &node);

	//fbmao add for support split module. nodes have position constraint
	void insert_node(Node &parent, Node &node, bool isplaceleft);

    void delete_node(Node &node);
    
    bool delete_node2(Node &node,DIR pull);
    void insert_node2(Node &parent,Node &node,
                     DIR edge,DIR push,bool fold=false);

    
	////int contour_root;
 ////   vector<Contour> contour;
 ////   int nodes_root;
 ////   vector<Node> nodes;   
	////bool isRecoverSol;
 

	int contour_root;
	map<int, Contour> contour;
	int nodes_root;
	map<int, Node> nodes;
	bool isRecoverSol;


  private:        
 ////   struct Solution
	////{
 ////     int nodes_root;
 ////     vector<Node> nodes;
	////  vector<ModInfoForRecSol> recoverInfo;
 ////     double cost;
 ////     Solution() { cost = 1; }
	////  void clear() { cost = 1, nodes.clear(); recoverInfo.clear();}
 ////   };


	  struct Solution
	  {
		  int nodes_root;
		  int contour_root;
		  int modules_N;

		  ///keep modules, modules_Info
		  map<int, Module> modules;
		  map<int, Module_Info> modules_info;

		  //split nodes, node id will be changed. keep original node id unchanged
		  map<int, Node> nodes;
		  map<int, Contour> contour;
		

		//parent id.  submod-1  submod-2 .  if submodule is empty, set (NIL == -1) to this module
		//////Wirelength Calculation:  Used to get the top module ID from the submodule, then get module connections
		map<int, pair<int, int> > parentsubmoduleid;     /////if a module has several ratio. each ratio can be partitioned. 
		/////a parent may have several groups of submodule. consider ratio
		///need to change to  map<int, map<float, pair<int, int> > >


//		  map<int, ModInfoForRecSol> recoverInfo;  
		 ///// Need to change the recover function

		  double cost;
		  Solution() { cost = 1; }
		  void clear()
		  {
			  cost = 1;
			  nodes.clear();
			  modules_N = 0;
			  nodes_root = NIL;
			  contour_root = NIL;
			  modules.clear();
			  modules_info.clear();
			  contour.clear();
			  parentsubmoduleid.clear();
		  }
	  };
  
    void get_solution(Solution &sol);
    void recover(Solution &sol);
    void recover_partial();

    Solution best_sol, last_sol;
    // for partial recover
    vector<Node> changed_nodes;
    int changed_root;    
};

//---------------------------------------------------------------------------
#endif
