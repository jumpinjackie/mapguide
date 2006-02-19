/**********************************************************************
 * $Id: EdgeEnd.cpp,v 1.3.2.2 2005/05/23 18:41:51 strk Exp $
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
#include <geos/util.h>
#include <typeinfo>
#include <cmath>
#include <sstream>

namespace geos {

//CGAlgorithms* EdgeEnd::cga=new RobustCGAlgorithms();

EdgeEnd::EdgeEnd() {
	this->edge=NULL;
	label=NULL;
	node=NULL;
	dx=dy=0.0;
	quadrant=0;
}

EdgeEnd::~EdgeEnd() {
//	delete edge;
	delete label;
//	delete node;  
}

EdgeEnd::EdgeEnd(Edge* newEdge){
	this->edge=newEdge;
	label=NULL;
	node=NULL;
	dx=dy=0.0;
	quadrant=0;
}

EdgeEnd::EdgeEnd(Edge* newEdge,Coordinate& newP0, Coordinate& newP1){
	this->edge=newEdge;
	node=NULL;
	dx=dy=0.0;
	quadrant=0;
	init(newP0,newP1);
	label=NULL;
}

EdgeEnd::EdgeEnd(Edge* newEdge, Coordinate& newP0, Coordinate& newP1, Label* newLabel){
	this->edge=newEdge;
	node=NULL;
	dx=dy=0.0;
	quadrant=0;
	init(newP0,newP1);
	label=newLabel;
}

void EdgeEnd::init(const Coordinate& newP0, const Coordinate& newP1){
	p0=newP0;
	p1=newP1;
	dx=p1.x-p0.x;
	dy=p1.y-p0.y;
	quadrant=Quadrant::quadrant(dx,dy);
	Assert::isTrue(!(dx == 0 && dy == 0), "EdgeEnd with identical endpoints found");
}

Edge* EdgeEnd::getEdge() {return edge;}
Label* EdgeEnd::getLabel() {return label;}
Coordinate& EdgeEnd::getCoordinate() {return p0;}
Coordinate& EdgeEnd::getDirectedCoordinate() {return p1;}
int EdgeEnd::getQuadrant() {return quadrant;}
double EdgeEnd::getDx() {return dx;}
double EdgeEnd::getDy() {return dy;}
void EdgeEnd::setNode(Node* newNode) {node=newNode;}
Node* EdgeEnd::getNode() {return node;}

int EdgeEnd::compareTo(EdgeEnd* e) {
	return compareDirection(e);
}

int EdgeEnd::compareDirection(EdgeEnd* e) {
	if (dx == e->dx && dy == e->dy)
		return 0;
	// if the rays are in different quadrants, determining the ordering is trivial
	if (quadrant>e->quadrant) return 1;
	if (quadrant<e->quadrant) return -1;
	// vectors are in the same quadrant - check relative orientation of direction vectors
	// this is > e if it is CCW of e
	return CGAlgorithms::computeOrientation(e->p0,e->p1,p1);
}

void EdgeEnd::computeLabel(){
	// subclasses should override this if they are using labels
}

string EdgeEnd::print() {
	ostringstream s;

	s<<"EdgeEnd: ";
	s<<p0.toString();
	s<<" - ";
	s<<p1.toString();
	s<<" ";
	s<<quadrant<<":"<<atan2(dy,dx);
	s<<"  ";
	s<<label->toString();

	return s.str();
}

} // namespace geos

/**********************************************************************
 * $Log: EdgeEnd.cpp,v $
 * Revision 1.3.2.2  2005/05/23 18:41:51  strk
 * Replaced sprintf uses with ostringstream
 *
 *
 * Revision 1.3.2.1  2005/05/23 17:10:08  strk
 * Stricter C++ syntax (math.h=>cmath, ieeefp.h in "C" block)
 *
 * Revision 1.3  2004/10/21 22:29:54  strk
 * Indentation changes and some more COMPUTE_Z rules
 *
 * Revision 1.2  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.1  2004/03/19 09:48:45  ybychkov
 * "geomgraph" and "geomgraph/indexl" upgraded to JTS 1.4
 *
 **********************************************************************/
