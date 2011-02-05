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
// FILE: bufrutil.cpp.
//
// PURPOSE: Implementation of the BufferUtility class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

const double BufferUtility::TwoPi = 2.0 * ::acos(-1.0);
const double BufferUtility::MinHookEdgeLength = 5.0;
const int BufferUtility::MaxEdgesPerChain = 1024;

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize a BufferUtility object.
//
// PARAMETERS:
//
//     Input:
//
//         nSegsPerCircle - passes the number of segments to use in the poly-
//                          gonization of a complete circle; for reasons pecu-
//                          liar to the generation of bufferzones, this number
//                          is best given as a multiple of 4 (if fact this cond-
//                          ition is enforced).
//         offsetDist     - passes the buffer offset distance.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BufferUtility::BufferUtility(float fOffsetDist) :
    m_pfBufferVerts(NULL),
    m_offsetDist(fOffsetDist),
    m_hookEdgeLength(MinHookEdgeLength)
{

} // end: constructor

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize a BufferUtility object.
//
// PARAMETERS:
//
//     Input:
//
//         nSegsPerCircle - passes the number of segments to use in the poly-
//                          gonization of a complete circle; for reasons pecu-
//                          liar to the generation of bufferzones, this number
//                          is best given as a multiple of 4 (if fact this cond-
//                          ition is enforced).
//         offsetDist     - passes the buffer offset distance.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BufferUtility::BufferUtility(int nSegsPerCircle, float offsetDist) :
    m_pfBufferVerts(NULL),
    m_offsetDist(offsetDist),
    m_hookEdgeLength(MinHookEdgeLength)
{
    Initialize(nSegsPerCircle);

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

BufferUtility::~BufferUtility()
{
    delete [] m_pfBufferVerts;

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
//         nSegsPerCircle - passes the number of segments to use in the poly-
//                          gonization of a complete circle; for reasons pecu-
//                          liar to the generation of bufferzones, this number
//                          is best given as a multiple of 4 (if fact this cond-
//                          ition is enforced).
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void BufferUtility::Initialize(int nSegsPerCircle)
{
    // No use of the Line-By-Line Buffer Offset Chain algorithm by default
    // since it is slow.
    m_bCreatOffsetChainLBL = FALSE;

    m_nSegmentsPerCircle = nSegsPerCircle;

    // force the number of segments to be a multiple of 4

    if (m_nSegmentsPerCircle < 4)
        m_nSegmentsPerCircle = 4;
    else if (m_nSegmentsPerCircle % 4 != 0)
        m_nSegmentsPerCircle += 4 - (m_nSegmentsPerCircle % 4);

    // precompute arc polygonization parameters

    m_dDeltaThetaRad = TwoPi / m_nSegmentsPerCircle;
    m_cosDeltaTheta = ::cos(m_dDeltaThetaRad);
    m_sinDeltaTheta = ::sin(m_dDeltaThetaRad);
} // end: Initialize()

//------------------------------------------------------------------------------
//
// METHOD: InitPointLine().
//
// PURPOSE: Allocate buffer vertices storage space for point/line primitives.
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

void BufferUtility::InitPointLine()
{
    delete [] m_pfBufferVerts;

    // allocate a buffer used to generate point/line segment buffers

    m_pfBufferVerts = new OpsFloatPoint[2 * m_nSegmentsPerCircle];
} // end: InitPointLine()

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

void BufferUtility::InitPolyObject()
{
    delete [] m_pfBufferVerts;

    // allocate the buffer used by CreateConvexOffsetChains() - note that the
    // buffer is given an "overrun" area at the end - this helps simplify the
    // logic in CreateConvexOffsetChains()

    m_pfBufferVerts = new OpsFloatPoint[MaxEdgesPerChain + 2 * m_nSegmentsPerCircle];

    // compute the hook edge length - the computed length is taken to be 1.5
    // times the maximum error resulting from polygonization of circular arcs

    assert(m_nSegmentsPerCircle >= 4);
    double hookLength = 1.5 * m_offsetDist *
        (1.0 - ::cos(::acos(-1.0) / m_nSegmentsPerCircle));

    if (hookLength > MinHookEdgeLength)
        m_hookEdgeLength = hookLength;
} // end: InitPolyObject()

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

void BufferUtility::CreatePointBuffer(const OpsFloatPoint &point,
    OrientedPolyPolygon &bufferZone)
{
    InitPointLine();

    PolygonizeCircle(point, m_pfBufferVerts);

    OpsFloatExtent bufferExtent;
    int nVertices = m_nSegmentsPerCircle + 1;
    GetExtentOfPoints(m_pfBufferVerts, nVertices, bufferExtent);

    bufferZone.AddBoundary(m_pfBufferVerts, nVertices, bufferExtent);

} // end: CreatePointBuffer()


//------------------------------------------------------------------------------
//
// METHOD: CreateLineSegmentBuffer().
//
// PURPOSE: Create a line segment buffer zone.
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

void BufferUtility::CreateLineSegmentBuffer(const OpsFloatPoint endPoints[],
    OrientedPolyPolygon &bufferZone)
{
    OpsDoublePoint offsetVector;
    GetOffsetVector(endPoints[0], endPoints[1], offsetVector);

    OpsFloatPoint arcStartPt(static_cast<float>(endPoints[1].x + offsetVector.x),
        static_cast<float>(endPoints[1].y+offsetVector.y));
    OpsFloatPoint arcEndPt(static_cast<float>(endPoints[1].x - offsetVector.x),
        static_cast<float>(endPoints[1].y - offsetVector.y));

    int nArcVertices1;
    PolygonizeCircularArc(arcStartPt, arcEndPt, endPoints[1], m_pfBufferVerts,
        nArcVertices1);

    GetOffsetVector(endPoints[1], endPoints[0], offsetVector);

    arcStartPt.x = static_cast<float>(endPoints[0].x + offsetVector.x);
    arcStartPt.y = static_cast<float>(endPoints[0].y + offsetVector.y);
    arcEndPt.x = static_cast<float>(endPoints[0].x - offsetVector.x);
    arcEndPt.y = static_cast<float>(endPoints[0].y - offsetVector.y);

    int nArcVertices2;
    PolygonizeCircularArc(arcStartPt, arcEndPt, endPoints[0],
        &m_pfBufferVerts[nArcVertices1], nArcVertices2);

    int nVertices = nArcVertices1 + nArcVertices2;
    m_pfBufferVerts[nVertices++] = m_pfBufferVerts[0];

    OpsFloatExtent bufferExtent;
    GetExtentOfPoints(m_pfBufferVerts, nVertices, bufferExtent);

    bufferZone.AddBoundary(m_pfBufferVerts, nVertices, bufferExtent);

} // end: CreateLineSegmentBuffer()


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

void BufferUtility::PolygonizeCircle(const OpsFloatPoint &center,
    OpsFloatPoint vertices[]) const
{
    // polygonize the top half of the circle

    int i;
    int nSegsPerSemiCircle = m_nSegmentsPerCircle / 2;
    double x = m_offsetDist;
    double y = 0.0;

    vertices[0].x = static_cast<float>(x + center.x);
    vertices[0].y = center.y;

    for (i = 1; i < nSegsPerSemiCircle; i++) {
        double temp = x;
        x = x * m_cosDeltaTheta - y * m_sinDeltaTheta;
        y = temp * m_sinDeltaTheta + y * m_cosDeltaTheta;
        vertices[i].x = static_cast<float>(x + center.x);
        vertices[i].y = static_cast<float>(y + center.y);
    }

    vertices[i].x = static_cast<float>(center.x - m_offsetDist);
    vertices[i++].y = center.y;

    // now reflect the vertices in the top half about the line y = center.y
    // to derive the polygonized version of the bottom half (this step
    // should help to reduce the number of event intervals generated during a
    // plane sweep!)

    int j = nSegsPerSemiCircle - 1;

    for (; i < m_nSegmentsPerCircle; i++, j--) {
        vertices[i].x = vertices[j].x;
        vertices[i].y = center.y - (vertices[j].y - center.y);
    }

    // explicitly close off the polygonized circle

    vertices[i] = vertices[0];
    assert(i == m_nSegmentsPerCircle);

} // end: PolygonizeCircle()


//------------------------------------------------------------------------------
//
// METHOD: PolygonizeCircularArc().
//
// PURPOSE: Generate the polygonal approximation to the specified circular arc.
//          The arc is assumed to be specified in a counter-clockwise fashion.
//
// PARAMETERS:
//
//     Input:
//
//         startPt   - passes a reference to an OpsFloatPoint containing the
//                     coordinates of the starting point of the circular arc.
//         endPt     - passes a reference to an OpsFloatPoint containing the
//                     coordinates of the ending point of the circular arc.
//         center    - passes a reference to an OpsFloatPoint containing the
//                     center of the circle the arc is embedded in.
//
//     Output:
//
//         vertices  - passes a pointer to an array OpsFloatPoint object. On
//                     return, the array contains the vertices of the polygonal
//                     approximation to the circular arc. The array is assumed
//                     to be large enough to hold the vertices.
//         nVertices - passes a reference to an int. The number of vertices is
//                     copied to the referenced int.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void BufferUtility::PolygonizeCircularArc(const OpsFloatPoint &startPt,
    const OpsFloatPoint &endPt, const OpsFloatPoint &center,
    OpsFloatPoint vertices[], int &nVertices) const
{
    // get the start and end angles in radians

    double startAngle = ::atan2(startPt.y - center.y, startPt.x - center.x);
    double endAngle = ::atan2(endPt.y - center.y, endPt.x - center.x);

    // normalize to range 0 to 2 pi

    if (startAngle < 0.0)
        startAngle += TwoPi;

    if (endAngle < 0.0)
        endAngle += TwoPi;

    // make sure end angle is greater than start angle

    if (endAngle < startAngle)
        endAngle += TwoPi;

    // compute the angle subtended by the arc

    double angle = ::ceil(startAngle / m_dDeltaThetaRad) * m_dDeltaThetaRad;

    // make sure we don't redundantly compute the start point

    if (angle == startAngle)
        angle += m_dDeltaThetaRad;

    vertices[0] = startPt;
    nVertices = 1;

    // if angle is less than endAngle, then polygonize the arc (else
    // a single edge from startPt to endPt is all that is required)

    if (angle < endAngle) {
        double x = m_offsetDist * ::cos(angle);
        double y = m_offsetDist * ::sin(angle);

        vertices[nVertices].x = static_cast<float>(x + center.x);
        vertices[nVertices++].y = static_cast<float>(y + center.y);

        angle += m_dDeltaThetaRad;

        while (angle < endAngle) {
            double temp = x;
            x = x * m_cosDeltaTheta - y * m_sinDeltaTheta;
            y = temp * m_sinDeltaTheta + y * m_cosDeltaTheta;

            vertices[nVertices].x = static_cast<float>(x + center.x);
            vertices[nVertices++].y = static_cast<float>(y + center.y);

            angle += m_dDeltaThetaRad;
        }
    }

    assert(nVertices <= m_nSegmentsPerCircle);

    // don't redundantly add in the arc end point

    if (vertices[nVertices-1] != endPt)
        vertices[nVertices++] = endPt;

} // end: PolygonizeCircularArc()


//------------------------------------------------------------------------------
//
// METHOD: GetUnitOffsetVector().
//
// PURPOSE: Get the unit offset vector for the specified line segment.
//
// PARAMETERS:
//
//     Input:
//
//         endPt1           - passes a reference to the OpsFloatPoint containing
//                            coordinates of the first endpoint of the line
//                            segment.
//         endPt2           - passes a reference to the OpsFloatPoint containing
//                            coordinates of the second endpoint of the line
//                            segment.
//
//     Output:
//
//         unitOffsetVector - passes a reference to a OpsDoublePoint. The
//                            endpoint coordinates of the unit offset vector are
//                            copied to the referenced object. Note that the
//                            offset vector is oriented so that it points to the
//                            right of the directed line defined by endPt1->
//                            endPt2.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void BufferUtility::GetUnitOffsetVector(const OpsFloatPoint &endPt1,
    const OpsFloatPoint &endPt2, OpsDoublePoint &unitOffsetVector) const
{
    // generate unit offset vector to the right of the segment

    double deltaX = endPt2.x - endPt1.x;
    double deltaY = endPt2.y - endPt1.y;
    double segmentLength = ::sqrt(deltaX * deltaX + deltaY * deltaY);
    assert(segmentLength > 0.0);

    unitOffsetVector.x = deltaY / segmentLength;
    unitOffsetVector.y = -deltaX / segmentLength;

} // end: GetUnitOffsetVector()


//------------------------------------------------------------------------------
//
// METHOD: GetOffsetVector().
//
// PURPOSE: Get the offset vector for the specified line segment using the
//          offset distance specified at construction time.
//
// PARAMETERS:
//
//     Input:
//
//         endPt1       - passes a reference to the OpsFloatPoint containing
//                        coordinates of the first endpoint of the line
//                        segment.
//         endPt2       - passes a reference to the OpsFloatPoint containing
//                        coordinates of the second endpoint of the line
//                        segment.
//
//     Output:
//
//         offsetVector - passes a reference to a OpsDoublePoint. The endpoint
//                        coordinates of the offset vector are copied to the
//                        referenced object. Note that the offset vector is
//                        oriented so that it points to the right of the
//                        directed line defined by endPt1->endPt2.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void BufferUtility::GetOffsetVector(const OpsFloatPoint &endPt1,
    const OpsFloatPoint &endPt2, OpsDoublePoint &offsetVector) const
{
    // generate the unit offset vector to the right of the segment, and scale
    // it using the offset distance

    GetUnitOffsetVector(endPt1, endPt2, offsetVector);
    offsetVector.x *= m_offsetDist;
    offsetVector.y *= m_offsetDist;

} // end: GetOffsetVector()


//------------------------------------------------------------------------------
//
// METHOD: LineSide().
//
// PURPOSE: Determine which side of a directed line a specified point lies on.
//
// PARAMETERS:
//
//     Input:
//
//         endPoint1 - passes a reference to the OpsDoublePoint containing the
//                     coordinates of the first end point of the line segment
//                     defining the directed line.
//         endPoint2 - passes a reference to the OpsDoublePoint containing the
//                     coordinates of the second end point of the line segment
//                     defining the directed line (the line is directed from
//                     endPoint1 to endPoint2).
//         point     - passes a reference to the OpsDoublePoint containing the
//                     coordinates of the point to be tested.
//
//     Output:
//
//         None.
//
// RETURNS: An enumerated value of type BufferUtility::SideOfLine specifying the
//          side of the directed line 'point' lies to.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BufferUtility::SideOfLine BufferUtility::LineSide(const OpsDoublePoint &fromPt,
    const OpsDoublePoint &toPt, const OpsDoublePoint &pt)
{
    double signedCrossProd = (toPt.x - fromPt.x) * (pt.y - fromPt.y) -
                             (pt.x - fromPt.x) * (toPt.y - fromPt.y);

    if (signedCrossProd > 0.0)
        return LeftOfLine;
    else if (signedCrossProd < 0.0)
        return RightOfLine;
    else
        return OnTheLine;

} // end: LineSide()


//------------------------------------------------------------------------------
//
// METHOD: WindingNumber().
//
// PURPOSE: Compute the winding number of a point with respect to a polygon
//          boundary.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes an array of the polygon boundary vertices. Note
//                     that the polygon is assumed to be explicitly closed.
//         nVertices - passes the number of vertices
//         point     - passes a reference to a OpsDoublePoint. The referenced
//                     structure contains the coordinates of the point the
//                     winding number is to be computed for.
//     Output:
//
//         None.
//
// RETURNS: The winding number of the point with respect to the polygon.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int BufferUtility::WindingNumber(const OpsFloatPoint vertices[], int nVertices,
    const OpsDoublePoint &point)
{
    // the polygon boundary must be explicitly closed

    assert(nVertices > 2);
    int nEdges = 0;

    // Remember an extra point has been added for polygons.
    if ( vertices[0] == vertices[nVertices-1])
        nEdges = nVertices-1;
    else if ( vertices[0] == vertices[nVertices-2])
        nEdges = nVertices-2;
    else
        assert(vertices[0] == vertices[nEdges]);

    // loop through the polygon edges incrementing/decrementing the winding
    // number according to the following rules. If the horizontal ray drawn
    // from the point to +infinity crosses an edge at any point other than
    // its maximum in y, then increment or decrement the winding number
    // depending on whether the edge crosses the ray from below to above,
    // or above to below

    int windNumber = 0;

    for (int i = 0; i < nEdges; i++) {
        OpsDoublePoint endPt1(vertices[i].x, vertices[i].y);
        OpsDoublePoint endPt2(vertices[i+1].x, vertices[i+1].y);

        double yMin = Min(endPt1.y, endPt2.y);

        if (yMin < point.y) {
            double yMax = Max(endPt1.y, endPt2.y);

            if (yMax >= point.y) {
                if (yMin == endPt1.y) {
                    if (LineSide(endPt1, endPt2, point) != LeftOfLine)
                        windNumber++;
                }
                else if (LineSide(endPt1, endPt2, point) != RightOfLine)
                    windNumber--;
            }
        }
    }

    return windNumber;

} // end: WindingNumber()


//------------------------------------------------------------------------------
//
// METHOD: DistFromPointToLineSeg().
//
// PURPOSE: Compute the distance between a point and a line segment.
//
// PARAMETERS:
//
//     Input:
//
//         endPoints - passes an array containing the end points of the line
//                     segment.
//         point     - passes a reference to a OpsDoublePoint containing the
//                     coordinates of the point.
//
//     Output:
//
//         None.
//
// RETURNS: The distance between the point and the line segment.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double BufferUtility::DistFromPointToLineSeg(const OpsFloatPoint endPoints[],
    const OpsDoublePoint &point)
{
    // compute the length of the x and y intervals spanned by the line segment

    double deltaX = static_cast<double>(endPoints[1].x) -
                    static_cast<double>(endPoints[0].x);
    double deltaY = static_cast<double>(endPoints[1].y) -
                    static_cast<double>(endPoints[0].y);

    // if the segment is degenerate use the distance between the point and the
    // first endpoint of the degenerate segment

    if (deltaX == 0.0 && deltaY == 0.0)
        return DistBetweenPoints(endPoints[0], point);

    // else translate to the origin, and rotate to horizontal, then compute
    // the shortest distance between the point and the line segment

    else {
        double theta = ::atan2(deltaY, deltaX);
        double cosTheta = ::cos(theta);
        double sinTheta = ::sin(theta);

        double x1 = point.x - static_cast<double>(endPoints[0].x);
        double y1 = point.y - static_cast<double>(endPoints[0].y);
        double minusX1 = -x1;

        x1 = x1 * cosTheta + y1 * sinTheta;

        if (x1 < 0.0)
            return DistBetweenPoints(endPoints[0], point);

        double x2 = deltaX * cosTheta + deltaY * sinTheta;

        if (x1 > x2)
            return DistBetweenPoints(endPoints[1], point);
        else
            return ::fabs(minusX1 * sinTheta + y1 * cosTheta);
    }

} // end: DistFromPointToLineSeg()


//------------------------------------------------------------------------------
//
// METHOD: DistBetweenPointsSqrd().
//
// PURPOSE: Compute the square of the distance between two points.
//
// PARAMETERS:
//
//     Input:
//
//         point1 - passes a reference to an OpsFloatPoint containing the
//                  coordinates of the first point.
//         point2 - passes a reference to an OpsDoublePoint containing the
//                  coordinates of the second point.
//
//     Output:
//
//         None.
//
// RETURNS: The square distance between the two points.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double BufferUtility::DistBetweenPointsSqrd(const OpsDoublePoint &point1,
    const OpsDoublePoint &point2)
{
    double deltaX = point1.x - point2.x;
    double deltaY = point1.y - point2.y;

    return deltaX * deltaX + deltaY * deltaY;

} // end: DistBetweenPointsSqrd()


//------------------------------------------------------------------------------
//
// METHOD: GetExtentOfPoints().
//
// PURPOSE: Get the extent of a list of points.
//
// PARAMETERS:
//
//     Input:
//
//         points  - passes a pointer to the array of points the extent is to
//                   be derived for.
//         nPoints - passes the number of points in the array.
//
//     Output:
//
//         extent  - passes a reference to an OpsFloatExtent structure; the
//                   extent of the points is copied to the referenced object.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void BufferUtility::GetExtentOfPoints(const OpsFloatPoint points[], int nPoints,
    OpsFloatExtent &extent)
{
    assert(nPoints > 0);

    extent.Initialize(&points[0]);

    for (int i = 1; i < nPoints; i++)
        extent.Update(&points[i]);

} // end: GetExtentOfPoints()


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
//         vertex1 - passes a reference to the OpsFloatPoint containing the
//                   coordinates of the first vertex of the first line segment.
//         vertex2 - passes a reference to the OpsFloatPoint containing the
//                   shared vertex (the second vertex of the first line segment,
//                   and the first vertex of the second line segment).
//         vertex3 - passes a reference to the OpsFloatPoint containing the
//                   coordinates of the second vertex of the second line
//                   segment.
//
//     Output:
//
//         None.
//
// RETURNS: An enumerated value of type TurnType specifying the direction
//          of turn at vertex2.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BufferUtility::TurnType BufferUtility::GetTurnType(const OpsFloatPoint &vertex1,
    const OpsFloatPoint &vertex2, const OpsFloatPoint &vertex3)
{
    // test which side of the directed line vertex1->vertex2 that vertex3
    // lies on

    OpsDoublePoint endPoint1(vertex1.x, vertex1.y);
    OpsDoublePoint endPoint2(vertex2.x, vertex2.y);
    OpsDoublePoint point(vertex3.x, vertex3.y);

    SideOfLine side = LineSide(endPoint1, endPoint2, point);

    // if vertex3 is left/right of the line then a left/right turn is made at
    // vertex2

    if (side == BufferUtility::LeftOfLine)
        return LeftTurn;
    else if (side == BufferUtility::RightOfLine)
        return RightTurn;

    // else either no turn has been made, or the second line doubles back over
    // the first

    else {
        // compute delta X (or Y) for the lines

        double delta1 = endPoint2.x - endPoint1.x;
        double delta2;

        if (delta1 != 0.0)
            delta2 = point.x - endPoint2.x;
        else {
            delta1 = endPoint2.y - endPoint1.y;
            delta2 = point.y - endPoint2.y;
        }

        // if the delta's have the same sign then no turn is made at vertex2

        if (delta1 * delta2 >= 0.0)
            return NoTurn;

        // otherwise, the second line doubles back on the first

        else
            return DoublesBack;
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
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the offset chain edges.
//
//------------------------------------------------------------------------------

void BufferUtility::CreateOffsetChains(const OpsFloatPoint vertices[],
    int nVertices, vector<OpsFloatPoint*> &vChainEdges, vector<int> &vChainEdgesCount)
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
        for (int i = 0, j; i < nVertices - 1; i = j + 1)
        {
            // scan the line for adjacent segments that double back on each
            // other (need to prevent this type of polyobject from being passed
            // to CreateConvexOffsetChains() since it creates problems deciding
            // how to compute the offset chains)

            for (j = i; j < nVertices - 2; j++)
            {
                if (GetTurnType(vertices[j], vertices[j+1],
                        vertices[j+2]) == DoublesBack)
                {
                    break;
                }
            }

            // create the left-convex offset chains

            CreateConvexOffsetChains(&vertices[i], j + 2 - i);
        }
    }

    // else if there is only a single vertex (this may have resulted from
    // culling redundant points), then treat it as a point

    else if (nVertices == 1)
    {
        PolygonizeCircle(vertices[0], m_pfBufferVerts);
        AddChainEdges(m_pfBufferVerts, m_nSegmentsPerCircle + 1);
    }

} // end: CreateOffsetChains()


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
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the offset chain edges.
//
//------------------------------------------------------------------------------

void BufferUtility::CreateConvexOffsetChains(const OpsFloatPoint vertices[],
    int nVertices)
{
    // walk forward along the sequence of polyobject vertices, generating convex
    // offset chains to the right

    int i, j, nArcVertices;
    TurnType turn;
    OpsDoublePoint offsetVector;
    OpsFloatPoint arcStartPoint, arcEndPoint;

    GetOffsetVector(vertices[0], vertices[1], offsetVector);
    m_pfBufferVerts[0].x = static_cast<float>(vertices[0].x + offsetVector.x);
    m_pfBufferVerts[0].y = static_cast<float>(vertices[0].y + offsetVector.y);

    for (i = 0, j = 1; i < nVertices - 2; i++) {
        // determine which way the line turns at the next vertex

        turn = GetTurnType(vertices[i], vertices[i+1], vertices[i+2]);

        // if a left turn is made, then polygonize the arc centered on the
        // vertex where the turn is made, and continue generating the convex
        // offset chain

        if (turn == LeftTurn) {
            arcStartPoint.x = static_cast<float>(vertices[i+1].x + offsetVector.x);
            arcStartPoint.y = static_cast<float>(vertices[i+1].y + offsetVector.y);
            GetOffsetVector(vertices[i+1], vertices[i+2],
                offsetVector);
            arcEndPoint.x = static_cast<float>(vertices[i+1].x + offsetVector.x);
            arcEndPoint.y = static_cast<float>(vertices[i+1].y + offsetVector.y);
            PolygonizeCircularArc(arcStartPoint, arcEndPoint,
                vertices[i+1], &m_pfBufferVerts[j], nArcVertices);
            j += nArcVertices;
        }

        // else if a right turn was made, drop a hook edge, and advance to
        // the next segment

        else if (turn == RightTurn) {
            m_pfBufferVerts[j].x = static_cast<float>(vertices[i+1].x + offsetVector.x);
            m_pfBufferVerts[j++].y = static_cast<float>(vertices[i+1].y + offsetVector.y);
            GetHookEdge(vertices[i], vertices[i+1], FALSE, offsetVector,
                m_pfBufferVerts[j++]);
        }

        // if a right turn was made, or the offset chain has grown too large
        // then add the edges in the offset chain, and start a new chain

        if (turn == RightTurn || j >= MaxEdgesPerChain) {
            AddChainEdges(m_pfBufferVerts, j);
            GetOffsetVector(vertices[i+1], vertices[i+2],
                offsetVector);
            GetHookEdge(vertices[i+1], vertices[i+2], TRUE, offsetVector,
                m_pfBufferVerts[0]);
            m_pfBufferVerts[1].x = static_cast<float>(vertices[i+1].x + offsetVector.x);
            m_pfBufferVerts[1].y = static_cast<float>(vertices[i+1].y + offsetVector.y);
            j = 2;
        }
    }

    // add semicircular cap centered at the last vertex of the input polyobject

    arcStartPoint.x = static_cast<float>(vertices[nVertices-1].x + offsetVector.x);
    arcStartPoint.y = static_cast<float>(vertices[nVertices-1].y + offsetVector.y);
    GetOffsetVector(vertices[nVertices-1],
        vertices[nVertices-2], offsetVector);
    arcEndPoint.x = static_cast<float>(vertices[nVertices-1].x + offsetVector.x);
    arcEndPoint.y = static_cast<float>(vertices[nVertices-1].y + offsetVector.y);
    PolygonizeCircularArc(arcStartPoint, arcEndPoint,
        vertices[nVertices-1], &m_pfBufferVerts[j], nArcVertices);
    AddChainEdges(m_pfBufferVerts, j + nArcVertices);

    // now walk backwards along the poly-object vertices generating offset
    // chains on the other side

    m_pfBufferVerts[0] = arcEndPoint;

    for (i = nVertices - 1, j = 1; i > 1; i--) {
        // determine which way the line turns at the next vertex

        turn = GetTurnType(vertices[i], vertices[i-1], vertices[i-2]);

        // if a left turn is made, then polygonize the arc centered on the
        // vertex where the turn is made, and continue generating the convex
        // offset chain

        if (turn == LeftTurn) {
            arcStartPoint.x = static_cast<float>(vertices[i-1].x + offsetVector.x);
            arcStartPoint.y = static_cast<float>(vertices[i-1].y + offsetVector.y);
            GetOffsetVector(vertices[i-1], vertices[i-2],
                offsetVector);
            arcEndPoint.x = static_cast<float>(vertices[i-1].x + offsetVector.x);
            arcEndPoint.y = static_cast<float>(vertices[i-1].y + offsetVector.y);
            PolygonizeCircularArc(arcStartPoint, arcEndPoint,
                vertices[i-1], &m_pfBufferVerts[j], nArcVertices);
            j += nArcVertices;
        }

        // else advance one vertex, and compute the offset vector for the next
        // line segment

        else if (turn == RightTurn) {
            m_pfBufferVerts[j].x = static_cast<float>(vertices[i-1].x + offsetVector.x);
            m_pfBufferVerts[j++].y = static_cast<float>(vertices[i-1].y + offsetVector.y);
            GetHookEdge(vertices[i], vertices[i-1], FALSE, offsetVector,
                m_pfBufferVerts[j++]);
        }

        // if a right turn was made, or the offset chain has grown too large
        // then add the edges in the offset chain, and start a new chain

        if (turn == RightTurn || j >= MaxEdgesPerChain) {
            AddChainEdges(m_pfBufferVerts, j);

            GetOffsetVector(vertices[i-1], vertices[i-2],
                offsetVector);
            GetHookEdge(vertices[i-1], vertices[i-2], TRUE, offsetVector,
                m_pfBufferVerts[0]);
            m_pfBufferVerts[1].x = static_cast<float>(vertices[i-1].x + offsetVector.x);
            m_pfBufferVerts[1].y = static_cast<float>(vertices[i-1].y + offsetVector.y);
            j = 2;
        }
    }

    // add semicircular cap centered at the first vertex of the input polyobject

    arcStartPoint.x = static_cast<float>(vertices[0].x + offsetVector.x);
    arcStartPoint.y = static_cast<float>(vertices[0].y + offsetVector.y);
    GetOffsetVector(vertices[0], vertices[1], offsetVector);
    arcEndPoint.x = static_cast<float>(vertices[0].x + offsetVector.x);
    arcEndPoint.y = static_cast<float>(vertices[0].y + offsetVector.y);
    PolygonizeCircularArc(arcStartPoint, arcEndPoint,
        vertices[0], &m_pfBufferVerts[j], nArcVertices);
    AddChainEdges(m_pfBufferVerts, j + nArcVertices);

} // end: CreateConvexOffsetChains()


//------------------------------------------------------------------------------
//
// METHOD: GetHookEdge().
//
// PURPOSE: Compute the coordinates of the "hook" edge for the specified line
//          segment. Hook edges are dropped from the offset segment toward the
//          corresponding line segment, to ensure that the offset segments form
//          a closed outer boundary. There two cases (that I know of) where
//          these hook edges become necessary:
//
//          1) Due to the rounding from double to float, adjacent segments cent-
//             ered on a vertex where a right turn is made may not intersect,
//             leaving a gap in the outer boundary (this is particularly the
//             case when the right turn is made at a very shallow angle); the
//             use of hook edges prevents this type of problem.
//
//          2) When the offset distance is large with respect to the buffered
//             object, the error introduced by polygonizing circular arcs can
//             become signifigant. Offset arcs and polygonized circular arcs
//             may fail to intersect; The introduction of a hook edge forces
//             a point of intersection with one of the edges of the polygonized
//             circular arc (this one is a little hard to visualize - you pretty
//             much have to see it happen).
//
//
// PARAMETERS:
//
//     Input:
//
//         endPt1        - passes a reference to the OpsFloatPoint containing
//                         the coordinates of the first end point of the line
//                         segment defining the directed line.
//         endPt2        - passes a reference to the OpsFloatPoint containing
//                         the coordinates of the second end point of the line
//                         segment defining the directed line (the line is
//                         directed from endPt1 to endPt2).
//         toEndPt1      - passes TRUE if the hook edges should be dropped
//                         toward endPt1, and FALSE if it should be dropped
//                         toward endPt2.
//         offsetVector  - passes a reference to a OpsDoublePoint. The
//                         referenced object contains the endpoint coordinates
//                         of the offset vector.
//
//     Output:
//
//         hookEdgeEndPt - passes a reference to an OpsFloatPoint. The endpoint
//                         coordinates of the hook edge are copied to the
//                         referenced object.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void BufferUtility::GetHookEdge(const OpsFloatPoint &endPt1,
    const OpsFloatPoint &endPt2, BOOL toEndPt1,
    const OpsDoublePoint &offsetVector,
    OpsFloatPoint &hookEdgeEndPt) const
{
    // initialize a reference to the segment endpoint the hook edge is to be
    // dropped toward

    const OpsFloatPoint &segEndPt = toEndPt1 ? endPt1 : endPt2;

    // if the offset distance is greater than the hook edge length, then
    // compute the end point hook edge endpoint

    if (GetOffsetDistance() > m_hookEdgeLength) {
        OpsDoublePoint unitVector;
        GetUnitOffsetVector(endPt1, endPt2, unitVector);
        hookEdgeEndPt.x = static_cast<float>(segEndPt.x + offsetVector.x -
            (unitVector.x * m_hookEdgeLength));
        hookEdgeEndPt.y = static_cast<float>(segEndPt.y + offsetVector.y -
            (unitVector.y * m_hookEdgeLength));
    }

    // else just set the hook edge endpoint to be the same as the segment
    // endpoint it was to be dropped toward

    else
        hookEdgeEndPt = segEndPt;

} // end: GetHookEdge()

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

void BufferUtility::AddChainEdges(const OpsFloatPoint vertices[], int nVertices)
{
    OpsFloatPoint *fEdgeVerts = new OpsFloatPoint[nVertices];

    // Copy vertices over.

    for (int i = 0; i < nVertices; ++i)
    {
        fEdgeVerts[i] = vertices[i];
    }

    m_vChainEdges->push_back(fEdgeVerts);
    m_vChainEdgesCount->push_back(nVertices);
}
