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

#ifndef _MGCOORDINATESYSTEMWKTFLAVOR_H_
#define _MGCOORDINATESYSTEMWKTFLAVOR_H_

/// \defgroup MgCoordinateSystemWktFlavor MgCoordinateSystemWktFlavor
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// Defines constants used to indicate the WKT flavor
///
class MG_GEOMETRY_API MgCoordinateSystemWktFlavor
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Unknown flavor
    ///
    static const INT32 Unknown = -1;
    ///////////////////////////////////////////////////////////////
    /// \brief
    /// OGC flavor
    ///
    static const INT32 Ogc = 0;
    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Oracle flavor
    ///
    static const INT32 Oracle = 1;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// ESRI flavor
    ///
    static const INT32 Esri = 2;
    //////////////////////////////////////////////////////////////
    /// \brief
    /// EPSG flavor
    ///
    static const INT32 Epsg = 3;
    //////////////////////////////////////////////////////////////
    /// \brief
    /// GeoTIFF flavor
    ///
    static const INT32 Geotiff = 4;
    //////////////////////////////////////////////////////////////
    /// \brief
    /// GeoTools flavor
    ///
    static const INT32 Geotools = 5;
};
/// \}

#endif //_MGCOORDINATESYSTEMWKTFLAVOR_H_
