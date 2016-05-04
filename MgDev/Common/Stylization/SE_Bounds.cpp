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

#include "stdafx.h"
#include "SE_Bounds.h"
#include "SE_LineBuffer.h"
#include "SE_BufferPool.h"


inline void AddToBounds(double x, double y, double* min, double* max)
{
    if (min[0] > x)
        min[0] = x;
    if (max[0] < x)
        max[0] = x;
    if (min[1] > y)
        min[1] = y;
    if (max[1] < y)
        max[1] = y;
}


SE_Bounds::SE_Bounds()
{
}


SE_Bounds::~SE_Bounds()
{
}


void SE_Bounds::Add(double x, double y)
{
    if (size >= capacity)
        return;
    hull[2*size  ] = x;
    hull[2*size+1] = y;

    AddToBounds(x, y, min, max);

    size++;
}


void SE_Bounds::Transform(const SE_Matrix& xform)
{
    double* last = hull + 2*size;
    double* cur = hull;
    min[0] = min[1] = +DBL_MAX;
    max[0] = max[1] = -DBL_MAX;

    while (cur < last)
    {
        xform.transform(cur[0], cur[1]);
        AddToBounds(cur[0], cur[1], min, max);
        cur += 2;
    }
}


/* NOTE: This method is intentionally unchecked, the caller must ensure that the
 *       destination SE_Bounds is large enough, that the source is valid, etc. */
void SE_Bounds::Transform(const SE_Matrix& xform, SE_Bounds* src)
{
    double* last = src->hull + 2*src->size;
    double* cur = src->hull;
    double* dst = hull;
    min[0] = min[1] = +DBL_MAX;
    max[0] = max[1] = -DBL_MAX;
    size = src->size;
    pivot = src->pivot;

    while (cur < last)
    {
        xform.transform(cur[0], cur[1], dst[0], dst[1]);
        AddToBounds(dst[0], dst[1], min, max);
        cur += 2;
        dst += 2;
    }
}


void SE_Bounds::Free()
{
    SE_BufferPool::FreeBounds(pool, this);
}


SE_Bounds* SE_Bounds::Clone(bool keepPool)
{
    SE_Bounds* clone = SE_BufferPool::NewBounds(pool, size);
    clone->size = size;
    clone->pivot = pivot;
    clone->min[0] = min[0];
    clone->min[1] = min[1];
    clone->max[0] = max[0];
    clone->max[1] = max[1];
    memcpy(clone->hull, hull, size*2*sizeof(double));

    // clear the pool if requested
    if (!keepPool)
        clone->pool = NULL;

    return clone;
}


void SE_Bounds::Contained(double minx, double miny, double maxx, double maxy, double &growx, double &growy)
{
    double sx, sy;
    double cx = (minx + maxx)/2.0;
    double cy = (miny + maxy)/2.0;
    minx -= cx;
    maxx -= cx;
    miny -= cy;
    maxy -= cy;
    double xfminx, xfminy, xfmaxx, xfmaxy;
    xfminx = min[0] - cx;
    xfminy = min[1] - cy;
    xfmaxx = max[0] - cx;
    xfmaxy = max[1] - cy;

    if (xfminx < minx) // minx always negative
    {
        sx = xfminx/minx - 1.0;
        growx = (growx > sx)? growx : sx;
    }
    if (xfmaxx > maxx) // maxx always positive
    {
        sx = xfmaxx/maxx - 1.0;
        growx = (growx > sx)? growx : sx;
    }
    if (xfminy < miny)
    {
        sy = xfminy/miny - 1.0;
        growy = (growy > sy)? growy : sy;
    }
    if (xfmaxy > maxy)
    {
        sy = xfmaxy/maxy - 1.0;
        growy = (growy > sy)? growy : sy;
    }
}


bool SE_Bounds::Contained(double minx, double miny, double maxx, double maxy)
{
    return (minx < min[0]) && (miny < min[1]) && (maxx > max[0]) && (maxy > max[1]);
}
