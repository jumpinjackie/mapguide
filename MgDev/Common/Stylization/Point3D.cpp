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
#include "Point3D.h"

//-------------------------------------------------------
// Constructors
//-------------------------------------------------------

// Initializes a new instance of the Point3D class with
// the specified coordinates.
Point3D::Point3D(const double X, const double Y, const double Z)
{
    x = X;
    y = Y;
    z = Z;
}

// Copy constructor
Point3D::Point3D(const Point3D& pt)
{
    x = pt.x;
    y = pt.y;
    z = pt.z;
}

// Default constructor
Point3D::Point3D()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

//-------------------------------------------------------
// Point3D implementation
//-------------------------------------------------------

// Returns whether this point is equal to the supplied point.
bool Point3D::isEqualTo(const Point3D& pt, double tolerance) const
{
    double xDiff = x - pt.x;
    double yDiff = y - pt.y;
    double zDiff = z - pt.z;
    return xDiff*xDiff + yDiff*yDiff + zDiff*zDiff <= tolerance*tolerance;
}

//-------------------------------------------------------
// Operator overloads
//-------------------------------------------------------

// Copy operator
Point3D& Point3D::operator=(const Point3D& pt)
{
    x = pt.x;
    y = pt.y;
    z = pt.z;
    return *this;
}

// Operator that determines whether two points are equal.  Two points
// are equal if they have the same coordinates.
bool Point3D::operator==(const Point3D& pt) const
{
    return isEqualTo(pt);
}

// Operator that determines whether two points are unequal.  Two points
// are unequal if they have one or more different coordinates.
bool Point3D::operator!=(const Point3D& pt) const
{
    return !(operator==(pt));
}

// Operator that adds a vector to a point.  The result is a point
// that is offset from the original point by the vector.
Point3D Point3D::operator+(const Vector3D& vec) const
{
    return Point3D(x + vec.x, y + vec.y, z + vec.z);
}

// Operator that subtracts a vector from a point.  The result is a
// point that is offset from the original point by the vector.
Point3D Point3D::operator-(const Vector3D& vec) const
{
    return Point3D(x - vec.x, y - vec.y, z - vec.z);
}

// Operator that subtracts two points.  The result is a vector going
// from the second point to the first point.
Vector3D Point3D::operator-(const Point3D& pt) const
{
    return Vector3D(x - pt.x, y - pt.y, z - pt.z);
}

// Operator to access x, y, z via indexer.  0 represents the X coordinate of
// the point, 1 represents the Y coordinate of the point, and 2 represents
// the Z coordinate of the point.
double Point3D::operator[](const unsigned int i) const
{
    _ASSERT(i >= 0 && i <= 2);
    return *(&x+i);
}

// Operator to access x, y, z via indexer.  0 represents the X coordinate of
// the point, 1 represents the Y coordinate of the point, and 2 represents
// the Z coordinate of the point.
double& Point3D::operator[](const unsigned int i)
{
    _ASSERT(i >= 0 && i <= 2);
    return *(&x+i);
}
