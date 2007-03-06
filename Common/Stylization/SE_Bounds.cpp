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

#include "SE_Bounds.h"
#include "SE_LineBuffer.h"
#include "SE_ConvexHull.h"
#include <float.h>

void SE_Bounds::Add(double x, double y)
{
    if (size >= capacity)
        return;
    hull[2*size] = x;
    hull[2*size+1] = y;

    if (min[0] > x)
        min[0] = x;
    if (max[0] < x)
        max[0] = x;
    if (min[1] > y)
        min[1] = y;
    if (max[1] < y)
        max[1] = y;

    size++;
}

void SE_Bounds::Transform(SE_Matrix& xform)
{
    double *last = hull + 2*size;
    double *cur = hull;
    
    while (cur < last)
        xform.transform(*cur++, *cur++);
}

struct SimplePoint
{
    double x;
    double y;
};

struct SimplePointUtil
{
    SE_INLINE double x(SimplePoint* point)
    {
        return point->x;
    }

    SE_INLINE double y(SimplePoint* point)
    {
        return point->y;
    }

    SE_INLINE bool equal(SimplePoint* a, SimplePoint* b)
    {
        return a->x == b->x && a->y == b->y;
    }
};

void SE_Bounds::Free()
{
    pool->FreeBounds(this);
}

SE_Bounds* SE_Bounds::Clone()
{
    SE_Bounds* clone = pool->NewBounds(size);
    clone->size = size;
    clone->pivot = pivot;
    clone->min[0] = min[0];
    clone->min[1] = min[1];
    clone->max[0] = max[0];
    clone->max[1] = max[1];
    memcpy(clone->hull, hull, size*2*sizeof(double));
    return clone;
}

void SE_Bounds::Contained(double minx, double miny, double maxx, double maxy, double &growx, double &growy)
{
    double sx, sy;
    
    if (min[0] < minx)
    {
        sx = (minx - min[0])/(maxx - minx);
        growx = (growx > sx) ? growx : sx;
    }
    if (max[0] > maxx)
    {
        sx = (max[0] - maxx)/(maxx - minx);
        growx = (growx > sx) ? growx : sx;
    }
    if (min[1] < miny)
    {
        sy = (miny - min[1])/(maxy - miny);
        growy = (growy > sy) ? growy : sy;
    }
    if (max[1] < maxy)
    {
        sy = (max[1] - maxy)/(maxy - miny);
        growy = (growy > sy) ? growy : sy;
    }
}

SE_Bounds* SE_Bounds::Union(SE_Bounds* bounds)
{
    /* Andrew Convex Hull again, but this time in linear time (the points of the
     * convex hulls can be sorted in O(n) instead of O(n log n) since the U and L
     * lines were both already in lexographic order. */
    int usize = bounds->size + size;
    double* vec;
    if (usize > 4096) // we don't want an overflow here
        vec = new double[2*usize];
    else
        vec = (double*)alloca(sizeof(double)*2*usize);
        
    double* start[4] = {hull, hull + 2*size - 2, bounds->hull, bounds->hull + 2*bounds->size - 2};
    double* end[4] = {hull + 2*pivot, hull + 2*pivot - 2, bounds->hull + 2*bounds->pivot, bounds->hull + 2*bounds->pivot - 2};

    bool invalid[4];
    int pnts = 0;

    /* Create a sorted list of all the vertices in both convex hulls */
    for(;;)
    {
        invalid[0] = start[0] >= end[0];
        invalid[1] = start[1] <= end[1];
        invalid[2] = start[2] >= end[2];
        invalid[3] = start[3] <= end[3];

        if (invalid[0] && invalid[1] && invalid[2] && invalid[3])
            break;

        double lx = DBL_MAX, ly = DBL_MAX;
        int index = -1;

        for (int i = 0; i < 4; i++)
            if (!invalid[i] && ((*start[i] < lx) || (*start[i] == lx && *(start[i]+1) <= ly)))
            {
                lx = *start[i];
                ly = *(start[i]+1);
                index = i;
            }
        if (index & 1) // U array
            start[index] -= 2;
        else // L array
            start[index] += 2;
        if (pnts < 2 || vec[pnts-2] != lx || vec[pnts-1] != ly)
        {
            vec[pnts++] = lx;
            vec[pnts++] = ly;
        }
    }

    double* last = vec + pnts - 2;
    double* first = vec;

    SE_Bounds* ubounds = AndrewHull<SimplePoint*, SimplePointUtil>((SimplePoint*)first, (SimplePoint*)last, pnts/2, pool);    

    if (usize > 4096)
        delete[] vec;

    return ubounds;
}