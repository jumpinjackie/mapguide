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

#include "stdafx.h"
#include "PolygonUtils.h"
#include <assert.h>


//-------------------------------------------------------
// Overlapping geometry helpers
//-------------------------------------------------------


// Splits the supplied line buffer up into distinct polygons.
void PolygonUtils::DetermineInteriorAndExteriorPolygons(LineBuffer* lineBuffer, SORTEDRINGS& rings)
{
    if (lineBuffer == NULL)
        return;

    // build a list of all rings, sorted by area from largest to smallest
    AddLineBuffer(rings, lineBuffer);

    // process the rings
    PolygonUtils::ProcessRings(rings);
}


// Clean up a set of sorted rings
void PolygonUtils::Cleanup(SORTEDRINGS& rings)
{
    // cleanup
    for (SORTEDRINGS::iterator sIter = rings.begin(); sIter != rings.end(); ++sIter)
    {
        RingData* pRingData = sIter->second;
        delete pRingData;
    }
    rings.clear();
}


// Adds all rings in the supplied linebuffer to the list.
void PolygonUtils::AddLineBuffer(SORTEDRINGS& sortedRings, LineBuffer* lineBuffer)
{
    if (lineBuffer == NULL)
        return;

    for (int i=0; i<lineBuffer->cntr_count(); ++i)
    {
        AddRing(sortedRings, lineBuffer, i);
    }
}


// Adds the specified ring to the list.
void PolygonUtils::AddRing(SORTEDRINGS& sortedRings, LineBuffer* lineBuffer, int cntr)
{
    if (0 == lineBuffer->cntr_size(cntr))
        return;

    double area;
    RS_Bounds bounds;
    if (!PolygonUtils::GetAreaAndBounds(lineBuffer, cntr, area, bounds))
        return;

    RingData* pRingData = new RingData();
    pRingData->m_minX = bounds.minx;
    pRingData->m_minY = bounds.miny;
    pRingData->m_maxX = bounds.maxx;
    pRingData->m_maxY = bounds.maxy;
    pRingData->m_area = fabs(area);
    pRingData->m_type = RingData::Unknown;
    pRingData->m_lineBuffer = lineBuffer;
    pRingData->m_cntr = cntr;
    pRingData->m_child = NULL;

    // insert the ring (automatically sorted by decreasing area)
    sortedRings.insert(SORTEDRINGS::value_type(pRingData->m_area, pRingData));
}


// Returns the area and bounds of the supplied ring.
bool PolygonUtils::GetAreaAndBounds(LineBuffer* lineBuffer, int cntr, double& area, RS_Bounds& bounds)
{
    // get the first point in the ring
    if (lineBuffer->cntr_size(cntr) < 1)
        return false;
    int pointIndex = lineBuffer->contour_start_point(cntr);
    int last = lineBuffer->contour_end_point(cntr);
    double x0;
    double y0;
    double x1;
    double y1;
    lineBuffer->get_point(pointIndex++, x1, y1);
    double xF = x1;
    double yF = y1;
    double area2 = 0.0;

    // initialize bounds using first point
    bounds.maxx = bounds.minx = x1;
    bounds.maxy = bounds.miny = y1;

    while (pointIndex <= last)
    {
        x0 = x1;
        y0 = y1;
        lineBuffer->get_point(pointIndex++, x1, y1);

        // update bounds
        if (x1 < bounds.minx)
        {
            bounds.minx = x1;
        }
        if (x1 > bounds.maxx)
        {
            bounds.maxx = x1;
        }
        if (y1 < bounds.miny)
        {
            bounds.miny = y1;
        }
        if (y1 > bounds.maxy)
        {
            bounds.maxy = y1;
        }

        area2 += x0*y1 - y0*x1;
    }
    area2 += x1*yF - y1*xF;
    area = 0.5*area2;

    return true;
}


// Processes the list of rings.  Any rings contained inside another ring
// are flagged appropriately.  For faster performance, the rings are assumed not
// to intersect each other.
void PolygonUtils::ProcessRings(SORTEDRINGS& sortedRings)
{
    // TODO: if performance of this method becomes a problem consider
    //       using an R-Tree instead of a linear list.  But then the
    //       logic below will have to change...
    RINGVECTOR processedRings;

    // loop over the sorted list
    for (SORTEDRINGS::iterator sIter = sortedRings.begin(); sIter != sortedRings.end(); ++sIter)
    {
        // ring A is the one we're going to process
        RingData* pRingDataA = sIter->second;

        // We need to check if ring A is fully contained in any other ring.
        // This can only happen for rings which have bigger area than A.
        // Note that you can have nested rings, so we want to find the smallest
        // ring containing A.
        //
        // The linear list of processed rings contains all rings whose area
        // is larger than A, stored in descending area.  We therefore want to
        // loop backwards over this list to find the smallest ring containing
        // ring A.
        int numProcRings = (int)processedRings.size();
        for (int i=numProcRings-1; i>=0; --i)
        {
            // ring B is the one we're checking against
            RingData* pRingDataB = processedRings[i];

            // check if ring A is contained in ring B - we know the rings don't
            // cross and therefore we only need to check one point
            if (Contains(pRingDataB, pRingDataA))
            {
                // ring A is contained in B - determine what kind it is
                if (pRingDataB->m_type == RingData::Outer)
                {
                    // ring B is outer, which makes ring A an inner ring
                    pRingDataA->m_type = RingData::Inner;

                    // add ring A as a child of ring B
                    pRingDataA->m_child = pRingDataB->m_child;
                    pRingDataB->m_child = pRingDataA;
                }
                else
                {
                    // Ring B is inner, which effectively makes ring A an
                    // outer ring.  We flag it as such so that a separate
                    // feature gets created.
                    pRingDataA->m_type = RingData::Outer;
                }

                // don't check any more rings
                break;
            }
        }

        // if the ring type is still unknown that means it didn't intersect
        // any of the other (bigger) rings, and therefore it must be an outer
        // ring
        if (pRingDataA->m_type == RingData::Unknown)
            pRingDataA->m_type = RingData::Outer;

        // add the ring to the list
        processedRings.push_back(pRingDataA);
    }
}


// Determines if ringA is contained within ringB.
bool PolygonUtils::Contains(RingData* ringB, RingData* ringA)
{
    bool ringBContainsRingA = false;
    if (ringA != NULL && ringB != NULL &&
        ringA->ringPointCount() >= 4 &&
        ringB->ringPointCount() >= 4)
    {
        // check if ring A's extent intersects ring B's - this allows
        // us to quickly weed out non-intersecting rings
        bool intersects = (ringA->m_minX <= ringB->m_maxX &&
                           ringA->m_maxX >= ringB->m_minX &&
                           ringA->m_minY <= ringB->m_maxY &&
                           ringA->m_maxY >= ringB->m_minY);
        if (intersects)
        {
            // get the ring's start point
            double ringAPtX;
            double ringAPtY;
            ringA->ringPoint(ringA->ringStartPoint(), ringAPtX, ringAPtY);
            if (ringAPtX > ringB->m_minX && ringAPtX < ringB->m_maxX &&
                ringAPtY > ringB->m_minY && ringAPtY < ringB->m_maxY)
            {
                // determine if the start point of ringA is within ringB.
                // if this is the case, and since we assume non-intersecting
                // rings, it indicates that ringB contains ringA
                int windNumber = WindingNumber(ringB, ringAPtX, ringAPtY);
                ringBContainsRingA = (windNumber % 2 != 0);
            }
        }
    }

    return ringBContainsRingA;
}


// Determine the winding number of a point relative to a ring. An odd winding number
// indicates that the point lies within the ring.
int PolygonUtils::WindingNumber(RingData* rd, double ptX, double ptY)
{
    // the polygon boundary must be explicitly closed
    assert(rd->ringPointCount() >= 4);
    assert(rd->ringClosed());
    // loop through the polygon edges incrementing/decrementing the winding
    // number according to the following rules. If the horizontal ray drawn
    // from the point to +infinity crosses an edge at any point other than
    // its maximum in y, then increment or decrement the winding number
    // depending on whether the edge crosses the ray from below to above,
    // or above to below

    int windNumber = 0;

    int nEdges = rd->ringPointCount() - 1;
    int first = rd->ringStartPoint();
    for (int i=0; i<nEdges; ++i)
    {

        double endPt1X;
        double endPt1Y;
        double endPt2X;
        double endPt2Y;
        rd->ringPoint(first+i  , endPt1X, endPt1Y);
        rd->ringPoint(first+i+1, endPt2X, endPt2Y);
        double yMin = rs_min(endPt1Y, endPt2Y);

        if (yMin < ptY)
        {
            double yMax = rs_max(endPt1Y, endPt2Y);

            if (yMax >= ptY)
            {
                if (yMin == endPt1Y)
                {
                    if (LineSide(endPt1X, endPt1Y, endPt2X, endPt2Y, ptX, ptY) != LeftOfLine)
                    {
                        windNumber++;
                    }
                }
                else if (LineSide(endPt1X, endPt1Y, endPt2X, endPt2Y, ptX, ptY) != RightOfLine)
                {
                    windNumber--;
                }
            }
        }
    }
    return windNumber;
}


// Determine which side of the line defined by the from and to points the query
// point (ptX, ptY) falls upon.
PolygonUtils::SideOfLine PolygonUtils::LineSide(double fromPtX, double fromPtY,
                                                double toPtX, double toPtY,
                                                double ptX, double ptY)
{
    double signedCrossProd = (toPtX - fromPtX) * (  ptY - fromPtY) -
                             (  ptX - fromPtX) * (toPtY - fromPtY);

    if (signedCrossProd > 0.0)
        return LeftOfLine;
    else if (signedCrossProd < 0.0)
        return RightOfLine;
    else
        return OnTheLine;
}
