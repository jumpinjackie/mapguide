/**********************************************************************
 * $Id: CommonBitsRemover.cpp,v 1.2 2004/07/02 13:28:29 strk Exp $
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
 * $Log: CommonBitsRemover.cpp,v $
 * Revision 1.2  2004/07/02 13:28:29  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.1  2004/04/10 22:41:25  ybychkov
 * "precision" upgraded to JTS 1.4
 *
 *
 **********************************************************************/


#include <geos/precision.h>

namespace geos {


CommonBitsRemover::CommonBitsRemover() {
	ccFilter=new CommonCoordinateFilter();
}

CommonBitsRemover::~CommonBitsRemover() {
	delete ccFilter;
}

/**
* Add a geometry to the set of geometries whose common bits are
* being computed.  After this method has executed the
* common coordinate reflects the common bits of all added
* geometries.
*
* @param geom a Geometry to test for common bits
*/
void CommonBitsRemover::add(Geometry *geom){
	geom->apply_rw(ccFilter);
	commonCoord=*(ccFilter->getCommonCoordinate());
}

/**
* The common bits of the Coordinates in the supplied Geometries.
*/
Coordinate& CommonBitsRemover::getCommonCoordinate() { 
	return commonCoord;
}

/**
* Removes the common coordinate bits from a Geometry.
* The coordinates of the Geometry are changed.
*
* @param geom the Geometry from which to remove the common coordinate bits
* @return the shifted Geometry
*/
Geometry* CommonBitsRemover::removeCommonBits(Geometry *geom){
	if (commonCoord.x == 0.0 && commonCoord.y == 0.0)
		return geom;
	Coordinate *invCoord=new Coordinate(commonCoord.x,commonCoord.y);
	invCoord->x=-invCoord->x;
	invCoord->y=-invCoord->y;
	Translater *trans=new Translater(*invCoord);
	delete invCoord;
	geom->apply_rw(trans);
	geom->geometryChanged();
	delete trans;
	return geom;
}

/**
* Adds the common coordinate bits back into a Geometry.
* The coordinates of the Geometry are changed.
*
* @param geom the Geometry to which to add the common coordinate bits
* @return the shifted Geometry
*/
void CommonBitsRemover::addCommonBits(Geometry *geom){
	Translater *trans=new Translater(commonCoord);
	geom->apply_rw(trans);
	geom->geometryChanged();
	delete trans;
}

CommonCoordinateFilter::CommonCoordinateFilter() {
	commonBitsX=new CommonBits();
	commonBitsY=new CommonBits();
}

CommonCoordinateFilter::~CommonCoordinateFilter() {
	delete commonBitsX;
	delete commonBitsY;
}

void CommonCoordinateFilter::filter_rw(Coordinate *coord){
	commonBitsX->add(coord->x);
	commonBitsY->add(coord->y);
}

void CommonCoordinateFilter::filter_ro(const Coordinate *coord){
	commonBitsX->add(coord->x);
	commonBitsY->add(coord->y);
}

Coordinate* CommonCoordinateFilter::getCommonCoordinate() {
	return new Coordinate(commonBitsX->getCommon(),commonBitsY->getCommon());
}

Translater::Translater(Coordinate &newTrans){
	trans=newTrans;
}

void Translater::filter_rw(Coordinate *coord){
	coord->x+=trans.x;
	coord->y+=trans.y;
}

}
