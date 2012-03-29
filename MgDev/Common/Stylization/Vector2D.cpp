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
#include "Vector2D.h"

//-------------------------------------------------------
// Constructors
//-------------------------------------------------------

// Initializes a new instance of the Vector2D class with
// the specified components.
Vector2D::Vector2D(const double X, const double Y)
{
    x = X;
    y = Y;
}

// Copy constructor
Vector2D::Vector2D(const Vector2D& vec)
{
    x = vec.x;
    y = vec.y;
}

// Default constructor
Vector2D::Vector2D()
{
    x = 0.0;
    y = 0.0;
}

//-------------------------------------------------------
// Vector2D implementation
//-------------------------------------------------------

// Returns whether this vector is equal to the supplied vector.
bool Vector2D::isEqualTo(const Vector2D& vec, double tolerance) const
{
    double xDiff = x - vec.x;
    double yDiff = y - vec.y;
    return xDiff*xDiff + yDiff*yDiff <= tolerance*tolerance;
}

// Returns the normal of the vector.
Vector2D Vector2D::normal(const double& tolerance) const
{
    double nx = x;
    double ny = y;
    double magSq = x*x + y*y;
    if (magSq > tolerance && magSq != 1.0)
    {
        double lengthInv = 1.0 / sqrt(magSq);
        nx = x * lengthInv;
        ny = y * lengthInv;
    }

    return Vector2D(nx, ny);
}

// Normalize the vector.  This ensures the vector has unit length.
Vector2D& Vector2D::normalize(const double& tolerance)
{
    double magSq = x*x + y*y;
    if (magSq > tolerance && magSq != 1.0)
    {
        double lengthInv = 1.0 / sqrt(magSq);
        x *= lengthInv;
        y *= lengthInv;
    }

    return *this;
}

// Returns the length of this vector.
double Vector2D::length() const
{
    return sqrt(x*x + y*y);
}

// Returns the length squared of this vector.
double Vector2D::lengthSqrd() const
{
    return x*x + y*y;
}

// Returns the dot product of this vector with the supplied vector.
double Vector2D::dotProduct(const Vector2D& vec) const
{
    return x*vec.x + y*vec.y;
}

//-------------------------------------------------------
// Operator overloads
//-------------------------------------------------------

// Copy operator
Vector2D& Vector2D::operator=(const Vector2D& vec)
{
    x = vec.x;
    y = vec.y;
    return *this;
}

// Operator that determines whether two vectors are equal.  Two vectors
// are equal if they have the same components.
bool Vector2D::operator==(const Vector2D& vec) const
{
    return isEqualTo(vec);
}

// Operator that determines whether two vectors are unequal.  Two vectors
// are unequal if they have one or more different components.
bool Vector2D::operator!=(const Vector2D& vec) const
{
    return !(operator==(vec));
}

// Operator that scales a vector.
Vector2D Vector2D::operator*(const double scale) const
{
    return Vector2D(x * scale, y * scale);
}

// Operator that inverse-scales a vector.
Vector2D Vector2D::operator/(const double scale) const
{
    return Vector2D(x / scale, y / scale);
}

// Operator that adds two vectors.
Vector2D Vector2D::operator+(const Vector2D& vec) const
{
    return Vector2D(x + vec.x, y + vec.y);
}

// Operator that subtracts two vectors.
Vector2D Vector2D::operator-(const Vector2D& vec) const
{
    return Vector2D(x - vec.x, y - vec.y);
}

// Operator that negates a vector.
Vector2D Vector2D::operator-() const
{
    return Vector2D(-x, -y);
}

// Operator to access x, y via indexer.  0 represents the X component of
// the vector, and 1 represents the Y component of the vector.
double Vector2D::operator[](const unsigned int i) const
{
    _ASSERT(i >= 0 && i <= 1);
    return *(&x+i);
}

// Operator to access x, y via indexer.  0 represents the X component of
// the vector, and 1 represents the Y component of the vector.
double& Vector2D::operator[](const unsigned int i)
{
    _ASSERT(i >= 0 && i <= 1);
    return *(&x+i);
}
