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
    /// Initializes a new instance of the Vector3D class with
    /// the specified components.
    /// </summary>
    /// <param name="X">X component of the vector.</param>
    /// <param name="Y">Y component of the vector.</param>
    /// <param name="Z">Z component of the vector.</param>
    Vector3D(const double X, const double Y, const double Z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    /// <summary>
    /// Copy constructor
    /// </summary>
    /// <param name="vec">The vector to be copied.</param>
    Vector3D(const Vector3D& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
    }

    /// <summary>
    /// Default constructor
    /// </summary>
    Vector3D()
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }

    //-------------------------------------------------------
    // Vector3D implementation
    //-------------------------------------------------------

    /// <summary>
    /// Returns whether this vector is equal to the supplied vector.
    /// </summary>
    /// <param name="vec">The vector to compare to.</param>
    /// <param name="tolerance">The tolerance to use when comparing.</param>
    /// <returns>True if the vectors are equal; otherwise false.</returns>
    bool isEqualTo(const Vector3D& vec, double tolerance = 1.0e-12) const
    {
        double xDiff = x - vec.x;
        double yDiff = y - vec.y;
        double zDiff = z - vec.z;
        return xDiff*xDiff + yDiff*yDiff + zDiff*zDiff <= tolerance*tolerance;
    }

    /// <summary>
    /// Returns the normal of the vector.
    /// </summary>
    /// <returns>A new normalized vector.</returns>
    Vector3D normal(const double& tolerance = 1.0e-12) const
    {
        double nx = x;
        double ny = y;
        double nz = z;
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
    /// Normalize the vector.  This ensures the vector has unit length.
    /// </summary>
    /// <returns>A reference to this vector after normalization.</returns>
    Vector3D& normalize(const double& tolerance = 1.0e-12)
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

    /// <summary>
    /// Returns the length of this vector.
    /// </summary>
    double length() const
    {
        return sqrt(x*x + y*y + z*z);
    }

    /// <summary>
    /// Returns the length squared of this vector.
    /// </summary>
    double lengthSqrd() const
    {
        return x*x + y*y + z*z;
    }

    /// <summary>
    /// Returns the dot product of this vector with the supplied vector.
    /// </summary>
    /// <param name="vec">The vector to perform the dot product with.</param>
    double dotProduct(const Vector3D& vec) const
    {
        return x*vec.x + y*vec.y + z*vec.z;
    }

    /// <summary>
    /// Returns the cross product of this vector with the supplied vector.
    /// </summary>
    /// <param name="vec">The vector to perform the cross product with.</param>
    Vector3D crossProduct(const Vector3D& vec) const
    {
        return Vector3D(y*vec.z - z*vec.y,
                        z*vec.x - x*vec.z,
                        x*vec.y - y*vec.x);
    }

    //-------------------------------------------------------
    // Operator overloads
    //-------------------------------------------------------

    /// <summary>
    /// Copy operator
    /// </summary>
    /// <param name="vec">The vector to copy.</param>
    /// <returns>A reference to this vector.</returns>
    Vector3D& operator=(const Vector3D& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }

    /// <summary>
    /// Operator that determines whether two vectors are equal.  Two vectors
    /// are equal if they have the same components.
    /// </summary>
    /// <param name="vec">The vector to compare to.</param>
    /// <returns>True if the vectors are equal; otherwise false.</returns>
    bool operator==(const Vector3D& vec) const
    {
        return isEqualTo(vec);
    }

    /// <summary>
    /// Operator that determines whether two vectors are unequal.  Two vectors
    /// are unequal if they have one or more different components.
    /// </summary>
    /// <param name="vec">The vector to compare to.</param>
    /// <returns>True if the vectors are unequal; otherwise false.</returns>
    bool operator!=(const Vector3D& vec) const
    {
        return !(operator==(vec));
    }

    /// <summary>
    /// Operator that scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <returns>The scaled vector.</returns>
    Vector3D operator*(const double scale) const
    {
        return Vector3D(x * scale, y * scale, z * scale);
    }

    /// <summary>
    /// Operator that scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <param name="vec">The vector to scale.</param>
    /// <returns>The scaled vector.</returns>
    friend Vector3D operator*(const double scale, const Vector3D& vec)
    {
        return Vector3D(vec.x * scale, vec.y * scale, vec.z * scale);
    }

    /// <summary>
    /// Operator that inverse-scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <returns>The scaled vector.</returns>
    Vector3D operator/(const double scale) const
    {
        return Vector3D(x / scale, y / scale, z / scale);
    }

    /// <summary>
    /// Operator that adds two vectors.
    /// </summary>
    /// <param name="vec">The vector to add.</param>
    /// <returns>The result of adding the specified vector to this vector.</returns>
    Vector3D operator+(const Vector3D& vec) const
    {
        return Vector3D(x + vec.x, y + vec.y, z + vec.z);
    }

    /// <summary>
    /// Operator that subtracts two vectors.
    /// </summary>
    /// <param name="vec">The vector to subtract.</param>
    /// <returns>The result of subtracting the specified vector from this vector.</returns>
    Vector3D operator-(const Vector3D& vec) const
    {
        return Vector3D(x - vec.x, y - vec.y, z - vec.z);
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
    /// Operator to access x, y, z via indexer.  0 represents the X component of
    /// the vector, 1 represents the Y component of the vector, and 2 represents
    /// the Z component of the vector.
    /// </summary>
    /// <returns>The indexed x, y, or z value.</returns>
    double operator[](const unsigned int i) const
    {
//      _ASSERT(i >= 0 && i <= 2);
        return *(&x+i);
    }

    /// <summary>
    /// Operator to access x, y, z via indexer.  0 represents the X component of
    /// the vector, 1 represents the Y component of the vector, and 2 represents
    /// the Z component of the vector.
    /// </summary>
    /// <returns>The indexed x, y, or z value.</returns>
    double& operator[](const unsigned int i)
    {
//      _ASSERT(i >= 0 && i <= 2);
        return *(&x+i);
    }
};

#endif
