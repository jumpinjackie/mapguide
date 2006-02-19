/**********************************************************************
 * $Id: PointCoordinateSequenceFactory.cpp,v 1.1.4.1 2005/11/29 16:58:17 strk Exp $
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
 * $Log: PointCoordinateSequenceFactory.cpp,v $
 * Revision 1.1.4.1  2005/11/29 16:58:17  strk
 * Back-ported WKB IO and C api.
 * Added required higher dimensional interfaces for CoordinateSequence
 *
 * Revision 1.1  2004/07/08 19:38:56  strk
 * renamed from *List* equivalents
 *
 **********************************************************************/


#include <geos/geom.h>
#include <stdio.h>

namespace geos {

CoordinateSequence *
PointCoordinateSequenceFactory::create(vector<Coordinate> *coords) const
{
	CoordinateSequence *cs = new PointCoordinateSequence((*coords)[0]);
	delete coords;
	return cs;
}

CoordinateSequence *
PointCoordinateSequenceFactory::create(unsigned int size, unsigned int dims) const
{
	return new PointCoordinateSequence(size);
}


}
