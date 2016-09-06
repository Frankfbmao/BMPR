/*
 * SCGrg.h:
 */

# ifndef SC_GRG_INCLUDED
# define SC_GRG_INCLUDED

/*
 * Grg definition
 */

struct _MGRG
{
  MNode         *head;
  MCell         *cellPtr;  /* ~ */

  // the following members are added by hanbin.
  int			rowNum, colNum;

  MGrc		***	grcMatrix;
  MGrc		**	grcList;

  MNode		*** nodeMatrix;
  MNode		**	nodeList;

  MSegment	***	hSegMatrix;
  MSegment	**	hSegList;
  MSegment	***	vSegMatrix;
  MSegment	**	vSegList;
  MSegment	**	segList;

} ;

/* Node definition, used for building FPG  */
struct _MNode
{
  int           No;       /* No. of node */
  int           rowNo;
  int           colNo;
  long          x;        /* the x coordinate */
  long          y;        /* the y coordinate */
 // unsigned      active;   /* UNVISITED/ACTIVE/VISITED */
  unsigned      status;   /* TOP/BOTTOM/LEFT/RIGHT/NORMAL/MIDDLE */
//  MSegment      *s_linked[4];   /* @LinkSeg segments connected with the node according to up/down,left,right */
//  MNode         *n_linked[4];       /* ~ @LinkNode nodes connected with the  node according to up/dowm,left,right */
//  int           dir[4]; 
  int           prop;     /* 0 - source, 1 - sink, -1 - others */
//  int           vnode;

//  MGrc          *grcPtr;  /* Corresponding GRC */
} ;

/* Segment difinition, used for building GRG  */
struct _MSegment
{
  int           id;       /* the id of segment */
  int           dir;      /* segment dir : HORI,VERT_FEED or VERT_OVER */
  long          length;   /* the length of segment */
  float         weight;   /* weight of segment */
 // int           nopath;   /* path number */
  long          x1;             
  long          y1;
  long          x2;
  long          y2;
  MNode         *s_node[2];       /* @SegNode ~ node of this segment */
//  MGrcEdge      *grcEdgePtr;      /* corresponding GRC edge */
//  int           s_tracks;     /* @SegTrack the number of tracks of this segment */
  float         s_tracks;
//  int		*	s_layerTracks; // seg tracks of each layer.
  							   // added by hanbin for use of layer assignment.	
  int           u_tracks;     /* @UseTrack the number of tracks used */
//  MPassedPath   *passedPathList; /* `LNK indicates which path passed the seg */
} ;

struct _MPassedPath
{
  MPath         *pathPtr;  /* `LNK */
  MPassedPath   *prevPtr;
  MPassedPath   *nextPtr;
} ;

/*** vertex definition, used for building tree ***/
struct _MVertex
{
  int           id;	/* vertex id */
  int           id1;
  int           type;   /* vertex type : PIN_INST */
  int           degree;	/* vertex degree */
  int           visit; 	/* 0 -unvisited, 1 - visited */
  long          x;
  long          y;
  MNode         *nodePtr;	/* ~ GRG node that the vertex belongs to */
  MPath         *p_linked[4];	/* @LinkPath ~ four linked path */
  MVertex       *v_linked[4];	/* @LinkVertex ~ four linked vertex */
} ;




/*
 * GRC (Global Routing Cell) definition 
 */

struct _MGrc
{
  int           id;		/* GRC id */
  int           place;		/* GRC position */
  MGrcEdge      *leftEdge;	/* ~ left edge */
  MGrcEdge      *rightEdge;	/* ~ right edge */
  MGrcEdge      *upEdge;	/* ~ up edge */
  MGrcEdge      *downEdge;	/* ~ down edge */
  MNode         *grgNodePtr;	/* ~ correspoding GRG node */
} ;

/*
 * GRC edge definition
 */

struct _MGrcEdge
{
  int	        id; 		/* GRC edge identification */
  unsigned      edgeSide;	/* edge side : TOP/BOTTOM/LEFT/RIGHT */
  unsigned      edgeType;	/* edge type : HORT/VERT */
//  MGrc          *lbGrcPtr;	/* ~ left or bottom GRC of the edge */
//  MGrc          *rtGrcPtr;	/* ~ right or top GRC of the edge */
//  MCrossPoint   *crossPointList; /* @CrossPoint `LNK cross point list */
} ;

/*
 * Cross point definition 
 */

struct _MCrossPoint
{
  int           id;		/* cross point id */
  int           col;		/* the sub column */
  unsigned      layer;		/* layer number that cross-point in */
  MNet	        *netPtr;	/* ~ net that the point belongs to */
  MGrcEdge      *edgePtr;	/* ~ GRC edge that the point is in */
  MCrossPoint   *nextPtr;  	/* next cross-point in the same edge */
} ;

# endif

