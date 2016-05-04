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

#ifndef _MGCOORDINATESYSTEMGRIDORIENTATION_H_
#define _MGCOORDINATESYSTEMGRIDORIENTATION_H_

/// \defgroup MgCoordinateSystemGridOrientation MgCoordinateSystemGridOrientation
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// An enumeration of the supported values for the <c>m_Orientation</c> member
/// of several objects related to grids and graticules. <p>
/// This value is used to qualify objects which are of the "iso" type.  I.e. a
/// grid line is referred to as an isoline as it is the locus of points which
/// have a specific value for either the easting or the northing.  It is this
/// value which indicates which.  Thus, a grid line which is classified as
/// having an "EastWest" orientation will be a isoline which is the locus of
/// points which share a common <b>easting</b> value, and the "m_Value" element
/// of that object will be an <b>easting</b> value.  Note that in this example,
/// the line is typically a vertical line.
class MG_GEOMETRY_API MgCoordinateSystemGridOrientation
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Not specified yet, initialize to this value.
    static const INT8 None = 0;
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// The object represents a constant easting value.
    static const INT8 EastWest = 1;         // generally indicates a vertical grid line
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// The object represents a constant northing value.
    static const INT8 NorthSouth = 2;       // generally indicates a horizontal grid line
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Indicates the failure of an algorithm or other problem.
    static const INT8 Unknown = 3;          // indicates a failure of an algorithm
};

/// \}

#endif //_MGCOORDINATESYSTEMGRIDORIENTATION_H_
