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
// FILE: plgnbufr.cpp.
//
// PURPOSE: Implementation of the PolygonBuffer class.
//
//------------------------------------------------------------------------------
#include "Foundation.h"
#include "buffer.h"
//#include <mapobjec.h>
//#include <plgnobj.h>

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Create and initialize a PolygonBuffer object.
//
// PARAMETERS:
//
//     Input:
//
//         polygonObj     - passes a reference to the polygon object that the
//                          buffer is to be generated for.
//         pBufferUtil    - passes the number of segments that would be used in
//                          the polygonization of a complete circle, and also
//                          the buffer offset distance.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the PolygonBuffer object.
//
//------------------------------------------------------------------------------

//PolygonBuffer::PolygonBuffer(const PolygonObject &polygonObj, BufferUtility *pBufferUtil) :
//    PolyObjectBuffer(pBufferUtil)
//{
//    PolygonObjVertexAccessor vertAccessor(polygonObj);
//    Initialize(vertAccessor);
//} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Create and initialize a PolygonBuffer object.
//
// PARAMETERS:
//
//     Input:
//
//         polygon        - passes a reference to the OpsPolygon object that
//                          the buffer is to be generated for.
//         pBufferUtil    - passes the number of segments that would be used in
//                          the polygonization of a complete circle, and also
//                          the buffer offset distance.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the PolygonBuffer object.
//
//------------------------------------------------------------------------------

PolygonBuffer::PolygonBuffer(const OpsPolygon &polygon, BufferUtility *pBufferUtil) :
    PolyObjectBuffer(pBufferUtil)
{
    OpsPolygonVertexAccessor vertAccessor(polygon);
    Initialize(vertAccessor);
} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Create and initialize a PolygonBuffer object.
//
// PARAMETERS:
//
//     Input:
//
//         polyPolygon    - passes a reference to the OpsPolyPolygon object that
//                          the buffer is to be generated for.
//         pBufferUtil    - passes the number of segments that would be used in
//                          the polygonization of a complete circle, and also
//                          the buffer offset distance.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the PolygonBuffer object.
//
//------------------------------------------------------------------------------

PolygonBuffer::PolygonBuffer(const OpsPolyPolygon &polyPolygon, BufferUtility *pBufferUtil) :
    PolyObjectBuffer(pBufferUtil)
{
    OpsPolyPolygonVertexAccessor vertAccessor(polyPolygon);
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

PolygonBuffer::~PolygonBuffer()
{
    Cleanup();
} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: AcceptBoundary().
//
// PURPOSE: Determine whether a potential polygon boundary should be included
//          as part of the buffer zone polygon.
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
//         interiorPt     - passes a reference to a OpsDoublePoint containing the
//                          coordinates of a point that is strictly interior to
//                          the boundary.
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

BOOL PolygonBuffer::AcceptBoundary(const OpsFloatExtent &boundaryExt,
    Orientation boundaryOrient, const OpsFloatPoint &boundaryVert,
    const OpsDoublePoint &interiorPt) const
{
    // if the boundary has counter-clockwise orientation, then it is a potential
    // outer boundary - accept if the boundary extent fully contains the input
    // polygon object extent, or the boundary vertex does not lie within the
    // input polygon object

    if (boundaryOrient == CounterClockwise) {
       if (boundaryExt.Contains(&m_polyObjExt))
           return TRUE;
       else {
           OpsDoublePoint vertex(boundaryVert.x, boundaryVert.y);
           return !PointInPolygon(vertex);
       }
    }

    // else the boundary has clockwise orientation, and may be a boundary of
    // a hole in the buffer zone - accept if the boundary vertex is not contain-
    // ed in the input polygon object, and the interior point is not within the
    // offset distance of any of the input polygons

    else {
        OpsDoublePoint vertex(boundaryVert.x, boundaryVert.y);
        if (PointInPolygon(vertex))
            return FALSE;

        for (int i = 0, j = 0; i < m_nPolyObjects; i++) {
            if(PointWithinOffsetDist(&m_pVertices[j], m_pnPolyVerts[i],
                    interiorPt))
                return FALSE;

            j += m_pnPolyVerts[i];
        }

        return TRUE;
    }
} // end: AcceptBoundary()


//------------------------------------------------------------------------------
//
// METHOD: PointInPolygon().
//
// PURPOSE: Test a point to determine if it is contained within the polygon
//          object specified at construction time.
//
// PARAMETERS:
//
//     Input:
//
//         point - passes a reference to a OpsDoublePoint containing the coord-
//                 inates of the point to test.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the point is contained within the input polygon, and FALSE
//          otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL PolygonBuffer::PointInPolygon(const OpsDoublePoint &point) const
{
    // sum the winding numbers over all polygon boundaries

    int windingNumber = 0;

    for (int i = 0, j = 0; i < m_nPolyObjects; i++) {
        // check that the number of vertices in the polygon is greater than 2.
        // On occaision, the code in the constructor that culls redundant points
        // may cause a degenerate boundary to collapse into a line segment or
        // a point. We need to avoid passing the WindingNumber() method  degen-
        // erate input since it assumes a closed polygon as input

        if (m_pnPolyVerts[i] > 2) {
            windingNumber += BufferUtility::WindingNumber(&m_pVertices[j],
                m_pnPolyVerts[i], point);
            j += m_pnPolyVerts[i];
        }
    }

    // if the winding number is odd then the point is contained (according to
    // the even-odd rule)

    return static_cast<BOOL>(windingNumber % 2);
} // end: PointInPolygon()


//------------------------------------------------------------------------------
//
// Implementation of the vertex accessor objects.
//
//------------------------------------------------------------------------------

PolygonBuffer::OpsPolygonVertexAccessor::OpsPolygonVertexAccessor(
    const OpsPolygon &polygon) :
    polygon(polygon)
{
}

int PolygonBuffer::OpsPolygonVertexAccessor::GetTotalVertices() const
{
    return polygon.GetNVertices();
}

int PolygonBuffer::OpsPolygonVertexAccessor::GetNPolyObjects() const
{
    return 1;
}

int PolygonBuffer::OpsPolygonVertexAccessor::GetNPolyVerts(int index) const
{
    assert(index == 0);
    return polygon.GetNVertices();
}

const OpsFloatPoint &PolygonBuffer::OpsPolygonVertexAccessor::operator[](
    int index) const
{
    return polygon[index];
}

void PolygonBuffer::OpsPolygonVertexAccessor::GetExtent(
    OpsFloatExtent &extent) const
{
    polygon.GetExtent(&extent);
}

PolygonBuffer::OpsPolyPolygonVertexAccessor::OpsPolyPolygonVertexAccessor(
    const OpsPolyPolygon &polyPolygon) : polyPolygon(polyPolygon)
{
}

int PolygonBuffer::OpsPolyPolygonVertexAccessor::GetTotalVertices() const
{
    return polyPolygon.GetTotalVertices();
}

int PolygonBuffer::OpsPolyPolygonVertexAccessor::GetNPolyObjects() const
{
    return polyPolygon.GetNPolygons();
}

int PolygonBuffer::OpsPolyPolygonVertexAccessor::GetNPolyVerts(int index) const
{
    return polyPolygon.GetNPolyVerts(index);
}

const OpsFloatPoint &PolygonBuffer::OpsPolyPolygonVertexAccessor::operator[](
    int index) const
{
    return polyPolygon[index];
}

void PolygonBuffer::OpsPolyPolygonVertexAccessor::GetExtent(
    OpsFloatExtent &extent) const
{
    polyPolygon.GetExtent(&extent);
}
