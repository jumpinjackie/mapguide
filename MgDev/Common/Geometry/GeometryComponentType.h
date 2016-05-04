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

#ifndef _MGGEOMETRYCOMPONENTTYPE_H_
#define _MGGEOMETRYCOMPONENTTYPE_H_

/// \defgroup MgGeometryComponentType MgGeometryComponentType
/// \ingroup Geometry_Module_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// MgGeometryComponentType defines a set of integer constants
/// used to signify the type of a geometry component instance.
///
/// \remarks
/// A
/// geometry component instance is used in the construction of a
/// geometry.
///
class MgGeometryComponentType
{
PUBLISHED_API:
    ////////////////////////////////////////////////////////
    /// \brief
    /// Specifies the geometry component is a MgLinearRing.
    ///
    static const INT32 LinearRing = 129;

    /////////////////////////////////////////////////////////
    /// \brief
    /// Specifies the geometry component is an MgArcSegment.
    ///
    static const INT32 ArcSegment = 130;

    ////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies the geometry component is an MgLinearSegment.
    ///
    static const INT32 LinearSegment = 131;

    ////////////////////////////////////////////////////////
    /// \brief
    /// Specifies the geometry component is an MgCurveRing.
    ///
    static const INT32 CurveRing = 132;
};
/// \}

#endif //_MGGEOMETRYCOMPONENTTYPE_H_
