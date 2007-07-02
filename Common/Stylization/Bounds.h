//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef BOUNDS_H
#define BOUNDS_H

#include "RendererStyles.h"

#define rs_max(a,b)     (((a) > (b)) ? (a) : (b))
#define rs_min(a,b)     (((a) < (b)) ? (a) : (b))

struct RS_Bounds
{
    RS_Bounds()
    {
    }

    RS_Bounds(double dminx, double dminy, double dmaxx, double dmaxy)
    : minx(dminx),
      miny(dminy),
      maxx(dmaxx),
      maxy(dmaxy)
    {
    }

    double width()
    {
        return maxx - minx;
    }

    double height()
    {
        return maxy - miny;
    }

    double minx;
    double miny;
    double maxx;
    double maxy;

    static RS_Bounds Intersect(RS_Bounds& b1, RS_Bounds& b2)
    {
        return RS_Bounds(rs_max(b1.minx, b2.minx),
                         rs_max(b1.miny, b2.miny),
                         rs_min(b1.maxx, b2.maxx),
                         rs_min(b1.maxy, b2.maxy));
    }

    bool IsValid()
    {
        return (width() >= 0) && (height() >=0);
    }

    void add_point(const RS_F_Point& pt)
    {
        if (pt.x < minx) minx = pt.x;
        if (pt.y < miny) miny = pt.y;
        if (pt.x > maxx) maxx = pt.x;
        if (pt.y > maxy) maxy = pt.y;
    }

    void get_points(RS_F_Point* pts)
    {
        pts[0].x = minx;    pts[0].y = miny;
        pts[1].x = maxx;    pts[1].y = miny;
        pts[2].x = maxx;    pts[2].y = maxy;
        pts[3].x = minx;    pts[3].y = maxy;
    }
};

#endif
