/**********************************************************************
 * $Id: LineMergeDirectedEdge.cpp,v 1.3 2004/10/13 10:03:02 strk Exp $
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
 * $Log: LineMergeDirectedEdge.cpp,v $
 * Revision 1.3  2004/10/13 10:03:02  strk
 * Added missing linemerge and polygonize operation.
 * Bug fixes and leaks removal from the newly added modules and
 * planargraph (used by them).
 * Some comments and indentation changes.
 *
 * Revision 1.2  2004/07/02 13:28:28  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.1  2004/04/07 06:55:50  ybychkov
 * "operation/linemerge" ported from JTS 1.4
 *
 *
 **********************************************************************/


#include <geos/opLinemerge.h>
#include <geos/util.h>

namespace geos {

LineMergeDirectedEdge::LineMergeDirectedEdge(planarNode *newFrom, planarNode *newTo, const Coordinate& newDirectionPt,bool nEdgeDirection): planarDirectedEdge(newFrom,newTo,newDirectionPt,nEdgeDirection) {}

/**
* Returns the directed edge that starts at this directed edge's end point, or null
* if there are zero or multiple directed edges starting there.  
* @return
*/
LineMergeDirectedEdge* LineMergeDirectedEdge::getNext() {
	if (getToNode()->getDegree()!=2) {
		return NULL;
	}
	if ((*(getToNode()->getOutEdges()->getEdges()))[0]==getSym()) {
		return (LineMergeDirectedEdge*) (*(getToNode()->getOutEdges()->getEdges()))[1];
	}
	Assert::isTrue((*(getToNode()->getOutEdges()->getEdges()))[1]==getSym());
	return (LineMergeDirectedEdge*) (*(getToNode()->getOutEdges()->getEdges()))[0];
}
}
