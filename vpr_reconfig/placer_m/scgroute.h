
#ifndef Included_ScGRoute
#define Included_ScGRoute


#include <stdlib.h>
//#include "sttools.h"
/*
The following define exclude the ScSteiner.h
I think we should not define variables in include files.
Variables should be defined in the C file, but used 'extern' keyword
in the include file to indicate it is a global variable.
*/
#define DEFexcludeSteiner_H
#include "SCinclude.h"


/*
You can prevent this header file to define DEBUG parameters by the definition of BHYdebugParametersDefined
*/
#ifndef BHYdebugParametersDefined
#define	BHYdebugAllMain		0x01
#define BHYdebugMain		0x1b
#define	BHYdebugArraySteiner	0x03
#define	BHYdebugFixness		0x0f
#define	BHYdebugBuildGrg	0x07
#define BHYdebugBuildNet	0x01
#define	BHYdebugTimingTree	0x1f
#define BHYdebugPert		0x0f
#define BHYdebugQueueTools	0x01

#define BHYsubscriptStartsFrom1
#define BHYaccuratePERT
#define BHYdeltaCongestValueAccurate

#endif


#include "sttools.h"

/*
#define BHYpresetCapacityOfSegments
*/

typedef unsigned int T16Bits;


#ifndef UP
#	define UP           0
#	define DOWN         1
#	define LEFT         2
#	define RIGHT        3
#endif

#define     LIMITED   15

#ifndef NAMESIZE
#	define	    NAMESIZE  80
#endif

#ifndef HORI
#	define     HORI      0
#	define     VERT      1
#endif

#ifdef BHY4BC
#	define     LSH_NETSIZE   2000
#	define     CIRCUITSIZE  100
#	define     COUNT     1000
#	define     MAXLENGTH 1000
#else
#	define     LSH_NETSIZE   25000  //90000
#	ifndef CIRCUITSIZE
#		define     CIRCUITSIZE  30000
#	endif
#	define     COUNT     100000
#	define     MAXLENGTH 100000
#endif


/*
#define BHYwireOnly
*/

/*
#define BHYalphaCongestion      1.0
#define BHYalphaLength          0.10
#define BHYalphaTwist           0.00
*/


#ifndef BHYwireOnly
/*
增量网络结构 -- 邻接表形式
*/

/* 边链表	*/typedef struct _BHYAFNedge
{
	/* 斜率		*/ 
	float	slope;
	/* 临点指针	*/ 
	struct _BHYAFNnode	*adjacent;
	/* 下一边	*/ 
	struct _BHYAFNedge	*next;
	/* 反向边。Added by Bao Hai-yun on 1998.10.13	*/ 
#ifdef BHYaccuratePERT
	struct _BHYAFNedge	*backEdge;
#endif
} BHYAFNedge;


/* 需求点在优先队列中		*/ 
#define BHYMaskAFNnodeFlagVisited		0x01
#define BHYMaskAFNnodeFlagNotVisited \
	( \
		BHYmaskInt \
		^BHYMaskAFNnodeFlagVisited \
	)

/* 节点链表	*/ 
typedef struct _BHYAFNnode
{
	/* 对应需求点指针	*/ 
	struct _BHYrequiredNode *rn;
	/* 第一条增流网边	*/ 
	struct _BHYAFNedge	*edge;
	/* 下一节点		*/ 
	struct _BHYAFNnode	*next;
	/* 第一条增流网反向边：Added by Bao Hai-yun on 1998.10.13	*/ 
#ifdef BHYaccuratePERT
	struct _BHYAFNedge	*backEdge;
#endif
	int			flag;
} BHYAFNnode;


/*
用于表示单元内部时延传递关系的边链表
*/
typedef struct _BHYdelayTransmit
{
	/* 相应的需求点指针	*/ 
	struct _BHYrequiredNode		*rn;
	/* 单元内部延时		*/ 
	float				delay;
	/* 下一节点		*/ 
	struct	_BHYdelayTransmit	*next;
} BHYdelayTransmit;
#endif

#define BHYdelayTransmitPush( node, head )\
	(node)->next = head; \
	head = node;


/* 需求点在优先队列中		*/ 
#define BHYMaskRequiredNodeFlagInQueue		0x002
/* 需求点在时延关系中是起点	*/ 
#define BHYMaskRequiredNodeFlagBegin		0x004
/* 需求点在时延关系中是终点	*/ 
#define BHYMaskRequiredNodeFlagEnd		0x008
/* 需求点在时延关系中是中间点	*/ 
#define BHYMaskRequiredNodeFlagMid		0x010
/* 需求点不在优先队列中		*/ 
#define BHYMaskRequiredNodeFlagNotInQueue \
	( \
		BHYmaskInt \
		^ BHYMaskRequiredNodeFlagInQueue \
	)
/* 需求点未确定时延关系		*/ 
#define BHYMaskRequiredNodeFlagNotSet \
	( \
		BHYmaskInt \
		^ BHYMaskRequiredNodeFlagBefgin \
		^ BHYMaskRequiredNodeFlagEnd \
		^ BHYMaskRequiredNodeFlagMid \
	)

/* 需求点结构	*/
typedef struct _BHYrequiredNode
{
	/* The ID of the node	*/ 
	int			nodeID;
#ifndef BHYwireOnly
	/* 够唯一确定需求点的 ID。由线网号和该节点在线网中的序号决定。 */ 
	/* 线网号	*/ 
	int			netId;
	/* 节点顺序号，如果该节点是源则为 -1	*/ 
	int			nodeSeq;
	/* 偏序关系号	*/ 
	int			partialOrder;
	/* The flag used in the process of delay transmission */ 
	int			flag;
	/* The electric capacity of the node. 	*/ 
	float			cIn;
	/* Required delay of the node	*/ 
	float			requiredDelay;
	/* Current delay of the node	*/ 
	float			currentDelay;
	/* 对应的增量网络中的节点指针，若增量网络中没有该节点则为 NULL	*/ 
	BHYAFNnode		*afnNode;
	/* 时延传递关系，只保留 Cell 内部的传递关系	*/ 
	BHYdelayTransmit	*trans;
#endif
} BHYrequiredNode;


#define BHYMaskSegmentFlagUsed		0x01
#define BHYMaskSegmentFlagFixed		0x02
#define BHYMaskSegmentFlagCongest	0x04
#define BHYMaskSegmentFlagInFormerTree	0x08
#define BHYMaskSegmentFlagNotFixed \
	( \
		BHYmaskInt \
		^ BHYMaskSegmentFlagFixed \
	)
#define BHYMaskSegmentFlagNotCongest \
	( \
		BHYmaskInt \
		^ BHYMaskSegmentFlagCongest \
	)
#define BHYMaskSegmentFlagNotInFormerTree \
	( \
		BHYmaskInt \
		^ BHYMaskSegmentFlagInFormerTree \
	)
#define BHYMaskSegmentFlagClear \
	( \
		BHYmaskInt \
		^ BHYMaskSegmentFlagUsed \
		^ BHYMaskSegmentFlagFixed \
		^ BHYMaskSegmentFlagInFormerTree \
	)

/*
The following structure is used to index all
the Segments in the GRG
*/
typedef struct _BHYGRGSegmentIndex
{
	/* see above	*/ 
	int		flag;
	MSegment	*segment;
} BHYGRGSegmentIndex;


/*
FUNCTION :
    Indexes the GRG Segments. You can use glbSegments[ ID_of_Negment ].segment
    to get the Segment pointer of the corresponding Segment.
INITIALIZATION :
    It is defined in BuildGrg.c. It is alloced in BuildGrg.c::BuildGRG.
    All the initial should be done in the BuildGrg.c file.
*/
extern BHYGRGSegmentIndex	*glbSegments;


/*
FUNCTION :
    Check if the MSegment pointer s refers to an Used MSegment.
*/
#define BHYisSegmentUsed( s )	(( (s) != NULL )\
	&&( glbSegments[(s)->id].flag & BHYMaskSegmentFlagUsed ))

/*
FUNCTION :
    Refresh the wieght of a segment, according to its capacity and usage.
*/
#define BHYweightOfSegment( s )\
	((( s )->u_tracks == 0 ) \
	? ( float )(( 0.03125 ) / ( (s)->s_tracks + 0.0625 )) \
	: ( float )(((s)->u_tracks+0.0625 ) / ( (s)->s_tracks+0.0625 )))
#define BHYrefreshWeightOfSegment( s )\
{\
	(s)->weight = BHYweightOfSegment( s );\
}

#define BHYMaskNodeFlagRequired		0x01
#define BHYMaskNodeFlagUsed		0x02
#define BHYMaskNodeFlagMeasured		0x200
#define BHYMaskNodeFlagLoaded		0x400
#define BHYMaskNodeFlagCounted		0x800
#define BHYMaskNodeFlagPart1		0x8000
#define BHYMaskNodeFlagNotUsed	\
	( \
		BHYmaskInt \
		^ BHYMaskNodeFlagUsed \
	)
/*
The following structure is used to index all
the Nodes in the GRG
*/
typedef struct _BHYGRGNodeIndex
{
	/* see above	*/ 
	int	flag;
	/* 节点的几何信息	*/ 
	MNode	*node;
#ifndef BHYwireOnly
	/* 用于更新 Steiner 树延时信息的临时延时信息	*/ 
	float	delayFromSource;
	/* 用于更新 Steiner 树延时信息的临时电容值	*/ 
	float	c;
#endif
} BHYGRGNodeIndex;

/*
FUNCTION :
    Indexes the GRG Nodes. You can use glbNodes[ ID_of_Node ].node
    to get the Node pointer of the corresponding Node.
INITIALIZATION :
    It is defined in BuildGrg.c. It is alloced in BuildGrg.c::BuildGRG.
    All the initial should be done in the BuildGrg.c file.
*/
extern BHYGRGNodeIndex		*glbNodes;


#ifdef BHYdebug
#define BHYcheckNodeID( ID )\
	if(( ID < 0 )||( ID >= BHYNumberOfNodes( rowNum, colNum )))\
	{\
		BHYhalt( "NodeID out of range.", -2 );\
	}\
	if( glbNodes[ID].node == NULL )\
	{\
		BHYhalt( "Invalid NodeID used.", -3 );\
	}
#else
#define BHYcheckNodeID( ID )
#endif


#define BHYMaskArrayStyleTreeEdgeFlagFixed	(1u<<(sizeof(T16Bits)*8-1))
#define BHYMaskArrayStyleTreeEdgeNumber		(BHYMaskArrayStyleTreeEdgeFlagFixed-1)
/*
The following structure is used to hold the a Steiner Tree in Array mode.
*/
typedef struct _BHYArrayStyleTree
{
	/* 通过的边数	*/ 
	int		nEdges;
	/* 总的线长	*/ 
	int		length;
	/* The measure of the priority of this tree in timing-driven.	*/ 
	/*   if the value is large, prevent the tree from being changed	*/ 
/*
	double		priority;
*/
	/* Number of extra bends	*/ 
	int		nExtraBends;
#ifndef BHYwireOnly
	/* 从线网的源到每个漏的延时，必须与线网的需求点顺序一致	*/ 
	float		*delay;
#endif
	/* The next tree of the net */ 
	struct _BHYArrayStyleTree	*next;
	/* 保存树所通过 GRG 边的数组。最高位表示该边是否能够	*/ 
	/* 被替代，其它 15 位表示编号ID				*/ 
	T16Bits		*edges;
} BHYArrayStyleTree;


/*
The following structure is used to represent a net.
*/
typedef struct _BHYNetIndex
{
	/* 布线树的数目					*/ 
	int			nTrees;
	/* 具有多余拐弯的树的个数			*/ 
	int			nSpecialTrees;
	/* 当前布线树指针，也是布线树循环链表的头指针	*/ 
	BHYArrayStyleTree	*curTree;
	/* 到目前为止取得的最佳解对应的布线树		*/ 
	BHYArrayStyleTree	*bestTree;
	/* 到目前为止取得的临时最佳解对应的布线树	*/ 
	BHYArrayStyleTree	*goodTree;
	/* 该线网在重布优先队列中的位置			*/ 
	int			queuePos;
	/* 线网在 GRG 中不同的需求点个数		*/ 
	int			nRequired;
	/* 线网在 GRG 中不同的需求点数组		*/ 
	BHYrequiredNode		*required;
	/* 线网的源					*/ 
	BHYrequiredNode		source;
	/* MNet 结构的指针				*/ 
	MNet			*netPtr;
#ifndef BHYwireOnly
	/* 临时保存最优的时延驱动树以备优化时使用	*/ 
	BHYArrayStyleTree	*timingDrivenTree;
#endif
	int flag;
} BHYNetIndex;

/*
FUNCTION :
    Indexes the GRG Nets. You can use glbNets[ ID_of_Net ].netPtr
    to get the Net pointer of the corresponding Net.
INITIALIZATION :
    It is defined in BuildNet.c. It is alloced in BuildNet.c::BuildNet.
    All the initial should be done in the BuildNet.c file.
*/
extern BHYNetIndex	*glbNets;


/*
The following structure is used to hold the evolution parameter of a GR solution.
*/
typedef struct
{
	/* 水平边最大拥挤度		*/ 
	float	maxH;
	/* 达到最大拥挤度的水平边数	*/ 
	int	nMaxH;
	/* 水平边的平均拥挤度	*/ 
	float	avgH;
	/* 水平边的超容拥挤度和	(-1) */ 
	float	beyondH;
	/* 水平边拥挤度平方和		*/ 
	float	D_H;
	/* 与垂直边对应的参数	*/ 
	float	maxV;
	int	nMaxV;
	float	avgV;
	float	beyondV;
	float	D_V;
	/*各边拥挤度的方差，用来衡量布线是否资源占用均匀*/
	double Sigma;
	/*平均的拥挤度 total beyond average*/
	float avg;
} BHYgrEvolutionParameter;


/*
The following structure is used to hold all the parameters for GR.
*/
typedef struct _BHYGlobalParameters
{
#ifndef BHYwireOnly
	/* 单位线长的电容	*/ 
	float	cc1;
	/* 单位线长的电阻	*/ 
	float	rr1;
	/* QCE 规则参数		*/ 
	float	qceFactor;
	/* Sakurai 模型的参数	*/ 
	float	alfa;
	float	beta;
	/* PERT 节点数		*/ 
	long	nNodes;
	/* PERT 关键节点数	*/ 
	long	nCriticalNodes;
	/* PERT 偏序层数	*/ 
	int	nPartialOrderLevel;
	/* 时延的最小富余量	*/ 
	float	minRedundant;
	/* PERT 的总源和漏	*/ 
	BHYdelayTransmit		*input, *output;
#endif
	/* t : timing-driven   i : timing-info only   w : wire-only	*/ 
	char	mode;
	/* 权衡参数		*/ 
	float	alfaCongestion;
	float	alfaLength;
	float	alfaTwist;
	/* The evolution parameter of the best GR solution	*/ 
	BHYgrEvolutionParameter		best;
} BHYGlobalParameters;


/*
FUNCTION :
    Deliver global parameters to all functions.
INITIALIZATION :
    It should be Defined and Initialized in BhyParameter.c.
*/
extern BHYGlobalParameters bhyPar;


/*
FUNCTION :
    Indexes Backward Direction of a Node. You can use BHYBackDirection[2]
    to get 3, where 2/3 means LEFT/RIGHT.
INITIALIZATION :
    It is Defined and Initialized in bhyfixni.c.
*/
extern int BHYBackDirection[4];


/*
FUNCTION :
    They are the number of columns and rows respectively.
    We assume that the Number of Segments is
	(rowNum-1)*colNum + rowNum*(colNum-1).
    And the Number of Nodes is
	rowNum * colNum.

INITIALIZATION :
    They are defined in BuildGrg.c.
    All the initial should be done in the BuildGrg.c file.
*/
extern long	colNum;
extern long	rowNum;

#define BHYNumberOfSegments( rowNum, colNum )\
	((rowNum-1)*colNum + rowNum*(colNum-1))
#define BHYNumberOfNodes( rowNum, colNum )\
	( rowNum * colNum )



int coordinates2SegmentID(
	int	col1,
	int	row1,
	int	col2,
	int	row2 );










/*
FUNCTION :
    They are the number of nets.
INITIALIZATION :
    They are defined in BuildNet.c.
    All the initial should be done in the BuildNet.c file.
*/


extern int	netNum;

extern MGRG	*grg;
extern MDesign	*designPtr;
extern int	r_number;
extern int	c_number;
extern int	horRLayerNum;
extern int	verRLayerNum;

#endif
