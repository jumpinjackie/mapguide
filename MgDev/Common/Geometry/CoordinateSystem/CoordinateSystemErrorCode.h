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

#ifndef _MGCOORDINATESYSTEMERRORCODE_H_
#define _MGCOORDINATESYSTEMERRORCODE_H_

/// \defgroup MgCoordinateSystemErrorCode MgCoordinateSystemErrorCode
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
///
///
class MG_GEOMETRY_API MgCoordinateSystemErrorCode
{
PUBLISHED_API:
    static const INT32 AZM      = 201;    // Y axis azimuth specification is invalid.
    static const INT32 AZMTH    = 202;    // Great circle azimuth value is invalid.
    static const INT32 LAT      = 203;    // Invalid latitude detected in definition.
    static const INT32 LATEQU   = 204;    // Latitudes of defining points are equal.
    static const INT32 LNG      = 205;    // Invalid longitude detected in definition.
    static const INT32 LNGEQU   = 206;    // First defining point may not be on the equator.
    static const INT32 MAPSCL   = 207;    // Invalid map scale value detected.
    static const INT32 MEREQU   = 208;    // Standard meridians are the same.
    static const INT32 MRCAT    = 209;    // Defining great circle is a parallel, try Mercator.
    static const INT32 MSCOEF   = 210;    // Invalid coefficient value specified.
    static const INT32 NOREF    = 211;    // System must be referenced to a datum or an ellipsoid.
    static const INT32 NOTNRTH  = 212;    // Standard paralells are not ordered correctly.
    static const INT32 NRTHLAT  = 213;    // Standard parallells are not ordered correctly.
    static const INT32 NRTHPNT  = 214;    // Point defining Y axis direction incorrectly specified.
    static const INT32 ORGLAT   = 215;    // Invalid latitude value given for projection origin.
    static const INT32 ORGLNG   = 216;    // Invalid longitude value given for projection origin.
    static const INT32 PLL90    = 217;    // Standard parallel location exceeds 90 degrees.
    static const INT32 PLLEQU   = 218;    // Standard parallel locations are the same.
    static const INT32 PLLLRG   = 219;    // Standard parallel locations not inbetween poles.
    static const INT32 PLLREV   = 220;    // Order of standard parallels incorrect.
    static const INT32 PLLZERO  = 221;    // Standard parallel location is zero.
    static const INT32 POLDD    = 222;    // Poles are too far apart.
    static const INT32 POLDUP   = 223;    // Poles are not sufficiently separated.
    static const INT32 POLLAT   = 224;    // Latitude of pole incorrectly specified.
    static const INT32 POLLNG   = 225;    // Longitude of pole incorrectly specified.
    static const INT32 QUAD     = 226;    // Invalid quad specification detected.
    static const INT32 SCLRED   = 227;    // Invalid scale reduction value encountered.
    static const INT32 SOTHLAT  = 228;    // Standard parallels are not ordered correctly.
    static const INT32 STDLAT   = 229;    // Standard parallel is incorrectly specified.
    static const INT32 STDLNG   = 230;    // Standard meridian specification is invalid.
    static const INT32 STDPLL   = 231;    // Standard parallel specifications are invalid.
    static const INT32 STDSOU   = 232;    // Southern parallel is equal to or north of northern.
    static const INT32 STDWEST  = 233;    // Standard meridian must be east of the central meridian.
    static const INT32 UNIT     = 234;    // Invalid unit specification detected.
    static const INT32 INVPRJ   = 235;    // Invalid projection key name detected.
    static const INT32 INVDTM   = 236;    // Invalid datum name specification detected.
    static const INT32 INVELP   = 237;    // Invalid ellipsoid name specification detected.
    static const INT32 LLRNG    = 238;    // Longitude range is too small.
    static const INT32 RNGORD   = 239;    // Longitude range values out of order.
    static const INT32 INVQUAD  = 240;    // Invalid quadrant specification given.
    static const INT32 GEOMM    = 241;    // Invalid invalid geographic useful range detected.
    static const INT32 CRTMM    = 242;    // Invalid invalid cartesian useful range detected.
    static const INT32 PLLED    = 243;    // Standard parallels must be equidistant from poles.
    static const INT32 PLRLAT   = 244;    // Origin latitude is not polar; use oblique form of projection.
    static const INT32 USEPLR   = 245;    // Origin latitude is polar; use polar form of projection.
    static const INT32 UTMZON   = 246;    // Invalid value given for UTM zone number.
    static const INT32 HMISPHR  = 247;    // Invalid hemisphere specification (+1 = north, -1 = south).
    static const INT32 USESW    = 248;    // Central azimuth is 90, use the Swiss Oblique Mercator.
    static const INT32 MAX15    = 249;    // Eastern meridian is more than 15 degrees from the central meridian.
    static const INT32 OBLQPOLE = 250;    // Invalid oblique pole specification.
    static const INT32 AFFZERO  = 251;    // Denominator of affine is zero.
    static const INT32 NRDATUM  = 252;    // Non-earth system referenced to a datum.
    static const INT32 OSTN97   = 253;    // OSTN97.TXT grid shift file could not be located or opened with read access.
    static const INT32 DENRGN   = 253;    // Invalid region code specified for Danish System 34/45 (1=J, 2=S, 3=B).
    static const INT32 ELEVEL   = 254;    // Value supplied for elevated ellipsoid is outside acceptable range
    static const INT32 OSTN02   = 255;    // OSTN02.TXT grid shift file could not be located or opened with read access.
    static const INT32 TMKRG0   = 256;    // Transverse Mercator Kruger formulation requested with non-zero origin latitude.

    static const INT32 Ok                   = 1000;
    static const INT32 OutOfMemory          = 1001;
    static const INT32 InitializationFailed = 1002;
    static const INT32 ConversionFailed     = 1003;
    static const INT32 NullArgument         = 1004;
    static const INT32 InvalidArgument      = 1005;
};
/// \}

#endif //_MGCOORDINATESYSTEMERRORCODE_H_
