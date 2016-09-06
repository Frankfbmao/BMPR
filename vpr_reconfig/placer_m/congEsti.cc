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
#include "congEsti.h"



grid_net tempNet;  //用于把信息存到grids中          //mao add 




int IsIn(vector<grid_net> &net, grid_net &temp  )
{
	if(net.empty ()||net.size()==0)
		return 0;
	else
		for(int i=0; i< net.size(); i++)
			if((net[i].from==temp.from)&&(net[i].to==temp.to))
				return 1;
	return 0;
}




//存储概率上track的使用情况，不能用glbSegments[hsId].segment->u_tracks，因为它是int型
float* p_u_tracks_=NULL;

static double routTab_[MAX_TAB][MAX_TAB];//为了使运算速度提高，我们保存了中间的结果。初始时，会自动赋0，由于是static
/*
static long g_twoPins=0;
static long g_flatPins=0;
*/

  long g_twoPins;
  long g_flatPins;









//其实，上述矩阵为一个对称矩阵，可以用三角阵存储用getIndex（x，y）

/*
*计算从整点（x,y），其实相当于是x行，y列
*递推公式：
*f(0,0)=1
*f(x,0)=1
*f(0,y)=1
*f(x,y)= f(x-1,y)+f(x,y-1)
*/
double calNRouts(long x, long y )
{
	double temp=0;
	if(x<0||y<0){
		fprintf(stderr, "x ,y must be nonnegative integer.\n");
		exit(1);
	}
	else if(x==0||y==0) return 1;
	else if(routTab_[x][y])return routTab_[x][y];//其实，routTab_[0][*] routTab_[*][0]没有被用到，也没赋值
	else{
	
		temp = calNRouts(x-1,y)+calNRouts(x,y-1);
		if(x<MAX_TAB && y<MAX_TAB) routTab_[x][y] = temp;
		return temp;
	}
	return -1;
	
}

/*
下面两个函数用来计算水平和垂直边的通过概率，
在以O(x1,y1),P(x2, y2)其中x1<=x2, y1<=y2的标准形式下，以O P为对角点的Bounding Box,
水平边h(x,y)指向右的边(x,y)~(x+1,y)
垂直边v(x,y)指向上的边(x,y)~(x,y+1)
O(0,0)~P(u,v)Bounding Box
C(x,y)为内点
*/

float probHoriE(long u, long v, long x, long y)
{
	double pro=0;
	if(u<0||v<0||x<0||y<0){
		fprintf(stderr, "u,v;x,y must be nonnegative integer.\n");
		exit(1);
	}
	else if(y>v){
		fprintf(stderr, "y mustn't be larger than v.\n");
		exit(1);
	}
	else if( x>=u )return 0.0;//水平边伸了了出界
	else {
		pro+=calNRouts(x,y)*calNRouts(u-x-1, v-y);
		pro/=calNRouts(u,v);
		return (float)pro;
	}
	return 0.0;
}

float probVertE(long u, long v, long x, long y)
{
	double pro=0;
	if(u<0||v<0||x<0||y<0){
		fprintf(stderr, "u,v;x,y must be nonnegative integer.\n");
		exit(1);
	}
	else if(x>u){
		fprintf(stderr, "x mustn't be larger than u.\n");
		exit(1);
	}
	else if(y>=v)return 0.0;//垂直边伸出了边界
	else{
		pro+=calNRouts(x,y)*calNRouts(u-x,v-y-1);
		pro/=calNRouts(u,v);
		return (float)pro;
	}
	return 0.0;
		
}

/*
void addProb2GRG(NodeID m, NodeID n)
{
	int x1,y1,x2,y2;//实际记录的点的行列号 x为列号，y为行号
	int u=0;
	int v=0;
	int flag;
	int i,j;
	int vsId=0;//垂直 记录segmentID
	int hsId=0;//水平
	//int p1,p2,p3,p4;
	NodeID temp=0;
	//使m为n下面的点
	if( glbNodes[m].node->rowNo > glbNodes[n].node->rowNo ){
		temp = m;
		m = n;
		n = temp;
	}
	x1 = glbNodes[m].node->colNo;
	y1 = glbNodes[m].node->rowNo;
	x2 = glbNodes[n].node->colNo;
	y2 = glbNodes[n].node->rowNo;
	
	u = x2-x1;
	flag =u;//标志x是正，还是负
	u = abs(u);
	v = y2-y1;//y>=0
	
    if(flag>=0){   	
		for(i=0; i<=u; i++){
			for(j=0; j<=v; j++){
				if( x1+i<=colNum && y1+j+1<rowNum){
					//rowNum是行的个数，但是记录时是从0记的，而colNum是列的个数，但记录是从1记的
					//row[0-8] col[1-11]
					vsId=coordinates2SegmentID( (x1 + i), (y1 + j), (x1 + i), (y1 + j + 1) );
					if(glbSegments[vsId].segment)
						glbSegments[vsId].segment->weight += probVertE(u, v, i, j);//垂直边
				}
				if( x1+i+1<=colNum && y1+j<rowNum ){//same as above
					hsId=coordinates2SegmentID(x1+i, y1+j, x1+i+1, y1+j);
					if(glbSegments[hsId].segment)
						glbSegments[hsId].segment->weight += probHoriE(u, v, i, j);//水平边
				}
			}
		}
	}else{
		vsId=hsId=0;
		for(i=0; i<=u; i++){
			for(j=0; j<=v; j++){
				//printf("m=%d, n=%d\n", m, n);
				//printf("A\n");
				if( x1-i>0 && y1+j+1<rowNum){//liush x1 starts from 1 same as col
					vsId=coordinates2SegmentID( (x1 - i), (y1 + j), (x1 - i), (y1 + j + 1) );///liush modified 0818
					//printf("B\n");
					//printf("vsId=%d \n", vsId);
					if( glbSegments[vsId].segment ){
						//printf("*****Haha\n");
					//发现会有一些glbSegments的segments为空，比如180，182，184空，而181，183不空，不知为什么,因为超出了界
						//printf("vsId=%d \n",vsId);
						//printf("weight=%f", glbSegments[vsId].segment->weight);
						glbSegments[vsId].segment->weight += probVertE(u, v, i, j);//垂直边
					}
				}
				//printf("C\n");
				if( x1-i-1>0 && y1+j<rowNum ){//liush modified 0818; x1 starts from 1 same as col
					hsId=coordinates2SegmentID(x1-i, y1+j, x1-i-1, y1+j);//liush modified 0818
					//printf("D\n");
					if(glbSegments[hsId].segment)
						glbSegments[hsId].segment->weight += probHoriE(u, v, i, j);//水平边
					//printf("E\n");
				}
			}
		}
    }
    
    return;
}
*///liush 0905 commented. The main difference is segment的weight给的方法。

/*
FUNCTION:
	Refresh the congestion information of GRG ( glbSegments ).
PROGRAMED BY Bao Hai-yun.
*/

static void refreshCongestionInfoOfGRG()
{
	int	i;

	for(  i=0;  i<BHYNumberOfSegments( rowNum, colNum );  i++ )
	{
		BHYrefreshWeightOfSegment( glbSegments[i].segment );
    }
}

/*
FUNCTION :
    Refresh the wieght of a segment, according to its capacity and usage.
*/
#define BHYweightOfSegment( s )\
	((( s )->u_tracks == 0 ) \
	? ( float )(( 0.03125 ) / ( (s)->s_tracks + 0.0625 )) \
	: ( float )(((s)->u_tracks+0.0625 ) / ( (s)->s_tracks+0.0625 )))
#define BHYrefreshWeightOfSegment( s )\
{\
	(s)->weight = BHYweightOfSegment( s );\
}

#define LSHF(m) (float)(m*m+m+1)

/*
	cal the probability of tow-pin net which pins in one row--liush add 06-01-17
*/
void probOfPinsInOneRow(NodeID m, NodeID n)
{
	int x1,x2,y;//实际记录的点的行列号 x为列号，y为行号
	NodeID temp;
	int u=0;
	int i;
	int sId=0;
	float total=0;
	if(glbNodes[m].node->colNo == glbNodes[n].node->colNo) return;
	else if(glbNodes[m].node->colNo > glbNodes[n].node->colNo){
		temp = m;
		m=n;
		n=temp;
	}
	x1 = glbNodes[m].node->colNo;
	y = glbNodes[m].node->rowNo;// the same row
	x2 = glbNodes[n].node->colNo;
//	y2 = glbNodes[n].node->rowNo;
	u=x2-x1;
	total=LSHF(u);
	for(i=0; i<u; i++){
		sId=coordinates2SegmentID(x1+i, y, x1+i+1, y);
		if(glbSegments[sId].segment)
		{
			//printf("%f\n", LSHF(u));
			p_u_tracks_[sId] += (LSHF(i)+ LSHF(u-i-1)-1)/total;
		}
		sId=coordinates2SegmentID(x1+i, y+1, x1+i+1, y+1);
		if(glbSegments[sId].segment)
			p_u_tracks_[sId] += (i+1)*(u-i)/total;
		sId=coordinates2SegmentID(x1+i, y-1, x1+i+1, y-1);
		if(glbSegments[sId].segment)
			p_u_tracks_[sId] += (i+1)*(u-i)/total;
		sId=coordinates2SegmentID(x1+i, y, x1+i, y+1);
		if(glbSegments[sId].segment)
			p_u_tracks_[sId] += u/total;
		sId=coordinates2SegmentID(x1+i, y, x1+i, y-1);
		if(glbSegments[sId].segment)
			p_u_tracks_[sId] += u/total;
	}
	sId = coordinates2SegmentID(x2, y, x2, y-1);
	if(glbSegments[sId].segment)
			p_u_tracks_[sId] += u/total;
	sId = coordinates2SegmentID(x2, y, x2, y+1);
	if(glbSegments[sId].segment)
			p_u_tracks_[sId] += u/total;
	//two
	
	return;
}

/*
	cal the probability of tow-pin net which pins in one row--liush add 06-01-18
*/
void probOfPinsInOneCol(NodeID m, NodeID n)
{
	int x, y1,y2;
	NodeID temp;
	int v=0;
	int i;
	int sId=0;
	float total;
	//Input should be m is lower and n is upper
	if(glbNodes[m].node->rowNo == glbNodes[n].node->rowNo)return;
	else if(glbNodes[m].node->rowNo > glbNodes[n].node->rowNo){
		temp = m;
		m = n;
		n = temp;		
	}
	x = glbNodes[m].node->colNo;
	y1 = glbNodes[m].node->rowNo;
	y2 = glbNodes[n].node->rowNo;
	v= y2-y1;
	total = LSHF(v);
	for(i=0; i<v; i++){
		sId=coordinates2SegmentID(x, y1+i, x, y1+i+1);
		if(glbSegments[sId].segment)
			p_u_tracks_[sId] += (LSHF(i)+ LSHF(v-i-1)-1)/total;
		sId=coordinates2SegmentID(x+1, y1+i, x+1, y1+i+1);
		if(glbSegments[sId].segment)
			p_u_tracks_[sId] += (i+1)*(v-i)/total;
		sId=coordinates2SegmentID(x-1, y1+i, x-1, y1+i+1);
		if(glbSegments[sId].segment)
			p_u_tracks_[sId] += (i+1)*(v-i)/total;
		sId=coordinates2SegmentID(x, y1+i, x+1, y1+i);
		if(glbSegments[sId].segment)
			p_u_tracks_[sId] += v/total;
		sId=coordinates2SegmentID(x, y1+i, x-1, y1+i);
		if(glbSegments[sId].segment)
			p_u_tracks_[sId] += v/total;
	}
	sId = coordinates2SegmentID(x , y2, x+1, y2);
	if(glbSegments[sId].segment)
			p_u_tracks_[sId] += v/total;
	sId = coordinates2SegmentID(x, y2, x-1, y2);
	if(glbSegments[sId].segment)
			p_u_tracks_[sId] += v/total;
	//two
	return;	
}
/*
估计从glbNode m 到 n所组成的Bounding Box中的glbSegments的通过概率，并加到GRG上
The main difference is segment的weight给的方法。
*/
void addProb2GRG(NodeID m, NodeID n)
{

	int mflag;   //mao add  用于判断是否是模块上的pin  flag==1表明“是” flag==0表明“不是” 

	int x1,y1,x2,y2;//实际记录的点的行列号 x为列号，y为行号
	int u=0;
	int v=0;
	int flag;
	int i,j;
	int vsId=0;//垂直 记录segmentID
	int hsId=0;//水平
	//int p1,p2,p3,p4;
	NodeID temp=0;
	//使m为n下面的点
	if( glbNodes[m].node->rowNo > glbNodes[n].node->rowNo ){
		temp = m;
		m = n;
		n = temp;
	}
                                         // mao add
	x1 = glbNodes[m].node->colNo;     //由行列可知grid ID，用一个关系式计算出grid ID   //i*col+j 得到网格号
	y1 = glbNodes[m].node->rowNo;
	x2 = glbNodes[n].node->colNo;
	y2 = glbNodes[n].node->rowNo;
	g_twoPins++;
        if((x1==x2&&y1!=y2)||(x1!=x2&&y1==y2))
	   g_flatPins++;
	//对extending Bbox实行分流
#ifndef LSH_NO_EXTEND
	if( (y1 == y2)&&(y1<rowNum-1)&&(y1>0) ){
       

#if 0	
                if(grids[m].m_id.size()==0)
				{        
					mflag=0;
					if((m-1>=0)&&grids[m-1].m_id.size()!=0)
					{
						m=m-1;
						mflag++;
					}
					else
						if( ((m-col_number)>=0)&&grids[m-col_number].m_id.size()!=0)
						{
							m=m-col_number;
							mflag++;
						}   
						else
							if( ( (m-col_number-1)>=0 ) &&  (grids[m-col_number-1].m_id.size()!=0) )
							{
								m=m-col_number-1;
								mflag++;
							}

							if (mflag!=0)
								tempNet.from=m;          // mao add
							else 
								tempNet.from=FFlag;       // 不是模块上的pin，舍去调整模块时不考虑
				}      
				else
					tempNet.from=m;
				
				if(grids[n].m_id.size()==0)
				{        
					mflag=0;
					if((n-1>=0)&&grids[n-1].m_id.size()!=0)
					{
						n=n-1;
						mflag++;
					}
					else
						if( ((n-col_number)>=0)&&grids[n-col_number].m_id.size()!=0)
						{
							n=n-col_number;
							mflag++;
						}   
						else
							if( ( (n-col_number-1)>=0 ) && (grids[n-col_number-1].m_id.size()!=0) )
							{
								n=n-col_number-1;
								mflag++;
							}
							
							
							if (mflag!=0)
								tempNet.to=n;          // mao add
							else 
								tempNet.to=FFlag;
				}      
				else
					tempNet.to=n;
				
				if( (tempNet.from!=FFlag)||(tempNet.to!=FFlag) )
				{
					if(x1>x2)
						for(int k=x2;k<=x1;k++)
							if(!IsIn(grids[y1*col_number+k-1].net,tempNet))
								grids[y1*col_number+k-1].net.push_back(tempNet);
							else
								for(int k=x1;k<=x2;k++)
									if(!IsIn(grids[y1*col_number+k-1].net,tempNet))
										grids[y1*col_number+k-1].net.push_back(tempNet);	
				}

#endif
				//puts("probOfPinsInOneRow");
				probOfPinsInOneRow(m, n);
				
				return;
}



	if((x1 == x2)&&(x1<colNum)&&(x1>1) ){
		
# if 0
		if(grids[m].m_id.size()==0)
		{        
	       mflag=0;
		   if((m-1>=0)&&grids[m-1].m_id.size()!=0)
		   {
			   m=m-1;
			   mflag++;
		   }
		   else
			   if( ((m-col_number)>=0)&&grids[m-col_number].m_id.size()!=0)
			   {
				   m=m-col_number;
				   mflag++;
			   }   
			   else
				   if( ( (m-col_number-1)>=0 ) &&  (grids[m-col_number-1].m_id.size()!=0) )
				   {
					   m=m-col_number-1;
					   mflag++;
				   }
				   if (mflag!=0)
					   tempNet.from=m;          // mao add
				   else 
					   tempNet.from=FFlag;       // 不是模块上的pin，舍去调整模块时不考虑
		}      
		else
			tempNet.from=m;
		
		if(grids[n].m_id.size()==0)
		{        
			mflag=0;
			if((n-1>=0)&&grids[n-1].m_id.size()!=0)
			{
				n=n-1;
				mflag++;
			}
			else
				if( ((n-col_number)>=0)&&grids[n-col_number].m_id.size()!=0)
				{
					n=n-col_number;
					mflag++;
				}   
				else
					if( ( (n-col_number-1)>=0 ) && (grids[n-col_number-1].m_id.size()!=0) )
					{
						n=n-col_number-1;
						mflag++;
					}

					if (mflag!=0)
						tempNet.to=n;          // mao add
					else 
						tempNet.to=FFlag;
		}      
		else
			tempNet.to=n;
		
		
		if( (tempNet.from!=FFlag)||(tempNet.to!=FFlag) )
		{
			if(y1>y2)
				for(int k=y2;k<=y1;k++)
					if(!IsIn(grids[k*col_number+x1-1].net,tempNet))
						grids[k*col_number+x1-1].net.push_back(tempNet);
					else
						for(int k=y1;k<=y2;k++)
							if(!IsIn(grids[k*col_number+x1-1].net,tempNet))
								grids[k*col_number+x1-1].net.push_back(tempNet);
		}
#endif
		
		probOfPinsInOneCol(m, n);
		return;
}
#endif
	u = x2-x1;
	flag =u;//标志x是正，还是负
	u = abs(u);
	v = y2-y1;//y>=0
	
    if(flag>=0){   	
		for(i=0; i<=u; i++){
			for(j=0; j<=v; j++){
				if( x1+i<=colNum && y1+j+1<rowNum){
					//rowNum是行的个数，但是记录时是从0记的，而colNum是列的个数，但记录是从1记的
					//row[0-8] col[1-11]
					vsId=coordinates2SegmentID( (x1 + i), (y1 + j), (x1 + i), (y1 + j + 1) );     // mao add 需要由点的行，列计算出相应grid ID 可以由前面的方法进行计算  然后记录经过的线网号及（from ,to） 点对
					if(glbSegments[vsId].segment)
					{
						p_u_tracks_[vsId] += probVertE(u, v, i, j);//垂直边                      
                          //   i*col+j 得到网格号

#if 0
                         if(grids[m].m_id.size()==0)
						 {        
							 mflag=0;
							 if((m-1>=0)&&grids[m-1].m_id.size()!=0)
							 {
								 m=m-1;
								 mflag++;
							 }
							 else
								 if( ((m-col_number)>=0)&&grids[m-col_number].m_id.size()!=0)
								 {
									 m=m-col_number;
									 mflag++;
								 }   
								 else
									 if( ( (m-col_number-1)>=0 ) &&  (grids[m-col_number-1].m_id.size()!=0) )
									 {
										 m=m-col_number-1;
										 mflag++;
									 }

									 if (mflag!=0)
										 tempNet.from=m;          // mao add
									 else 
										 tempNet.from=FFlag;       // 不是模块上的pin，舍去调整模块时不考虑
						 }      
						 else
							 tempNet.from=m;
						 
						 
						 if(grids[n].m_id.size()==0)
						 {       
							 mflag=0;
							 if((n-1>=0)&&grids[n-1].m_id.size()!=0)
							 {
								 n=n-1;
								 mflag++;
							 }
							 else
								 if( ((n-col_number)>=0)&&grids[n-col_number].m_id.size()!=0)
								 {
									 n=n-col_number;
									 mflag++;
								 }   
								 else
									 if( ( (n-col_number-1)>=0 ) && (grids[n-col_number-1].m_id.size()!=0) )
									 {
										 n=n-col_number-1;
										 mflag++;
									 }
									 
									 if (mflag!=0)
										 tempNet.to=n;          // mao add
									 else 
										 tempNet.to=FFlag;
						 }      
						 else
							 tempNet.to=n;
						 
						 
						 if( (tempNet.from!=FFlag)||(tempNet.to!=FFlag) )
						 {
							 if(!IsIn(grids[(y1 + j)*col_number+(x1 + i-1)].net, tempNet))
								 grids[(y1 + j)*col_number+(x1 + i-1)].net.push_back(tempNet);

							 if(!IsIn(grids[(y1 + j+1)*col_number+(x1 + i-1)].net, tempNet))
								 grids[(y1 + j+1)*col_number+(x1 + i-1)].net.push_back(tempNet);	
						 }

#endif

					}
				}
				
				
				
				if( x1+i+1<=colNum && y1+j<rowNum ){//same as above
					hsId=coordinates2SegmentID(x1+i, y1+j, x1+i+1, y1+j);
					if(glbSegments[hsId].segment)
					{

#if 0
						if(grids[m].m_id.size()==0)
						{        
							mflag=0;
							if((m-1>=0)&&grids[m-1].m_id.size()!=0)
							{
								m=m-1;
								mflag++;
							}
							else
								if( ((m-col_number)>=0)&&grids[m-col_number].m_id.size()!=0)
								{
									m=m-col_number;
									mflag++;
								}   
								else
									if( ( (m-col_number-1)>=0 ) &&  (grids[m-col_number-1].m_id.size()!=0) )
									{
										m=m-col_number-1;
										mflag++;
									}

									if (mflag!=0)
										tempNet.from=m;          // mao add
									else 
										tempNet.from=FFlag;       // 不是模块上的pin，舍去调整模块时不考虑
						}      
						else
							tempNet.from=m;
						
						if(grids[n].m_id.size()==0)
						{        
							mflag=0;
							if((n-1>=0)&&grids[n-1].m_id.size()!=0)
							{
								n=n-1;
								mflag++;
							}
							else
								if( ((n-col_number)>=0)&&grids[n-col_number].m_id.size()!=0)
								{
									n=n-col_number;
									mflag++;
								}   
								else
									if( ( (n-col_number-1)>=0 ) && (grids[n-col_number-1].m_id.size()!=0) )
									{
										n=n-col_number-1;
										mflag++;
									}

									if (mflag!=0)
										tempNet.to=n;          // mao add
									else 
										tempNet.to=FFlag;
						}      
						else
							tempNet.to=n;
						
						
						if( (tempNet.from!=FFlag)||(tempNet.to!=FFlag) )
						{
							if(!IsIn(grids[(y1 + j)*col_number+(x1 + i-1)].net, tempNet))
								grids[(y1 + j)*col_number+(x1 + i-1)].net.push_back(tempNet);
							if(!IsIn(grids[(y1 + j)*col_number+(x1 + i+1-1)].net, tempNet))
								grids[(y1 + j)*col_number+(x1 + i+1-1)].net.push_back(tempNet);
						}
#endif 

						p_u_tracks_[hsId] += probHoriE(u, v, i, j);//水平边
					}
				}
      }
}
	}else{
		vsId=hsId=0;
		for(i=0; i<=u; i++){
			for(j=0; j<=v; j++){
				//printf("m=%d, n=%d\n", m, n);
				//printf("A\n");
				if( x1-i>0 && y1+j+1<rowNum){//liush x1 starts from 1 same as col
					vsId=coordinates2SegmentID( (x1 - i), (y1 + j), (x1 - i), (y1 + j + 1) );///liush modified 0818
					//printf("B\n");
					//printf("vsId=%d \n", vsId);
					if( glbSegments[vsId].segment ){

#   if 0
                         if(grids[m].m_id.size()==0)
						 {        
							 mflag=0;
							 if((m-1>=0)&&grids[m-1].m_id.size()!=0)
							 {
								 m=m-1;
								 mflag++;
							 }
							 else
								 if( ((m-col_number)>=0)&&grids[m-col_number].m_id.size()!=0)
								 {
									 m=m-col_number;
									 mflag++;
								 }   
								 else
									 if( ( (m-col_number-1)>=0 ) &&  (grids[m-col_number-1].m_id.size()!=0) )
									 {
										 m=m-col_number-1;
										 mflag++;
									 }
									 
									 if (mflag!=0)
										 tempNet.from=m;          // mao add
									 else 
										 tempNet.from=FFlag;       // 不是模块上的pin，舍去调整模块时不考虑
						 }      
						 else
							 tempNet.from=m;
						 
						 
						 if(grids[n].m_id.size()==0)
						 {        
							 mflag=0;
							 if((n-1>=0)&&grids[n-1].m_id.size()!=0)
							 {
								 n=n-1;
								 mflag++;
							 }
							 else
								 if( ((n-col_number)>=0)&&grids[n-col_number].m_id.size()!=0)
								 {
									 n=n-col_number;
									 mflag++;
								 }   
								 else
									 if( ( (n-col_number-1)>=0 ) && (grids[n-col_number-1].m_id.size()!=0) )
									 {
										 n=n-col_number-1;
										 mflag++;
									 }
									 
									 if (mflag!=0)
										 tempNet.to=n;          // mao add
									 else 
										 tempNet.to=FFlag;
						 }      
						 else
							 tempNet.to=n;
						 
						 if( (tempNet.from!=FFlag)||(tempNet.to!=FFlag) )
						 {
							 if(!IsIn(grids[(y1 + j)*col_number+(x1 - i-1)].net, tempNet))
								 grids[(y1 + j)*col_number+(x1 - i-1)].net.push_back(tempNet);
							 if(!IsIn(grids[(y1 + j+1)*col_number+(x1-i-1)].net, tempNet))
								 grids[(y1 + j+1)*col_number+(x1-i-1)].net.push_back(tempNet);
						 }

#endif

						//printf("*****Haha\n");
					//发现会有一些glbSegments的segments为空，比如180，182，184空，而181，183不空，不知为什么,因为超出了界
						//printf("vsId=%d \n",vsId);
						//printf("weight=%f", glbSegments[vsId].segment->weight);
						p_u_tracks_[vsId] += probVertE(u, v, i, j);//垂直边
					}
				}
				//printf("C\n");
				if( x1-i-1>0 && y1+j<rowNum ){//liush modified 0818; x1 starts from 1 same as col
					hsId=coordinates2SegmentID(x1-i, y1+j, x1-i-1, y1+j);//liush modified 0818
					//printf("D\n");
					if(glbSegments[hsId].segment)

					{

#if 0						
						if(grids[m].m_id.size()==0)
						{        
							mflag=0;
							if((m-1>=0)&&grids[m-1].m_id.size()!=0)
							{
								m=m-1;
								mflag++;
							}
							else
								if( ((m-col_number)>=0)&&grids[m-col_number].m_id.size()!=0)
								{
									m=m-col_number;
									mflag++;
								}   
								else
									if( ( (m-col_number-1)>=0 ) &&  (grids[m-col_number-1].m_id.size()!=0) )
									{
										m=m-col_number-1;
										mflag++;
									}
									
									if (mflag!=0)
										tempNet.from=m;          // mao add
									else 
										tempNet.from=FFlag;       // 不是模块上的pin，舍去调整模块时不考虑
						}      
						else
							tempNet.from=m;
						
						
						if(grids[n].m_id.size()==0)
						{        
							mflag=0;
							if((n-1>=0)&&grids[n-1].m_id.size()!=0)
							{
								n=n-1;
								mflag++;
							}
							else
								if( ((n-col_number)>=0)&&grids[n-col_number].m_id.size()!=0)
								{
									n=n-col_number;
									mflag++;
								}   
								else
									if( ( (n-col_number-1)>=0 ) && (grids[n-col_number-1].m_id.size()!=0) )
									{
										n=n-col_number-1;
										mflag++;
									}

									if (mflag!=0)
										tempNet.to=n;          // mao add
									else 
										tempNet.to=FFlag;
						}      
						else
							tempNet.to=n;
						
						if( (tempNet.from!=FFlag)||(tempNet.to!=FFlag) )
						{
							if(!IsIn(grids[(y1 + j)*col_number+(x1 - i-1)].net, tempNet))
								grids[(y1 + j)*col_number+(x1 - i-1)].net.push_back(tempNet);
						
							if(!IsIn(grids[(y1 + j)*col_number+(x1 -i-1-1)].net, tempNet))
								grids[(y1 + j)*col_number+(x1 -i-1-1)].net.push_back(tempNet);
						}
#endif

						p_u_tracks_[hsId] += probHoriE(u, v, i, j);//水平边
					//printf("E\n");
					}
				}
			}
		}
    }
    return;
}




/*
对线网n的占用资源估计
*/
void estiCongANet(long _netId)
{
	int i=0;
	for(i=0; i<msts_[_netId].nNode-1; i++){
		addProb2GRG(msts_[_netId].mstE[i].from, msts_[_netId].mstE[i].to); // 增加一个网参数作传递
	}
	return;
}

/*
FUNCTION :
    Refresh the wieght of a segment, according to its capacity and usage.
*/
#define LSHweightOfSegment( s, p_u )\
	(p_u == 0.0 )\
	? ( float )(( 0.03125 ) / ( (s)->s_tracks + 0.0625 )) \
	: ( float )((p_u + 0.0625 ) / ( (s)->s_tracks+0.0625 ))
#define LSHrefreshPWeightOfSegment( s , p_u)\
{\
	(s)->weight = LSHweightOfSegment( s,  p_u);\
}

void addPenalty2WeightOfSegment(float factor)
{
	int i;
	for(i=0; i<BHYNumberOfSegments( rowNum, colNum ); i++)
	{
		if(glbSegments[i].flag& BHYMaskSegmentFlagCongest)	
			glbSegments[i].segment->weight *=factor;
	}		
}
 

void pEstiCongestionInfoOfGRG(void){
	int	i;

	for(  i=0;  i<BHYNumberOfSegments( rowNum, colNum );  i++ )
	{
		LSHrefreshPWeightOfSegment( glbSegments[i].segment, p_u_tracks_[i]);
    }
	
}

/*
对每条线网的MST的每个树枝进行通过概率估计,并将结果赋到GRG图中Msegment::weight
*/
void estiCongAllNets()
{
    
   g_twoPins=0;        //mao add
   g_flatPins=0;       //mao add
   for(int i=0;i <grids.size(); i++)
	   grids[i].net.clear();          //清空vector


	int	i;
	int n;
	n=BHYNumberOfSegments( rowNum, colNum );
//	p_u_tracks_=alloc1DArray(sizeof(float),n);

	p_u_tracks_=(float*)malloc(sizeof(float)*n);
	if(p_u_tracks_==NULL)
	{
	  printf("p_u_tracks allocate error\n");
	  exit(-1);
	
	}
    
	for(i=0; i<n; i++)
	{
	    
		p_u_tracks_[i]=0;
	}
	for(  i = 0;  i < netNum;  i ++ )    //i <= netNum
	{
      /*
		if(( glbNets[i].netPtr != NULL )
			&&(( glbNets[i].nRequired > 1 )            
			||( glbNets[i].source.nodeID != glbNets[i].required[0].nodeID )))
		*/
		if(numbersets[i]>1)
		{
			estiCongANet(i);
		}
	}
//	printf("\nthe number of two-pin section and flat pins: %ld, %ld\n", g_twoPins, g_flatPins);
	pEstiCongestionInfoOfGRG();///added 0905 to give the segment weight.

	#ifdef LSH_PRINT_PRE_ESTI
	printSWeig();
	#endif
	if(p_u_tracks_!=NULL)
	free(p_u_tracks_);
	//free(p_u_tracks_);
	p_u_tracks_=NULL;
	return;
}

void printSWeig()
{
	int i;FILE *fp;
	if(!(fp=fopen("congW.f", "w"))) {
		fprintf(stderr,"can't open file mst.f\n");
		exit(1);
	}
	
	for(i=0;  i<BHYNumberOfSegments( rowNum, colNum );  i++){
		//fprintf(fp, "S %d  weight: %f\n", i, glbSegments[i].segment->weight );
		fprintf(fp, "S %d  P_u_track: %f\n", i, p_u_tracks_[i]);
		//fprintf(fp, "S %d  u_track: %d\n", i, glbSegments[i].segment->u_tracks );
	}	
	return;
}
