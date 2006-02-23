/**********************************************************************
 * $Id: CoordinateSequence.cpp,v 1.6.2.1 2005/06/23 11:20:38 strk Exp $
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

#include <geos/geom.h>
#include <stdio.h>

namespace geos {

bool
CoordinateSequence::hasRepeatedPoints() const
{
	int size=getSize();
	for(int i=1; i<size; i++) {
		if (getAt(i-1)==getAt(i)) {
			return true;
		}
	}
	return false;
}

/*
 * Returns either the given coordinate array if its length is greater than the
 * given amount, or an empty coordinate array.
 */
CoordinateSequence *
CoordinateSequence::atLeastNCoordinatesOrNothing(int n, CoordinateSequence *c)
{
	return c->getSize()>=n?c:DefaultCoordinateSequenceFactory::instance()->create(NULL);
}      


bool
CoordinateSequence::hasRepeatedPoints(const CoordinateSequence *cl) {
	int size=(int) cl->getSize();
	for(int i=1;i<size; i++) {
		if (cl->getAt(i-1)==cl->getAt(i)) {
			return true;
		}
	}
	return false;
}

const Coordinate* CoordinateSequence::minCoordinate() const {
	const Coordinate* minCoord=NULL;
	int size=(int) getSize();
	for(int i=0; i<size; i++) {
		if(minCoord==NULL || minCoord->compareTo(getAt(i))>0) {
			minCoord=&getAt(i);
		}
	}
	return minCoord;
}

const Coordinate*
CoordinateSequence::minCoordinate(CoordinateSequence *cl)
{
	const Coordinate* minCoord=NULL;
	int size=(int) cl->getSize();
	for(int i=0;i<size; i++) {
		if(minCoord==NULL || minCoord->compareTo(cl->getAt(i))>0) {
			minCoord=&(cl->getAt(i));
		}
	}
	return minCoord;
}

int
CoordinateSequence::indexOf(const Coordinate *coordinate, const CoordinateSequence *cl)
{
	for (int i=0; i<cl->getSize(); i++) {
		if ((*coordinate)==cl->getAt(i)) {
			return i;
		}
	}
	return -1;
}

void
CoordinateSequence::scroll(CoordinateSequence* cl,const Coordinate* firstCoordinate)
{
	int i, j=0;
	int ind=indexOf(firstCoordinate,cl);
	if (ind<1) return; // not found or already first
	int length=cl->getSize();
	vector<Coordinate> v(length);
	for (i=ind; i<length; i++) {
		v[j++]=cl->getAt(i);
	}
	for (i=0; i<ind; i++) {
		v[j++]=cl->getAt(i);
	}
	cl->setPoints(v);
}

void CoordinateSequence::reverse(CoordinateSequence *cl){
	int last=cl->getSize()-1;
	int mid=last/2;
	for(int i=0;i<=mid;i++) {
		const Coordinate tmp=cl->getAt(i);
		cl->setAt(cl->getAt(last-i),i);
		cl->setAt(tmp,last-i);
	}
}

bool CoordinateSequence::equals(CoordinateSequence *cl1,CoordinateSequence *cl2){
	if (cl1==cl2) return true;
	if (cl1==NULL||cl2==NULL) return false;
	if (cl1->getSize()!=cl2->getSize()) return false;
	for (int i = 0; i<cl1->getSize(); i++) {
		if (!(cl1->getAt(i)==cl2->getAt(i))) return false;
	}
	return true;
}

/** Add an array of coordinates
 * @param vc The coordinates
 * @param allowRepeated if set to false, repeated coordinates are collapsed
 * @return true (as by general collection contract)
 */
void
CoordinateSequence::add(const vector<Coordinate>* vc,bool allowRepeated)
{
	unsigned int size=vc->size();
	for(unsigned int i=0; i<size; i++) {
		add((*vc)[i],allowRepeated);
	}
}

/** Add a coordinate
* @param c The coordinate to add
* @param allowRepeated if set to false, repeated coordinates are collapsed
* @return true (as by general collection contract)
*/
void CoordinateSequence::add(const Coordinate& c, bool allowRepeated) {
	if (!allowRepeated) {
		if (getSize()>=1) {
			const Coordinate& last=getAt(getSize()-1);
			if (last.equals2D(c)) return;
		}
	}
	add(c);
}

/* Here for backward compatibility */
void
CoordinateSequence::add(CoordinateSequence *cl,bool allowRepeated,bool direction)
{
	add((const CoordinateSequence *)cl,allowRepeated,direction);
}

/**
 * Add an array of coordinates
 * @param cl The coordinates
 * @param allowRepeated if set to false, repeated coordinates are collapsed
 * @param direction if false, the array is added in reverse order
 * @return true (as by general collection contract)
 */
void
CoordinateSequence::add(const CoordinateSequence *cl,bool allowRepeated,bool direction)
{
	if (direction) {
		for (int i = 0; i < cl->getSize(); i++) {
			add(cl->getAt(i), allowRepeated);
		}
	} else {
		for (int j =cl->getSize()-1;j>=0;j--) {
			add(cl->getAt(j), allowRepeated);
		}
	}
}


/**
 * This function allocates space for a CoordinateSequence object
 * being a copy of the input once with consecutive equal points
 * removed.
 */
CoordinateSequence*
CoordinateSequence::removeRepeatedPoints(const CoordinateSequence *cl)
{
	CoordinateSequence* ret=DefaultCoordinateSequenceFactory::instance()->create(NULL);
	const vector<Coordinate> *v=cl->toVector();
	ret->add(v,false);
	//delete v;
	return ret;
}

} // namespace geos

/**********************************************************************
 * $Log: CoordinateSequence.cpp,v $
 * Revision 1.6.2.1  2005/06/23 11:20:38  strk
 * Minor performance improvements
 *
 * Revision 1.6  2004/12/03 22:52:56  strk
 * enforced const return of CoordinateSequence::toVector() method to derivate classes.
 *
 * Revision 1.5  2004/11/04 19:08:06  strk
 * Cleanups, initializers list, profiling.
 *
 * Revision 1.4  2004/10/13 10:03:02  strk
 * Added missing linemerge and polygonize operation.
 * Bug fixes and leaks removal from the newly added modules and
 * planargraph (used by them).
 * Some comments and indentation changes.
 *
 * Revision 1.3  2004/09/23 21:36:22  strk
 * Fixed a bug in ::reverse (thanks to Elliott Edwards)
 *
 * Revision 1.2  2004/07/21 09:55:24  strk
 * CoordinateSequence::atLeastNCoordinatesOrNothing definition fix.
 * Documentation fixes.
 *
 * Revision 1.1  2004/07/08 19:38:56  strk
 * renamed from *List* equivalents
 *
 **********************************************************************/

