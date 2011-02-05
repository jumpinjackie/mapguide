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
// FILE: ogeomtry.cpp.
//
// PURPOSE: Implementation of low level geometric functions; these functions are
//          used by the graphic primitive classes in OPS++ to implement the pick
//          methods. In addition, the functions can be used directly by the
//          application.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"
#include <math.h>

const int OPS_FILLRULE_EVEN_ODD  = 0;
//------------------------------------------------------------------------------
//
// FUNCTION: Min().
//
// PURPOSE: Inline method to determine the minimum of two floats.
//
//------------------------------------------------------------------------------

static inline float Min(float x1, float x2)
{
    return x1 <= x2 ? x1 : x2;
}


//------------------------------------------------------------------------------
//
// FUNCTION: Max().
//
// PURPOSE: Inline method to determine the maximum of two floats.
//
//------------------------------------------------------------------------------

static inline float Max(float x1, float x2)
{
    return x1 <= x2 ? x2 : x1;
}


//------------------------------------------------------------------------------
//
// FUNCTION: Square().
//
// PURPOSE: Inline method to compute the square of a number.
//
//------------------------------------------------------------------------------

static inline float Square(float x)
{
    return x * x;
}


//------------------------------------------------------------------------------
//
// FUNCTION: OpsLineSide().
//
// PURPOSE: Determine which side of a directed line (defined by two points)
//          a given point lies on.
//
// PARAMETERS:
//
//     Input:
//
//         endPts - passes the coordinates of the two points that define
//                  the directed line. The line is considered to be directed
//                  from endPt[0] to endPt[1].
//         point  - pass the coordinates of the point to test.
//
//     Output:
//
//         None.
//
// RETURNS: Returns one of the constants OPS_LEFT_OF_LINE, OPS_RIGHT_OF_
//          LINE, or OPS_ON_LINE depending on whether the point (x, y) is
//          left of, right of or on the directed line defined by (endPts[0].x,
//          endPts[0].y), and (endPts[1].x, endPts[1].y).
//
// EXCEPTIONS: None.
//
// ALGORITM: The algorithm is adapted from one described in:
//
//    "Introduction to Algorithms", Cormen, Leiserson, and Rivest,
//    McGraw-Hill, 1990.
//
//------------------------------------------------------------------------------

int OpsLineSide(const OpsFloatPoint endPts[], const OpsFloatPoint *point)
{
    double signedCrossProd =
        (static_cast<double>(endPts[1].x) - static_cast<double>(endPts[0].x)) *
        (static_cast<double>(point->y)    - static_cast<double>(endPts[0].y)) -
        (static_cast<double>(point->x)    - static_cast<double>(endPts[0].x)) *
        (static_cast<double>(endPts[1].y) - static_cast<double>(endPts[0].y));

    if (signedCrossProd > 0.0)
        return(OPS_LEFT_OF_LINE);
    else if (signedCrossProd < 0.0)
        return(OPS_RIGHT_OF_LINE);
    else
        return(OPS_ON_LINE);

} // end: OpsLineSide()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsSegmentsMidpoint().
//
// PURPOSE: Compute the midpoint of a line segment.
//
// PARAMETERS:
//
//     Input:
//
//         endPts  - pass the coordinates of the endpoints of the line segment.
//
//     Output:
//
//        None.
//
// RETURNS: The midpoint of the segment in an OpsFloatPoint.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsFloatPoint OpsSegmentMidpoint(const OpsFloatPoint endPts[])
{
    OpsFloatPoint midPoint((endPts[1].x + endPts[0].x) / 2.0f,
         (endPts[1].y + endPts[0].y) / 2.0f);

    return midPoint;

} // end: OpsSegmentMidpoint()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsLineSegmentsIntersect().
//
// PURPOSE: Determine if two line segments intersect.
//
// PARAMETERS:
//
//     Input:
//
//         endPts1 - pass the coordinates of the endpoints of the first
//                   line segment.
//         endPts2 - pass the coordinates of the endpoints of the second
//                   second line segment.
//
//     Output:
//
//        None.
//
// RETURNS: Returns TRUE if the segments intersect, otherwise returns FALSE.
//
// EXCEPTIONS: None.
//
// ALGORITHM: The algorithm used is adapted from one described in:
//            "Introduction to Algorithms", Cormen, Leiserson, and
//            Rivest, McGraw Hill, 1990..
//
//------------------------------------------------------------------------------

BOOL OpsLineSegmentsIntersect(const OpsFloatPoint endPts1[],
    const OpsFloatPoint endPts2[])
{
    // get min/max x values for both segments

    float xMin1, xMax1, xMin2, xMax2;

    if (endPts1[0].x <= endPts1[1].x) {
        xMin1 = endPts1[0].x;
        xMax1 = endPts1[1].x;
    }
    else {
        xMin1 = endPts1[1].x;
        xMax1 = endPts1[0].x;
    }

    if (endPts2[0].x <= endPts2[1].x) {
        xMin2 = endPts2[0].x;
        xMax2 = endPts2[1].x;
    }
    else {
        xMin2 = endPts2[1].x;
        xMax2 = endPts2[0].x;
    }

    // if the x extents do not overlap then segments don't intersect

    if (xMax1 < xMin2 || xMin1 > xMax2)
        return FALSE;

    // get min/max y values for both segments

    float yMin1, yMax1, yMin2, yMax2;

    if (endPts1[0].y <= endPts1[1].y) {
        yMin1 = endPts1[0].y;
        yMax1 = endPts1[1].y;
    }
    else {
        yMin1 = endPts1[1].y;
        yMax1 = endPts1[0].y;
    }

    if (endPts2[0].y <= endPts2[1].y) {
        yMin2 = endPts2[0].y;
        yMax2 = endPts2[1].y;
    }
    else {
        yMin2 = endPts2[1].y;
        yMax2 = endPts2[0].y;
    }

    // if the y extents do not overlap then the segments don't intersect

    if (yMax1 < yMin2 || yMin1 > yMax2)
        return FALSE;

    // return TRUE iff the segments straddle one another. Note that a segment
    // is considered to straddle the other segment if one of its endpoints lies
    // exactly on the line containing the other segment

    int side = ::OpsLineSide(endPts1, &endPts2[0]);

    if (side != OPS_ON_LINE && side == ::OpsLineSide(endPts1, &endPts2[1]))
        return FALSE;

    side = ::OpsLineSide(endPts2, &endPts1[0]);

    return side == OPS_ON_LINE || side != ::OpsLineSide(endPts2, &endPts1[1]);

} // end: OpsLineSegmentsIntersect()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPointInEllipse().
//
// PURPOSE: Determine if a point lies inside an ellipse.
//
// PARAMETERS:
//
//     Input:
//
//         ellipseRect - pointer to an OpsFloatExtent structure. The
//                       structure pointed to contains the coordinates
//                       of the bounding box of the ellipse.
//         point       - passes the coordinates of the point.
//
//     Output:
//
//         None
//
// RETURNS: Returns TRUE if the point is inside the ellipse and FALSE
//          otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OpsPointInEllipse(const OpsFloatExtent *ellipseRect,
    const OpsFloatPoint *point)
{
    // compute half width and half height of the bounding box

    float halfWidth = static_cast<float>((ellipseRect->xMax - ellipseRect->xMin) / 2.0);
    float halfHeight = static_cast<float>((ellipseRect->yMax - ellipseRect->yMin) / 2.0);

    // handle degenerate ellipse as if it were a rectangle

    if (halfWidth == 0.0 || halfHeight == 0.0)
        return ellipseRect->Contains(point);

    // compute the center of the ellipse

    float xCenter = ellipseRect->xMin + halfWidth;
    float yCenter = ellipseRect->yMin + halfHeight;

    // translate to the origin

    float xPt = point->x - xCenter;
    float yPt = point->y - yCenter;

    // reduce the problem to 'point in circle' if necessary by scaling
    // about the origin

    float radius;

    if (halfWidth < halfHeight) {
        xPt *= (halfHeight / halfWidth);
        radius = halfHeight;
    }
    else if (halfWidth > halfHeight) {
        yPt *= (halfWidth / halfHeight);
        radius = halfWidth;
    }
    else
        radius = halfWidth;

    // return TRUE if point is within a distance of 'radius' of the origin

    return Square(xPt) + Square(yPt) <= Square(radius);

} // end: OpsPointInEllipse()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsLineSegmentIntersectsEllipse().
//
// PURPOSE: Determine if a line segment intersects an ellipse.
//
// PARAMETERS:
//
//     Input:
//
//         ellipseRect - pointer to an OpsFloatExtent structure. The
//                       structure pointed to contains the coordinates
//                       of the bounding box of the ellipse.
//         segEndPts   - passes an array of 2 OpsFloatPoint structures.
//                       The structures contain the coordinates of the
//                       segment endpoints.
//
//     Output:
//
//         None.
//
// RETURNS: Returns TRUE if the line segment intersects the ellipse and
//          FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OpsLineSegmentIntersectsEllipse(const OpsFloatExtent *ellipseRect,
    const OpsFloatPoint segEndPts[])
{
    // compute half width and half height of the bounding box

    float halfWidth = static_cast<float>((ellipseRect->xMax -
        ellipseRect->xMin) / 2.0);
    float halfHeight = static_cast<float>((ellipseRect->yMax -
        ellipseRect->yMin) / 2.0);

    // check for degenerate ellipse

    if (halfWidth == 0.0 || halfHeight == 0.0)
        return FALSE;

    // compute the center of the ellipse

    float xCenter = ellipseRect->xMin + halfWidth;
    float yCenter = ellipseRect->yMin + halfHeight;

    // translate to the origin

    OpsFloatPoint endPt0, endPt1;
    endPt0.x = segEndPts[0].x - xCenter;
    endPt0.y = segEndPts[0].y - yCenter;
    endPt1.x = segEndPts[1].x - xCenter;
    endPt1.y = segEndPts[1].y - yCenter;

    // reduce the problem to 'line segment intersects circle' if necessary by
    // scaling about the origin

    float radius;

    if (halfWidth < halfHeight) {
        float scaleFactor = (halfHeight / halfWidth);
        endPt0.x *= scaleFactor;
        endPt1.x *= scaleFactor;
        radius = halfHeight;
    }
    else if (halfWidth > halfHeight) {
        float scaleFactor = (halfWidth / halfHeight);
        endPt0.y *= scaleFactor;
        endPt1.y *= scaleFactor;
        radius = halfWidth;
    }
    else
        radius = halfWidth;

    // rotate the segment to vertical if necessary

    if (endPt0.x != endPt1.x) {
        double theta = ::atan2(endPt1.x - endPt0.x, endPt1.y - endPt1.y);
        double cosTheta = ::cos(theta);
        double sinTheta = ::sin(theta);
        double temp = endPt0.x;

        endPt0.x = static_cast<float>(temp * cosTheta - endPt0.y * sinTheta);
        endPt0.y = static_cast<float>(temp * sinTheta + endPt0.y * cosTheta);
        endPt1.y = static_cast<float>(endPt1.x * sinTheta + endPt1.y * cosTheta);

        // order y values

        if (endPt0.y > endPt1.y) {
            temp = endPt1.y;
            endPt1.y = endPt0.y;
            endPt0.y = static_cast<float>(temp);
        }
    }

    // return TRUE if some point on the segment is within a distance
    // of 'radius' from the origin

    if (endPt0.y > 0.0)
        return Square(endPt0.x) + Square(endPt0.y) <= Square(radius);
    else if (endPt1.y < 0.0)
        return Square(endPt0.x) + Square(endPt1.y) <= Square(radius);
    else
        return ::fabs(endPt0.x) <= radius;

} // end: OpsLineSegmentIntersectsEllipse()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPointInPolygon().
//
// PURPOSE: Determine if a point is interior to a polygon using the either
//          even-odd rule, or the non-zero winding rule.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes an array of the polygon vertices. NOTE - the
//                     polygon boundary must be explicitly closed.
//         nVertices - passes the number of vertices in the polygon.
//         point     - passes the coordinates of the point to test.
//         fillRule  - passes the polygon fill rule (one of the constants
//                     OPS_FILLRULE_EVEN_ODD, or OPS_FILLRULE_WINDING).
//
//     Output:
//
//         None.
//
// RETURNS: Returns TRUE if the point is interior to the polygon else
//          returns FALSE.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OpsPointInPolygon(const OpsFloatPoint vertices[], int nVertices,
    const OpsFloatPoint *point, int fillRule)
{
    // check for explicit closure in debug mode

    assert(nVertices > 2);
    assert(vertices[0] == vertices[nVertices-1]);

    // if polygon is degenerate, then just return FALSE

    if (nVertices <= 2)
        return FALSE;

    if (fillRule == OPS_FILLRULE_EVEN_ODD)
        return ::OpsWindingNumber(vertices, nVertices, point) % 2 != 0;
    else
        return ::OpsWindingNumber(vertices, nVertices, point) != 0;

} // end: OpsPointInPolygon()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPointInPolyPolygon().
//
// PURPOSE: Determine if a point is interior to a poly-polygon using the
//          even-odd rule or non-zero winding rule.
//
// PARAMETERS:
//
//     Input:
//
//         vertices   - passes an array of the poly-polygon vertices. Each
//                      polygon must be explicitly closed.
//         nPolygons  - passes the number of polygons.
//         nPolyVerts - passes a pointer to an array of polygon vertex
//                      counts.
//         point      - passes the coordinates of the point to test.
//         fillRule   - passes the polygon fill rule (one of the constants
//                      OPS_FILLRULE_EVEN_ODD, or OPS_FILLRULE_WINDING).
//
//     Output:
//
//        None.
//
// RETURNS: Returns TRUE if the point is interior to the poly-polygon; else
//          returns FALSE.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OpsPointInPolyPolygon(const OpsFloatPoint vertices[], int nPolygons,
    const int nPolyVerts[], const OpsFloatPoint *point, int fillRule)
{
    // must have at least one polygon

    assert(nPolygons > 0);

    // if poly-polygon is degenerate, then just return FALSE

    if (nPolygons <= 0)
        return FALSE;

    // sum the winding numbers over all polygon boundaries

    const OpsFloatPoint *polygon = vertices;

    int i = 0, windingNumber = 0;
    for (i = 0, windingNumber = 0; i < nPolygons; i++) {
        // check for explicit closure in debug mode

        assert(nPolyVerts[i] > 2);
        assert(polygon[0] == polygon[nPolyVerts[i]-1]);

        windingNumber += ::OpsWindingNumber(polygon, nPolyVerts[i], point);
        polygon += nPolyVerts[i];
    }

    if (fillRule == OPS_FILLRULE_EVEN_ODD)
        return windingNumber % 2 != 0;
    else
        return windingNumber != 0;

} // end: OpsPointInPolyPolygon()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsWindingNumber().
//
// PURPOSE: Compute the winding number of a point with respect to a (closed)
//          polygon boundary.
//
// PARAMETERS:
//
//     Input:
//
//         vertices   - passes an array of the polygon vertices. NOTE -
//                      the polygon boundary is must be explicitly
//                      closed.
//         nVertices  - passes the number of vertices in the polygon.
//         point      - pass the coordinates of the point to test.
//
//     Output:
//
//         None.
//
// RETURNS: Returns the winding number of the point with respect to the
//          polygon boundary.
//
// EXCEPTIONS: None.
//
// ALGORITHM: The algorithm used is adapted from a similar algorithm (for
//            determining whether a point is interior to a polygon under
//            the even odd rule) described in:
//            "Computational Geometry An Introduction", Preperata, and
//            Shamos, Springer Verlag.
//
//------------------------------------------------------------------------------

int OpsWindingNumber(const OpsFloatPoint vertices[], int nVertices,
    const OpsFloatPoint *point)
{
    // the polygon boundary must be explicitly closed

    assert(nVertices > 2);
    int nEdges = PERF_ADD_START_POINT_TO_LOOP ? nVertices - 2 : nVertices - 1;
    assert(vertices[0] == vertices[nEdges]);

    // loop through the polygon edges testing each
    int windNumber = 0;

    for (int i = 0; i < nEdges; i++) {
        float yMin = Min(vertices[i].y, vertices[i+1].y);

        if (yMin <= point->y) {
            float yMax = Max(vertices[i].y, vertices[i+1].y);

            if (yMax >= point->y) {
                int lineSide = ::OpsLineSide(&vertices[i], point);

                if (lineSide == OPS_ON_LINE)
                    return 1;
                else if (yMin != point->y) {
                    if (yMin == vertices[i].y) {
                        if (lineSide != OPS_LEFT_OF_LINE)
                            windNumber++;
                    }
                    else if (lineSide != OPS_RIGHT_OF_LINE)
                        windNumber--;
                }
            }
        }
    }

    return windNumber;

} // end: OpsWindingNumber()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolygonCentroid().
//
// PURPOSE: Determine the centroid of a simple polygon. A number of
//          different algorithms are employed, and the first that yields
//          a centroid in the polygon is returned.
//
// PARAMETERS:
//
//     Input:
//
//        vertices  - passes a pointer to the array vertices of the
//                    the polygon. Note - the polygon is must be
//                    explicitly closed.
//        nVertices - passes the number of vertices in the polygon.
//
//     Output:
//
//         centroid - passes a pointer to an OpsFloatPoint structure. The
//                    centroid of the polygon is copied to location pointed
//                    to.
//
// RETURNS: Returns TRUE if the computed centroid is interior to the
//          polygon and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OpsPolygonCentroid(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatPoint *centroid)
{
    // test for explicit closure in debug mode

    assert(nVertices > 2);
    assert(vertices[0] == vertices[nVertices-1]);

    // if polygon is degenerate, then just treat it as a polyline

    if (nVertices <= 2) {
        ::OpsPolylineCentroid(vertices, nVertices, centroid);
        return FALSE;
    }

    // try the trapezoid area weighted centroid first

    const int fillRule = OPS_FILLRULE_EVEN_ODD;

    OpsFloatPoint tawCentroid;
    ::OpsPolygonCentroidTAW(vertices, nVertices, &tawCentroid);

    if (::OpsPointInPolygon(vertices, nVertices, &tawCentroid, fillRule)) {
        *centroid = tawCentroid;
        return TRUE;
    }

    // try the bivariate mean centroid next

    ::OpsPolygonCentroidBVM(vertices, nVertices, centroid);

    if (::OpsPointInPolygon(vertices, nVertices, centroid, fillRule))
        return TRUE;

    // try the weighted mean centroid next

    ::OpsPolygonCentroidWMC(vertices, nVertices, centroid);

    if (::OpsPointInPolygon(vertices, nVertices, centroid, fillRule))
        return TRUE;

    // none of the centroids was interior to the polygon, use the
    // trapezoid area weighted centroid, and return FALSE

    *centroid = tawCentroid;
    return FALSE;

} // end: OpsPolygonCentroid()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolygonCentroidTAW().
//
// PURPOSE: Determine the centroid of a simple polygon using the trapezoid
//          area weighted algorithm.
//
// PARAMETERS:
//
//     Input:
//
//        vertices  - passes a pointer to the array vertices of the
//                    the polygon. Note - the polygon is must be
//                    explicitly closed.
//        nVertices - passes the number of vertices in the polygon.
//
//     Output:
//
//         centroid - passes a pointer to an OpsFloatPoint structure. The
//                    weighted area centroid of the polygon is copied to
//                    location pointed to.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
// ALGORITHM: The algorithm used in described in: "A Comparison of simple
//            mathematical approaches to the placement of spot symbols",
//            L. W. Carstensen Jr., Cartographica, 24(3), 1987.
//
//------------------------------------------------------------------------------

void OpsPolygonCentroidTAW(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatPoint *centroid)
{
    // test for explicit closure in debug mode

    assert(nVertices > 2);
    assert(vertices[0] == vertices[nVertices-1]);

    // compute the area weighted centroid

    int i;
    double x1, x2, y1, y2, xt1, xt2, yt1, yt2;

    for (i = 1, xt1=0.0, xt2=0.0, yt1=0.0, yt2=0.0; i < nVertices; i++) {
        x1 = static_cast<double>(vertices[i].x) +
             static_cast<double>(vertices[i-1].x);
        x2 = static_cast<double>(vertices[i].x) -
             static_cast<double>(vertices[i-1].x);

        y1 = static_cast<double>(vertices[i].y) +
             static_cast<double>(vertices[i-1].y);
        y2 = static_cast<double>(vertices[i].y) -
             static_cast<double>(vertices[i-1].y);

        xt1 += (x1 * x2 * y1);
        xt2 += (x2 * y1);

        yt1 += (y1 * y2 * x1);
        yt2 += (y2 * x1);
    }

    // compute weighted centroid if xt2 and yt2 are not zero

    if (xt2 != 0.0 && yt2 != 0.0) {
        centroid->x = static_cast<float>(0.5 * xt1 / xt2);
        centroid->y = static_cast<float>(0.5 * yt1 / yt2);
    }

    // else just set to 0

    else {
        centroid->x = 0.0f;
        centroid->y = 0.0f;
    }

} // end: OpsPolygonCentroidTAW()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolygonCentroidBVM().
//
// PURPOSE: Determine the centroid of a polygon using the the bivariate
//          mean algorithm.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes a pointer to the array vertices of the
//                     the polygon. Note - the polygon is must be
//                     explicitly closed.
//         nVertices - passes the number of vertices in the polygon.
//
//     Output:
//
//         centroid  - returns the bivariate mean centroid of the polygon.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
// ALGORITHM: The algorithm used in described in: "A Comparison of simple
//            mathematical approaches to the placement of spot symbols",
//            L. W. Carstensen Jr., Cartographica, 24(3), 1987.
//
//------------------------------------------------------------------------------

void OpsPolygonCentroidBVM(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatPoint *centroid)
{
    int i;
    double xSum, ySum;

    for (i = 1, xSum = 0.0, ySum = 0.0; i < nVertices; i++) {
        xSum += vertices[i].x;
        ySum += vertices[i].y;
    }

    if (nVertices > 0.0) {
        centroid->x = static_cast<float>(xSum / (nVertices - 1));
        centroid->y = static_cast<float>(ySum / (nVertices - 1));
    }
    else {
        centroid->x = 0.0f;
        centroid->y = 0.0f;
    }

} // end: OpsPolygonCentroidBVM()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolygonCentroidWMC().
//
// PURPOSE: Determine the centroid of a polygon using the the weighted
//          mean center algorithm.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes a pointer to the array vertices of the
//                     the polygon. Note - the polygon is must be
//                     explicitly closed.
//         nVertices - passes the number of vertices in the polygon.
//
//     Output:
//
//         centroid  - returns the weighted mean centroid of the polygon.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
// ALGORITHM: The algorithm used in described in: "A Comparison of simple
//            mathematical approaches to the placement of spot symbols",
//            L. W. Carstensen Jr., Cartographica, 24(3), 1987.
//
//------------------------------------------------------------------------------

void OpsPolygonCentroidWMC(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatPoint *centroid)
{
    int i;
    double xSum, ySum, segLength;

    for (i = 0, xSum = 0.0, ySum = 0.0; i < nVertices - 1; i++) {
        segLength = ::OpsLineLength(&vertices[i+1], &vertices[i]);
        xSum += vertices[i].x * segLength;
        ySum += vertices[i].y * segLength;
    }

    double totalLength = ::OpsPolylineLength(vertices, nVertices);

    if (totalLength > 0.0) {
        centroid->x = static_cast<float>(xSum / totalLength);
        centroid->y = static_cast<float>(ySum / totalLength);
    }
    else {
        centroid->x = 0.0f;
        centroid->y = 0.0f;
    }

} // end: OpsPolygonCentroidBVM()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolylineCentroid().
//
// PURPOSE: Determine the "centroid" of a polyline; the centroid is deemed to be
//          the point on the polyline that is halfway along the length of the
//          polyline.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes a pointer to the array of vertices of the
//                     polyline.
//         nVertices - passes the number of vertices in the polyline.
//
//     Output:
//
//         centroid  - returns the centroid of the polyline.
//
// RETURNS: Returns the index of the first endpoint of the polyline segment
//          containing the centroid.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OpsPolylineCentroid(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatPoint *centroid)
{
    // determine total length of the polyline

    double totalLength = ::OpsPolylineLength(vertices, nVertices);

    // find the segment containing the halfway point

    int i;
    double length, halfway, segLength;

    for (i = 0, length = 0.0, halfway = totalLength / 2.0; ; i++) {
        segLength = ::OpsLineLength(&vertices[i+1], &vertices[i]);

        if ((length + segLength) >= halfway)
             break;

        length += segLength;
    }

    // compute the halfway point

    double scaleFactor = (segLength > 0.0 ? (halfway-length)/segLength : 0.0);
    double deltaX = vertices[i+1].x - vertices[i].x;
    centroid->x = static_cast<float>(vertices[i].x + scaleFactor * deltaX);
    double deltaY = vertices[i+1].y - vertices[i].y;
    centroid->y = static_cast<float>(vertices[i].y + scaleFactor * deltaY);
    return i;

} // end: OpsPolylineCentroid()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolylineLength().
//
// PURPOSE: Determine the length of a polyline.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes a pointer to the array of vertices of the
//                     polyline.
//         nVertices - passes the number of vertices in the polyline.
//
//     Output:
//
//         None.
//
// RETURNS: Returns the length of the polyline.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double OpsPolylineLength(const OpsFloatPoint vertices[], int nVertices)
{
    int i;
    double length, deltaX, deltaY;

    for (i = 1, length = 0.0; i < nVertices; i++) {
        deltaX = vertices[i].x - vertices[i-1].x;
        deltaY = vertices[i].y - vertices[i-1].y;
        length += ::sqrt(deltaX * deltaX + deltaY * deltaY);
    }

    return length;

} // end: OpsPolylineLength()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsLineLength().
//
// PURPOSE: Determine the length of a line segment.
//
// PARAMETERS:
//
//     Input:
//
//         endPt1 - passes a pointer to the first endpoint.
//         endPt2 - passes a pointer to the second endpoint.
//
//     Output:
//
//         None.
//
// RETURNS: Returns the length of the line segment.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double OpsLineLength(const OpsFloatPoint *endPt1, const OpsFloatPoint *endPt2)
{
    double deltaX = endPt2->x - endPt1->x;
    double deltaY = endPt2->y - endPt1->y;
    return ::sqrt(deltaX * deltaX + deltaY * deltaY);

} // end: OpsLineLength()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolyPolygonCentroid().
//
// PURPOSE: Determine the centroid of a poly-polygon. The polygon that
//          encloses the largest area is used to compute the centroid.
//
// PARAMETERS:
//
//     Input:
//
//         vertices   - passes a pointer to the array of vertices. Each
//                      polygon must be explicitly closed.
//         nPolyVerts - passes a pointer to an array of polygon vertex
//                      counts.
//         nPolygons  - passes the number of polygons.
//
//     Output:
//
//         centroid   - returns the centroid of the poly-polygon.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsPolyPolygonCentroid(const OpsFloatPoint vertices[],
    const int nPolyVerts[], int nPolygons, OpsFloatPoint *centroid)
{
    // must be at least one polygon

    assert(nPolygons > 0);

    // determine which polygon encloses the largest area

    const OpsFloatPoint *maxPolygon = vertices;
    const OpsFloatPoint *currPolygon = vertices;
    int nVertices = nPolyVerts[0];
    double maxArea = -HUGE_VAL;

    for (int i = 0; i < nPolygons; i++) {
        double area = ::OpsPolygonArea(currPolygon, nPolyVerts[i]);

        if (area > maxArea) {
            maxPolygon = currPolygon;
            nVertices = nPolyVerts[i];
            maxArea = area;
        }

        currPolygon += nPolyVerts[i];
    }

    // get the centroid of the chosen polygon

    ::OpsPolygonCentroid(maxPolygon, nVertices, centroid);

} // end: OpsPolyPolygonCentroid()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolygonArea().
//
// PURPOSE: Determine the area of a simple polygon.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes a pointer to the array of vertices of the
//                     polygon. Note that the polygon is assumed to be
//                     explicitly closed.
//         nVertices - passes the number of vertices in the polygon.
//
//     Output:
//
//         None.
//
// RETURNS: Returns the area of the polygon.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double OpsPolygonArea(const OpsFloatPoint vertices[], int nVertices)
{
    // test for explicit closure in debug mode

    assert(nVertices > 2);
    assert(vertices[0] == vertices[nVertices-1]);

    // compute the signed area by summing the twice the (signed) areas of the
    // trapezoids formed by the polygon edges and the x-axis

    int i;
    double sum;

    for (i = 1, sum = 0.0; i < nVertices; i++)
        sum += (static_cast<double>(vertices[i-1].x) *
                static_cast<double>(vertices[i].y)) -
               (static_cast<double>(vertices[i-1].y) *
                static_cast<double>(vertices[i].x));

    // return the absolute value of the signed area

    return ::fabs(sum * 0.5);

} // end: OpsPolygonArea()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolyPolygonArea().
//
// PURPOSE: Determine the area of a poly-polygon.
//
// PARAMETERS:
//
//     Input:
//
//         vertices   - passes a pointer to the array of vertices. Each
//                      polygon must be explicitly closed.
//         nPolyVerts - passes a pointer to an array of polygon vertex
//                      counts.
//         nPolygons  - passes the number of polygons.
//
//     Output:
//
//         None.
//
// RETURNS: Returns the area of the poly-polygon.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double OpsPolyPolygonArea(const OpsFloatPoint vertices[],
    const int nPolyVerts[], int nPolygons)
{
    // loop through each polygon computing its area - add the absolute
    // value of the area if the polygon represents an exterior boundary,
    // otherwise subtract the absolute value of the area

    const OpsFloatPoint *currPolygon = vertices;
    double totalArea = 0.0;

    for (int i = 0; i < nPolygons; i++) {
        // compute the area of the current polygon

        double polygonArea = ::OpsPolygonArea(currPolygon, nPolyVerts[i]);
        assert(polygonArea >= 0.0);

        // test the first vertex of the current polygon for inclusion in
        // each of the remaining polygons, and keep track of how many contain-
        // ments are detected. If there are an odd number of containments, then
        // the polygon is the boundary of a hole and the area needs to be
        // subtracted. If there are an even number of containments, the polygon
        // is and exterior boundary or the boundary of an island, and the area
        // needs to be added.

        const OpsFloatPoint *testPolygon = vertices;
        int nContainments = 0;

        for (int j = 0; j < nPolygons; j++) {
            if (j != i) {
                if (::OpsPointInPolygon(testPolygon, nPolyVerts[j],
                        currPolygon, OPS_FILLRULE_EVEN_ODD)) {
                    nContainments++;
                }
            }

            testPolygon += nPolyVerts[j];
        }

        if (nContainments % 2 == 0)
            totalArea += polygonArea;
        else
            totalArea -= polygonArea;

        currPolygon += nPolyVerts[i];
    }

    // return the area

    assert(totalArea >= 0.0);

    if (totalArea >= 0.0)
        return totalArea;
    else
        return -totalArea;

} // end: OpsPolyPolygonArea()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolygonSelfIntersects().
//
// PURPOSE: Test a polygon to determine whether or not it is self-intersecting.
//          A polygon is self-intersecting if two or more edges intersect at
//          any point other than a shared endpoint. Note that the algorithm runs
//          is time quadratic in the number of polygon edges.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes a pointer to the array of vertices of the polygon.
//         nVertices - passes the number of vertices in the polygon.
//
//     Output:
//
//         None.
//
// RETURNS: Returns TRUE if the polygon self-intersects and FALSE
//          otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OpsPolygonSelfIntersects(const OpsFloatPoint vertices[], int nVertices)
{
    // test the first edge for intersection with the other non-adjacent
    // edges
    int i = 0;
    for (i = 2; i < nVertices - 2; i++)
        if (::OpsLineSegmentsIntersect(&vertices[0], &vertices[i]))
                 return TRUE;

    // test the remaining edge for intersection with the other non-adjacent
    // edges

    for (i = 1; i < nVertices - 3; i++) {
         for (int j = i + 2; j < nVertices - 1; j++) {
             if (::OpsLineSegmentsIntersect(&vertices[i], &vertices[j]))
                 return TRUE;
         }
    }

    return FALSE;

} // end: OpsPolygonSelfIntersects()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolygonizeEllipse().
//
// PURPOSE: Generate a polygonal approximation to an ellipse.
//
// PARAMETERS:
//
//     Input:
//
//         ellipseRect - pointer to an OpsFloatExtent structure. The struc-
//                       ture pointed to contains the coordinates of the
//                       bounding box of the ellipse.
//         nSegments   - passes the number of segments to generate in the
//                       polygonal approximation. The total number of vert-
//                       ices will be nSegments + 1.
//
//     Output:
//
//         vertices    - passes a pointer to an array of OpsFloatPoint
//                       structures; the vertices of the polygonal
//                       approximation of the ellipse are copied to
//                       the array.
//
// RETURNS: Returns FALSE if the ellipse is degenerate, i.e., the rectangle
//          enclosing the ellipse has zero area, otherwise returns TRUE. If
//          the ellipse is degenerate, the no vertices are copied to the
//          output array.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OpsPolygonizeEllipse(const OpsFloatExtent *ellipseRect, int nSegments,
    OpsFloatPoint vertices[])
{
    // call method to polygonize an ellipse arc, specifying identical
    // start and end angles in order to sweep out the entire ellipse

    int nVertices;
    ::OpsPolygonizeEllipseArc(ellipseRect, 0.0, 0.0, nSegments,
        OPS_ARC_CLOSE_NONE, vertices, nVertices);
    return nVertices > 0;

} // end: OpsPolygonizeEllipse()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsPolygonizeEllipseArc().
//
// PURPOSE: Generate a polygonal approximation to an ellipse arc.
//
// PARAMETERS:
//
//     Input:
//
//         ellipseRect   - pointer to an OpsFloatExtent structure. The struc-
//                         ture pointed to contains the coordinates of the
//                         bounding box of the ellipse defining the arc.
//         startAngleDeg - passes the start angle of the arc in degrees.
//         endAngleDeg   - passes the end angle of the arc in degrees. If
//                         endAngleDeg = startAngleDeg, then the entire arc
//                         of the ellipse is polygonized.
//         closeStyle    - passes the close style for the ellipse arc (one
//                         of the constants defined in ogeomtry.h).
//         maxSegments   - passes the number of segments that should be used
//                         if the entire arc of the ellipse is polygonized.
//
//     Output:
//
//         vertices      - passes a pointer to an array of OpsFloatPoint
//                         structures; the vertices of the polygonal
//                         approximation of the ellipse arc are copied to
//                         the array.
//         nVertices     - passes a reference to an int. The total number
//                         of vertices (including closure vertices) is copied
//                         to the referenced int. If the defining ellipse is
//                         degenerate, then 0 is copied to the int.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsPolygonizeEllipseArc(const OpsFloatExtent *ellipseRect,
    double startAngleDeg, double endAngleDeg, int maxSegments,
    int closeStyle, OpsFloatPoint vertices[], int &nVertices)
{
    // check for reasonable input

    assert(maxSegments > 0);

    // compute semimajor, and semiminor axes of the ellipse

    double a = (ellipseRect->xMax - ellipseRect->xMin) / 2.0;
    double b = (ellipseRect->yMax - ellipseRect->yMin) / 2.0;

    // if the ellipse is degenerate, then nothing to do

    if (a == 0.0 || b == 0.0) {
        nVertices = 0;
        return;
    }

    // compute center of ellipse

    double xCenter = ellipseRect->xMin + a;
    double yCenter = ellipseRect->yMin + b;

    // compute angular increment to use, its sine and cosine

    const double PiRads = ::acos(-1.0);
    const double TwoPiRads = PiRads * 2.0;
    double deltaTheta = TwoPiRads / maxSegments;
    double cosDeltaTheta = ::cos(deltaTheta);
    double sinDeltaTheta = ::sin(deltaTheta);

    // convert start and end angles to radians

    double startAngleRad = startAngleDeg * PiRads / 180.0;
    double endAngleRad = endAngleDeg * PiRads / 180.0;
    BOOL entireArc = endAngleDeg == startAngleDeg;

    // arrange for the end angle to be numerically greater than the start angle

    while (endAngleRad <= startAngleRad)
        endAngleRad += TwoPiRads;

    // precompute the polygonization factors

    double aOverbSinDeltaTheta = (a / b) * sinDeltaTheta;
    double bOveraSinDeltaTheta = (b / a) * sinDeltaTheta;

    // polygonize the ellipse

    double x = a * ::cos(startAngleRad);
    double y = b * ::sin(startAngleRad);

    int maxIndex;

    if (entireArc)
        maxIndex = maxSegments;
    else
        maxIndex = static_cast<int>(::floor((endAngleRad - startAngleRad) /
            deltaTheta)) - 1;

    int i = 0;

    while (TRUE) {
        // compute the next vertex, translating it to the center

        vertices[i].x = static_cast<float>(x + xCenter);
        vertices[i++].y = static_cast<float>(y + yCenter);

        // if all segments have been completed then break out of loop

        if (i >= maxIndex)
           break;

        // compute the next origin centered vertex

        double temp = x;
        x = x * cosDeltaTheta - y * aOverbSinDeltaTheta;
        y = temp * bOveraSinDeltaTheta + y * cosDeltaTheta;
    }

    // compute last vertex in the arc

    if (entireArc)
        vertices[i++] = vertices[0];
    else {
        vertices[i].x = static_cast<float>(a * ::cos(endAngleRad) + xCenter);
        vertices[i++].y = static_cast<float>(b * ::sin(endAngleRad) + yCenter);
    }

    // close the arc if required

    if (closeStyle == OPS_ARC_CLOSE_PIE) {
        vertices[i].x = static_cast<float>(xCenter);
        vertices[i++].y = static_cast<float>(yCenter);
        vertices[i++] = vertices[0];
    }
    else if (closeStyle == OPS_ARC_CLOSE_CHORD)
        vertices[i++] = vertices[0];

    nVertices = i;

} // end: OpsPolygonizeEllipseArc()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsClipCode() (inline).
//
// PURPOSE: Determine the clip-code of a point with respect to the clip
//          rectangle (required by the Cohen-Sutherland line clipping
//          algorithm).
//
// PARAMETERS:
//
//     Input:
//
//         clipRect - passes a reference to an OpsFloatExtent structure; the
//                    structure contains the coordinates of the corners of
//                    the clip rectangle.
//         point    - reference to a OpsFloatPoint structure; the contents of
//                    the structure contain the coordinates of the point the
//                    clip code is to be determined for.
//
//     Output:
//
//         None.
//
// RETURNS: The function returns the clip-code.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

// define the clip-code bits

static const int LEFT   = 0x01;
static const int RIGHT  = 0x02;
static const int TOP    = 0x04;
static const int BOTTOM = 0x08;
static const int INSIDE = 0x00;

inline int OpsClipCode(const OpsFloatExtent &clipRect,
    const OpsFloatPoint &point)
{
    int clipCode;

    if (point.x < clipRect.xMin)
        clipCode = LEFT;
    else if (point.x > clipRect.xMax)
        clipCode = RIGHT;
    else
        clipCode = INSIDE;

    if (point.y < clipRect.yMin)
        return clipCode |= BOTTOM;
    else if (point.y > clipRect.yMax)
        return clipCode |= TOP;
    else
        return clipCode;

} // end: OpsClipCode()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsClipLine().
//
// PURPOSE: Clip a line (given in application coordinates) against the
//          specified clip rectangle using the Cohen-Sutherland clipping
//          algorithm. Note - the clipped segment is guaranteed to have
//          the same directon as the original.
//
// PARAMETERS:
//
//     Input:
//
//         clipRect - passes a reference to an OpsFloatExtent structure; the
//                    structure contains the coordinates of the corners of
//                    the clip rectangle.
//         endPt1   - passes a reference to an OpsFloatPoint structure; the
//                    structure contains the coordinates of the first endpoint
//                    of the line segment.
//         endPt2   - passes a reference to an OpsFloatPoint structure; the
//                    structure contains the coordinates of the second endpoint
//                    of the line segment.
//
//     Output:
//
//         endPt1   - the coordinates of the (possibly clipped) first endpoint
//                    are returned in the referenced structure.
//         endPt2   - the coordinates of the (possibly clipped) second endpoint
///                   are returned in the referenced structure.
//
// RETURNS: The method returns 0 if the line segment is not visible with respect
//          the clip rectangle; 1 if the line segment is visible and the second
//          endpoint was not clipped off; 2 if the segment is visible and the
//          second endpoint was clipped off.
//
// EXCEPTIONS: None.
//
// IMPLEMENTATION: The implementation is based on a version of the
//                 algorithm presented in: "Computer Graphics: Principles
//                 and Practice", Foley, Van Dam, Feiner, and Hughes,
//                 Addison Wesley.
//
//------------------------------------------------------------------------------

int OpsClipLine(const OpsFloatExtent &clipRect, OpsFloatPoint &endPt1,
    OpsFloatPoint &endPt2)
{
    // Compute the initial clip codes for the endpoints

    int clipCode1 = OpsClipCode(clipRect, endPt1);
    int clipCode2 = OpsClipCode(clipRect, endPt2);
    int initClipCode2 = clipCode2;

    // compute the difference between the x and y values of the endpoints

    double deltaX = endPt2.x - endPt1.x;
    double deltaY = endPt2.y - endPt1.y;

    // try to clip against all four boundaries of the clip rectangle

    for (int i = 0; i < 4; i++) {
        double x, y;

        // trivially reject or accept the line segment ?

        if (clipCode1 & clipCode2)
            return 0;
        else if (clipCode1 == clipCode2)
            return initClipCode2 == INSIDE ? 1 : 2;

        // use the clip code for an endpoint outside the clip rectangle

        int clipCode = (clipCode1 ? clipCode1 : clipCode2);

        // clip the line against the first boundary it crosses

        if (clipCode & TOP) {
            x = endPt1.x + ((clipRect.yMax - endPt1.y) * deltaX / deltaY);
            y = clipRect.yMax;
        }
        else if (clipCode & BOTTOM) {
            x = endPt1.x + ((clipRect.yMin - endPt1.y) * deltaX / deltaY);
            y = clipRect.yMin;
        }
        else if (clipCode & RIGHT) {
            y = endPt1.y + ((clipRect.xMax - endPt1.x) * deltaY / deltaX);
            x = clipRect.xMax;
        }
        else {
            y = endPt1.y + ((clipRect.xMin - endPt1.x) * deltaY / deltaX);
            x = clipRect.xMin;
        }

        // Set up for next pass through loop

        if (clipCode == clipCode1) {
            endPt1.x = static_cast<float>(x);
            endPt1.y = static_cast<float>(y);
            clipCode1 = OpsClipCode(clipRect, endPt1);
        }
        else {
            endPt2.x = static_cast<float>(x);
            endPt2.y = static_cast<float>(y);
            clipCode2 = OpsClipCode(clipRect, endPt2);
        }
    }

    // if we get here, then the second endpoint must have been clipped off

    return 2;

} // end OpsClipLine()


//------------------------------------------------------------------------------
//
// METHOD: OpsClipPolyline().
//
// PURPOSE: Clip a polyline against the specified clip rectangle. The clipped
//          result is returned as a poly-polyline containing zero or more
//          polylines.
//
// PARAMETERS:
//
//     Input:
//
//         clipRect    - passes a reference to an OpsFloatExtent structure; the
//                       structure contains the coordinates of the corners of
//                       the clip rectangle.
//         inVertices  - passes an array of OpsFloatPoint structures; the array
//                       contains the vertices of the polyline.
//         nVertices   - passes number of vertices in the polyline.
//
//     Output:
//
//         outVertices - pointer to an array of OpsFloatPoint structures; the
//                       vertices of the clipped result are copied to the array
//                       pointed to.
//         nPolylines  - reference to an integer. The number of polylines in the
//                       clipped result is copied to the referenced integer.
//         nPolyVerts  - pointer to an array of integers. The number of vertices
//                       in each polyline of the poly-polyline are copied to the
//                       array elements.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsClipPolyline(const OpsFloatExtent &clipRect,
    const OpsFloatPoint inVertices[], int nVertices,
    OpsFloatPoint outVertices[], int &nPolylines, int nPolyVerts[])
{
    // initialize

    outVertices[0] = inVertices[0];
    nPolylines = 0;
    nPolyVerts[0] = 0;

    // clip each segment, collecting up connected sequences of segments

    for (int i = 0, j = 0; i < nVertices - 1; i++) {

        // clip the next segment

        outVertices[j+1] = inVertices[i+1];
        int clipStatus;
        clipStatus = ::OpsClipLine(clipRect,outVertices[j], outVertices[j+1]);

        // advance to next segment if current segment is invisible

        if (clipStatus == 0)
            outVertices[j] = inVertices[i+1];

        // else add clipped result to the output

        else {
            // advance output array index and polyline vertex counter

            j++;
            nPolyVerts[nPolylines]++;

            // if the second endpoint has been clipped off or this is the
            // last segment then end the current polyline and start a new
            // one (provided this is not the last segment)

            if (clipStatus == 2 || i == nVertices - 2) {
                j++;
                nPolyVerts[nPolylines]++;
                nPolylines++;

                if (i != nVertices - 2) {
                    outVertices[j] = inVertices[i+1];
                    nPolyVerts[nPolylines] = 0;
                }
            }
        }
    }

} // end: OpsClipPolyline()


//------------------------------------------------------------------------------
//
// METHOD: OpsClipPolyPolyline().
//
// PURPOSE: Clip a poly-polyline against the specified clip rectangle.
//
// PARAMETERS:
//
//     Input:
//
//         clipRect      - passes a reference to an OpsFloatExtent structure;
//                         the structure contains the coordinates of the corners
//                         of the clip rectangle.
//         inVertices    - passes an array of OpsFloatPoint structures; the
//                         array contains the vertices of the poly-polyline.
//         nInPolyVerts  - passes an array of integers. The number of
//                         vertices in each polyline of the poly-polyline
//                         are are stored by successive array elements.
//         nInPolylines  - passes the number of polylines.
//
//     Output:
//
//         outVertices   - pointer to an array of OpsFloatPoint structures;
//                         the vertices of the clipped result are copied to
//                         the array pointed to.
//         nOutPolyVerts - pointer to an array of integers. The number of
//                         vertices in each polyline of the clipped poly-
//                         polyline are copied to the array elements.
//         nOutPolylines - reference to an integer. The number of polylines
//                         in the clipped result is copied to the referenced
//                         integer.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsClipPolyPolyline(const OpsFloatExtent &clipRect,
    const OpsFloatPoint inVertices[], const int nInPolyVerts[],
    int nInPolylines, OpsFloatPoint outVertices[], int nOutPolyVerts[],
    int &nOutPolylines)
{
    // initialize

    nOutPolylines = 0;

    // loop through the input polylines, clipping each in turn

    const OpsFloatPoint *currPolyline = inVertices;

    for (int i = 0, j = 0; i < nInPolylines; i++) {
        // setup to clip the first segment of the current polyline

        nOutPolyVerts[nOutPolylines] = 0;
        outVertices[j] = currPolyline[0];

        for (int k = 0; k < nInPolyVerts[i] - 1; k++) {
            outVertices[j+1] = currPolyline[k+1];

            int clipStatus = ::OpsClipLine(clipRect, outVertices[j],
                outVertices[j+1]);

            // if current segment is invisible then advance to the next

            if (clipStatus == 0)
                outVertices[j] = currPolyline[k+1];

            // else add clipped result to the output

            else {
                // advance output array index and polyline vertex counter

                j++;
                nOutPolyVerts[nOutPolylines]++;

                // if the second endpoint has been clipped off or this is the
                // last segment then end the current polyline and start a new
                // one (provided this is not the last segment)

                if (clipStatus == 2 || k == nInPolyVerts[i] - 2) {
                    j++;
                    nOutPolyVerts[nOutPolylines]++;
                    nOutPolylines++;

                    if (k != nInPolyVerts[i] - 2) {
                        outVertices[j] = currPolyline[k+1];
                        nOutPolyVerts[nOutPolylines] = 0;
                    }
                }
            }
        }

        // move to next input polyline

        currPolyline += nInPolyVerts[i];
    }

} // end: OpsClipPolyPolyline()


//------------------------------------------------------------------------------
//
// FUNCTION: AppendLBClipVertex().
//
// PURPOSE: Append a vertex from the Liang-Barsky polygon clipping algorithm to
//          the end of an output array. Degenerate edges are culled.
//
// PARAMETERS:
//
//     Input:
//
//         clipRect  - passes a reference to an OpsFloatExtent structure; the
//                     structure pointed to contains the coordinates of the
//                     corners of the clip rectangle.
//         xClip     - passes the x coordinate of the clipped vertex.
//         yClip     - passes the y coordiante of the clipped vertex.
//         nVerts    - reference to an integer containing the current number of
//                     vertices in verts[].
//
//
//     Output:
//
//         verts     - pointer to an array of OpsFloatPoint structures; the
//                     clipped vertex is appended to the end of the array.
//         nVerts    - the referenced integer is (possibly) incremented on
//                     return to reflect the number of vertices in verts[].
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

static void AppendLBClipVertex(const OpsFloatExtent &clipRect, float x,
    float y, OpsFloatPoint verts[], int &nVerts)
{
    // if there is at least one edge in the output array, then determine if the
    // edge defined by the last two vertices in the array plus the new clip
    // vertex induce a degenerate edge; the edge is considered degenerate if
    // if doubles back on the last edge in the output array, and lies along
    // one of the boundaries of the clip rectangle

    BOOL degenerate;

    if (nVerts > 1) {
        float x1 = verts[nVerts-1].x;
        float y1 = verts[nVerts-1].y;
        float x2 = verts[nVerts-2].x;
        float y2 = verts[nVerts-2].y;

        degenerate = (x == x1 && x == x2 &&
                          (x == clipRect.xMin || x == clipRect.xMax) &&
                          ((y <= y1 && y2 <= y1) || (y >= y1 && y2 >= y1))) ||
                     (y == y1 && y == y2 &&
                          (y == clipRect.yMin || y == clipRect.yMax) &&
                          ((x <= x1 && x2 <= x1) || (x >= x1 && x2 >= x1)));
    }

    // else if there is only one vertex, and the new vertex is identical, then
    // flag as degenerate

    else if (nVerts == 1)
        degenerate = x == verts[0].x && y == verts[0].y;

    // else not degenerate

    else
        degenerate = FALSE;

    // if the new vertex does not induce a degeneracy, then just append

    if (!degenerate) {
        verts[nVerts].x = x;
        verts[nVerts].y = y;
        nVerts++;
    }

    // else overwrite the last vertex with the new vertex - this will eliminate
    // the degeneracy

    else {
        verts[nVerts-1].x = x;
        verts[nVerts-1].y = y;
    }

} // end: AppendLBClipVertex()


//------------------------------------------------------------------------------
//
// FUNCTION: OpsClipPolygon().
//
// PURPOSE: Clip a polygon to the specified clip rectangle using the Liang-
//          Barsky algorithm.
//
// PARAMETERS:
//
//     Input:
//
//         clipRect  - passes a reference to an OpsFloatExtent structure; the
//                     structure pointed to contains the coordinates of the
//                     corners of the clip rectangle.
//         inVerts   - passes an array of OpsFloatPoint structures; the
//                     structures contain the coordinates of the vertices
//                     of the polygon (the polygon must be explicitly closed).
//         nInVerts  - passes the number of vertices in the polygon.
//
//
//     Output:
//
//         outVerts  - pointer to an array of OpsFloatPoint structures; the
//                     coordinates of the vertices of the clipped polygon
//                     are copied to the structures (the clipped version
//                     of the polygon is not explicitly closed!).
//         nOutVerts - reference to an integer. The number of vertices in
//                     the clipped result is copied to the referenced int.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
// IMPLEMENTATION: The implementation is based on a version of the
//                 algorithm presented in: "Computer Graphics: Principles
//                 and Practice", Foley, Van Dam, Feiner, and Hughes,
//                 Addison Wesley.
//
//------------------------------------------------------------------------------

const double POSITIVE_INFINITY = HUGE_VAL;
const double MINUS_INFINITY = -HUGE_VAL;

void OpsClipPolygon(const OpsFloatExtent &clipRect,
    const OpsFloatPoint inVerts[], int nInVerts,
    OpsFloatPoint outVerts[], int &nOutVerts)
{
    // loop through polygon edges, clipping each ...
    int i = 0, j = 0;
    for (i = 0, j = 0; i < nInVerts - 1; i++) {
        double xIn, xOut, yIn, yOut, tInX, tInY,
               tOutX, tOutY, tIn2, tOut1, tOut2;

        // determine the direction of the edge

        double deltaX = double(inVerts[i+1].x) - double(inVerts[i].x);
        double deltaY = double(inVerts[i+1].y) - double(inVerts[i].y);

        // determine which bounding lines for the clip rectangle the containing
        // line first hits

        if (deltaX > 0.0 || (deltaX == 0.0 && inVerts[i].x > clipRect.xMax)) {
            xIn  = clipRect.xMin;
            xOut = clipRect.xMax;
        }
        else {
            xIn  = clipRect.xMax;
            xOut = clipRect.xMin;
        }

        if (deltaY > 0.0 || (deltaY == 0.0 && inVerts[i].y > clipRect.yMax)) {
            yIn  = clipRect.yMin;
            yOut = clipRect.yMax;
        }
        else {
            yIn  = clipRect.yMax;
            yOut = clipRect.yMin;
        }

        // find the t values for x and y exit points

        if (deltaX != 0.0)
            tOutX = (xOut - inVerts[i].x) / deltaX;
        else if (clipRect.xMin<=inVerts[i].x && inVerts[i].x<=clipRect.xMax)
            tOutX = POSITIVE_INFINITY;
        else
            tOutX = MINUS_INFINITY;

        if (deltaY != 0.0)
            tOutY = (yOut - inVerts[i].y) / deltaY;
        else if (clipRect.yMin<=inVerts[i].y && inVerts[i].y<=clipRect.yMax)
            tOutY = POSITIVE_INFINITY;
        else
            tOutY = MINUS_INFINITY;

        // Order the two exits points

        if (tOutX < tOutY) {
            tOut1 = tOutX;
            tOut2 = tOutY;
        }
        else {
            tOut1 = tOutY;
            tOut2 = tOutX;
        }

        // if tOut2 > 0.0 then there could be some output

        if (tOut2 > 0.0) {
            // compute tIn2

            if (deltaX != 0.0)
                tInX = (xIn - inVerts[i].x) / deltaX;
            else
                tInX = MINUS_INFINITY;

            if (deltaY != 0.0)
                tInY = (yIn - inVerts[i].y) / deltaY;
            else
                tInY = MINUS_INFINITY;

            if (tInX < tInY)
                tIn2 = tInY;
            else
                tIn2 = tInX;

            // if tOut1 < tIn2 then there is no visible segment - check for a
            // a possible turning vertex

            if (tOut1 < tIn2) {

                // if tOut1 > 0.0 and tOut1 < 1.0 then line crosses over
                // intermediate corner region - generate turning vertex

                if (tOut1 > 0.0 && tOut1 <= 1.0) {
                    float xClip;
                    float yClip;

                    if (tInX < tInY) {
                        xClip = static_cast<float>(xOut);
                        yClip = static_cast<float>(yIn);
                    }
                    else {
                        xClip = static_cast<float>(xIn);
                        yClip = static_cast<float>(yOut);
                    }

                    AppendLBClipVertex(clipRect, xClip, yClip, outVerts, j);
                }
            }

            // else if tOut1 > 0.0 and tIn2 <=1.0 then there is a visible
            // segment

            else if (tOut1 > 0.0 && tIn2 <= 1.0) {

                // if tIn2 > 0.0 then the line segment begins outside the
                // window - find the entering intersection

                if (tIn2 > 0.0) {
                    float xClip;
                    float yClip;

                    if (tInX > tInY) {
                        xClip = static_cast<float>(xIn);
                        yClip = static_cast<float>(inVerts[i].y + tInX * deltaY);
                    }
                    else {
                        xClip = static_cast<float>(inVerts[i].x + tInY * deltaX);
                        yClip = static_cast<float>(yIn);
                    }

                    AppendLBClipVertex(clipRect, xClip, yClip, outVerts, j);
                }

                // if tOut1 < 1.0 then the line segment terminates outside the
                // window - find the exiting intersection

                if (tOut1 < 1.0) {
                    float xClip;
                    float yClip;

                    if (tOutX < tOutY) {
                        xClip = static_cast<float>(xOut);
                        yClip = static_cast<float>(inVerts[i].y + tOutX * deltaY);
                    }
                    else {
                        xClip = static_cast<float>(inVerts[i].x + tOutY * deltaX);
                        yClip = static_cast<float>(yOut);
                    }

                    AppendLBClipVertex(clipRect, xClip, yClip, outVerts, j);
                }

                // else the line segment terminates inside the window

                else {
                    float xClip = static_cast<float>(inVerts[i+1].x);
                    float yClip = static_cast<float>(inVerts[i+1].y);
                    AppendLBClipVertex(clipRect, xClip, yClip, outVerts, j);
                }
            }

            // if 0.0 < tOut2 and tOut2 <= 1.0 then the line segment enters
            // into a final corner region - add the turning vertex

            if (tOut2 <= 1.0) {
                float xClip = static_cast<float>(xOut);
                float yClip = static_cast<float>(yOut);
                AppendLBClipVertex(clipRect, xClip, yClip, outVerts, j);
            }
        }
    }

    nOutVerts = j;

} // end: OpsClipPolygon()


//------------------------------------------------------------------------------
//
// METHOD: OpsClipPolyPolygon().
//
// PURPOSE: Clip a poly-polygon against the specified clip rectangle.
//
// PARAMETERS:
//
//     Input:
//
//         clipRect      - passes a reference to an OpsFloatExtent structure;
//                         the structure pointed to contains the coordinates
//                         of the corners of the clip rectangle.
//         inVertices    - passes an array of OpsFloatPoint structures; the
//                         array contains the vertices of the poly-polygon.
//         nInPolyVerts  - passes an array of integers. The number of
//                         vertices in each polygon of the poly-polygon are
//                         are stored by successive array elements.
//         nInPolygons   - passes the number of polygons.
//
//     Output:
//
//         outVertices   - pointer to an array of OpsFloatPoint structures;
//                         the vertices of the clipped result are copied to
//                         the array pointed to.
//         nOutPolyVerts - pointer to an array of integers. The number of
//                         vertices in each polygon of the clipped poly-
//                         polygon are copied to the array elements.
//         nOutPolygons  - reference to an integer. The number of polygons
//                         in the clipped result is copied to the referenced
//                         integer.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsClipPolyPolygon(const OpsFloatExtent &clipRect,
    const OpsFloatPoint inVertices[], const int nInPolyVerts[],
    int nInPolygons, OpsFloatPoint outVertices[], int nOutPolyVerts[],
    int &nOutPolygons)
{
    // clip each polygon boundary ...

    nOutPolygons = 0;

    for (int i = 0, j = 0, k = 0; i < nInPolygons; i++) {
        // clip next polygon

        int nOutVerts = 0;
        ::OpsClipPolygon(clipRect, &inVertices[j], nInPolyVerts[i],
            &outVertices[k], nOutVerts);

        // if the clipped result is non-empty, then explicitly close

        if (nOutVerts > 2) {
            outVertices[k + nOutVerts] = outVertices[k];
            nOutVerts++;
            k += nOutVerts;
            nOutPolyVerts[nOutPolygons++] = nOutVerts;
        }

        j += nInPolyVerts[i];
    }

} // end: OpsClipPolyPolygon()


//------------------------------------------------------------------------------
//
// METHOD: OpsTranslatePoints().
//
// PURPOSE: Translate a list of points by the specified amounts in x and y.
//
// PARAMETERS
//
//     Input:
//
//         inPoints  - pointer to an array of OpsFloatPoint structures; the
//                     array contains the coordinates of the input points.
//         nPoints   - passes the number of points.
//         deltaX    - passes the amount to translate by in the x direction.
//         deltaY    - passes the amount to translate by in the y direction.
//
//     Output:
//
//         outPoints - pointer to an array of OpsFloatPoint structures; the
//                     array contains the translated points on return.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsTranslatePoints(const OpsFloatPoint inPoints[], int nPoints,
    float deltaX, float deltaY, OpsFloatPoint outPoints[])
{
    for (int i = 0; i < nPoints; i++) {
        outPoints[i].x = inPoints[i].x + deltaX;
        outPoints[i].y = inPoints[i].y + deltaY;
    }

} // end: OpsTranslatePoints()


//------------------------------------------------------------------------------
//
// METHOD: OpsScalePoints().
//
// PURPOSE: Scale a list of points about the origin by the specified
//          scaling factors.
//
// PARAMETERS
//
//     Input:
//
//         inPoints  - passes an array of OpsFloatPoint structures; the
//                     array contains the coordinates of the input points.
//         nPoints   - passes the number of points.
//         xScale    - passes the x scaling factor.
//         yScale    - passes the y scaling factor.
//
//     Output:
//
//         outPoints - passes an array of OpsFloatPoint structures; the
//                     scaled points are copied to this array. outPoints and
//                     inPoints are allowed to point to the same array.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsScalePoints(const OpsFloatPoint inPoints[], int nPoints, float xScale,
    float yScale, OpsFloatPoint outPoints[])
{
    for (int i = 0; i < nPoints; i++) {
        outPoints[i].x = inPoints[i].x * xScale;
        outPoints[i].y = inPoints[i].y * yScale;
    }

} // end: OpsScalePoints()


//------------------------------------------------------------------------------
//
// METHOD: OpsRotatePoints().
//
// PURPOSE: Rotate a list of points about the origin by the specified
//          angle.
//
// PARAMETERS
//
//     Input:
//
//         InPoints       - pointer to an array of OpsFloatPoint structures;
//                          the array contains the coordinates of the input
//                          points.
//         nPoints        - passes the number of points.
//         rotateAngleRad - passes the rotation angle in radians.
//
//     Output:
//
//         outPoints      - pointer to an array of OpsFloatPoint structures;
//                          the array contains the rotated points on return.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsRotatePoints(const OpsFloatPoint inPoints[], int nPoints,
    float rotateAngleRad, OpsFloatPoint outPoints[])
{
    double cosAngle = ::cos(rotateAngleRad);
    double sinAngle = ::sin(rotateAngleRad);

    for (int i = 0; i < nPoints; i++) {
        double x = inPoints[i].x;
        outPoints[i].x = static_cast<float>(x * cosAngle -
            inPoints[i].y * sinAngle);
        outPoints[i].y = static_cast<float>(x * sinAngle +
            inPoints[i].y * cosAngle);
    }

} // end: OpsRotatePoints()
