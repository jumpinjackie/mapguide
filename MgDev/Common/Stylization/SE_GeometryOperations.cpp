//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
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

#include "SE_GeometryOperations.h"


void TransformLB(LineBuffer* src,
                 LineBuffer* dst,
                 const SE_Matrix& tx,
                 bool updateBounds)
{
    if (src != dst)
        *dst = *src;

    int size = src->point_count();
    for (int i = 0; i < size; ++i)
        tx.transform(dst->x_coord(i), dst->y_coord(i));

    // invalidate bounds (we just transformed everything, after all)
    RS_Bounds& bounds = const_cast<RS_Bounds&>(dst->bounds());
    bounds.minx = bounds.miny = bounds.minz = +DBL_MAX;
    bounds.maxx = bounds.maxy = bounds.maxz = -DBL_MAX;

    if (updateBounds)
        dst->ComputeBounds(bounds);
}


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
