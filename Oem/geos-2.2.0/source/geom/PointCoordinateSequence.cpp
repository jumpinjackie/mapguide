/**********************************************************************
 * $Id: PointCoordinateSequence.cpp,v 1.2.2.1.2.1 2005/11/29 16:58:17 strk Exp $
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

#include <sstream>
#include <geos/geom.h>

namespace geos {

PointCoordinateSequence::PointCoordinateSequence(const CoordinateSequence *c)
{
	cached_vector = NULL;
	vect=new vector<point_3d>();
	point_3d pt;
	int size=c->getSize();
	for(int i=0; i<size; i++) {
		pt.x=c->getAt(i).x;
		pt.y=c->getAt(i).y;
		pt.z=c->getAt(i).z;
		vect->push_back(pt);
	}
}

PointCoordinateSequence::PointCoordinateSequence() {
	vect=new vector<point_3d>();
	cached_vector = NULL;
}

PointCoordinateSequence::PointCoordinateSequence(int n) {
	cached_vector = NULL;
	vect=new vector<point_3d>();
//	vect->reserve(n);
	vect->resize(n);
}

PointCoordinateSequence::PointCoordinateSequence(const Coordinate& c) {
	point_3d pt={c.x,c.y,c.z};
	vect=new vector<point_3d>(1,pt);
	cached_vector = NULL;
}

PointCoordinateSequence::PointCoordinateSequence(const PointCoordinateSequence &c) {
	vect=new vector<point_3d>(*(c.vect));
	cached_vector = NULL;
}

CoordinateSequence *
PointCoordinateSequence::clone() const
{
	return new PointCoordinateSequence(*this);
}

void PointCoordinateSequence::setPoints(const vector<Coordinate> &v) {
	delete vect;
	vect=new vector<point_3d>();
	point_3d pt;
	for(unsigned int i=0; i<v.size(); i++) {
		pt.x=v[i].x;
		pt.y=v[i].y;
		pt.z=v[i].z;
		vect->push_back(pt);
	}
}

void PointCoordinateSequence::setPoints(vector<point_3d> &v) {
	vect=new vector<point_3d>(v);
}

const vector<Coordinate>*
PointCoordinateSequence::toVector() const
{
	if ( cached_vector ) return cached_vector;
	vector<Coordinate>* v=new vector<Coordinate>();
	for(unsigned int i=0; i<vect->size(); i++)
	{
		Coordinate c((*vect)[i].x, (*vect)[i].y, (*vect)[i].z);
		v->push_back(c);
	}
	cached_vector = v;
	return v;
}

vector<point_3d>* PointCoordinateSequence::toPointVector() {
	return vect;
}

bool PointCoordinateSequence::isEmpty() const {
	return vect->empty();
}

void PointCoordinateSequence::add(const Coordinate& c){
	delete cached_vector; cached_vector = NULL;
	point_3d pt={c.x,c.y,c.z};
	vect->push_back(pt);
}

void PointCoordinateSequence::add(point_3d p){
	delete cached_vector; cached_vector = NULL;
	vect->push_back(p);
}

int PointCoordinateSequence::getSize() const {
	return (int) vect->size();
}

const Coordinate& PointCoordinateSequence::getAt(int pos) const {
	point_3d pt;
//	if (pos>=0 && pos<=vect->size()-1) {
		pt=(*vect)[pos];
		return *(new Coordinate(pt.x,pt.y,pt.z));
//	} else
//		throw "PointCoordinateSequence exception: can't retrieve element\n";
}

point_3d PointCoordinateSequence::getPointAt(int pos){
//	if (pos>=0 && pos<=vect->size()-1) {
		return (*vect)[pos];
//	} else
//		throw "PointCoordinateSequence exception: can't retrieve element\n";
}

void PointCoordinateSequence::setAt(const Coordinate& c, int pos){
	point_3d pt={c.x,c.y,c.z};
	(*vect)[pos]=pt;
	if ( cached_vector ) (*cached_vector)[pos] = c;
}

void PointCoordinateSequence::setAt(point_3d p, int pos){
	(*vect)[pos]=p;
	Coordinate c(p.x, p.y, p.z);
	if ( cached_vector ) (*cached_vector)[pos] = c;
}

void PointCoordinateSequence::deleteAt(int pos){
	vect->erase(vect->begin()+pos);
	if ( cached_vector ) cached_vector->erase(cached_vector->begin()+pos);
}

string PointCoordinateSequence::toString() const {
	ostringstream s;
	if (getSize()>0) {
		for (unsigned int i=0; i<vect->size(); i++) {
			point_3d c=(*vect)[i];
			s<<"("<<c.x<<","<<c.y<<","<<c.z<<") ";
		}
	}
	return s.str();
}

PointCoordinateSequence::~PointCoordinateSequence() {
	delete vect;
	delete cached_vector;
}

double
PointCoordinateSequence::getOrdinate(unsigned int index, unsigned int ordinateIndex) const
{

#if PARANOIA_LEVEL > 0
	if ( index < 0 || index >= vect->size() ) 
	throw IllegalArgumentException("Coordinate number out of range");
#endif

	switch (ordinateIndex)
	{
		case CoordinateSequence::X:
			return (*vect)[index].x;
		case CoordinateSequence::Y:
			return (*vect)[index].y;
		case CoordinateSequence::Z:
			return (*vect)[index].z;
		default:
			return DoubleNotANumber;
	}
}

void
PointCoordinateSequence::setOrdinate(unsigned int index, unsigned int ordinateIndex,
	double value)
{

#if PARANOIA_LEVEL > 0
	if ( index < 0 || index >= vect->size() ) 
	throw IllegalArgumentException("Coordinate number out of range");
#endif

	switch (ordinateIndex)
	{
		case CoordinateSequence::X:
			(*vect)[index].x = value;
		case CoordinateSequence::Y:
			(*vect)[index].y = value;
		case CoordinateSequence::Z:
			(*vect)[index].z = value;
		default:
			return;
	}
}

} // namespace geos

/**********************************************************************
 * $Log: PointCoordinateSequence.cpp,v $
 * Revision 1.2.2.1.2.1  2005/11/29 16:58:17  strk
 * Back-ported WKB IO and C api.
 * Added required higher dimensional interfaces for CoordinateSequence
 *
 * Revision 1.2.2.1  2005/05/23 18:41:51  strk
 * Replaced sprintf uses with ostringstream
 *
 * Revision 1.2  2004/12/03 22:52:56  strk
 * enforced const return of CoordinateSequence::toVector() method to derivate classes.
 *
 * Revision 1.1  2004/07/08 19:38:56  strk
 * renamed from *List* equivalents
 *
 **********************************************************************/

