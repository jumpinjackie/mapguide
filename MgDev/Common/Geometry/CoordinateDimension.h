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

#ifndef _MGCOORDINATEDIMENSION_H_
#define _MGCOORDINATEDIMENSION_H_

/// \defgroup MgCoordinateDimension MgCoordinateDimension
/// \ingroup Geometry_Module_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// MgCoordinateDimension defines a set of integer constants
/// which can be OR'd together to specify the dimensionality of
/// an MgCoordinate instance.
///
class MgCoordinateDimension
{
EXTERNAL_API:
    /////////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the coordinate has X and Y ordinates.
    ///
    ///
    static const INT32 XY = 0;

    /////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the coordinate has X, Y, and Z ordinates.
    ///
    ///
    static const INT32 XYZ = 1;

    /////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the coordinate has an M ordinate.
    ///
    ///
    static const INT32 M = 2;
};
/// \}

#endif //_MGCOORDINATEDIMENSION_H_
