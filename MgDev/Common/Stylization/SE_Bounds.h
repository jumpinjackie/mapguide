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

#ifndef SE_BOUNDS_H_
#define SE_BOUNDS_H_

#include "StylizationAPI.h"
#include "SE_Matrix.h"

class SE_BufferPool;

struct SE_Bounds
{
friend class SE_BufferPool;
private:
    SE_Bounds();
    ~SE_Bounds();
    int capacity;
    SE_BufferPool* pool;

public:
    double* hull;
    int size;
    int pivot;

    double min[2];
    double max[2];

    STYLIZATION_API void Add(double x, double y);
    STYLIZATION_API void Transform(const SE_Matrix& xform);
    STYLIZATION_API void Transform(const SE_Matrix& xform, SE_Bounds* src);
    STYLIZATION_API bool Contained(double minx, double miny, double maxx, double maxy);
    STYLIZATION_API void Contained(double minx, double miny, double maxx, double maxy, double &growx, double &growy);
    STYLIZATION_API void Free();
    STYLIZATION_API SE_Bounds* Clone(bool keepPool = true);
};

#endif
