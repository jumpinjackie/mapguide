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

#ifndef POLYGON_UTILS_H
#define POLYGON_UTILS_H

#include "LineBuffer.h"

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
    double* m_ringPoints;
    int m_ringPointOffset;
    int m_ringPointCount;
    RingData* m_child;
};


////////////////////////////////////////////////////////////////////////////////
// Provides utilities used in processing polygons
class PolygonUtils
{
public:
    enum SideOfLine {LeftOfLine, RightOfLine, OnTheLine, OppositeOnTheLine};

    typedef std::map<double, RingData*, std::greater<double> > SORTEDRINGS;
    typedef std::vector<RingData*> RINGVECTOR;

    static void DetermineInteriorAndExteriorPolygons(LineBuffer* lineBuffer, SORTEDRINGS& rings);
    static void Cleanup(SORTEDRINGS& rings);
    static void AddLineBuffer(SORTEDRINGS& sortedRings, LineBuffer* lineBuffer);
    static void AddRing(SORTEDRINGS& sortedRings, double* points, int offset, int numPoints);
    static bool GetAreaAndBounds(double* points, int offset, int numPoints, double& area, RS_Bounds& bounds);
    static void ProcessRings(SORTEDRINGS& sortedRings);
    static bool Contains(RingData* ringA, RingData* ringB);

    static int WindingNumber(const double* vertices, int offset, int numPoints, double ptX, double ptY);
    static SideOfLine LineSide(double fromPtX, double fromPtY, double toPtX, double toPtY, double ptX, double ptY);
};

#endif //POLYGON_UTILS_H
