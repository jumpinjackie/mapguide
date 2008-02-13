//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "LineBuffer.h"
#include "CSysTransformer.h"
#include "RS_OutputStream.h"


// For point reduction loop -- point will be dropped if distance
// between them squared is more than 1.96 (i.e. 1.4 pixels).
// Dave said 1.4 is a good number.
#define OPTIMIZE_DISTANCE_SQ 1.96

// polygons and polylines below this # of points will not be victims of point reduction
#define MIN_RING_SIZE_TO_OPTIMIZE 6

// Cohen - Sutherland outcodes
#define LEFT   0x01
#define RIGHT  0x02
#define TOP    0x04
#define BOTTOM 0x08
#define INSIDE 0x00

#define min4(x1,x2,x3,x4) rs_min(x1, rs_min(x2, rs_min(x3, x4)))
#define max4(x1,x2,x3,x4) rs_max(x1, rs_max(x2, rs_max(x3, x4)))


LineBuffer::LineBuffer(int size, FdoDimensionality dimensionality, bool bIgnoreZ) :
    m_bounds(DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX),
    m_types(NULL),
    m_types_len(0),
    m_cur_types(0),
    m_pts(NULL),
    m_cntrs(NULL),
    m_csp(NULL),
    m_cntrs_len(0),
    m_cur_cntr(-1), //will increment with first MoveTo segment
    m_cur_geom(-1),
    m_geom_type(0),
    m_drawingScale(0.0)
{
    ResizePoints(rs_max(size,2));
    ResizeContours(4);
    m_dimensionality = dimensionality;
    m_bIgnoreZ = bIgnoreZ;
    m_bProcessZ = (m_dimensionality & FdoDimensionality_Z) && !m_bIgnoreZ;
    m_bTransform2DPoints = false;
    m_num_geomcntrs_len = m_cntrs_len;
    m_num_geomcntrs = new int[m_num_geomcntrs_len];
    m_num_geomcntrs[0] = 0;
    if (!m_bProcessZ)
        m_bounds.minz = m_bounds.maxz = 0.0;
}


LineBuffer::LineBuffer() :
    m_bounds(DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, 0.0, 0.0),
    m_types(NULL),
    m_types_len(0),
    m_cur_types(0),
    m_pts(NULL),
    m_cntrs(NULL),
    m_csp(NULL),
    m_cntrs_len(0),
    m_cur_cntr(-1),
    m_cur_geom(-1),
    m_geom_type(0),
    m_drawingScale(0.0),
    m_dimensionality(FdoDimensionality_XY),
    m_bIgnoreZ(true),
    m_bProcessZ(false),
    m_bTransform2DPoints(false),
    m_num_geomcntrs_len(0),
    m_num_geomcntrs(NULL)
{
}


LineBuffer::~LineBuffer()
{
    delete[] m_types;
    delete[] m_pts;
    delete[] m_cntrs;
    delete[] m_csp;
    delete[] m_num_geomcntrs;
}


void LineBuffer::Reset(FdoDimensionality dimensionality, bool bIgnoreZ)
{
    m_cur_types = 0;
    m_cur_cntr = -1;//will increment with first MoveTo segment

    m_bIgnoreZ = bIgnoreZ;
    m_dimensionality = dimensionality;
    m_bProcessZ = (m_dimensionality & FdoDimensionality_Z) && !m_bIgnoreZ;
    m_bTransform2DPoints = false;
    m_cur_geom = -1;
    m_num_geomcntrs[0] = 0;
    m_drawingScale = 0.0;

    m_bounds.minx = m_bounds.miny = +DBL_MAX;
    m_bounds.maxx = m_bounds.maxy = -DBL_MAX;
    if (m_bProcessZ)
    {
        m_bounds.minz = +DBL_MAX;
        m_bounds.maxz = -DBL_MAX;
    }
    else
    {
        m_bounds.minz = 0.0;
        m_bounds.maxz = 0.0;
    }
}


LineBuffer& LineBuffer::operator=(const LineBuffer& src)
{
    m_bIgnoreZ = src.m_bIgnoreZ;
    m_bProcessZ = src.m_bProcessZ;
    m_dimensionality = src.m_dimensionality;
    m_geom_type = src.m_geom_type;
    m_bTransform2DPoints = src.m_bTransform2DPoints;
    m_T = *(const_cast<Matrix3D*>(&src.m_T));   // Matrix3D assignment operator takes non-const src
    m_bounds = src.m_bounds;
    m_drawingScale = src.m_drawingScale;

    // types, points
    if (m_types_len < src.m_cur_types)
    {
        delete[] m_types;
        delete[] m_pts;
        m_types_len = src.m_types_len;
        m_types = new unsigned char[m_types_len];
        m_pts = new double[m_types_len][3];
    }

    m_cur_types = src.m_cur_types;
    memcpy(m_types, src.m_types, m_cur_types);
    memcpy(m_pts, src.m_pts, sizeof(double)*m_cur_types*3);

    // contours
    if (m_cntrs_len <= src.m_cur_cntr)
    {
        delete[] m_cntrs;
        delete[] m_csp;
        m_cntrs_len = src.m_cntrs_len;
        m_cntrs = new int[m_cntrs_len];
        m_csp = new int[m_cntrs_len];
    }

    m_cur_cntr = src.m_cur_cntr;
    memcpy(m_cntrs, src.m_cntrs, sizeof(int)*(1+m_cur_cntr)); // actual use is m_cur_cntr+1
    memcpy(m_csp, src.m_csp, sizeof(int)*(1+m_cur_cntr)); // actual use is m_cur_cntr+1

    // geometries
    if (m_num_geomcntrs_len <= src.m_cur_geom)
    {
        delete [] m_num_geomcntrs;
        m_num_geomcntrs_len = src.m_num_geomcntrs_len;
        m_num_geomcntrs = new int[m_num_geomcntrs_len];
    }

    m_cur_geom = src.m_cur_geom;
    memcpy(m_num_geomcntrs, src.m_num_geomcntrs, sizeof(int)*(1+m_cur_geom));

    return *this;
}


void LineBuffer::SetGeometryType(int geomType)
{
    m_geom_type = geomType;
}


void LineBuffer::NewGeometry()
{
    m_cur_geom++;
    // m_num_geomcntrs_len is initialized to 4 in the constructor,
    // so we shouldn't run into the case where we call
    // ResizeNumGeomContours() passing a value of 0.
    if (m_cur_geom >= m_num_geomcntrs_len)
        ResizeNumGeomContours(m_cur_geom * 2);
    m_num_geomcntrs[m_cur_geom] = 0;
}


void LineBuffer::MoveTo(double x, double y, double z)
{
    if (m_cur_types == m_types_len)
        Resize();

    append_segment(stMoveTo, x, y, z);

    if (m_cur_cntr+1 == m_cntrs_len)
        ResizeContours();
    increment_contour();

    // NewGeometry() is a new API to allow for multiple geometries within a
    // LineBuffer. It is called from LoadFromFgf() but may not be called from
    // code building a LineBuffer using the API. Make sure that NewGeometry()
    // gets called once.
    if (m_cur_geom < 0)
        NewGeometry();
    _ASSERT(m_cur_geom >= 0 && m_cur_geom < m_num_geomcntrs_len);
    // increment the number of contours for this geometry
    m_num_geomcntrs[m_cur_geom] += 1;

    AddToBounds(x, y, z);
}


void LineBuffer::LineTo(double x, double y, double z)
{
    if (m_cur_types == m_types_len)
        Resize();

    // Check to see if incoming points are 2D (no Z) and need to be transfomed
    // back into 3D space.  This is used by 3D circular arc tesselation
    if (m_bTransform2DPoints)
    {
        Point3D pt(x, y, z);
        Point3D result = m_T.Transform(pt);

        x = result.x;
        y = result.y;
        z = result.z;
    }

    append_segment(stLineTo, x, y, z);
    increment_contour_pts();
    AddToBounds(x, y, z);
}


void LineBuffer::Close()
{
    if (m_cur_types == 0)
        return;

    if (m_types[m_cur_types-1] == (unsigned char)stMoveTo)
        return;

    // find out if it is already closed
    if (contour_closed(m_cur_cntr))
        return;

    int cntr_start = m_csp[m_cur_cntr];
    LineTo(x_coord(cntr_start), y_coord(cntr_start), z_coord(cntr_start));
}


void LineBuffer::AddToBounds(double x, double y, double z)
{
    if (x < m_bounds.minx)
        m_bounds.minx = x;

    if (x > m_bounds.maxx)
        m_bounds.maxx = x;

    if (y < m_bounds.miny)
        m_bounds.miny = y;

    if (y > m_bounds.maxy)
        m_bounds.maxy = y;

    if (m_bProcessZ)
    {
        if (z < m_bounds.minz)
            m_bounds.minz = z;

        if (z > m_bounds.maxz)
            m_bounds.maxz = z;
    }
}


void LineBuffer::Resize()
{
    // double the capacity of the arrays
    ResizePoints(2 * point_capacity());
}


void LineBuffer::ResizePoints(int n)
{
    if (n <= point_capacity()) // unnecessary at the very least
        return;

    // new point array
    double (*tempPts)[3] = new double[n][3];

    // copy used data
    if (m_pts && (m_cur_types>0))
        memcpy(tempPts, m_pts, sizeof(double)*m_cur_types*3);

    // cleanup
    delete[] m_pts;
    m_pts = tempPts;

    // new segment type array
    unsigned char* tempTypes = new unsigned char[n];

    // copy used data
    if (m_types && (m_cur_types > 0))
        memcpy(tempTypes, m_types, sizeof(unsigned char) * m_cur_types);

    // cleanup
    delete[] m_types;
    m_types = tempTypes;
    m_types_len = n;
}


void LineBuffer::ResizeContours()
{
    // double the capacity of the contour array
    ResizeContours(2 * m_cntrs_len);
}


void LineBuffer::ResizeContours(int n)
{
    if (n <= m_cntrs_len) // unnecessary at the very least
        return;

    // new contour array
    int* tempCntrs = new int[n];
    int* tempCsp = new int[n];

    // copy data
    if (m_cntrs)
        memcpy(tempCntrs, m_cntrs, sizeof(int)*(1+m_cur_cntr));
    if (m_csp)
        memcpy(tempCsp, m_csp, sizeof(int)*(1+m_cur_cntr));

    // cleanup
    delete[] m_cntrs;
    delete[] m_csp;
    m_cntrs = tempCntrs;
    m_csp = tempCsp;
    m_cntrs_len = n;
}


LineBuffer& LineBuffer::operator+=(LineBuffer& other)
{
    // only if there is something to copy
    if (other.point_count() == 0)
        return *this;

    // make sure there is room
    EnsurePoints(other.point_count());
    EnsureContours(other.cntr_count());

    // copy contours
    memcpy(m_cntrs+m_cur_cntr+1, other.m_cntrs, sizeof(int)*(1+other.m_cur_cntr));
    memcpy(m_csp+m_cur_cntr+1, other.m_csp, sizeof(int)*(1+other.m_cur_cntr));
    for (int i = m_cur_cntr + 1; i < m_cur_cntr + other.m_cur_cntr + 2; ++i)
        m_csp[i] += m_cur_types;
    m_cur_cntr += other.m_cur_cntr + 1; // example: add two single contour buffers new cur contour is index 1

    // copy point data
    memcpy(m_pts + m_cur_types, other.m_pts, sizeof(double)*(other.point_count() * 3));

    // copy segment types
    memcpy(m_types + m_cur_types, other.m_types, other.m_cur_types);
    m_cur_types += other.m_cur_types;

    // copy geometry
    if (m_cur_geom + other.m_cur_geom + 2 > m_num_geomcntrs_len)
        ResizeNumGeomContours(m_cur_geom + other.m_cur_geom + 2);
    memcpy(m_num_geomcntrs + m_cur_geom + 1,
           other.m_num_geomcntrs,
           sizeof(int)*(1+other.m_cur_geom));
    m_cur_geom += other.m_cur_geom + 1;   // follows same pattern as contour

    m_bounds.add_point(RS_F_Point(other.m_bounds.minx, other.m_bounds.miny));
    m_bounds.add_point(RS_F_Point(other.m_bounds.maxx, other.m_bounds.maxy));

    return *this;
}


void LineBuffer::ResizeNumGeomContours(int size)
{
    _ASSERT(size > m_num_geomcntrs_len);
    int* tempCntrs = new int[size];
    memcpy(tempCntrs, m_num_geomcntrs, sizeof(int)*m_num_geomcntrs_len);
    delete[] m_num_geomcntrs;
    m_num_geomcntrs = tempCntrs;
    m_num_geomcntrs_len = size;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////                          Curve tesselation
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void LineBuffer::CircularArcTo(double x1, double y1, double x2, double y2)
{
    _ASSERT(point_count() > 0);

    //get the start point
    double x0, y0, z0;

    // NOTE: This is really an error condition.  The user has specified that
    // LineBuffer should have Z, but is now calling the circular arc routine
    // with no Z value.  Assert and draw nothing.
    _ASSERT(!m_bProcessZ);

    if (!m_bProcessZ)
    {
        last_point(x0, y0, z0);
        CircularArcTo2D(x0, y0, x1, y1, x2, y2);
    }
}


void LineBuffer::CircularArcTo(double x1, double y1, double z1, double x2, double y2, double z2)
{
    _ASSERT(point_count() > 0);

    //get the start point
    double x0, y0, z0;
    last_point(x0, y0, z0);
    if (m_bProcessZ)
        CircularArcTo3D(x0, y0, z0, x1, y1, z1, x2, y2, z2);
    else
        CircularArcTo2D(x0, y0, x1, y1, x2, y2);
}


// 3D circular arc tesselation.
// We transform the 3D circlular arc into the XY Z=0 plane and then tesselate using
// the 2D circular arc tesselation algorithm.  Then we transform the points back
// into the original plane of the 3D circular arc.
void LineBuffer::CircularArcTo3D(double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2)
{
    Point3D start(x0, y0, z0);
    Point3D mid(x1, y1, z1);
    Point3D end(x2, y2, z2);

    // Avoid tesselation if two points are equal.  This is considered an error
    // condition because two points don't define a circular arc segment.
    // This also means we can't draw full circles as there is no way to specify one.
    if (start == mid || mid == end || start == end)
    {
        LineTo(x1, y1, z1);
        LineTo(x2, y2, z2);
        return;
    }

    // Compute normal
    Vector3D p0p2(end.x-start.x, end.y-start.y, end.z-start.z);
    Vector3D p0p1(mid.x-start.x, mid.y-start.y, mid.z-start.z);
    Vector3D normal = p0p2.crossProduct(p0p1);

    // If normal has near zero length, then points are co-linear.
    // Avoid tesselation and draw two line segments
    if (normal.lengthSqrd() < 1.0e-12)
    {
        LineTo(x1, y1, z1);
        LineTo(x2, y2, z2);
        return;
    }
    normal.normalize();

    // Create the coordinate system transform for the arc's coordinate system.
    // Do this by using P0P2 as the major axis, then compute minor axis
    // (third perpendicular) by taking cross product of normal and major axis.
    // Use P0 as the arbitrary origin of the coordinate system.
    Vector3D majorAxis = p0p2;
    majorAxis.normalize();
    Vector3D minorAxis = normal.crossProduct(majorAxis);
    minorAxis.normalize();

    m_T.SetCoordSystem(start, majorAxis, minorAxis, normal);

    // Compute the inverse transform, which will take points from world
    // coordinates into the arc coordinate system.
    Matrix3D Tinverse;
    Tinverse = m_T;
    Tinverse.Invert();

    // transform defining points
    Point3D startPrime = Tinverse.Transform(start);
    Point3D midPrime   = Tinverse.Transform(mid);
    Point3D endPrime   = Tinverse.Transform(end);

    // Turn on flag indicating that the resulting points are 2D and must
    // be transformed back into 3D space.
    m_bTransform2DPoints = true;

    // Tesselate arc in 2d and store in point list
    CircularArcTo2D(startPrime.x, startPrime.y, midPrime.x, midPrime.y, endPrime.x, endPrime.y);

    m_bTransform2DPoints = false;
}


void LineBuffer::CircularArcTo2D(double x0, double y0, double x1, double y1, double x2, double y2)
{
    //now we can compute the circle that those 3 points describe
    double dx1 = x1 - x0;
    double dy1 = y1 - y0;
    double dx2 = x2 - x0;
    double dy2 = y2 - y0;

    double area = 0.5 * (dx1 * dy2 - dx2 * dy1);

    //if it is almost a straight line, avoid numeric instability
    //by just adding two lines instead of tesselating a curve
    double minx = rs_min(x0, rs_min (x1, x2));
    double maxx = rs_max(x0, rs_max (x1, x2));
    double miny = rs_min(y0, rs_min (y1, y2));
    double maxy = rs_max(y0, rs_max (y1, y2));

    double boxArea = (maxx - minx) * (maxy - miny);

    //TODO: in case the area of the bounding box is 0,
    //we may need special handling for arcs that represent a full circle
    if (fabs(boxArea) > 1.0e-20)
    {
        double areaRatio = fabs(area) / boxArea;

        if (areaRatio < 1.0e-10)
        {
            LineTo(x1, y1);
            LineTo(x2, y2);
            return;
        }
    }

    double sqLen10 = dx1 * dx1 + dy1 * dy1;
    double sqLen20 = dx2 * dx2 + dy2 * dy2;
    double invArea = 1.0 / area;

    //find center point and radius of circuscribing circle using
    //formulas from Geometric Tools (Schneider & Eberly)
    double cx = x0 + 0.25 * invArea * (dy2 * sqLen10 - dy1 * sqLen20);
    double cy = y0 + 0.25 * invArea * (dx1 * sqLen20 - dx2 * sqLen10);

    double rdx = cx - x0;
    double rdy = cy - y0;

    double r = sqrt(rdx*rdx + rdy*rdy);

    //start and end angles of circular arc, in radians
    double startAngle = atan2(y0 - cy, x0 - cx);
    double endAngle = atan2(y2 - cy, x2 - cx);

    //now fix the start and end angles so that the cubic approximation
    //tracks the arc in the correct direction
    if (area < 0.0) //CW
    {
        if (startAngle < endAngle)
            startAngle += 2.0 * M_PI;
    }
    else //CCW
    {
        if (startAngle > endAngle)
            endAngle += 2.0 * M_PI;

    }

    ArcTo(cx, cy, r, r, startAngle, endAngle);
}


//computes cubic spline parameter to be used
//when approximating an elliptical arc
double LineBuffer::CubicApproxParameter(double halfAngle)
{
    //small angle case
    if (fabs(halfAngle) < 5.0e-4)
        return (2.0/3.0) * halfAngle;

    //otherwise use sin/cos
    return (4.0/3.0) * (1.0 - cos(halfAngle)) / sin(halfAngle);
}


void LineBuffer::ArcTo(double cx, double cy, double a, double b, double startRad, double endRad)
{
    double extent = endRad - startRad;
    if (extent == 0.0)
        return;

    double extentA = fabs(extent);
    if (extentA > PI2)
    {
        extent  = PI2;  // sign doesn't matter anymore
        extentA = PI2;
    }

    //we will approximate one full ellipse by 8 cubics
    //so here compute what portion of a full ellipse we have
    //so that we know how many cubics will be needed
    int num_segs = (int)ceil(8.0 * extentA / PI2);
    double increment = extent / num_segs;

    double angle = startRad;
    double ellx = cos(angle);
    double elly = sin(angle);

    double Ke = CubicApproxParameter(increment*0.5);

    double c0x, c0y, c3x(0.0), c3y(0.0);
    for (int i=0; i<num_segs; i++)
    {
        // move to start point
        if (i==0)
        {
            c0x = cx + a * ellx;
            c0y = cy + b * elly;
            LineTo(c0x, c0y);
        }
        else
        {
            c0x = c3x;
            c0y = c3y;
        }

        double c1x = cx + a*(ellx - Ke * elly);
        double c1y = cy + b*(elly + Ke * ellx);

        angle += increment;
        ellx = cos(angle);
        elly = sin(angle);

        double c2x = cx + a * (ellx + Ke * elly);
        double c2y = cy + b * (elly - Ke * ellx);
        c3x = cx + ellx * a;
        c3y = cy + elly * b;

        TesselateCubicTo(c0x, c0y, c1x, c1y, c2x, c2y, c3x, c3y);
    }
}


void LineBuffer::TesselateCubicTo(double px1, double py1, double px2, double py2, double px3, double py3, double px4, double py4)
{
    double w = m_bounds.width();
    double h = m_bounds.height();
    double minSegLen = 0.0;
    double dt = INV_TESSELATION_ITERATIONS; //= 1.0 / 100.0 Iterate 100 times through loop

    if (m_drawingScale != 0.0)
    {
        // If we have drawing scale available, use drawing scale to make sure
        // the minimum segment length is equal to # of drawing units occupied
        // by one pixel.  This ensures no "flat" segments are visible along
        // the curve.
//      minSegLen = m_drawingScale;

        // Setting the minimum segment length to the drawing scale creates so
        // many segments in dense drawings at high zoom levels that we run out
        // of memory.  Use the larger of the drawing scale and one two hundredth
        // of the "size" of the arc we are tesselating.  This compromise seems
        // to work in all "reasonable" cases.

        // Compute bounding box of the four control points of the curve
        double minx = min4(px1, px2, px3, px4);
        double miny = min4(py1, py2, py3, py4);
        double maxx = max4(px1, px2, px3, px4);
        double maxy = max4(py1, py2, py3, py4);

        double deltax = maxx - minx;
        double deltay = maxy - miny;

        double maxlength = rs_max(deltax, deltay);

        // 0.005 (1/200) is an arbitrary number, but it seems to work well
        minSegLen = rs_max(m_drawingScale, 0.005*maxlength);

        // This code will compute the # of pixels occupied by the curve segment.
        // It can then reduce the # of iterations in the forward differencing
        // loop to that # of pixels.  This reduces the time taken to generate
        // each segment of the tessellation.

        // Figure out how many pixels are covered by that max length.
        double numPixels = maxlength / m_drawingScale;

        // Ensure that the # of loop iterations is at most 100 or # of pixels
        // covered (whichever is smaller).
        dt = rs_max(INV_TESSELATION_ITERATIONS, (1.0 / numPixels));
    }
    else
    {
        // We will base the max number of segments to use for approximation
        // on the bounds of the full line buffer contents.
        // TODO: as an improvement we could take the bounds of this particular
        // curve with respect to the full bounds of the line buffer data.
        double maxdim = rs_max(w, h);

        // minimum length of tesselation segment, set to 1/100 of the bounds
        minSegLen = maxdim * 0.01;
    }

//  double dt2 = dt*dt;
    double dt3 = dt * dt * dt;

    double pre1 = 3.0 * dt;
    double pre2 = pre1 * dt;
    double pre3 = pre2 + pre2;
    double pre4 = 6.0 * dt3;

    double temp1x = px1 - 2.0 * px2 + px3;
    double temp1y = py1 - 2.0 * py2 + py3;
    double temp2x = 3.0 * (px2 - px3) - px1 + px4;
    double temp2y = 3.0 * (py2 - py3) - py1 + py4;

    double fx    = px1;
    double fy    = py1;
    double dfx   = (px2 - px1) * pre1 + temp1x * pre2 + temp2x * dt3;
    double dfy   = (py2 - py1) * pre1 + temp1y * pre2 + temp2y * dt3;
    double ddfx  = temp1x * pre3 + temp2x * pre4;
    double ddfy  = temp1y * pre3 + temp2y * pre4;
    double dddfx = temp2x * pre4;
    double dddfy = temp2y * pre4;

    double error = 0.0;

    // forward differencing loop
    int tMax = (int)(1.0/dt - 0.5);
    for (int t=0; t<tMax; t++)
    {
        fx   += dfx;
        fy   += dfy;
        dfx  += ddfx;
        dfy  += ddfy;
        ddfy += dddfy;
        ddfx += dddfx;

//      error += sqrt(dfx*dfx + dfy*dfy);   //slow but accurate for flattening

        // faster but less accurate error estimate
        w = fabs(dfx);
        h = fabs(dfy);
        error += rs_max(w, h);
        if (error >= minSegLen) // add segment only if we have reached threshold length
        {
            // line to current
            LineTo(fx, fy);
            error = 0.0;
        }
    }

    LineTo(px4, py4);
}


void LineBuffer::TesselateQuadTo(double px1, double py1, double px2, double py2, double px3, double py3)
{
    //we will base the max number of segments to use for approximation
    //on the bounds of the full line buffer contents
    //TODO: as an improvement we could take the bounds of this particular curve
    //with respect to the full bounds of the line buffer data.
    double w = m_bounds.width();
    double h = m_bounds.height();
    double maxdim = rs_max(w, h);

    //minimum length of tesselation segment
    //set to 1/100 of the bounds
    double minSegLen = maxdim * 0.01;

    /*
    //if we know the pixels per mapping unit ratio, we can use
    //this code to determine how many times at most
    //we want to loop in the forward difference loop

    double invscale = 1.0/pixelsperunit;
    double dt = rs_max(1.0e-2, invscale / diff); // * m_invscale;
    */

    //but for now we will iterate 100 times
    double dt = INV_TESSELATION_ITERATIONS;

    double dt2 = dt*dt;

    double ax = px1 - 2.0*px2 + px3;        //replace 2* by addition?
    double ay = py1 - 2.0*py2 + py3;        //replace 2* by addition?

    double bx = 2.0*(px2-px1);
    double by = 2.0*(py2-py1);

    double fx   = px1;
    double fy   = py1;
    double dfx  = bx*dt + ax*dt2;
    double dfy  = by*dt + ay*dt2;
    double ddfx = 2.0*ax*dt2;
    double ddfy = 2.0*ay*dt2;

    double error = 0.0;

    //forward differencing loop
    int tMax = (int)(1.0/dt - 0.5);
    for (int t=0; t<tMax; t++)
    {
        fx   += dfx;
        fy   += dfy;
        dfx  += ddfx;
        dfy  += ddfy;

        // slow but accurate
        //error += sqrt(dfx*dfx + dfy*dfy);

        //faster but less accurste error estimate
        w = fabs(dfx);
        h = fabs(dfy);
        error += rs_max(w, h);

        if (error >= minSegLen)  // how many pixels should each line be?)
        {
            LineTo(fx, fy);
            error = 0.0;
        }
    }

    LineTo(px3, py3);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////                          AGF reader
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//simple helper macro that reads x and y and (possibly) z and skips m and does coord sys transform
#define READ_POINT(x, y, z) \
    x = *dreader++;         \
    y = *dreader++;         \
    if (m_bProcessZ)        \
        z = *dreader++;     \
    else                    \
        z = 0.0;            \
    dreader += skip;        \
    if (xformer)            \
        xformer->TransformPoint(x, y);


void LineBuffer::LoadFromAgf(unsigned char* RESTRICT data, int /*sz*/, CSysTransformer* xformer)
{
    int* ireader = (int*)data;

    // the geometry type
    m_geom_type = (FdoGeometryType)*ireader++;

    switch (m_geom_type)
    {
        // all the linear types...
        case FdoGeometryType_MultiLineString:
        case FdoGeometryType_MultiPolygon:
        case FdoGeometryType_MultiPoint:
        case FdoGeometryType_LineString:
        case FdoGeometryType_Polygon:
        case FdoGeometryType_Point:
        {
            bool is_multi = (m_geom_type == FdoGeometryType_MultiLineString)
                || (m_geom_type == FdoGeometryType_MultiPolygon
                || (m_geom_type == FdoGeometryType_MultiPoint));

            // the coordinate type
            int skip = 0; //0=XY, 1=XYZ or XYM, 2 = XYZM
            int num_geoms = 1;
            double x;
            double y;
            double z;

            //in case of multipolygon or multilinestring or multipoint,
            //read poly or linestring count
            if (is_multi)
                num_geoms = *ireader++;

            for (int q=0; q<num_geoms; q++)
            {
                //skip past geometry type of subgeometry
                //we know it is LineString or Polygon or Point respectively
                if (is_multi) *ireader++;

                //read cordinate typeB
                FdoDimensionality dim = (FdoDimensionality)*ireader++;

                // ensure that all dimensionalities of each geometry are the same
                _ASSERT(q==0 || m_dimensionality == dim);

                m_dimensionality = dim;
                m_bProcessZ = (m_dimensionality & FdoDimensionality_Z) && !m_bIgnoreZ;

                skip = 0;
                if ((dim & FdoDimensionality_Z) && m_bIgnoreZ) skip++;
                if (dim & FdoDimensionality_M) skip++;

                // the number of contours in current polygon/linestring
                int contour_count = 1; //for linestrings

                if (m_geom_type == FdoGeometryType_Polygon ||
                    m_geom_type == FdoGeometryType_MultiPolygon)
                    contour_count = *ireader++;

                for (int i=0; i<contour_count; i++)
                {
                    int point_count = 1;

                    //point geoms do not have a point count, since
                    //each piece is just one point each
                    if (m_geom_type != FdoGeometryType_MultiPoint &&
                        m_geom_type != FdoGeometryType_Point)
                        point_count = *ireader++;

                    //*** ireader not valid from here down
                    double* dreader = (double*) ireader;

                    READ_POINT(x, y, z);

                    int num_pts_read = 1;

                    MoveTo(x, y, z);

                    if (point_count == 1)
                    {
                        //if current contour is just a point, add a second point
                        //for easier time in the line style algorithm
                        //but only do this for line and polygons, not points!!!!!
                        if (m_geom_type != FdoGeometryType_MultiPoint &&
                            m_geom_type != FdoGeometryType_Point)
                            LineTo(x, y, z);
                    }
                    else
                    {
                        while (num_pts_read  < point_count)
                        {
                            READ_POINT(x, y, z);

                            LineTo(x, y, z);

                            num_pts_read++;
                        }
                    }

                    ireader = (int*)dreader;
                    //*** ireader valid again
                }
            }
            break;
        }

        // all the non-linear types...
        case FdoGeometryType_CurveString:
        case FdoGeometryType_CurvePolygon:
        case FdoGeometryType_MultiCurveString:
        case FdoGeometryType_MultiCurvePolygon:
        {
            bool is_multi = false;

            int real_geom_type = m_geom_type;

            // change geometry type over to flattened type
            switch (m_geom_type)
            {
                case FdoGeometryType_CurveString:
                    m_geom_type = FdoGeometryType_LineString;
                    break;
                case FdoGeometryType_CurvePolygon:
                    m_geom_type = FdoGeometryType_Polygon;
                    break;
                case FdoGeometryType_MultiCurveString:
                    m_geom_type = FdoGeometryType_MultiLineString;
                    is_multi = true;
                    break;
                case FdoGeometryType_MultiCurvePolygon:
                    m_geom_type = FdoGeometryType_MultiPolygon;
                    is_multi = true;
                    break;
            }

            // the coordinate type
            int skip = 0; //0=XY, 1=XYZ or XYM, 2 = XYZM
            int num_geoms = 1;
            double x;
            double y;
            double z;

            //in case of multicurvepolygon or multicurvestring
            //read poly or linestring count
            if (is_multi)
                num_geoms = *ireader++;

            for (int q=0; q<num_geoms; q++)
            {
                //skip past geometry type of subgeometry
                //we know it is CurveString or CurvePolygon respectively
                if (is_multi)
                    *ireader++;

                //read cordinate typeB
                FdoDimensionality dim = (FdoDimensionality)*ireader++;

                // ensure that all dimensionalities of each geometry are the same
                _ASSERT(q==0 || m_dimensionality == dim);

                m_dimensionality = dim;
                m_bProcessZ = (m_dimensionality & FdoDimensionality_Z) && !m_bIgnoreZ;

                skip = 0;
                if ((dim & FdoDimensionality_Z) && m_bIgnoreZ) skip++;
                if (dim & FdoDimensionality_M) skip++;

                // the number of contours in current polygon/linestring
                int contour_count = 1; //for linestrings, no rings, just one

                if (real_geom_type == FdoGeometryType_CurvePolygon ||
                    real_geom_type == FdoGeometryType_MultiCurvePolygon)
                    contour_count = *ireader++; //#rings for polygons

                for (int i=0; i<contour_count; i++)
                {
                    //*** ireader not valid from here down
                    double* dreader = (double*) ireader;

                    READ_POINT(x, y, z);

                    MoveTo(x, y, z); //first point is always a move

                    // ireader valid again
                    ireader = (int*)dreader;
                    int seg_count = *ireader++; //# curve segments

                    for (int j = 0; j < seg_count; j++)
                    {
                        int seg_type = *ireader++;

                        switch (seg_type)
                        {
                        case FdoGeometryComponentType_CircularArcSegment:
                            dreader = (double*)ireader;

                            //circular arc: read midpont and endpoint
                            //first point was either the start point or
                            //the last point if the previous segment

                            double midx;
                            double midy;
                            double midz;

                            READ_POINT(midx, midy, midz);

                            double endx;
                            double endy;
                            double endz;

                            READ_POINT(endx, endy, endz);

                            CircularArcTo(midx, midy, midz, endx, endy, endz);

                            ireader = (int*)dreader;
                            break;

                        case FdoGeometryComponentType_LineStringSegment:

                            //line string segment -- just read the points
                            //and do LineTos

                            int num_pts = *ireader++;
                            dreader = (double*)ireader;

                            for (int k = 0; k < num_pts; k++)
                            {
                                READ_POINT(x, y, z);
                                LineTo(x, y, z);
                            }

                            ireader = (int*)dreader;
                            break;
                        }
                    }

                    if (m_geom_type == FdoGeometryType_Polygon ||
                        m_geom_type == FdoGeometryType_MultiPolygon)
                    {
                        Close();
                    }
                } //contour count of polygons
            }
            break;
        }

        case FdoGeometryType_MultiGeometry:
        {
            //can't do that yet
            break;
        }
    }
}


#define WRITE_INT(os, val) { \
    int val2 = val;          \
    os->write(&val2, 4);   } \

#define WRITE_DOUBLE(os, val) { \
    double val3 = val   ;       \
    os->write(&val3, 8);   }    \


//AGF writer
void LineBuffer::ToAgf(RS_OutputStream* os)
{
    int ptindex = 0;
    int cntrindex = 0;

    switch (m_geom_type)
    {
        //all the linear types...
        case FdoGeometryType_MultiLineString:
        case FdoGeometryType_MultiPolygon:
        case FdoGeometryType_MultiPoint:
        case FdoGeometryType_LineString:
        case FdoGeometryType_Polygon:
        case FdoGeometryType_Point:
        {
            //write geometry type
            WRITE_INT(os, m_geom_type);

            bool is_multi = (m_geom_type == FdoGeometryType_MultiLineString ||
                             m_geom_type == FdoGeometryType_MultiPolygon ||
                             m_geom_type == FdoGeometryType_MultiPoint);

            // the coordinate type
            //int skip = 0; //0=XY, 1=XYZ or XYM, 2 = XYZM
            double x;
            double y;
            double z;

            //in case of multipolygon or multilinestring or multipoint,
            //read poly or linestring count
            int num_geoms = m_cur_geom + 1;
            if (is_multi)
            {
                //write number of geometries
                WRITE_INT(os, num_geoms);
            }

            for (int q=0; q<num_geoms; q++)
            {
                //skip past geometry type of subgeometry
                //we know it is LineString or Polygon or Point respectively
                if (is_multi)
                {
                    switch (m_geom_type)
                    {
                    case FdoGeometryType_MultiLineString:
                        WRITE_INT(os, FdoGeometryType_LineString);
                        break;
                    case FdoGeometryType_MultiPolygon:
                        WRITE_INT(os, FdoGeometryType_Polygon);
                        break;
                    case FdoGeometryType_MultiPoint:
                        WRITE_INT(os, FdoGeometryType_Point);
                        break;
                    }
                }

                //write cordinate type
                // If user specifies ignored Z, then we don't have Z values
                // even though m_dimensionality indicates we do.
                // Write out FdoDimensionality_XY in this case.
                if (m_bProcessZ)
                {
                    WRITE_INT(os, m_dimensionality);
                }
                else
                {
                    WRITE_INT(os, FdoDimensionality_XY);
                }

                // the number of contours in current polygon/linestring
                int contour_count = 1; //for linestrings

                if (m_geom_type == FdoGeometryType_MultiPolygon ||
                    m_geom_type == FdoGeometryType_Polygon)
                {
                    contour_count = m_num_geomcntrs[q];
                    WRITE_INT(os, contour_count);
                }

                for (int i=cntrindex; i< cntrindex + contour_count; i++)
                {
                    int point_count = 1;

                    //point geoms do not have a point count, since
                    //each piece is just one point each
                    if ((m_geom_type != FdoGeometryType_MultiPoint)
                        && (m_geom_type != FdoGeometryType_Point))
                    {
                        point_count = m_cntrs[i];
                        WRITE_INT(os, point_count);
                    }

                    int num_pts_written = 0;

                    while (num_pts_written < point_count)
                    {
                        if (m_bProcessZ)
                        {
                            x = m_pts[ptindex][0];
                            y = m_pts[ptindex][1];
                            z = m_pts[ptindex][2];
                            WRITE_DOUBLE(os, x);
                            WRITE_DOUBLE(os, y);
                            WRITE_DOUBLE(os, z);
                        }
                        else
                        {
                            x = m_pts[ptindex][0];
                            y = m_pts[ptindex][1];
                            WRITE_DOUBLE(os, x);
                            WRITE_DOUBLE(os, y);
                        }
                        ptindex++;

                        num_pts_written++;
                    }
                }

                cntrindex += contour_count;
                if (cntrindex >= m_cntrs_len && q + 1 < num_geoms)
                {
                    _ASSERT(cntrindex < m_cntrs_len);
                    break;
                }
            }
            break;
        }

        case FdoGeometryType_CurveString:
        case FdoGeometryType_CurvePolygon:
        case FdoGeometryType_MultiCurveString:
        case FdoGeometryType_MultiCurvePolygon:
        {
            //TODO: the LineBuffer is already tesselated, so in case of these
            //we need to actually use a tesselated type, i.e. the code above
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////                          Geometric operations
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//WARNING: caller responsible for deleting resulting line buffer
LineBuffer* LineBuffer::Optimize(double drawingScale, LineBufferPool* lbp)
{
    // the minimum allowed separation
    double d2Min = OPTIMIZE_DISTANCE_SQ * drawingScale * drawingScale;

    //don't set any offset for new linebuffer, because it is already applied,
    //and MoveTo and LineTo would apply it again. Instead, set at end
    LineBuffer* ret = LineBufferPool::NewLineBuffer(lbp, m_cur_types, m_dimensionality, m_bIgnoreZ);

    //optimization
    int index = 0;
    double x, y, z=0.0, lx, ly, lz=0.0;
    for (int i=0; i< m_cur_cntr+1; i++)
    {
        int numPoints = m_cntrs[i];

        //if not enough points, just add the entire contour
        if (numPoints < MIN_RING_SIZE_TO_OPTIMIZE)
        {
            x = m_pts[index][0];
            y = m_pts[index][1];
            if (m_bProcessZ)
                z = m_pts[index][2];
            ret->MoveTo(x, y, z);
            index++;

            for (int j=1; j< numPoints; j++)
            {
                x = m_pts[index][0];
                y = m_pts[index][1];
                if (m_bProcessZ)
                    z = m_pts[index][2];
                ret->LineTo(x, y, z);
                index++;
            }
        }
        else
        {
            //add first point
            lx = m_pts[index][0];
            ly = m_pts[index][1];
            if (m_bProcessZ)
                lz = m_pts[index][2];
            ret->MoveTo(lx, ly, lz);
            index++;
            int numAdded = 1;

            //middle points
            for (int j=1; j< numPoints-1; j++)
            {
                x = m_pts[index][0];
                y = m_pts[index][1];
                if (m_bProcessZ)
                    z = m_pts[index][2];
                index++;
                // always ensure we add at least 2 middle points
                int numRemaining = numPoints - j - 1;
                int numRequired  = 3 - numAdded;
                if (numRequired >= numRemaining)
                {
                    ret->LineTo(x, y, z);
                    numAdded++;
                    lx = x;
                    ly = y;
                    lz = z;
                    continue;
                }

                double dx = x - lx;
                double dy = y - ly;
                double dz = z - lz;
                double dist2;
                if (m_bProcessZ)
                    dist2 = dx*dx + dy*dy + dz*dz;
                else
                    dist2 = dx*dx + dy*dy;

                if (dist2 >= d2Min)
                {
                    ret->LineTo(x, y, z);
                    numAdded++;
                    lx = x;
                    ly = y;
                    lz = z;
                }
            }

            //add last point for to ensure closure
            x = m_pts[index][0];
            y = m_pts[index][1];
            if (m_bProcessZ)
                z = m_pts[index][2];
            ret->LineTo(x, y, z);
            index++;
        }
    }

    return ret;
}


//WARNING: caller is responsible for deleting resulting line buffer
//if return is equal to this pointer, no clipping was needed (geometry was
//fully inside given box), so no need to free it
//if return pointer is NULL, geometry was fully outside the clip box
LineBuffer* LineBuffer::Clip(RS_Bounds& b, GeomOperationType clipType, LineBufferPool* lbp)
{
    // We don't handle 3D clipping correctly yet
    // TODO: Implement 3D clipping if necessary
    if (hasZ())
        return this;

    //check if line buffer is fully contained in box
    if (   b.minx <= m_bounds.minx
        && b.maxx >= m_bounds.maxx
        && b.miny <= m_bounds.miny
        && b.maxy >= m_bounds.maxy)
        return this;

    //check if line buffer is completely outside box
    if (   m_bounds.minx > b.maxx
        || m_bounds.miny > b.maxy
        || m_bounds.maxx < b.minx
        || m_bounds.maxy < b.miny)
        return NULL;

    LineBuffer* dest = LineBufferPool::NewLineBuffer(lbp, m_cur_types, m_dimensionality, m_bIgnoreZ);

    if (clipType == ctArea)
        return ClipPolygon(b, dest);
    else if (clipType == ctLine)
        return ClipPolyline(b, dest);
    else if (clipType == ctPoint)
        return ClipPoints(b, dest);
    else if (clipType == ctAGF)
    {
        switch (m_geom_type)
        {
            case FdoGeometryType_MultiPolygon:
            case FdoGeometryType_Polygon:
                return ClipPolygon(b, dest);
            case FdoGeometryType_MultiLineString:
            case FdoGeometryType_LineString:
                return ClipPolyline(b, dest);
            case FdoGeometryType_Point:
            case FdoGeometryType_MultiPoint:
                return ClipPoints(b, dest);
            default:
            {
                LineBufferPool::FreeLineBuffer(lbp, dest);
                return NULL;
            }
        }
    }
    else
    {
        LineBufferPool::FreeLineBuffer(lbp, dest);
        return NULL;
    }
}


LineBuffer* LineBuffer::ClipPoints(RS_Bounds& b, LineBuffer* dst)
{
    dst->m_geom_type = m_geom_type;

    for (int i=0; i<point_count(); i++)
    {
        double x = m_pts[i][0];
        double y = m_pts[i][1];

        if (x >= b.minx && y >= b.miny &&
            x <= b.maxx && y <= b.maxy)
        {
            dst->MoveTo(x, y);
        }
    }

    return dst;
}


LineBuffer* LineBuffer::ClipPolygon(RS_Bounds& b, LineBuffer* dest)
{
    _ASSERT(dest);

    //TODO: NOT Z AWARE
    //TODO: handle polygons that become multipolygons when clipped
    dest->m_geom_type = m_geom_type;

    //unlike polylines, here we don't need to exapnd the clip region
    //because we will still get a polygon point on the edge if we throw
    //away one which is just outside the edge (due to floating point precision)
    RS_Bounds clipRect( b.minx, b.miny, b.maxx, b.maxy);

    LineBuffer* ret = dest;

    double xIn, xOut, yIn, yOut, tInX, tInY, tOutX, tOutY, tIn2, tOut1, tOut2;
    double aline[4];

    bool move = false;

    //loop over all segments, MoveTos denote start of polygon
    //in a multipolygon
    for (int i=0; i<m_cur_types; i++)
    {
        if (m_types[i] == (unsigned char)stMoveTo)
        {
            //if last segment added to ret was a move, roll it back
            //otherwise close the last contour
            if (i)
                ret->FinalizeContour();
            move = true;
        }
        else if (m_types[i] == (unsigned char)stLineTo)
        {
            get_point(i-1, aline[0], aline[1]);
            get_point(i  , aline[2], aline[3]);

            // determine the direction of the edge
            double deltaX = aline[2] - aline[0];
            double deltaY = aline[3] - aline[1];

            // determine which bounding lines for the clip rectangle the containing
            // line first hits
            if (deltaX > 0.0 || (deltaX == 0.0 && aline[0] > clipRect.maxx))
            {
                xIn  = clipRect.minx;
                xOut = clipRect.maxx;
            }
            else
            {
                xIn  = clipRect.maxx;
                xOut = clipRect.minx;
            }

            if (deltaY > 0.0 || (deltaY == 0.0 && aline[1] > clipRect.maxy))
            {
                yIn  = clipRect.miny;
                yOut = clipRect.maxy;
            }
            else
            {
                yIn  = clipRect.maxy;
                yOut = clipRect.miny;
            }

            // find the t values for x and y exit points
            if (deltaX != 0.0)
                tOutX = (xOut - aline[0]) / deltaX;
            else if (clipRect.minx<=aline[0] && aline[0]<=clipRect.maxx)
                tOutX = std::numeric_limits<double>::infinity();
            else
                tOutX = -std::numeric_limits<double>::infinity();

            if (deltaY != 0.0)
                tOutY = (yOut - aline[1]) / deltaY;
            else if (clipRect.miny<=aline[1]&& aline[1]<=clipRect.maxy)
                tOutY = std::numeric_limits<double>::infinity();
            else
                tOutY = -std::numeric_limits<double>::infinity();

            // Order the two exits points
            if (tOutX < tOutY)
            {
                tOut1 = tOutX;
                tOut2 = tOutY;
            }
            else
            {
                tOut1 = tOutY;
                tOut2 = tOutX;
            }

            if (tOut2 > 0.0)
            {
                // compute tIn2
                if (deltaX != 0.0)
                    tInX = (xIn - aline[0]) / deltaX;
                else
                    tInX = -std::numeric_limits<double>::infinity();

                if (deltaY != 0.0)
                    tInY = (yIn - aline[1]) / deltaY;
                else
                    tInY = -std::numeric_limits<double>::infinity();

                if (tInX < tInY)
                    tIn2 = tInY;
                else
                    tIn2 = tInX;

                // if tOut1 < tIn2 then there is no visible segment - check for a
                // a possible turning vertex
                if (tOut1 < tIn2)
                {
                    // if tOut1 > 0.0 and tOut1 < 1.0 then line crosses over
                    // intermediate corner region - generate turning vertex
                    if (tOut1 > 0.0 && tOut1 <= 1.0)
                    {
                        double xClip;
                        double yClip;

                        if (tInX < tInY)
                        {
                            xClip = xOut;
                            yClip = yIn;
                        }
                        else
                        {
                            xClip = xIn;
                            yClip = yOut;
                        }

                        AppendLBClipVertex(clipRect, xClip, yClip, ret, move);
                        move = false;
                    }
                }

                // else if tOut1 > 0.0 and tIn2 <=1.0 then there is a visible
                // segment
                else if (tOut1 > 0.0 && tIn2 <= 1.0)
                {
                    // if tIn2 > 0.0 then the line segment begins outside the
                    // window - find the entering intersection
                    if (tIn2 > 0.0)
                    {
                        double xClip;
                        double yClip;

                        if (tInX > tInY)
                        {
                            xClip = xIn;
                            yClip = aline[1] + tInX * deltaY;
                        }
                        else
                        {
                            xClip = aline[0] + tInY * deltaX;
                            yClip = yIn;
                        }

                        AppendLBClipVertex(clipRect, xClip, yClip, ret, move);
                        move = false;
                    }

                    // if tOut1 < 1.0 then the line segment terminates outside the
                    // window - find the exiting intersection
                    if (tOut1 < 1.0)
                    {
                        double xClip;
                        double yClip;

                        if (tOutX < tOutY)
                        {
                            xClip = xOut;
                            yClip = aline[1] + tOutX * deltaY;
                        }
                        else
                        {
                            xClip = aline[0] + tOutY * deltaX;
                            yClip = yOut;
                        }

                        AppendLBClipVertex(clipRect, xClip, yClip, ret, move);
                        move = false;
                    }

                    // else the line segment terminates inside the window
                    else
                    {
                        double xClip = aline[2];
                        double yClip = aline[3];
                        AppendLBClipVertex(clipRect, xClip, yClip, ret, move);
                        move = false;
                    }
                }

                // if 0.0 < tOut2 and tOut2 <= 1.0 then the line segment enters
                // into a final corner region - add the turning vertex
                if (tOut2 <= 1.0)
                {
                    double xClip = xOut;
                    double yClip = yOut;
                    AppendLBClipVertex(clipRect, xClip, yClip, ret, move);
                    move = false;
                }
            }
        }
    }

    //roll back or close the last contour
    ret->FinalizeContour();

    return ret;
}


// called by the polygon clipper in order to weed out contours that are incomplete
// after clipping (i.e. they have a single point or two points)
void LineBuffer::FinalizeContour()
{
    //if last segment added to ret was a move, roll it back one point
    if (m_cur_types > 0 && m_types[m_cur_types-1] == (unsigned char)stMoveTo)
    {
        m_cur_types --; //roll back the MoveTo segment
        m_cur_cntr --;  //roll back the contour count by 1 also
        m_num_geomcntrs[m_cur_geom] -= 1;

        if (m_num_geomcntrs[m_cur_geom] == 0)
            m_cur_geom--;
    }
    //if last segment was only a line, take it out also
    else if (m_cur_types > 1 && m_types[m_cur_types-2] == (unsigned char)stMoveTo)
    {
        m_cur_types -= 2;   //roll back 2 segment types
        m_cur_cntr --;      //roll back the contour count by 1

        m_num_geomcntrs[m_cur_geom] -= 1;

        if (m_num_geomcntrs[m_cur_geom] == 0)
            m_cur_geom--;
    }
    else
    {
        Close();
    }
}


//------------------------------------------------------------------------------
//
// FUNCTION: AppendLBClipVertex().
//
// PURPOSE: Append a vertex from the Liang-Barsky polygon clipping algorithm to
//          the end of an output LineBuffer. Degenerate edges are culled.
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
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------
void LineBuffer::AppendLBClipVertex(RS_Bounds& clipRect, double x, double y, LineBuffer* lb, bool move)
{
    // if there is at least one edge in the output array, then determine if the
    // edge defined by the last two vertices in the array plus the new clip
    // vertex induce a degenerate edge; the edge is considered degenerate if
    // if doubles back on the last edge in the output array, and lies along
    // one of the boundaries of the clip rectangle

    // TODO: NOT Z AWARE
    bool degenerate = false;

    //only line segments can be degenerate -- a move indicates the start of a new
    //polygon, so it is not degenerate.
    int npts = move ? 0 : lb->m_cntrs[lb->m_cur_cntr];

    if (npts > 1)
    {
        double x1, y1, x2, y2;
        lb->get_point(lb->point_count()-1, x1, y1);
        lb->get_point(lb->point_count()-2, x2, y2);

        degenerate = (x == x1 && x == x2 &&
            (x == clipRect.minx || x == clipRect.maxx) &&
            ((y <= y1 && y2 <= y1) || (y >= y1 && y2 >= y1))) ||
            (y == y1 && y == y2 &&
            (y == clipRect.miny || y == clipRect.maxy) &&
            ((x <= x1 && x2 <= x1) || (x >= x1 && x2 >= x1)));
    }

    // else if there is only one vertex, and the new vertex is identical, then
    // flag as degenerate
    else if (npts == 1)
    {
        degenerate =   x == lb->x_coord(lb->point_count()-1)
                    && y == lb->y_coord(lb->point_count()-1);
    }


    // else not degenerate
    else
        degenerate = false;

    // if the new vertex does not induce a degeneracy, then just append
    if (!degenerate)
    {
        if (npts == 0)
            lb->MoveTo(x, y);
        else
            lb->LineTo(x, y);
    }

    // else overwrite the last vertex with the new vertex - this will eliminate
    // the degeneracy
    else
    {
        lb->x_coord(lb->point_count()-1) = x;
        lb->y_coord(lb->point_count()-1) = y;
    }
}


LineBuffer* LineBuffer::ClipPolyline(RS_Bounds& b, LineBuffer* dest)
{
    _ASSERT(dest);

    //TODO: NOT Z AWARE
    //TODO: handle polylines that become multipolylines when clipped
    dest->m_geom_type = m_geom_type;

    // expand clip region a little so that we don't throw
    // out points which lie on the edge of the clip region
    double sizex = b.width() * 1.0e-12;
    double sizey = b.height() * 1.0e-12;

    double aline[4];
    double bline[4];

    RS_Bounds clipRect( b.minx- sizex,
                        b.miny- sizey,
                        b.maxx+ sizex,
                        b.maxy+ sizey);

    LineBuffer* ret = dest;

    bool move = false;

    for (int i=0; i<m_cur_types; i++)
    {
        if (m_types[i] == (unsigned char)stMoveTo)
            move = true;
        else if (m_types[i] == (unsigned char)stLineTo)
        {
            get_point(i-1, aline[0], aline[1]);
            get_point(i  , aline[2], aline[3]);
            int res = ClipLine(clipRect, aline, bline);

            if (res == 0)
                continue;

            if (res == 1) //second point was not clipped
            {
                if (move)
                    ret->MoveTo(bline[0], bline[1]);

                ret->LineTo(bline[2], bline[3]);

                move = false;
            }

            if (res == 2) //second point was clipped
            {
                if (move)
                    ret->MoveTo(bline[0], bline[1]);

                ret->LineTo(bline[2], bline[3]);

                move = true;
            }
        }
    }

    return ret;
}


int LineBuffer::ClipCode(RS_Bounds& b, double x, double y)
{
    int clipCode;

    if (x < b.minx)
        clipCode = LEFT;
    else if (x > b.maxx)
        clipCode = RIGHT;
    else
        clipCode = INSIDE;

    if (y < b.miny)
        return clipCode |= BOTTOM;
    else if (y > b.maxy)
        return clipCode |= TOP;
    else
        return clipCode;
}


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
//                    are returned in the referenced structure.
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
int LineBuffer::ClipLine(RS_Bounds& clipRect, double* line, double* RESTRICT ret)
{
    ret[0] = line[0];
    ret[1] = line[1];
    ret[2] = line[2];
    ret[3] = line[3];

    // compute the initial clip codes for the endpoints
    int clipCode1 = ClipCode(clipRect, ret[0], ret[1]);
    int clipCode2 = ClipCode(clipRect, ret[2], ret[3]);
    int initClipCode2 = clipCode2;

    // compute the difference between the x and y values of the endpoints
    double deltaX = ret[2] - ret[0];
    double deltaY = ret[3] - ret[1];

    // try to clip against all four boundaries of the clip rectangle
    for (int i = 0; i < 4; i++)
    {
        double x, y;

        // trivially reject or accept the line segment?
        if ((clipCode1 & clipCode2) != 0)
            return 0;
        else if (clipCode1 == clipCode2)
            return (initClipCode2 == INSIDE)? 1 : 2;

        // use the clip code for an endpoint outside the clip rectangle
        int clipCode = (clipCode1 != 0)? clipCode1 : clipCode2;

        // clip the line against the first boundary it crosses
        if ((clipCode & TOP) != 0)
        {
            x = ret[0] + ((clipRect.maxy - ret[1]) * deltaX / deltaY);
            y = clipRect.maxy;
        }
        else if ((clipCode & BOTTOM) != 0)
        {
            x = ret[0] + ((clipRect.miny - ret[1]) * deltaX / deltaY);
            y = clipRect.miny;
        }
        else if ((clipCode & RIGHT) != 0)
        {
            y = ret[1] + ((clipRect.maxx - ret[0]) * deltaY / deltaX);
            x = clipRect.maxx;
        }
        else
        {
            y = ret[1] + ((clipRect.minx - ret[0]) * deltaY / deltaX);
            x = clipRect.minx;
        }

        // set up for next pass through loop
        if (clipCode == clipCode1)
        {
            ret[0] = x;
            ret[1] = y;
            clipCode1 = ClipCode(clipRect, x, y);
        }
        else
        {
            ret[2] = x;
            ret[3] = y;
            clipCode2 = ClipCode(clipRect, x, y);
        }
    }

    // if we get here, then the second endpoint must have been clipped off
    return 2;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////                          Centroid
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void LineBuffer::Centroid(GeomOperationType type, double* x, double* y, double* slope)
{
    switch (type)
    {
    case ctArea:
        0 == m_cur_cntr? PolygonCentroid(0, x, y) :
                         MultiPolygonCentroid(x, y);
        break;
    case ctLine:
        0 == m_cur_cntr? PolylineCentroid(0, x, y, slope) :
                         MultiPolylineCentroid(x, y, slope);
        break;
    case ctPoint:
        MultiPointCentroid(x, y);
        break;
    default:
        break;
    }
}


void LineBuffer::PolygonCentroid(int cntr, double* cx, double* cy)
{
    // for polygons, we just consider the outer ring and ignore any holes

    // handle special cases
    int numpts = cntr_size(cntr);
    if (numpts == 0)
    {
        *cx = std::numeric_limits<double>::quiet_NaN();
        *cy = std::numeric_limits<double>::quiet_NaN();
        return;
    }
    if (numpts == 1 || numpts == 2)
    {
        get_point(cntr, *cx, *cy);
        return;
    }
    if (numpts == 3)
    {
        *cx = 0.5 * (x_coord(cntr) + x_coord(cntr+1));
        *cy = 0.5 * (y_coord(cntr) + y_coord(cntr+1));
        return;
    }

    // don't consider the closing point
    numpts -= 1;

    // try TAW centroid first
    PolygonCentroidTAW(cntr, cx, cy);
    if (PointInPolygon(cntr, *cx, *cy))
        return;

    PolygonCentroidBVM(cntr, cx, cy);
    if (PointInPolygon(cntr, *cx, *cy))
        return;

    PolygonCentroidWMC(cntr, cx, cy);
    if (PointInPolygon(cntr, *cx, *cy))
        return;

    //TODO: what do we do if none of the centroids are inside the polygon?
    *cx = std::numeric_limits<double>::quiet_NaN();
    *cy = std::numeric_limits<double>::quiet_NaN();
    return;
}


void LineBuffer::PolygonCentroidTAW(int cntr, double* cx, double* cy)
{
    double x1, x2, y1, y2, xt1(0.0), xt2(0.0), yt1(0.0), yt2(0.0);
    const int dpv = 3;  // triplets
    double* pts = m_pts[contour_start_point(cntr)];
    int len = cntr_size(cntr) - 1;  // don't consider closing point
    double* i;
    double* j = pts;
    double* k = pts + (dpv*(len-1));
    while (j < k)
    {
        i = j + dpv;
        x1 = (*i) + (*j);
        x2 = (*i) - (*j);
        y1 = (*(i+1)) + (*(j+1));
        y2 = (*(i+1)) - (*(j+1));
        xt1 += x1 * x2 * y1;
        xt2 += x2 * y1;
        yt1 += y1 * y2 * x1;
        yt2 += y2 * x1;
        j = i;
    }

    // closing segment
    x1 = (*pts) + (*j);
    x2 = (*pts) - (*j);
    y1 = (*(pts+1)) + (*(j+1));
    y2 = (*(pts+1)) - (*(j+1));
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


void LineBuffer::PolygonCentroidBVM(int cntr, double* cx, double* cy)
{
    const int dpv = 3;  // triplets
    double* pts = m_pts[contour_start_point(cntr)];
    int len = cntr_size(cntr) - 1;  // don't consider closing point
    double xSum = 0.0, ySum = 0.0;
    double* i = pts;
    double* k = pts + (dpv*len);
    while (i < k)
    {
        xSum += *i;
        ySum += *(i+1);
        i += dpv;
    }
    if (len > 0)
    {
        *cx = xSum / len;
        *cy = ySum / len;
    }
}


void LineBuffer::PolygonCentroidWMC(int cntr, double* cx, double* cy)
{
    const int dpv = 3;  // triplets
    double* pts = m_pts[contour_start_point(cntr)];
    int len = cntr_size(cntr) - 1;  // dont' consider closing point
    double xSum = 0.0, ySum = 0.0, segLength, dx, dy;
    double totalLength = 0.0;

    for (int i=0, k=1; k<len; i+=dpv, k++)
    {
        dx = pts[i  ] - pts[i+dpv];
        dy = pts[i+1] - pts[i+dpv+1];
        segLength = sqrt(dx*dx + dy*dy);
        totalLength += segLength;
        xSum += pts[i  ] * segLength;
        ySum += pts[i+1] * segLength;
    }

    // add the last segment to the total accumulated length
    int lastIx = (len-1) * dpv;
    dx = pts[0] - pts[lastIx];
    dy = pts[1] - pts[lastIx+1];
    totalLength += sqrt(dx*dx + dy*dy);

    if (totalLength > 0.0)
    {
        *cx = xSum / totalLength;
        *cy = ySum / totalLength;
    }
}


// computes the length of a polyline
double LineBuffer::PolylineLength(int cntr)
{
    // pointer arithmetic looks ugly, and can be difficult to follow
    // but it produces tighter asm and this routine is called a lot
    int numpts = cntr_size(cntr);
    if (numpts < 2)
        return 0.0;
    double* pts = m_pts[contour_start_point(cntr)];
    double dx, dy;
    double len = 0.0;
    const int dpv = 3; // triplets
    double* j = pts;
    double* k = pts + (dpv*(numpts-1));
    double* i;
    while (j < k)
    {
        i = j+dpv;
        dx = *i - *j;
        dy = *(i+1) - *(j+1);
        len += sqrt(dx*dx + dy*dy);
        j = i;
    }
    return len;
}


// computes the squared length of a polyline - fast for relative length testing
// same algorithm as PolylineLength without sqrt call
double LineBuffer::PolylineLengthSqr(int cntr)
{
    // pointer arithmetic looks ugly, and can be difficult to follow
    // but it produces tighter asm and this routine is called a lot
    int numpts = cntr_size(cntr);
    if (numpts < 2)
        return 0.0;
    double* pts = m_pts[contour_start_point(cntr)];
    double dx, dy;
    double len = 0.0;
    const int dpv = 3; // triplets
    double* j = pts;
    double* k = pts + (dpv*(numpts-1));
    double* i;
    while (j < k)
    {
        i = j+dpv;
        dx = *i - *j;
        dy = *(i+1) - *(j+1);
        len += dx*dx + dy*dy;
        j = i;
    }
    return len;
}


double LineBuffer::PolygonArea(int cntr)
{
    // pointer arithmetic looks ugly, and can be difficult to follow
    // but it produces tighter asm and this routine is called a lot
    int numpts = cntr_size(cntr);
    if (numpts < 3)
        return 0.0;
    double* pts = m_pts[contour_start_point(cntr)];
    double sum = 0.0;
    const int dpv = 3; // always three double per vertex
    double* i = pts;
    double* j;
    double* k = i + (dpv*(numpts-1));

    // sum cross products to get twice the area
    while (i < k)
    {
        j = i+dpv;  // next coordinate
        sum += ((*i) * (*(j+1))) - ((*(i+1)) * (*j));
        i = j;
    }

    // make sure we add the close segment
    sum += (*i)*pts[1] - (*(i+1))*pts[0];
    return 0.5*fabs(sum);
}


void LineBuffer::MultiPolygonCentroid(double* cx, double* cy)
{
    // Computes the centroid of a MultiPolygon.  This is defined
    // to be the centroid of the largest polygon (polygon whose outer
    // ring has the largest area).

    if (point_count() == 0)
        return;

    if (point_count() == 1 || point_count() == 2)
    {
        *cx = m_pts[0][0];
        *cy = m_pts[0][1];
        return;
    }

    int cntr = 0;
    double area;
    double maxarea = -DBL_MAX;

    // iterate over all contours in buffer, find largest area
    for (int i=0; i<=m_cur_cntr; i++)
    {
        area = PolygonArea(i);
        if (area > maxarea)
        {
            // found a longer contour, remember it
            cntr = i;
            maxarea = area;
        }
    }
    PolygonCentroid(cntr, cx, cy);
}


void LineBuffer::MultiPolylineCentroid(double* cx, double* cy, double* slope)
{
    // Computes the centroid of a MultiPolyline.  This is defined
    // to be the centroid of the longest polyline.

    if (point_count() == 0)
    {
        *cx = std::numeric_limits<double>::quiet_NaN();
        *cy = std::numeric_limits<double>::quiet_NaN();
        return;
    }
    else if (point_count() == 1)
    {
        *cx = m_pts[0][0];
        *cy = m_pts[0][1];
        return;
    }

    int cntr = 0;
    double len, maxlen(0);

    // iterate over all contours in buffer, find longest
    for (int i = 0; i <= m_cur_cntr; i++)
    {
        len = PolylineLengthSqr(i);
        if (len>maxlen)
        {
            // found a longer contour, remember it
            cntr = i;
            maxlen = len;
        }
    }
    PolylineCentroid(cntr, cx, cy, slope);
}


void LineBuffer::PolylineCentroid(int cntr, double* cx, double* cy, double* slope)
{
    if ((m_cur_cntr < cntr) || (0 == m_cntrs[cntr]))
    {
        _ASSERT(false);
        return;
    }

    double* pts = m_pts[contour_start_point(cntr)];
    int numpts = cntr_size(cntr);

    // determine halfway-length of the linestring
    double halfLength = 0.5*PolylineLength(cntr);

    double lineLength = 0.0;
    double walkLength = 0.0;
    const int dpv = 3; // triplets
    double dx = 0.0;
    double dy = 0.0;
    double* j = pts;
    double* k = pts + (dpv*(numpts-1));
    double* i;
    while (j < k)
    {
        i = j+dpv;
        dx = *i - *j;
        dy = *(i+1) - *(j+1);
        lineLength = sqrt(dx*dx + dy*dy);
        if (walkLength + lineLength >= halfLength)
            break;
        walkLength += lineLength;
        j = i;
    }

    // compute the halfway point
    double fact = (lineLength > 0.0)? (halfLength-walkLength) / lineLength : 0.0;

    *cx = (*j) + fact*dx;
    *cy = (*(j+1)) + fact*dy;

    // compute the slope (as a rotation)
    double lineSlope;
    if (dx == 0.0)
        lineSlope = (dy < 0.0)? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
    else
        lineSlope = dy / dx;
    *slope = atan(lineSlope);
}


void LineBuffer::MultiPointCentroid(double* cx, double* cy)
{
    int len = point_count();
    if (len == 0)
        return;

    //case of single point
    if (len == 1)
    {
        *cx = m_pts[0][0];
        *cy = m_pts[0][1];
        return;
    }

    double xSum = 0.0, ySum = 0.0;
    for (int i=0; i<point_count(); i++)
    {
        xSum += m_pts[i][0];
        ySum += m_pts[i][1];
    }

    if (len > 0)
    {
        *cx = xSum / (double)len;
        *cy = ySum / (double)len;
    }
}


// point in specific contour
bool LineBuffer::PointInPolygon(int contour, double& x, double& y)
{
    if (m_cur_cntr < contour)
    {
        _ASSERT(false);
        return false;
    }

    double (*pts)[3] = &m_pts[contour_start_point(contour)];
    int numPts = cntr_size(contour);
    bool yflag0, yflag1;
    double vtx0X, vtx0Y, vtx1X, vtx1Y;

    bool inside_flag = false;

    // get the last point in the polygon
    vtx0X = pts[numPts-1][0];
    vtx0Y = pts[numPts-1][1];

    // get test bit for above/below X axis
    yflag0 = (vtx0Y >= y);

    for (int j=0; j<numPts; j++)
    {
        vtx1X = pts[j][0];
        vtx1Y = pts[j][1];

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

    return inside_flag;
}


// point in any contour
bool LineBuffer::PointInPolygon(double& x, double& y)
{
    for (int i = 0; i <= m_cur_cntr; i++)
    {
        if (LineBuffer::PointInPolygon(i, x, y))
            return true;
    }
    return false;
}


bool LineBuffer::hasZ()
{
    return m_bProcessZ;
}


bool LineBuffer::ignoreZ()
{
    return m_bIgnoreZ;
}


FdoDimensionality LineBuffer::dimensionality()
{
    return m_dimensionality;
}


void LineBuffer::ComputeBounds(RS_Bounds& bounds)
{
    // update the bounds, if they're not already set
    if (!m_bounds.IsValid())
    {
        for (int i=0; i<point_count();i++)
        {
            double x = m_pts[i][0];
            double y = m_pts[i][1];
            m_bounds.minx = rs_min(m_bounds.minx, x);
            m_bounds.maxx = rs_max(m_bounds.maxx, x);
            m_bounds.miny = rs_min(m_bounds.miny, y);
            m_bounds.maxy = rs_max(m_bounds.maxy, y);
            if (m_bProcessZ)
            {
                 double z = m_pts[i][2];
                 m_bounds.minz = rs_min(m_bounds.minz, z);
                 m_bounds.maxz = rs_max(m_bounds.maxz, z);
            }
        }
    }

    bounds = m_bounds;
}


void LineBuffer::SetDrawingScale(double drawingScale)
{
    m_drawingScale = drawingScale;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////                          LineBuffer object pool
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------
// Pooling -- it's very basic.
//--------------------------------------------------------

LineBufferPool::LineBufferPool()
{
}


LineBufferPool::~LineBufferPool()
{
    while (!m_pool.empty())
        delete m_pool.pop();
}


LineBuffer* LineBufferPool::NewLineBuffer(LineBufferPool* pool, int requestSize, FdoDimensionality dimensionality, bool bIgnoreZ)
{
    if (pool && !pool->m_pool.empty())
    {
        LineBuffer* lb = pool->m_pool.pop();
        lb->Reset(dimensionality, bIgnoreZ);
        return lb;
    }

    return new LineBuffer(requestSize, dimensionality, bIgnoreZ);
}


void LineBufferPool::FreeLineBuffer(LineBufferPool* pool, LineBuffer* lb)
{
    if (pool)
        pool->m_pool.push(lb);
    else
        delete lb;
}
