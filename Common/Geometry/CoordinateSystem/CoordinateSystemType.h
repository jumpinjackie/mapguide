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

#ifndef _MGCOORDINATESYSTEMTYPE_H_
#define _MGCOORDINATESYSTEMTYPE_H_

/// \defgroup MgCoordinateSystemType MgCoordinateSystemType
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// Defines constants used to indicate the type of a coordinate
/// system.
///
class MG_GEOMETRY_API MgCoordinateSystemType
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies the coordinate system is not set.
    ///
    ///
    static const INT32 Unknown = 0;
    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the coordinate system is arbitrary. That is,
    /// it is not spatially bound to the earth, and the initial
    /// keyword in the WKT definition of the coordinate system is
    /// LOCAL_CS.
    ///
    ///
    static const INT32 Arbitrary = 1;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the coordinate system is geographic. That is,
    /// the initial keyword in the WKT definition of the coordinate
    /// system is GEOGCS.
    ///
    ///
    static const INT32 Geographic = 2;
    //////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the coordinate system is projected. In most
    /// cases, the initial keyword in the WKT definition of the
    /// coordinate system is PROJCS. In some cases, the initial
    /// keyword is GEOGCS.
    ///
    ///
    static const INT32 Projected = 3;
};
/// \}

#endif //_MGCOORDINATESYSTEMTYPE_H_
