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

#ifndef VECTOR2D_H_
#define VECTOR2D_H_

#include "StylizationAPI.h"
#include "StylizationDefs.h"

/// <summary>
/// A class that represents a two-dimensional vector.
/// </summary>
class Vector2D
{
public:
    //-------------------------------------------------------
    // Constructors
    //-------------------------------------------------------

    /// <summary>
    /// Initializes a new instance of the Vector2D class with
    /// the specified components.
    /// </summary>
    /// <param name="X">X component of the vector.</param>
    /// <param name="Y">Y component of the vector.</param>
    STYLIZATION_API Vector2D(const double X, const double Y);

    /// <summary>
    /// Copy constructor
    /// </summary>
    /// <param name="vec">The vector to be copied.</param>
    STYLIZATION_API Vector2D(const Vector2D& vec);

    /// <summary>
    /// Default constructor
    /// </summary>
    STYLIZATION_API Vector2D();

    //-------------------------------------------------------
    // Vector2D implementation
    //-------------------------------------------------------

    /// <summary>
    /// Returns whether this vector is equal to the supplied vector.
    /// </summary>
    /// <param name="vec">The vector to compare to.</param>
    /// <param name="tolerance">The tolerance to use when comparing.</param>
    /// <returns>True if the vectors are equal; otherwise false.</returns>
    STYLIZATION_API bool isEqualTo(const Vector2D& vec, double tolerance = 1.0e-12) const;

    /// <summary>
    /// Returns the normal of the vector.
    /// </summary>
    /// <returns>A new normalized vector.</returns>
    STYLIZATION_API Vector2D normal(const double& tolerance = 1.0e-12) const;

    /// <summary>
    /// Normalize the vector.  This ensures the vector has unit length.
    /// </summary>
    /// <returns>A reference to this vector after normalization.</returns>
    STYLIZATION_API Vector2D& normalize(const double& tolerance = 1.0e-12);

    /// <summary>
    /// Returns the length of this vector.
    /// </summary>
    STYLIZATION_API double length() const;

    /// <summary>
    /// Returns the length squared of this vector.
    /// </summary>
    STYLIZATION_API double lengthSqrd() const;

    /// <summary>
    /// Returns the dot product of this vector with the supplied vector.
    /// </summary>
    /// <param name="vec">The vector to perform the dot product with.</param>
    STYLIZATION_API double dotProduct(const Vector2D& vec) const;

    //-------------------------------------------------------
    // Operator overloads
    //-------------------------------------------------------

    /// <summary>
    /// Copy operator
    /// </summary>
    /// <param name="vec">The vector to copy.</param>
    /// <returns>A reference to this vector.</returns>
    STYLIZATION_API Vector2D& operator=(const Vector2D& vec);

    /// <summary>
    /// Operator that determines whether two vectors are equal.  Two vectors
    /// are equal if they have the same components.
    /// </summary>
    /// <param name="vec">The vector to compare to.</param>
    /// <returns>True if the vectors are equal; otherwise false.</returns>
    STYLIZATION_API bool operator==(const Vector2D& vec) const;

    /// <summary>
    /// Operator that determines whether two vectors are unequal.  Two vectors
    /// are unequal if they have one or more different components.
    /// </summary>
    /// <param name="vec">The vector to compare to.</param>
    /// <returns>True if the vectors are unequal; otherwise false.</returns>
    STYLIZATION_API bool operator!=(const Vector2D& vec) const;

    /// <summary>
    /// Operator that scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <returns>The scaled vector.</returns>
    STYLIZATION_API Vector2D operator*(const double scale) const;

    /// <summary>
    /// Operator that scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <param name="vec">The vector to scale.</param>
    /// <returns>The scaled vector.</returns>
    STYLIZATION_API friend Vector2D operator*(const double scale, const Vector2D& vec)
    {
        return Vector2D(vec.x * scale, vec.y * scale);
    }

    /// <summary>
    /// Operator that inverse-scales a vector.
    /// </summary>
    /// <param name="scale">The scale factor for the vector.</param>
    /// <returns>The scaled vector.</returns>
    STYLIZATION_API Vector2D operator/(const double scale) const;

    /// <summary>
    /// Operator that adds two vectors.
    /// </summary>
    /// <param name="vec">The vector to add.</param>
    /// <returns>The result of adding the specified vector to this vector.</returns>
    STYLIZATION_API Vector2D operator+(const Vector2D& vec) const;

    /// <summary>
    /// Operator that subtracts two vectors.
    /// </summary>
    /// <param name="vec">The vector to subtract.</param>
    /// <returns>The result of subtracting the specified vector from this vector.</returns>
    STYLIZATION_API Vector2D operator-(const Vector2D& vec) const;

    /// <summary>
    /// Operator that negates a vector.
    /// </summary>
    /// <returns>The negated vector.</returns>
    STYLIZATION_API Vector2D operator-() const;

    /// <summary>
    /// Operator to access x, y via indexer.  0 represents the X component of
    /// the vector, and 1 represents the Y component of the vector.
    /// </summary>
    /// <returns>The indexed x or y value.</returns>
    STYLIZATION_API double operator[](const unsigned int i) const;

    /// <summary>
    /// Operator to access x, y via indexer.  0 represents the X component of
    /// the vector, and 1 represents the Y component of the vector.
    /// </summary>
    /// <returns>The indexed x or y value.</returns>
    STYLIZATION_API double& operator[](const unsigned int i);

public:
    //-------------------------------------------------------
    // Variables
    //-------------------------------------------------------

    // the components
    double x;
    double y;
};

#endif
