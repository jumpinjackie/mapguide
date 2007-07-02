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
    Point3D(double X, double Y, double Z)
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

    bool isEqualTo(const Point3D& v, double tolerance = 1.0e-12) const
    {
        double xDiff = x - v.x;
        double yDiff = y - v.y;
        double zDiff = z - v.z;
        return xDiff*xDiff + yDiff*yDiff + zDiff*zDiff <= tolerance*tolerance;
    }

    /// <summary>
    /// Gets or sets the point coordinates by using index values..  0 represents
    /// the X coordinate of the point, 1 represents the Y coordinate of
    /// the point, and 2 represents the Z coordinate of the point.
    /// </summary>
    double operator[] (int index) const
    {
        switch (index)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                return 0.0;         // Error
        }
    }

    /// <summary>
    /// Gets or sets the point coordinates by using index values..  0 represents
    /// the X coordinate of the point, 1 represents the Y coordinate of
    /// the point, and 2 represents the Z coordinate of the point.
    /// </summary>
    double& operator[] (int index)
    {
        switch (index)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                return x;         // Error
        }
    }

    //-------------------------------------------------------
    // Operator overloads
    //-------------------------------------------------------

    /// <summary>
    /// Copy operator
    /// </summary>
    /// <param name="pt2">The second point.</param>
    /// <returns>Reference to this point.</returns>
    Point3D& operator=(const Point3D& pt2)
    {
        x = pt2.x;
        y = pt2.y;
        z = pt2.z;
        return *this;
    }

    /// <summary>
    /// Operator that determines whether two points are equal.  Two points
    /// are equal if they have the same coordinates.
    /// </summary>
    /// <param name="pt2">The second point.</param>
    /// <returns>true if the points are equal; otherwise false.</returns>
    bool operator==(const Point3D pt2) const
    {
        return isEqualTo(pt2);
    }

    /// <summary>
    /// Operator that determines whether two points are unequal.  Two points
    /// are unequal if they have one or more different coordinates.
    /// </summary>
    /// <param name="pt2">The second point.</param>
    /// <returns>true if the points are unequal; otherwise false.</returns>
    bool operator!=(const Point3D pt2) const
    {
        return !(operator==(pt2));
    }

    /// <summary>
    /// Operator that adds a vector to a point.  The result is a
    /// point that is offset from the original point by the
    /// vector.
    /// </summary>
    /// <param name="pt">The point to add the vector to.</param>
    /// <param name="vec">The vector to add to the point.</param>
    /// <returns>The result of adding vector to the point.</returns>
    Point3D operator+(Vector3D& vec) const
    {
        return Point3D(x + vec.x, y + vec.y, z + vec.z);
    }

    /// <summary>
    /// Operator that subtracts a vector from a point.  The result
    /// is a point that is offset from the original point by the
    /// vector.
    /// </summary>
    /// <param name="vec">The vector to subtract from the point.</param>
    /// <returns>The result of subtracting vector from the point.</returns>
    Point3D operator-(Vector3D& vec) const
    {
        return Point3D(x - vec.x, y - vec.y, z - vec.z);
    }

    /// <summary>
    /// Operator that subtracts two points.  The
    /// result is a vector going from the second point to the
    /// first point.
    /// </summary>
    /// <param name="pt2">The second point.</param>
    /// <returns>A vector going from the second point to the
    /// first point.</returns>
    Vector3D operator-(Point3D& pt2) const
    {
        return Vector3D(x - pt2.x, y - pt2.y, z - pt2.z);
    }
};

#endif
