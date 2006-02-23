//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "stdafx.h"
#include "Centroid.h"
#include <limits>
#include <math.h>
#include <float.h>


void Centroid::PolylineCentroid(double* pts, int numpts, double* cx, double* cy, double* slope)
{
    if (numpts == 0)
    {
        *cx = std::numeric_limits<double>::quiet_NaN();
        *cy = std::numeric_limits<double>::quiet_NaN();
        return;
    }

    // determine halfway-length of the linestring
    double halfLength = 0.5*PolylineLength(pts, numpts);

    // find the segment containing the halfway point
    double x0 = pts[0];
    double y0 = pts[1];
    double x1 = x0;
    double y1 = y0;
    double dx;
    double dy;
    double lineLength = 0.0;
    double walkLength = 0.0;
    for (int j=1; j<numpts; j++)
    {
        x1 = pts[2*j  ];
        y1 = pts[2*j+1];
        dx = x1 - x0;
        dy = y1 - y0;

        lineLength = sqrt(dx*dx + dy*dy);
        if (walkLength + lineLength >= halfLength)
            break;

        walkLength += lineLength;
        x0 = x1;
        y0 = y1;
    }

    // compute the halfway point
    double fact = (lineLength > 0.0)? (halfLength-walkLength)/lineLength : 0.0;
    dx = x1 - x0;
    dy = y1 - y0;

    *cx = x0 + fact*dx;
    *cy = y0 + fact*dy;

    // compute the slope
    if (dx == 0.0)
        *slope = (dy < 0.0)? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
    else
        *slope = dy / dx;
}


/// <summary>
/// Determines whether a point lies inside a polygon defined by a set
/// of contours.  The polygon is specified using two arrays: one
/// defining the coordinates of the polygon vertices and the other
/// defining the number of points in each contour.  The first and
/// last point in each contour can be different.  In this case the
/// contour is closed using an imaginary line between these points.
/// </summary>
/// <param name="pointData">The array defining the 2D point data
/// for the polygon.  Each pair of elements in the array represents
/// a set of X / Y coordinates.</param>
/// <param name="numPointData">The number of actual coordinates in
/// the pointData array.  This must always be less than or equal to
/// the length of the pointData array.</param>
/// <param name="contourSizes">The array defining the number of
/// points in each contour.  Note that the sum of the contour sizes
/// must equal the total number of actual points in the polygon, or
/// half the value of numPointData.</param>
/// <param name="numContourSizes">The number of actual contours in
/// the contourSizes array.  This must always be less than or equal
/// to the length of the contourSizes array.</param>
/// <param name="x">The X coordinate of the point to test.</param>
/// <param name="y">The Y coordinate of the point to test.</param>
/// <returns>True if the test point lies within the polygon.</returns>
bool Centroid::PointInPolygon(double* pointData, int numPointData, int* contourSizes, int numContourSizes, double x, double y )
{
    // verify the number of coordinates is even
    _ASSERT(2*(numPointData / 2) == numPointData);

    if (contourSizes == NULL)
    {
        _ASSERT(numContourSizes == 0);
        numContourSizes = 1;
    }

    bool yflag0, yflag1;
    double vtx0X, vtx0Y, vtx1X, vtx1Y;

    bool inside_flag = false;
    int count = 0;

    for (int i=0; i<numContourSizes; i++)
    {
        int numverts = (contourSizes == NULL)? numPointData / 2 : contourSizes[i];

        // get the last point in the contour
        int temp = count+(numverts*2)-2;
        vtx0X = pointData[temp  ];
        vtx0Y = pointData[temp+1];

        // get test bit for above/below X axis
        yflag0 = (vtx0Y >= y);

        for (int j=0; j<numverts; j++)
        {
            vtx1X = pointData[count++];
            vtx1Y = pointData[count++];

            yflag1 = (vtx1Y >= y);

            // Check if endpoints straddle (are on opposite sides) of X axis
            // (i.e. the Y's differ); if so, +X ray could intersect this edge.
            // The old test also checked whether the endpoints are both to the
            // right or to the left of the test point.  However, given the faster
            // intersection point computation used below, this test was found to
            // be a break-even proposition for most polygons and a loser for
            // triangles (where 50% or more of the edges which survive this test
            // will cross quadrants and so have to have the X intersection computed
            // anyway).  I credit Joseph Samosky with inspiring me to try dropping
            // the "both left or both right" part of my code.
            if (yflag0 != yflag1)
            {
                // Check intersection of pgon segment with +X ray.
                // Note if >= point's X; if so, the ray hits it.
                // The division operation is avoided for the ">=" test by checking
                // the sign of the first vertex wrto the test point; idea inspired
                // by Joseph Samosky's and Mark Haigh-Hutchinson's different
                // polygon inclusion tests.
                if (((vtx1Y-y)*(vtx0X-vtx1X) >=
                        (vtx1X-x)*(vtx0Y-vtx1Y)) == yflag1)
                {
                    inside_flag = !inside_flag;
                }
            }

            // move to the next pair of vertices, retaining info as possible
            yflag0 = yflag1;
            vtx0X = vtx1X;
            vtx0Y = vtx1Y;
        }
    }

    return inside_flag;
}


void Centroid::PolygonCentroid(double* pts, int numpts, double* cx, double* cy)
{
    // handle special cases
    if (numpts == 0)
    {
        *cx = std::numeric_limits<double>::quiet_NaN();
        *cy = std::numeric_limits<double>::quiet_NaN();
        return;
    }
    if (numpts == 1 || numpts == 2)
    {
        *cx = pts[0];
        *cy = pts[1];
        return;
    }
    if (numpts == 3)
    {
        *cx = 0.5 * (pts[0] + pts[2]);
        *cy = 0.5 * (pts[1] + pts[3]);
        return;
    }

    // don't consider the closing point
    numpts -= 1;

    // try TAW centroid first
    PolygonCentroidTAW(pts, numpts, cx, cy);
    if (PointInPolygon(pts, 2*numpts+2, NULL, 0, *cx, *cy))
        return;

    PolygonCentroidBVM(pts, numpts, cx, cy);
    if (PointInPolygon(pts, 2*numpts+2, NULL, 0, *cx, *cy))
        return;

    PolygonCentroidWMC(pts, numpts, cx, cy);
    if (PointInPolygon(pts, 2*numpts+2, NULL, 0, *cx, *cy))
        return;

    //TODO: what do we do if none of the centroids are inside
    //the polygon?
    *cx = std::numeric_limits<double>::quiet_NaN();
    *cy = std::numeric_limits<double>::quiet_NaN();
    return;
}


void Centroid::PolygonCentroidTAW(double* pts, int len, double* cx, double* cy)
{
    _ASSERT(len >= 2);

    double x1, x2, y1, y2, xt1 = 0.0, xt2 = 0.0, yt1 = 0.0, yt2 = 0.0;

    int index = 2;
    for (int i=1; i<len; i++, index+=2)
    {
        x1 = pts[index  ] + pts[index-2];
        x2 = pts[index  ] - pts[index-2];

        y1 = pts[index+1] + pts[index-1];
        y2 = pts[index+1] - pts[index-1];

        xt1 += x1 * x2 * y1;
        xt2 += x2 * y1;

        yt1 += y1 * y2 * x1;
        yt2 += y2 * x1;
    }

    // do it for the close segment also, blah blah duplicated code
    x1 = pts[0] + pts[len*2-2];
    x2 = pts[0] - pts[len*2-2];

    y1 = pts[1] + pts[len*2-1];
    y2 = pts[1] - pts[len*2-1];

    xt1 += x1 * x2 * y1;
    xt2 += x2 * y1;

    yt1 += y1 * y2 * x1;
    yt2 += y2 * x1;

    // compute weighted centroid if xt2 and yt2 are not zero
    if (xt2 != 0.0 && yt2 != 0.0)
    {
        *cx = 0.5 * xt1 / xt2;
        *cy = 0.5 * yt1 / yt2;
    }
}


void Centroid::PolygonCentroidBVM(double* pts, int len, double* cx, double* cy)
{
    double xSum = 0.0, ySum = 0.0;
    for (int i=0; i<len*2; i+=2)
    {
        xSum += pts[i  ];
        ySum += pts[i+1];
    }

    if (len > 0)
    {
        *cx = xSum / len;
        *cy = ySum / len;
    }
}


void Centroid::PolygonCentroidWMC(double* pts, int len, double* cx, double* cy)
{
    _ASSERT(len >= 2);

    double xSum = 0.0, ySum = 0.0, segLength, dx, dy;

    double totalLength = 0.0;
    for (int i=0; i<len*2-2; i+=2)
    {
        dx = pts[i  ] - pts[i+2];
        dy = pts[i+1] - pts[i+3];
        segLength = sqrt(dx*dx + dy*dy);
        totalLength += segLength;
        xSum += pts[i  ] * segLength;
        ySum += pts[i+1] * segLength;
    }

    // add the last segment to the total accumulated length
    dx = pts[0] - pts[len*2-2];
    dy = pts[1] - pts[len*2-1];
    totalLength += sqrt(dx*dx + dy*dy);

    if (totalLength > 0.0)
    {
        *cx = xSum / totalLength;
        *cy = ySum / totalLength;
    }
}


// computes the length of a polyline
double Centroid::PolylineLength(double* pts, int numpts)
{
    if (numpts < 2)
        return 0.0;

    double x0;
    double y0;
    double x1 = pts[0];
    double y1 = pts[1];
    double len = 0.0;
    double dx;
    double dy;
    for (int j=1; j<numpts; j++)
    {
        x0 = x1;
        y0 = y1;
        x1 = pts[2*j  ];
        y1 = pts[2*j+1];

        dx = x1 - x0;
        dy = y1 - y0;
        len += sqrt(dx*dx + dy*dy);
    }

    return len;
}


// computes the area of a polygon
double Centroid::PolygonArea(double* pts, int numpts)
{
    if (numpts < 3)
        return 0.0;

    double sum = 0.0;
    double lx = pts[0];
    double ly = pts[1];

    // sum cross products to get twice the area
    for (int i=2; i<2*numpts; i+=2)
    {
        double cx = pts[i  ];
        double cy = pts[i+1];
        sum += lx*cy - cx*ly;
        lx = cx;
        ly = cy;
    }

    // make sure we add the close segment
    sum += lx*pts[1] - pts[0]*ly;

    return 0.5*fabs(sum);
}
