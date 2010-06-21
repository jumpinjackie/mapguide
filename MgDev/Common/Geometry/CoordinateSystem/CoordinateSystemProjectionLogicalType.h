//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMPROJECTIONLOGICALTYPE_H_
#define _MGCOORDINATESYSTEMPROJECTIONLOGICALTYPE_H_

/// \defgroup MgCoordinateSystemProjectionLogicalType MgCoordinateSystemProjectionLogicalType
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
///
///
class MG_GEOMETRY_API MgCoordinateSystemProjectionLogicalType
{
PUBLISHED_API:
    static const INT32 None = 0;                //no specification
    static const INT32 Longitude = 1;           //longitude
    static const INT32 Latitude = 2;            //latitude
    static const INT32 Azimuth = 3;             //azimuth
    static const INT32 AngularDistance = 4;     //angular distance
    static const INT32 ComplexCoefficient = 5;  //complex coefficient
    static const INT32 UTMZoneNumber = 6;       //UTM zone number
    static const INT32 HemisphereSelection = 7; //hemisphere selection: north > 0, south < 0
    static const INT32 GeoidHeight = 8;         //geoid height in meters
    static const INT32 Elevation = 9;           //elevation, system units
    static const INT32 AffineCoefficient = 10;  //affine coefficient
    static const INT32 XYCoordinate = 11;       //X/Y Coordinate
    static const INT32 Scale = 12;              //Scale value
};
/// \}

#endif //_MGCOORDINATESYSTEMPROJECTIONLOGICALTYPE_H_
