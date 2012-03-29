//
//  Copyright (C) 2011 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMGEODETICDIRECTION_H_
#define _MGCOORDINATESYSTEMGEODETICDIRECTION_H_

/// \defgroup MgCoordinateSystemGeodeticDirection MgCoordinateSystemGeodeticDirection
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// This class contains the constants for all current transformation definition types.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticDirection
{

PUBLISHED_API:

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Direction not been specified.  This appears when the
    /// direction member has not bee set, or in the case where
    /// direction is not applicable, such as when the
    /// is the null transformation.
    static const INT32 GeodeticDirectionNone = 0;

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Forward direction, the calculation the geodetic
    /// transformation definitions natural calculation.
    /// I.e. conversion from the definition's "source" datum to the
    /// "target" datum.
    static const INT32 GeodeticDirectionForward = 1;

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Inverse direction, i.e. conversion from the definition's
    /// "target" datum to the "source" datum.  Often inmplies an
    /// iterative technique using the forward technique as the
    /// estimator.
    static const INT32 GeodeticDirectionInverse = 2;

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Use this value to indicate an error condition of some sort;
    /// especially useful when this type is the return value of a
    /// function.
    static const INT32 GeodeticDirectionError = -1;
};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICDIRECTION_H_
