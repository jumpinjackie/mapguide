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
// FILE: GreatCircleBufferUtil.cpp.
//
// PURPOSE: Implementation of the GreatCircleBufferUtil class.
//
//------------------------------------------------------------------------------

#include "buffer.h"
#include "GreatCircleBufferUtil.h"
#include "../Spatial/SpatialUtilityCircularArc.h"

#include "performanceOptions.h"

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize a GreatCircleBufferUtil object.
//
// PARAMETERS:
//
//     Input:
//
//         fOffsetDistVDC    - passes the buffer offset distance in VDC.
//         dOffsetDistM        - passes the buffer offset distance in Meters.
//         pMgView            - passes a pointer to the MgView object.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

GreatCircleBufferUtil::GreatCircleBufferUtil(float fOffsetDistVDC,
    BorderWalker *pWalker, FloatTransform *transform) :
    BufferUtility(fOffsetDistVDC),
    m_dDeltaThetaDeg(0.0),
    m_dOffsetDistM(0.0),
    m_transform(transform),
    m_pBorderWalker(pWalker)
{
} // end: constructor

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize a GreatCircleBufferUtil object.
//
// PARAMETERS:
//
//     Input:
//
//         nSegsPerCircle    - passes the number of segments to use in the poly-
//                              gonization of a complete circle; for reasons pecu-
//                              liar to the generation of bufferzones, this number
//                            is best given as a multiple of 4 (if fact this cond-
//                            ition is enforced).
//         fOffsetDistVDC    - passes the buffer offset distance in VDC.
//         dOffsetDistM        - passes the buffer offset distance in Meters.
//         pMgView            - passes a pointer to the MgView object.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

GreatCircleBufferUtil::GreatCircleBufferUtil(INT32 nSegsPerCircle,
    float fOffsetDistVDC, FloatTransform *transform, BorderWalker *pWalker,
    MgCoordinateSystemMeasure* measure) :
    BufferUtility(nSegsPerCircle, fOffsetDistVDC),
    m_dDeltaThetaDeg(0.0),
    m_dOffsetDistM(0.0),
    m_transform(transform),
    m_pBorderWalker(pWalker)
{
    m_dOffsetDist = m_transform->Float2Double((float)fOffsetDistVDC);

    m_measure = SAFE_ADDREF(measure);
    Initialize(nSegsPerCircle);
} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Release all dynamically acquired resources, if any.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

GreatCircleBufferUtil::~GreatCircleBufferUtil()
{
} // end: destructor

//------------------------------------------------------------------------------
//
// METHOD: Initialize().
//
// PURPOSE: Pre-calculate some math variables for optimizations.
//
// PARAMETERS:
//
//     Input:
//
//         nSegsPerCircle    - passes the number of segments to use in the poly-
//                              gonization of a complete circle; for reasons pecu-
//                              liar to the generation of bufferzones, this number
//                            is best given as a multiple of 4 (if fact this cond-
//                            ition is enforced).
//     Output:
//
//           None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::Initialize(INT32 nSegsPerCircle)
{
    BufferUtility::Initialize(nSegsPerCircle);

    m_dDeltaThetaDeg = 360.0 / m_nSegmentsPerCircle;
} // end: Initialize()

//------------------------------------------------------------------------------
//
// METHOD: InitPolyObject().
//
// PURPOSE: Allocate buffer vertices storage space for poly-object primitives.
//
// PARAMETERS:
//
//     Input:
//
//         None.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::InitPolyObject()
{
    delete [] m_pfBufferVerts;

    // allocate the buffer used by CreateConvexOffsetChains() - note that the
    // buffer is given an "overrun" area at the end - this helps simplify the
    // logic in CreateConvexOffsetChains()

    m_pfBufferVerts = new OpsFloatPoint[MaxEdgesPerChain + 2 * m_nSegmentsPerCircle];

    // compute the hook edge length - the computed length is taken to be 1.5
    // times the maximum error resulting from polygonization of circular arcs

    assert(m_nSegmentsPerCircle >= 4);
    double hookLength = PERF_INCREASE_HOOKLENGTH_BY * 1.5 * m_offsetDist *
        (1.0 - ::cos(::acos(-1.0) / m_nSegmentsPerCircle));

    if (hookLength > MinHookEdgeLength)
        m_hookEdgeLength = hookLength;

    m_dHookEdgeLength = m_transform->Float2Double((float)m_hookEdgeLength);

} // end: InitPolyObject()

//------------------------------------------------------------------------------
//
// METHOD: CreateOffsetChainLBL().
//
// PURPOSE: Flag to use line-by-line algorithm while performing the
//            CreateOffsetChain function.
//
// PARAMETERS:
//
//     Input:
//
//         None.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL GreatCircleBufferUtil::CreateOffsetChainLBL(BOOL bLBL)
{
    m_bCreatOffsetChainLBL = bLBL;

    return TRUE;
}

//------------------------------------------------------------------------------
//
// METHOD: CreatePointBuffer().
//
// PURPOSE: Create a point buffer zone.
//
// PARAMETERS:
//
//     Input:
//
//         point      - passes a reference to an OpsFloatPoint object that
//                      contains the coordinates of the point around which the
//                      buffer zone is to be generated.
//
//     Output:
//
//         bufferZone - passes a reference to an OrientedPolyPolygon; the
//                      vertices of the point buffer are copied to the
//                      referenced object.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::CreatePointBuffer(const OpsFloatPoint &point,
    OrientedPolyPolygon &bufferZone)
{
    InitPointLine();

    PolygonizeCircle(point, m_pfBufferVerts);

    OpsFloatExtent bufferExtent;
    int nVertices = m_nSegmentsPerCircle + 1;

    if (m_pBorderWalker != NULL)
    {
        vector<OpsFloatPoint*> vBufferZones;
        vector<int> vBufferZonesCount;
        m_pBorderWalker->ScanAndWalk(m_pfBufferVerts,
            nVertices, vBufferZones, vBufferZonesCount);

        if (vBufferZones.empty())
        {
            GetExtentOfPoints(m_pfBufferVerts, nVertices, bufferExtent);

            bufferZone.AddBoundary(m_pfBufferVerts, nVertices, bufferExtent);
        }
        else
        {
            for (UINT32 i = 0; i < vBufferZones.size(); ++i)
            {
                GetExtentOfPoints(vBufferZones[i], vBufferZonesCount[i], bufferExtent);
                bufferZone.AddBoundary(vBufferZones[i], vBufferZonesCount[i], bufferExtent);

                delete [] vBufferZones[i];
                vBufferZones[i] = NULL;
            }
        }
    }
    else
    {
        GetExtentOfPoints(m_pfBufferVerts, nVertices, bufferExtent);

        bufferZone.AddBoundary(m_pfBufferVerts, nVertices, bufferExtent);
    }

} // end: CreatePointBuffer()

//------------------------------------------------------------------------------
//
// METHOD: CreateLineSegmentBuffer().
//
// PURPOSE: Create a line segment buffer zone.
//
//
//            <-  *   <-
//         /                \                   //other chars to avoid Linux warning
//
//     |                  ^
//     V                  |
//              eP[0]
//    P3 *<-------*-------->* P2
//         __        ^ ___  __
//         V3        | V10  V2
//       |        |          ^
//       V        | ___     |
//         __        | V01  __
//         V0        V       V1
//    P0 *<-------*-------->* P1
//              eP[1]
//       |                  ^
//     V                  |
//
//        \                /
//           ->   *   ->
//
// PARAMETERS:
//
//     Input:
//
//         endPoints  - passes a pointer to an array of two OpsFloatPoint
//                      objects, which specify the coordinates of the line
//                      segment endpoints.
//
//     Output:
//
//         bufferZone - passes a reference to an OrientedPolyPolygon; the
//                      vertices of the line segment buffer are copied to the
//                      referenced object.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::CreateLineSegmentBuffer(const OpsFloatPoint endPoints[],
    OrientedPolyPolygon &bufferZone)
{
    OpsFloatPoint fP0;
    OpsFloatPoint fP1;

    double dAzimuthV10 = GetAzimuth(endPoints[1], endPoints[0]);
    double dAzimuthV0 = dAzimuthV10 - 90.0;
    double dAzimuthV1 = dAzimuthV10 + 90.0;

    GetOffsetPoint(dAzimuthV0, endPoints[1], fP0);
    GetOffsetPoint(dAzimuthV1, endPoints[1], fP1);

    int nArcVertices1;
    PolygonizeCircularArc(endPoints[1], fP0, fP1,
        dAzimuthV0, dAzimuthV1, m_pfBufferVerts, nArcVertices1);

    OpsFloatPoint fP2, fP3;

    double dAzimuthV01 = GetAzimuth(endPoints[0], endPoints[1]);
    double dAzimuthV2 = dAzimuthV01 - 90.0;
    double dAzimuthV3 = dAzimuthV01 + 90.0;

    GetOffsetPoint(dAzimuthV2, endPoints[0], fP2);
    GetOffsetPoint(dAzimuthV3, endPoints[0], fP3);

    int nArcVertices2;
    PolygonizeCircularArc(endPoints[0], fP2, fP3,
        dAzimuthV2, dAzimuthV3, &m_pfBufferVerts[nArcVertices1], nArcVertices2);

    int nVertices = nArcVertices1 + nArcVertices2;
    m_pfBufferVerts[nVertices++] = m_pfBufferVerts[0];

    OpsFloatExtent bufferExtent;

    if (m_pBorderWalker != NULL)
    {
        vector<OpsFloatPoint*> vBufferZones;
        vector<int> vBufferZonesCount;
        m_pBorderWalker->ScanAndWalk(m_pfBufferVerts,
            nVertices, vBufferZones, vBufferZonesCount);

        if (vBufferZones.empty())
        {
            GetExtentOfPoints(m_pfBufferVerts, nVertices, bufferExtent);

            bufferZone.AddBoundary(m_pfBufferVerts, nVertices, bufferExtent);
        }
        else
        {
            for (UINT32 i = 0; i < vBufferZones.size(); ++i)
            {
                GetExtentOfPoints(vBufferZones[i], vBufferZonesCount[i], bufferExtent);
                bufferZone.AddBoundary(vBufferZones[i], vBufferZonesCount[i], bufferExtent);

                delete [] vBufferZones[i];
                vBufferZones[i] = NULL;
            }
        }
    }
    else
    {
        GetExtentOfPoints(m_pfBufferVerts, nVertices, bufferExtent);

        bufferZone.AddBoundary(m_pfBufferVerts, nVertices, bufferExtent);
    }
} // end: CreateLineSegmentBuffer()

//------------------------------------------------------------------------------
//
// METHOD: GetAzimuth().
//
// PURPOSE:
//                 North
//                   .
//                   .
//                   .     Azimuth (Angle with respect to the North)
//                   .----\                           //other chars to avoid Linux warning
//                   .    |
//                   .    V
//                P0 * ------------- * P1
//                   .
//                   .
//                   .
//                   .
//
//
// PARAMETERS:
//
//     Input:
//
//         fP0  - Passes a reference to an OpsFloatPoint containing P0.
//           fP1    - Passes a reference to an OpsFloatPoint containing P1.
//
//     Output:
//
//         None.
//
// RETURNS: Azimuth (Angle with respect to the North) formed by P0 and P1.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double GreatCircleBufferUtil::GetAzimuth(const OpsFloatPoint &fP0, const OpsFloatPoint &fP1)
{
    Ptr<MgCoordinate> startMCSCoord = m_transform->Float2Double(fP0);
    Ptr<MgCoordinate> endMCSCoord = m_transform->Float2Double(fP1);

    return m_measure->GetAzimuth(startMCSCoord->GetX(), startMCSCoord->GetY(), endMCSCoord->GetX(), endMCSCoord->GetY());
} // end: GetAzimuth()

//------------------------------------------------------------------------------
//
// METHOD: PolygonizeCircle().
//
// PURPOSE: Polygonize a circle centered at the specified location using the
//          number of segments per circle, and offset distance specified in
//          at construction time.
//
// PARAMETERS:
//
//     Input:
//
//         center    - passes a reference to an OpsFloatPoint containing the
//                     coordinates of the center of the circle.
//
//     Output:
//
//         vertices  - passes a pointer to an array OpsFloatPoint objects. On
//                     return, the array contains the nSegsPerCircle + 1
//                     vertices of the polygonized circle (the polygonized
//                     circle is explicitly closed).
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::PolygonizeCircle(const OpsFloatPoint &center,
    OpsFloatPoint vertices[]) const
{
    double degree = 360.0;
    INT32 i;
    for (i = 0; i < m_nSegmentsPerCircle; i++)
    {
        GetOffsetPoint(degree, center, vertices[i]);
        degree -= m_dDeltaThetaDeg;
    }
    vertices[i].x = vertices[0].x;
    vertices[i].y = vertices[0].y;

} // end: PolygonizeCircle()

//------------------------------------------------------------------------------
//
// METHOD: GetOffsetPoint().
//
// PURPOSE: Gets the offset point 'P' using the default offset distance.
//
//                 North
//                   .
//                   .
//                   .     Azimuth (Angle with respect to the North)
//                   .----\                   //other chars to avoid Linux warning
//                   .    |
//                   .    V
//                P0 * ---------------- * P
//                   .
//                   .
//                   .
//                   .
//
//
// PARAMETERS:
//
//     Input:
//
//         dAzimuth   - Azimuth (Angle with respect to the North) formed by 'P0' and 'P'.
//           fP0          - Passes a reference to an OpsFloatPoint containing 'P0'.
//
//     Output:
//
//         fP          - Passes a reference to an OpsFloatPoint containing 'P'.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::GetOffsetPoint(double dAzimuth,
    const OpsFloatPoint &fP0, OpsFloatPoint &fP) const
{
    GetOffsetPoint(dAzimuth, fP0, fP, m_offsetDist);
} // end: GetOffsetPoint()

//------------------------------------------------------------------------------
//
// METHOD: GetOffsetPoint().
//
// PURPOSE: Gets the offset point 'P' using the specified offset distance.
//
//                 North
//                   .
//                   .
//                   .     Azimuth (Angle with respect to the North)
//                   .----\               //other chars to avoid Linux warning
//                   .    |
//                   .    V
//                P0 * ---------------- * P
//                   . (OffsetDistance)
//                   .
//                   .
//                   .
//
//
// PARAMETERS:
//
//     Input:
//
//         dAzimuth            - Azimuth (Angle with respect to the North) formed by P0 and P.
//           fP0                - Passes a reference to an OpsFloatPoint containing 'P0'.
//           dOffsetDistance    - The offset distance in Meters.
//
//     Output:
//
//         fP          - Passes a reference to an OpsFloatPoint containing P.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::GetOffsetPoint(double dAzimuth,
    const OpsFloatPoint &fP0, OpsFloatPoint &fP, double dOffsetDistance) const
{
    Ptr<MgCoordinate> coordMCS = m_transform->Float2Double(fP0);

    double offset;

    if (PERF_USE_CACHED_OFFSET)
    {
        // Use the cached values
        if ( dOffsetDistance == m_offsetDist )
            offset = m_dOffsetDist;
        else if ( dOffsetDistance == m_hookEdgeLength )
            offset = m_dHookEdgeLength;
        else
            offset = m_transform->Float2Double((float)dOffsetDistance);
    }
    else
    {
        offset = m_transform->Float2Double((float)dOffsetDistance);
    }

    Ptr<MgCoordinate> returnCoord = m_measure->GetCoordinate(coordMCS, dAzimuth, offset);

    m_transform->Double2Float(returnCoord->GetX(), returnCoord->GetY(), fP);
} // end: GetOffsetPoint()

//------------------------------------------------------------------------------
//
// METHOD: GetDistance().
//
// PURPOSE: Gets the Great Circle distance between two VDC points.
//
// PARAMETERS:
//
//     Input:
//
//         fP0  - Passes a reference to an OpsFloatPoint containing the first point 'P0'.
//           fP1  - Passes a reference to an OpsFloatPoint containing the second point 'P1'.
//
//     Output:
//
//         None.
//
// RETURNS: Distance in Great Circle.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double GreatCircleBufferUtil::GetDistance(const OpsFloatPoint &fP0, const OpsFloatPoint &fP1)
{
    Ptr<MgCoordinate> startMCSCoord = m_transform->Float2Double(fP0);
    Ptr<MgCoordinate> endMCSCoord = m_transform->Float2Double(fP1);

    return m_measure->GetDistance(startMCSCoord->GetX(), startMCSCoord->GetY(), endMCSCoord->GetX(), endMCSCoord->GetY());
} // end: GetDistance()

//------------------------------------------------------------------------------
//
// METHOD: PolygonizeCircularArc().
//
// PURPOSE: Generate the polygonal approximation to the specified circular arc.
//          The arc is assumed to be specified in a counter-clockwise fashion.
//
//
//                  North
//                    .
//                    .
//        theta1      .     theta2
//              /-----.------\            //other chars to avoid Linux warning
//             |        .      |
//             V        .P0    V
//    P1 * ---------- * ------------ * P2
//                    |     |
//                    |     /
//                    |<---/
//                    |        delta theta
//                    |
//                    |
//                    *
//                    P
//
// PARAMETERS:
//
//     Input:
//
//         fP0       - Passes a reference to an OpsFloatPoint containing the
//                     center 'P0' of the circle the arc is embedded in.
//         fP1       - Passes a reference to an OpsFloatPoint containing the
//                     coordinates of the starting point 'P1' of the circular arc.
//         fP2       - Passes a reference to an OpsFloatPoint containing the
//                     coordinates of the ending point 'P2' of the circular arc.
//           dTheta1     - Passes the angle theta1 formed by 'P0' and 'P1'.
//           dTheta2   - Passes the angle theta2 formed by 'P0' and 'P2'.
//
//     Output:
//
//         vertices  - Passes a pointer to an array OpsFloatPoint object. On
//                     return, the array contains the vertices of the polygonal
//                     approximation to the circular arc. The array is assumed
//                     to be large enough to hold the vertices.
//         nVertices - Passes a reference to an int. The number of vertices is
//                     copied to the referenced int.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::PolygonizeCircularArc(const OpsFloatPoint &fP0,
    const OpsFloatPoint &fP1, const OpsFloatPoint &fP2,
    double dTheta1, double dTheta2,
    OpsFloatPoint vertices[], int &nVertices) const
{
    // normalize to range 0.0 to 360.0

    if (dTheta1 < 0.0)
        dTheta1 += 360.0;

    if (dTheta2 < 0.0)
        dTheta2 += 360.0;

    // make sure Theta1 is greater than Theta2

    if (dTheta1 < dTheta2)
        dTheta1 += 360.0;

    // compute the angle, Phi, subtended by the arc
    //
    //                  North
    //                    .------------\              //other chars to avoid Linux warning
    //                    .             |
    //                    .    theta2  |
    //                    .---\         |
    //                      .   |         |
    //                    P0.   |         |
    //    P1 * ---------- * ..|............
    //                    | \ V  |     |
    //                    |  \  / delta theta
    //                    |<---/        /
    //                    |     \       /
    //                    |<----\---/     Phi
    //                    |      \                    //other chars to avoid Linux warning
    //                    *       *
    //                    P         P2

    double dDeltaThetaDeg = m_dDeltaThetaDeg;
    double dPhi = ::ceil(dTheta2 / dDeltaThetaDeg) * dDeltaThetaDeg;

    // make sure we don't redundantly compute the start point

    if (dPhi == dTheta2)
        dPhi += dDeltaThetaDeg;

    nVertices = 1;

    // Pre-calculate the number of points for this arc segment
    double dTempPhi = dPhi;

    while (dTempPhi < dTheta1)
    {
        ++nVertices;
        dTempPhi += dDeltaThetaDeg;
    }
    ++nVertices;

    // Polygonize the arc (else
    // a single edge from startPt to endPt is all that is required)
    vertices[nVertices - 1] = fP2;

    for (INT32 i = (nVertices - 2); i > 0; --i)
    {
        GetOffsetPoint(dPhi, fP0, vertices[i]);
        if ( i != (nVertices - 2) )
        {
            // Vertical or horizontal spike? collapse.
            if ( vertices[i].x == vertices[i+1].x || vertices[i].y == vertices[i+1].y)
                vertices[i] = vertices[i+1];
        }
        dPhi += dDeltaThetaDeg;
    }

    vertices[0] = fP1;

    // Eliminate spikes by collapsing the point in the middle
    if ( PERF_CORRECT_SPIKES2 )
    {
        for (INT32 i = 0; i < (nVertices - 2); i++)
        {
            if ( vertices[i] == vertices[i+2] )
            {
                vertices[i+1] = vertices[i];
            }
        }

        for (INT32 i = 0; i < (nVertices - 1); i++)
        {
            for (INT32 j = i + 1; j < nVertices; j++)
            {
                if ( vertices[i] == vertices[j] )
                {
                    for ( INT32 k = i + 1; k < j; k++ )
                        vertices[k] = vertices[i];
                    break; // Assume there is just one
                }
            }
        }
    }

    assert(nVertices <= m_nSegmentsPerCircle);

} // end: PolygonizeCircularArc()


//------------------------------------------------------------------------------
//
// METHOD: LineSide().
//
// PURPOSE: Determine which side of a directed line 'V1' a specified point 'P' lies on.
//
//                     North
//                       .
//                       .       * P
//                       .ThetaV ^_
//                       .----->/ V
//                       .     /
//                  -----.    /
//        ThetaV1     /       .   /
//                |       .  /
//                V       . /
//    P1 * ------------- *
//          __                . P0
//        V1               .
//                       .
//                       .
//
// PARAMETERS:
//
//     Input:
//
//         fP1 - Passes a reference to an OpsFloatPoint containing 'P1'.
//         fP0 - Passes a reference to an OpsFloatPoint containing 'P0'.
//         fP  - Passes a reference to an OpsFloatPoint containing 'P'.
//
//     Output:
//
//         None.
//
// RETURNS: An enumerated value of type BufferUtility::SideOfLine specifying the
//          side of the directed line 'P' lies to.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BufferUtility::SideOfLine GreatCircleBufferUtil::LineSide(const OpsFloatPoint &fP1,
    const OpsFloatPoint &fP0, const OpsFloatPoint &fP)
{
    double dThetaV1 = GetAzimuth(fP0, fP1);
    double dThetaV = GetAzimuth(fP0, fP);

    if (dThetaV1 < 0.0)
        dThetaV1 += 360.0;

    if (dThetaV < 0.0)
        dThetaV += 360.0;

    double dThetaDiff = dThetaV - dThetaV1;

    if (dThetaDiff < 0.0)
        dThetaDiff += 360.0;

    if (dThetaDiff < 180.0)
        return LeftOfLine;
    else if (dThetaDiff > 180.0)
        return RightOfLine;
    else
        return OnTheLine;
} // end: LineSide()

//------------------------------------------------------------------------------
//
// METHOD: GetTurnType().
//
// PURPOSE: Determine the turn direction between two adjacent line segments at
//          their common vertex.
//
// PARAMETERS:
//
//     Input:
//
//         fP1 - passes a reference to the OpsFloatPoint containing the
//                   coordinates of the first vertex of the first line segment.
//         fP0 - passes a reference to the OpsFloatPoint containing the
//                   shared vertex (the second vertex of the first line segment,
//                   and the first vertex of the second line segment).
//         fP2 - passes a reference to the OpsFloatPoint containing the
//                   coordinates of the second vertex of the second line
//                   segment.
//
//     Output:
//
//         None.
//
// RETURNS: An enumerated value of type TurnType specifying the direction
//          of turn at fP0.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BufferUtility::TurnType GreatCircleBufferUtil::GetTurnType(const OpsFloatPoint &fP0,
    const OpsFloatPoint &fP1, const OpsFloatPoint &fP2)
{
    // test which side of the directed line fP0->fP1 that fP2
    // lies on

    SideOfLine side = LineSide(fP0, fP1, fP2);

    // if fP2 is left/right of the line then a left/right turn is made at
    // fP0

    if (side == BufferUtility::LeftOfLine)
        return LeftTurn;
    else if (side == BufferUtility::RightOfLine)
        return RightTurn;
    else if (side == BufferUtility::OppositeOnTheLine)
        return DoublesBack;

    // else either no turn has been made, or the second line doubles back over
    // the first

    else
    {
        double dDistV1 = GetDistance(fP0, fP1);
        double dDistV = GetDistance(fP0, fP2);

        return (dDistV < dDistV1 ? DoublesBack : NoTurn);
    }

} // end: GetTurnType()

//------------------------------------------------------------------------------
//
// METHOD: CreateOffsetChains().
//
// PURPOSE: Compute a set of "convex offset chains" around the vertices of the
//          specified poly-object (a polyline or polygon boundary). The input
//          poly-object is scanned for adjacent segments that double back on
//          each other, since these are not handled directly by the Create-
//          ConvexOffsetChains() method.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes the array of poly-object vertices. Note that the
//                     array of vertices is assumed to be free of redundancies;
//                     that is, no two adjacent vertices are identical.
//         nVertices - passes the number of vertices.
//
//     Output:
//
//           vChainEdges        - passes a reference to the vector<OpsFloatPoint*>
//                              containing the set of offset chain edges.
//           vChainEdgesCount    - passes a reference to the vector<int> containing
//                              the set of offset chain edges count.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::CreateOffsetChains(const OpsFloatPoint vertices[],
    INT32 nVertices, vector<OpsFloatPoint*> &vChainEdges, vector<int> &vChainEdgesCount)
{
    m_vChainEdges = &vChainEdges;
    m_vChainEdgesCount = &vChainEdgesCount;

    // ideally, we would like to warn in debug mode of degenerate conditions
    // but for some of the data from older SDFs, the assert could get fired
    // many many times

//    assert(nVertices >= 2);

    // the normal case is a non-degenerate polyobject (i.e., more than one
    // vertex)

    if (nVertices > 1)
    {
        if (m_bCreatOffsetChainLBL)
        {
            CreateOffsetChainsLBL(vertices, nVertices);

            return;
        }

        try
        {
            DoCreateOffsetChains(vertices, nVertices);
        }
        catch (BorderWalkerException* e)
        {
            if (BorderWalkerException::VerticesCrossingBorderError != e->GetErrorCode())
            {
                assert(false);
            }

            CreateOffsetChainsLBL(vertices, nVertices);
        }
    }

    // else if there is only a single vertex (this may have resulted from
    // culling redundant points), then treat it as a point

    else if (nVertices == 1)
    {
        PolygonizeCircle(vertices[0], m_pfBufferVerts);

        // TODO: compress this structure

        if (m_pBorderWalker != NULL)
        {
            vector<OpsFloatPoint*> vBufferZones;
            vector<int> vBufferZonesCount;
            m_pBorderWalker->ScanAndWalk(m_pfBufferVerts,
                m_nSegmentsPerCircle + 1, vBufferZones, vBufferZonesCount);

            if (vBufferZones.empty())
            {
                AddChainEdges(m_pfBufferVerts, m_nSegmentsPerCircle + 1);
            }
            else
            {
                for (UINT32 i = 0; i < vBufferZones.size(); ++i)
                {
                    AddChainEdges(vBufferZones[i], vBufferZonesCount[i]);

                    delete [] vBufferZones[i];
                    vBufferZones[i] = NULL;
                }
            }
        }
        else
        {
            AddChainEdges(m_pfBufferVerts, m_nSegmentsPerCircle + 1);
        }
    }
}

//------------------------------------------------------------------------------
//
// METHOD: DoCreateOffsetChains().
//
// PURPOSE: Compute a set of "convex offset chains" around the vertices of the
//          specified poly-object (a polyline or polygon boundary). The input
//          poly-object is scanned for adjacent segments that double back on
//          each other, since these are not handled directly by the Create-
//          ConvexOffsetChains() method.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes the array of poly-object vertices. Note that the
//                     array of vertices is assumed to be free of redundancies;
//                     that is, no two adjacent vertices are identical.
//         nVertices - passes the number of vertices.
//
//     Output:
//
//           vChainEdges        - passes a reference to the vector<OpsFloatPoint*>
//                              containing the set of offset chain edges.
//           vChainEdgesCount    - passes a reference to the vector<int> containing
//                              the set of offset chain edges count.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::DoCreateOffsetChains(const OpsFloatPoint vertices[],
    INT32 nVertices)
{
    GreatCircleBufferUtil::TurnPointInfo *turnPtInfo =
        new GreatCircleBufferUtil::TurnPointInfo [nVertices];

    // Calculate the first point turning information.

    turnPtInfo[0].dNextAzimuth = GetAzimuth(vertices[0], vertices[1]);
    turnPtInfo[0].dPrevAzimuth = 0.0;
    turnPtInfo[0].nextTurn = BufferUtility::NoTurn;
    turnPtInfo[0].prevTurn = BufferUtility::NoTurn;

    // Calculate the last point turning information.

    turnPtInfo[nVertices - 1].dNextAzimuth = 0.0;
    turnPtInfo[nVertices - 1].dPrevAzimuth =
        GetAzimuth(vertices[nVertices - 1], vertices[nVertices - 2]);
    turnPtInfo[nVertices - 1].nextTurn = BufferUtility::NoTurn;
    turnPtInfo[nVertices - 1].prevTurn = BufferUtility::NoTurn;

    for (INT32 i = 0, j, k; i < nVertices - 1; i = j + 1)
    {
        // scan the line for adjacent segments that double back on each
        // other (need to prevent this type of polyobject from being passed
        // to CreateConvexOffsetChains() since it creates problems deciding
        // how to compute the offset chains)

        for (j = i, k = j + 1; j < nVertices - 2; j++, k = j + 1)
        {
            turnPtInfo[k].dNextAzimuth = GetAzimuth(vertices[j + 1], vertices[j + 2]);
            turnPtInfo[k].dPrevAzimuth = GetAzimuth(vertices[j + 1], vertices[j]);
            turnPtInfo[k].nextTurn =
                GetTurnType(vertices[j], vertices[j + 1], vertices[j + 2]);
            turnPtInfo[k].prevTurn =
                GetTurnType(vertices[j + 2], vertices[j + 1], vertices[j]);

            if (turnPtInfo[k].nextTurn == BufferUtility::DoublesBack)
            {
                break;
            }
        }

        try
        {
            // create the left-convex offset chains

            CreateConvexOffsetChains(&vertices[i], j + 2 - i, turnPtInfo);
        }
        catch(...)
        {
            for (UINT32 h = 0; h < m_vChainEdges->size(); ++h)
            {
                delete [] (*m_vChainEdges)[h];
            }

            m_vChainEdges->clear();
            m_vChainEdgesCount->clear();

            delete [] turnPtInfo;
            turnPtInfo = NULL;

            throw;
        }
    }

    delete [] turnPtInfo;
    turnPtInfo = NULL;


} // CreateOffsetChains

//------------------------------------------------------------------------------
//
// METHOD: CreateOffsetChainsLBL().
//
// PURPOSE: Compute a set of "offset chains" around the vertices of the
//          specified poly-object (a polyline or polygon boundary) in a line-by-line
//            fashion.  Each "offset chain" will be created using a similar
//            CreateLineSegmentBuffer() function.
//
//            <-  *   <-
//         /                \                   //other chars to avoid Linux warning
//
//     |                  ^
//     V                  |
//              eP[0]
//    P3 *<-------*-------->* P2
//         __        ^ ___  __
//         V3        | V10  V2
//       |        |          ^
//       V        | ___     |
//         __        | V01  __
//         V0        V       V1
//    P0 *<-------*-------->* P1
//              eP[1]
//       |                  ^
//     V                  |
//
//        \                /
//           ->   *   ->
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes the array of poly-object vertices. Note that the
//                     array of vertices is assumed to be free of redundancies;
//                     that is, no two adjacent vertices are identical.
//         nVertices - passes the number of vertices.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::CreateOffsetChainsLBL(const OpsFloatPoint vertices[], INT32 nVertices)
{
    OpsFloatPoint fP0, fP1, fP2, fP3;
    double dAzimuthV10, dAzimuthV0, dAzimuthV1;
    double dAzimuthV01, dAzimuthV2, dAzimuthV3;
    int nTotalArcVertices, nArcVertices1, nArcVertices2;

    for (INT32 i = 0; i < (nVertices - 1); ++i)
    {
        dAzimuthV10 = GetAzimuth(vertices[i + 1], vertices[i]);
        dAzimuthV0 = dAzimuthV10 - 90.0;
        dAzimuthV1 = dAzimuthV10 + 90.0;

        GetOffsetPoint(dAzimuthV0, vertices[i + 1], fP0);
        GetOffsetPoint(dAzimuthV1, vertices[i + 1], fP1);

        PolygonizeCircularArc(vertices[i + 1], fP0, fP1,
            dAzimuthV0, dAzimuthV1, &m_pfBufferVerts[0], nArcVertices1);

        dAzimuthV01 = GetAzimuth(vertices[i], vertices[i + 1]);
        dAzimuthV2 = dAzimuthV01 - 90.0;
        dAzimuthV3 = dAzimuthV01 + 90.0;

        GetOffsetPoint(dAzimuthV2, vertices[i], fP2);
        GetOffsetPoint(dAzimuthV3, vertices[i], fP3);

        PolygonizeCircularArc(vertices[i], fP2, fP3,
            dAzimuthV2, dAzimuthV3, &m_pfBufferVerts[nArcVertices1], nArcVertices2);

        nTotalArcVertices = nArcVertices1 + nArcVertices2;

        m_pfBufferVerts[nTotalArcVertices++] = m_pfBufferVerts[0];

        if (m_pBorderWalker != NULL)
        {
            vector<OpsFloatPoint*> vBufferZones;
            vector<int> vBufferZonesCount;
            m_pBorderWalker->ScanAndWalk(m_pfBufferVerts,
                nTotalArcVertices, vBufferZones, vBufferZonesCount);

            if (vBufferZones.empty())
            {
                AddChainEdges(m_pfBufferVerts, nTotalArcVertices);
            }
            else
            {
                for (UINT32 i = 0; i < vBufferZones.size(); ++i)
                {
                    AddChainEdges(vBufferZones[i], vBufferZonesCount[i]);

                    delete [] vBufferZones[i];
                    vBufferZones[i] = NULL;
                }
            }
        }
        else
        {
            AddChainEdges(m_pfBufferVerts, nTotalArcVertices);
        }
    }

} // end: CreateOffsetChainsLBL()

//------------------------------------------------------------------------------
//
// METHOD: GetVerticesDirection().
//
// PURPOSE: Compute the verticies orientation (CCW or CW)
//
// PARAMETERS:
//
//     Input:
//
//         vertices   - passes the array of poly-object vertices. Note that the
//                      array of vertices is assumed to be free of redundancies;
//                      that is, no two adjacent vertices are identical.
//         nVertices  - passes the number of vertices.
//
//     Output:
//
//         None.
//
// RETURNS: true if CCW, false if CW.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

bool GreatCircleBufferUtil::GetVerticesDirection(const OpsFloatPoint vertices[], INT32 nVertices1)
{
    // Find the vertex with maxy

    double maxy = vertices[0].y;
    int    maxIndex = 0;

    INT32 nVertices = PERF_ADD_START_POINT_TO_LOOP? nVertices1 - 1 : nVertices1;

    for (int i = 1; i < nVertices; i++ )
    {
        if ( vertices[i].y > maxy)
        {
            maxy = vertices[i].y;
            maxIndex = i;
        }
    }

    OpsFloatPoint ptMax = vertices[maxIndex];

    bool ccw = false;

    // Compute left and right angles.
    int next = (maxIndex < nVertices - 2) ? maxIndex + 1 : 0;
    OpsFloatPoint pt1 = vertices[next];

    int prev = (maxIndex > 0) ? maxIndex - 1 : nVertices - 2; // point 0 == nVertices - 1
    OpsFloatPoint pt2 = vertices[prev];

    double theta1 = ::atan2(pt1.x - ptMax.x, pt1.y - ptMax.y);
    double theta2 = ::atan2(pt2.x - ptMax.x, pt2.y - ptMax.y);

    if ( theta1 < 0 )
        theta1 += 2 * M_PI;
    if ( theta2 < 0 )
        theta2 += 2 * M_PI;

    ccw = ( theta1 > theta2 );

    return ccw;
}

//------------------------------------------------------------------------------
//
// METHOD: CreateConvexOffsetChains().
//
// PURPOSE: Compute a set of "convex offset chains" around the vertices of the
//          specified poly-object (a polyline or polygon boundary). The edges in
//          the offset chains are added to the plane sweep process, from which
//          the buffer zone polygon surrounding the poly-object will later be
//          extracted.
//
// PARAMETERS:
//
//     Input:
//
//         vertices   - passes the array of poly-object vertices. Note that the
//                      array of vertices is assumed to be free of redundancies;
//                      that is, no two adjacent vertices are identical.
//         nVertices  - passes the number of vertices.
//           turnPtInfo - passes the array of TurnPointInfo structure containing
//                        the pre-calculated azimuths and turning directions for
//                        all vertices.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::CreateConvexOffsetChains(const OpsFloatPoint vertices[],
    INT32 nVertices, const TurnPointInfo turnPtInfo[])
{
    // walk forward along the sequence of polyobject vertices, generating convex
    // offset chains to the right

    int i, j, k;
    int nArcVertices = 0;
    OpsFloatPoint fP1, fP2;
    double dTheta1, dTheta2;



    bool    isCCW = false;
    bool    traceBothSides = PERF_TRACE_BOTH_SIDES;
    bool    addedStartPointToLoop = PERF_ADD_START_POINT_TO_LOOP;

    // In the case of other geometries than polygons, we trace both sides
    // Assume here that first point == last point for polygons.

    if (vertices[0] != vertices[nVertices - 2] )
    {
        traceBothSides = true;
        addedStartPointToLoop = false;
    }

    // When not tracing on both sides we need the CCW vertices direction.
    if ( !traceBothSides )
        isCCW = GetVerticesDirection(vertices, nVertices);

    if ( m_offsetDist < 0 )
    {
        isCCW = !isCCW;
        if ( m_dOffsetDist < 0.0 ) // Because we use cached values.
            m_dOffsetDist = -m_dOffsetDist;
    }

    if ( traceBothSides || isCCW)
    {
        dTheta1 = turnPtInfo[0].dNextAzimuth + 90.0;
        dTheta2 = 0.0;

        GetHookPoint(vertices[0], vertices[1], TRUE, dTheta1, m_pfBufferVerts[0]);

        GetOffsetPoint(dTheta1, vertices[0], m_pfBufferVerts[1]);

        for (i = 0, j = 2, k = 1; i < nVertices - 2; i++, k = i + 1)
        {
            // determine which way the line turns at the next vertex

            // if a left turn is made, then polygonize the arc centered on the
            // vertex where the turn is made, and continue generating the convex
            // offset chain

            //                  North
            //                       .
            //                    .
            //       *            * vertices[i]
            //                    |
            //       |            |
            //       V            |
            //                    |
            //        theta1      |     theta2
            //              /-----|-------\               //other chars to avoid Linux warning
            //             |        |       |
            //             V vertices[i+1]|        vertices[i+2]
            //    P1 * ---------- * ------|----------- *
            //                    |       |
            //       |            |       /
            //       V            |<-----/
            //                    |
            //        \            |
            //                    |
            //           ->        *        ->        ->     *
            //                    P2

            if (turnPtInfo[k].nextTurn == BufferUtility::LeftTurn)
            {
                dTheta1 = turnPtInfo[k].dPrevAzimuth - 90.0;
                GetOffsetPoint(dTheta1, vertices[i + 1], fP1);
                dTheta2 = turnPtInfo[k].dNextAzimuth + 90.0;
                GetOffsetPoint(dTheta2, vertices[i + 1], fP2);
                PolygonizeCircularArc(vertices[i + 1], fP1, fP2,
                    dTheta1, dTheta2, &m_pfBufferVerts[j], nArcVertices);
                j += nArcVertices;
            }

            // else if a right turn was made, drop a hook edge, and advance to
            // the next segment

            //                  North
            //                       .
            //                    .
            //                    .------\  theta1
            //                    .        \                   //other chars to avoid Linux warning
            //                    .        |
            //                    .        V               m_pfBufferVerts[j]
            //     vertices[i+1]    * ----------*  <-  <-  *------- * vertices[i+2]
            //                       | m_pfBufferVerts[j+1]
            //                       |     (Hook Point)       ^
            //                    |                       |
            //                    |
            //                    |
            //                    |
            //                       |                       ^
            //                    |                       |
            //                    |
            //                    |
            //       vertices[i]  *                       *



            else if (turnPtInfo[k].nextTurn == BufferUtility::RightTurn)
            {
                dTheta1 = turnPtInfo[k].dPrevAzimuth - 90.0;
                GetOffsetPoint(dTheta1, vertices[i + 1], m_pfBufferVerts[j++]);
                GetHookPoint(vertices[i], vertices[i + 1],
                    FALSE, dTheta1, m_pfBufferVerts[j++]);
            }

            // if a right turn was made, or the offset chain has grown too large
            // then add the edges in the offset chain, and start a new chain

            //                      North
            //                           .
            //                        .
            //                        .-----\  theta1
            //                        .       \            //other chars to avoid Linux warning
            //                        .       |
            //                        .       |
            //     vertices[i+1]        * -----|--------------- * vertices[i+2]
            //                           |       |
            //                        |<-----/
            //                        |
            //                        |
            //  m_pfBufferVerts[0]    *
            //
            //                        |
            //                        V
            //
            //  m_pfBufferVerts[1] *   ->        ->        ->  *
            //                        |
            //                        |
            //                        |
            //                        |
            //       vertices[i]        *

            if ((turnPtInfo[k].nextTurn == BufferUtility::RightTurn) || (j >= MaxEdgesPerChain))
            {
                // Add the edges in the offset chain and check for border conditions.
                if ( j != 2 ) // Avoid just the hook point
                    AddChainEdgesAndCheck(m_pfBufferVerts, j);

                // Start a new chain.
                dTheta1 = turnPtInfo[k].dNextAzimuth + 90.0;
                GetHookPoint(vertices[i + 1], vertices[i + 2],
                    TRUE, dTheta1, m_pfBufferVerts[0]);
                GetOffsetPoint(dTheta1, vertices[i + 1],
                    m_pfBufferVerts[1]);
                j = 2;
            }
        }

        // add semicircular cap centered at the last vertex of the input polyobject

        //                      North
        //                           .
        //                        .
        //                <-        *        <-
        //                        |
        //          /                |                ^
        //         V                |                 \          //other chars to avoid Linux warning
        //                        |
        //            theta2      |     theta1
        //       |          /-----|-------\            ^
        //       V         |        |       |            |
        //                 V        |       V
        //              vertices[nVertices-1]
        //    P2 * -------------- * ----------------- * P1
        //                        |
        //       |                |                    ^
        //       V                |                    |
        //                        |
        //                        |
        //       |                |                    ^
        //       V                 *                    |
        //              vertices[nVertices-2]

        if ( !addedStartPointToLoop )
        {
            dTheta1 = turnPtInfo[nVertices - 1].dPrevAzimuth - 90.0;
            GetOffsetPoint(dTheta1, vertices[nVertices - 1], fP1);
            dTheta2 = turnPtInfo[k].dPrevAzimuth + 90.0;

            GetOffsetPoint(dTheta2, vertices[nVertices - 1], fP2);
            PolygonizeCircularArc(vertices[nVertices - 1], fP1, fP2,
                dTheta1, dTheta2, &m_pfBufferVerts[j], nArcVertices);

            AddChainEdgesAndCheck(m_pfBufferVerts, j + nArcVertices);
        }
        else if ( j != 2 ) //Avoid just the hook point
        {
            AddChainEdgesAndCheck(m_pfBufferVerts, j);
        }
    }

    // now walk backwards along the poly-object vertices generating offset
    // chains on the other side

    if (traceBothSides || !isCCW)
    {
        dTheta1 = turnPtInfo[nVertices - 1].dPrevAzimuth + 90.0;
        GetHookPoint(vertices[nVertices - 1], vertices[nVertices - 2],
            TRUE, dTheta1, m_pfBufferVerts[0]);
        GetOffsetPoint(dTheta1, vertices[nVertices - 1], m_pfBufferVerts[1]);

        for (i = nVertices - 1, j = 2, k = i - 1; i > 1; i--, k = i - 1)
        {
            // determine which way the line turns at the next vertex



            // if a left turn is made, then polygonize the arc centered on the
            // vertex where the turn is made, and continue generating the convex
            // offset chain

            //                  North
            //                       .
            //                    .
            //       *            * vertices[i]
            //                    |
            //       |            |
            //       V            |
            //                    |
            //        theta1      |     theta2
            //              /-----|-------\               //other chars to avoid Linux warning
            //             |        |       |
            //             V vertices[i-1]|        vertices[i-2]
            //    P1 * ---------- * ------|----------- *
            //                    |       |
            //       |            |       /
            //       V            |<-----/
            //                    |
            //        \            |
            //                    |
            //           ->        *        ->        ->     *
            //                    P2

            if (turnPtInfo[k].prevTurn == BufferUtility::LeftTurn)
            {
                dTheta1 = turnPtInfo[k].dNextAzimuth - 90.0;
                GetOffsetPoint(dTheta1, vertices[i - 1], fP1);
                dTheta2 = turnPtInfo[k].dPrevAzimuth + 90.0;
                GetOffsetPoint(dTheta2, vertices[i - 1], fP2);
                PolygonizeCircularArc(vertices[i - 1], fP1, fP2,
                    dTheta1, dTheta2, &m_pfBufferVerts[j], nArcVertices);
                j += nArcVertices;
            }

            // else advance one vertex, and compute the offset vector for the next
            // line segment





            //                  North
            //                       .
            //                    .
            //                    .------\  theta1
            //                    .        \               //other chars to avoid Linux warning
            //                    .        |
            //                    .        V               m_pfBufferVerts[j]
            //     vertices[i-1]    * ----------*  <-  <-  *------- * vertices[i-2]
            //                       | m_pfBufferVerts[j+1]
            //                       |     (Hook Point)       ^
            //                    |                       |
            //                    |
            //                    |
            //                    |
            //                       |                       ^
            //                    |                       |
            //                    |
            //                    |
            //       vertices[i]  *                       *

            else if (turnPtInfo[k].prevTurn == BufferUtility::RightTurn)
            {
                dTheta1 = turnPtInfo[k].dNextAzimuth - 90.0;
                GetOffsetPoint(dTheta1, vertices[i - 1], m_pfBufferVerts[j++]);
                GetHookPoint(vertices[i], vertices[i - 1],
                    FALSE, dTheta1, m_pfBufferVerts[j++]);
            }

            // if a right turn was made, or the offset chain has grown too large
            // then add the edges in the offset chain, and start a new chain

            //                      North
            //                           .
            //                        .
            //                        .-----\  theta1
            //                        .       \                //other chars to avoid Linux warning
            //                        .       |
            //                        .       |
            //     vertices[i-1]        * -----|--------------- * vertices[i-2]
            //                           |       |
            //                        |<-----/
            //                        |
            //                        |
            //  m_pfBufferVerts[0]    *
            //
            //                        |
            //                        V
            //
            //  m_pfBufferVerts[1]  *   ->        ->        ->  *
            //                        |
            //                        |
            //                        |
            //                        |
            //       vertices[i]        *

            if (((turnPtInfo[k].prevTurn == BufferUtility::RightTurn) ) || (j >= MaxEdgesPerChain))
            {
                // Add the edges in the offset chain and check for border conditions.
                if ( j != 2 ) // Avoid just the hook point for 1st vertex
                {
                    AddChainEdgesAndCheck(m_pfBufferVerts, j);
                }

                // Start a new chain.
                dTheta1 = turnPtInfo[k].dPrevAzimuth + 90.0;
                GetHookPoint(vertices[i - 1], vertices[i - 2], TRUE, dTheta1/*-180*/, m_pfBufferVerts[0]);
                GetOffsetPoint(dTheta1, vertices[i - 1], m_pfBufferVerts[1]);
                j = 2;
            }
        }

        // add semicircular cap centered at the first vertex of the input polyobject

        //                      North
        //                           .
        //                        .
        //                <-        *        <-
        //                        |
        //          /                |                ^
        //         V                |                 \          //other chars to avoid Linux warning
        //                        |
        //            theta2      |     theta1
        //       |          /-----|-------\            ^
        //       V         |        |       |            |
        //                 V        |       V
        //                    vertices[0]
        //    P2 * -------------- * ----------------- * P1
        //                        |
        //       |                |                    ^
        //       V                |                    |
        //                        |
        //                        |
        //       |                |                    ^
        //       V                 *                    |
        //                    vertices[1]

        if ( !addedStartPointToLoop )
        {
            dTheta1 = turnPtInfo[0].dNextAzimuth - 90.0;
            GetOffsetPoint(dTheta1, vertices[0], fP1);
            dTheta2 = turnPtInfo[0].dNextAzimuth + 90.0;
            GetOffsetPoint(dTheta2, vertices[0], fP2);

            PolygonizeCircularArc(vertices[0], fP1, fP2, dTheta1, dTheta2, &m_pfBufferVerts[j], nArcVertices);

            AddChainEdgesAndCheck(m_pfBufferVerts, j + nArcVertices);
        }
        else if ( j != 2 ) //Avoid just the hook point
        {
            AddChainEdgesAndCheck(m_pfBufferVerts, j);
        }
    }


} // end: CreateConvexOffsetChains()


//------------------------------------------------------------------------------
//
// METHOD: GetHookPoint().
//
// PURPOSE:
//
//
//                          North
//                            .
//                            .
//                            .     OffsetAzimuth
//                            .----\                  //other chars to avoid Linux warning
//                            .    |
//                            .    V
//                     P2  P1 * ------------- * ------ * P3
//    (if bOffP0 == FALSE)    |                P         | (if bOffP0 == FALSE)
//                            |                (if bOffP0 == FALSE)
//                            |                         |
//                            |                         |
//                            |                         |
//                            |                         |
//                     P2  P0 * ------------- * ------ * P3
//   (if bOffP0 == TRUE)                    P           (if bOffP0 == TRUE)
//                                            (if bOffP0 == TRUE)
//
// PARAMETERS:
//
//     Input:
//
//         fP0             - passes a reference to the OpsFloatPoint containing
//                         the coordinates of the first end point 'P0' of the line
//                         segment defining the directed line.
//         fP1             - passes a reference to the OpsFloatPoint containing
//                         the coordinates of the second end point 'P1' of the line
//                         segment defining the directed line (the line is
//                         directed from endPt1 to endPt2).
//         bOffP0        - passes TRUE if the hook edges should be dropped
//                         toward fP0, and FALSE if it should be dropped
//                         toward fP1.
//         dOffsetAzimuth - passes the azimuth of the offset vector.
//
//     Output:
//
//         fP             - passes a reference to an OpsFloatPoint. The endpoint 'P'
//                         coordinates of the hook edge are copied to the
//                         referenced object.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::GetHookPoint(const OpsFloatPoint &fP0,
    const OpsFloatPoint &fP1, bool bOffP0,
    double dOffsetAzimuth, OpsFloatPoint &fP)
{
    // initialize a reference to the segment endpoint the hook edge is to be
    // dropped toward

    const OpsFloatPoint &fP2 = bOffP0 ? fP0 : fP1;

    // if the offset distance is greater than the hook edge length, then
    // compute the end point hook edge endpoint

    if (m_offsetDist > m_hookEdgeLength)
    {
        OpsFloatPoint fP3;

        GetOffsetPoint(dOffsetAzimuth, fP2, fP3);
        double dTheta1 = GetAzimuth(fP3, fP2);

        // Special case: the orientation is not deterministic.
        if ( dTheta1 == 0 )
        {
            fP = fP2;
        }
        else
        {
            GetOffsetPoint(dTheta1, fP3, fP, m_hookEdgeLength);
        }
    }

    // else just set the hook edge endpoint to be the same as the segment
    // endpoint it was to be dropped toward

    else
    {
        fP = fP2;
    }

} // end: GetHookPoint()

//------------------------------------------------------------------------------
//
// METHOD: AddChainEdges().
//
// PURPOSE: Adds a sequences of edges that are to be swept over in the PlaneSweep
//            algorithm.
//
// PARAMETERS:
//
//     Input:
//
//         vertices     - passes an array of edge vertices.
//         nVertices    - passes the number of vertices in the sequence (the
//                        number of edges = nVertices - 1).
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
// NOTES: It is the callers' responsibility to free up any resources allocated
//          in the vector vChainEdges.
//
//------------------------------------------------------------------------------

void GreatCircleBufferUtil::AddChainEdgesAndCheck(const OpsFloatPoint vertices[], INT32 nVertices)
{
    // Before adding edges into the offset chain, check for the added set
    // of edges for any cross border conditions.  If there is, a
    // BorderWalker exception will be thrown from this function.

    if (m_pBorderWalker != NULL && PERF_CHECK_BORDER_CROSSING_VERT)
    {
        m_pBorderWalker->CheckBorderCrossingVertices(vertices, nVertices);
    }

    BufferUtility::AddChainEdges(vertices, nVertices);
}
