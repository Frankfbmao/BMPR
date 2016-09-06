/*
 * SCCell.h: Cell Definition
 */

#ifndef SC_CELL_INCLUDED
#define SC_CELL_INCLUDED

/*
 * cell master definition, including compositive cell and library cell
 */

struct _MCell
{
//  char          *name;		/* @Name cell name */
 // long          area;           /* @Area esmated area for soft block */
//  float         aspRatio;       /* @Ratio expected chip or block ratio */
   int           id;		/* @Id the id of the cell */
  MGRG          *grg;		/* point to floor plan graph */
} ;



#endif	/* SC_CELL_INCLUDED */

