
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
��������ṹ -- �ڽӱ���ʽ
*/

/* ������	*/typedef struct _BHYAFNedge
{
	/* б��		*/ 
	float	slope;
	/* �ٵ�ָ��	*/ 
	struct _BHYAFNnode	*adjacent;
	/* ��һ��	*/ 
	struct _BHYAFNedge	*next;
	/* ����ߡ�Added by Bao Hai-yun on 1998.10.13	*/ 
#ifdef BHYaccuratePERT
	struct _BHYAFNedge	*backEdge;
#endif
} BHYAFNedge;


/* ����������ȶ�����		*/ 
#define BHYMaskAFNnodeFlagVisited		0x01
#define BHYMaskAFNnodeFlagNotVisited \
	( \
		BHYmaskInt \
		^BHYMaskAFNnodeFlagVisited \
	)

/* �ڵ�����	*/ 
typedef struct _BHYAFNnode
{
	/* ��Ӧ�����ָ��	*/ 
	struct _BHYrequiredNode *rn;
	/* ��һ����������	*/ 
	struct _BHYAFNedge	*edge;
	/* ��һ�ڵ�		*/ 
	struct _BHYAFNnode	*next;
	/* ��һ������������ߣ�Added by Bao Hai-yun on 1998.10.13	*/ 
#ifdef BHYaccuratePERT
	struct _BHYAFNedge	*backEdge;
#endif
	int			flag;
} BHYAFNnode;


/*
���ڱ�ʾ��Ԫ�ڲ�ʱ�Ӵ��ݹ�ϵ�ı�����
*/
typedef struct _BHYdelayTransmit
{
	/* ��Ӧ�������ָ��	*/ 
	struct _BHYrequiredNode		*rn;
	/* ��Ԫ�ڲ���ʱ		*/ 
	float				delay;
	/* ��һ�ڵ�		*/ 
	struct	_BHYdelayTransmit	*next;
} BHYdelayTransmit;
#endif

#define BHYdelayTransmitPush( node, head )\
	(node)->next = head; \
	head = node;


/* ����������ȶ�����		*/ 
#define BHYMaskRequiredNodeFlagInQueue		0x002
/* �������ʱ�ӹ�ϵ�������	*/ 
#define BHYMaskRequiredNodeFlagBegin		0x004
/* �������ʱ�ӹ�ϵ�����յ�	*/ 
#define BHYMaskRequiredNodeFlagEnd		0x008
/* �������ʱ�ӹ�ϵ�����м��	*/ 
#define BHYMaskRequiredNodeFlagMid		0x010
/* ����㲻�����ȶ�����		*/ 
#define BHYMaskRequiredNodeFlagNotInQueue \
	( \
		BHYmaskInt \
		^ BHYMaskRequiredNodeFlagInQueue \
	)
/* �����δȷ��ʱ�ӹ�ϵ		*/ 
#define BHYMaskRequiredNodeFlagNotSet \
	( \
		BHYmaskInt \
		^ BHYMaskRequiredNodeFlagBefgin \
		^ BHYMaskRequiredNodeFlagEnd \
		^ BHYMaskRequiredNodeFlagMid \
	)

/* �����ṹ	*/
typedef struct _BHYrequiredNode
{
	/* The ID of the node	*/ 
	int			nodeID;
#ifndef BHYwireOnly
	/* ��Ψһȷ�������� ID���������ź͸ýڵ��������е���ž����� */ 
	/* ������	*/ 
	int			netId;
	/* �ڵ�˳��ţ�����ýڵ���Դ��Ϊ -1	*/ 
	int			nodeSeq;
	/* ƫ���ϵ��	*/ 
	int			partialOrder;
	/* The flag used in the process of delay transmission */ 
	int			flag;
	/* The electric capacity of the node. 	*/ 
	float			cIn;
	/* Required delay of the node	*/ 
	float			requiredDelay;
	/* Current delay of the node	*/ 
	float			currentDelay;
	/* ��Ӧ�����������еĽڵ�ָ�룬������������û�иýڵ���Ϊ NULL	*/ 
	BHYAFNnode		*afnNode;
	/* ʱ�Ӵ��ݹ�ϵ��ֻ���� Cell �ڲ��Ĵ��ݹ�ϵ	*/ 
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
	/* �ڵ�ļ�����Ϣ	*/ 
	MNode	*node;
#ifndef BHYwireOnly
	/* ���ڸ��� Steiner ����ʱ��Ϣ����ʱ��ʱ��Ϣ	*/ 
	float	delayFromSource;
	/* ���ڸ��� Steiner ����ʱ��Ϣ����ʱ����ֵ	*/ 
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
	/* ͨ���ı���	*/ 
	int		nEdges;
	/* �ܵ��߳�	*/ 
	int		length;
	/* The measure of the priority of this tree in timing-driven.	*/ 
	/*   if the value is large, prevent the tree from being changed	*/ 
/*
	double		priority;
*/
	/* Number of extra bends	*/ 
	int		nExtraBends;
#ifndef BHYwireOnly
	/* ��������Դ��ÿ��©����ʱ�������������������˳��һ��	*/ 
	float		*delay;
#endif
	/* The next tree of the net */ 
	struct _BHYArrayStyleTree	*next;
	/* ��������ͨ�� GRG �ߵ����顣���λ��ʾ�ñ��Ƿ��ܹ�	*/ 
	/* ����������� 15 λ��ʾ���ID				*/ 
	T16Bits		*edges;
} BHYArrayStyleTree;


/*
The following structure is used to represent a net.
*/
typedef struct _BHYNetIndex
{
	/* ����������Ŀ					*/ 
	int			nTrees;
	/* ���ж����������ĸ���			*/ 
	int			nSpecialTrees;
	/* ��ǰ������ָ�룬Ҳ�ǲ�����ѭ�������ͷָ��	*/ 
	BHYArrayStyleTree	*curTree;
	/* ��ĿǰΪֹȡ�õ���ѽ��Ӧ�Ĳ�����		*/ 
	BHYArrayStyleTree	*bestTree;
	/* ��ĿǰΪֹȡ�õ���ʱ��ѽ��Ӧ�Ĳ�����	*/ 
	BHYArrayStyleTree	*goodTree;
	/* ���������ز����ȶ����е�λ��			*/ 
	int			queuePos;
	/* ������ GRG �в�ͬ����������		*/ 
	int			nRequired;
	/* ������ GRG �в�ͬ�����������		*/ 
	BHYrequiredNode		*required;
	/* ������Դ					*/ 
	BHYrequiredNode		source;
	/* MNet �ṹ��ָ��				*/ 
	MNet			*netPtr;
#ifndef BHYwireOnly
	/* ��ʱ�������ŵ�ʱ���������Ա��Ż�ʱʹ��	*/ 
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
	/* ˮƽ�����ӵ����		*/ 
	float	maxH;
	/* �ﵽ���ӵ���ȵ�ˮƽ����	*/ 
	int	nMaxH;
	/* ˮƽ�ߵ�ƽ��ӵ����	*/ 
	float	avgH;
	/* ˮƽ�ߵĳ���ӵ���Ⱥ�	(-1) */ 
	float	beyondH;
	/* ˮƽ��ӵ����ƽ����		*/ 
	float	D_H;
	/* �봹ֱ�߶�Ӧ�Ĳ���	*/ 
	float	maxV;
	int	nMaxV;
	float	avgV;
	float	beyondV;
	float	D_V;
	/*����ӵ���ȵķ���������������Ƿ���Դռ�þ���*/
	double Sigma;
	/*ƽ����ӵ���� total beyond average*/
	float avg;
} BHYgrEvolutionParameter;


/*
The following structure is used to hold all the parameters for GR.
*/
typedef struct _BHYGlobalParameters
{
#ifndef BHYwireOnly
	/* ��λ�߳��ĵ���	*/ 
	float	cc1;
	/* ��λ�߳��ĵ���	*/ 
	float	rr1;
	/* QCE �������		*/ 
	float	qceFactor;
	/* Sakurai ģ�͵Ĳ���	*/ 
	float	alfa;
	float	beta;
	/* PERT �ڵ���		*/ 
	long	nNodes;
	/* PERT �ؼ��ڵ���	*/ 
	long	nCriticalNodes;
	/* PERT ƫ�����	*/ 
	int	nPartialOrderLevel;
	/* ʱ�ӵ���С������	*/ 
	float	minRedundant;
	/* PERT ����Դ��©	*/ 
	BHYdelayTransmit		*input, *output;
#endif
	/* t : timing-driven   i : timing-info only   w : wire-only	*/ 
	char	mode;
	/* Ȩ�����		*/ 
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
