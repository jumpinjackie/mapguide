//
//  Copyright (C) 2005-2007 by Autodesk, Inc.
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

#ifndef POINT3D_H
#define POINT3D_H

#include "Vector3D.h"

/// <summary>
/// A class that represents a three-dimensional point.
/// </summary>
class Point3D
{
public:
    //-------------------------------------------------------
    // Variables
    //-------------------------------------------------------

    // the coordinates
    double x;
    double y;
    double z;

    //-------------------------------------------------------
    // Constructors
    //-------------------------------------------------------

    /// <summary>
    /// Initializes a new instance of the Point3D class with
    /// the specified coordinates.
    /// </summary>
    /// <param name="X">X coordinate of the point.</param>
    /// <param name="Y">Y coordinate of the point.</param>
    /// <param name="Z">Z coordinate of the point.</param>
    Point3D(const double X, const double Y, const double Z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    /// <summary>
    /// Copy constructor
    /// </summary>
    /// <param name="pt">The point to be copied.</param>
    Point3D(const Point3D& pt)
    {
        x = pt.x;
        y = pt.y;
        z = pt.z;
    }

    /// <summary>
    /// Default constructor
    /// </summary>
    Point3D()
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }

    //-------------------------------------------------------
    // Point3D implementation
    //-------------------------------------------------------

    /// <summary>
    /// Returns whether this point is equal to the supplied point.
    /// </summary>
    /// <param name="pt">The point to compare to.</param>
    /// <param name="tolerance">The tolerance to use when comparing.</param>
    /// <returns>True if the points are equal; otherwise false.</returns>
    bool isEqualTo(const Point3D& pt, double tolerance = 1.0e-12) const
    {
        double xDiff = x - pt.x;
        double yDiff = y - pt.y;
        double zDiff = z - pt.z;
        return xDiff*xDiff + yDiff*yDiff + zDiff*zDiff <= tolerance*tolerance;
    }

    //-------------------------------------------------------
    // Operator overloads
    //-------------------------------------------------------

    /// <summary>
    /// Copy operator
    /// </summary>
    /// <param name="pt">The point to copy.</param>
    /// <returns>A reference to this point.</returns>
    Point3D& operator=(const Point3D& pt)
    {
        x = pt.x;
        y = pt.y;
        z = pt.z;
        return *this;
    }

    /// <summary>
    /// Operator that determines whether two points are equal.  Two points
    /// are equal if they have the same coordinates.
    /// </summary>
    /// <param name="pt">The point to compare to.</param>
    /// <returns>True if the points are equal; otherwise false.</returns>
    bool operator==(const Point3D& pt) const
    {
        return isEqualTo(pt);
    }

    /// <summary>
    /// Operator that determines whether two points are unequal.  Two points
    /// are unequal if they have one or more different coordinates.
    /// </summary>
    /// <param name="pt">The point to compare to.</param>
    /// <returns>True if the points are unequal; otherwise false.</returns>
    bool operator!=(const Point3D& pt) const
    {
        return !(operator==(pt));
    }

    /// <summary>
    /// Operator that adds a vector to a point.  The result is a point
    /// that is offset from the original point by the vector.
    /// </summary>
    /// <param name="vec">The vector to add.</param>
    /// <returns>The result of adding the specified vector to this point.</returns>
    Point3D operator+(const Vector3D& vec) const
    {
        return Point3D(x + vec.x, y + vec.y, z + vec.z);
    }

    /// <summary>
    /// Operator that subtracts a vector from a point.  The result is a
    /// point that is offset from the original point by the vector.
    /// </summary>
    /// <param name="vec">The vector to subtract.</param>
    /// <returns>The result of subtracting the specified vector from this point.</returns>
    Point3D operator-(const Vector3D& vec) const
    {
        return Point3D(x - vec.x, y - vec.y, z - vec.z);
    }

    /// <summary>
    /// Operator that subtracts two points.  The result is a vector going
    /// from the second point to the first point.
    /// </summary>
    /// <param name="pt">The point to subtract.</param>
    /// <returns>A vector going from the second point to this point.</returns>
    Vector3D operator-(const Point3D& pt) const
    {
        return Vector3D(x - pt.x, y - pt.y, z - pt.z);
    }

    /// <summary>
    /// Operator to access x, y, z via indexer.  0 represents the X coordinate of
    /// the point, 1 represents the Y coordinate of the point, and 2 represents
    /// the Z coordinate of the point.
    /// </summary>
    /// <returns>The indexed x, y, or z value.</returns>
    double operator[](const unsigned int i) const
    {
        _ASSERT(i >= 0 && i <= 2);
        return *(&x+i);
    }

    /// <summary>
    /// Operator to access x, y, z via indexer.  0 represents the X coordinate of
    /// the point, 1 represents the Y coordinate of the point, and 2 represents
    /// the Z coordinate of the point.
    /// </summary>
    /// <returns>The indexed x, y, or z value.</returns>
    double& operator[](const unsigned int i)
    {
        _ASSERT(i >= 0 && i <= 2);
        return *(&x+i);
    }
};

#endif
