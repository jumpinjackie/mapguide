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

#ifndef _MGCOORDINATESYSTEMMGRSGRIDLEVEL_H_
#define _MGCOORDINATESYSTEMMGRSGRIDLEVEL_H_

/// \defgroup MgCoordinateSystemMgrsGridLevel MgCoordinateSystemMgrsGridLevel
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// Defines constants used to indicate the MGRS grid level
///
class MG_GEOMETRY_API MgCoordinateSystemMgrsGridLevel
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// None
    ///
    static const INT32 MgrsNone    =  0;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// UTM
    ///
    static const INT32 MgrsUtm     =  1;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// UPS
    ///
    static const INT32 MgrsUps     =  2;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// 100km
    ///
    static const INT32 Mgrs100Km   =  3;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// 10km
    ///
    static const INT32 Mgrs10Km    =  4;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// 1km
    ///
    static const INT32 Mgrs1Km     =  5;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// 100m
    ///
    static const INT32 Mgrs100m    =  6;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// 10m
    ///
    static const INT32 Mgrs10m     =  7;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// 1m
    ///
    static const INT32 Mgrs1m      =  8;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Unknown 
    ///
    static const INT32 MgrsUnknown = 99;
};

/// \}

#endif //_MGCOORDINATESYSTEMMGRSGRIDLEVEL_H_
