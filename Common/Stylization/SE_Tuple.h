//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
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

#ifndef SE_TUPLE_H
#define SE_TUPLE_H

#include "StylizationDefs.h"
#include <math.h>

struct SE_Tuple
{
    double x, y;

    SE_INLINE SE_Tuple();
    SE_INLINE SE_Tuple(double x, double y);

    SE_INLINE double length() const;
    SE_INLINE double lengthSquared() const;

    SE_INLINE SE_Tuple normalize() const;
    SE_INLINE double dot(const SE_Tuple& tup) const;
    SE_INLINE double cross(const SE_Tuple& tup) const;

    SE_INLINE bool operator==(const SE_Tuple& tup) const;
    SE_INLINE SE_Tuple& operator=(const SE_Tuple& tup);
    SE_INLINE SE_Tuple operator+(const SE_Tuple& tup) const;
    SE_INLINE SE_Tuple& operator+=(const SE_Tuple& tup);
    SE_INLINE SE_Tuple operator-(const SE_Tuple& tup) const;
    SE_INLINE SE_Tuple& operator-=(const SE_Tuple& tup);
    SE_INLINE SE_Tuple operator*(double scale) const;
    SE_INLINE SE_Tuple operator*(const SE_Tuple& tup) const;
    SE_INLINE SE_Tuple& operator*=(double scale);
    SE_INLINE SE_Tuple& operator*=(const SE_Tuple& tup);
    SE_INLINE SE_Tuple operator-() const;
};


/* Function Definitions */

SE_Tuple::SE_Tuple()
{
}

SE_Tuple::SE_Tuple(double vx, double vy) :
    x(vx),
    y(vy)
{
}


double SE_Tuple::length() const
{
    return sqrt(x*x + y*y);
}


double SE_Tuple::lengthSquared() const
{
    return x*x + y*y;
}


SE_Tuple SE_Tuple::normalize() const
{
    double invlen = 1.0/sqrt(x*x + y*y);
    return SE_Tuple(x*invlen, y*invlen);
}


double SE_Tuple::dot(const SE_Tuple& tup) const
{
    return x*tup.x + y*tup.y;
}


double SE_Tuple::cross(const SE_Tuple& tup) const
{
    return x*tup.y - y*tup.x;
}

bool SE_Tuple::operator==(const SE_Tuple& tup) const
{
    return (x == tup.x) && (y == tup.y);
}

SE_Tuple& SE_Tuple::operator=(const SE_Tuple& tup)
{
    x = tup.x;
    y = tup.y;
    return *this;
}


SE_Tuple SE_Tuple::operator+(const SE_Tuple& tup) const
{
    return SE_Tuple(x + tup.x, y + tup.y);
}


SE_Tuple& SE_Tuple::operator+=(const SE_Tuple& tup)
{
    x += tup.x;
    y += tup.y;
    return *this;
}


SE_Tuple SE_Tuple::operator-(const SE_Tuple& tup) const
{
    return SE_Tuple(x - tup.x, y - tup.y);
}


SE_Tuple& SE_Tuple::operator-=(const SE_Tuple& tup)
{
    x -= tup.x;
    y -= tup.y;
    return *this;
}


SE_Tuple SE_Tuple::operator*(double scale) const
{
    return SE_Tuple(x*scale, y*scale);
}


SE_Tuple SE_Tuple::operator*(const SE_Tuple& tup) const
{
    return SE_Tuple(x*tup.x, y*tup.y);
}


SE_Tuple& SE_Tuple::operator*=(double scale)
{
    x *= scale;
    y *= scale;
    return *this;
}


SE_Tuple& SE_Tuple::operator*=(const SE_Tuple& tup)
{
    x *= tup.x;
    y *= tup.y;
    return *this;
}


SE_Tuple SE_Tuple::operator-() const
{
    return SE_Tuple(-x, -y);
}

#endif // SE_TUPLE_H
