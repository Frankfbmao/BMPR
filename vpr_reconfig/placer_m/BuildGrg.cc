
#define BHYdebugBuildGrgSegmentID	0x1
#define BHYdebugBuildGrgCheckNode	0x2
#define BHYdebugBuildGrgUnexpected	0x4


#include<iostream> 
#include <string.h>
using namespace std;


#include "sttools.h"
#include "scgroute.h"
#include "SCglobals.h"
#include "SCMisc.h"

#if( BHYdebugBuildGrg != 0 )&& !defined( BHYdebug )
#define BHYdebug
#endif


BHYGRGSegmentIndex	*glbSegments = NULL;
BHYGRGNodeIndex		*glbNodes = NULL;
long			colNum = 0;
long			rowNum = 0;


/* defined only for baohy	*/

#define VERT_FEED 1
#define VERT_CHAN 4





/*
FUNCTION :
	Translate (col1, row1)--(col2, row2) to segment's id.
	Assume (col1, row1)--(col2, row2) is valid, and colNum
	is available.
CREATED by Bao Hai-yun
*/

int coordinates2SegmentID(
	int	col1,
	int	row1,
	int	col2,
	int	row2 )
{

	if(( row1 != row2 )&&( col1 != col2 ))
		printf( "The Segment is not horizontal or vertical\n");
	if(( rowNum == 0 )||( colNum == 0 ))
		printf( "rowNum or colNum has not been initialized\n");
	if(( col1 < 1 )||( col1 > colNum )||( col2 < 1 )||( col2 > colNum ))
		printf( "col ID of Segment out of range\n" );
	if(( row1 < 0 )||( row1 >= rowNum )||( row2 < 0 )||( row2 >= rowNum ))
		printf( "row ID of Segment out of range\n" );


	if( row1 == row2 )
		return row1*(colNum-1) + min( col1, col2 ) - 1;
	else
		return rowNum*(colNum-1)
			+ (col1-1)*( rowNum-1 ) + min( row1, row2 );
}






void lshBuildGRG(){

	long		rowNo, colNo;
	int   i, j;
	grg = designPtr->cellPtr->grg;	
	grg->head = designPtr->cellPtr->grg->nodeList[0];		
	rowNum = designPtr->cellPtr->grg->rowNum;	
	colNum = designPtr->cellPtr->grg->colNum;
	glbSegments = balloc( BHYGRGSegmentIndex,
				BHYNumberOfSegments( rowNum, colNum ));
					
	glbNodes = balloc(
			BHYGRGNodeIndex,
			BHYNumberOfNodes( rowNum, colNum ));
	for(  i=0;  i < BHYNumberOfNodes(rowNum,colNum);  i++ )
	{
		glbNodes[i].flag = 0;
#ifndef BHYwireOnly
		glbNodes[i].delayFromSource = 0.0;
		glbNodes[i].c = 0.0;
#endif
#if BHYdebugBuildGrg & BHYdebugBuildGrgCheckNode
		glbNodes[i].node = NULL;
#endif
		#if 1
		if ((i+1)!=designPtr->cellPtr->grg->nodeList[i]->No){
			printf("No not equals index\n");
			exit(1);
		}
		#endif
		glbNodes[ i ].node = designPtr->cellPtr->grg->nodeList[i];
		glbNodes[i].node->colNo++;
		glbNodes[i].node->No--;
		glbNodes[i].node->status = 4;
		rowNo = glbNodes[i].node->rowNo;
		colNo = glbNodes[i].node->colNo;
		if (rowNo == 0)
		{
			glbNodes[i].node->status = 1;
			/*BOTEDGE = node->y;*/
		} else if (rowNo == rowNum - 1) {
			glbNodes[i].node->status = 0;
			/*TOPEDGE = node->y;*/
		}

		if( colNo == 1 )
		{
			glbNodes[i].node->status = 2;
			/*LEFTEDGE = node->x;*/
		} else if( colNo == colNum ){
			glbNodes[i].node->status = 3;
			/*RIGHTEDGE = node->x;*/
		}
	
	}
	for(  i=0;  i<BHYNumberOfSegments(rowNum,colNum);  i++ )
	{
		glbSegments[i].flag = 0;
#if BHYdebugBuildGrg & BHYdebugBuildGrgSegmentID
		glbSegments[i].segment = NULL;
#endif
		#if 1
		if(i != designPtr->cellPtr->grg->segList[i]->id){
			printf("id not equals index\n");
			exit(1);
		}
		#endif
		glbSegments[i].segment = designPtr->cellPtr->grg->segList[i];
		
	    
	}
	return;

}


/*
FUNCTION:
	Create GRG by reading XXXX.r file.
PARAMETERS:
	Name XXXX is the circuit name.
*/








void
BuildGRG(
	char	*name )

{
	
}

