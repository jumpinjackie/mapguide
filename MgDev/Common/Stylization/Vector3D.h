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

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <math.h>

/// <summary>
/// A class that represents a three-dimensional vector.
/// </summary>
class Vector3D
{
public:
    //-------------------------------------------------------
    // Variables
    //-------------------------------------------------------

    // the components
    double x;
    double y;
    double z;

    //-------------------------------------------------------
    // Constructors
    //-------------------------------------------------------

    /// <summary>
    /// Initializes a new instance of the Vector3D class to
    /// the specified components.
    /// </summary>
    /// <param name="X">X component of the vector.</param>
    /// <param name="Y">Y component of the vector.</param>
    /// <param name="Z">Z component of the vector.</param>
    Vector3D(double X, double Y, double Z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    /// <summary>
    /// Initializes a new instance of the Vector3D class to
    /// the specified vector.
    /// </summary>
    /// <param name="vec">The vector to be copied.</param>
    Vector3D(const Vector3D& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
    }

    Vector3D()
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }


    bool isEqualTo(const Vector3D& v, double tolerance = 1.0e-12) const
    {
        double xDiff = x - v.x;
        double yDiff = y - v.y;
        double zDiff = z - v.z;
        return xDiff*xDiff + yDiff*yDiff + zDiff*zDiff <= tolerance*tolerance;
    }

    //-------------------------------------------------------
    // Vector3D implementation
    //-------------------------------------------------------

    /// <summary>
    /// Returns the normal of the vector.  This ensures the vector has a unit length.
    /// </summary>
    /// <returns>A reference to this vector after normalization.</returns>
    Vector3D normal(const double& tolerance = 1.0e-12) const
    {
        double nx = 0.0, ny = 0.0, nz = 0.0;
        double magSq = x*x + y*y + z*z;
        if (magSq > tolerance && magSq != 1.0)
        {
            double lengthInv = 1.0 / sqrt(magSq);
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
        }

        return Vector3D(nx, ny, nz);
    }

    /// <summary>
    /// Normalize the vector.  This ensures the vector has a unit length.
    /// </summary>
    /// <returns>A reference to this vector after normalization.</returns>
    Vector3D&  normalize(const double& tolerance = 1.0e-12)
    {
        double magSq = x*x + y*y + z*z;
        if (magSq > tolerance && magSq != 1.0)
        {
            double lengthInv = 1.0 / sqrt(magSq);
            x *= lengthInv;
            y *= lengthInv;
            z *= lengthInv;
        }

        return *this;
    }

    double length() const
    {
        return sqrt(x*x + y*y + z*z);
    }

    double lengthSqrd() const
    {
        return (x*x + y*y + z*z);
    }

    double dotProduct(const Vector3D& v) const
    {
        return x*v.x + y*v.y + z*v.z;
    }

    Vector3D crossProduct(const Vector3D& v) const
    {
        return Vector3D(y*v.z - z*v.y,
                        z*v.x - x*v.z,
                        x*v.y - y*v.x);
    }

    //-------------------------------------------------------
    // Operator overloads
    //-------------------------------------------------------

    /// <summary>
    /// Copy constructor
    /// </summary>
    /// <param name="vec">The vectpr to copy.</param>
    /// <returns>A reference to this vector.</returns>
    Vector3D& operator = (const Vector3D& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }

    /// <summary>
    /// Determines whether this vector has the same components
    /// as the specified object.
    /// </summary>
    /// <param name="obj">The object to compare to this vector.</param>
    /// <returns>True if the specified object is a Vector3D and has the same
    /// components as this vector; otherwise returns false.</returns>
    bool operator == (const Vector3D& vec) const
    {
        return isEqualTo(vec);
    }

    /// <summary>
    /// Operator that determines whether two vectors are unequal.  Two vectors
    /// are unequal if they have one or more different components.
    /// </summary>
    /// <param name="vec2">The second vector.</param>
    /// <returns>true if the vectors are unequal; otherwise false.</returns>
    bool operator!=(const Vector3D& vec2) const
    {
        return !(operator ==(vec2));
    }

    /// <summary>
    /// Operator that scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <returns>The scaled vector.</returns>
    Vector3D operator*(double scale) const
    {
        return Vector3D(x * scale, y * scale, z * scale);
    }

    /// <summary>
    /// Operator that scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <param name="vec">The vector to scale.</param>
    /// <returns>The scaled vector.</returns>
    friend Vector3D operator*(double scale, const Vector3D& vec)
    {
        return Vector3D(vec.x * scale, vec.y * scale, vec.z * scale);
    }

    /// <summary>
    /// Operator that inverse-scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <returns>The scaled vector.</returns>
    Vector3D operator/(double scale) const
    {
        return Vector3D(x / scale, y / scale, z / scale);
    }

    /// <summary>
    /// Operator that adds two vectors.  The addition is
    /// achieved by adding the components of each vector.
    /// </summary>
    /// <param name="vec2">The second vector.</param>
    /// <returns>The result of adding the first vector to the second vector.</returns>
    Vector3D operator+(const Vector3D& vec2) const
    {
        return Vector3D(x + vec2.x, y + vec2.y, z + vec2.z);
    }

    /// <summary>
    /// Operator that subtracts two vectors.  The
    /// subtraction is achieved by subtracting the components of
    /// second vector from the components of the first vector.
    /// </summary>
    /// <param name="vec2">The second vector.</param>
    /// <returns>The result of subtracting the second vector from the first vector.</returns>
    Vector3D operator-(const Vector3D& vec2) const
    {
        return Vector3D(x - vec2.x, y - vec2.y, z - vec2.z);
    }

    /// <summary>
    /// Operator that negates a vector.
    /// </summary>
    /// <returns>The negated vector.</returns>
    Vector3D operator-() const
    {
        return Vector3D(-x, -y, -z);
    }

    /// <summary>
    /// Operator to access x, y, z via indexer.
    /// </summary>
    /// <returns>The indexed x, y, or z value.</returns>
    double  operator[] (unsigned int i) const
    {
        return *(&x+i);
    }

    /// <summary>
    /// Operator to access x, y, z via indexer.
    /// </summary>
    /// <returns>The indexed x, y, or z value.</returns>
    double& operator[] (unsigned int i)
    {
        return *(&x+i);
    }
};

#endif
