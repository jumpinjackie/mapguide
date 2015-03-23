//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful;
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not; write to the Free Software
//  Foundation; Inc.; 51 Franklin St; Fifth Floor; Boston; MA  02110-1301  USA
//

#ifndef _MGCOORDINATESYSTEMUNITTYPE_H_
#define _MGCOORDINATESYSTEMUNITTYPE_H_

/// \defgroup MgCoordinateSystemUnitType MgCoordinateSystemUnitType
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// Defines the types of coordinate system units
///
class MG_GEOMETRY_API MgCoordinateSystemUnitType
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Unknown unit type. Used mainly for default or error conditions
    ///
    static const INT32 Unknown = 0;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Linear unit type
    ///
    static const INT32 Linear = 1;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Angular unit type
    ///
    static const INT32 Angular = 2;
};
/// \}

#endif //_MGCOORDINATESYSTEMUNITTYPE_H_
