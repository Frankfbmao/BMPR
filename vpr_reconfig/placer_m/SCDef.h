

/*
 * SCDef.h
 */
 
# ifndef SC_DEF_INCLUDED
# define SC_DEF_INCLUDED

/* some definitions for design */
# define TRUE		 1
# define FALSE		 0
# define M_MARKED	 1
# define M_UNMARKED	 0
# define NAMELENGTH      100
# define MAXNET	 	 2000000
# define CHIPID		 100000
# define MAXPORT         16
# define MAXPIN          16
# define MAXCONTACT      30000
# define MAXECONNECT     5000
# define MAXICONNECT     2500
# define PADNUM          500 /* 1/4 pad total number */
# define ROW_NUMBER      50
# define COL_NUMBER      50
# define MAX_NODE_NUM    50
# define MAXLAYER        64
# ifdef HUGET
# undef HUGET       
# endif
# define HUGET            100000000

/* some definitions for SCSteiner.h */
# define LIMITED         15
# define NETSIZE         2500
# define NAMESIZE        20
# define CIRCUITSIZE     10000
# define MUSTJOINNETSIZE 1000
# define COUNT           100000
# define MAXLENGTH       100000
# define SOURCE          0
# define SINK            1
# define SPOINT          2
# define SPECIAL         3
# define CRITICAL        4
# define ALL             5
# define OTHERS          -1
# define YES             1
# define NO              -1
# define M_TRUE          1
# define M_FALSE         0
# define VERT            1

# define NORTH           0
# define SOUTH           1
# define WEST            2
# define EAST            3
# define NORTH_WEST      4
# define SOUTH_WEST      5
# define NORTH_EAST      6
# define SOUTH_EAST      7
# define CENTER          8
# define FLIP_SOUTH      9
# define FLIP_NORTH      10
# define FLIP_EAST       11
# define FLIP_WEST       12

# define Alpha           1.02
# define Beta            2.21
# define r1              0.00002 /* kOhm/micron: wire resistance 
				    per unit length */
# define c1              0.0002  /* pf: wire capacitance per unit length */

# define M_Malloc(Type)  ((Type *) malloc(sizeof(Type)))
# define M_Free(pointer) {(void) free((char *) pointer);}
# ifdef NIL
# undef NIL
# endif
# define NIL(Type)       (Type *) 0
# define M_ABS(A)        ((A)<0?-(A):(A))
# define M_MAX(A,B)      ((A)>(B)? (A):(B))
# define M_MIN(A,B)      ((A)<(B)? (A):(B))

# define SEARCH_CONTINUED  0
# define SEARCH_COMPLETED  1
# define SEARCH_STOPPED    2
# define OK                3
# define ERROR             -1
# ifdef ABS
# undef ABS
# endif
# define ABS(A)            ((A)<0? -(A):(A))
# ifdef MIN
# undef MIN
# endif
# ifdef MAX
# undef MAX
# endif
# define MIN(A,B)          ((A)<(B)? (A):(B))
# define MAX(A,B)          ((A)<(B)? (B):(A))
# define SCFree(a)         free(a)
# ifdef NONE
# undef NONE
# endif
# define NONE              "           "
       
#endif	/* SC_DEF_INCLUDED */
