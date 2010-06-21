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

#ifndef _MGCOORDINATESYSTEMPROJECTIONPARAMETERTYPE_H_
#define _MGCOORDINATESYSTEMPROJECTIONPARAMETERTYPE_H_

/// \defgroup MgCoordinateSystemProjectionParameterType MgCoordinateSystemProjectionParameterType
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
///
///
class MG_GEOMETRY_API MgCoordinateSystemProjectionParameterType
{
PUBLISHED_API:
    static const INT32 NotUsed = 0;         //Not used.
    static const INT32 Cntmer = 1;          //Central Meridian
    static const INT32 Nstdpll = 2;         //Northern Standard Parallel
    static const INT32 Sstdpll = 3;         //Southern Standard Parallel
    static const INT32 Stdpll = 4;          //Standard Parallel
    static const INT32 Gcp1lng = 5;         //First Great Circle Point Longitude
    static const INT32 Gcp1lat = 6;         //First Great Circle Point Latitude
    static const INT32 Gcp2lng = 7;         //Second Great Circle Point Longitude
    static const INT32 Gcp2lat = 8;         //Second Great Circle Point Latitude
    static const INT32 Gcplng = 9;          //Great Circle Point Longitude
    static const INT32 Gcplat = 10;         //Great Circle Point Latitude
    static const INT32 Gcazm = 11;          //Great Circle Azimuth
    static const INT32 Yaxisaz = 12;        //Y axis azimuth
    static const INT32 Estdmer = 13;        //Eastern Standard Meridian
    static const INT32 Nparall = 14;        //Northern Parallel
    static const INT32 Sparall = 15;        //Southern Parallel
    static const INT32 P1lng = 16;          //First pole longitude
    static const INT32 P1lat = 17;          //First pole latitude
    static const INT32 P2lng = 18;          //Second pole longitude
    static const INT32 P2lat = 19;          //Second pole latitude
    static const INT32 Adp1p2 = 20;         //Distance between the poles.
    static const INT32 Adsp1 = 21;          //Angular distance to the first standard parallel
    static const INT32 Adsp2 = 22;          //Angular distance to the second standard parallel
    static const INT32 Cmplxan = 23;        //Complex Parameter A (n). Label string includes a %s for generation by sprintf.
    static const INT32 Cmplxbn = 24;        //Complex Parameter B (n). Label string includes a %s for generation by sprintf.
    static const INT32 Westll = 25;         //Western end of Longitude range.
    static const INT32 Eastll = 26;         //Eastern end of Longitude range.
    static const INT32 Utmzn = 27;          //UTM Zone Number
    static const INT32 Hsns = 28;           //North/South Hemisphere
    static const INT32 Ghgt = 29;           //Average Geoid Height
    static const INT32 Aelev = 30;          //Average Elevation
    static const INT32 Polelng = 31;        //Oblique Pole Longitude
    static const INT32 Polelat = 32;        //Oblique Pole Latitude
    static const INT32 Ostdpll = 33;        //Oblique Cone Standard Parallel
    static const INT32 Stdcir = 34;         //Standard Circle Latitude
    static const INT32 Afa0 = 35;           //Affine A0 Coefficient
    static const INT32 Afb0 = 36;           //Affine B0 Coefficient
    static const INT32 Afa1 = 37;           //Affine A1 Coefficient
    static const INT32 Afa2 = 38;           //Affine A2 Coefficient
    static const INT32 Afb1 = 39;           //Affine B1 Coefficient
    static const INT32 Afb2 = 40;           //Affine B2 Coefficient
    static const INT32 Nrmlpll = 41;        //Normal Parallel
    static const INT32 Denrgn = 42;         //Danish System 24 Region
    static const INT32 Skwazm = 43;         //cs_PRMCOD_SKWAZM - Skew Azimuth at Rectified Origin.
    static const INT32 Sclrotorgx = 44;     //X Coordinate of Scale/Roatet Origin
    static const INT32 Sclrotorgy = 45;     //Y Coordinate of Scale/Roatet Origin
    static const INT32 Nrthscl = 46;        //Cartesian Scale Factor
    static const INT32 Nrthrot = 47;        //Cartesian Rotation Angle
};
/// \}

#endif //_MGCOORDINATESYSTEMPROJECTIONPARAMETERTYPE_H_
