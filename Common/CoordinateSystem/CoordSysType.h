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

#ifndef _CCOORDINATESYSTEMTYPE_H_
#define _CCOORDINATESYSTEMTYPE_H_

namespace CSLibrary
{
///////////////////////////////////////////////////////////////////////////////
///<summary>
/// CCoordinateSystemType defines enumerated values used to determine the type
/// of a coordinate system.
///</summary>

class COORDINATE_SYSTEM_API CCoordinateSystemType
{
public:
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Specifies the coordinate system is not set.
    ///</summary>
    static const int Unknown = 0;
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Specifies the coordinate system is arbitrary, e.g. not spatially bound
    /// to the earth.
    ///</summary>
    static const int Arbitrary = 1;
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Specifies the coordinate system is geographic.
    ///</summary>
    static const int Geographic = 2;
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Specifies the coordinate system is projected.
    ///</summary>
    static const int Projected = 3;
};

} // End of namespace

#endif //_CCOORDINATESYSTEMTYPE_H_
