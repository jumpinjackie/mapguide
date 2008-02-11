//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

/// \defgroup MgFeatureGeometricType MgFeatureGeometricType
/// \ingroup Feature_Service_classes
/// \{

#ifndef _MG_FEATURE_GEOMETRIC_TYPE_H_
#define _MG_FEATURE_GEOMETRIC_TYPE_H_

/////////////////////////////////////////////////////////////////
/// \brief
/// Is a set of constants specifying the dimensions of geometries
/// which can be stored in a geometry property.
/// \remarks
/// The constants  can be bit-wise OR'd together. A value of this
/// type is returned by
/// MgGeometricPropertyDefinition::GetGeometryTypes.
///
class MgFeatureGeometricType
{
PUBLISHED_API:
    /////////////////////////////////////////////////////////////
    /// Represents zero-dimensional geometric primitives, such as
    /// MgPoint.
    static const int Point      =  1;

    ////////////////////////////////////////////////////////////
    /// Represents one-dimensional geometric primitives, such as
    /// MgLineString and MgCurveString.
    static const int Curve      =  2;

    ////////////////////////////////////////////////////////////
    /// Represents two-dimensional geometric primitives, such as
    /// MgPolygon and MgCurvePolygon.
    static const int Surface    =  4;

    //////////////////////////////////////////////////////////////
    /// Represents three-dimensional geometric primitives, such as
    /// Cubes.
    static const int Solid      =  8;
};
/// \}

#endif
