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

#ifndef _MGGEOMETRYTYPE_H_
#define _MGGEOMETRYTYPE_H_

/// \defgroup MgGeometryType MgGeometryType
/// \ingroup Geometry_Module_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// MgGeometryType defines integer constants used to signify the
/// derived type of an MgGeometry instance.
///
class MgGeometryType
{
PUBLISHED_API:
    ////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is a Point.
    ///
    static const INT32 Point = 1;

    /////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is a LineString.
    ///
    static const INT32 LineString = 2;

    //////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is a Polygon.
    ///
    static const INT32 Polygon = 3;

    /////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is a MultiPoint.
    ///
    static const INT32 MultiPoint = 4;

    //////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is a MultiLineString.
    ///
    static const INT32 MultiLineString = 5;

    ///////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is a MultiPolygon.
    ///
    static const INT32 MultiPolygon = 6;

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is a MultiGeometry. In OpenGIS
    /// this is referred to as a GeometryCollection.
    ///
    static const INT32 MultiGeometry = 7;

    //////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is a CurveString.
    ///
    static const INT32 CurveString = 10;

    ///////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is a CurvePolygon.
    ///
    static const INT32 CurvePolygon = 11;

    //////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is a MultiLineString.
    ///
    static const INT32 MultiCurveString = 12;

    //////////////////////////////////////////////
    /// \brief
    /// Specifies the geometry is a MultiPolygon.
    ///
    static const INT32 MultiCurvePolygon = 13;
};
/// \}

#endif //_MGGEOMETRYTYPE_H_
