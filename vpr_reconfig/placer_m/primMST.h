#ifndef LIUSH_TYPE
#define LIUSH_TYPE

typedef int NodeID;
# define M_ABS(A)            ((A)<0?-(A):(A))
typedef struct _MSTEdge{
	NodeID from;//one of the points of the MST edge
	NodeID to;//
} MSTEdge;

typedef struct _LSHMST{
	long netID;
	int nNode;
	MSTEdge *mstE;//the collection of edges of the MST
} LSHMST;

extern LSHMST *msts_;
int allPrimMST();
int printMST();

#ifndef OPTI2
//#define MAX_PAIR 2048
#define MAX_PAIR 4096
#endif

#define MAX_DOUBLE 1e18
#define MAX_NODE 1024
#define USE_DIST_TAB_LIMIT 100
#endif
	
