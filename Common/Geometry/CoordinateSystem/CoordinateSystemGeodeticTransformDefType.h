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

#ifndef _MGCOORDINATESYSTEMGEODETICTRANSFORMDEFTYPE_H_
#define _MGCOORDINATESYSTEMGEODETICTRANSFORMDEFTYPE_H_

/// \defgroup MgCoordinateSystemGeodeticTransformDefType MgCoordinateSystemGeodeticTransformDefType
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// This class contains the constants for all current transformation definition types.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticTransformDefType
{

PUBLISHED_API:

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Default 0 constant available for programming convenience only. It does not
    /// describe a specific geodetic transformation type.
    static const INT32 None = 0;

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// The transformation definition describes a built-in transformation. That is, that
    /// transformation is well-known and cannot be parameterized because
    /// the information is available to the transformation engine.
    static const INT32 Standalone = 1;

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// The transformation definition describes an analytical transformation where
    /// the transformation between the source and the target datum is defined
    /// through a formular that's fed with up to 10 parameters.
    static const INT32 Analytical = 2;

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// The transformation definition describes a transformation that
    /// uses grid files to calculate the actual datum shift for a given
    /// coordinate by interpolating between given grid points.
    static const INT32 Interpolation = 3;

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// The transformation definition describes a transformation that
    /// uses a multiple regression calculation to perform the actual datum shift.
    static const INT32 MultipleRegression = 4;
};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICTRANSFORMDEFTYPE_H_
