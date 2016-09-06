
#ifndef DEF_sttools
#define DEF_sttools

#include <stdio.h>



#include <limits.h>
#include <float.h>
#define	MAXINT INT_MAX
#define MAXFLOAT FLT_MAX
#define min( x, y ) (((x)<=(y))?(x):(y))
#define max( x, y ) (((x)>=(y))?(x):(y))
#define balloc( name, num )  ( name * ) malloc( sizeof( name )*( num ))

#define brealloc( p, name, num ) \
	((p==NULL)\
	? balloc( name, num )\
	: (name*) realloc(p,sizeof(name)*(num) ))

void	randomize();
int bhy_random(	int	range );


#endif





