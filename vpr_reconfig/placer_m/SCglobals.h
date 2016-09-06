/*
 * SCglobals.h:
 */

# ifndef SCGLOBALS_H_
# define SCGLOBALS_H_


#include "SCDesign.h"
#include "SCTypeDef.h"



/* some global parameters get from know information */

/* some global parameters */
extern MDesign *designPtr;     /* current design pointer */
extern char    *designName;    /* current design name */
extern int     cellNum;        /* total cellInst number */
extern int     usedCellNum;    /* valid cellInst number */
extern int     netnum;         /* number of nets */
extern int     mustJoinNetNum; /* number of must_join ports */
extern int     powerNetNumber; /* number of power nets */
extern int     groundNetNumber;/* number of ground nets */
extern char    *powerNetName[10]; /* power net name */
extern char    *groundNetName[10];/* ground net name */
extern int     clockNetNumber; /* number of clock nets */
extern int     clockBufferNum; /* number of buffer added for clokc tree */
extern int     rpadNum;        /* number of right pad */
extern int     lpadNum;        /* number of left pad */
extern int     bpadNum;        /* number of bottom pad */
extern int     tpadNum;        /* number of top pad */
extern int     inputNum;       /* number of input signals */
extern int     outputNum;      /* number of output signals */
extern long    cell_h;         /* height of the cell */
extern long    chip_w; 	       /* width of chip */
extern long    chip_h; 	       /* height of chip */
extern long    IOpad_w;        /* width of IO pads */
extern long    IOpad_h;        /* height of IO pads */
extern long    IOpad_s;        /* space of IO pads */
extern long    PG_Size;        /* inner P/G wire width */
extern long    PGpad_w;        /* P/G pad width */
extern long    PGpad_h;        /* P/G pad heigth */
extern long    feed_w; 	       /* width of feedthru */
extern long    link_w;         /* width of link */
extern long    grid_w;         /* horizontal width of grid = l2_w + l2_s */
extern long    grid_h;         /* vertical width of grid = l1_w + l1_s */
extern long    hori_w;         /* horizontal wire width */
extern long    padxleft;       /* left_most coordinate of all pads */
extern long    padxright;      /* right_most coordinate of all pads */
extern long    padytop;        /* top_most coordinate of all pads */
extern long    padybot;        /* bottom_most coordinate of all pads */
extern long    cellxleft;      /* left_most coordinate of all cellInsts */
extern long    cellxright;     /* right_most coordinate of all cellInsts */
extern long    cellytop;       /* top_most coordinate of all cellInsts */
extern long    cellybot;       /* bottom_most coordinate of all cellInsts */

/* global parameters generated during P&R */
extern int     padsys; 	       /* pad definition system */
			       /* 0 - T(0),B(3),L(6),R(7) */
                               /* 1 - B(0),T(3),R(6),L(7) */ 
			       /* 2 - L(0),R(3),B(6),T(7) */
                               /* 3 - R(0),L(3),T(6),B(7) */
extern int     numDchan;       /* number of down channel */
extern int     numUchan;       /* number of up channel */
extern int     numLchan;       /* number of left channel */
extern int     numRchan;       /* number of right channel */
extern int     chipLevel;      /* 1 -- chip level, 0 -- block level */
extern float   chipRatio;      /* chip width/height ratio */
extern float   max_delay;      /* maximum delay of chip after P&R */

/* used in queue operations */
extern int     scNameListSize;
extern char    **scNameList;
extern MCell   *chipPtr;

/*** for ReadTree and Generator ***/
extern MGRG    *grg;
extern MNode   *currNode;
extern long    Xdeviat;   /* deviation of x_coordinate between the origin point
                             of cellInst and grid line */
extern long    Ydeviat;   /* deviation of y_coordinate between the origin point
                             of cellInst and grid line */
extern long    channel_h; /* height of the center channel */
extern int     chanDen;   /* maximum density requirement of channels */
extern long    r_space;   /* space between two adjacent rows in GRG*/
extern long    c_space;	  /* space between adjacent rows in geometry */
extern int     r_number;  /* cell row number */
extern int     c_number;  /* column number of GRG */
extern int     horRLayerNum;     /* number of horizontal routing layer */
extern int     verRLayerNum;     /* number of vertical routing layer */
extern int     mid_no; 	  /* begin middle vertex number in GRG */
                          /* mid_no = c_number * (r_number + 1) + 2 */
extern long    Xoffset;   /* cellxleft - real left_most coordinate */
                          /* of cellInst in placement result */
extern long    Xleft; 	  /* left_most coordinate of GRG */
extern long    Xright; 	  /* right_most coordinate of GRG */
extern long    Ybottom;   /* bottom_most coordinate of GRG */
extern long    Ytop;      /* top_most coordinate of GRG */
extern long    Ysecond;   /* y_coordinate of the second line from bo GRG */

/* for CFD and IDW for timing-driven Steiner tree generation */
extern float   Rtr, Cin;  /* drive resistance and loading capacity */
extern long    Dist1;
extern long    Wire;      /* total wire length */
extern long    Dist;      /* distance between source and sink */
extern float   Coef1;     /* coefficient used in Elmore delay calculation */
extern float   Coef2;     /* optimize(Coef1*W + Coef2*Dist) */

/************ used in cross point assignment *********/
extern long    Xmin;	  /* left x_coordinate of GRG area */
extern long    Ymin;	  /* bottom y_coordinate of GRG area */
extern long    Xmax;	  /* right x_coordinate of GRG area */
extern long    Ymax;	  /* top y_coordinate of GRG area */



# endif
