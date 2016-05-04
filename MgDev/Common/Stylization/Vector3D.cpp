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
#include "Vector3D.h"

//-------------------------------------------------------
// Constructors
//-------------------------------------------------------

// Initializes a new instance of the Vector3D class with
// the specified components.
Vector3D::Vector3D(const double X, const double Y, const double Z)
{
    x = X;
    y = Y;
    z = Z;
}

// Copy constructor
Vector3D::Vector3D(const Vector3D& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
}

// Default constructor
Vector3D::Vector3D()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

//-------------------------------------------------------
// Vector3D implementation
//-------------------------------------------------------

// Returns whether this vector is equal to the supplied vector.
bool Vector3D::isEqualTo(const Vector3D& vec, double tolerance) const
{
    double xDiff = x - vec.x;
    double yDiff = y - vec.y;
    double zDiff = z - vec.z;
    return xDiff*xDiff + yDiff*yDiff + zDiff*zDiff <= tolerance*tolerance;
}

// Returns the normal of the vector.
Vector3D Vector3D::normal(const double& tolerance) const
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

// Normalize the vector.  This ensures the vector has unit length.
Vector3D& Vector3D::normalize(const double& tolerance)
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

// Returns the length of this vector.
double Vector3D::length() const
{
    return sqrt(x*x + y*y + z*z);
}

// Returns the length squared of this vector.
double Vector3D::lengthSqrd() const
{
    return x*x + y*y + z*z;
}

// Returns the dot product of this vector with the supplied vector.
double Vector3D::dotProduct(const Vector3D& vec) const
{
    return x*vec.x + y*vec.y + z*vec.z;
}

// Returns the cross product of this vector with the supplied vector.
Vector3D Vector3D::crossProduct(const Vector3D& vec) const
{
    return Vector3D(y*vec.z - z*vec.y,
                    z*vec.x - x*vec.z,
                    x*vec.y - y*vec.x);
}

//-------------------------------------------------------
// Operator overloads
//-------------------------------------------------------

// Copy operator
Vector3D& Vector3D::operator=(const Vector3D& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
    return *this;
}

// Operator that determines whether two vectors are equal.  Two vectors
// are equal if they have the same components.
bool Vector3D::operator==(const Vector3D& vec) const
{
    return isEqualTo(vec);
}

// Operator that determines whether two vectors are unequal.  Two vectors
// are unequal if they have one or more different components.
bool Vector3D::operator!=(const Vector3D& vec) const
{
    return !(operator==(vec));
}

// Operator that scales a vector.
Vector3D Vector3D::operator*(const double scale) const
{
    return Vector3D(x * scale, y * scale, z * scale);
}

// Operator that inverse-scales a vector.
Vector3D Vector3D::operator/(const double scale) const
{
    return Vector3D(x / scale, y / scale, z / scale);
}

// Operator that adds two vectors.
Vector3D Vector3D::operator+(const Vector3D& vec) const
{
    return Vector3D(x + vec.x, y + vec.y, z + vec.z);
}

// Operator that subtracts two vectors.
Vector3D Vector3D::operator-(const Vector3D& vec) const
{
    return Vector3D(x - vec.x, y - vec.y, z - vec.z);
}

// Operator that negates a vector.
Vector3D Vector3D::operator-() const
{
    return Vector3D(-x, -y, -z);
}

// Operator to access x, y, z via indexer.  0 represents the X component of
// the vector, 1 represents the Y component of the vector, and 2 represents
// the Z component of the vector.
double Vector3D::operator[](const unsigned int i) const
{
    _ASSERT(i >= 0 && i <= 2);
    return *(&x+i);
}

// Operator to access x, y, z via indexer.  0 represents the X component of
// the vector, 1 represents the Y component of the vector, and 2 represents
// the Z component of the vector.
double& Vector3D::operator[](const unsigned int i)
{
    _ASSERT(i >= 0 && i <= 2);
    return *(&x+i);
}
