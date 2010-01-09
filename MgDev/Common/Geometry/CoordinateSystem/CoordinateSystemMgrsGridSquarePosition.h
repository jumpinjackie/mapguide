//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMMGRSGRIDSQUAREPOSITION_H_
#define _MGCOORDINATESYSTEMMGRSGRIDSQUAREPOSITION_H_

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// MgCoordinateSystemMgrsGridSquarePosition defines enumerated values used to
/// specify the the location desired relative to the subject MGRS grid sqaure
/// when evaluating an MGRS string.
///
/// Note that the values used in this enumeration match those used by the
// CS-MAP interface.
///</summary>
class MG_GEOMETRY_API MgCoordinateSystemMgrsGridSquarePosition
{
    // MENTOR_MAINTENANCE --> These enumeration values are assumed to be the
    // same as those used by CS-MAP.
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Specifies the grid square position is not set as yet.
    ///</summary>
    static const INT32 None = 0;
    ///<summary>
    /// Specifies the center of the grid square; the default previous to this
    /// reivison (Dec 2009).
    ///</summary>
    static const INT32 Center = 1;
    ///<summary>
    /// Specifies the southwest corner of the MGRS grid square.
    ///</summary>
    static const INT32 SouthWest = 2;
    ///<summary>
    /// Specifies the midpoint of the western edge of the MGRS grid square.
    ///</summary>
    static const INT32 West = 3;
    ///<summary>
    /// Specifies the northwest corner of the MGRS grid square.
    ///</summary>
    static const INT32 NorthWest = 4;
    ///<summary>
    /// Specifies the midpoint of the northern edge of the MGRS grid square.
    ///</summary>
    static const INT32 North = 5;
    ///<summary>
    /// Specifies the southwest corner of the MGRS grid square.
    ///</summary>
    static const INT32 NorthEast = 6;
    ///<summary>
    /// Specifies the midpoint of the eastern edge of the MGRS grid square.
    ///</summary>
    static const INT32 East = 7;
    ///<summary>
    /// Specifies the southeast corner of the MGRS grid square.
    ///</summary>
    static const INT32 SouthEast = 8;
    ///<summary>
    /// Specifies the midpoint of the southern edge of the MGRS grid square.
    ///</summary>
    static const INT32 South = 9;
    ///<summary>
    /// Specifies an error return value, end of table, or other abnormal situation.
    ///</summary>
    static const INT32 Unknown = 10;
};

#endif //_MGCOORDINATESYSTEMMGRSGRIDSQUAREPOSITION_H_
