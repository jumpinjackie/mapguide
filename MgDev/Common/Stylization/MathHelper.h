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

#ifndef MATHHELPER_H_
#define MATHHELPER_H_

#include <math.h>
#include "Foundation.h"
#include <assert.h>

const double EPSILON = 1e-10;

//this is the definition of an empty value for double precision fields
#ifdef _WIN32
const INT64 dblNaN = 0xFFFFFFFFFFFFFFFF; //quiet NaN
#else
const INT64 dblNaN = 0xFFFFFFFFFFFFFFFFLL; //quiet NaN
#endif
const double DBL_NAN = *(double*)&dblNaN;

inline bool ISNAN(double& d)
{
    return *(INT64*)&d == dblNaN;
}

const INT32 fltNaN= 0xFFFFFFFF; //quiet NaN
const float FLT_NAN = *(float*)&fltNaN;

inline bool ISNAN(float& d)
{
    return *(int*)&d == fltNaN;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///<summary>
/// Helper function for converting double to integer.
/// If the double value's decimal part is greater than or equal to 0.5,
/// then increase the integer part by 1 and returns it.
//  Otherwise, returns the integer part.
///</summary>
inline int Double2Int(double flt)
{
#if defined(_WIN32) && !defined(_WIN64)
    //the default state of the FPU is to use rounding when truncating from floating point
    //to integer, so we can use that here
    int val;
    __asm
    {
        fld flt
        fistp val
    }
    return val;
#else
    return static_cast<int>(flt > 0 ? flt += 0.5 : flt -= 0.5);
#endif
}

inline INT64 Double2Int64(double flt)
{
    return static_cast<INT64>(flt > 0 ? flt += 0.5 : flt -= 0.5);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///<summary>
/// Helper function for comparing doubles.
/// If two doubles' interval is less than or equal to delta, we do think they are equal.
/// It is in an anonymous namesapce, so it is unvisible to outer users.
///</summary>
///<returns>
/// -1 : left < right
///  0 : left == right
///  1 : left > right
///</returns>
inline int CompareDoubles(double left, double right, double delta = EPSILON)
{
    // Don't do any operations on DBL_NAN. All those operations cost huge time.
    // Here just checks whether users passed DBL_NAN.
    assert(!ISNAN(left));
    assert(!ISNAN(right));

    double minusResult = left - right;

    return (fabs(minusResult) <= delta ? 0 : (minusResult < 0 ? -1 : 1));
}

#endif
