/*
 * SCDesign.h 
 */

#ifndef SC_DESIGN_INCLUDED
#define SC_DESIGN_INCLUDED

#include "SCTypeDef.h"
/*
 * design definition, it can be a chip or macro cell
 */
struct _MDesign /* MDesign */  
{ 
 MCell          *cellPtr;   /* designed chip or macro cell */
 long           *xMapList;   /* @XMap */
 long           *yMapList;   /* @YMap */
// float           *xMapList;   /* @XMap */
// float           *yMapList;   /* @YMap */
 int            netNum;     /* number of nets */
 int            cellNum;    /* number of children in chip or macro cell */   
} ; 



#endif	/* SC_DESIGN_INCLUDED */

