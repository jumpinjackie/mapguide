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
// FILE: plinbufr.cpp.
//
// PURPOSE: Implementation of the PolylineBuffer class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"
//#include <mapobjec.h>
//#include "plinobj.h"

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Create and initialize a PolylineBuffer object.
//
// PARAMETERS:
//
//     Input:
//
//         polylineObj    - passes a reference to the polyline object that the
//                          buffer is to be generated for.
//         nSegsPerCircle - passes the number of segments that would be used in
//                          the polygonization of a complete circle. This para-
//                          meter specifies the smoothness of polygonized offset
//                          arcs.
//         pBufferUtil    - passes a pointer to the BufferUtility object.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the buffer zone.
//
//------------------------------------------------------------------------------

//PolylineBuffer::PolylineBuffer(const PolylineObject &polylineObj, BufferUtility *pBufferUtil) :
//    PolyObjectBuffer(pBufferUtil)
//{
//    PolylineObjVertexAccessor vertAccessor(polylineObj);
//    Initialize(vertAccessor);
//} // end: constructor

PolylineBuffer::PolylineBuffer(const OpsPolyline &polyline, BufferUtility *pBufferUtil) :
    PolyObjectBuffer(pBufferUtil)
{
    OpsPolylineVertexAccessor vertAccessor(polyline);
    Initialize(vertAccessor);
} // end: constructor

PolylineBuffer::PolylineBuffer(const OpsPolyPolyline &polyPolyline, BufferUtility *pBufferUtil) :
    PolyObjectBuffer(pBufferUtil)
{
    OpsPolyPolylineVertexAccessor vertAccessor(polyPolyline);
    Initialize(vertAccessor);
} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Release all dynamically allocated resources.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

PolylineBuffer::~PolylineBuffer()
{
    Cleanup();
} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: CreateBufferZone().
//
// PURPOSE: Create a buffer zone around the polyline object specified at
//          construction time.
//
// PARAMETERS:
//
//     Input:
//
//         callback       - passes a pointer to a ProgressCallback object. The
//                          object is periodically notified of progress, and
//                          checked to determine if the buffer operation has
//                          been canceled.
//
//     Output:
//
//         bufferPolygon  - passes a reference to an OrientedPolyPolygon object.
//                          The boundaries of the buffer zone polygon are copied
//                          to the referenced object.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the buffer zone. A PlaneSweepException is
//             thrown if an error is detected during the execution of the plane
//             sweep algorithm.
//
//------------------------------------------------------------------------------

void PolylineBuffer::CreateBufferZone(ProgressCallback &callback,
    OrientedPolyPolygon &bufferPolygon)
{
    // if the polyline object consists of a single line segment, then call the
    // buffering utility class to compute the buffer directly

    if (m_nPolyObjects == 1 && m_pnPolyVerts[0] == 2) {
        m_pBufferUtil->InitPointLine();
        callback.BeginProgressInterval();
        m_pBufferUtil->CreateLineSegmentBuffer(m_pVertices, bufferPolygon);
        callback.EndProgressInterval();
    }

    // else forward call to base class method to do the full plane sweep

    else
    {
        PolyObjectBuffer::CreateBufferZone(callback, bufferPolygon);
    }
} // end: CreateBufferZone()


//------------------------------------------------------------------------------
//
// METHOD: AcceptBoundary().
//
// PURPOSE: Determine whether a potential polygon boundary should be included
//          as part of the buffer zone polygon. If the boundary has counter-
//          clockwise orientation, then it is accepted without further testing.
//          For boundaries with clockwise orientation, the interior point is
//          tested to ensure that is does not lie within the buffer offset
//          distance of any line segment of the polyline object.
//
// PARAMETERS:
//
//     Input:
//
//         boundaryExt    - passes a reference to an OpsFloatExtent containing
//                          the extent of the boundary.
//         boundaryOrient - passes an enumerated value of type Orientation,
//                          specifying the orientation of the boundary.
//         boundaryVert   - passes a reference to an OpsFloatPoint. The refer-
//                          enced object contains the coordinates of a point
//                          that lies on the polygon boundary.
//         interiorPt     - passes a reference to a OpsDoublePoint containing
//                          the coordinates of a point that is strictly interior
//                          to the boundary.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the polygon boundary should be accepted as part of the
//          output and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL PolylineBuffer::AcceptBoundary(const OpsFloatExtent &,
    Orientation boundaryOrient, const OpsFloatPoint &,
    const OpsDoublePoint &interiorPt) const
{
    if (boundaryOrient == Clockwise) {
        for (int i = 0, j = 0; i < m_nPolyObjects; i++) {
            if (PointWithinOffsetDist(&m_pVertices[j], m_pnPolyVerts[i],
                    interiorPt))
                return FALSE;

            j += m_pnPolyVerts[i];
        }
    }

    return TRUE;
} // end: AcceptBoundary()


//------------------------------------------------------------------------------
//
// Implementation of the vertex accessor objects.
//
//------------------------------------------------------------------------------

//PolylineBuffer::PolylineObjVertexAccessor::PolylineObjVertexAccessor(
//    const PolylineObject &polylineObj) : polylineObj(polylineObj)
//{
//}

//int PolylineBuffer::PolylineObjVertexAccessor::GetTotalVertices() const
//{
//    return polylineObj.GetTotalVertices();
//}

//int PolylineBuffer::PolylineObjVertexAccessor::GetNPolyObjects() const
//{
//    return polylineObj.GetNPolylines();
//}

//int PolylineBuffer::PolylineObjVertexAccessor::GetNPolyVerts(int index) const
//{
//    return polylineObj.GetNPolyVerts(index);
//}

//const OpsFloatPoint &PolylineBuffer::PolylineObjVertexAccessor::operator[](
//    int index) const
//{
//    return polylineObj[index];
//}

//void PolylineBuffer::PolylineObjVertexAccessor::GetExtent(
//    OpsFloatExtent &extent) const
//{
//    polylineObj.GetExtent(&extent);
//}


PolylineBuffer::OpsPolylineVertexAccessor::OpsPolylineVertexAccessor(
    const OpsPolyline &polyline) :
    polyline(polyline)
{
}

int PolylineBuffer::OpsPolylineVertexAccessor::GetTotalVertices() const
{
    return polyline.GetNVertices();
}

int PolylineBuffer::OpsPolylineVertexAccessor::GetNPolyObjects() const
{
    return 1;
}

int PolylineBuffer::OpsPolylineVertexAccessor::GetNPolyVerts(int index) const
{
    assert(index == 0);
    return polyline.GetNVertices();
}

const OpsFloatPoint &PolylineBuffer::OpsPolylineVertexAccessor::operator[](
    int index) const
{
    return polyline[index];
}

void PolylineBuffer::OpsPolylineVertexAccessor::GetExtent(
    OpsFloatExtent &extent) const
{
    polyline.GetExtent(&extent);
}


PolylineBuffer::OpsPolyPolylineVertexAccessor::OpsPolyPolylineVertexAccessor(
    const OpsPolyPolyline &polyPolyline) : polyPolyline(polyPolyline)
{
}

int PolylineBuffer::OpsPolyPolylineVertexAccessor::GetTotalVertices() const
{
    return polyPolyline.GetTotalVertices();
}

int PolylineBuffer::OpsPolyPolylineVertexAccessor::GetNPolyObjects() const
{
    return polyPolyline.GetNPolylines();
}

int PolylineBuffer::OpsPolyPolylineVertexAccessor::GetNPolyVerts(int index) const
{
    return polyPolyline.GetNPolyVerts(index);
}

const OpsFloatPoint &PolylineBuffer::OpsPolyPolylineVertexAccessor::operator[](
    int index) const
{
    return polyPolyline[index];
}

void PolylineBuffer::OpsPolyPolylineVertexAccessor::GetExtent(
    OpsFloatExtent &extent) const
{
    polyPolyline.GetExtent(&extent);
}
