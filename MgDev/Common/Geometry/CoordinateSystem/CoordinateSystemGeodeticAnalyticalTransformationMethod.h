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

#ifndef _MGCOORDINATESYSTEMGEODETICANALYTICALTRANSFORMATIONMETHOD_H_
#define _MGCOORDINATESYSTEMGEODETICANALYTICALTRANSFORMATIONMETHOD_H_

/// \defgroup MgCoordinateSystemGeodeticAnalyticalTransformationMethod MgCoordinateSystemGeodeticAnalyticalTransformationMethod
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// This class defines the currently supported transformation methods for
/// analytical geodetic transformations as they can be read via the
/// MgCoordinateSystemGeodeticTransformDefDictionary. That is, the constants
/// defined below are only valid for MgCoordinateSystemGeodeticTransformDef instances
/// of type MgCoordinateSystemGeodeticTransformDefType.Analytical.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticAnalyticalTransformationMethod
{
PUBLISHED_API:
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// None
    ///
    static const INT32 None = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Three Parameter
    ///
    static const INT32 ThreeParameter =     8193;     //cs_DTCMTH_3PARM

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Molodensky
    ///
    static const INT32 Molodensky =         8194;     //cs_DTCMTH_MOLOD

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Abridged Molodensky
    ///
    static const INT32 AbridgedMolodensky = 8195;     //cs_DTCMTH_AMOLO

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Geocentric
    ///
    static const INT32 Geocentric =         8196;     //cs_DTCMTH_GEOCT

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Four Parameter
    ///
    static const INT32 FourParameter =      8197;     //cs_DTCMTH_4PARM

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Six Parameter
    ///
    static const INT32 SixParameter =       8198;     //cs_DTCMTH_6PARM

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Bursa
    ///
    static const INT32 Bursa =              8199;     //cs_DTCMTH_BURSA

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Frame
    ///
    static const INT32 Frame =              8200;     //cs_DTCMTH_FRAME

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Seven Parameter
    ///
    static const INT32 SevenParameter =     8201;     //cs_DTCMTH_7PARM

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Molodensky Badekas
    ///
    static const INT32 MolodenskyBadekas =  8202;     //cs_DTCMTH_BDKAS

};
/// \}

#endif //_MGCOORDINATESYSTEMGEODETICANALYTICALTRANSFORMATIONMETHOD_H_
