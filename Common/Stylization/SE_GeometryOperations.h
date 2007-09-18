//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef SE_GEOMETRYOPERATIONS_H_
#define SE_GEOMETRYOPERATIONS_H_

#include "SE_Matrix.h"
#include "SE_Tuple.h"
#include "LineBuffer.h"

/* 
 * Transforms the geometry in src by tx, and stores the result in dst.
 * The bounds property of dst will not be computed unless updatebounds
 * is true.
 */
void TransformLB( LineBuffer* src, 
                  LineBuffer* dst, 
                  const SE_Matrix& tx, 
                  bool updatebounds )
{
    *dst = *src;

    int size = src->point_count();
    for (int i = 0; i < size; ++i)
        tx.transform(dst->x_coord(i), dst->y_coord(i));

    /* Invalidate bounds (we just transformed everything, after all) */
    RS_Bounds& bounds = const_cast<RS_Bounds&>(dst->bounds());
    bounds.maxx = -DBL_MAX;
    bounds.minx = DBL_MAX;

    if (updatebounds)
        dst->ComputeBounds(bounds);
}

/* 
 * Computes the intersection of (A0, A1) with (B0, B1).
 * If an intersection exists, returns true,
 * and stores the uv (in units of A1-A0, B1-B0) intersection
 * point in T, and the true intersection point in isection.
 */
bool Intersects(const SE_Tuple& A0, const SE_Tuple& A1,
                const SE_Tuple& B0, const SE_Tuple& B1,
                SE_Tuple& T, SE_Tuple& isection)
 {
     SE_Tuple Va  = A1 - A0;
     SE_Tuple Vb  = B1 - B0;
     SE_Tuple Vab = B0 - A0;
     double iaxb = 1.0 / Va.cross(Vb);

     T.x = Vab.cross(Vb) * iaxb;
     T.y = Vab.cross(Va) * iaxb;

     isection = A0 + (Va * T.x);
     
     return T.x >= 0.0 && T.x <= 1.0 && T.y >= 0.0 && T.y <= 1.0;
 }

#endif // SE_GEOMETRYOPERATIONS_H_
