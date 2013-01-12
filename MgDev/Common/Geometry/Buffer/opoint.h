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

/// \ingroup Geometry_Module

//------------------------------------------------------------------------------
//
// FILE: opoint.h.
//
// PURPOSE: Declaration of the OpsPoint<> templte. Typedefs for OpsFloatPoint,
//          OpsDoublePoint, and OpsIntegerPoint provided (note that OpsInteger-
//          Point derives from CPoint, rather than instantiating OpsPoint<>;
//          this is done so that an OpsIntegerPoint can be used wherever a
//          CPoint is expected).
//
//------------------------------------------------------------------------------

#ifndef _OPOINT_H_
#define _OPOINT_H_

//------------------------------------------------------------------------------
//
// Template: OpsPoint<>.
//
// Purpose: Stores coordinates of a point in using the specified numerical type.
//
//------------------------------------------------------------------------------

template <class ScalarType> struct OpsPoint {
    ScalarType x;
    ScalarType y;

    OpsPoint();
    OpsPoint(ScalarType x, ScalarType y);
    int operator==(const OpsPoint<ScalarType> &point) const;
    int operator!=(const OpsPoint<ScalarType> &point) const;
};


//------------------------------------------------------------------------------
//
// Constructor - Initialize an OpsPoint<> setting x and y to zero.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline OpsPoint<ScalarType>::OpsPoint() :
    x(0),
    y(0)
{
}


//------------------------------------------------------------------------------
//
// Constructor - Initialize an OpsPoint<> setting x and y to the specified
//               values.
//
//------------------------------------------------------------------------------

template <class ScalarType>
inline OpsPoint<ScalarType>::OpsPoint(ScalarType x, ScalarType y) :
    x(x),
    y(y)
{
}


//------------------------------------------------------------------------------
//
// OpsPoint<>::operator==() - Method to test two points for equality.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline int
OpsPoint<ScalarType>::operator==(const OpsPoint<ScalarType> &point) const
{
    return (x != point.x || y != point.y ? 0 : 1);
}


//------------------------------------------------------------------------------
//
// OpsPoint<>::operator!=() - Method to test two points for inequality.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline int
OpsPoint<ScalarType>::operator!=(const OpsPoint<ScalarType> &point) const
{
    return (x != point.x || y != point.y ? 1 : 0);
}


// typedefs for commonly used point types

typedef OpsPoint<float> OpsFloatPoint;
typedef OpsPoint<double> OpsDoublePoint;
// typedef CPoint OpsIntegerPoint;

#endif
