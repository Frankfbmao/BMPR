#include <malloc.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include<iostream> 
using namespace std;


#include "sttools.h"





void randomize()
{
	srand( time( NULL ));
}



int bhy_random(
	int	range )

{
	return (int)( rand() % range );
}







