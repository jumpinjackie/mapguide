//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef complex_polygon_gd_H
#define complex_polygon_gd_H

#include "gd.h"

struct EdgeState;
struct Point;

class complex_polygon_gd
{
public:
    complex_polygon_gd();
    ~complex_polygon_gd();

    void FillPolygon(Point * VertexList, int nVerts, int* Contours, int nContours, int Color, gdImagePtr target);

private:
    //This is what you want to implement for your own render target
    //for all the rest of the code to work with it
    inline void DrawHorizontalLineSeg(int Y, int StartX, int EndX, int Color, gdImagePtr target)
    {
        if (StartX <= EndX)
            gdImageLine(target, StartX, Y, EndX, Y, Color);
    }

    void BuildGET(Point *, int* Contours, int nContours, EdgeState *, int MaxY);
    void MoveXSortedToAET(int);
    void ScanOutAET(int, int, gdImagePtr);
    void AdvanceAET();
    void XSortAET();

    /* Pointers to global edge table (GET) and active edge table (AET) */
    struct EdgeState *m_GETPtr, *m_AETPtr;
    EdgeState* m_bufEdgeState;
    size_t m_nBufEdgeState;
};

#endif
