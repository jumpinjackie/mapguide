//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef SE_MATRIX_H_
#define SE_MATRIX_H_

#include "StylizationDefs.h"

/* For the points P0, P1, P2, true if P2 is left of (P0, P1),
 * as indicated by sign of the cross product (P0, P1) X (P0, P2) */
#define PointLeft(pt0x, pt0y, pt1x, pt1y, pt2x, pt2y) \
    (((pt1x - pt0x)*(pt2y - pt0y) - (pt2x - pt0x)*(pt1y - pt0y)) > 0)

struct SE_Matrix
{
    double x0, x1, x2, y0, y1, y2;

    static const SE_Matrix Identity;

    SE_INLINE SE_Matrix();
    SE_INLINE SE_Matrix(const SE_Matrix & matrix);
    SE_INLINE SE_Matrix(double x0, double x1, double x2,
                        double y0, double y1, double y2);

    SE_INLINE void setIdentity();
    SE_INLINE bool isIdentity() const;
    /* Scale, then translate */
    SE_INLINE void setTransform(double scaleX, double scaleY, double transX, double transY);
    /* Scale, then rotate, then translate */
    SE_INLINE void setTransform(double scaleX, double scaleY, double transX, double transY, double rot);
    /* Pre-scale */
    SE_INLINE void scaleX(double x);
    SE_INLINE void scaleY(double y);
    SE_INLINE void scale(double x, double y);
    /* Pre-translate */
    SE_INLINE void translateX(double x);
    SE_INLINE void translateY(double y);
    SE_INLINE void translate(double x, double y);
    /* Pre-rotate */
    SE_INLINE void rotate(double angleRad);
    SE_INLINE void rotate(double angleSin, double angleCos);

    SE_INLINE void premultiply(const SE_Matrix& matrix);
    SE_INLINE void postmultiply(const SE_Matrix& matrix);
    SE_INLINE void transform(double& x, double& y) const;
    SE_INLINE void transform(double x, double y, double& tx, double& ty) const;
    SE_INLINE void transformVector(double& x, double& y) const;
    SE_INLINE void transformVector(double x, double y, double& tx, double &ty) const;

    SE_INLINE void inverse(SE_Matrix& inv);

    SE_INLINE void operator*=(const SE_Matrix& matrix);
    SE_INLINE bool operator==(const SE_Matrix& matrix);
};


// Function definitions

SE_Matrix::SE_Matrix() :
    x0(1.0), x1(0.0), x2(0.0),
    y0(0.0), y1(1.0), y2(0.0)
{ }


SE_Matrix::SE_Matrix( double a00, double a01, double a02,
                      double a10, double a11, double a12) :
    x0(a00), x1(a01), x2(a02),
    y0(a10), y1(a11), y2(a12)
{ }


SE_Matrix::SE_Matrix(const SE_Matrix & matrix) :
    x0(matrix.x0), x1(matrix.x1), x2(matrix.x2),
    y0(matrix.y0), y1(matrix.y1), y2(matrix.y2)
{ }


void SE_Matrix::setIdentity()
{
    x0 = y1 = 1.0;
    x1 = x2 = y0 = y2 = 0.0;
}


bool SE_Matrix::isIdentity() const
{
    return (x0 == 1.0 && y1 == 1.0 && x1 == 0.0 && x2 == 0.0 && y0 == 0.0 && y2 == 0.0);
}


void SE_Matrix::setTransform(double scaleX, double scaleY, double transX, double transY)
{
    x0 = scaleX;
    x1 = 0.0;
    x2 = transX;

    y0 = 0.0;
    y1 = scaleY;
    y2 = transY;
}


void SE_Matrix::setTransform(double scaleX, double scaleY, double transX, double transY, double rot)
{
    double sn = sin(rot);
    double cs = cos(rot);

    x0 =  scaleX*cs;
    x1 = -scaleY*sn;
    x2 =  transX;

    y0 =  scaleX*sn;
    y1 =  scaleY*cs;
    y2 =  transY;
}


void SE_Matrix::scaleX(double x)
{
    x0 *= x;
    x1 *= x;
    x2 *= x;
}


void SE_Matrix::scaleY(double y)
{
    y0 *= y;
    y1 *= y;
    y2 *= y;
}


void SE_Matrix::scale(double x, double y)
{
    x0 *= x;
    x1 *= x;
    x2 *= x;
    y0 *= y;
    y1 *= y;
    y2 *= y;
}


void SE_Matrix::translateX(double x)
{
    x2 += x;
}


void SE_Matrix::translateY(double y)
{
    y2 += y;
}


void SE_Matrix::translate(double x, double y)
{
    x2 += x;
    y2 += y;
}


void SE_Matrix::rotate(double angleRad)
{
    double cs = cos(angleRad);
    double sn = sin(angleRad);

    double a00 = x0*cs - y0*sn;
    double a10 = y0*cs + x0*sn;

    double a01 = x1*cs - y1*sn;
    double a11 = y1*cs + x1*sn;

    double a02 = x2*cs - y2*sn;
    double a12 = y2*cs + x2*sn;

    x0 = a00; x1 = a01; x2 = a02;
    y0 = a10; y1 = a11; y2 = a12;
}


void SE_Matrix::rotate(double angleSin, double angleCos)
{
    double a00 = x0*angleCos - y0*angleSin;
    double a10 = y0*angleCos + x0*angleSin;

    double a01 = x1*angleCos - y1*angleSin;
    double a11 = y1*angleCos + x1*angleSin;

    double a02 = x2*angleCos - y2*angleSin;
    double a12 = y2*angleCos + x2*angleSin;

    x0 = a00; x1 = a01; x2 = a02;
    y0 = a10; y1 = a11; y2 = a12;
}


void SE_Matrix::premultiply(const SE_Matrix & matrix)
{
    double a00 = x0*matrix.x0 + y0*matrix.x1;
    double a10 = x0*matrix.y0 + y0*matrix.y1;

    double a01 = x1*matrix.x0 + y1*matrix.x1;
    double a11 = x1*matrix.y0 + y1*matrix.y1;

    double a02 = x2*matrix.x0 + y2*matrix.x1 + matrix.x2;
    double a12 = x2*matrix.y0 + y2*matrix.y1 + matrix.y2;

    x0 = a00; x1 = a01; x2 = a02;
    y0 = a10; y1 = a11; y2 = a12;
}


void SE_Matrix::postmultiply(const SE_Matrix& matrix)
{
    double a00 = x0*matrix.x0 + x1*matrix.y0;
    double a10 = y0*matrix.x0 + y1*matrix.y0;

    double a01 = x0*matrix.x1 + x1*matrix.y1;
    double a11 = y0*matrix.x1 + y1*matrix.y1;

    double a02 = x0*matrix.x2 + x1*matrix.y2 + x2;
    double a12 = y0*matrix.x2 + y1*matrix.y2 + y2;

    x0 = a00; x1 = a01; x2 = a02;
    y0 = a10; y1 = a11; y2 = a12;
}


void SE_Matrix::transform(double &x, double &y) const
{
    double vx = x;
    double vy = y;
    x = vx*x0 + vy*x1 + x2;
    y = vx*y0 + vy*y1 + y2;
}


void SE_Matrix::transform(double x, double y, double& tx, double& ty) const
{
    tx = x*x0 + y*x1 + x2;
    ty = x*y0 + y*y1 + y2;
}


void SE_Matrix::transformVector(double& x, double& y) const
{
    double vx = x;
    double vy = y;
    x = vx*x0 + vy*x1;
    y = vx*y0 + vy*y1;
}


void SE_Matrix::transformVector(double x, double y, double& tx, double &ty) const
{
    tx = x*x0 + y*x1;
    ty = x*y0 + y*y1;
}


void SE_Matrix::operator*=(const SE_Matrix & matrix)
{
    premultiply(matrix);
}


bool SE_Matrix::operator==(const SE_Matrix& matrix)
{
    return x0 == matrix.x0 && x1 == matrix.x1 && x2 == matrix.x2 &&
           y0 == matrix.y0 && y1 == matrix.y1 && y2 == matrix.y2;
}


void SE_Matrix::inverse(SE_Matrix& inv)
{
    double idet = 1.0 / (x0 * y1 - y0 * x1);

    inv.x0 =  y1 * idet;
    inv.x1 = -x1 * idet;
    inv.y0 = -y0 * idet;
    inv.y1 =  x0 * idet;
    inv.x2 = (y2 * x1 - x2 * y1) * idet;
    inv.y2 = (x2 * y0 - y2 * x0) * idet;
}

#endif
