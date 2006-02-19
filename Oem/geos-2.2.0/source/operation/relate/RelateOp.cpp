/**********************************************************************
 * $Id: RelateOp.cpp,v 1.15 2004/07/27 16:35:47 strk Exp $
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
 * $Log: RelateOp.cpp,v $
 * Revision 1.15  2004/07/27 16:35:47  strk
 * Geometry::getEnvelopeInternal() changed to return a const Envelope *.
 * This should reduce object copies as once computed the envelope of a
 * geometry remains the same.
 *
 * Revision 1.14  2004/07/02 13:28:29  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.13  2004/03/29 06:59:25  ybychkov
 * "noding/snapround" package ported (JTS 1.4);
 * "operation", "operation/valid", "operation/relate" and "operation/overlay" upgraded to JTS 1.4;
 * "geom" partially upgraded.
 *
 * Revision 1.12  2004/03/01 22:04:59  strk
 * applied const correctness changes by Manuel Prieto Villegas <ManuelPrietoVillegas@telefonica.net>
 *
 * Revision 1.11  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/


#include <geos/opRelate.h>
#include <stdio.h>
#include <typeinfo>

namespace geos {

IntersectionMatrix* RelateOp::relate(const Geometry *a, const Geometry *b) {
	RelateOp relOp(a,b);
	return relOp.getIntersectionMatrix();
}

///**
//* Implements relate on GeometryCollections as the sum of
//* the Intersection matrices for the components of the
//* collection(s).
//* This may or may not be appropriate semantics for this operation.
//* @param a a List of Geometries, none of which are a basic GeometryCollection
//* @param b a List of Geometries, none of which are a basic GeometryCollection
//* @return the matrix representing the topological relationship of the geometries
//*/
//IntersectionMatrix* RelateOp::relateGC(vector<const Geometry*> *a,vector<const Geometry*> *b) {
//	IntersectionMatrix *finalIM=new IntersectionMatrix();
//	for(int i=0;i<(int)a->size();i++) {
//		const Geometry *aGeom=(*a)[i];
//		for(int j=0;j<(int)b->size();j++) {
//			const Geometry *bGeom=(*b)[j];
//			RelateOp *relOp=new RelateOp(aGeom,bGeom);
//			IntersectionMatrix *im=relOp->getIntersectionMatrix();
//			finalIM->add(im);
//		}
//	}
//	return finalIM;
//}

RelateOp::RelateOp(const Geometry *g0, const Geometry *g1):GeometryGraphOperation(g0,g1), relateComp(arg) {
	//relateComp=new RelateComputer(arg);
}

RelateOp::~RelateOp() {
	//delete relateComp;
}

IntersectionMatrix* RelateOp::getIntersectionMatrix() {
	return relateComp.computeIM();
}

//vector<const Geometry*>* RelateOp::toList(const Geometry *geom) {
//	vector<const Geometry*> *geomList=new vector<const Geometry*>();
//	return addToList(geom,geomList);
//}

//vector<const Geometry*>* RelateOp::addToList(const Geometry *geom, vector<const Geometry*>* geomList) {
//	if (isBaseGeometryCollection(geom)) {
//		const GeometryCollection *gc=(GeometryCollection*) geom;
//		for(int i=0;i<gc->getNumGeometries();i++) {
//			addToList(gc->getGeometryN(i),geomList);
//		}
//	} else
//		geomList->push_back(geom);
//	return geomList;
//}

//bool RelateOp::isBaseGeometryCollection(const Geometry* geom) {
//	//if ((typeid(*geom)==typeid(GeometryCollection)) ||
//	//	(typeid(*geom)==typeid(MultiPoint)) ||
//	//	(typeid(*geom)==typeid(MultiLineString)) ||
//	//	(typeid(*geom)==typeid(MultiPolygon))) {
//	//	return true;
//	//} else {
//	//	return false;
//	//}
//	if (typeid(*geom)==typeid(GeometryCollection)) {
//		return true;
//	} else {
//		return false;
//	}
//
//}
}

