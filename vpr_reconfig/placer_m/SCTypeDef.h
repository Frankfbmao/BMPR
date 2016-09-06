/*
 * SCTypeDef.h
 */
# ifndef SC_TYPEDEF_INCLUDED
# define SC_TYPEDEF_INCLUDED

/* SCCell.h */
typedef struct _MCell MCell;
typedef struct _MBlockage MBlockage;
typedef struct _MIBlockage MIBlockage;
typedef struct _MCellInst MCellInst;

/* SCCluster.h */
typedef struct _MCluster MCluster;
typedef struct _MTerm MTerm;

/* SCConx.h */
typedef struct _MConstr MConstr;
typedef struct _MLRule MLRule;
typedef struct _MRClass MRClass;
typedef struct _MRegion MRegion;
typedef struct _MSGroup MSGroup;
typedef struct _MHGroup MHGroup;
typedef struct _MPart MPart;
typedef struct _MFloor MFloor;
typedef struct _MPad MPad;
typedef struct _MPgr MPgr;
typedef struct _MPPad MPPad;
typedef struct _MGPad MGPad;
typedef struct _MTiming MTiming;
typedef struct _MIgnore MIgnore;
typedef struct _MSigPath MSigPath;
typedef struct _MSigNet MSigNet;
typedef struct _MNetMatching MNetMatching;
typedef struct _MPower MPower;

/* SCDesign.h */
typedef struct _MDesign MDesign;
typedef struct _MLib MLib;
typedef struct _MCtrl MCtrl;
typedef struct _MSite MSite;
typedef struct _MSiteList MSiteList;
typedef struct _MTrack MTrack;

/* SCDetail.h */
typedef struct _MPGRouting MPGRouting;
typedef struct _MPGNode MPGNode;
typedef struct _MPGAStrap MPGAStrap;
typedef struct _MPGMesh MPGMesh;
typedef struct _MMeshNode MMeshNode;
typedef struct _MMeshLink MMeshLink;

/* SCGeom.h */
typedef struct _MPoint MPoint;
typedef struct _MSize MSize;
typedef struct _MBox MBox;
typedef struct _MPolygon MPolygon;
typedef struct _MPathLine MPathLine;
typedef struct _MLineseg MLineseg;
typedef struct _MCircle MCircle;
typedef struct _MArc MArc;
typedef struct _MText MText;
typedef struct _MConnect MConnect;

/* SCGrg.h */
typedef struct _MGRG MGRG;
typedef struct _MNode MNode;
typedef struct _MSegment MSegment;
typedef struct _MPassedPath MPassedPath;
typedef struct _MVertex MVertex;
typedef struct _MPath MPath;
typedef struct _MGrc MGrc;
typedef struct _MGrcEdge MGrcEdge;
typedef struct _MCrossPoint MCrossPoint;

/* SCNet.h */
typedef struct _MNet MNet;
typedef struct _MGrcRoute MGrcRoute;
typedef struct _MRoute MRoute;
typedef struct _MSubTreeNode MSubTreeNode;
typedef struct _MSeg MSeg;
typedef struct _MSubVertex MSubVertex;
typedef struct _MVia MVia;

/* SCPin.h */
typedef struct _MPin MPin;
typedef struct _MPinInst MPinInst;
typedef struct _MRPin MRPin;

/* SCPort.h */
typedef struct _MPort MPort;
typedef struct _MPortInst MPortInst;
typedef struct _MLinkedPort MLinkedPort;
typedef struct _MLinkedPortInst MLinkedPortInst;
typedef struct _MPortDelay MPortDelay;

/* SCQue.h */
typedef struct que SC_QUE;
typedef struct queNode SCQueNode;

/* SCTech.h */
typedef struct _MTech MTech;
typedef struct _MLayerDef MLayerDef;
typedef struct _MRLayer MRLayer;
typedef struct _MSRule MSRule;
typedef struct _MColorDef MColorDef;
typedef struct _MViaDef MViaDef;

/* other type defintions */
typedef long             MId;
typedef char *           MString;
typedef unsigned char    uchar;
typedef char *           MClientData;
typedef int              MLayer;
typedef int              MStatus;
typedef int              MCellType;
typedef int              BOOL;
typedef void *SCPointer;
typedef void *SCClientData;
typedef int (*SCTopOpr)();

# endif /* SC_TYPEDEF_INCLUDED */
