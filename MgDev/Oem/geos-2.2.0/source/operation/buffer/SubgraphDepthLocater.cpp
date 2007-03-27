/**********************************************************************
 * $Id: SubgraphDepthLocater.cpp,v 1.5.2.4 2005/06/30 18:31:21 strk Exp $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 * Copyright (C) 2005 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/opBuffer.h>

namespace geos {

SubgraphDepthLocater::SubgraphDepthLocater(vector<BufferSubgraph*> *newSubgraphs){
	seg=new LineSegment();
	cga=new RobustCGAlgorithms();
	subgraphs=newSubgraphs;
}

SubgraphDepthLocater::~SubgraphDepthLocater(){
	delete seg;
	delete cga;
}

int
SubgraphDepthLocater::getDepth(Coordinate &p)
{
	vector<DepthSegment*> *stabbedSegments=findStabbedSegments(p);
	// if no segments on stabbing line subgraph must be outside all others->
	if ((int)stabbedSegments->size()==0)
	{
		delete stabbedSegments;
		return 0;
	}
	sort(stabbedSegments->begin(),stabbedSegments->end(),DepthSegmentLT);
	DepthSegment *ds=(*stabbedSegments)[0];
	int ret = ds->leftDepth;

	vector<DepthSegment *>::iterator it;
	for (it=stabbedSegments->begin(); it != stabbedSegments->end(); it++)
		delete *it;
	delete stabbedSegments;

	return ret;
}

/**
 * Finds all non-horizontal segments intersecting the stabbing line
 * The stabbing line is the ray to the right of stabbingRayLeftPt
 *
 * @param stabbingRayLeftPt the left-hand origin of the stabbing line
 * @return a List of DepthSegments intersecting the stabbing line
 */
vector<DepthSegment*>*
SubgraphDepthLocater::findStabbedSegments(Coordinate &stabbingRayLeftPt)
{
	vector<DepthSegment*> *stabbedSegments=new vector<DepthSegment*>();
	unsigned int size = subgraphs->size();
	for (unsigned int i=0; i<size; ++i)
	{
		BufferSubgraph *bsg=(*subgraphs)[i];

		// optimization - don't bother checking subgraphs
		// which the ray does not intersect
		Envelope *env = bsg->getEnvelope();
		if ( stabbingRayLeftPt.y < env->getMinY()
			|| stabbingRayLeftPt.y > env->getMaxY() )
				continue;

		findStabbedSegments(stabbingRayLeftPt, bsg->getDirectedEdges(), stabbedSegments);
	}
	return stabbedSegments;
}

/**
* Finds all non-horizontal segments intersecting the stabbing line
* in the list of dirEdges->
* The stabbing line is the ray to the right of stabbingRayLeftPt->
*
* @param stabbingRayLeftPt the left-hand origin of the stabbing line
* @param stabbedSegments the current list of {@link DepthSegments} intersecting the stabbing line
*/
void SubgraphDepthLocater::findStabbedSegments(Coordinate &stabbingRayLeftPt,vector<DirectedEdge*> *dirEdges,vector<DepthSegment*> *stabbedSegments){
	/**
	* Check all forward DirectedEdges only->  This is still general,
	* because each Edge has a forward DirectedEdge->
	*/
	for(int i=0;i<(int)dirEdges->size();i++) {
		DirectedEdge *de=(*dirEdges)[i];
		if (!de->isForward())
			continue;
		findStabbedSegments(stabbingRayLeftPt, de, stabbedSegments);
	}
}

/**
 * Finds all non-horizontal segments intersecting the stabbing line
 * in the input dirEdge->
 * The stabbing line is the ray to the right of stabbingRayLeftPt->
 *
 * @param stabbingRayLeftPt the left-hand origin of the stabbing line
 * @param stabbedSegments the current list of DepthSegments
 * intersecting the stabbing line
 */
void
SubgraphDepthLocater::findStabbedSegments(Coordinate &stabbingRayLeftPt,DirectedEdge *dirEdge,vector<DepthSegment*> *stabbedSegments)
{
	const CoordinateSequence *pts=dirEdge->getEdge()->getCoordinates();

// It seems that LineSegment is *very* slow... undef this
// to see yourself
#define SKIP_LS 1

	int n = pts->getSize()-1;
	for (int i=0; i<n; ++i) {
#ifndef SKIP_LS
		seg->p0=pts->getAt(i);
		seg->p1=pts->getAt(i + 1);
#else

		const Coordinate *low=&(pts->getAt(i));
		const Coordinate *high=&(pts->getAt(i+1));
		const Coordinate *swap=NULL;

#endif

#if DEBUG
	cerr<<" SubgraphDepthLocater::findStabbedSegments: segment "<<i<<" ("<<seg->toString()<<") ";
#endif

#ifndef SKIP_LS
		// ensure segment always points upwards
		//if (seg->p0.y > seg->p1.y)
		{
			seg->reverse();
#if DEBUG
			cerr<<" reverse ("<<seg->toString()<<") ";
#endif
		}
#else
		if (low->y > high->y)
		{
			swap=low;
			low=high;
			high=swap;
		}
#endif

		// skip segment if it is left of the stabbing line
		// skip if segment is above or below stabbing line
#ifndef SKIP_LS
		double maxx=max(seg->p0.x, seg->p1.x);
#else
		double maxx=max(low->x, high->x);
#endif
		if (maxx < stabbingRayLeftPt.x)
		{
#if DEBUG
			cerr<<" segment is left to stabbing line, skipping "<<endl;
#endif
			continue;
		}

		// skip horizontal segments (there will be a non-horizontal
		// one carrying the same depth info
#ifndef SKIP_LS
		if (seg->isHorizontal())
#else
		if (low->y == high->y)
#endif
		{
#if DEBUG
			cerr<<" segment is horizontal, skipping "<<endl;
#endif
			continue;
		}

		// skip if segment is above or below stabbing line
#ifndef SKIP_LS
		if (stabbingRayLeftPt.y < seg->p0.y ||
			stabbingRayLeftPt.y > seg->p1.y)
#else
		if (stabbingRayLeftPt.y < low->y ||
			stabbingRayLeftPt.y > high->y)
#endif
		{
#if DEBUG
			cerr<<" segment above or below stabbing line, skipping "<<endl;
#endif
			continue;
		}

		// skip if stabbing ray is right of the segment
#ifndef SKIP_LS
		if (CGAlgorithms::computeOrientation(seg->p0, seg->p1,
#else
		if (CGAlgorithms::computeOrientation(*low, *high,
#endif
				stabbingRayLeftPt)==CGAlgorithms::RIGHT)
		{
#if DEBUG
			cerr<<" stabbing ray right of segment, skipping"<<endl;
#endif
			continue;
		}

#ifndef SKIP_LS
		// stabbing line cuts this segment, so record it
		int depth=dirEdge->getDepth(Position::LEFT);
		// if segment direction was flipped, use RHS depth instead
		if (! (seg->p0==pts->getAt(i)))
			depth=dirEdge->getDepth(Position::RIGHT);
#else
		int depth = swap ?
			dirEdge->getDepth(Position::RIGHT)
			:
			dirEdge->getDepth(Position::LEFT);
#endif

#if DEBUG
	cerr<<" depth: "<<depth<<endl;
#endif

#ifdef SKIP_LS
		seg->p0 = *low;
		seg->p1 = *high;
#endif
		DepthSegment *ds=new DepthSegment(seg, depth);
		stabbedSegments->push_back(ds);
	}
}

DepthSegment::DepthSegment(LineSegment *seg, int depth){
	// input seg is assumed to be normalized
	upwardSeg=new LineSegment(*seg);
	//upwardSeg->normalize();
	leftDepth=depth;
}

DepthSegment::~DepthSegment(){
	delete upwardSeg;
}
/**
* Defines a comparision operation on DepthSegments
* which orders them left to right
*
* <pre>
* DS1 < DS2   if   DS1->seg is left of DS2->seg
* DS1 > DS2   if   DS1->seg is right of DS2->seg
* </pre>
*
* @param obj
* @return
*/
int DepthSegment::compareTo(void* obj){
	DepthSegment *other=(DepthSegment*) obj;
	/**
	* try and compute a determinate orientation for the segments->
	* Test returns 1 if other is left of this (i->e-> this > other)
	*/
	int orientIndex=upwardSeg->orientationIndex(other->upwardSeg);
	/**
	* If comparison between this and other is indeterminate,
	* try the opposite call order->
	* orientationIndex value is 1 if this is left of other,
	* so have to flip sign to get proper comparison value of
	* -1 if this is leftmost
	*/
	if (orientIndex==0)
		orientIndex=-1 * other->upwardSeg->orientationIndex(upwardSeg);
	// if orientation is determinate, return it
	if (orientIndex != 0)
		return orientIndex;
	// otherwise, segs must be collinear - sort based on minimum X value
	return compareX(upwardSeg, other->upwardSeg);
}

/**
* Compare two collinear segments for left-most ordering->
* If segs are vertical, use vertical ordering for comparison->
* If segs are equal, return 0->
* Segments are assumed to be directed so that the second coordinate is >= to the first
* (e->g-> up and to the right)->
*
* @param seg0 a segment to compare
* @param seg1 a segment to compare
* @return
*/
int DepthSegment::compareX(LineSegment *seg0, LineSegment *seg1){
	int compare0=seg0->p0.compareTo(seg1->p0);
	if (compare0!=0)
		return compare0;
	return seg0->p1.compareTo(seg1->p1);

}

bool DepthSegmentLT(DepthSegment *first, DepthSegment *second) {
	if (first->compareTo(second)<0)
		return true;
	else
		return false;
}

} // geos

/**********************************************************************
 * $Log: SubgraphDepthLocater.cpp,v $
 * Revision 1.5.2.4  2005/06/30 18:31:21  strk
 * Ported SubgraphDepthLocator optimizations from JTS code
 *
 * Revision 1.5.2.3  2005/06/28 21:13:54  strk
 * Fixed a bug introduced by LineSegment skip - made LineSegment skip a compile-time option
 *
 * Revision 1.5.2.2  2005/06/27 21:55:44  strk
 * Bug fix in DepthSegmentLT comparator as suggested by Graeme Hiebert
 *
 * Revision 1.5.2.1  2005/06/27 21:29:00  strk
 * Reduced Coordinate copies due to LineSegment overuse
 *
 * Revision 1.5  2004/07/08 19:34:49  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added DefaultCoordinateSequenceFactory::instance() function.
 *
 * Revision 1.4  2004/07/02 13:28:28  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.3  2004/05/05 12:29:44  strk
 * memleak fixed in ::getDepth
 *
 * Revision 1.2  2004/05/03 22:56:44  strk
 * leaks fixed, exception specification omitted.
 *
 * Revision 1.1  2004/04/10 08:40:01  ybychkov
 * "operation/buffer" upgraded to JTS 1.4
 *
 *
 **********************************************************************/

