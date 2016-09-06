/*
 * SCMisc.h:
 */


# ifndef SC_MISC_INCLUDED
# define SC_MISC_INCLUDED

/* some global parameters get from know information */

/* some global parameters */

MDesign *designPtr;     /* current design pointer */
char    *designName;    /* current design name */
int     cellNum;        /* total cellInst number */
int     usedCellNum;    /* valid cellInst number */
int     netnum;         /* number of nets */
int     mustJoinNetNum; /* number of must_join ports */
int     powerNetNumber; /* number of power nets */
int     groundNetNumber;/* number of ground nets */
char    *powerNetName[10]; /* power net name */
char    *groundNetName[10];/* ground net name */
int     clockNetNumber; /* number of clock nets */
int     clockBufferNum; /* number of buffer added for clokc tree */
int     rpadNum;        /* number of right pad */
int     lpadNum;        /* number of left pad */
int     bpadNum;        /* number of bottom pad */
int     tpadNum;        /* number of top pad */
int     inputNum;       /* number of input signals */
int     outputNum;      /* number of output signals */
long    cell_h;         /* height of the cell */
long    chip_w; 	       /* width of chip */
long    chip_h; 	       /* height of chip */
long    IOpad_w;        /* width of IO pads */
long    IOpad_h;        /* height of IO pads */
long    IOpad_s;        /* space of IO pads */
long    PG_Size;        /* inner P/G wire width */
long    PGpad_w;        /* P/G pad width */
long    PGpad_h;        /* P/G pad heigth */
long    feed_w; 	       /* width of feedthru */
long    link_w;         /* width of link */
long    grid_w;         /* horizontal width of grid = l2_w + l2_s */
long    grid_h;         /* vertical width of grid = l1_w + l1_s */
long    hori_w;         /* horizontal wire width */
long    padxleft;       /* left_most coordinate of all pads */
long    padxright;      /* right_most coordinate of all pads */
long    padytop;        /* top_most coordinate of all pads */
long    padybot;        /* bottom_most coordinate of all pads */
long    cellxleft;      /* left_most coordinate of all cellInsts */
long    cellxright;     /* right_most coordinate of all cellInsts */
long    cellytop;       /* top_most coordinate of all cellInsts */
long    cellybot;       /* bottom_most coordinate of all cellInsts */

/* global parameters generated during P&R */
int     padsys; 	       /* pad definition system */
			       /* 0 - T(0),B(3),L(6),R(7) */
                               /* 1 - B(0),T(3),R(6),L(7) */ 
			       /* 2 - L(0),R(3),B(6),T(7) */
                               /* 3 - R(0),L(3),T(6),B(7) */
int     numDchan;       /* number of down channel */
int     numUchan;       /* number of up channel */
int     numLchan;       /* number of left channel */
int     numRchan;       /* number of right channel */
int     chipLevel;      /* 1 -- chip level, 0 -- block level */
float   chipRatio;      /* chip width/height ratio */
float   max_delay;      /* maximum delay of chip after P&R */

/* used in queue operations */
int     scNameListSize;
char    **scNameList;
MCell   *chipPtr;

/*** for ReadTree and Generator ***/
MGRG    *grg;
MNode   *currNode;
long    Xdeviat;   /* deviation of x_coordinate between the origin point
                             of cellInst and grid line */
long    Ydeviat;   /* deviation of y_coordinate between the origin point
                             of cellInst and grid line */
long    channel_h; /* height of the center channel */
int     chanDen;   /* maximum density requirement of channels */
long    r_space;   /* space between two adjacent rows in GRG*/
long    c_space;	  /* space between adjacent rows in geometry */
int     r_number;  /* cell row number */
int     c_number;  /* column number of GRG */
int     horRLayerNum;     /* number of horizontal routing layer */
int     verRLayerNum;     /* number of vertical routing layer */
int     mid_no; 	  /* begin middle vertex number in GRG */
                          /* mid_no = c_number * (r_number + 1) + 2 */
long    Xoffset;   /* cellxleft - real left_most coordinate */
                          /* of cellInst in placement result */
long    Xleft; 	  /* left_most coordinate of GRG */
long    Xright; 	  /* right_most coordinate of GRG */
long    Ybottom;   /* bottom_most coordinate of GRG */
long    Ytop;      /* top_most coordinate of GRG */
long    Ysecond;   /* y_coordinate of the second line from bo GRG */

/* for CFD and IDW for timing-driven Steiner tree generation */
float   Rtr, Cin;  /* drive resistance and loading capacity */
long    Dist1;
long    Wire;      /* total wire length */
long    Dist;      /* distance between source and sink */
float   Coef1;     /* coefficient used in Elmore delay calculation */
float   Coef2;     /* optimize(Coef1*W + Coef2*Dist) */

/************ used in cross point assignment *********/
long    Xmin;	  /* left x_coordinate of GRG area */
long    Ymin;	  /* bottom y_coordinate of GRG area */
long    Xmax;	  /* right x_coordinate of GRG area */
long    Ymax;	  /* top y_coordinate of GRG area */
/*
# ifndef   SC_981201
char   *SCSep = " ,\t\\";
char   *SCEnd = ";:\n";
# else  //SC_981201
# endif //SC_981201
*/

# endif SC_MISC_INCLUDED
