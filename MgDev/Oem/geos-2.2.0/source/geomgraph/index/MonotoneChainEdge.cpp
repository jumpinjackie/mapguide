/**********************************************************************
 * $Id: MonotoneChainEdge.cpp,v 1.5 2004/11/23 19:53:06 strk Exp $
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
 **********************************************************************/

#include <geos/geomgraphindex.h>

namespace geos {

/**
 * MonotoneChains are a way of partitioning the segments of an edge to
 * allow for fast searching of intersections.
 * They have the following properties:
 * <ol>
 * <li>the segments within a monotone chain will never intersect each other
 * <li>the envelope of any contiguous subset of the segments in a monotone chain
 * is simply the envelope of the endpoints of the subset.
 * </ol>
 * Property 1 means that there is no need to test pairs of segments from within
 * the same monotone chain for intersection.
 * Property 2 allows
 * binary search to be used to find the intersection points of two monotone chains.
 * For many types of real-world data, these properties eliminate a large number of
 * segment comparisons, producing substantial speed gains.
 * @version 1.1
 */

MonotoneChainEdge::MonotoneChainEdge() {
	env1=new Envelope();
	env2=new Envelope();
	pts=NULL;
	startIndex=new vector<int>();
}

MonotoneChainEdge::~MonotoneChainEdge() {
	delete env1;
	delete env2;
//	delete pts;
	delete startIndex;
}

MonotoneChainEdge::MonotoneChainEdge(Edge *newE) {
	pts=newE->getCoordinates();
	env1=new Envelope();
	env2=new Envelope();
	e=newE;
	MonotoneChainIndexer *mcb=new MonotoneChainIndexer();
	startIndex=mcb->getChainStartIndices(pts);
	delete mcb;
}

const CoordinateSequence* MonotoneChainEdge::getCoordinates() {
	return pts;
}

vector<int>* MonotoneChainEdge::getStartIndexes() {
	return startIndex;
}

double MonotoneChainEdge::getMinX(int chainIndex){
	double x1=pts->getAt((*startIndex)[chainIndex]).x;
	double x2=pts->getAt((*startIndex)[chainIndex+1]).x;
	return x1<x2?x1:x2;
}

double MonotoneChainEdge::getMaxX(int chainIndex) {
	double x1=pts->getAt((*startIndex)[chainIndex]).x;
	double x2=pts->getAt((*startIndex)[chainIndex+1]).x;
	return x1>x2?x1:x2;
}

void MonotoneChainEdge::computeIntersects(MonotoneChainEdge *mce,SegmentIntersector *si){
	for(int i=0;i<(int)startIndex->size()-1;i++) {
		for(int j=0;j<(int)(mce->startIndex->size())-1;j++) {
			computeIntersectsForChain(i,mce,j,si);
		}
	}
}

void
MonotoneChainEdge::computeIntersectsForChain(int chainIndex0,
	MonotoneChainEdge *mce,int chainIndex1,SegmentIntersector *si)
{
	computeIntersectsForChain((*startIndex)[chainIndex0],(*startIndex)[chainIndex0+1],mce, (*(mce->startIndex))[chainIndex1],(*(mce->startIndex))[chainIndex1+1],si);
}

void
MonotoneChainEdge::computeIntersectsForChain(int start0, int end0,
	MonotoneChainEdge *mce, int start1, int end1, SegmentIntersector *ei)
{
	const Coordinate& p00=pts->getAt(start0);
	const Coordinate& p01=pts->getAt(end0);
	const Coordinate& p10=(mce->pts)->getAt(start1);
	const Coordinate& p11=(mce->pts)->getAt(end1);
	// terminating condition for the recursion
	if (end0-start0==1 && end1-start1==1) {
		ei->addIntersections(e, start0, mce->e, start1);
		return;
	}
	// nothing to do if the envelopes of these chains don't overlap
	env1->init(p00,p01);
	env2->init(p10,p11);
	if (!env1->intersects(env2)) return;
	// the chains overlap, so split each in half and iterate  (binary search)
	int mid0=(start0+end0)/2;
	int mid1=(start1+end1)/2;
	// Assert: mid != start or end (since we checked above for end - start <= 1)
	// check terminating conditions before recursing
	if (start0<mid0) {
		if (start1<mid1) computeIntersectsForChain(start0,mid0,mce,start1,mid1,ei);
		if (mid1<end1) computeIntersectsForChain(start0,mid0,mce,mid1,end1,ei);
	}
	if (mid0<end0) {
		if (start1<mid1) computeIntersectsForChain(mid0,end0,mce,start1,mid1,ei);
		if (mid1<end1) computeIntersectsForChain(mid0,end0,mce,mid1,end1,ei);
	}
}

} // namespace geos

/**********************************************************************
 * $Log: MonotoneChainEdge.cpp,v $
 * Revision 1.5  2004/11/23 19:53:06  strk
 * Had LineIntersector compute Z by interpolation.
 *
 * Revision 1.4  2004/10/20 17:32:14  strk
 * Initial approach to 2.5d intersection()
 *
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
 * Revision 1.17  2004/03/19 09:49:29  ybychkov
 * "geomgraph" and "geomgraph/indexl" upgraded to JTS 1.4
 *
 * Revision 1.16  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 * Revision 1.15  2003/10/15 16:39:03  strk
 * Made Edge::getCoordinates() return a 'const' value. Adapted code set.
 *
 **********************************************************************/

