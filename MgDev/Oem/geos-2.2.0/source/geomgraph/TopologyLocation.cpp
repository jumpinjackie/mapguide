/**********************************************************************
 * $Id: TopologyLocation.cpp,v 1.2 2004/07/02 13:28:26 strk Exp $
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
 * $Log: TopologyLocation.cpp,v $
 * Revision 1.2  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.1  2004/03/19 09:48:45  ybychkov
 * "geomgraph" and "geomgraph/indexl" upgraded to JTS 1.4
 *
 * Revision 1.8  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/


#include <geos/geomgraph.h>

namespace geos {

TopologyLocation::TopologyLocation(const vector<int>* newLocation){
	location=new vector<int>();
	init((int)newLocation->size());
}

TopologyLocation::TopologyLocation(){
	location=NULL;
}

TopologyLocation::~TopologyLocation(){
	if ( location )
	{
		location->clear();
		delete location;
	}
}

/**
* Constructs a TopologyLocation specifying how points on, to the left of, and to the
* right of some GraphComponent relate to some Geometry. Possible values for the
* parameters are Location.NULL, Location.EXTERIOR, Location.BOUNDARY, 
* and Location.INTERIOR.
* @see Location
*/
TopologyLocation::TopologyLocation(int on, int left, int right) {
	location=new vector<int>();
	init(3);
	(*location)[Position::ON]=on;
	(*location)[Position::LEFT]=left;
	(*location)[Position::RIGHT]=right;
}

TopologyLocation::TopologyLocation(int on) {
	location=new vector<int>();
	init(1);
	(*location)[Position::ON]=on;
}

TopologyLocation::TopologyLocation(const TopologyLocation *gl) {
	location=new vector<int>(gl->location->begin(),gl->location->end());
}

void TopologyLocation::init(int size){
	location->resize(size);
	setAllLocations(Location::UNDEF);
}

int TopologyLocation::get(int posIndex) const {
	if (posIndex< (int)location->size()) return (*location)[posIndex];
	return Location::UNDEF;
}

/**
 * @return true if all locations are NULL
 */
bool TopologyLocation::isNull() const {
	for (unsigned int i=0; i<location->size(); i++) {
		if ((*location)[i]!=Location::UNDEF) return false;
	}
	return true;
}

/**
 * @return true if any locations are NULL
 */
bool TopologyLocation::isAnyNull() const {
	for (unsigned int i=0; i<location->size(); i++) {
		if ((*location)[i]==Location::UNDEF) return true;
	}
	return false;
}

bool TopologyLocation::isEqualOnSide(const TopologyLocation &le, int locIndex) const {
	return (*location)[locIndex]==(*(le.location))[locIndex];
}

bool TopologyLocation::isArea() const {
	return location->size()>1;
}

bool TopologyLocation::isLine() const {
	return location->size()==1;
}

void TopologyLocation::flip() {
	if (location->size()<=1) return;
	int temp=(*location)[Position::LEFT];
	(*location)[Position::LEFT]=(*location)[Position::RIGHT];
	(*location)[Position::RIGHT] = temp;
}

void TopologyLocation::setAllLocations(int locValue){
	for (unsigned int i=0; i<location->size(); i++) {
		(*location)[i]=locValue;
	}
}

void TopologyLocation::setAllLocationsIfNull(int locValue){
	for (unsigned int i=0; i<location->size(); i++) {
		if ((*location)[i]==Location::UNDEF) (*location)[i]=locValue;
	}
}

void TopologyLocation::setLocation(int locIndex, int locValue){
	(*location)[locIndex]=locValue;
}

void TopologyLocation::setLocation(int locValue){
	setLocation(Position::ON, locValue);
}

const vector<int>* TopologyLocation::getLocations() const {
	return location;
}

void TopologyLocation::setLocations(int on, int left, int right) {
	(*location)[Position::ON]=on;
	(*location)[Position::LEFT]=left;
	(*location)[Position::RIGHT]=right;
}

void TopologyLocation::setLocations(const TopologyLocation& gl){
	for (unsigned int i=0; i<gl.location->size(); i++) {
		(*location)[i]=(*(gl.location))[i];
	}
}

bool TopologyLocation::allPositionsEqual(int loc) const {
	for (unsigned int i=0; i<location->size(); i++) {
		if ((*location)[i]!=loc) return false;
	}
	return true;
}

/**
 * merge updates only the NULL attributes of this object
 * with the attributes of another.
 */
void TopologyLocation::merge(const TopologyLocation* gl){
	// if the src is an Area label & and the dest is not, increase the dest to be an Area
	if (gl->location->size()>location->size()) {
		vector<int> newLoc(3);
		newLoc[Position::ON]=(*location)[Position::ON];
		newLoc[Position::LEFT]=Location::UNDEF;
		newLoc[Position::RIGHT]=Location::UNDEF;
		location->swap(newLoc);
	}
	for (unsigned int i=0; i<location->size(); i++) {
		if ((*location)[i]==Location::UNDEF && i<gl->location->size())
			(*location)[i]=(*(gl->location))[i];
	}
}

string TopologyLocation::toString() const {
	string buf="";
	if (location->size()>1) buf+=Location::toLocationSymbol((*location)[Position::LEFT]);
	buf+=Location::toLocationSymbol((*location)[Position::ON]);
	if (location->size()>1) buf+=Location::toLocationSymbol((*location)[Position::RIGHT]);
	return buf;
}
}

