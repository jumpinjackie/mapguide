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
// FILE: LatLonBorderWalker.cpp.
//
// PURPOSE: Implementation of the LatLonBorderWalker class.
//
//------------------------------------------------------------------------------

#include "buffer.h"
#include "LatLonBorderWalker.h"

#define AZIMUTH_TOLERANCE 1.0

bool LatLonBorderWalker::BorderPoint::operator < (const BorderPoint &bpBorderPt)
{
    return fpPt.y < bpBorderPt.fpPt.y ? true : false;
}

bool LatLonBorderWalker::BorderPoint::operator > (const BorderPoint &bpBorderPt)
{
    return fpPt.y > bpBorderPt.fpPt.y ? true : false;
}

bool LatLonBorderWalker::BorderPoint::operator == (const BorderPoint &bpBorderPt)
{
    return fpPt == bpBorderPt.fpPt ? true : false;
}

bool LatLonBorderWalker::BorderPoint::operator != (const BorderPoint &bpBorderPt)
{
    return fpPt != bpBorderPt.fpPt ? true : false;
}

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize a LatLonBorderWalker object.
//
// PARAMETERS:
//
//     Input:
//
//         nSegsPerCircle -
//
//     Output:
//
//         None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

LatLonBorderWalker::LatLonBorderWalker(FloatTransform *transform, MgCoordinateSystemMeasure *measure) :
    m_transform(transform),
    m_bLinkedVertsInitialized(FALSE)
{
    m_measure = SAFE_ADDREF((MgCoordinateSystemMeasure*)measure);
    m_cSys = m_measure->GetCoordSys();

    Ptr<MgEnvelope> env = measure->GetEnvelope();
    Ptr<MgCoordinate> upperRight = env->GetUpperRightCoordinate();
    Ptr<MgCoordinate> lowerLeft = env->GetLowerLeftCoordinate();
    m_dMaxX = upperRight->GetX();
    m_dMaxY = upperRight->GetY();
    m_dMinX = lowerLeft->GetX();
    m_dMinY = lowerLeft->GetY();

    m_transform->Double2Float(m_dMaxX, m_dMaxY, m_fpPtNE);
    m_transform->Double2Float(m_dMinX, m_dMaxY, m_fpPtNW);
    m_transform->Double2Float(m_dMinX, m_dMinY, m_fpPtSW);
    m_transform->Double2Float(m_dMaxX, m_dMinY, m_fpPtSE);

    m_dWestMin = m_dMinX + (m_dMaxX - m_dMinX) / 4.0;
    m_dEastMin = m_dMaxX - (m_dMaxX - m_dMinX) / 4.0;
} // end: constructor

//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Release all dynamically acquired resources.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

LatLonBorderWalker::~LatLonBorderWalker()
{
} // end: destructor

//------------------------------------------------------------------------------
//
// METHOD: GetLLAzimuthType().
//
// PURPOSE:
//                 North
//                   .
//                   .
//                   .     Azimuth (Angle with respect to the North)
//                   .----\               //other chars to avoid Linux warning
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
//         mpP0LL  - Passes a reference to an MapPoint containing P0.
//           mpP1LL  - Passes a reference to an MapPoint containing P1.
//
//     Output:
//
//         None.
//
// RETURNS: Azimuth Type (Angle type with respect to the North) formed by P0 and P1.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------


LatLonBorderWalker::AzimuthType LatLonBorderWalker::GetLLAzimuthType(MgCoordinate* p0MCS,
    MgCoordinate* p1MCS)
{
    //be consistent with GetMCSAzimuthType for points that are equal
    if (p0MCS->GetX() == p1MCS->GetX() &&
        p0MCS->GetY() == p1MCS->GetY())
        return SouthNorthAzimuth;

    double dAzimuth = m_measure->GetAzimuth(p0MCS, p1MCS);

    if ((dAzimuth == 0.0) || (dAzimuth == 180.0))
    {
        return SouthNorthAzimuth;
    }

    if (dAzimuth > 180.0)
    {
        dAzimuth -= 360.0;
    }
    else if (dAzimuth < -180.0)
    {
        dAzimuth += 360.0;
    }

    if (dAzimuth > 0.0)
    {
        return PositiveAzimuth;
    }
    else
    {
        return NegativeAzimuth;
    }

} // end: GetLLAzimuthType()

//------------------------------------------------------------------------------
//
// METHOD: GetMCSAzimuthType().
//
// PURPOSE:
//                 North
//
//                   PN
//                __ *
//                VN .
//                   .
//                   .     Azimuth (Angle with respect to the North)
//                   .----\                   //other chars to avoid Linux warning
//                   .    |
//                   .    V
//                P0 * ------------- * P1
//                   .             __
//                   .             V1
//                   .
//                   .
//
//
// PARAMETERS:
//
//     Input:
//
//         mpP0LL  - Passes a reference to an MapPoint containing P0.
//           mpP1LL  - Passes a reference to an MapPoint containing P1.
//
//     Output:
//
//         None.
//
// RETURNS: Azimuth Type (Angle type with respect to the North) formed by P0 and P1.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

LatLonBorderWalker::AzimuthType LatLonBorderWalker::GetMCSAzimuthType(MgCoordinate* p0MCS,
    MgCoordinate* p1MCS)
{
    double V1x, V1y, VNx, VNy;

    V1x = p1MCS->GetX() - p0MCS->GetX();
    V1y = p1MCS->GetY() - p0MCS->GetY();
    VNx = 0.0;
    VNy = m_dMaxY - p0MCS->GetY();

    double dCrossProduct = (VNx * V1y) - (V1x * VNy);

    if (dCrossProduct > 0.0)
    {
        return NegativeAzimuth;
    }
    else if (dCrossProduct < 0.0)
    {
        return PositiveAzimuth;
    }
    else
    {
        return SouthNorthAzimuth;
    }

} // end: GetMCSAzimuthType()

//------------------------------------------------------------------------------
//
// METHOD: GetMCSAbsAzimuth().
//
// PURPOSE:
//
// PARAMETERS:
//
//     Input:
//
//         mpP0LL  - Passes a reference to an MapPoint containing P0.
//           mpP1LL  - Passes a reference to an MapPoint containing P1.
//
//     Output:
//
//         None.
//
// RETURNS: Absolute Azimuth (Absoluate angle with respect to the North) formed by P0 and P1.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double LatLonBorderWalker::GetMCSAbsAzimuth(MgCoordinate* p0MCS, MgCoordinate* p1MCS)
{
    Ptr<MgCoordinate> v1 = new MgCoordinateXY(p1MCS->GetX() - p0MCS->GetX(), p1MCS->GetY() - p0MCS->GetY());
    Ptr<MgCoordinate> vN = new MgCoordinateXY(0.0, m_dMaxY - p0MCS->GetY());

    double dV1DotVN, dV1Norm, dVNNorm, dTheta;

    dV1Norm = ::sqrt(v1->GetX() * v1->GetX() + v1->GetY() * v1->GetY());
    dVNNorm = ::sqrt(vN->GetX() * vN->GetX() + vN->GetY() * vN->GetY());
    dV1DotVN = v1->GetX() * vN->GetX() + v1->GetY() * vN->GetY();

    dTheta = ::acos(dV1DotVN / dV1Norm / dVNNorm) * 180.0 / MgMathUtility::GetPi();

    return ::fabs(dTheta);

} // end: GetMCSAbsAzimuth()

bool LatLonBorderWalker::CheckBorderCrossing(MgCoordinate* p0MCS, MgCoordinate* p1MCS)
{
    if (((p0MCS->GetX() > m_dEastMin) && (p1MCS->GetX() < m_dWestMin)) ||
        ((p0MCS->GetX() < m_dWestMin) && (p1MCS->GetX() > m_dEastMin)))
    {
        if (GetMCSAbsAzimuth(p0MCS, p1MCS) > AZIMUTH_TOLERANCE)
        {
            return TRUE;
        }
    }

    return FALSE;
} // end: CheckBorderCrossing()


void LatLonBorderWalker::AddCrossBorderPoints(int nP0Index, int nP1Index,
    AzimuthType azimuthTypeMCS)
{
    assert(azimuthTypeMCS != SouthNorthAzimuth);

    //                Negative Azimuth from P0 to P1
    //         ----------------------------------------------
    //         |                                              |
    //          |   * P1                                      |
    // North ^  /                                          ^ North
    //         | /                                          |
    //     BP1 *                                            * BP0
    //         |                                             / |
    //         |                                           /  |
    //         |                                       P0 *      |
    //         |                                              |
    //         ----------------------------------------------
    //

    if (azimuthTypeMCS == NegativeAzimuth)
    {
        // Add BP0 to the East border points list.

        BorderPoint bpBP0;
        bpBP0.wdDirection = North;
        bpBP0.fpPt.x = m_fpPtNE.x;
        bpBP0.fpPt.y = (m_fpInputVerts[nP0Index].y + m_fpInputVerts[nP1Index].y) / 2.0f;
        m_lstEastBorderPts.push_back(bpBP0);

        // Add BP0 to the linked vertices and
        // point the next vertex of P0 to be BP0.

        LinkedVertex lvBP0;
        lvBP0.nInputVertIndex = -1;
        lvBP0.pfpVertex = &(m_lstEastBorderPts.back().fpPt);
        lvBP0.plvNextVertex = NULL;
        m_vLinkedVerts.push_back(lvBP0);
        // The first four indecies are reserved for the corner points.
        m_vLinkedVerts[nP0Index + 4].plvNextVertex = &(m_vLinkedVerts.back());
        m_lstEastBorderPts.back().lvLinkedVert = &(m_vLinkedVerts.back());

        // Add BP1 to the West border points list.

        BorderPoint bpBP1;
        bpBP1.wdDirection = North;
        bpBP1.fpPt.x = m_fpPtNW.x;
        bpBP1.fpPt.y = bpBP0.fpPt.y;
        m_lstWestBorderPts.push_back(bpBP1);

        // Add BP1 to the linked vertices and
        // point the next vertex of BP1 to be P1.

        LinkedVertex lvBP1;
        lvBP1.nInputVertIndex = -1;
        lvBP1.pfpVertex = &(m_lstWestBorderPts.back().fpPt);
        // The first four indecies are reserved for the corner points.
        lvBP1.plvNextVertex = &(m_vLinkedVerts[nP1Index + 4]);
        m_vLinkedVerts.push_back(lvBP1);
        m_lstWestBorderPts.back().lvLinkedVert = &(m_vLinkedVerts.back());
    }

    //                Positive Azimuth from P0 to P1
    //         ----------------------------------------------
    //         |                                              |
    //          |   * P0                                      |
    //       |  /                                          |
    //         | /                                          |
    //     BP0 *                                                * BP1
    //         |                                             / |
    // South V                                           /  V South
    //         |                                       P1 *      |
    //         |                                              |
    //         ----------------------------------------------
    //

    else if (azimuthTypeMCS == PositiveAzimuth)
    {
        // Add BP0 to the West border points list.

        BorderPoint bpBP0;
        bpBP0.wdDirection = South;
        bpBP0.fpPt.x = m_fpPtNW.x;
        bpBP0.fpPt.y = (m_fpInputVerts[nP0Index].y + m_fpInputVerts[nP1Index].y) / 2.0f;
        m_lstWestBorderPts.push_back(bpBP0);

        // Add BP0 to the linked vertices and
        // point the next vertex of P0 to be BP0.

        LinkedVertex lvBP0;
        lvBP0.nInputVertIndex = -1;
        lvBP0.pfpVertex = &(m_lstWestBorderPts.back().fpPt);
        lvBP0.plvNextVertex = NULL;
        m_vLinkedVerts.push_back(lvBP0);
        // The first four indecies are reserved for the corner points.
        m_vLinkedVerts[nP0Index + 4].plvNextVertex = &(m_vLinkedVerts.back());
        m_lstWestBorderPts.back().lvLinkedVert = &(m_vLinkedVerts.back());

        // Add BP1 to the East border points list.

        BorderPoint bpBP1;
        bpBP1.wdDirection = South;
        bpBP1.fpPt.x = m_fpPtNE.x;
        bpBP1.fpPt.y = bpBP0.fpPt.y;
        m_lstEastBorderPts.push_back(bpBP1);

        // Add BP1 to the linked vertices and
        // point the next vertex of BP1 to be P1.

        LinkedVertex lvBP1;
        lvBP1.nInputVertIndex = -1;
        lvBP1.pfpVertex = &(m_lstEastBorderPts.back().fpPt);
        // The first four indecies are reserved for the corner points.
        lvBP1.plvNextVertex = &(m_vLinkedVerts[nP1Index + 4]);
        m_vLinkedVerts.push_back(lvBP1);
        m_lstEastBorderPts.back().lvLinkedVert = &(m_vLinkedVerts.back());
    }
}

void LatLonBorderWalker::CheckBorderCrossingVertices(const OpsFloatPoint *fpVerts, int nNumVerts)
{
    AzimuthType azimuthTypeLL, azimuthTypeMCS;
    Ptr<MgCoordinate> p0MCS;
    Ptr<MgCoordinate> p1MCS;

    // Check every pair of vertices to see if any of the pairs crosses border.

    for (int i = 0; i < (nNumVerts - 1); ++i)
    {
        p0MCS = m_transform->Float2Double(fpVerts[i]);
        p1MCS = m_transform->Float2Double(fpVerts[i + 1]);

        azimuthTypeLL = GetLLAzimuthType(p0MCS, p1MCS);
        azimuthTypeMCS = GetMCSAzimuthType(p0MCS, p1MCS);

        if (azimuthTypeLL == SouthNorthAzimuth)
        {
            if (CheckBorderCrossing(p0MCS, p1MCS))
            {
                throw new BorderWalkerException(BorderWalkerException::VerticesCrossingBorderError);
            }
        }

        // If Lat-Lon azimuth type is different than the MCS azimuth type,
        // we have found a cross-border condition.

        else if (azimuthTypeLL != azimuthTypeMCS)
        {
            if (CheckBorderCrossing(p0MCS, p1MCS))
            {
                throw new BorderWalkerException(BorderWalkerException::VerticesCrossingBorderError);
            }
        }
    }
}

void LatLonBorderWalker::InitLinkedVertices()
{
    m_vLinkedVerts.clear();

    // Initialize the linked vertices to contain corner vertices first.

    LinkedVertex lvVertCorner;

    lvVertCorner.nInputVertIndex = -1;
    lvVertCorner.pfpVertex = &m_fpPtNE;
    lvVertCorner.plvNextVertex = NULL;
    m_vLinkedVerts.push_back(lvVertCorner);

    lvVertCorner.nInputVertIndex = -1;
    lvVertCorner.pfpVertex = &m_fpPtNW;
    lvVertCorner.plvNextVertex = NULL;
    m_vLinkedVerts.push_back(lvVertCorner);

    lvVertCorner.nInputVertIndex = -1;
    lvVertCorner.pfpVertex = &m_fpPtSW;
    lvVertCorner.plvNextVertex = NULL;
    m_vLinkedVerts.push_back(lvVertCorner);

    lvVertCorner.nInputVertIndex = -1;
    lvVertCorner.pfpVertex = &m_fpPtSE;
    lvVertCorner.plvNextVertex = NULL;
    m_vLinkedVerts.push_back(lvVertCorner);

    // Initialize the linked vertices to contain the set of input vertices.

    assert(m_fpInputVerts[0] == m_fpInputVerts[m_nNumInputVerts - 1]);

    LinkedVertex lvVert;

    // P0, P1, ..., P(N-2), P(N-1)

    int k;
    for (k = 0; k < m_nNumInputVerts; ++k)
    {
        lvVert.nInputVertIndex = k;
        lvVert.pfpVertex = &(m_fpInputVerts[k]);
        lvVert.plvNextVertex = NULL;
        m_vLinkedVerts.push_back(lvVert);
    }

    // The last vertex is the same as the first vertex.
    // Make its index to equal to zero so that the CreateBufferZone function
    // can detects its closure condition.
    m_vLinkedVerts.back().nInputVertIndex = 0;

    //
    // [0] [1] [2] [3] [4] [5] [...] [N-2+4] [N-1+4] [N+4] [N+4+1] [N+4+...]
    // PNE PNW PSW PSE P0  P1  ...     P(N-2)  P(N-1)  BP0   BP1     ...
    //
    // P0 -> P1 -> ... -> P(N-2) -> P(N-1)

    for (k = 4; k < (m_nNumInputVerts - 1 + 4); ++k)
    {
        m_vLinkedVerts[k].plvNextVertex = &(m_vLinkedVerts[k + 1]);
    }

    // As k = N - 1 + 4
    //
    // [0] [1] [2] [3] [4] [5] [...] [N-2+4] [N-1+4] [N+4] [N+4+1] [N+4+...]
    // PNE PNW PSW PSE P0  P1  ...     P(N-2)  P(N-1)  BP0   BP1     ...
    //
    // P0 -> P1 -> ... -> P(N-2) -> P(N-1) -> NULL
    //

    m_vLinkedVerts[k].plvNextVertex = NULL;

    // Flag to indicate the initialization for linked vertices.

    m_bLinkedVertsInitialized = TRUE;
}

void LatLonBorderWalker::ScanAndWalk(const OpsFloatPoint *fpVerts, int nNumVerts,
    vector<OpsFloatPoint*> &vBufferZones, vector<int> &vBufferZonesCount)
{
    // Note: The input vertices are assumed to be closed.

    m_fpInputVerts = fpVerts;
    m_nNumInputVerts = nNumVerts;
    m_pvBufferZones = &vBufferZones;
    m_pvBufferZonesCount = &vBufferZonesCount;

    m_pvBufferZones->clear();
    m_pvBufferZonesCount->clear();
    m_lstEastBorderPts.clear();
    m_lstWestBorderPts.clear();

    ScanVertices();

    if (!m_lstEastBorderPts.empty())
    {
        WalkBorder();

//#ifdef _DEBUG
//        TRACE("\n--- Start tracing linked verts ---\n\n");
//        Ptr<MgCoordinate> p0;
//        Ptr<MgCoordinate> p1;
//        Ptr<MgCoordinate> p0LL;
//        Ptr<MgCoordinate> p1LL;
//
//        for (unsigned int i = 4; i < m_vLinkedVerts.size(); ++i)
//        {
//            p0 = m_transform->Float2Double(m_vLinkedVerts[i].pfpVertex);
//            double xValue, yValue;
//            m_cSys->ConvertToLonLat(p0->GetX(), p0->GetY(), xValue, yValue);
//            p0LL = new MgCoordinateXY(xValue, yValue);
//            if (m_vLinkedVerts[i].plvNextVertex != NULL)
//            {
//                p1 = m_transform->Float2Double(&m_vLinkedVerts[i].plvNextVertex->pfpVertex);
//                m_cSys->ConvertToLonLat(p1->GetX(), p1->GetY(), xValue, yValue);
//                p1LL = new MgCoordinateXY(xValue, yValue);
//            }
//            else
//            {
//                p1LL = new MgCoordinateXY(0.0, 0.0);
//            }
//            TRACE("x, y, i: %f %f %i\n", p0LL->GetX, p0LL->GetY, m_vLinkedVerts[i].nInputVertIndex);
//            TRACE(" next: %f %f\n", p1LL->GetX, p1LL->GetY);
//        }
//
//        TRACE("\n-- Border Points --\n");
//        TRACE("E: 0 S: 1 W: 2 N: 3\n\n");
//
//        list<BorderPoint>::iterator iter = m_lstEastBorderPts.begin();
//
//        while (iter != m_lstEastBorderPts.end())
//        {
//            p0 = m_transform->Float2Double(&(iter->fpPt));
//            double xValue, yValue;
//            m_cSys->ConvertToLonLat(p0->GetX(), p0->GetY(), xValue, yValue);
//            p0LL = new MgCoordinateXY(xValue, yValue);
//            TRACE("e x, y, dir: %f %f %i\n", p0LL->GetX, p0LL->GetY, iter->wdDirection);
//              iter++;
//        }
//        iter = m_lstWestBorderPts.begin();
//        while (iter != m_lstWestBorderPts.end())
//        {
//            p0 = m_transform->Float2Double(&(iter->fpPt));
//            double xValue, yValue;
//            m_cSys->ConvertToLonLat(p0->GetX(), p0->GetY(), xValue, yValue);
//            p0LL = new MgCoordinateXY(xValue, yValue);
//            TRACE("w x, y, dir: %f %f %i\n", p0LL->GetX, p0LL->GetY, iter->wdDirection);
//            iter++;
//        }
//#endif

        CreateBufferZones();
    }

#ifdef _DEBUG
    else
    {
        assert(m_lstWestBorderPts.empty());
    }
#endif

    m_bLinkedVertsInitialized = FALSE;

} // end: ScanAndWalk()


void LatLonBorderWalker::ScanVertices()
{
    AzimuthType azimuthTypeLL, azimuthTypeMCS;
    Ptr<MgCoordinate> p0MCS;
    Ptr<MgCoordinate> p1MCS;

    // Check every pair of vertices to see if any of the pairs crosses border.

    for (int i = 0; i < (m_nNumInputVerts - 1); ++i)
    {
        p0MCS = m_transform->Float2Double(m_fpInputVerts[i]);
        p1MCS = m_transform->Float2Double(m_fpInputVerts[i + 1]);

        azimuthTypeLL = GetLLAzimuthType(p0MCS, p1MCS);
        azimuthTypeMCS = GetMCSAzimuthType(p0MCS, p1MCS);

        if (azimuthTypeLL == SouthNorthAzimuth)
        {
            if (CheckBorderCrossing(p0MCS, p1MCS))
            {
                throw new BorderWalkerException(BorderWalkerException::VerticesCrossingBorderError);
            }
        }

        // If Lat-Lon azimuth type is different than the MCS azimuth type,
        // we have found a cross-border condition.

        else if (azimuthTypeLL != azimuthTypeMCS)
        {
            if (CheckBorderCrossing(p0MCS, p1MCS))
            {

                if (!m_bLinkedVertsInitialized)
                {
                    InitLinkedVertices();
                }

                AddCrossBorderPoints(i, i + 1, azimuthTypeMCS);
            }
        }
    }
} // end: ScanVertices()

void LatLonBorderWalker::WalkBorder()
{
    // Prepare the east-side border points for walking.

    m_lstEastBorderPts.sort();

    bool bIteratorValid = false;
    list<BorderPoint>::iterator iterBorderWalker = m_lstEastBorderPts.begin();
    list<BorderPoint>::iterator iterWalkBegin;

    // Let's start a walk around the border beginning with the East border.

    while (iterBorderWalker != m_lstEastBorderPts.end())
    {
        if (iterBorderWalker->wdDirection == North)
        {
            // Begin to walk when we see a border point walking
            // North.

            if (!bIteratorValid)
            {
                iterWalkBegin = iterBorderWalker;
                bIteratorValid = true;
            }
        }

        if (iterBorderWalker->wdDirection == South)
        {
            // Simply skip through all the south walking border points
            // when border walker has not begun to walk yet.

            if (bIteratorValid)
            {
                // When we find a matching south walk direction while
                // starting with a north walk direction, link these two
                // border points by pointing the north walking border point
                // to the south walking border point.

                if (iterWalkBegin->wdDirection == North)
                {
                    iterWalkBegin->lvLinkedVert->plvNextVertex = iterBorderWalker->lvLinkedVert;

                    // Finish walking.
                    bIteratorValid = false;
                }
            }
        }

        ++iterBorderWalker;
    }

    // Include the top corner vertices if the walker is currently walking.

    if (bIteratorValid)
    {
        assert(iterWalkBegin->wdDirection == North);

        iterWalkBegin->lvLinkedVert->plvNextVertex = &(m_vLinkedVerts[NE]);
        m_vLinkedVerts[NE].plvNextVertex = &(m_vLinkedVerts[NW]);
    }

    m_lstWestBorderPts.sort();
    m_lstWestBorderPts.reverse();

    // Let's now walk through the West border.

    iterBorderWalker = m_lstWestBorderPts.begin();

    while (iterBorderWalker != m_lstWestBorderPts.end())
    {
        if (iterBorderWalker->wdDirection == South)
        {
            // Begin to walk when we see a border point walking
            // South.

            if (!bIteratorValid)
            {
                iterWalkBegin = iterBorderWalker;
                bIteratorValid = true;
            }
        }

        if (iterBorderWalker->wdDirection == North)
        {
            // Simply skip through all the south walking border points
            // when border walker has not begun to walk yet.

            if (bIteratorValid)
            {
                // When we find a matching north walk direction while
                // starting with a north walk direction, the starting
                // border point must come from the east-side border.
                // Hence, link the NW corner point with this west-side
                // border point by pointing the NW corner point to the
                // north walking border point of the west-side border.

                if (iterWalkBegin->wdDirection == North)
                {
                    m_vLinkedVerts[NW].plvNextVertex = iterBorderWalker->lvLinkedVert;

                    bIteratorValid = false;
                }

                // When we find a matching north walk direction while
                // starting with a south walk direction, link these two
                // border points by pointing the south walking border point
                // to the north walking border point.

                else if (iterWalkBegin->wdDirection == South)
                {
                    iterWalkBegin->lvLinkedVert->plvNextVertex = iterBorderWalker->lvLinkedVert;

                    // Finish walking.
                    bIteratorValid = false;
                }
            }
        }

        ++iterBorderWalker;
    }

    // Include the bottom corner vertices if the walker is currently walking.

    if (bIteratorValid)
    {
        assert(iterWalkBegin->wdDirection == South);

        iterWalkBegin->lvLinkedVert->plvNextVertex = &(m_vLinkedVerts[SW]);
        m_vLinkedVerts[SW].plvNextVertex = &(m_vLinkedVerts[SE]);

        // Finally, find the closing point on the East border.  This closing point
        // has to be a south walking point.

        iterBorderWalker = m_lstEastBorderPts.begin();

        while (iterBorderWalker != m_lstEastBorderPts.end())
        {
            if (iterBorderWalker->wdDirection == South)
            {
                m_vLinkedVerts[SE].plvNextVertex = iterBorderWalker->lvLinkedVert;
            }

            ++iterBorderWalker;
        }
    }

} // end: WalkBorder()


void LatLonBorderWalker::CreateBufferZones()
{
    int nNumBufferZoneVerts = m_nNumInputVerts * 2 + 4;
    OpsFloatPoint *pfpBufferZone;
    bool *bInputVertsVisited = new bool [m_nNumInputVerts];
    int i, j;
    int nNumInputVertsVisited = 0;

    // Initialize the array of input vertices visiting information to
    // be non-visited;

    for (i = 0; i < (m_nNumInputVerts - 1); ++i)
    {
        bInputVertsVisited[i] = FALSE;
    }

    // Go through each input vertex and make sure that
    // every input vertex is visited.

    for (i = 0; i < (m_nNumInputVerts - 1); ++i)
    {
        if (bInputVertsVisited[i] == FALSE)
        {
            pfpBufferZone = new OpsFloatPoint [nNumBufferZoneVerts];
            LinkedVertex *plvBegin;
            LinkedVertex *plvNext;

            plvBegin = &(m_vLinkedVerts[i + 4]);

            pfpBufferZone[0] = *(plvBegin->pfpVertex);
            bInputVertsVisited[plvBegin->nInputVertIndex] = TRUE;
            ++nNumInputVertsVisited;

            plvNext = plvBegin->plvNextVertex;

            for (j = 1; plvNext->nInputVertIndex != plvBegin->nInputVertIndex; ++j)
            {
                assert(j <= nNumBufferZoneVerts);

                pfpBufferZone[j] = *(plvNext->pfpVertex);

                if (plvNext->nInputVertIndex != -1)
                {
                    int index = plvNext->nInputVertIndex;
                    bInputVertsVisited[index] = TRUE;
                    ++nNumInputVertsVisited;
                }

                assert(plvNext->plvNextVertex != NULL);

                plvNext = plvNext->plvNextVertex;
            }

            pfpBufferZone[j] = pfpBufferZone[0];
            bInputVertsVisited[plvNext->nInputVertIndex] = TRUE;
            ++nNumInputVertsVisited;

            m_pvBufferZones->push_back(pfpBufferZone);
            m_pvBufferZonesCount->push_back(j + 1);

            if (nNumInputVertsVisited == m_nNumInputVerts)
            {
                break;
            }
        }
    }

    delete [] bInputVertsVisited;
    bInputVertsVisited = NULL;

} // end: CreateBufferZones()
