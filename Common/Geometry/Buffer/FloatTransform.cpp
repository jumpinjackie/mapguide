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

#include "Foundation.h"
#include "buffer.h"
#include "FloatTransform.h"

const float MINVDC = -8388608.0f;
const float MAXVDC = 8388607.0f;
const float VDCRANGE = 16777215.0f;

//-------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Create and initialize a FloatTransform object using the double
//          space extents passed in to initialize the transform to optimize
//          use of floating point space.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//-------------------------------------------------------------------------

FloatTransform::FloatTransform(MgEnvelope* doubleExtents)
{
    Ptr<MgCoordinate> ll = doubleExtents->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = doubleExtents->GetUpperRightCoordinate();

    doubleExtent.xMin = ll->GetX();
    doubleExtent.yMin = ll->GetY();

    doubleExtent.xMax = ur->GetX();
    doubleExtent.yMax = ur->GetY();

    double aspect = 1.0;

    if ( doubleExtent.Height() != 0.0)
        aspect = doubleExtent.Width() / doubleExtent.Height();

    if ( IsDoubleNan( aspect ) )
        aspect = 1.0;

    if (aspect >= 1.0)
    {
        floatExtent.xMin = MINVDC;
        floatExtent.xMax = MAXVDC;
        float halfHeight = float(floor(float((VDCRANGE / aspect) * 0.5)));
        floatExtent.yMin = -halfHeight;
        floatExtent.yMax =  halfHeight;
    }
    else
    {
        floatExtent.yMin = MINVDC;
        floatExtent.yMax = MAXVDC;
        float halfWidth = float(floor(float((VDCRANGE * aspect) * 0.5)));
        floatExtent.xMin = -halfWidth;
        floatExtent.xMax =  halfWidth;
    }

    if ( doubleExtent.Width() != 0.0)
    {
        xScale = floatExtent.Width()  / doubleExtent.Width();
        yScale = floatExtent.Height() / doubleExtent.Height();
    }
    else
    {
        xScale = yScale = 1.0;
    }
}

//-------------------------------------------------------------------------
//
// METHOD: Double2Float().
//
// PURPOSE: Transform a point from double space to float space.
//
// PARAMETERS:
//
//     Input:
//
//         x - double precision x-coordinate
//         y - double precision y-coordinate
//
//     Output:
//
//         floatPoint - reference to the structure the transformed
//                      coordinates are to be copied to.
//
// RETURNS: None.
//
//-------------------------------------------------------------------------

void FloatTransform::Double2Float(double x, double y, OpsFloatPoint& floatPoint) const
{
    floatPoint.x = float(floatExtent.xMin + xScale * (x - doubleExtent.xMin));
    floatPoint.y = float(floatExtent.yMin + yScale * (y - doubleExtent.yMin));
}

//-------------------------------------------------------------------------
//
// METHOD: Double2Float().
//
// PURPOSE: Transform a value from double space to float space.
//
// PARAMETERS:
//
//     Input:
//
//         value - the double value to transform.
//
// RETURNS: the value in float space.
//
//-------------------------------------------------------------------------
float FloatTransform::Double2Float(double distance)
{
    return float(distance * xScale);
}

//-------------------------------------------------------------------------
//
// METHOD: Float2Double().
//
// PURPOSE: Transform a value from space to double space.
//
// PARAMETERS:
//
//     Input:
//
//         value - the float value to transform.
//
// RETURNS: the value in double space.
//
//-------------------------------------------------------------------------
double FloatTransform::Float2Double(float distance)
{
    return (double)distance / xScale;
}


//-------------------------------------------------------------------------
//
// METHOD: Float2Double().
//
// PURPOSE: Transform a point from float space to double space.
//
// PARAMETERS:
//
//     Input:
//
//         floatPoint - reference to the structure containing
//                      the float coordinate.
//
//     Output:
//
//         floatPoint - pointer to the Point2D structure the
//                      transformed coordinate are copied to.
//
// RETURNS: None.
//
//-------------------------------------------------------------------------

MgCoordinate* FloatTransform::Float2Double(const OpsFloatPoint& floatPoint) const
{
    double dx = doubleExtent.xMin + (floatPoint.x - floatExtent.xMin) / xScale;
    double dy = doubleExtent.yMin + (floatPoint.y - floatExtent.yMin) / yScale;

    MgGeometryFactory factory;
    return factory.CreateCoordinateXY(dx, dy);
}
