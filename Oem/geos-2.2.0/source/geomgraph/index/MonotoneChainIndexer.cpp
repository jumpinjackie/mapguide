/**********************************************************************
 * $Id: MonotoneChainIndexer.cpp,v 1.3 2004/07/08 19:34:49 strk Exp $
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
 * $Log: MonotoneChainIndexer.cpp,v $
 * Revision 1.3  2004/07/08 19:34:49  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added DefaultCoordinateSequenceFactory::instance() function.
 *
 * Revision 1.2  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.1  2004/04/14 06:04:26  ybychkov
 * "geomgraph/index" committ problem fixed.
 *
 * Revision 1.10  2004/03/19 09:49:29  ybychkov
 * "geomgraph" and "geomgraph/indexl" upgraded to JTS 1.4
 *
 * Revision 1.9  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 * Revision 1.8  2003/10/15 16:39:03  strk
 * Made Edge::getCoordinates() return a 'const' value. Adapted code set.
 *
 **********************************************************************/


#include <geos/geomgraphindex.h>

namespace geos {

vector<int>*
MonotoneChainIndexer::getChainStartIndices(const CoordinateSequence* pts)
{
	// find the startpoint (and endpoints) of all monotone chains in this edge
	int start=0;
	vector<int>* startIndexList=new vector<int>();
	startIndexList->push_back(start);
	do {
		int last=findChainEnd(pts,start);
		startIndexList->push_back(last);
		start=last;
	} while(start<(int)pts->getSize()-1);
	// copy list to an array of ints, for efficiency
	return startIndexList;
}

/**
* @return the index of the last point in the monotone chain
*/
int MonotoneChainIndexer::findChainEnd(const CoordinateSequence* pts,int start){
	// determine quadrant for chain
	int chainQuad=Quadrant::quadrant(pts->getAt(start),pts->getAt(start + 1));
	int last=start+1;
	while(last<(int)pts->getSize()) {
		// compute quadrant for next possible segment in chain
		int quad=Quadrant::quadrant(pts->getAt(last - 1),pts->getAt(last));
		if (quad!=chainQuad) break;
		last++;
	}
	return last-1;
}
}

