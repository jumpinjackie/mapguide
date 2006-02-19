/**********************************************************************
 * $Id: ConsistentAreaTester.cpp,v 1.7 2004/07/02 13:28:29 strk Exp $
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
 * $Log: ConsistentAreaTester.cpp,v $
 * Revision 1.7  2004/07/02 13:28:29  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.6  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/


#include <geos/opValid.h>
#include <geos/opRelate.h>
#include <stdio.h>

namespace geos {

ConsistentAreaTester::ConsistentAreaTester(GeometryGraph *newGeomGraph){
	geomGraph=newGeomGraph;
	nodeGraph=new RelateNodeGraph();
	li=new RobustLineIntersector();
}

ConsistentAreaTester::~ConsistentAreaTester(){
	delete nodeGraph;
	delete li;
}

Coordinate& ConsistentAreaTester::getInvalidPoint(){
	return invalidPoint;
}

bool ConsistentAreaTester::isNodeConsistentArea() {
    /**
     * To fully check validity, it is necessary to
     * compute ALL intersections, including self-intersections within a single edge.
     */
	auto_ptr<SegmentIntersector> intersector(geomGraph->computeSelfNodes(li,true));
	if (intersector->hasProperIntersection()) {
		invalidPoint=intersector->getProperIntersectionPoint();
		return false;
	}
	nodeGraph->build(geomGraph);
	return isNodeEdgeAreaLabelsConsistent();
}

/**
* Check all nodes to see if their labels are consistent.
* If any are not, return false
*/
bool ConsistentAreaTester::isNodeEdgeAreaLabelsConsistent() {
	map<Coordinate,Node*,CoordLT> *nMap=nodeGraph->getNodeMap();
	map<Coordinate,Node*,CoordLT>::iterator nodeIt;
	for(nodeIt=nMap->begin();nodeIt!=nMap->end();nodeIt++) {
		RelateNode *node=(RelateNode*) nodeIt->second;
		if (!node->getEdges()->isAreaLabelsConsistent()) {
			Coordinate *c=new Coordinate(node->getCoordinate());
			invalidPoint=*c;
			delete c;
			return false;
		}
	}
	return true;
}

/**
* Checks for two duplicate rings in an area.
* Duplicate rings are rings that are topologically equal
* (that is, which have the same sequence of points up to point order).
* If the area is topologically consistent (determined by calling the
* <code>isNodeConsistentArea</code>,
* duplicate rings can be found by checking for EdgeBundles which contain
* more than one EdgeEnd.
* (This is because topologically consistent areas cannot have two rings sharing
* the same line segment, unless the rings are equal).
* The start point of one of the equal rings will be placed in
* invalidPoint.
*
* @return true if this area Geometry is topologically consistent but has two duplicate rings
*/
bool ConsistentAreaTester::hasDuplicateRings() {
	map<Coordinate,Node*,CoordLT> *nMap=nodeGraph->getNodeMap();
	map<Coordinate,Node*,CoordLT>::iterator nodeIt;
	for(nodeIt=nMap->begin();nodeIt!=nMap->end();nodeIt++) {
		RelateNode *node=(RelateNode*) nodeIt->second;
		vector<EdgeEnd*> *v=node->getEdges()->getEdges();
		for(int i=0; i<(int)v->size(); i++) {
			EdgeEndBundle *eeb=(EdgeEndBundle*) (*v)[i];
			if (eeb->getEdgeEnds()->size()>1) {
				invalidPoint=eeb->getEdge()->getCoordinate(0);
				return true;
			}
		}
	}
	return false;
}
}

