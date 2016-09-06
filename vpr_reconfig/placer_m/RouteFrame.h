/*
 * File: RouteFrame.h
 * Use: framework of all routing step, includes
 * (1): resource estimation --- RE_step
 * (2): bus routing -- BUS_step
 * (3): global routing --- GR_step
 * (4): layer assignment --- LA_step
 * (5): cross point assignment --- CPA_step
 * (6): detailed routing --- DR_step
 */

#ifndef ROUTEFRAME_H_
#define ROUTEFRAME_H_


# include <iostream>
using namespace std;

void  mfbRENewMain();
extern int RoutEstiMain();


#endif


