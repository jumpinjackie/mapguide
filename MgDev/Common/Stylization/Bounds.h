//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#define rs_max(a,b)            (((a) > (b)) ? (a) : (b))
#define rs_min(a,b)            (((a) < (b)) ? (a) : (b))

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
};

#endif
