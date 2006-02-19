/**********************************************************************
 * $Id: ElevationMatrixFilter.cpp,v 1.3 2004/11/29 16:05:33 strk Exp $
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

#include <geos/geom.h>
#include <geos/opOverlay.h>

#define DEBUG 0

namespace geos
{

ElevationMatrixFilter::ElevationMatrixFilter(const ElevationMatrix *newEm):
	em(newEm), avgElevation(newEm->getAvgElevation())
{
}

ElevationMatrixFilter::~ElevationMatrixFilter()
{
}

void
ElevationMatrixFilter::filter_rw(Coordinate *c)
{
#if DEBUG
	cerr<<"ElevationMatrixFilter::filter_rw("<<c->toString()<<") called"
		<<endl;
#endif
	if ( ISNAN(c->z) && !ISNAN(avgElevation) )
	{
		try {
			const ElevationMatrixCell &emc = em->getCell(*c);
			c->z = emc.getAvg();
			if ( ISNAN(c->z) ) c->z = avgElevation;
#if DEBUG
			cerr<<"  z set to "<<c->z<<endl;
#endif
		} catch (IllegalArgumentException *ex) {
			delete ex;
			c->z = avgElevation;
		}
	}
#if DEBUG
	else
	{
		cerr<<"  already has a Z or avgElevation == NaN"<<endl;
	}
#endif
}

} // namespace geos

/**********************************************************************
 * $Log: ElevationMatrixFilter.cpp,v $
 * Revision 1.3  2004/11/29 16:05:33  strk
 * Fixed a bug in LineIntersector::interpolateZ causing NaN values
 * to come out.
 * Handled dimensional collapses in ElevationMatrix.
 * Added ISNAN macro and changed ISNAN/FINITE macros to avoid
 * dispendious isnan() and finite() calls.
 *
 * Revision 1.2  2004/11/26 09:22:50  strk
 * Added FINITE(x) macro and its use.
 * Made input geoms average Z computation optional in OverlayOp.
 *
 * Revision 1.1  2004/11/23 16:22:49  strk
 * Added ElevationMatrix class and components to do post-processing draping of overlayed geometries.
 *
 **********************************************************************/
