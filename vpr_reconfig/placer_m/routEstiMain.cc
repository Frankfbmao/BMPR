#include <stdio.h>
#include <stdio.h>
#include<iostream> 
#include <string.h>
#include <time.h>
using namespace std;

#include "routEsti.h"



#define SC_DESIGN_PATH  "Design"
#define BHYdebugAllMainPinInst	0x01



int RoutEstiMain()
{

	 
	char    mode;
    mode = 'w';
 	
	routEstimator(mode);

	return 0;
	
}
