/**********************************************************************
 * $Id: Edge.cpp,v 1.10 2004/12/08 13:54:43 strk Exp $
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

#include <geos/geomgraph.h>

//#define DEBUG_INTERSECT 0
#ifndef DEBUG
#define DEBUG 0
#endif

namespace geos {

/**
 * Updates an IM from the label for an edge.
 * Handles edges from both L and A geometrys.
 */
void Edge::updateIM(Label *lbl,IntersectionMatrix *im){
	im->setAtLeastIfValid(lbl->getLocation(0,Position::ON),lbl->getLocation(1,Position::ON),1);
	if (lbl->isArea()) {
		im->setAtLeastIfValid(lbl->getLocation(0,Position::LEFT),lbl->getLocation(1,Position::LEFT),2);
		im->setAtLeastIfValid(lbl->getLocation(0,Position::RIGHT),lbl->getLocation(1,Position::RIGHT),2);
	}
}

Edge::Edge(){
	//cerr<<"["<<this<<"] Edge()"<<endl;
	eiList=NULL;
	isIsolatedVar=true;
	depth=NULL;
	depthDelta=0;
	mce=NULL;
	pts=NULL;
	env=NULL;
}

Edge::~Edge(){
	//cerr<<"["<<this<<"] ~Edge()"<<endl;
	delete eiList;
	delete depth;
	delete mce;
	delete pts;
	delete env;
}

Edge::Edge(CoordinateSequence* newPts, Label *newLabel):GraphComponent(newLabel){
	//cerr<<"["<<this<<"] Edge(CoordinateSequence *, Label *)"<<endl;
	eiList=new EdgeIntersectionList(this);
	isIsolatedVar=true;
	depth=new Depth();
	depthDelta=0;
//	label=newLabel;
	mce=NULL;
	pts=newPts;
	env=NULL;
}

Edge::Edge(CoordinateSequence* newPts){
	//cerr<<"["<<this<<"] Edge(CoordinateSequence *)"<<endl;
	eiList=new EdgeIntersectionList(this);
	isIsolatedVar=true;
	depth=new Depth();
	depthDelta=0;
	label=NULL;
	mce=NULL;
	pts=newPts;
	env=NULL;
}

int Edge::getNumPoints() {
	return pts->getSize();
}

void Edge::setName(string newName) {
	name=newName;
}

//CoordinateSequence Edge::getCoordinates(){
//	return pts;
//}

const CoordinateSequence* Edge::getCoordinates() const {
	return pts;
}

const
Coordinate& Edge::getCoordinate(int i)
{
	return pts->getAt(i);
}

// const or new return value ??
const
Coordinate& Edge::getCoordinate()
{
	if (pts->getSize()>0) return pts->getAt(0);
	auto_ptr<const Coordinate> ret(new Coordinate(DoubleNotANumber,DoubleNotANumber,DoubleNotANumber));
	return *ret;
}

Depth*
Edge::getDepth()
{
	return depth;
}

/*
 * The depthDelta is the change in depth as an edge is crossed from R to L
 * @return the change in depth as the edge is crossed from R to L
 */
int
Edge::getDepthDelta()
{
	return depthDelta;
}

void
Edge::setDepthDelta(int newDepthDelta)
{
	depthDelta=newDepthDelta;
}

int
Edge::getMaximumSegmentIndex()
{
	return pts->getSize()-1;
}

EdgeIntersectionList*
Edge::getEdgeIntersectionList()
{
	return eiList;
}

MonotoneChainEdge*
Edge::getMonotoneChainEdge()
{
	if (mce==NULL) mce=new MonotoneChainEdge(this);
	return mce;
}

bool
Edge::isClosed()
{
	return pts->getAt(0)==pts->getAt(pts->getSize()-1);
}

/*
 * An Edge is collapsed if it is an Area edge and it consists of
 * two segments which are equal and opposite (eg a zero-width V).
 */
bool
Edge::isCollapsed()
{
	if (!label->isArea()) return false;
	if (pts->getSize()!= 3) return false;
	if (pts->getAt(0)==pts->getAt(2) ) return true;
	return false;
}

Edge*
Edge::getCollapsedEdge()
{
	CoordinateSequence *newPts = new DefaultCoordinateSequence(2);
	newPts->setAt(pts->getAt(0),0);
	newPts->setAt(pts->getAt(1),1);
	return new Edge(newPts, Label::toLineLabel(label));
}

void
Edge::setIsolated(bool newIsIsolated)
{
	isIsolatedVar=newIsIsolated;
}

bool
Edge::isIsolated()
{
	return isIsolatedVar;
}

/*
 * Adds EdgeIntersections for one or both
 * intersections found for a segment of an edge to the edge intersection list.
 */
void
Edge::addIntersections(LineIntersector *li, int segmentIndex, int geomIndex)
{
#if DEBUG
	cerr<<"["<<this<<"] Edge::addIntersections("<<li->toString()<<", "<<segmentIndex<<", "<<geomIndex<<") called"<<endl;
#endif
	for (int i=0; i<li->getIntersectionNum();i++) {
		addIntersection(li,segmentIndex,geomIndex,i);
	}
}

/*
 * Add an EdgeIntersection for intersection intIndex.
 * An intersection that falls exactly on a vertex of the edge is normalized
 * to use the higher of the two possible segmentIndexes
 */
void
Edge::addIntersection(LineIntersector *li,int segmentIndex,int geomIndex,int intIndex)
{
#if DEBUG
	cerr<<"["<<this<<"] Edge::addIntersection("<<li->toString()<<", "<<segmentIndex<<", "<<geomIndex<<", "<<intIndex<<") called"<<endl;
#endif
	const Coordinate& intPt=li->getIntersection(intIndex);
	int normalizedSegmentIndex=segmentIndex;
	double dist=li->getEdgeDistance(geomIndex,intIndex);

	// normalize the intersection point location
	int nextSegIndex=normalizedSegmentIndex+1;
	if (nextSegIndex<pts->getSize()) {
		const Coordinate& nextPt=pts->getAt(nextSegIndex);
        // Normalize segment index if intPt falls on vertex
        // The check for point equality is 2D only - Z values are ignored
		if (intPt.equals2D(nextPt)) {
			normalizedSegmentIndex=nextSegIndex;
			dist=0.0;
		}
	}

	/*
	 * Add the intersection point to edge intersection list.
	 */
#if DEBUG
	cerr<<"Edge::addIntersection adding to edge intersection list point "<<intPt.toString()<<endl;
#endif
	//EdgeIntersection *ei=eiList->add(intPt,normalizedSegmentIndex,dist);
	eiList->add(intPt,normalizedSegmentIndex,dist);
}

/*
 * Update the IM with the contribution for this component.
 * A component only contributes if it has a labelling for both parent geometries
 */
void
Edge::computeIM(IntersectionMatrix *im)
{
	updateIM(label,im);
}

/*
 * equals is defined to be:
 * 
 * e1 equals e2
 * <b>iff</b>
 * the coordinates of e1 are the same or the reverse of the coordinates in e2
 */
bool operator==(Edge e1, Edge e2)
{
	if (e1.pts->getSize()!=e2.pts->getSize()) return false;
	bool isEqualForward=true;
	bool isEqualReverse=true;
	int iRev=e1.pts->getSize();
	for (int i=0; i<e1.pts->getSize();i++) {
		if (!e1.pts->getAt(i).equals2D(e2.pts->getAt(i))) {
			isEqualForward=false;
		}
		if (!e1.pts->getAt(i).equals2D(e2.pts->getAt(--iRev))) {
			isEqualReverse=false;
		}
		if (!isEqualForward && !isEqualReverse) return false;
	}
	return true;
}

/*
 * equals is defined to be:
 * <p>
 * e1 equals e2
 * <b>iff</b>
 * the coordinates of e1 are the same or the reverse of the coordinates in e2
 */
bool Edge::equals(Edge *e){
	if (pts->getSize()!=e->pts->getSize()) return false;
	bool isEqualForward=true;
	bool isEqualReverse=true;
	int iRev=pts->getSize();
	for (int i=0; i<pts->getSize();i++) {
		if (!pts->getAt(i).equals2D(e->pts->getAt(i))) {
			isEqualForward=false;
		}
		if (!pts->getAt(i).equals2D(e->pts->getAt(--iRev))) {
			isEqualReverse=false;
		}
		if (!isEqualForward && !isEqualReverse) return false;
	}
	return true;
}

/*
 * @return true if the coordinate sequences of the Edges are identical
 */
bool
Edge::isPointwiseEqual(Edge *e)
{
#if DEBUG > 2
	cerr<<"Edge::isPointwiseEqual call"<<endl;
#endif
	if (pts->getSize()!=e->pts->getSize()) return false;
#if DEBUG
	cerr<<"Edge::isPointwiseEqual scanning "<<e->pts->getSize()<<"x"<<pts->getSize()<<" points"<<endl;
#endif
	for (int i=0;i<pts->getSize();i++) {
		if (!pts->getAt(i).equals2D(e->pts->getAt(i))) {
			return false;
		}
	}
	return true;
}

string Edge::print(){
	string out="edge " + name + ": ";
	out+="LINESTRING (";
	for(int i=0; i<pts->getSize();i++) {
		if (i>0) out+=",";
		out+=pts->getAt(i).toString();
	}
	out+=")  ";
	out+=label->toString();
	out+=" ";
	out+=depthDelta;
	return out;
}
  
string Edge::printReverse(){
	string out="edge " + name + ": ";
	for(int i=pts->getSize()-1;i>=0;i--) {
		out+=pts->getAt(i).toString() + " ";
	}
	out+="\n";
	return out;
}

Envelope* Edge::getEnvelope(){
	// compute envelope lazily
	if (env==NULL) {
		env=new Envelope();
		for (int i = 0; i<pts->getSize(); i++) {
			env->expandToInclude(pts->getAt(i));
	}
	}
	return env;
}
}

/**********************************************************************
 * $Log: Edge.cpp,v $
 * Revision 1.10  2004/12/08 13:54:43  strk
 * gcc warnings checked and fixed, general cleanups.
 *
 * Revision 1.9  2004/11/23 19:53:06  strk
 * Had LineIntersector compute Z by interpolation.
 *
 * Revision 1.8  2004/11/22 12:59:25  strk
 * Added debugging lines
 *
 * Revision 1.7  2004/11/01 16:43:04  strk
 * Added Profiler code.
 * Temporarly patched a bug in DoubleBits (must check drawbacks).
 * Various cleanups and speedups.
 *
 * Revision 1.6  2004/10/20 17:32:14  strk
 * Initial approach to 2.5d intersection()
 *
 * Revision 1.5  2004/07/21 09:55:24  strk
 * CoordinateSequence::atLeastNCoordinatesOrNothing definition fix.
 * Documentation fixes.
 *
 * Revision 1.4  2004/07/08 19:34:49  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added DefaultCoordinateSequenceFactory::instance() function.
 *
 * Revision 1.3  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.2  2004/06/16 13:13:25  strk
 * Changed interface of SegmentString, now copying CoordinateSequence argument.
 * Fixed memory leaks associated with this and MultiGeometry constructors.
 * Other associated fixes.
 *
 * Revision 1.1  2004/03/19 09:48:45  ybychkov
 * "geomgraph" and "geomgraph/indexl" upgraded to JTS 1.4
 *
 * Revision 1.14  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 * Revision 1.13  2003/10/15 16:39:03  strk
 * Made Edge::getCoordinates() return a 'const' value. Adapted code set.
 *
 **********************************************************************/
