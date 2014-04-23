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
#include "Matrix3D.h"
#include <math.h>
#include <string.h>   // for memcpy

// Declare space for static member variables of Matrix3D
double Matrix3D::matArray[4][4];
double Matrix3D::invArray[4][4];
double Matrix3D::wkCoords[32];
Matrix3D Matrix3D::wkMat3Da;
Matrix3D Matrix3D::wkMat3Db;


//-------------------------------------------------------
// Constructors
//-------------------------------------------------------


/// <summary>
/// Initializes a new instance of the Matrix3D class to
/// the identity matrix.
/// </summary>
Matrix3D::Matrix3D()
{
    SetToIdentity();
}


/// <summary>
/// Initializes a new instance of the Matrix3D class to
/// the specified matrix.
/// </summary>
/// <param name="matrix">The matrix to be copied.</param>
Matrix3D::Matrix3D(const Matrix3D& matrix)
{
    SetToMatrix(matrix);
}


//-------------------------------------------------------
// Implementation - properties
//-------------------------------------------------------


/// <summary>
/// Gets or sets the matrix elements by using index pairs.  The
/// first index specifies the row and the second index specifies
/// the column.  Both indexes must be in the range 0 to 3.
/// </summary>
double Matrix3D::operator() (unsigned int row, unsigned int column) const
{
    return *(&m00+((row<<2)+column));
}


double& Matrix3D::operator() (unsigned int row, unsigned int column)
{
    return *(&m00+((row<<2)+column));
}


/// <summary>
/// <para>Gets a boolean value that determines whether this matrix is the identity matrix.</para>
/// <para>A value of true indicates this matrix is the identity matrix.
/// A value of false indicates that this matrix is not the identity
/// matrix.</para>
/// </summary>
bool Matrix3D::IsIdentity()
{
    return m00 == 1.0 && m01 == 0.0 && m02 == 0.0 && m03 == 0.0 &&
           m10 == 0.0 && m11 == 1.0 && m12 == 0.0 && m13 == 0.0 &&
           m20 == 0.0 && m21 == 0.0 && m22 == 1.0 && m23 == 0.0 &&
           m30 == 0.0 && m31 == 0.0 && m32 == 0.0 && m33 == 1.0;
}


//-------------------------------------------------------
// Implementation - operators
//-------------------------------------------------------


Matrix3D& Matrix3D::operator=(Matrix3D& src)
{
    m00 = src. m00;
    m01 = src. m01;
    m02 = src. m02;
    m03 = src. m03;
    m10 = src. m10;
    m11 = src. m11;
    m12 = src. m12;
    m13 = src. m13;
    m20 = src. m20;
    m21 = src. m21;
    m22 = src. m22;
    m23 = src. m23;
    m30 = src. m30;
    m31 = src. m31;
    m32 = src. m32;
    m33 = src. m33;

    return *this;
}


//-------------------------------------------------------
// Implementation - array accessors
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
double* Matrix3D::GetElements(double elements[16]) const
{
    // validate input
    if (elements == NULL)
        elements = new double[16];

    // set the array elements
    elements[ 0] = m00;
    elements[ 1] = m01;
    elements[ 2] = m02;
    elements[ 3] = m03;
    elements[ 4] = m10;
    elements[ 5] = m11;
    elements[ 6] = m12;
    elements[ 7] = m13;
    elements[ 8] = m20;
    elements[ 9] = m21;
    elements[10] = m22;
    elements[11] = m23;
    elements[12] = m30;
    elements[13] = m31;
    elements[14] = m32;
    elements[15] = m33;

    return elements;
}


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
void Matrix3D::SetElements(double elements[16])
{
    // validate input
    if (elements == NULL)
        return;                 // Error

    // set the matrix elements
    m00 = elements[ 0];
    m01 = elements[ 1];
    m02 = elements[ 2];
    m03 = elements[ 3];
    m10 = elements[ 4];
    m11 = elements[ 5];
    m12 = elements[ 6];
    m13 = elements[ 7];
    m20 = elements[ 8];
    m21 = elements[ 9];
    m22 = elements[10];
    m23 = elements[11];
    m30 = elements[12];
    m31 = elements[13];
    m32 = elements[14];
    m33 = elements[15];
}


//-------------------------------------------------------
// Implementation - determinant and inverse
//-------------------------------------------------------


/// <summary>
/// Gets the determinant of this matrix.  If this value is non-zero
/// then this matrix is invertible.
/// </summary>
double Matrix3D::Determinant()
{
    double subDet0 = m11*(m22*m33 - m23*m32) -
                     m12*(m21*m33 - m23*m31) +
                     m13*(m21*m32 - m22*m31);
    double subDet1 = m10*(m22*m33 - m23*m32) -
                     m12*(m20*m33 - m23*m30) +
                     m13*(m20*m32 - m22*m30);
    double subDet2 = m10*(m21*m33 - m23*m31) -
                     m11*(m20*m33 - m23*m30) +
                     m13*(m20*m31 - m21*m30);
    double subDet3 = m10*(m21*m32 - m22*m31) -
                     m11*(m20*m32 - m22*m30) +
                     m12*(m20*m31 - m21*m30);

    return m00*subDet0 - m01*subDet1 + m02*subDet2 - m03*subDet3;
}


/// <summary>
/// <para>Calculates the inverse of this matrix.</para>
/// </summary>
/// <param name="matrix">Optional matrix to set to the inverse.</param>
/// <returns>TRUE if the operation succeeded, FALSE otherwise</returns>
bool Matrix3D::Invert(double zeroTolerance)
{
    // get the minimum non-zero value
    double matScale = MinMagnitude();
    matScale = 1.0/(matScale*matScale*matScale);

    // check if matrix is homogeneous (will be unless perspective present)...
    if (m30 == 0.0 && m31 == 0.0 && m32 == 0.0 && m33 == 1.0)
    {
        // if so we can obtain the inverse fairly easily
        double subDet00 = m11*m22 - m12*m21;
        double subDet10 = m01*m22 - m02*m21;
        double subDet20 = m01*m12 - m02*m11;
        double subDet01 = m10*m22 - m12*m20;
        double subDet11 = m00*m22 - m02*m20;
        double subDet21 = m00*m12 - m02*m10;
        double subDet02 = m10*m21 - m11*m20;
        double subDet12 = m00*m21 - m01*m20;
        double subDet22 = m00*m11 - m01*m10;

        // Calculate determinant by Laplace expansion.  Arbitrarily chose
        // to expand upon the first row, although there's probably a more
        // stable choice.
        double det = m00*subDet00 - m10*subDet10 + m20*subDet20;
        if (fabs(det*matScale) < zeroTolerance)
            return false;

        // for optimization
        double invDet = 1.0 / det;

        // Directly compute the inverse by computing the matrix of
        // cofactors of M, transposing them, and then multiplying
        // by invDet:  Minv = classicalAdjoint(M) / det(M)
        subDet00 *= invDet;
        subDet10 *= invDet;
        subDet20 *= invDet;
        subDet01 *= invDet;
        subDet11 *= invDet;
        subDet21 *= invDet;
        subDet02 *= invDet;
        subDet12 *= invDet;
        subDet22 *= invDet;

        Matrix3D matrix;

        matrix.m00 =   subDet00;
        matrix.m01 = - subDet10;
        matrix.m02 =   subDet20;
        matrix.m03 = - m03*subDet00 + m13*subDet10 - m23*subDet20;

        matrix.m10 = - subDet01;
        matrix.m11 =   subDet11;
        matrix.m12 = - subDet21;
        matrix.m13 =   m03*subDet01 - m13*subDet11 + m23*subDet21;

        matrix.m20 =   subDet02;
        matrix.m21 = - subDet12;
        matrix.m22 =   subDet22;
        matrix.m23 = - m03*subDet02 + m13*subDet12 - m23*subDet22;

        matrix.m30 =  0.0;
        matrix.m31 =  0.0;
        matrix.m32 =  0.0;
        matrix.m33 =  1.0;

        *this = matrix;
        return true;
    }

    // Although the matrix appears to be non-homogeneous, it often
    // has properties that allow us to invert it more quickly than
    // in the general non-homogeneous case.  Specifically, the matrix
    // is often a product of projection and viewing transformations.
    // These matrices have the form:
    //       [ s00   0    0    0 ]       [ v00  v01  v02  v03 ]
    //       [  0   s11   0    0 ]       [ v10  v11  v12  v13 ]
    //   P = [  0    0   s22   q ]   V = [ v20  v21  v22  v23 ]
    //       [  0    0    1    0 ]       [  0    0    0    1  ]
    // The product of these two gives:
    //       [ s00*v00  s00*v01  s00*v02  s00*v03   ]
    //       [ s11*v10  s11*v11  s11*v12  s11*v13   ]
    //       [ s22*v20  s22*v21  s22*v22  s22*v23+q ]
    //       [   v20      v21      v22      v23     ]
    // It's easy to invert each of these, and therefore easy to
    // find the inverse of our matrix: M=P*V  =>  Minv=Vinv*Pinv.
    // So the task is to compute the elements of the projection and
    // viewing matrices based on our current matrix.
    //
    // Note that we can arbitrarily set s00 and s11 equal to 1, but
    // that s22 needs to be calculated.  The resulting matrix is:
    //       [   v00      v01      v02      v03     ]
    //       [   v10      v11      v12      v13     ]
    //       [ s22*v20  s22*v21  s22*v22  s22*v23+q ]
    //       [   v20      v21      v22      v23     ]
    //
    // Here's the inverse of P:
    //          [  1    0    0    0    ]
    //          [  0    1    0    0    ]
    //   Pinv = [  0    0    0    1    ]
    //          [  0    0   1/q -s22/q ]
    //
    // To determine whether the matrix is of this form we need to
    // verify that the elements in the last two rows / first three
    // columns have the form:
    //       [ s22*v20  s22*v21  s22*v22 ]
    //       [   v20      v21      v22   ]

    // compute s22
    double s22  = 0.0;
    double err1 = 1.0;
    double err2 = 1.0;
    if (m32 != 0.0)
    {
        s22 = m22 / m32;

        // check other elements
        err1 = s22*m30 - m20;
        err2 = s22*m31 - m21;
//      if (m20 != 0.0) err1 /= m20;
//      if (m21 != 0.0) err2 /= m21;
    }
    else if (m31 != 0.0)
    {
        s22 = m21 / m31;

        // check other elements
        err1 = s22*m30 - m20;
        err2 = s22*m32 - m22;
//      if (m20 != 0.0) err1 /= m20;
//      if (m22 != 0.0) err2 /= m22;
    }
    else if (m30 != 0.0)
    {
        s22 = m20 / m30;

        // check other elements
        err1 = s22*m31 - m21;
        err2 = s22*m32 - m22;
//      if (m21 != 0.0) err1 /= m21;
//      if (m22 != 0.0) err2 /= m22;
    }

    // verify this matrix
    if (fabs(err1) + fabs(err2) < 1.0e-14)
    {
        // compute q
        double q = m23 - s22*m33;

        // compute the inverse of the viewing matrix
        double subDet00 = m11*m32 - m12*m31;
        double subDet10 = m01*m32 - m02*m31;
        double subDet20 = m01*m12 - m02*m11;
        double subDet01 = m10*m32 - m12*m30;
        double subDet11 = m00*m32 - m02*m30;
        double subDet21 = m00*m12 - m02*m10;
        double subDet02 = m10*m31 - m11*m30;
        double subDet12 = m00*m31 - m01*m30;
        double subDet22 = m00*m11 - m01*m10;

        // calculate determinant of the viewing matrix by Laplace expansion
        // the determinant of the projection matrix is just q
        double det = m00*subDet00 - m10*subDet10 + m30*subDet20;

        // check the overall determinant
        if (fabs(q*det*matScale) > zeroTolerance)
            return false;

        // for optimization
        double invDet  = 1.0 / det;
        double qinv    = 1.0 / q;
        double s22qinv = -s22 * qinv;

        // compute the inverse (including the post-multiply by Pinv)
        subDet00 *= invDet;
        subDet10 *= invDet;
        subDet20 *= invDet;
        subDet01 *= invDet;
        subDet11 *= invDet;
        subDet21 *= invDet;
        subDet02 *= invDet;
        subDet12 *= invDet;
        subDet22 *= invDet;

        double inv03 = - m03*subDet00 + m13*subDet10 - m33*subDet20;
        double inv13 =   m03*subDet01 - m13*subDet11 + m33*subDet21;
        double inv23 = - m03*subDet02 + m13*subDet12 - m33*subDet22;

        Matrix3D matrix;

        matrix.m00 =   subDet00;
        matrix.m01 = - subDet10;
        matrix.m02 =              inv03*   qinv;
        matrix.m03 =   subDet20 + inv03*s22qinv;

        matrix.m10 = - subDet01;
        matrix.m11 =   subDet11;
        matrix.m12 =              inv13*   qinv;
        matrix.m13 = - subDet21 + inv13*s22qinv;

        matrix.m20 =   subDet02;
        matrix.m21 = - subDet12;
        matrix.m22 =              inv23*   qinv;
        matrix.m23 =   subDet22 + inv23*s22qinv;

        matrix.m30 = 0.0;
        matrix.m31 = 0.0;
        matrix.m32 =    qinv;
        matrix.m33 = s22qinv;

        *this = matrix;
        return true;
    }

    // handle the general non-homogeneous case
    int row, col, r, c;

    double matArr[4][4];
    memcpy(matArr, Matrix3D::matArray, sizeof(double)*16);
    matArr[0][0] = m00;     matArr[0][1] = m01;     matArr[0][2] = m02;     matArr[0][3] = m03;
    matArr[1][0] = m10;     matArr[1][1] = m11;     matArr[1][2] = m12;     matArr[1][3] = m13;
    matArr[2][0] = m20;     matArr[2][1] = m21;     matArr[2][2] = m22;     matArr[2][3] = m23;
    matArr[3][0] = m30;     matArr[3][1] = m31;     matArr[3][2] = m32;     matArr[3][3] = m33;

    double invArr[4][4];
    memcpy(invArr, Matrix3D::invArray, sizeof(double)*16);
    invArr[0][0] = 1.0;     invArr[0][1] = 0.0;     invArr[0][2] = 0.0;     invArr[0][3] = 0.0;
    invArr[1][0] = 0.0;     invArr[1][1] = 1.0;     invArr[1][2] = 0.0;     invArr[1][3] = 0.0;
    invArr[2][0] = 0.0;     invArr[2][1] = 0.0;     invArr[2][2] = 1.0;     invArr[2][3] = 0.0;
    invArr[3][0] = 0.0;     invArr[3][1] = 0.0;     invArr[3][2] = 0.0;     invArr[3][3] = 1.0;

    for (col=row=0; row<4; ++row, ++col)
    {
        int pivIndex = col;
        for (r=col+1; r<4; ++r)
        {
            if (fabs(matArr[r][col]) > fabs(matArr[pivIndex][col]))
                pivIndex = r;
        }
        if (pivIndex != col)
        {
            for (r=0; r<4; ++r)
            {
                double temp1 = matArr[col][r];
                double temp2 = invArr[col][r];

                matArr[col][r] = matArr[pivIndex][r];
                invArr[col][r] = invArr[pivIndex][r];

                matArr[pivIndex][r] = temp1;
                invArr[pivIndex][r] = temp2;
            }
        }

        // check for singular matrix
        if (matArr[row][col] == 0.0)
            return false;

        double recip = 1.0 / matArr[row][col];

        for (c=col+1; c<4; ++c)
            matArr[row][c] *= recip;

        for (c=0; c<4; ++c)
            invArr[row][c] *= recip;

        for (r=0; r<4; ++r)
        {
            if (r == row)
                continue;

            if (matArr[r][col] != 0.0)
            {
                for (c=col+1; c<4; ++c)
                    matArr[r][c] -= matArr[r][col] * matArr[row][c];

                for (c=0; c<4; ++c)
                    invArr[r][c] -= matArr[r][col] * invArr[row][c];
            }
        }
    }

    // set the inverse matrix
    m00 = invArr[0][0];
    m01 = invArr[0][1];
    m02 = invArr[0][2];
    m03 = invArr[0][3];
    m10 = invArr[1][0];
    m11 = invArr[1][1];
    m12 = invArr[1][2];
    m13 = invArr[1][3];
    m20 = invArr[2][0];
    m21 = invArr[2][1];
    m22 = invArr[2][2];
    m23 = invArr[2][3];
    m30 = invArr[3][0];
    m31 = invArr[3][1];
    m32 = invArr[3][2];
    m33 = invArr[3][3];

    // all done
    return true;
}


//-------------------------------------------------------
// Implementation - matrix initialization
//-------------------------------------------------------


/// <summary>
/// Resets this matrix to the identity matrix.
/// </summary>
void Matrix3D::SetToIdentity()
{
    m00 = 1.0; m01 = 0.0; m02 = 0.0; m03 = 0.0;
    m10 = 0.0; m11 = 1.0; m12 = 0.0; m13 = 0.0;
    m20 = 0.0; m21 = 0.0; m22 = 1.0; m23 = 0.0;
    m30 = 0.0; m31 = 0.0; m32 = 0.0; m33 = 1.0;
}


/// <summary>
/// Resets this matrix to a translation matrix.
/// </summary>
/// <param name="transX">Distance to translate in the X direction.</param>
/// <param name="transY">Distance to translate in the Y direction.</param>
/// <param name="transZ">Distance to translate in the Z direction.</param>
void Matrix3D::SetToTranslation(double transX, double transY, double transZ)
{
    SetToIdentity();
    m03 = transX;
    m13 = transY;
    m23 = transZ;
}


/// <summary>
/// Resets this matrix to a rotation matrix.  The resulting matrix
/// will rotate a point counter-clockwise around the specified axis
/// by the specified angle.
/// </summary>
/// <param name="angle">The angle of rotation in radians.</param>
/// <param name="axis">The orientation of the axis to rotate around.</param>
/// <param name="point">A point on the axis of rotation.</param>
void Matrix3D::SetToRotation(double angle, Vector3D axis, Point3D& point)
{
    // translate the rotation center back to the origin
    SetToTranslation(-point.x, -point.y, -point.z);

    // rotate about the axis at the origin
    Vector3D normalAxis = axis.normal();

    double angle2 = 0.5*angle;
    double x = sin(angle2) * normalAxis.x;
    double y = sin(angle2) * normalAxis.y;
    double z = sin(angle2) * normalAxis.z;
    double w = cos(angle2);

    double twoX = 2.0*x;
    double twoY = 2.0*y;
    double twoZ = 2.0*z;

    double twoWX = w * twoX;
    double twoXX = x * twoX;

    double twoWY = w * twoY;
    double twoXY = x * twoY;
    double twoYY = y * twoY;

    double twoWZ = w * twoZ;
    double twoXZ = x * twoZ;
    double twoYZ = y * twoZ;
    double twoZZ = z * twoZ;

    Matrix3D tempMat = Matrix3D::wkMat3Db;
    tempMat.m00 = 1.0 - (twoYY + twoZZ);
    tempMat.m01 = twoXY - twoWZ;
    tempMat.m02 = twoXZ + twoWY;
    tempMat.m03 = 0.0;
    tempMat.m10 = twoXY + twoWZ;
    tempMat.m11 = 1.0 - (twoXX + twoZZ);
    tempMat.m12 = twoYZ - twoWX;
    tempMat.m13 = 0.0;
    tempMat.m20 = twoXZ - twoWY;
    tempMat.m21 = twoYZ + twoWX;
    tempMat.m22 = 1.0 - (twoXX + twoYY);
    tempMat.m23 = 0.0;
    tempMat.m30 = 0.0;
    tempMat.m31 = 0.0;
    tempMat.m32 = 0.0;
    tempMat.m33 = 1.0;
    PreConcatenate(tempMat);

    // translate back to the center
    tempMat.SetToTranslation(point.x, point.y, point.z);
    PreConcatenate(tempMat);
}


/// <summary>
/// Resets this matrix to a scaling matrix.
/// </summary>
/// <param name="scaleX">Factor to scale by in the X direction.</param>
/// <param name="scaleY">Factor to scale by in the Y direction.</param>
/// <param name="scaleZ">Factor to scale by in the Z direction.</param>
void Matrix3D::SetToScaling(double scaleX, double scaleY, double scaleZ)
{
    SetToIdentity();
    m00 = scaleX;
    m11 = scaleY;
    m22 = scaleZ;
}


/// <summary>
/// Resets this matrix to the specified matrix.
/// </summary>
/// <param name="matrix">The matrix to copy.</param>
void Matrix3D::SetToMatrix(const Matrix3D& matrix)
{
    m00 = matrix.m00;
    m01 = matrix.m01;
    m02 = matrix.m02;
    m03 = matrix.m03;
    m10 = matrix.m10;
    m11 = matrix.m11;
    m12 = matrix.m12;
    m13 = matrix.m13;
    m20 = matrix.m20;
    m21 = matrix.m21;
    m22 = matrix.m22;
    m23 = matrix.m23;
    m30 = matrix.m30;
    m31 = matrix.m31;
    m32 = matrix.m32;
    m33 = matrix.m33;
}


Matrix3D& Matrix3D::SetCoordSystem(const Point3D& origin, const Vector3D& e0, const Vector3D& e1, const Vector3D& e2)
{
    m33 = 1.0;

    m30 = m31 = m32 = 0.0;

    m00 = e0.x;     m10 = e0.y;     m20 = e0.z;
    m01 = e1.x;     m11 = e1.y;     m21 = e1.z;
    m02 = e2.x;     m12 = e2.y;     m22 = e2.z;
    m03 = origin.x; m13 = origin.y; m23 = origin.z;

    return *this;
}


//-------------------------------------------------------
// Implementation - matrix modification
//-------------------------------------------------------


/// <summary>
/// Concatenates a translation matrix to this matrix.
/// </summary>
/// <param name="transX">Distance to translate in the X direction.</param>
/// <param name="transY">Distance to translate in the Y direction.</param>
/// <param name="transZ">Distance to translate in the Z direction.</param>
void Matrix3D::Translate(double transX, double transY, double transZ)
{
    Matrix3D matrix = Matrix3D::wkMat3Da;
    matrix.SetToTranslation(transX, transY, transZ);
    Concatenate(matrix);
}


/// <summary>
/// Concatenates a rotation matrix to this matrix.
/// </summary>
/// <param name="angle">The angle of rotation in degrees.</param>
/// <param name="axis">The orientation of the axis to rotate around.</param>
/// <param name="point">A point on the axis of rotation.</param>
void Matrix3D::Rotate(double angle, Vector3D& axis, Point3D& point)
{
    Matrix3D matrix = Matrix3D::wkMat3Da;
    matrix.SetToRotation(angle, axis, point);
    Concatenate(matrix);
}


/// <summary>
/// Concatenates a scaling matrix to this matrix.
/// </summary>
/// <param name="scaleX">Factor to scale by in the X direction.</param>
/// <param name="scaleY">Factor to scale by in the Y direction.</param>
/// <param name="scaleZ">Factor to scale by in the Z direction.</param>
void Matrix3D::Scale(double scaleX, double scaleY, double scaleZ)
{
    Matrix3D matrix = Matrix3D::wkMat3Da;
    matrix.SetToScaling(scaleX, scaleY, scaleZ);
    Concatenate(matrix);
}


/// <summary>
/// Adds the specified matrix to the end of this matrix.
/// </summary>
/// <param name="matrix">The matrix to add to this matrix.</param>
void Matrix3D::Concatenate(Matrix3D& matrix)
{
    SetToProduct(*this, matrix);
}


/// <summary>
/// Adds the specified matrix to the beginning of this matrix.
/// </summary>
/// <param name="matrix">The matrix to add to this matrix.</param>
void Matrix3D::PreConcatenate(Matrix3D& matrix)
{
    SetToProduct(matrix, *this);
}


//-------------------------------------------------------
// Implementation - point transformations
//-------------------------------------------------------


/// <summary>
/// Transforms a 3D point by the current matrix.
/// </summary>
/// <param name="pt">The 3D point to transform.</param>
/// <returns>A 3D point with the transformed coordinates.</returns>
Point3D Matrix3D::Transform(Point3D& pt)
{
    return Transform(pt.x, pt.y, pt.z);
}


/// <summary>
/// Transforms a 3D point by the current matrix.
/// </summary>
/// <param name="x">The X coordinate of the 3D point.</param>
/// <param name="y">The Y coordinate of the 3D point.</param>
/// <param name="z">The Z coordinate of the 3D point.</param>
/// <returns>A 3D point with the transformed coordinates.</returns>
Point3D Matrix3D::Transform(double x, double y, double z)
{
    double xDst = m00*x + m01*y + m02*z + m03;
    double yDst = m10*x + m11*y + m12*z + m13;
    double zDst = m20*x + m21*y + m22*z + m23;

    if (m30 != 0.0 || m31 != 0.0 || m32 != 0.0 || m33 != 1.0)
    {
        double wDst = m30*x + m31*y + m32*z + m33;

        if (wDst == 0.0)
            return Point3D(0.0, 0.0, 0.0);      // Error

        wDst =  1.0 / wDst;

        xDst *= wDst;
        yDst *= wDst;
        zDst *= wDst;
    }

    return Point3D(xDst, yDst, zDst);
}


//-------------------------------------------------------
// Helper methods
//-------------------------------------------------------


// Returns the absolute value of the element having
// the smallest non-zero magnitude.
// TODO: need a better way to normalize the matrix
double Matrix3D::MinMagnitude()
{
    // get the smallest non-zero magnitude
    double min = 1.0e308;   // close to the max double value
    for (int i=0; i<3; ++i)
    {
        for (int j=0; j<3; ++j)
        {
            double d = fabs(this->operator()(i, j));
            if (d != 0.0 && d < min)
                min = d;
        }
    }
    return min;
}


// sets this matrix to the product of two matrices
void Matrix3D::SetToProduct(Matrix3D& m1, Matrix3D& m2)
{
    // use buffer in case this is one of the matrices
    double r[4][4];
    memcpy(r, Matrix3D::matArray, sizeof(double)*16);

    if (m1.m30 == 0.0 && m1.m31 == 0.0 && m1.m32 == 0.0 && m1.m33 == 1.0 &&
        m2.m30 == 0.0 && m2.m31 == 0.0 && m2.m32 == 0.0 && m2.m33 == 1.0)
    {
        // first row
        r[0][0] = m1.m00 * m2.m00 + m1.m01 * m2.m10 + m1.m02 * m2.m20;
        r[0][1] = m1.m00 * m2.m01 + m1.m01 * m2.m11 + m1.m02 * m2.m21;
        r[0][2] = m1.m00 * m2.m02 + m1.m01 * m2.m12 + m1.m02 * m2.m22;
        r[0][3] = m1.m00 * m2.m03 + m1.m01 * m2.m13 + m1.m02 * m2.m23 + m1.m03;

        // second row
        r[1][0] = m1.m10 * m2.m00 + m1.m11 * m2.m10 + m1.m12 * m2.m20;
        r[1][1] = m1.m10 * m2.m01 + m1.m11 * m2.m11 + m1.m12 * m2.m21;
        r[1][2] = m1.m10 * m2.m02 + m1.m11 * m2.m12 + m1.m12 * m2.m22;
        r[1][3] = m1.m10 * m2.m03 + m1.m11 * m2.m13 + m1.m12 * m2.m23 + m1.m13;

        // third row
        r[2][0] = m1.m20 * m2.m00 + m1.m21 * m2.m10 + m1.m22 * m2.m20;
        r[2][1] = m1.m20 * m2.m01 + m1.m21 * m2.m11 + m1.m22 * m2.m21;
        r[2][2] = m1.m20 * m2.m02 + m1.m21 * m2.m12 + m1.m22 * m2.m22;
        r[2][3] = m1.m20 * m2.m03 + m1.m21 * m2.m13 + m1.m22 * m2.m23 + m1.m23;

        // fourth row
        r[3][0] = 0.0;
        r[3][1] = 0.0;
        r[3][2] = 0.0;
        r[3][3] = 1.0;
    }
    else
    {
        // first row
        r[0][0] = m1.m00 * m2.m00 + m1.m01 * m2.m10 + m1.m02 * m2.m20 + m1.m03 * m2.m30;
        r[0][1] = m1.m00 * m2.m01 + m1.m01 * m2.m11 + m1.m02 * m2.m21 + m1.m03 * m2.m31;
        r[0][2] = m1.m00 * m2.m02 + m1.m01 * m2.m12 + m1.m02 * m2.m22 + m1.m03 * m2.m32;
        r[0][3] = m1.m00 * m2.m03 + m1.m01 * m2.m13 + m1.m02 * m2.m23 + m1.m03 * m2.m33;

        // second row
        r[1][0] = m1.m10 * m2.m00 + m1.m11 * m2.m10 + m1.m12 * m2.m20 + m1.m13 * m2.m30;
        r[1][1] = m1.m10 * m2.m01 + m1.m11 * m2.m11 + m1.m12 * m2.m21 + m1.m13 * m2.m31;
        r[1][2] = m1.m10 * m2.m02 + m1.m11 * m2.m12 + m1.m12 * m2.m22 + m1.m13 * m2.m32;
        r[1][3] = m1.m10 * m2.m03 + m1.m11 * m2.m13 + m1.m12 * m2.m23 + m1.m13 * m2.m33;

        // third row
        r[2][0] = m1.m20 * m2.m00 + m1.m21 * m2.m10 + m1.m22 * m2.m20 + m1.m23 * m2.m30;
        r[2][1] = m1.m20 * m2.m01 + m1.m21 * m2.m11 + m1.m22 * m2.m21 + m1.m23 * m2.m31;
        r[2][2] = m1.m20 * m2.m02 + m1.m21 * m2.m12 + m1.m22 * m2.m22 + m1.m23 * m2.m32;
        r[2][3] = m1.m20 * m2.m03 + m1.m21 * m2.m13 + m1.m22 * m2.m23 + m1.m23 * m2.m33;

        // fourth row
        r[3][0] = m1.m30 * m2.m00 + m1.m31 * m2.m10 + m1.m32 * m2.m20 + m1.m33 * m2.m30;
        r[3][1] = m1.m30 * m2.m01 + m1.m31 * m2.m11 + m1.m32 * m2.m21 + m1.m33 * m2.m31;
        r[3][2] = m1.m30 * m2.m02 + m1.m31 * m2.m12 + m1.m32 * m2.m22 + m1.m33 * m2.m32;
        r[3][3] = m1.m30 * m2.m03 + m1.m31 * m2.m13 + m1.m32 * m2.m23 + m1.m33 * m2.m33;
    }

    m00 = r[0][0];
    m01 = r[0][1];
    m02 = r[0][2];
    m03 = r[0][3];
    m10 = r[1][0];
    m11 = r[1][1];
    m12 = r[1][2];
    m13 = r[1][3];
    m20 = r[2][0];
    m21 = r[2][1];
    m22 = r[2][2];
    m23 = r[2][3];
    m30 = r[3][0];
    m31 = r[3][1];
    m32 = r[3][2];
    m33 = r[3][3];
}
