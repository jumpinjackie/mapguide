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

#ifndef _MGCOORDINATESYSTEMGEODETICTRANSFORMGRIDFILEFORMAT_H_
#define _MGCOORDINATESYSTEMGEODETICTRANSFORMGRIDFILEFORMAT_H_

/// \defgroup MgCoordinateSystemGeodeticTransformGridFileFormat MgCoordinateSystemGeodeticTransformGridFileFormat
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// This class contains the constants for all currently supported grid file formats.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticTransformGridFileFormat
{
PUBLISHED_API:

    static const INT32 None = 0;
    static const INT32 NTv1 = 1;
    static const INT32 NTv2 = 2;
    static const INT32 NADCON = 3;
    static const INT32 RGF = 4;
    static const INT32 PAR = 5;
    static const INT32 ATS77 = 6;
};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICTRANSFORMGRIDFILEFORMAT_H_