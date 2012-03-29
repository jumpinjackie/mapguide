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


//------------------------------------------------------------------------------
//
// FILE: LatLonBorderWalker.h.
//
// PURPOSE: Declaration of the LatLonBorderWalker class. The LatLonBorderWalker class
//          contains declarations and utility methods used in the construction
//          of point, polyline, and polygon buffer zones.
//
//------------------------------------------------------------------------------

#ifndef _LATLONBORDERWALKER_H_
#define _LATLONBORDERWALKER_H_

#include <vector>
#include <list>
using namespace std;

#include "BorderWalker.h"
#include "Spatial/MathUtility.h"
#include "FloatTransform.h"

class LatLonBorderWalker : public BorderWalker
{
public:
    /// constructor/destructor

    LatLonBorderWalker(FloatTransform *transform, MgCoordinateSystemMeasure *measure);
    virtual ~LatLonBorderWalker();

    virtual void ScanAndWalk(const OpsFloatPoint *fpVerts, int nNumVerts,
        vector<OpsFloatPoint*> &vBufferZones, vector<int> &vBufferZonesCount);

    virtual void CheckBorderCrossingVertices(const OpsFloatPoint *fpVerts, int nNumVerts);

    enum AzimuthType { PositiveAzimuth, NegativeAzimuth, SouthNorthAzimuth };
    enum WalkDirection { East, South, West, North };
    enum Corner { NE, NW, SW, SE };

    /// The linked vertex is an uni-directional linked-list vertex structure.
    struct LinkedVertex
    {
        int nInputVertIndex;
        const OpsFloatPoint *pfpVertex;
        LinkedVertex *plvNextVertex;
    };

    struct BorderPoint
    {
        bool operator < (const BorderPoint &bpBorderPt);
        bool operator > (const BorderPoint &bpBorderPt);
        bool operator == (const BorderPoint &bpBorderPt);
        bool operator != (const BorderPoint &bpBorderPt);

        OpsFloatPoint fpPt;
        WalkDirection wdDirection;
        LinkedVertex *lvLinkedVert;
    };

private:
    void InitLinkedVertices();
    void ScanVertices();
    void WalkBorder();
    void CreateBufferZones();

    AzimuthType GetLLAzimuthType(MgCoordinate* p0MCS, MgCoordinate* p1MCS);
    AzimuthType GetMCSAzimuthType(MgCoordinate* p0MCS, MgCoordinate* p1MCS);
    double GetMCSAbsAzimuth(MgCoordinate* p0MCS, MgCoordinate* p1MCS);
    bool CheckBorderCrossing(MgCoordinate* p0MCS, MgCoordinate* p1MCS);
    void AddCrossBorderPoints(int nP0Index, int nP1Index, AzimuthType azimuthTypeMCS);

    Ptr<MgCoordinateSystemMeasure> m_measure;
    double m_distance;
    FloatTransform *m_transform;
    Ptr<MgCoordinateSystem> m_cSys;
    OpsFloatPoint m_fpPtNE, m_fpPtNW, m_fpPtSW, m_fpPtSE;
    double m_dMaxX, m_dMaxY, m_dMinX, m_dMinY;
    double m_dWestMin, m_dEastMin;
    const OpsFloatPoint *m_fpInputVerts;
    int m_nNumInputVerts;
    vector<OpsFloatPoint*> *m_pvBufferZones;
    vector<int> *m_pvBufferZonesCount;
    list<BorderPoint> m_lstEastBorderPts;
    list<BorderPoint> m_lstWestBorderPts;

    /// Linked vertices array structure:
    /// [0] [1] [2] [3] [4] [5] [...] [N-2+4] [N-1+4] [N+4] [N+4+1] [N+4+...]
    /// PNE PNW PSW PSE P0  P1  ...     P(N-2)  P(N-1)  BP0   BP1     ...

    vector<LinkedVertex> m_vLinkedVerts;
    bool m_bLinkedVertsInitialized;
};

#endif // _LATLONBORDERWALKER_H_
