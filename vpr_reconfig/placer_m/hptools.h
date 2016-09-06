
#ifndef DEF_hptools
#define DEF_hptools

#ifdef BHYdebug 
#ifndef _Windows
#ifdef BHYoldANSI
void
checkHeap();
#else
void checkHeap( char* );
#endif
#define viewHeap( s ) checkHeap( s )

#else
#define viewHeap( s )
#endif
#else
#define viewHeap( s )
#endif

#endif

