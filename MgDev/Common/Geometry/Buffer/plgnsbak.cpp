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
// FILE: plgnsbak.cpp.
//
// PURPOSE: Implementation of the PolygonSetback class.
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
// PURPOSE: Create and initialize a PolygonSetback object.
//
// PARAMETERS:
//
//     Input:
//
//         polygonObj     - passes a reference to the polygon object that the
//                          setback is to be generated for.
//         pBufferUtil    - passes the number of segments that would be used in
//                          the polygonization of a complete circle, and also
//                          the buffer offset distance.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the PolygonSetback object.
//
//------------------------------------------------------------------------------

//PolygonSetback::PolygonSetback(const PolygonObject &polygonObj, BufferUtility *pBufferUtil) :
//    PolygonBuffer(polygonObj, pBufferUtil)
//{
//} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Create and initialize a PolygonSetback object.
//
// PARAMETERS:
//
//     Input:
//
//         polygon        - passes a reference to the OpsPolygon object that
//                          the setback is to be generated for.
//         pBufferUtil    - passes the number of segments that would be used in
//                          the polygonization of a complete circle, and also
//                          the buffer offset distance.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the PolygonSetback object.
//
//------------------------------------------------------------------------------

PolygonSetback::PolygonSetback(const OpsPolygon &polygon, BufferUtility *pBufferUtil) :
    PolygonBuffer(polygon, pBufferUtil)
{
} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Create and initialize a PolygonSetback object.
//
// PARAMETERS:
//
//     Input:
//
//         polyPolygon    - passes a reference to the OpsPolyPolygon object that
//                          the setback is to be generated for.
//         pBufferUtil    - passes the number of segments that would be used in
//                          the polygonization of a complete circle, and also
//                          the buffer offset distance.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the PolygonSetback object.
//
//------------------------------------------------------------------------------

PolygonSetback::PolygonSetback(const OpsPolyPolygon &polyPolygon, BufferUtility *pBufferUtil) :
    PolygonBuffer(polyPolygon, pBufferUtil)
{
} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: CreateSetback().
//
// PURPOSE: Create a setback inside the polygon object specified at construc-
//          tion time.
//
// PARAMETERS:
//
//     Input:
//
//         callback       - passes a pointer to a ProgressCallback object. The
//                          object is periodically notified of progress, and
//                          checked to determine if the setback operation has
//                          been canceled.
//
//     Output:
//
//         setbackPolygon - passes a reference to an OrientedPolyPolygon object.
//                          The boundaries of the setback polygon are copied
//                          to the referenced object.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the setback. A PlaneSweepException is thrown
//             if an error is detected during the execution of the planesweep
//             algorithm.
//
//------------------------------------------------------------------------------

void PolygonSetback::CreateSetback(ProgressCallback &callback,
    OrientedPolyPolygon &setbackPolygon)
{
    // forward call to base class method. Although it may seem a little odd to
    // call the buffering method of the base-class to generate the setback, it
    // is the AcceptBoundary method (below) that determines which boundaries to
    // include in the output that makes it all work

    CreateBufferZone(callback, setbackPolygon);

} // end: CreateSetback()


//------------------------------------------------------------------------------
//
// METHOD: AcceptBoundary().
//
// PURPOSE: Determine whether a potential polygon boundary should be included
//          as part of the setback polygon.
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

BOOL PolygonSetback::AcceptBoundary(const OpsFloatExtent &, Orientation,
    const OpsFloatPoint &boundaryVert, const OpsDoublePoint &interiorPt) const
{
    // first, test the boundary vertex and if it is not contained in the input
    // polygon, then the boundary is not part of the setback polygon - return
    // FALSE

    OpsDoublePoint vertex(boundaryVert.x, boundaryVert.y);

    if (!PointInPolygon(vertex))
        return FALSE;

    // otherwise, test the interior point to ensure that it does not lie within
    // the inset distance of the input polygon boundary

    for (int i = 0, j = 0; i < m_nPolyObjects; i++) {
        if (PointWithinOffsetDist(&m_pVertices[j], m_pnPolyVerts[i], interiorPt))
            return FALSE;

        j += m_pnPolyVerts[i];
    }

    // if we got here, then it must be a legitimate output boundary - return
    // TRUE

    return TRUE;

} // end: AcceptBoundary()


//------------------------------------------------------------------------------
//
// METHOD: GenerateBufferZone().
//
// PURPOSE: Create the buffer zone polygon for the specified poly-object (where
//          a poly-object is a poly-polyline or poly-polygon).
//
// PARAMETERS:
//
//          vertices      - passes an array of the poly-object vertices.
//          nPolyVerts    - passes an array of the counts of the number of
//                          vertices in each sub-object (polyline or polygon).
//          nPolyObjects  - passes the number of sub-objects.
//          callback      - passes a reference to a ProgressCallback object. The
//                          object is periodically notified of progress, and
//                          checked to determine if the buffer operation has
//                          been canceled.
//
//     Output:
//
//          bufferPolygon - passes a reference to an OrientedPolyPolygon object.
//                          The buffer zone polygon containing for the poly-
//                          object is copied to the referenced object.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the buffer zone. A PlaneSweepException is
//             thrown if an error is detected during the execution of the plane
//             sweep algorithm.
//
//------------------------------------------------------------------------------

void PolygonSetback::GenerateBufferZone(const OpsFloatPoint vertices[],
    const int nPolyVerts[], int nPolyObjects, ProgressCallback &callback,
    OrientedPolyPolygon &bufferPolygon)
{
    // We'll get a PlaneSweepException if the setback distance causes the
    // polygon to shrink to zero area.  Catch this and don't pass it on.

    try
    {
        // call the base class first - this will generate an OrientedPolyPolygon
        // with reversed orientation

        PolyObjectBuffer::GenerateBufferZone(vertices, nPolyVerts, nPolyObjects, callback, bufferPolygon);

        // reverse the boundaries / points of the polygon

        bufferPolygon.ReverseBoundaries();
    }
    catch (PlaneSweepException* ex)
    {
        delete ex;
    }

} // end: GenerateBufferZone()
