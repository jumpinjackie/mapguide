//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMGRIDSPECIALIZATIONTYPE_H_
#define _MGCOORDINATESYSTEMGRIDSPECIALIZATIONTYPE_H_

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// An enumeration of the various types of specialized grids/graticules
/// currently supported.  A specialized grid is one for which specific
/// standards exist to which the results of this feature are compliant with.
/// A generic grid is a simple non-standardized grid of a coordinate system
/// drawn in a viewport based on another coordinate system. <p>
/// The values assigned are intended to support grouping standardized grids
/// with similar features and is currently entirely arbitrary.  Using a
/// numeric literal instead of thes names of the defined constants is a sure
/// way to write code that will get broken in the future.
/// </summary>
class MG_GEOMETRY_API MgCoordinateSystemGridSpecializationType
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// /brief Not specified yet, initialize to this value.
    static const INT32 None              = 0;
    ///////////////////////////////////////////////////////////////////////////
    /// /brief Generic grid of a specified coordinate system; may be
    /// geographic or projected.
    static const INT32 Generic           = (0 + 1);     // Generic grid of a specified coordinate system;
                                                        // may be geographic or projected
    ///////////////////////////////////////////////////////////////////////////
    /// /brief Specialized grid: MGRS (Military Grid Reference System)
    static const INT32 MGRS              = (16 + 1);
    ///////////////////////////////////////////////////////////////////////////
    /// /brief Specialized grid: USNG (United States National Grid)
    static const INT32 USNG              = (16 + 2);
    ///////////////////////////////////////////////////////////////////////////
    /// /brief Indicates the failure of an algorithm or other problem.
    static const INT32 Unknown           = (65366);
};

#endif //_MGCOORDINATESYSTEMGRIDSPECIALIZATIONTYPE_H_
