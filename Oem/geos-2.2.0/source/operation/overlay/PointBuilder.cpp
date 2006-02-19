/**********************************************************************
 * $Id: PointBuilder.cpp,v 1.13.2.2 2005/06/28 01:07:11 strk Exp $
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

#include <geos/opOverlay.h>
#include <geos/io.h>
#include <stdio.h>

#ifndef DEBUG
#define DEBUG 0
#endif
#ifndef COMPUTE_Z
#define COMPUTE_Z 0
#endif

namespace geos {

PointBuilder::PointBuilder(OverlayOp *newOp,
	const GeometryFactory *newGeometryFactory,
	PointLocator *newPtLocator):
		op(newOp),
		geometryFactory(newGeometryFactory),
		resultPointList(new vector<Point *>())
{
}

/*
 * @return a list of the Points in the result of the specified
 * overlay operation
 */
vector<Point*>*
PointBuilder::build(int opCode)
{
	extractNonCoveredResultNodes(opCode);
	return resultPointList;
}

/*
 * Determines nodes which are in the result, and creates Point for them.
 *
 * This method determines nodes which are candidates for the result via their
 * labelling and their graph topology.
 *
 * @param opCode the overlay operation
 */
void
PointBuilder::extractNonCoveredResultNodes(int opCode)
{
	map<Coordinate,Node*,CoordLT> *nodeMap =
		op->getGraph()->getNodeMap()->nodeMap;
	map<Coordinate,Node*,CoordLT>::iterator it=nodeMap->begin();
	for (; it!=nodeMap->end(); ++it)
	{
		Node *n=it->second;

		// filter out nodes which are known to be in the result
		if (n->isInResult()) continue;

		// if an incident edge is in the result, then
		// the node coordinate is included already
		if (n->isIncidentEdgeInResult()) continue;

		if ( n->getEdges()->getDegree() == 0 ||
			opCode == OverlayOp::INTERSECTION )
		{

			/**
			 * For nodes on edges, only INTERSECTION can result 
			 * in edge nodes being included even
			 * if none of their incident edges are included
			 */
			Label *label=n->getLabel();
			if (OverlayOp::isResultOfOp(label,opCode)) 
				filterCoveredNodeToPoint(n);
		}
	}
}

void
PointBuilder::filterCoveredNodeToPoint(const Node *n)
{
	const Coordinate& coord=n->getCoordinate();
	if(!op->isCoveredByLA(coord)) {
		Point *pt=geometryFactory->createPoint(coord);
		resultPointList->push_back(pt);
	}
}

} // namespace geos

/**********************************************************************
 * $Log: PointBuilder.cpp,v $
 * Revision 1.13.2.2  2005/06/28 01:07:11  strk
 * improved extraction of result points in overlay op
 *
 * Revision 1.13.2.1  2005/06/26 09:40:19  strk
 * Backport of OverlayOp performance improvements
 *
 * Revision 1.13  2004/11/17 15:09:08  strk
 * Changed COMPUTE_Z defaults to be more conservative
 *
 * Revision 1.12  2004/11/17 08:13:16  strk
 * Indentation changes.
 * Some Z_COMPUTATION activated by default.
 *
 * Revision 1.11  2004/10/21 22:29:54  strk
 * Indentation changes and some more COMPUTE_Z rules
 *
 * Revision 1.10  2004/10/20 17:32:14  strk
 * Initial approach to 2.5d intersection()
 *
 * Revision 1.9  2004/07/02 13:28:29  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.8  2004/06/30 20:59:13  strk
 * Removed GeoemtryFactory copy from geometry constructors.
 * Enforced const-correctness on GeometryFactory arguments.
 *
 * Revision 1.7  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/

