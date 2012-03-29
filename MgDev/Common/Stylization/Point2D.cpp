//
//  Copyright (C) 2005-2011 by Autodesk, Inc.
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

#include "stdafx.h"
#include "Point2D.h"

//-------------------------------------------------------
// Constructors
//-------------------------------------------------------

// Initializes a new instance of the Point2D class with
// the specified coordinates.
Point2D::Point2D(const double X, const double Y)
{
    x = X;
    y = Y;
}

// Copy constructor
Point2D::Point2D(const Point2D& pt)
{
    x = pt.x;
    y = pt.y;
}

// Default constructor
Point2D::Point2D()
{
    x = 0.0;
    y = 0.0;
}

//-------------------------------------------------------
// Point2D implementation
//-------------------------------------------------------

// Returns whether this point is equal to the supplied point.
bool Point2D::isEqualTo(const Point2D& pt, double tolerance) const
{
    double xDiff = x - pt.x;
    double yDiff = y - pt.y;
    return xDiff*xDiff + yDiff*yDiff <= tolerance*tolerance;
}

//-------------------------------------------------------
// Operator overloads
//-------------------------------------------------------

// Copy operator
Point2D& Point2D::operator=(const Point2D& pt)
{
    x = pt.x;
    y = pt.y;
    return *this;
}

// Operator that determines whether two points are equal.  Two points
// are equal if they have the same coordinates.
bool Point2D::operator==(const Point2D& pt) const
{
    return isEqualTo(pt);
}

// Operator that determines whether two points are unequal.  Two points
// are unequal if they have one or more different coordinates.
bool Point2D::operator!=(const Point2D& pt) const
{
    return !(operator==(pt));
}

// Operator that adds a vector to a point.  The result is a point
// that is offset from the original point by the vector.
Point2D Point2D::operator+(const Vector2D& vec) const
{
    return Point2D(x + vec.x, y + vec.y);
}

// Operator that subtracts a vector from a point.  The result is a
// point that is offset from the original point by the vector.
Point2D Point2D::operator-(const Vector2D& vec) const
{
    return Point2D(x - vec.x, y - vec.y);
}

// Operator that subtracts two points.  The result is a vector going
// from the second point to the first point.
Vector2D Point2D::operator-(const Point2D& pt) const
{
    return Vector2D(x - pt.x, y - pt.y);
}

// Operator to access x, y via indexer.  0 represents the X coordinate of
// the point, and 1 represents the Y coordinate of the point.
double Point2D::operator[](const unsigned int i) const
{
    _ASSERT(i >= 0 && i <= 1);
    return *(&x+i);
}

// Operator to access x, y via indexer.  0 represents the X coordinate of
// the point, and 1 represents the Y coordinate of the point.
double& Point2D::operator[](const unsigned int i)
{
    _ASSERT(i >= 0 && i <= 1);
    return *(&x+i);
}
