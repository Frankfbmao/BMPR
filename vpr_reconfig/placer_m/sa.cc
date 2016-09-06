// Project: B*-trees floorplanning
// Advisor: Yao-Wen Chang  <ywchang@cis.nctu.edu.tw>
// Authors: Jer-Ming Hsu   <barz@i.cis.nctu.edu.tw>
// 	    Hsun-Cheng Lee <gis88526@cis.nctu.edu.tw>
// Sponsor: Arcadia Inc.
// Date:    7/19/2000 ~

//---------------------------------------------------------------------------
#include <cmath>
//#include "sa.h"
#include <iostream>
#include <cassert>
#include <iomanip>
#include "fplan.h"
#include "sa.h"
using namespace std;
//---------------------------------------------------------------------------
double init_avg = 0.00001;
int hill_climb_stage = 7;
double avg_ratio=150;
double lamda=1.3;
bool forgeFlag = false;
double P=0.9;


double mean(vector<double> &chain)
{
  double sum=0;
  for(int i=0; i < chain.size();i++)
     sum+= chain[i];
  return sum/chain.size();
}

double var(vector<double> &chain)
{
  double m = mean(chain);
  double sum=0;
  double var;
  int N= chain.size();

  for(int i=0; i < N;i++)
     sum += (chain[i]-m)*(chain[i]-m);

  var = sum/(N);
  printf("  m=%.4f ,v=%.4f\n",m,var);

  return var;
}

/* Simulated Annealing B*Tree Floorplan
   k: factor of the number of permutation in one temperature
   local: local search iterations
   termT: terminating temperature
*/
double SA_Floorplan(FPlan &fp, int k, int local/*=0*/, double term_T/*=0.1*/)
{
  int MT,uphill,reject;
  double pre_cost,best,cost;
  float d_cost,reject_rate=0;

  double pre_congValue ;   //mao add 
  double congValue;

  int N = k * fp.size();

  //add 
  if(0 != timesformsize)  //if  timesformsize ==0, sa para donnot adaptive consider modulesize
  {                         //timesformsize ==1, sa para adadptive consider module size
      int virtualmodulesize = fp.size() * fp.getmaxnumratio();
      if(virtualmodulesize > sabigmodulesize) // default 80)
		  N = (int)(k * fp.size() * sacoefficienta); // default 2);
	  else if (virtualmodulesize < sasmallmodulesize) //default 50)
		  N = (int)(k * fp.size() * sacoefficientb); //default 0.5); 
  }

  //float P=0.9;
  float T,actual_T=1;
  double avg=init_avg;
  float conv_rate = 0.99;
  double time; 
  double time_start;

  //double estimate_avg = 0.08 / avg_ratio;
  //cout << "Estimate Average Delta Cost = " << estimate_avg << endl;
/*
  if(local==0)
    avg = estimate_avg;
*/    

  avg = Random_Floorplan( fp,N);
  avg_ratio = avg;
  //avg = 0.05;
  //avg = 2e-6;
  cout << "Average delta cost = " << avg;
  //estimate_avg = avg;  
  
  time = time_start = seconds();


  T = avg / log(1/P);
  actual_T = T;

  cout << "Intial T = " << T << endl;
  // get inital solution
  fp.packing();
  fp.keep_sol();  
  fp.keep_best();
  pre_cost =  best = fp.getCost();
  
  int good_num=0,bad_num=0;
  double total_cost=0;
  int count=0;
  float p_avg = 0;
  float dcost_avg = 0;
  int bad_count=0;
  int good_count = 0;  
  int itcount = 0;
  //ofstream of("btree_debug");
  //ofstream of2("btree_debug_2");
  //ofstream of3("btree_debug_3");

  cout << setiosflags( ios::fixed );
  //of2 << setiosflags( ios::fixed );
  //of3 << setiosflags( ios::fixed );

  bool first_feasible = false;

  do{
   count++;
   total_cost = 0;
   MT=uphill=reject=0;
   printf("Iteration %d ", count);
   cout << " T= " << T << endl;
   
   vector<double> chain; 
   good_count = 0;
   p_avg = 0;
   dcost_avg = 0;
   
   for(; uphill < N && MT < 2*N; MT++)
   {
       fp.perturb();
       fp.packing();
       cost = fp.getCost(); 
       d_cost = cost - pre_cost;
       float p = 1/(exp(d_cost/T));

       p = p>1? 1:p;
       
	   itcount++;
	   p_avg+=p;  // observ
       dcost_avg += fabs(d_cost); // observ

       chain.push_back(cost);

       if(d_cost <=0 || rand_01() < p )
	   {

         fp.keep_sol();
         pre_cost = cost;

         if(d_cost > 0)
		 {       
           uphill++, bad_num++;
           //of << d_cost << ": " << p << endl;
         }
		 else  
			 if(d_cost < 0)  
				 good_num++;

         // keep best solution/count
         if(cost < best)
		 {
             bool feasible = fp.isFit();  //if donot consider fixed-outline. isFit is alway 0 (false??)

			 ////fbmao add     Each time, the solution is legal
			 // check which step, need to legalize the result
			 //bool isHeterResEnough()

            if( first_feasible == false)
            {
                fp.keep_best();
                best = cost;
            }
            else
            {
                if( feasible )
                {
                    fp.keep_best();
                    best = cost;
                }
            }

            if( feasible )
                first_feasible = true;

		    printf("   ==>  Cost= %f, Area= %.6f (%.2f%%), Wire= %.3f, ar= %.2f, fit= %d, time= %.1f\n", 
			    best, 
			    fp.getArea()*1e-6, 
			    fp.getDeadSpace(),
                fp.getWireLength()*1e-3,
                fp.getAR(), fp.isFit(),
                seconds()-time_start );

            assert(fp.getArea() >= fp.getTotalArea());
            time = seconds();
	        good_count++;

			{
		      string tmpdrawname = tmpoutfile + ".draw";
			  string tmppltname = tmpoutfile + ".plt";
			  fp.outDraw(tmpdrawname.c_str());
			  fp.gnuplotDraw(tmppltname.c_str());

			  //for original draw. Different from the draw function for heterogeneous arch
			  string noWidthUpdateTmpdrawname = tmpoutfile + ".nowidthchange.draw";
			  fp.noWidthUpdateOutDraw(noWidthUpdateTmpdrawname.c_str());
		    //   fp.outDraw( "tmp.draw" );
		 	//fp.gnuplotDraw("tmp.plt");
			}

		    //of2 << time-time_start << "\t" << fp.getDeadSpace() << endl;
	   
         }
       }
       else
	   {
         reject++;
         fp.recover();
       }
   }
   if( good_count == 0 )
       bad_count++;
   else 
       bad_count = 0;
   
   //p_avg/= MT;
   //dcost_avg /=MT;

   p_avg/= itcount;
   dcost_avg /=itcount;

//   cout << T << endl;
   double sv = var(chain);

   cout << "Average accept rate " << p_avg;
   cout << " Average dcost:sv " << dcost_avg/sv << " " << dcost_avg << " " << sv << endl;   

   // record T before cooling    
   reject_rate = float(reject)/MT;

   float r_t = exp(-T/sv); 
   //double r_t = 50*T*T*T/sv;

  /*   
   if(count > local){
     conv_rate = 0.99;

   }
  */ 
/*   
   if( reject_rate <= 0.1 && !forgeFlag)
   {		 
	   actual_T = T; 
           forgeFlag = true;
	   cout << "record last temp !! " << endl;	   

   }
 
   if( reject_rate > conv_rate && forgeFlag)
   {
	   actual_T /=2; 
	   T = actual_T;
           forgeFlag = false;	
	   cout << "forging !! BANG!! T is now " << T<<endl;
	   reject_rate = 0;
   }else */
     
   //of3 << count << "\t" << T << "\t" << reject_rate << endl;
       
   //T = actual_T/((float)count/sv);             
   
   //T = r_t * T;
   if( count < local )
   {
	   if(1 == io3) //use the port-to-port wirelength estimation
		   T = T * 0.2;
	   else
	     T =(actual_T/100)/((double)count/dcost_avg);  
   }
   else
   {
	   if(1 == io3)
		   T = T * 0.1;
	   else
  	      T =actual_T/((double)count/dcost_avg);  
   }

   printf("  Intial_T= %.2f, r= %f, reject= %.2f, next_T = %f\n\n", actual_T, r_t, reject_rate,T);


   // After apply local-search, start to use normal SA
   /*
   if(count == local){
     T = estimate_avg/ log(P);
     T *= pow(0.9,local);		// smothing the annealing schedule
     actual_T = exp(estimate_avg/T);
   }
   */


   //here io3 1: means wirelength using the port-to-port approach
   //if using the port-to-port approach, control the iteration time to be 10 times
   //if using the center-to-center(modules' center) approach, io3 is 0, so donont control iteration time

  }while( ((reject_rate<conv_rate && T>1e-4 ) || bad_count<=3) && (!io3 || count < 10)); ///2));  // for test 
                                                                                  
  if(reject_rate >= conv_rate)
    cout << "\n  Convergent!\n";
  else if(T <= 1e-4)
    cout << "\n Cooling Enough!\n";
  else
    cout << "\n Bad count is "<<bad_count<< " T= "<< T;
  printf("\n good = %d, bad=%d\n\n", good_num, bad_num);

  fp.recover_best();
  fp.packing();
  return time; 
}

double Random_Floorplan(FPlan &fp,int times)
{
  int N =times,t=0;
  double total_cost=0,pre_cost,cost,best;

  fp.packing();
  pre_cost = best = fp.getCost();
  fp.keep_best();

 do
 {
  for(int i=0; i < N;i++)
  {
    fp.perturb();
    fp.packing();
  
    cost = fp.getCost();
    if(cost-pre_cost > 0)
	{
      t++;
      total_cost += cost-pre_cost;
      pre_cost = cost;
    }

    if(cost < best)
	{
      //fp.keep_best();
      best = cost;
      cout << "==> Cost=" << best << endl;
    }
  }	
 }while(total_cost==0);

  fp.recover_best();
  fp.packing();

  total_cost /= t;
  return total_cost;
}

