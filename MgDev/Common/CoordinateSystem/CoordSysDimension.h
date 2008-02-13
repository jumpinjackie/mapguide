//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _CCOORDINATESYSTEMDIMENSION_H_
#define _CCOORDINATESYSTEMDIMENSION_H_

namespace CSLibrary
{
///////////////////////////////////////////////////////////////////////////////
///<summary>
/// CCoordinateSystemDimension defines enumerated values used to determine the
/// # of dimensions of a coordinate system.
///</summary>

class COORDINATE_SYSTEM_API CCoordinateSystemDimension
{
public:
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Specifies the coordinate system dimension is not set.
    ///</summary>
    static const int Unknown = 0;
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Specifies the coordinate system is 2-D
    ///</summary>
    static const int  XY = 2;
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Specifies the coordinate system 3-D.
    ///</summary>
    static const int XYZ = 3;
};

} // End of namespace

#endif //_CCOORDINATESYSTEMDIMENSION_H_
