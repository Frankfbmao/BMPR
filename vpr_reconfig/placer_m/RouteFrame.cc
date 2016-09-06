/*
 * File: RouteFrame.cpp
 * Use:
 */
#include <cstdlib>
# include "SCinclude.h"

#include "fplan.h"

#include  "SCGrg.h"
#include "SCTypeDef.h"
#include "SCCell.h"
#include "RouteFrame.h"

#include "SCglobals.h"

using namespace std;



#define HORI 0//liush add
#define VERT_CHAN 4//liush add

# define MET_HORI        0
# define MET_VERT        1
# define MGRCET_LEFT     0
# define MGRCET_RIGHT    1
# define MGRCET_TOP      2
# define MGRCET_BOTTOM   3  




void  mfbRENewMain()
{
	

	int	m_rowNum = r_number;
	int	m_colNum = c_number;

	int	m_layerNum = 8;


	designPtr->cellPtr->grg = new MGRG;
	designPtr->cellPtr->grg->rowNum = m_rowNum;
	designPtr->cellPtr->grg->colNum = m_colNum;


	designPtr->cellPtr->grg->nodeList = (MNode **) malloc( m_rowNum*m_colNum * sizeof(MNode *) );
	designPtr->cellPtr->grg->nodeMatrix = (MNode ***) malloc( m_rowNum * sizeof(MNode **) );

	for( int row = 0; row < m_rowNum; row++ )
	{
		designPtr->cellPtr->grg->nodeMatrix[row] = (MNode **) malloc( m_colNum * sizeof(MNode *) );
		for( int col = 0; col < m_colNum; col++ )
		{

			designPtr->cellPtr->grg->nodeMatrix[row][col] = (MNode *) malloc(sizeof(MNode));
			if (designPtr->cellPtr->grg->nodeMatrix[row][col] == NULL)
			{
				printf("Memory overflow");
				exit(-1);
			}
			designPtr->cellPtr->grg->nodeMatrix[row][col]->No = 0;
			designPtr->cellPtr->grg->nodeMatrix[row][col]->rowNo = 0;
			designPtr->cellPtr->grg->nodeMatrix[row][col]->colNo = 0;
			designPtr->cellPtr->grg->nodeMatrix[row][col]->x = 0;
			designPtr->cellPtr->grg->nodeMatrix[row][col]->y = 0;
			designPtr->cellPtr->grg->nodeMatrix[row][col]->status = 0;

			designPtr->cellPtr->grg->nodeList[row*m_colNum + col] = designPtr->cellPtr->grg->nodeMatrix[row][col];
		}
	}

	designPtr->cellPtr->grg->hSegList = (MSegment **)malloc( m_rowNum*(m_colNum - 1) * sizeof(MSegment *) );
	designPtr->cellPtr->grg->hSegMatrix = (MSegment ***) malloc( m_rowNum * sizeof(MSegment **) );

	for( int row = 0; row < m_rowNum; row++ )
	{
	    designPtr->cellPtr->grg->hSegMatrix[row] = (MSegment **)malloc( (m_colNum - 1) * sizeof(MSegment *) );
		for( int col = 0; col < m_colNum - 1; col++ )
		{

			designPtr->cellPtr->grg->hSegMatrix[row][col] =  (MSegment *) malloc(sizeof(MSegment));
			if (designPtr->cellPtr->grg->hSegMatrix[row][col] == NULL)
			{
				printf("Memory overflow");
				exit(-1);
			}
			designPtr->cellPtr->grg->hSegMatrix[row][col]->id = 0;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->dir = 0;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->length = 0;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->weight = 0.0;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->x1 = 0;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->y1 = 0;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->x2 = 0;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->y2 = 0;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->s_node[0] = NULL;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->s_node[1] = NULL;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->s_tracks = 0;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->u_tracks = 0;

			designPtr->cellPtr->grg->hSegList[row*(m_colNum-1) + col] = designPtr->cellPtr->grg->hSegMatrix[row][col];
		}
	}

	designPtr->cellPtr->grg->vSegList = (MSegment **)malloc( (m_rowNum - 1)*m_colNum * sizeof(MSegment *) );
	designPtr->cellPtr->grg->vSegMatrix = (MSegment ***)malloc( (m_rowNum - 1) * sizeof(MSegment **) );

	for( int row = 0; row < m_rowNum - 1; row++ )
	{
		designPtr->cellPtr->grg->vSegMatrix[row] = (MSegment **)malloc( m_colNum * sizeof(MSegment *) );
		for( int col = 0; col < m_colNum; col++ )
		{

			designPtr->cellPtr->grg->vSegMatrix[row][col] =  (MSegment *) malloc(sizeof(MSegment));
			if (designPtr->cellPtr->grg->vSegMatrix[row][col] == NULL)
			{
				printf("Memory overflow");
				exit(-1);
			}
			designPtr->cellPtr->grg->vSegMatrix[row][col]->id = 0;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->dir = 0;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->length = 0;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->weight = 0.0;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->x1 = 0;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->y1 = 0;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->x2 = 0;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->y2 = 0;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->s_node[0] = NULL;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->s_node[1] = NULL;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->s_tracks = 0;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->u_tracks = 0;


			// Note here: we place vertical segments col by col, not row by row.
			designPtr->cellPtr->grg->vSegList[col*(m_rowNum-1) + row] = 
				designPtr->cellPtr->grg->vSegMatrix[row][col];
			//m_grg->vSegList[row*m_colNum + col] = m_grg->vSegMatrix[row][col];
		}
	}

	designPtr->cellPtr->grg->segList = (MSegment **) malloc((m_rowNum*(m_colNum-1) + (m_rowNum-1)*m_colNum) * sizeof(MSegment *));

	for( int index = 0; index < m_rowNum*(m_colNum-1); index++ )
	{
		designPtr->cellPtr->grg->segList[index] = designPtr->cellPtr->grg->hSegList[index];
	}

	for( int index = 0; index < (m_rowNum-1)*m_colNum; index++ )
	{
		designPtr->cellPtr->grg->segList[index+m_rowNum*(m_colNum-1)] = designPtr->cellPtr->grg->vSegList[index];
	}


	// Step 2: Initialize all the elements in the matrixes and lists.

	for( int row = 0; row < m_rowNum; row++ )           //行列又变为0
	{
		for( int col = 0; col < m_colNum; col++ )
		{
	
			// Initialize current node.
			designPtr->cellPtr->grg->nodeMatrix[row][col]->No = 1 + row*m_colNum + col;
			designPtr->cellPtr->grg->nodeMatrix[row][col]->rowNo = row;
			designPtr->cellPtr->grg->nodeMatrix[row][col]->colNo = col;

			designPtr->cellPtr->grg->nodeMatrix[row][col]->x = 
				(designPtr->xMapList[col]+designPtr->xMapList[col+1])/2;   //中心点坐标
			designPtr->cellPtr->grg->nodeMatrix[row][col]->y =
				(designPtr->yMapList[row]+designPtr->yMapList[row+1])/2;

		}
	}

	for( int row = 0; row < m_rowNum; row++ )
	{
		for( int col = 0; col < m_colNum - 1; col++ )
		{

			designPtr->cellPtr->grg->hSegMatrix[row][col]->id = row*(m_colNum-1) + col;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->length = designPtr->cellPtr->grg->nodeMatrix[row][col+1]->x - designPtr->cellPtr->grg->nodeMatrix[row][col]->x;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->x1 = designPtr->cellPtr->grg->nodeMatrix[row][col]->x;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->y1 = designPtr->cellPtr->grg->nodeMatrix[row][col]->y;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->x2 = designPtr->cellPtr->grg->nodeMatrix[row][col+1]->x;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->y2 = designPtr->cellPtr->grg->nodeMatrix[row][col+1]->y;
			designPtr->cellPtr->grg->hSegMatrix[row][col]->s_node[0] = designPtr->cellPtr->grg->nodeMatrix[row][col];
			designPtr->cellPtr->grg->hSegMatrix[row][col]->s_node[1] = designPtr->cellPtr->grg->nodeMatrix[row][col+1];
//			designPtr->cellPtr->grg->hSegMatrix[row][col]->s_tracks =1;       // mao modify          use for give the hori capacity 
            designPtr->cellPtr->grg->hSegMatrix[row][col]->s_tracks =(grids[0].y2-grids[0].y1)/6 ;

			designPtr->cellPtr->grg->hSegMatrix[row][col]->dir = HORI;

			 designPtr->cellPtr->grg->hSegMatrix[row][col]->u_tracks = 0;
		}
	}

	for( int row = 0; row < m_rowNum - 1; row++ )
	{
		for( int col = 0; col < m_colNum; col++ )
		{

			designPtr->cellPtr->grg->vSegMatrix[row][col]->id = m_rowNum*(m_colNum - 1) + col*(m_rowNum-1) + row;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->length = designPtr->cellPtr->grg->nodeMatrix[row+1][col]->y - designPtr->cellPtr->grg->nodeMatrix[row][col]->y;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->x1 = designPtr->cellPtr->grg->nodeMatrix[row][col]->x;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->y1 = designPtr->cellPtr->grg->nodeMatrix[row][col]->y;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->x2 = designPtr->cellPtr->grg->nodeMatrix[row+1][col]->x;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->y2 = designPtr->cellPtr->grg->nodeMatrix[row+1][col]->y;
			designPtr->cellPtr->grg->vSegMatrix[row][col]->s_node[0] = designPtr->cellPtr->grg->nodeMatrix[row][col];
			designPtr->cellPtr->grg->vSegMatrix[row][col]->s_node[1] = designPtr->cellPtr->grg->nodeMatrix[row+1][col];

//			designPtr->cellPtr->grg->vSegMatrix[row][col]->s_tracks =  (designPtr->xMapList[1]- designPtr->xMapList[0])/((float)(designPtr->yMapList[1]- designPtr->yMapList[0])); // mao modify
            
			designPtr->cellPtr->grg->vSegMatrix[row][col]->s_tracks = (grids[0].x2-grids[0].x1)/7; 

			designPtr->cellPtr->grg->vSegMatrix[row][col]->dir = VERT_CHAN;

			 designPtr->cellPtr->grg->vSegMatrix[row][col]->u_tracks = 0;

		}
	}

	// Step 3: evaluate head and cellPtr of m_grg
	designPtr->cellPtr->grg->head = designPtr->cellPtr->grg->nodeMatrix[0][0];
	designPtr->cellPtr->grg->cellPtr = designPtr->cellPtr;

	return ;
}

