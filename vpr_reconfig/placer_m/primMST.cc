//global viarabals generally have '_' tail

 
#include <malloc.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include<iostream> 
using namespace std;


#include "fplan.h"
#include "primMST.h"
#include "scgroute.h"



LSHMST *msts_;//point to mst array of all nets
double perColDis_ =0;// glbNodes[2].node->x - glbNodes[1].node->x;
double perRowDis_ =0;// glbNodes[ glbNodes[1].node->n_linked[0]->No ].node->y - glbNodes[1].node->y;
	
long getIndex(int a, int b)
{
	int t;
	if(a<b){
		t=a;
		a=b;
		b=t;
		
	}
	return (long)( a*(a+1)/2 + b );

}

/* 用？？距离代替欧氏距离
double calDist(NodeID a, NodeID b)
{
	double ax, bx, ay, by;

	ax=(double)glbNodes[a].node->x;
	ay=(double)glbNodes[a].node->y;
	bx=(double)glbNodes[b].node->x;
	by=(double)glbNodes[b].node->y;

	return sqrt(((ax-bx)*(ax-bx) + (ay-by)*(ay-by) ));
}
*/
double calDist(NodeID a, NodeID b)   
{
	long ax, bx, ay, by;

	ax=glbNodes[a].node->x;
	ay=glbNodes[a].node->y;
	bx=glbNodes[b].node->x;
	by=glbNodes[b].node->y;
	
  return (double)(M_ABS(ax-bx)+M_ABS(ay-by));
}


//int stat[MAX_NODE];
//int selected[MAX_NODE];
/*
  generate an MST using prim Algr.
*/
int
aPrimMST(long netId)
{
	int i,j,k,s=-1;
	int l=0;//the number of edges which have generated at present.
	
#ifndef OPTI2
	int stat[MAX_NODE];
	int selected[MAX_NODE];
	double distTab[MAX_PAIR];
	NodeID nod[MAX_NODE];
#else
	int *stat;
	int *selected;
	double *distTab=NULL;
	//record the distant of all pairs of points int the specific net.
	NodeID *nod; 
#endif	

	double temp=MAX_DOUBLE;
	double dis=0;
    MSTEdge e;
	
	int nNode = glbNets[netId].nRequired+1;


	int nSele=0;//selected node number
	//if(glbNets[netId].nRequired<1)return 0;
#ifdef OPTI2
	nod = balloc(NodeID, nNode);
	selected = balloc(int, nNode);
	//memset(selected, -1, nNode);
	stat = balloc(int, nNode);
	//memset(stat, 0, nNode);
	if(nNode<USE_DIST_TAB_LIMIT)//因为当nNode过大时，disTab会超过一个数组所能表示的长度,即超过int
		distTab = balloc( double, nNode*(nNode+1)/2+1);//distance table
#endif
	
	
	for(i=0; i<nNode; i++)selected[i]=-1;
	for(i=0; i<nNode; i++)stat[i]=0;
	
	for(i=0; i<nNode; i++)
	{
		if(i==0) nod[0]=glbNets[netId].source.nodeID;
		else
			nod[i]=glbNets[netId].required[i-1].nodeID;
	}
/*	mao test
       for(i=0; i<nNode; i++)
		 printf("nod[%d]=%d ",i,nod[i]);
	   printf("this is ***************");
 printf("this is ++++++++++++++++++++++++++++++++\n");
*/
	if(nNode<USE_DIST_TAB_LIMIT){
		for(i=0; i<nNode; i++)
			for(j=0; j<=i; j++)
			{
				if(i==j) distTab[getIndex(i,j)]=0.0;
				else
				{ 
//					printf("nod[%d]= %d  nod[%d]=%d\n",i,nod[i],j,nod[j]);       // mao test
					distTab[getIndex(i,j)]=calDist(nod[i],nod[j]);
					//distTab[getIndex(i,j)]=calDist(nod[i],nod[j]);
				}
			}
	}
	
	msts_[netId].netID = netId;
	msts_[netId].nNode = nNode;
	msts_[netId].mstE = balloc(MSTEdge, nNode-1);///the edges of MST is nNode-1.
	
	// the core of prim algr.
	selected[0]=0;//selected 记录的是从0 - nsele-1次所选的点的序号
	stat[0]=1;//selected nod is set 1 to stat
	nSele++;
	if(nNode<USE_DIST_TAB_LIMIT){
	  l=0;
	  for(i=1; i<nNode; i++){
		//if(stat[i]) continue;
		for(j=0;j<nSele;j++){
			for(k=1; k<nNode; k++){
				if(stat[k])continue;
				if(distTab[getIndex(selected[j], k)]<=temp)
				{
					temp = distTab[getIndex(selected[j], k)];
					e.from = nod[selected[j]];
					e.to = nod[k];
					s=k;//临时选中的点s，为nod中第s个点
					if( 0.5<perColDis_/perRowDis_<2 ){
						if(temp<=perColDis_ || temp<=perRowDis_) goto lsh_RECORD;
					}else{
						if(temp <= 1.1*min(perColDis_, perRowDis_)) goto lsh_RECORD;
					}
				}
			}
		}
		lsh_RECORD:
		if( (s!=-1)&&(!stat[s]) ){
			selected[nSele++]=s;
			stat[s]=1;
			msts_[netId].mstE[l].from = e.from;
			msts_[netId].mstE[l].to = e.to;
			l++;
			temp = MAX_DOUBLE;
			s=-1;
		}else{
			printf("No selected.\n");
			exit(1);
		}
	  }
	}else{//局部MST
	  /*for(i=1; i<nNode; i++){
		//if(stat[i]) continue;
		for(j=0;j<nSele;j++){
			for(k=1; k<nNode; k++){
				if(stat[k])continue;
				if( calDist(nod[selected[j]], nod[k])<=temp )
				{
					temp = calDist( nod[selected[j]], nod[k] );
					e.from = nod[selected[j]];
					e.to = nod[k];
					s=k;//临时选中的点s，为nod中第s个点
				}
			}
		}
		if( (s!=-1)&&(!stat[s]) ){
			selected[nSele++]=s;
			stat[s]=1;
			msts_[netId].mstE[l].from = e.from;
			msts_[netId].mstE[l].to = e.to;
			l++;
			temp = MAX_DOUBLE;
			s=-1;
		}
	  }*/
	  	l=0;
	  	for(i=0; i<nNode-1; i++){
	  		for(k=1; k<nNode; k++){
	  			if(stat[k])continue;
	  			dis = calDist(nod[selected[i]], nod[k]);
	  			if( dis<perColDis_*5 || dis<perRowDis_*5 ){
	  				s =k;
	  				break;
	  			}else if(dis < temp){
	  				temp = dis;
	  				s = k;
	  			}	
	  		}
	  		if( (s!=-1)&&(!stat[s]) ){
				selected[i+1]=s;
				stat[s]=1;
				msts_[netId].mstE[l].from = nod[selected[i]];
	  			msts_[netId].mstE[l].to = nod[s];
				l++;
				temp = MAX_DOUBLE;
				s=-1;
				}else{
					printf("No selected.\n");
					exit(1);
	  		    }
	  	}
	}
	//i=0;
	#ifdef LSH_CHECK_MST
	for(j=0; j<nNode; j++){
		if(!stat[j]){
			printf("Woo!!\n");
			fprintf(stderr,"The MST generated incorrectly.\n");
			exit(1);
		}
	}
	#endif
#ifdef OPTI2		
	free(selected);
	free(stat);
	if(nNode<USE_DIST_TAB_LIMIT) free(distTab);
	free(nod);
#endif
	return 0;
}

int 
allPrimMST()
{   
	int	i;
	perColDis_ = designPtr->xMapList[1]-designPtr->xMapList[0]; // glbNodes[2].node->x - glbNodes[1].node->x;
	perRowDis_ = designPtr->yMapList[1]-designPtr->yMapList[0]; //glbNodes[ glbNodes[1].node->n_linked[0]->No ].node->y - glbNodes[1].node->y;
	msts_ = balloc( LSHMST, netNum );//netNum is a global viarables.
	for( i=0; i< netNum; i++){          // mao modify       
		msts_[i].netID = i;
		msts_[i].nNode = 0;
		msts_[i].mstE=NULL;
	}//initialize the mst
		

	for(  i = 0;  i < netNum;  i ++ ) 
	{

		 if(numbersets[i]>1)
		{
			#ifdef LSH_DEBUG
			printf("MST id: %d \tis being constructed.\n", i);
			#endif
			aPrimMST(i);
		}
	}
	return 0;	
}

int printMST()
{
	int i, j;
	FILE *fp;
	if(!(fp=fopen("mst.f", "w"))) {
		fprintf(stderr,"can't open file mst.f\n");
		exit(1);
	}
	printf("MST is recorded in mst.f.\n");
	for(i=0; i<= netNum; i++){
		if(( glbNets[i].netPtr != NULL )
			&&(( glbNets[i].nRequired > 1 )
			||( glbNets[i].source.nodeID != glbNets[i].required[0].nodeID )))
		{
			fprintf(fp, "net %ld ", msts_[i].netID);
			for(j=0; j<msts_[i].nNode-1; j++)
			{
				fprintf(fp, "E %d--%d ", msts_[i].mstE[j].from+1, msts_[i].mstE[j].to+1);
			}
			fprintf(fp, "\n");
		}
	}
	
	fclose(fp);
	
	return 0;
}










