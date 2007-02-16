//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

//Polygon definitions for common SLD symbols.
//Data is designed to fit in a unit square

#ifndef SLDSYMBOLS_H
#define SLDSYMBOLS_H

#define _USE_MATH_DEFINES
#include <math.h>


//names
static const wchar_t* SLD_SQUARE_NAME =   L"sq";
static const wchar_t* SLD_CIRCLE_NAME =   L"c";
static const wchar_t* SLD_TRIANGLE_NAME = L"t";
static const wchar_t* SLD_X_NAME =        L"x";
static const wchar_t* SLD_STAR_NAME =     L"st";
static const wchar_t* SLD_CROSS_NAME =    L"p";


//////////////////////////////////////////////
//square
static const double SLD_SQUARE[] = {
                          0., 0.,
                          1., 0.,
                          1., 1.,
                          0., 1.,
                          0., 0.
};


//////////////////////////////////////////////
static const double sq34 = 0.5 - 0.25 * sqrt(3.);

//equilateral triangle
static const double SLD_TRIANGLE[] = {
                            0.,  sq34,
                            1.,  sq34,
                            0.5, 1. - sq34,
                            0.,  sq34
};


//////////////////////////////////////////////
static const double cos36 = cos(36. * M_PI / 180.);
static const double cos18 = cos(18. * M_PI / 180.);
static const double sin36 = sin(36. * M_PI / 180.);
static const double plen  = 1. - 0.5 / cos36; //length of the side of star point edge

//star (pentagonal)
// CCW list order
//                       6
//                       |
//                  8___7|5___4
//                     9/1\3
//                     /   \
//                    0     2

static const double SLD_STAR[] = {
        1. - cos36,                 0.5 - 0.5 * cos18,
        1. - cos36 + plen * cos36,  0.5 - 0.5 * cos18 + plen * sin36,
        cos36,                      0.5 - 0.5 * cos18,
        1. - plen * cos36,          sin36 - plen * sin36,
        1.,                         sin36,
        1. - plen,                  sin36,
        0.5,                        0.5 + 0.5 * cos18,
        plen,                       sin36,
        0.,                         sin36,
        plen * cos36,               sin36 - plen * sin36,
        1. - cos36,                 0.5 - 0.5 * cos18
};

//////////////////////////////////////////////

//constant to derive a thickness for the plus
static const double phi = 1.6180339887498948482045868343656; //Golden ratio
static const double t = 0.5 / (2. * (phi + 1.));

//cross (looks like plus sign)
static const double SLD_CROSS[] = {
        0.5 - t, 0.,
        0.5 + t, 0.,
        0.5 + t, 0.5 - t,
        1.,      0.5 - t,
        1.,      0.5 + t,
        0.5 + t, 0.5 + t,
        0.5 + t, 1.,
        0.5 - t, 1.,
        0.5 - t, 0.5 + t,
        0.,      0.5 + t,
        0.,      0.5 - t,
        0.5 - t, 0.5 - t,
        0.5 - t, 0.
};


//////////////////////////////////////////////

static const double t2 = 2. * t / sqrt(2.);

static const double SLD_X[] = {
        t2,       0.,
        0.5,      0.5 - t2,
        1. - t2,  0.,
        1.,       t2,
        0.5 + t2, 0.5,
        1.,       1. - t2,
        1. - t2,  1.,
        0.5,      0.5 + t2,
        t2,       1.,
        0.,       1. - t2,
        0.5 - t2, 0.5,
        0.,       t2,
        t2,       0.
};

//////////////////////////////////////////////

static const double incr = M_PI / 10.;
#define circpt(x) 0.5 + 0.5 * cos(x * incr), 0.5 + 0.5 * sin(x * incr)

//circle, approximated by 20-gon
static const double SLD_CIRCLE[] = {
    circpt(0),    circpt(1),  circpt(2),    circpt(3),
    circpt(4),    circpt(5),  circpt(6),    circpt(7),
    circpt(8),    circpt(9),  circpt(10),   circpt(11),
    circpt(12),   circpt(13), circpt(14),   circpt(15),
    circpt(16),   circpt(17), circpt(18),   circpt(19),
    circpt(20)
};


//////////////////////////////////////////////

//simle x symbol for use in error conditions

static const double SLD_ERROR[] = {
    0., 0.,
    1., 0.,
    1., 1.,
    0., 0.,
    0., 1.,
    1., 1.,
    1., 0.,
    0., 1.
};

#endif
