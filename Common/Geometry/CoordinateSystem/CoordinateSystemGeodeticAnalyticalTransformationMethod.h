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

    static const INT32 None = 0;

    static const INT32 ThreeParameter =     8193;     //cs_DTCMTH_3PARM
    static const INT32 Molodensky =         8194;     //cs_DTCMTH_MOLOD
    static const INT32 AbridgedMolodensky = 8195;     //cs_DTCMTH_AMOLO
    static const INT32 Geocentric =         8196;     //cs_DTCMTH_GEOCT
    static const INT32 FourParameter =      8197;     //cs_DTCMTH_4PARM
    static const INT32 SixParameter =       8198;     //cs_DTCMTH_6PARM
    static const INT32 Bursa =              8199;     //cs_DTCMTH_BURSA
    static const INT32 Frame =              8200;     //cs_DTCMTH_FRAME
    static const INT32 SevenParameter =     8201;     //cs_DTCMTH_7PARM
    static const INT32 MolodenskyBadekas =  8202;     //cs_DTCMTH_BDKAS

};
/// \}

#endif //_MGCOORDINATESYSTEMGEODETICANALYTICALTRANSFORMATIONMETHOD_H_
