//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef _FLOATTRANSFORM_
#define _FLOATTRANSFORM_

/// \ingroup Geometry_Module

//-------------------------------------------------------------------------
//
// FloatTransform - Class used to transform coordinates in double space
// to coordinates in float space and back again. The transform optimizes
// the floating point space to ensure float point number stability during
// buffering calculations.
//
//-------------------------------------------------------------------------

class FloatTransform
{
public:

    FloatTransform(MgEnvelope* doubleExtents);
    void  Double2Float(double x, double y, OpsFloatPoint& floatPoint) const;
    float Double2Float(double distance);
    MgCoordinate* Float2Double(const OpsFloatPoint& floatPoint) const;
    double Float2Double(float distance);

private:

    OpsDoubleExtent doubleExtent;   // Original extents in double space
    OpsFloatExtent floatExtent;     // Scaled extents in float space
    double xScale;                  // x scaling factor
    double yScale;                  // y scaling factor
};

#endif
