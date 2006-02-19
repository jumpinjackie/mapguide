/**********************************************************************
 * $Id: PolygonExtracter.cpp,v 1.4 2004/07/02 13:28:26 strk Exp $
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
 * $Log: PolygonExtracter.cpp,v $
 * Revision 1.4  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.3  2004/05/14 13:42:46  strk
 * DistanceOp bug removed, cascading errors fixed.
 *
 * Revision 1.2  2004/05/14 09:20:47  strk
 * Mem leaks fixed
 *
 * Revision 1.1  2004/04/04 06:29:11  ybychkov
 * "planargraph" and "geom/utill" upgraded to JTS 1.4
 *
 *
 **********************************************************************/


#include <geos/geomUtil.h>
#include <typeinfo>

namespace geos {
/**
* Returns the Polygon components from a single geometry.
* If more than one geometry is to be processed, it is more
* efficient to create a single {@link PolygonExtracterFilter} instance
* and pass it to multiple geometries.
*/
vector<Geometry*>* PolygonExtracter::getPolygons(const Geometry *geom){
	vector<Geometry*> *ret=new vector<Geometry*>();
	PolygonExtracter pe(ret);
	geom->apply_ro(&pe);
	return ret;
}

/**
* Constructs a PolygonExtracterFilter with a list in which to store Polygons found.
*/
PolygonExtracter::PolygonExtracter(vector<Geometry*> *newComps){
	comps=newComps;
}

void PolygonExtracter::filter_rw(Geometry *geom) {
	if (typeid(*geom)==typeid(Polygon)) comps->push_back(geom);
}

void PolygonExtracter::filter_ro(const Geometry *geom) {
	if (typeid(*geom)==typeid(Polygon)) comps->push_back((Geometry*)geom);
}

}
