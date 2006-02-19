/**********************************************************************
 * $Id: SweepLineInterval.cpp,v 1.6 2004/07/02 13:28:27 strk Exp $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 * $Log: SweepLineInterval.cpp,v $
 * Revision 1.6  2004/07/02 13:28:27  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.5  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/


#include <geos/indexSweepline.h>
#include <stdio.h>

namespace geos {

SweepLineInterval::SweepLineInterval(double newMin, double newMax) {
	SweepLineInterval(newMin,newMax,NULL);
}

SweepLineInterval::SweepLineInterval(double newMin, double newMax, void* newItem){
	min=newMin<newMax?newMin:newMax;
	max=newMax>newMin?newMax:newMin;
	item=newItem;
}

double SweepLineInterval::getMin() {
	return min;
}
double SweepLineInterval::getMax() {
	return max;
}
void* SweepLineInterval::getItem() {
	return item;
}
}

