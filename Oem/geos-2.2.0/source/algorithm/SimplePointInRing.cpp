/**********************************************************************
 * $Id: SimplePointInRing.cpp,v 1.15 2004/07/02 13:28:26 strk Exp $
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
 * $Log: SimplePointInRing.cpp,v $
 * Revision 1.15  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.14  2004/03/17 02:00:33  ybychkov
 * "Algorithm" upgraded to JTS 1.4
 *
 * Revision 1.13  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 * Revision 1.12  2003/10/16 08:50:00  strk
 * Memory leak fixes. Improved performance by mean of more calls to 
 * new getCoordinatesRO() when applicable.
 *
 **********************************************************************/

#include <geos/geosAlgorithm.h>

namespace geos {

SimplePointInRing::SimplePointInRing(LinearRing *ring){
	pts=ring->getCoordinatesRO();
}

SimplePointInRing::~SimplePointInRing(){
}

bool SimplePointInRing::isInside(const Coordinate& pt){
	return CGAlgorithms::isPointInRing(pt,pts);
}

}

