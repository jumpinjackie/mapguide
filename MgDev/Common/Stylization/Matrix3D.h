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

#ifndef MATRIX3D_H_
#define MATRIX3D_H_

#include "StylizationAPI.h"
#include "Point3D.h"
#include "Vector3D.h"

/// <summary>
/// <para>A three-dimensional transformation matrix, with support for
/// translation, scaling, rotation, and projection.</para>
/// <para>This is represented with a 4x4 matrix as follows:</para>
/// <para>[ m00  m01  m02  m03 ]</para>
/// <para>[ m10  m11  m12  m13 ]</para>
/// <para>[ m20  m21  m22  m23 ]</para>
/// <para>[ m30  m31  m32  m33 ]</para>
/// </summary>
class Matrix3D
{
public:
    //-------------------------------------------------------
    // Constructors
    //-------------------------------------------------------

    /// <summary>
    /// Initializes a new instance of the Matrix3D class to
    /// the identity matrix.
    /// </summary>
    STYLIZATION_API Matrix3D();

    /// <summary>
    /// Initializes a new instance of the Matrix3D class to
    /// the specified matrix.
    /// </summary>
    /// <param name="matrix">The matrix to be copied.</param>
    STYLIZATION_API Matrix3D(const Matrix3D& matrix);

    //-------------------------------------------------------
    // Properties
    //-------------------------------------------------------

    /// <summary>
    /// Gets or sets the matrix elements by using index pairs.  The
    /// first index specifies the row and the second index specifies
    /// the column.  Both indexes must be in the range 0 to 3.
    /// </summary>
    STYLIZATION_API double operator() (unsigned int row, unsigned int column) const;
    STYLIZATION_API double& operator() (unsigned int row, unsigned int column);

    /// <summary>
    /// <para>Gets a boolean value that determines whether this matrix is the identity matrix.</para>
    /// <para>A value of true indicates this matrix is the identity matrix.
    /// A value of false indicates that this matrix is not the identity
    /// matrix.</para>
    /// </summary>
    STYLIZATION_API bool IsIdentity();

    //-------------------------------------------------------
    // Operators
    //-------------------------------------------------------

    STYLIZATION_API Matrix3D& operator=(Matrix3D& src);

    //-------------------------------------------------------
    // Array accessors
    //-------------------------------------------------------

    /// <summary>
    /// <para>Returns this matrix as a one-dimensional array.  The array
    /// is grouped by rows (M00...M03, M10...M13, and so on).</para>
    /// <para>The input array is optional.  If you supply it, it is filled
    /// with the matrix elements and returned; otherwise a new array
    /// is returned.  Note that any supplied array must have a length
    /// of at least 16 elements.</para>
    /// </summary>
    /// <param name="elements">Optional array to fill with the matrix elements.</param>
    /// <returns>An array containing the matrix elements.</returns>
    STYLIZATION_API double* GetElements(double elements[16]) const;

    /// <summary>
    /// <para>Enables all elements of this matrix to be set from a one-dimensional
    /// array.</para>
    /// <para>The elements in the supplied array must be grouped by rows
    /// (M00...M03, M10...M13, and so on).  You must specify this array by using
    /// the elements parameter.   A null value throws an ArgumentNullException.
    /// An ArgumentException is also thrown if the array doesn't have
    /// a length of at least 16 elements.</para>
    /// </summary>
    /// <param name="elements">Array containing the elements to be set on this matrix.</param>
    STYLIZATION_API void SetElements(double elements[16]);

    //-------------------------------------------------------
    // Determinant and inverse
    //-------------------------------------------------------

    /// <summary>
    /// Gets the determinant of this matrix.  If this value is non-zero
    /// then this matrix is invertible.
    /// </summary>
    STYLIZATION_API double Determinant();

    /// <summary>
    /// <para>Calculates the inverse of this matrix.</para>
    /// </summary>
    /// <param name="matrix">Optional matrix to set to the inverse.</param>
    /// <returns>TRUE if the operation succeeded, FALSE otherwise</returns>
    STYLIZATION_API bool Invert(double zeroTolerance = 1.0e-12);

    //-------------------------------------------------------
    // Matrix initialization
    //-------------------------------------------------------

    /// <summary>
    /// Resets this matrix to the identity matrix.
    /// </summary>
    STYLIZATION_API void SetToIdentity();

    /// <summary>
    /// Resets this matrix to a translation matrix.
    /// </summary>
    /// <param name="transX">Distance to translate in the X direction.</param>
    /// <param name="transY">Distance to translate in the Y direction.</param>
    /// <param name="transZ">Distance to translate in the Z direction.</param>
    STYLIZATION_API void SetToTranslation(double transX, double transY, double transZ);

    /// <summary>
    /// Resets this matrix to a rotation matrix.  The resulting matrix
    /// will rotate a point counter-clockwise around the specified axis
    /// by the specified angle.
    /// </summary>
    /// <param name="angle">The angle of rotation in radians.</param>
    /// <param name="axis">The orientation of the axis to rotate around.</param>
    /// <param name="point">A point on the axis of rotation.</param>
    STYLIZATION_API void SetToRotation(double angle, Vector3D axis, Point3D& point);

    /// <summary>
    /// Resets this matrix to a scaling matrix.
    /// </summary>
    /// <param name="scaleX">Factor to scale by in the X direction.</param>
    /// <param name="scaleY">Factor to scale by in the Y direction.</param>
    /// <param name="scaleZ">Factor to scale by in the Z direction.</param>
    STYLIZATION_API void SetToScaling(double scaleX, double scaleY, double scaleZ);

    /// <summary>
    /// Resets this matrix to the specified matrix.
    /// </summary>
    /// <param name="matrix">The matrix to copy.</param>
    STYLIZATION_API void SetToMatrix(const Matrix3D& matrix);

    STYLIZATION_API Matrix3D& SetCoordSystem(const Point3D& origin, const Vector3D& e0, const Vector3D& e1, const Vector3D& e2);

    //-------------------------------------------------------
    // Matrix modification
    //-------------------------------------------------------

    /// <summary>
    /// Concatenates a translation matrix to this matrix.
    /// </summary>
    /// <param name="transX">Distance to translate in the X direction.</param>
    /// <param name="transY">Distance to translate in the Y direction.</param>
    /// <param name="transZ">Distance to translate in the Z direction.</param>
    STYLIZATION_API void Translate(double transX, double transY, double transZ);

    /// <summary>
    /// Concatenates a rotation matrix to this matrix.
    /// </summary>
    /// <param name="angle">The angle of rotation in degrees.</param>
    /// <param name="axis">The orientation of the axis to rotate around.</param>
    /// <param name="point">A point on the axis of rotation.</param>
    STYLIZATION_API void Rotate(double angle, Vector3D& axis, Point3D& point);

    /// <summary>
    /// Concatenates a scaling matrix to this matrix.
    /// </summary>
    /// <param name="scaleX">Factor to scale by in the X direction.</param>
    /// <param name="scaleY">Factor to scale by in the Y direction.</param>
    /// <param name="scaleZ">Factor to scale by in the Z direction.</param>
    STYLIZATION_API void Scale(double scaleX, double scaleY, double scaleZ);

    /// <summary>
    /// Adds the specified matrix to the end of this matrix.
    /// </summary>
    /// <param name="matrix">The matrix to add to this matrix.</param>
    STYLIZATION_API void Concatenate(Matrix3D& matrix);

    /// <summary>
    /// Adds the specified matrix to the beginning of this matrix.
    /// </summary>
    /// <param name="matrix">The matrix to add to this matrix.</param>
    STYLIZATION_API void PreConcatenate(Matrix3D& matrix);

    //-------------------------------------------------------
    // Point transformations
    //-------------------------------------------------------

    /// <summary>
    /// Transforms a 3D point by the current matrix.
    /// </summary>
    /// <param name="pt">The 3D point to transform.</param>
    /// <returns>A 3D point with the transformed coordinates.</returns>
    STYLIZATION_API Point3D Transform(Point3D& pt);

    /// <summary>
    /// Transforms a 3D point by the current matrix.
    /// </summary>
    /// <param name="x">The X coordinate of the 3D point.</param>
    /// <param name="y">The Y coordinate of the 3D point.</param>
    /// <param name="z">The Z coordinate of the 3D point.</param>
    /// <returns>A 3D point with the transformed coordinates.</returns>
    STYLIZATION_API Point3D Transform(double x, double y, double z);

private:
    //-------------------------------------------------------
    // Helper methods
    //-------------------------------------------------------

    // Returns the absolute value of the element having
    // the smallest non-zero magnitude.
    // TODO: need a better way to normalize the matrix
    double MinMagnitude();

    // sets this matrix to the product of two matrices
    void SetToProduct(Matrix3D& m1, Matrix3D& m2);

public:
    //-------------------------------------------------------
    // Variables
    //-------------------------------------------------------

    // the matrix elements as m-row-column
    double m00, m01, m02, m03;
    double m10, m11, m12, m13;
    double m20, m21, m22, m23;
    double m30, m31, m32, m33;

    // static objects used internally
    static double matArray[4][4];
    static double invArray[4][4];
    static double wkCoords[32];
    static Matrix3D wkMat3Da;
    static Matrix3D wkMat3Db;
};

#endif
