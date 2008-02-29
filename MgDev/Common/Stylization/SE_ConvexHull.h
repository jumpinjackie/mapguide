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

#ifndef SE_CONVEXHULL_H_
#define SE_CONVEXHULL_H_

#include "SE_BufferPool.h"

//---------------------------------------------
//---------------------------------------------

typedef std::pair<double, double>* SimpleITER;

struct SimplePOINT
{
    SE_INLINE double x(void* point)
    {
        return ((double*)point)[0];
    }

    SE_INLINE double y(void* point)
    {
        return ((double*)point)[1];
    }

    SE_INLINE bool equal(void* a, void* b)
    {
        return x(a) == x(b) && y(a) == y(b);
    }
};

//---------------------------------------------
//---------------------------------------------

// Monotone Chain Convex Hull Algorithm (Andrew, 1979)
// Input is a lexographically sorted (by x, then y) list of points.
// Given the sorted list, the running time is O(n)
template<class ITER, class POINT> SE_Bounds* AndrewHull(ITER spoints, ITER epoints, int npoints, SE_BufferPool* pool)
{
    POINT pnt;
    ITER minxminy, minxmaxy, maxxminy, maxxmaxy;

    ITER iter = spoints;

    minxminy = iter++;
    while ((iter != epoints) && (pnt.x(iter) == pnt.x(minxminy)))
        iter++;
    if (pnt.x(iter) == pnt.x(minxminy)) // all points have the same x coordinate
    {
        SE_Bounds* bounds = SE_BufferPool::NewBounds(pool, 3);
        bounds->pivot = 1;
        bounds->Add(pnt.x(minxminy), pnt.y(minxminy));
        if (pnt.y(iter) != pnt.y(minxminy))
            bounds->Add(pnt.x(iter), pnt.y(iter));
        bounds->Add(pnt.x(minxminy), pnt.y(minxminy));
        return bounds;
    }
    minxmaxy = --iter;

    iter = epoints;
    maxxmaxy = iter--;
    while ((iter != spoints) && (pnt.x(iter) == pnt.x(maxxmaxy)))
        iter--;
    if (pnt.x(iter) == pnt.x(maxxmaxy))
        maxxminy = iter;
    else
        maxxminy = ++iter;

    double* buffer;
    if (npoints > 4096) // we don't want an overflow here
        buffer = new double[4*npoints];
    else
        buffer = (double*)alloca(sizeof(double)*4*npoints);

    /* Compute Lower Hull */
    double* stack = buffer;
    *stack++ = pnt.x(minxmaxy);
    *stack++ = pnt.y(minxmaxy);
    iter = minxmaxy;

    while (iter != maxxminy)
    {
        iter++;

        if (PointLeft(pnt.x(minxminy), pnt.y(minxminy), pnt.x(maxxminy), pnt.y(maxxminy), pnt.x(iter), pnt.y(iter)) && iter != maxxminy)
        /* this point is trivially above the lower bounding line, and cannot be in the lower hull */
            continue;

        while ((stack - buffer) > 2) /* we have to have at least 2 points before this is defined */
        {
            if (PointLeft(*(stack - 4), *(stack - 3), *(stack - 2), *(stack - 1), pnt.x(iter), pnt.y(iter)))
                break; /* current point is inside the lower hull */
            else
                stack -= 2; /* current point is outside the lower hull, replace the last point */
        }
        *stack++ = pnt.x(iter);
        *stack++ = pnt.y(iter);
    }

    /* Compute Upper Hull */
    double *ubuf = stack;

    if (!pnt.equal(maxxmaxy, maxxminy))
    {
        *stack++ = pnt.x(maxxmaxy);
        *stack++ = pnt.y(maxxmaxy);
    }
    iter = maxxminy;

    while (iter != minxmaxy)
    {
        iter --;

        if (PointLeft(pnt.x(maxxmaxy), pnt.y(maxxmaxy), pnt.x(minxmaxy), pnt.y(minxmaxy), pnt.x(iter), pnt.y(iter)) && iter != minxmaxy)
        /* this point is trivially below the upper bounding line, and cannot be in the upper hull */
            continue;

        while ((stack - ubuf) > 2) /* we have to have at least 2 points before this is defined */
        {
            if (PointLeft(*(stack - 4), *(stack - 3), *(stack - 2), *(stack - 1), pnt.x(iter), pnt.y(iter)))
                break; /* current point is inside the upper hull */
            else
                stack -= 2; /* current point is outside the upper hull, replace the last point */
        }
        *stack++ = pnt.x(iter);
        *stack++ = pnt.y(iter);
    }

    if (!pnt.equal(minxmaxy, minxminy))
    {
        *stack++ = pnt.x(minxminy);
        *stack++ = pnt.y(minxminy);
    }

    SE_Bounds* bounds = SE_BufferPool::NewBounds(pool, (int)(stack - buffer)/2);
    bounds->pivot = (int)(ubuf - buffer)/2;
    double* point = buffer;

    while (point < stack)
    {
        double x = *point++;
        double y = *point++;
        bounds->Add(x, y);
    }

    if (npoints > 4096)
        delete[] buffer;

    return bounds;
}

#endif
