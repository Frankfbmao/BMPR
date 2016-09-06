#include <malloc.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include<iostream> 
using namespace std;

#include  "fplan.h"     //���ڲ���printglbNodes row_number col_number
#include "primMST.h"
#include "congEsti.h"


#include "BuildGrg.h"
#include "SCglobals.h"
#include "scgroute.h"
#include "BuildNet.h"


void lshBuildGRG();
/*
FUNCTION:
	Save the resolution into ????.route file. May be the .route file will
	be deserted later. And now it is used to compare with Roach.
PROGRAMED BY Bao Hai-yun
*/
//mao add  ���ڲ���glbNodes������NO��ϵ
void printglbNodes()
{	
	FILE *fa=fopen("printglbNodes.txt","a");
	
	for( int i=0;  i < BHYNumberOfNodes(row_number,col_number);  i++ )
	{		fprintf(fa,"\n");
	        fprintf( fa,"g[%d]  %d   %d  \n",i, glbNodes[i].node->rowNo, glbNodes[i].node->colNo);
           
	}
	 fclose(fa);
 return;
}





/*
FUNCTION:
	Refresh the congestion information of GRG ( glbSegments ).
PROGRAMED BY Bao Hai-yun.
*/

static void refreshCongestionInfoOfGRG( )
{
	int	i;

	for(  i=0;  i<BHYNumberOfSegments( rowNum, colNum );  i++ )
	{
		BHYrefreshWeightOfSegment( glbSegments[i].segment );
        }
}




/*
��glbSegments[id].segment->u_tracks����
*/
void clearSegmentUTracks()
{
	int i;
	for(i=0;  i<BHYNumberOfSegments( rowNum, colNum );  i++)
		glbSegments[i].segment->u_tracks=0;
	return;
}

void routEstimator(	char mode )
{
	
	// int     i, j;
	/* 0 for wire-only version, and 1 for timing-driven version	*/ 
	/* Current max and average congestion value, and the minimum vlue ever reached	*/ 
//	BHYgrEvolutionParameter	eP, good;

	/* The number of turns that didn't get better solution	*/ 
//	int	nNoDevelopTurn;
	/* The current Position of the Queue	*/ 
//	int	currentQueuePos;
//	int	currentWaitBound;
	float	e;
//	int	goodDelay;
	//time_t	beginTime;
//	long	StartTime,MiddleTime,EndTime;
//	StartTime = GetCpuTime();


//	initializeGRparameter( mode );

	randomize();//���������������������
	mode = 'w';//wire only	
	

/*
(4) ����ʼ��ת��Ϊ����ı߱�����顣���õ�ÿ��©���ʱ�Ӻ�ÿ���ߵĹ̶�ϵ����
*/
/*
Load all vertices, edges and nets into glbNodes, glbSegments and glbNets.
*/


	#if BHYdebugMain & BHYdebugMainTrace
//	puts( "Building GRG." );
	#endif
	//BuildGRG( workDir );
	lshBuildGRG();
//	puts("Building lshGRG end!");
	#if BHYdebugMain & BHYdebugMainTrace
//	puts( "Building nets." );
	#endif
	//BuildGRGNet( workDir );
	lshBuildGRGNet();
//	puts("Building lshGRGNet end!");

	#if BHYdebugMain & BHYdebugMainTrace
//	puts( "Reading initial Steiner Trees." );
	#endif

	/* �����ƣ���ʼ���ڵ�ĵ�����Ϣ	*/ 
	#ifndef BHYwireOnly
	if( mode != 'w' )
	{
		//initCapacityInfoOfNet();
	}
#endif
	#ifdef BHY_TIMER
	MiddleTime = GetCpuTime();
	printf("initial time:\n");
	GetElapsedTime(StartTime, MiddleTime);
	#endif
/*
	Step 1 generated the MST and estimate the congestion
*/
	allPrimMST();
	
	#ifdef BHY_TIMER
	EndTime = GetCpuTime();
	printf("PrimMST time:\n");
	GetElapsedTime(MiddleTime,EndTime);
	#endif
	#ifdef LSH_PRINT_MST
	printMST();//for check
	#endif

//	printglbNodes();        //mao for test

	estiCongAllNets();


/*
	#ifdef BHY_TIMER
	MiddleTime = GetCpuTime();
	printf("Estimate time:\n");
	GetElapsedTime(EndTime, MiddleTime);
	#endif
	//--clearSegmentUTracks();//liush add 0905
	setCongestionFlagOfGRG( 0.8, 0.8 );

//	Step 2 construct the first Steiner Tree according to segment weight.

	#ifdef LSH_WITH_PENALTY
	addPenalty2WeightOfSegment(20);
	#endif
	constructFirstTreeOfAllNet();
	#ifdef BHY_TIMER
	EndTime = GetCpuTime();
	printf("Construct First ST time:\n");
	GetElapsedTime(MiddleTime, EndTime);
	#endif
//	printArrayST();
	

//(5) ͳ���ܵ���Դʹ��������Լ��ؼ�·�������

	for( int i=0;  i<= netNum;  i++ )
		if( glbNets[ i ].curTree != NULL )
			allocResourceBySteinerArray( glbNets[i].curTree );
	

	getCongestionInfoOfGRG( &bhyPar.best );
	recordGoodSolution();
	setCongestionFlagOfGRG( 0.8, 0.8 );
	traceGlobalGRG2File( "Original GRG", &bhyPar.best );
	//postRandomOptimization( 1 );

	initEvolutionParameter( &bhyPar.best );

//	reroute();
	//postRandomOptimization(1);
//	traceGlobalGRG2File( "After Optimization:", &bhyPar.best );

*/
	for(int i=0;i<netsets.size();i++)
	{
		if(msts_[i].mstE!=NULL)
		{
			free(msts_[i].mstE);
	         msts_[i].mstE=NULL;
		}
	}


	free(msts_);
	return;
}







