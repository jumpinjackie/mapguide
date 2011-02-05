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

// Polygon definitions for common SLD symbols.
// Data is designed to fit in a unit square

#ifndef SLDSYMBOLS_H_
#define SLDSYMBOLS_H_

#include "StylizationDefs.h"


// Identifiers - do not change these values
enum SLDType
{
    SLDType_Square   = 0,
    SLDType_Circle   = 1,
    SLDType_Triangle = 2,
    SLDType_Star     = 3,
    SLDType_Cross    = 4,
    SLDType_X        = 5,
    SLDType_Error    = 6
};


//////////////////////////////////////////////
// Square
//
static const double SLD_SQUARE[] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0
};


//////////////////////////////////////////////
// Circle, approximated by 20-gon
//
static const double incr = M_PI / 10.0;
#define circpt(x) 0.5 + 0.5 * cos(x * incr), 0.5 + 0.5 * sin(x * incr)

static const double SLD_CIRCLE[] = {
        circpt( 0.0), circpt( 1.0), circpt( 2.0), circpt( 3.0),
        circpt( 4.0), circpt( 5.0), circpt( 6.0), circpt( 7.0),
        circpt( 8.0), circpt( 9.0), circpt(10.0), circpt(11.0),
        circpt(12.0), circpt(13.0), circpt(14.0), circpt(15.0),
        circpt(16.0), circpt(17.0), circpt(18.0), circpt(19.0),
        circpt( 0.0)
};


//////////////////////////////////////////////
// Equilateral triangle
//
static const double sq34 = 0.5 - 0.25 * sqrt(3.0);

static const double SLD_TRIANGLE[] = {
        0.0,       sq34,
        1.0,       sq34,
        0.5, 1.0 - sq34,
        0.0,       sq34
};


//////////////////////////////////////////////
// Star (pentagonal)
//
// CCW list order
//                       6
//                       |
//                  8___7|5___4
//                     9/1\3
//                     /   \
//                    0     2
//
static const double cos36 = cos(36.0 * M_PI180);
static const double cos18 = cos(18.0 * M_PI180);
static const double sin36 = sin(36.0 * M_PI180);
static const double plen  = 1.0 - 0.5 / cos36; //length of the side of star point edge

static const double SLD_STAR[] = {
        1.0 - cos36,                0.5 - 0.5 * cos18,
        1.0 - cos36 + plen * cos36, 0.5 - 0.5 * cos18 + plen * sin36,
        cos36,                      0.5 - 0.5 * cos18,
        1.0 - plen * cos36,         sin36 - plen * sin36,
        1.0,                        sin36,
        1.0 - plen,                 sin36,
        0.5,                        0.5 + 0.5 * cos18,
        plen,                       sin36,
        0.0,                        sin36,
        plen * cos36,               sin36 - plen * sin36,
        1.0 - cos36,                0.5 - 0.5 * cos18
};


//////////////////////////////////////////////
// Cross (looks like plus sign)
//

// constant to derive a thickness for the cross
static const double phi = 1.6180339887498948482045868343656; // Golden ratio
static const double t = 0.5 / (2.0 * (phi + 1.0));

static const double SLD_CROSS[] = {
        0.5 - t, 0.0,
        0.5 + t, 0.0,
        0.5 + t, 0.5 - t,
        1.0,     0.5 - t,
        1.0,     0.5 + t,
        0.5 + t, 0.5 + t,
        0.5 + t, 1.0,
        0.5 - t, 1.0,
        0.5 - t, 0.5 + t,
        0.0,     0.5 + t,
        0.0,     0.5 - t,
        0.5 - t, 0.5 - t,
        0.5 - t, 0.0
};


//////////////////////////////////////////////
// X
//
static const double t2 = 2.0 * t / sqrt(2.0);

static const double SLD_X[] = {
        t2,       0.0,
        0.5,      0.5 - t2,
        1.0 - t2, 0.0,
        1.0,      t2,
        0.5 + t2, 0.5,
        1.0,      1.0 - t2,
        1.0 - t2, 1.0,
        0.5,      0.5 + t2,
        t2,       1.0,
        0.0,      1.0 - t2,
        0.5 - t2, 0.5,
        0.0,      t2,
        t2,       0.0
};


//////////////////////////////////////////////
// Simple x symbol for use in error conditions
//
static const double SLD_ERROR[] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0
};

#endif
