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

/// \ingroup Geometry_Module

//------------------------------------------------------------------------------
//
// FILE: bufrutil.h.
//
// PURPOSE: Declaration of the BufferUtility class. The BufferUtility class
//          contains declarations and utility methods used in the construction
//          of point, polyline, and polygon buffer zones.
//
//------------------------------------------------------------------------------

#ifndef _BUFRUTIL_H_
#define _BUFRUTIL_H_

#include <vector>
using namespace std;

class BufferUtility {
public:
    /// constructor/destructor

    BufferUtility(float fOffsetDist);
    BufferUtility(int nSegsPerCircle, float offsetDist);
    virtual ~BufferUtility();

    virtual void Initialize(int nSegsPerCircle);
    virtual void InitPointLine();
    virtual void InitPolyObject();

    virtual BOOL CreateOffsetChainLBL(BOOL bLBL) { return FALSE; }

    /// methods to query the segments per circle, and offset distance

    int GetNSegmentsPerCircle() const {return m_nSegmentsPerCircle;}
    float GetOffsetDistance() const {return m_offsetDist;}

    /// utility methods

    /// Used by point objects.
    virtual void CreatePointBuffer(const OpsFloatPoint &point,
        OrientedPolyPolygon &bufferZone);

    /// Used by poly-objects.
    virtual void CreateLineSegmentBuffer(const OpsFloatPoint endPoints[],
        OrientedPolyPolygon &bufferZone);
    virtual void CreateOffsetChains(const OpsFloatPoint vertices[],
        int nVertices, vector<OpsFloatPoint*> &vChainEdges, vector<int> &vChainEdgesCount);

    virtual void PolygonizeCircle(const OpsFloatPoint &center,
        OpsFloatPoint vertices[]) const;
    void PolygonizeCircularArc(const OpsFloatPoint &startPt,
        const OpsFloatPoint &endPt, const OpsFloatPoint &center,
        OpsFloatPoint vertices[], int &nVertices) const;
    void GetUnitOffsetVector(const OpsFloatPoint &endPt1,
        const OpsFloatPoint &endPt2, OpsDoublePoint &unitOffsetVector) const;
    void GetOffsetVector(const OpsFloatPoint &endPt1,
        const OpsFloatPoint &endPt2, OpsDoublePoint &offsetVector) const;

    enum SideOfLine {LeftOfLine, RightOfLine, OnTheLine, OppositeOnTheLine};

    static SideOfLine LineSide(const OpsDoublePoint &fromPt,
        const OpsDoublePoint &toPt, const OpsDoublePoint &pt);
    static int WindingNumber(const OpsFloatPoint vertices[], int nVertices,
        const OpsDoublePoint &point);
    static double DistFromPointToLineSeg(const OpsFloatPoint endPoints[],
        const OpsDoublePoint &point);
    static double DistBetweenPointsSqrd(const OpsDoublePoint &point1,
        const OpsDoublePoint &point2);
    static double DistBetweenPoints(const OpsDoublePoint &point1,
        const OpsDoublePoint &point2);
    static double DistBetweenPoints(const OpsFloatPoint &fltPoint,
        const OpsDoublePoint &dblPoint);

    static void GetExtentOfPoints(const OpsFloatPoint points[], int nPoints,
        OpsFloatExtent &extent);

    static double Max(double x, double y);
    static double Min(double x, double y);

    enum TurnType {LeftTurn, RightTurn, NoTurn, DoublesBack};
    static TurnType GetTurnType(const OpsFloatPoint &vertex1,
        const OpsFloatPoint &vertex2, const OpsFloatPoint &vertex3);

protected:
    void AddChainEdges(const OpsFloatPoint vertices[], int nVertices);
    void CreateConvexOffsetChains(const OpsFloatPoint vertices[],
        int nVertices);
    void GetHookEdge(const OpsFloatPoint &endPt1, const OpsFloatPoint &endPt2,
        BOOL toEndPt1, const OpsDoublePoint &offsetVector,
        OpsFloatPoint &hookEdgeEndPt) const;

    int m_nSegmentsPerCircle;
    OpsFloatPoint *m_pfBufferVerts;
    float m_offsetDist;
    double m_dDeltaThetaRad;
    double m_cosDeltaTheta;
    double m_sinDeltaTheta;
    double m_hookEdgeLength;
    double m_dHookEdgeLength;
    vector<OpsFloatPoint*> *m_vChainEdges;
    vector<int> *m_vChainEdgesCount;
    BOOL m_bCreatOffsetChainLBL;
    static const double TwoPi;
    static const double MinHookEdgeLength;
    static const int MaxEdgesPerChain;
};


//-----------------------------------------------------------------------------
//
// Implementation of inline functions
//
//-----------------------------------------------------------------------------

inline double BufferUtility::DistBetweenPoints(const OpsDoublePoint &point1,
    const OpsDoublePoint &point2)
{
    return ::sqrt(DistBetweenPointsSqrd(point1, point2));

} // end: DistBetweenPoints()


inline double BufferUtility::DistBetweenPoints(const OpsFloatPoint &fltPoint,
    const OpsDoublePoint &dblPoint)
{
    OpsDoublePoint fltToDbl(fltPoint.x, fltPoint.y);

    return DistBetweenPoints(fltToDbl, dblPoint);

} // end: DistBetweenPoints()


inline double BufferUtility::Max(double x, double y)
{
    return x >= y ? x : y;

} // end: Max()


inline double BufferUtility::Min(double x, double y)
{
    return x <= y ? x : y;

} // end: Min()

#endif
