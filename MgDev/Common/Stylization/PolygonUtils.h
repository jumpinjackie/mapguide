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

#ifndef POLYGONUTILS_H_
#define POLYGONUTILS_H_

#include "LineBuffer.h"
#include <map>
#include <functional>

////////////////////////////////////////////////////////////////////////////////
// Used with overlapping contour computations.
struct RingData
{
    enum RingType
    {
        Unknown,
        Inner,
        Outer
    };

    double m_minX;
    double m_minY;
    double m_maxX;
    double m_maxY;
    double m_area;
    RingType m_type;
    RingData* m_child;
    LineBuffer* m_lineBuffer;
    int m_cntr;

    int ringStartPoint()
    {
        return m_lineBuffer->contour_start_point(m_cntr);
    }

    int ringPointCount()
    {
        return m_lineBuffer->cntr_size(m_cntr);
    }

    void ringPoint(int n, double&x, double&y) const
    {
        m_lineBuffer->get_point(n, x, y);
    }

    bool ringClosed()
    {
        return m_lineBuffer->contour_closed(m_cntr);
    }
};


////////////////////////////////////////////////////////////////////////////////
// Provides utilities used in processing polygons
class PolygonUtils
{
public:
    enum SideOfLine
    {
        LeftOfLine,
        RightOfLine,
        OnTheLine,
        OppositeOnTheLine
    };

    typedef std::map<double, RingData*, std::greater<double> > SORTEDRINGS;
    typedef std::vector<RingData*> RINGVECTOR;

    STYLIZATION_API static void DetermineInteriorAndExteriorPolygons(LineBuffer* lineBuffer, SORTEDRINGS& rings);
    STYLIZATION_API static void Cleanup(SORTEDRINGS& rings);
    STYLIZATION_API static void AddLineBuffer(SORTEDRINGS& sortedRings, LineBuffer* lineBuffer);
    STYLIZATION_API static void ProcessRings(SORTEDRINGS& sortedRings);
    STYLIZATION_API static bool Contains(RingData* ringA, RingData* ringB);

    static SideOfLine LineSide(double fromPtX, double fromPtY, double toPtX, double toPtY, double ptX, double ptY);

private:
    static int WindingNumber(RingData* rd, double ptX, double ptY);
    static void AddRing(SORTEDRINGS& sortedRings, LineBuffer* lineBuffer, int cntr);
    static bool GetAreaAndBounds(LineBuffer* lineBuffer, int cntr, double& area, RS_Bounds& bounds);
};

#endif
