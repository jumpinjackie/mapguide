/**********************************************************************
 * $Id: ElevationMatrixCell.cpp,v 1.3 2004/11/29 16:05:33 strk Exp $
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
#include <geos/opOverlay.h>

namespace geos
{

ElevationMatrixCell::ElevationMatrixCell(): ztot(0)
{
}

ElevationMatrixCell::~ElevationMatrixCell()
{
}

void
ElevationMatrixCell::add(const Coordinate &c)
{
	if ( !ISNAN(c.z) )
	{
		if ( zvals.insert(c.z).second )
		{
			ztot+=c.z;
		}
	}
}

void
ElevationMatrixCell::add(double z)
{
	if ( !ISNAN(z) )
	{
		if ( zvals.insert(z).second )
		{
			ztot+=z;
		}
	}
}

double
ElevationMatrixCell::getTotal() const
{
	return ztot;
}

double
ElevationMatrixCell::getAvg() const
{
	if ( ! zvals.size() ) return DoubleNotANumber;
	return (ztot/zvals.size());
}

string
ElevationMatrixCell::print() const
{
	ostringstream ret;
	//ret<<"["<<ztot<<"/"<<zvals.size()<<"]";
	ret<<"["<<ztot/zvals.size()<<"]";
	return ret.str();
}

} // namespace geos;

/**********************************************************************
 * $Log: ElevationMatrixCell.cpp,v $
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
