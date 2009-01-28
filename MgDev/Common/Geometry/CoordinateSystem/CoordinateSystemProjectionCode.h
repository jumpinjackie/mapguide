//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _MGCOORDINATESYSTEMPROJECTIONCODE_H_
#define _MGCOORDINATESYSTEMPROJECTIONCODE_H_

/// \defgroup MgCoordinateSystemProjectionCode MgCoordinateSystemProjectionCode
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
///
///
class MG_GEOMETRY_API MgCoordinateSystemProjectionCode
{
PUBLISHED_API:
    static const INT32 Alber = 4;           // Albers Equal Area Conic Projection
    static const INT32 Azede = 59;          // Azimuthal Equi-Distant; Elevated ellipsoid.
    static const INT32 Azmea = 11;          // Lambert Azimuthal Equal Area Projection
    static const INT32 Azmed = 7;           // Lambert Azimuthal Equidistant Projection
    static const INT32 Bipolar = 31;        // Bipolar Oblique Conformal Conic Projection
    static const INT32 Bonne = 24;          // Bonne Pseudoconical Projection
    static const INT32 Cassini = 22;        // Cassini Projection
    static const INT32 Eckert4 = 25;        // Ekert Pseudocylindrical Projection; Number IV
    static const INT32 Eckert6 = 26;        // Ekert Pseudocylindrical Projection; Number VI
    static const INT32 Edcnc = 12;          // Equidistant Conic Projection; aka Simple Conic
    static const INT32 Edcyl = 20;          // Equidistant Cylindrical Projection
    static const INT32 GaussK = 46;         // Gauss-Kruger: Transverse Mercator without scale reduction parameter.
    static const INT32 Gnomonic = 19;       // Gnomonic Projection
    static const INT32 Goode = 28;          // Goode Homolosine Projection
    static const INT32 Hom1uv = 1281;       // Unrectified Hotine Oblique Mercator Projection; Sngl Point Form
    static const INT32 Hom1xy = 1282;       // Rectified Hotine Oblique Mercator Projection; Single Point Form
    static const INT32 Hom2uv = 1283;       // Unrectified Hotine Oblique Mercator Projection; Two Point Form
    static const INT32 Hom2xy = 1284;       // Rectified Hotine Oblique Mercator Projection; Two Point Form
    static const INT32 Krovak = 47;         // Czech Krovak; original.
    static const INT32 Krvk95 = 51;         // Czech Krovak; includes 1995 adjustment.
    static const INT32 LL = 1;              // Null Projection; produces/processes Latitude & Longitude
    static const INT32 Lm1sp = 36;          // Single standard parallel variation of the Lambert Conformal Conic.
    static const INT32 Lm2sp = 37;          // Double standard parallel variation of the Lambert Conformal Conic.
    static const INT32 Lmblg = 38;          // Belgian variation of the Lambert Conformal Conic Projection.
    static const INT32 Lmtan = 8;           // Lambert Tangential Conformal Conic Projection
    static const INT32 Miller = 13;         // Miller Cylindrical Projection
    static const INT32 Mndotl = 41;         // Minnesota DOT variation of the Lambert Conformal Conic.
    static const INT32 Mndott = 42;         // Minnesota DOT variation of the Transverse Mercator projection.
    static const INT32 Modpc = 10;          // Lallemand IMW Modified Polyconic Projection
    static const INT32 Mollweid = 27;       // Mollweide Projection
    static const INT32 Mrcat = 6;           // Mercator Cylindrical Projection
    static const INT32 MrcatK = 49;         // Standard Mercator with a scale reduction factor instead of a standard parallel.
    static const INT32 Mstero = 15;         // Modified Sterographic Projection
    static const INT32 Neacyl = 29;         // Normal Aspect; Equal Area Cylindrical Projection
    static const INT32 Nerth = 55;          // Non-georeferenced coordinate system.  Named Non-Earth by Map Info.
    static const INT32 Nzealand = 16;       // New Zealand National Grid
    static const INT32 OblqM = 5;         // Oblique Mercator (obsolete)
    static const INT32 Obqcyl = 56;         // Oblique Cylindrical; a generalized version of the Swiss projection; specifically for Hungary
    static const INT32 Ortho = 18;          // Orthographic Projection
    static const INT32 Ostn02 = 60;         // The Transverse Mercator with specific parameters; with the OSTN02 grid shift tacked on.  This is a combination of a projection and a datum shift
    static const INT32 Ostn97 = 58;         // The Transverse Mercator with specific parameters; with the OSTN97 grid shoft tacked on.  This is a combination of a projection and a datum shift
    static const INT32 Ostro = 34;          // Oblique Sterographic
    static const INT32 Plycn = 9;           // Hassler American Polyconic Projection
    static const INT32 Pstro = 33;          // Polar sterographic
    static const INT32 Pstrosl = 53;        // Polar sterographic with standard latitude
    static const INT32 Rskew = 1285;        // cs_PRJCOD_RSKEW
    static const INT32 Rskewc = 1286;       // cs_PRJCOD_RSKEWC
    static const INT32 Rskewo = 1287;       // cs_PRJCOD_RSKEWO - Rectified Skew Orthomorphic; Skew Azimuth at Rectified Origin
    static const INT32 Robinson = 23;       // Robinson Cylindrical Projection
    static const INT32 Sinus = 17;          // Sinusoidal Projection; Optionally Interrupted
    static const INT32 Sotrm = 43;          // South Oriented variation of the Transverse Mercator Projection.
    static const INT32 Sstro = 35;          // Synder's Oblique Sterographic
    static const INT32 Swiss = 32;          // "Swiss" Projection
    static const INT32 Sys34 = 57;          // Combination of Transverse Mercator and a polynomial expansion used in Denmark
    static const INT32 Sys34_99 = 61;       // Combination of Transverse Mercator and polynomial expansion used in Denmark.  Polynominals are of the 1999 vintage.
    static const INT32 Teacyl = 30;         // Transverse Aspect; Equal Area Cylindrical Projection
    static const INT32 Tm = 3;              // Transverse Mercator or Gauss Kruger Projection
    static const INT32 Trmeraf = 54;        // Transverse Mercator with affiine post-processor.
    static const INT32 Trmrkrg = 62;        // Transverse Mercator using Kruger Formulation
    static const INT32 Trmrs = 45;          // Transverse Mercator per J. P. Snyder.
    static const INT32 Vdgrntn = 21;        // Van Der Grinten Projection
    static const INT32 Wccsl = 39;          // Wisconsin County Coord System variation; Lambert Conformal Conic
    static const INT32 Wccst = 40;          // Wisconsin County Coord System variation; Transverse Mercator projection
    static const INT32 Utm = 44;            // The UTM direct variation of the Transverse Mercator projection
    static const INT32 Winkl = 63;          // Winkel-Tripel; variable standard latitude
    static const INT32 Nrthsrt = 64;        // Nerth with scale and rotation
    static const INT32 Lmbrtaf = 65;        // Lambert Conformal Conic with affine post-processor.
    static const INT32 Unknown = 0;
};
/// \}

#endif //_MGCOORDINATESYSTEMPROJECTIONCODE_H_
