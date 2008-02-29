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

#ifndef _CCOORDINATESYSTEMFACTORY_H_
#define _CCOORDINATESYSTEMFACTORY_H_

#ifdef _WIN32
#pragma warning( disable: 4251 )
#endif

namespace CSLibrary
{
///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The CCoordinateSystemFactory class supports creation and initialization of
/// CCoordinateSystem objects. For now this class only supports the creation
/// of a coordinate system object based on a OpenGIS Well-Known Text (WKT)
/// definition of the coordinate system. Future versions of this class may
/// support creation of a coordinate system instance based on an authority name
/// and a coordinate system id, code, or name.
///</summary>

class COORDINATE_SYSTEM_API CCoordinateSystemFactory
{
public:
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Constructs and intializes a coordinate system factory.
    ///</summary>
    CCoordinateSystemFactory();

    virtual ~CCoordinateSystemFactory();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Creates a coordinate system instance given a definition of the
    /// coordinate system in OpenGIS Well-Known Text (WKT) format.
    ///</summary>
    ///<param name="String/string srsWkt">
    /// A string defining the coordinate system in OpenGIS WKT format.
    ///</param>
    ///<returns>
    /// An CCoordinateSystem instance that corresponds to the specified WKT
    /// definition.
    ///</returns>
    virtual CCoordinateSystem* Create(CREFSTRING srsWkt);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMFACTORY_H_
