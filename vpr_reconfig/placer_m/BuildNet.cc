 
#define BHYdebugBuildNetDupSource	0x2

#include <memory.h>
#include <string.h>
#include<iostream> 
using namespace std;

#include "fplan.h"
#include "scgroute.h"
#include "hptools.h"


#include "SCglobals.h"
#include "BuildNet.h"





int	netNum;

BHYNetIndex	*glbNets;




void mfbBuildAGRGNet(
	BHYrequiredNode *tmpNodes,
	BHYrequiredNode *tmpSource,
	int		*nNodes, 	
	int  networkId
  );                  //��������



//#define BHYnoReallocGlbNets


/*
FUNCTION:
	Put all the required node of the net into GRG by setting the flags 
	of the corresponding node.
PROGRAMED BY Bao Hai-yun.
*/

void
putRequiredNodeIntoGRG( 
	BHYNetIndex	*netIdx )
{
	int	i;

	for(  i=0;  i<netIdx->nRequired;  i++ )
		glbNodes[netIdx->required[i].nodeID].flag |= BHYMaskNodeFlagRequired;
	glbNodes[netIdx->source.nodeID].flag |= BHYMaskNodeFlagRequired;
}




//Function add mao  check the nodeId is unique 


 int isUnique( vector<virtualPin> &net , int pinId)
{

int i;

if(net.empty()||net.size()==0)
        return 1;

for(  i=1;  i< pinId; i++ )
    if(net[pinId].vertex==net[i].vertex)
			return 0;
	return 1;

}




/*
FUNCTION:
	Check if nodes[nNodes].nodeID is unique in the 'nNodes' nodes.
	internal function.
PROGRAMED BY Bao Hai-yun.
*/


static int
isUniqueNodeID(
	BHYrequiredNode *nodes,
	int		nNodes )

{
	int	i;

	for(  i=0;  i<nNodes;  i++ )
		if( nodes[ i ].nodeID == nodes[ nNodes ].nodeID )
			return 0;
	return 1;
}



/*
FUNCTION:
	Check if nodes[nNodes].nodeID is unique in the 'nNodes' nodes.
	internal function.
PROGRAMED BY Bao Hai-yun.
*/

static void
swapSourceToFirst(
	BHYrequiredNode *nodes,
	int		nNodes,
	int		sourceID )

{
	int		i;
	BHYrequiredNode	tmp;

	for(  i=0;  i<nNodes;  i++ )
	{
		if( nodes[i].nodeID == sourceID )
		{
			memcpy( &tmp, &(nodes[i]), sizeof( BHYrequiredNode ));
			memcpy( &(nodes[i]), nodes, sizeof( BHYrequiredNode ));
			memcpy( nodes, &tmp, sizeof( BHYrequiredNode ));
			return;
		}
	}
}








void lshBuildGRGNet()
{
	long	netId;
	int	nNodes;
	BHYrequiredNode tmpNodes[ LSH_NETSIZE ];
  
	BHYrequiredNode tmpSource;
//	MNet	*	lshNet;

//Initialize the global variables	
//#ifndef BHYnoReallocGlbNets
	netNum = 0;
	glbNets = NULL;
//#else
//	netNum = designPtr->netNum + 1;

    netNum = designPtr->netNum ;


	glbNets = balloc( BHYNetIndex, netNum );
	{
		int	j;
		for(  j=0;  j<netNum;  j++ )
		{
			glbNets[ j ].nTrees = 0;
			glbNets[ j ].nSpecialTrees = 0;
			glbNets[ j ].curTree = NULL;
			glbNets[ j ].bestTree = NULL;
			glbNets[ j ].queuePos = 0;
			glbNets[ j ].nRequired = 0;
			glbNets[ j ].required = NULL;
			glbNets[ j ].source.nodeID = -1;
			glbNets[ j ].netPtr = NULL;
#ifndef BHYwireOnly
			glbNets[ j ].timingDrivenTree = NULL;
#endif
		}
	}
//#endif
   
   

	for(int i=0; i<netsets.size();i++)
	{
	//	if(Network[i].size()>1)  //��Ϊnetwork�У��ų�ʵ����ͬ�Ķ��㣿��
		if(numbersets[i]>1)
		{
		     netId=i;
          mfbBuildAGRGNet(tmpNodes, &tmpSource, &nNodes, netId); //mao add 

			// ���Ƶ� glbNets ��, see "scgroute.h" for help	 
			glbNets[ netId ].nTrees = 0;
			glbNets[ netId ].nSpecialTrees = 0;
			glbNets[ netId ].curTree = NULL;
			glbNets[ netId ].bestTree = NULL;
			glbNets[ netId ].queuePos = 0;
			glbNets[ netId ].nRequired = numbersets[i]-1;
			glbNets[ netId ].required = balloc( BHYrequiredNode,numbersets[i]-1);
			memcpy( glbNets[ netId ].required, tmpNodes,
				sizeof( BHYrequiredNode ) *  (numbersets[i]-1));
			memcpy( &(glbNets[ netId ].source), &tmpSource,
			sizeof( BHYrequiredNode ));
			glbNets[ netId ].netPtr = NULL;
	#ifndef BHYwireOnly
			glbNets[ netId ].timingDrivenTree = NULL;
	#endif
		
		
		}
	 
	
	}
}






//mfbBuildAGRGNet(tmpNodes, &tmpSource, &nNodes, lshNet); 
void mfbBuildAGRGNet(
	BHYrequiredNode *tmpNodes,
	BHYrequiredNode *tmpSource,
	int		*nNodes, 	
	int  networkId
  )

{
        int i;
 		int netId;//liush
		netId = networkId;
		if( netId > netNum - 1 )
		{
	/*
	Ϊʹ�������ͨ���ԣ���ʱ���� glbNets �Ĵ�С������ ????.netlist.r �е�������
	����������ʱ��Ƚ�������ʱ���öࡣ
	*/
			glbNets = brealloc( glbNets, BHYNetIndex, netId+1 );
	/*
	��ʼ����������������
	*/
			for( int j = netNum;  j < netId;  j++ )
			{
				glbNets[ j ].nTrees = 0;
				glbNets[ j ].nSpecialTrees = 0;
				glbNets[ j ].curTree = NULL;
				glbNets[ j ].bestTree = NULL;
				glbNets[ j ].queuePos = 0;
				glbNets[ j ].nRequired = 0;
				glbNets[ j ].required = NULL;
				glbNets[ j ].source.nodeID = -1;
				glbNets[ j ].netPtr = NULL;
	#ifndef BHYwireOnly
				glbNets[ j ].timingDrivenTree = NULL;
	#endif
			}
			netNum = netId + 1;
		}


             int k;
  		     i=netId;
			if(numbersets[i]>1)
			{      k=0;
				for(int j=0;j<netsets[i].size();j++)
				{   
					if(netsets[i][j].prop==0)
					{
						tmpSource->nodeID = netsets[i][j].vertex-1;
						tmpSource->netId = i;
				        tmpSource->nodeSeq = -1;
				        tmpSource->partialOrder = 0;
			        	tmpSource->flag = 0;
				        tmpSource->cIn = (float)(0.0);
				        tmpSource->requiredDelay = MAXFLOAT;
				   /* Bao Hai-yun: Setting to -0.01, so that it will be reset	*/ 
				    /*              in the spreadForward definitely.		*/ 
				        tmpSource->currentDelay = (float)(-0.01);
				        tmpSource->afnNode = NULL;
				        tmpSource->trans = NULL;
				
					}
					else
						if(netsets[i][j].prop==1)
						{
						tmpNodes[numbersets[i]-2].nodeID = netsets[i][j].vertex-1;
						tmpNodes[numbersets[i]-2].netId = i;
					    tmpNodes[numbersets[i]-2].nodeSeq =numbersets[i]-2;
					    tmpNodes[numbersets[i]-2].partialOrder = MAXINT;
					    tmpNodes[numbersets[i]-2].flag = 0;
					    tmpNodes[numbersets[i]-2].cIn = 0.0;
					    tmpNodes[numbersets[i]-2].requiredDelay = MAXFLOAT;
					/* Bao Hai-yun: Setting to -0.01, so that it will be reset	*/ 
					/*              in the spreadForward definitely.		*/ 
					    tmpNodes[numbersets[i]-2].currentDelay = (float)(-0.01);
				    	tmpNodes[numbersets[i]-2].afnNode = NULL;
				    	tmpNodes[numbersets[i]-2].trans = NULL;
						}
						else
							if(numbersets[i]>2)
								if( isUnique(netsets[i], j) )       //����ǰ���vertex��ͬ������ռ丳ֵ ��k++
								{
									tmpNodes[k].nodeID = netsets[i][j].vertex-1;
									tmpNodes[k].netId = i;
					                tmpNodes[k].nodeSeq =k;
					                tmpNodes[k].partialOrder = MAXINT;
					                tmpNodes[k].flag = 0;
					                tmpNodes[k].cIn = 0.0;
					                tmpNodes[k].requiredDelay = MAXFLOAT;
					             /* Bao Hai-yun: Setting to -0.01, so that it will be reset	*/ 
					             /*              in the spreadForward definitely.		*/ 
					                tmpNodes[k].currentDelay = (float)(-0.01);
				    	            tmpNodes[k].afnNode = NULL;
				    	            tmpNodes[k].trans = NULL;
								    k++;
								}
				  }//for
			}//if
		return;
}



