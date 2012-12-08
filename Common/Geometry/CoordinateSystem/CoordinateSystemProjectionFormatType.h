//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful;
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not; write to the Free Software
//  Foundation; Inc.; 51 Franklin St; Fifth Floor; Boston; MA  02110-1301  USA
//

#ifndef _MGCOORDINATESYSTEMPROJECTIONFORMATTYPE_H_
#define _MGCOORDINATESYSTEMPROJECTIONFORMATTYPE_H_

/// \defgroup MgCoordinateSystemProjectionFormatType MgCoordinateSystemProjectionFormatType
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// Defines the type of projection format
///
class MG_GEOMETRY_API MgCoordinateSystemProjectionFormatType
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// none (no formatting)
    ///
    static const INT32 None = 0;        
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// longitude(cs_FRMT_LNG), azimuth(cs_FRMT_AZM)
    ///
    static const INT32 Lng = 57509;     
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// latitude (cs_FRMT_LAT)
    ///
    static const INT32 Lat = 41157;     
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// angular distance (cs_FRMT_ANGD)
    ///
    static const INT32 Angd = 32933;    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// coefficient (cs_FRMT_COEF)
    ///
    static const INT32 Coef = 13;       
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// coefficient (cs_FRMT_XXX)
    ///
    static const INT32 Xxx = 4099;      
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// coefficient (cs_FRMT_YYY)
    ///
    static const INT32 Yyy = 4099;      
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// coefficient (cs_FRMT_SCL)
    ///
    static const INT32 Scl = 7;         
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// coefficient (cs_FRMT_ROT)
    ///
    static const INT32 Rot = 4;         
};
/// \}

#endif //_MGCOORDINATESYSTEMPROJECTIONFORMATTYPE_H_
