//
//  Copyright (C) 2007 Autodesk, Inc.
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

#ifndef SE_BOUNDS_H
#define SE_BOUNDS_H

#include "Stylization.h"
#include "SE_Matrix.h"

struct SE_Bounds
{
friend class SE_LineBuffer;
friend class SE_LineBufferPool;
private:
    SE_Bounds();
    int capacity;
    SE_LineBufferPool* pool;

public:
    double* hull;
    int size;
    int pivot;

    double min[2];
    double max[2];

    STYLIZATION_API void Add(double x, double y);
    STYLIZATION_API void Transform(SE_Matrix& xform);
    STYLIZATION_API void Contained(double minx, double miny, double maxx, double maxy, double &growx, double &growy);
    STYLIZATION_API void Free();
    STYLIZATION_API SE_Bounds* Clone();
    STYLIZATION_API SE_Bounds* Union(SE_Bounds* bounds);
};

#endif // SE_BOUNDS_H