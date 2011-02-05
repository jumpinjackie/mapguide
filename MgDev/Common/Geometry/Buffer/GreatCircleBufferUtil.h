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
// FILE: GreatCircleBufferUtil.h.
//
// PURPOSE: Declaration of the GreatCircleBufferUtil class. The GreatCircleBufferUtil
//          class contains declarations and utility methods used in the construction
//          of point, polyline, and polygon buffer zones in great circle mode.
//
//------------------------------------------------------------------------------

#ifndef _GREATCIRCLEBUFFERUTIL_H_
#define _GREATCIRCLEBUFFERUTIL_H_

#include "bufrutil.h"
#include "Measure.h"
#include "LatLonBorderWalker.h"
#include "BorderWalker.h"
#include "FloatTransform.h"

class GreatCircleBufferUtil : public BufferUtility
{
public:
    /// constructor/destructor

    GreatCircleBufferUtil(float fOffsetDistVDC,
        BorderWalker *pWalker, FloatTransform *transform);
    GreatCircleBufferUtil(INT32 nSegsPerCircle, float fOffsetDistVDC,
        FloatTransform *transform, BorderWalker *pWalker,
        MgCoordinateSystemMeasure* measure);
    virtual ~GreatCircleBufferUtil();

    virtual void Initialize(INT32 nSegsPerCircle);
    virtual void InitPolyObject();

    virtual BOOL CreateOffsetChainLBL(BOOL bLBL);

    /// utility methods

    /// Used by point objects.
    virtual void CreatePointBuffer(const OpsFloatPoint &point,
        OrientedPolyPolygon &bufferZone);

    /// Used by poly-objects.
    virtual void CreateLineSegmentBuffer(const OpsFloatPoint endPoints[],
        OrientedPolyPolygon &bufferZone);
    virtual void CreateOffsetChains(const OpsFloatPoint vertices[],
        INT32 nVertices, vector<OpsFloatPoint*> &vChainEdges, vector<int> &vChainEdgesCount);

    virtual void PolygonizeCircle(const OpsFloatPoint &center,
        OpsFloatPoint vertices[]) const;
    void PolygonizeCircularArc(const OpsFloatPoint &fP0,
        const OpsFloatPoint &fP1, const OpsFloatPoint &fP2,
        double dTheta1, double dTheta2,
        OpsFloatPoint vertices[], int &nVertices) const;

    SideOfLine LineSide(const OpsFloatPoint &fP0,
        const OpsFloatPoint &fP1, const OpsFloatPoint &fP);

    struct TurnPointInfo
    {
        double dNextAzimuth;
        double dPrevAzimuth;
        TurnType nextTurn;
        TurnType prevTurn;
    };

    TurnType GetTurnType(const OpsFloatPoint &fP0,
        const OpsFloatPoint &fP1, const OpsFloatPoint &fP);

    FloatTransform *GetFloatTransform() { return m_transform;};

protected:
    void AddChainEdgesAndCheck(const OpsFloatPoint vertices[], INT32 nVertices);
    void DoCreateOffsetChains(const OpsFloatPoint vertices[],
        INT32 nVertices);

private:

    /// methods to query the segments per circle, and offset distance

    void GetOffsetPoint(double dAzimuth,
        const OpsFloatPoint &fP0, OpsFloatPoint &fP) const;
    void GetOffsetPoint(double dAzimuth,
        const OpsFloatPoint &fP0, OpsFloatPoint &fP, double dOffsetDistance) const;
    double GetAzimuth(const OpsFloatPoint &fP0, const OpsFloatPoint &fP1);
    double GetDistance(const OpsFloatPoint &fP0, const OpsFloatPoint &fP1);
    inline double GetOffsetDistance() const { return m_offsetDist; }

    void CreateOffsetChainsLBL(const OpsFloatPoint vertices[], INT32 nVertices);
    void CreateConvexOffsetChains(const OpsFloatPoint vertices[],
        INT32 nVertices, const TurnPointInfo turnPtInfo[]);
    void GetHookPoint(const OpsFloatPoint &fP0,
        const OpsFloatPoint &fP1, bool bOffP0,
        double dOffsetAzimuth, OpsFloatPoint &fP);
    bool GetVerticesDirection(const OpsFloatPoint vertices[], INT32 nVertices);

    double m_dDeltaThetaDeg;
    double m_dOffsetDistM;
    double m_dOffsetDist;

    Ptr<MgCoordinateSystemMeasure> m_measure;
    FloatTransform *m_transform;
    BorderWalker *m_pBorderWalker;
};

#endif // _GREATCIRCLEBUFFERUTIL_H_
