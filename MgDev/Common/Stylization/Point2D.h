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

#ifndef POINT2D_H_
#define POINT2D_H_

#include "StylizationAPI.h"
#include "StylizationDefs.h"
#include "Vector2D.h"

/// <summary>
/// A class that represents a two-dimensional point.
/// </summary>
class Point2D
{
public:
    //-------------------------------------------------------
    // Constructors
    //-------------------------------------------------------

    /// <summary>
    /// Initializes a new instance of the Point2D class with
    /// the specified coordinates.
    /// </summary>
    /// <param name="X">X coordinate of the point.</param>
    /// <param name="Y">Y coordinate of the point.</param>
    STYLIZATION_API Point2D(const double X, const double Y);

    /// <summary>
    /// Copy constructor
    /// </summary>
    /// <param name="pt">The point to be copied.</param>
    STYLIZATION_API Point2D(const Point2D& pt);

    /// <summary>
    /// Default constructor
    /// </summary>
    STYLIZATION_API Point2D();

    //-------------------------------------------------------
    // Point2D implementation
    //-------------------------------------------------------

    /// <summary>
    /// Returns whether this point is equal to the supplied point.
    /// </summary>
    /// <param name="pt">The point to compare to.</param>
    /// <param name="tolerance">The tolerance to use when comparing.</param>
    /// <returns>True if the points are equal; otherwise false.</returns>
    STYLIZATION_API bool isEqualTo(const Point2D& pt, double tolerance = 1.0e-8) const;

    //-------------------------------------------------------
    // Operator overloads
    //-------------------------------------------------------

    /// <summary>
    /// Copy operator
    /// </summary>
    /// <param name="pt">The point to copy.</param>
    /// <returns>A reference to this point.</returns>
    STYLIZATION_API Point2D& operator=(const Point2D& pt);

    /// <summary>
    /// Operator that determines whether two points are equal.  Two points
    /// are equal if they have the same coordinates.
    /// </summary>
    /// <param name="pt">The point to compare to.</param>
    /// <returns>True if the points are equal; otherwise false.</returns>
    STYLIZATION_API bool operator==(const Point2D& pt) const;

    /// <summary>
    /// Operator that determines whether two points are unequal.  Two points
    /// are unequal if they have one or more different coordinates.
    /// </summary>
    /// <param name="pt">The point to compare to.</param>
    /// <returns>True if the points are unequal; otherwise false.</returns>
    STYLIZATION_API bool operator!=(const Point2D& pt) const;

    /// <summary>
    /// Operator that adds a vector to a point.  The result is a point
    /// that is offset from the original point by the vector.
    /// </summary>
    /// <param name="vec">The vector to add.</param>
    /// <returns>The result of adding the specified vector to this point.</returns>
    STYLIZATION_API Point2D operator+(const Vector2D& vec) const;

    /// <summary>
    /// Operator that subtracts a vector from a point.  The result is a
    /// point that is offset from the original point by the vector.
    /// </summary>
    /// <param name="vec">The vector to subtract.</param>
    /// <returns>The result of subtracting the specified vector from this point.</returns>
    STYLIZATION_API Point2D operator-(const Vector2D& vec) const;

    /// <summary>
    /// Operator that subtracts two points.  The result is a vector going
    /// from the second point to the first point.
    /// </summary>
    /// <param name="pt">The point to subtract.</param>
    /// <returns>A vector going from the second point to this point.</returns>
    STYLIZATION_API Vector2D operator-(const Point2D& pt) const;

    /// <summary>
    /// Operator to access x, y via indexer.  0 represents the X coordinate of
    /// the point, and 1 represents the Y coordinate of the point.
    /// </summary>
    /// <returns>The indexed x or y value.</returns>
    STYLIZATION_API double operator[](const unsigned int i) const;

    /// <summary>
    /// Operator to access x, y via indexer.  0 represents the X coordinate of
    /// the point, and 1 represents the Y coordinate of the point.
    /// </summary>
    /// <returns>The indexed x or y value.</returns>
    STYLIZATION_API double& operator[](const unsigned int i);

public:
    //-------------------------------------------------------
    // Variables
    //-------------------------------------------------------

    // the coordinates
    double x;
    double y;
};

#endif
