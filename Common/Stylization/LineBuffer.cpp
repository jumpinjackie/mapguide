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

#include "stdafx.h"
#include "LineBuffer.h"
#include "CSysTransformer.h"
#include "RS_OutputStream.h"


// For point reduction loop -- points will be dropped if the distance
// between them squared is more than 1.96 (i.e. 1.4 pixels).
// Dave said 1.4 is a good number.
const double OPTIMIZE_DISTANCE_SQ = 1.96;

// polygons and polylines below this # of points will not be victims of point reduction
const int MIN_RING_SIZE_TO_OPTIMIZE = 6;

// Cohen - Sutherland outcodes
#define LEFT   0x01
#define RIGHT  0x02
#define TOP    0x04
#define BOTTOM 0x08
#define INSIDE 0x00

#define min4(x1,x2,x3,x4) rs_min(x1, rs_min(x2, rs_min(x3, x4)))
#define max4(x1,x2,x3,x4) rs_max(x1, rs_max(x2, rs_max(x3, x4)))

#define MAX_POINT_BLOCK 128

LineBuffer::LineBuffer(int size, int dimensionality, bool bIgnoreZ) :
    m_bounds(DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX),
    m_types(NULL),
    m_types_len(0),
    m_cur_types(0),
    m_pts(NULL),
    m_cntrs(NULL),
    m_csp(NULL),
    m_cntrs_len(0),
    m_cur_cntr(-1), // will increment with first MoveTo segment
    m_cur_geom(-1),
    m_arcs_sp_len(0),
    m_arcs_sp(NULL),
    m_cur_arcs_sp(-1),
    m_closeseg_len(0),
    m_closeseg(NULL),
    m_cur_closeseg(-1),
    m_geom_type(0),
    m_drawingScale(0.0)
{
    ResizePoints(rs_max(size, 2));
    ResizeContours(4);
    m_dimensionality = dimensionality;
    m_bIgnoreZ = bIgnoreZ;
    m_bProcessZ = (m_dimensionality & Dimensionality_Z) && !m_bIgnoreZ;
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
    m_arcs_sp_len(0),
    m_arcs_sp(NULL),
    m_cur_arcs_sp(-1),
    m_closeseg_len(0),
    m_closeseg(NULL),
    m_cur_closeseg(-1),
    m_drawingScale(0.0),
    m_dimensionality(Dimensionality_XY),
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
    delete[] m_arcs_sp;
    delete[] m_closeseg;
}


void LineBuffer::Reset(int dimensionality, bool bIgnoreZ)
{
    m_cur_types = 0;
    m_cur_cntr = -1; // will increment with first MoveTo segment

    m_bIgnoreZ = bIgnoreZ;
    m_dimensionality = dimensionality;
    m_bProcessZ = (m_dimensionality & Dimensionality_Z) && !m_bIgnoreZ;
    m_bTransform2DPoints = false;
    m_cur_geom = -1;
    m_num_geomcntrs[0] = 0;
    m_drawingScale = 0.0;

    m_cur_arcs_sp = -1;
    m_cur_closeseg = -1;

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

    // arc start point indices
    if (m_arcs_sp_len < src.m_arcs_sp_len)
    {
        delete[] m_arcs_sp;
        m_arcs_sp_len = src.m_arcs_sp_len;
        m_arcs_sp = new int[m_arcs_sp_len];
    }
    m_cur_arcs_sp = src.m_cur_arcs_sp;
    memcpy(m_arcs_sp, src.m_arcs_sp, sizeof(int)*(1+m_cur_arcs_sp));

    // close segment indices
    if (m_closeseg_len < src.m_closeseg_len)
    {
        delete[] m_closeseg;
        m_closeseg_len = src.m_closeseg_len;
        m_closeseg = new int[m_closeseg_len];
    }
    m_cur_closeseg = src.m_cur_closeseg;
    memcpy(m_closeseg, src.m_closeseg, sizeof(int)*(1+m_cur_closeseg));

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
    // LineBuffer.  It is called from LoadFromFgf() but may not be called from
    // code building a LineBuffer using the API.  Make sure that NewGeometry()
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

    // Check to see if incoming points are 2D (no Z) and need to be transformed
    // back into 3D space.  This is used by 3D circular arc tessellation.
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


void LineBuffer::AdjustArcEndPoint(double x, double y, double z)
{
    _ASSERT(m_types[m_cur_types-1] == stLineTo);

    // Check to see if incoming points are 2D (no Z) and need to be transformed
    // back into 3D space.  This is used by 3D circular arc tessellation.
    if (m_bTransform2DPoints)
    {
        Point3D pt(x, y, z);
        Point3D result = m_T.Transform(pt);

        x = result.x;
        y = result.y;
        z = result.z;
    }

    m_pts[m_cur_types-1][0] = x;
    m_pts[m_cur_types-1][1] = y;
    m_pts[m_cur_types-1][2] = z;

    AddToBounds(x, y, z);
}


void LineBuffer::Close()
{
    if (m_cur_types == 0)
        return;

    if (m_types[m_cur_types-1] == (unsigned char)stMoveTo)
        return;

    // find out if it's already closed
    if (contour_closed(m_cur_cntr))
        return;

    // store off close segment index
    EnsureCloseSegArray(1);
    m_closeseg[++m_cur_closeseg] = m_cur_types - 1;

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
    // double the capacity of the point arrays
    ResizePoints(2 * m_types_len);
}


void LineBuffer::ResizePoints(int n)
{
    if (n <= m_types_len) // unnecessary at the very least
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


void LineBuffer::ResizeArcsSpArray()
{
    // double the capacity of the arc start point array
    if (m_arcs_sp_len == 0)
        ResizeArcsSpArray(4);
    else
        ResizeArcsSpArray(2 * m_arcs_sp_len);
}


void LineBuffer::ResizeArcsSpArray(int n)
{
    if (n <= m_arcs_sp_len) // unnecessary at the very least
        return;

    // new arc start point array
    int* tempArcsSp = new int[n];

    // copy data
    if (m_arcs_sp)
        memcpy(tempArcsSp, m_arcs_sp, sizeof(int)*(1+m_cur_arcs_sp));

    // cleanup
    delete[] m_arcs_sp;
    m_arcs_sp = tempArcsSp;
    m_arcs_sp_len = n;
}


void LineBuffer::ResizeCloseSegArray()
{
    // double the capacity of the close segment array
    if (m_closeseg_len == 0)
        ResizeCloseSegArray(4);
    else
        ResizeCloseSegArray(2 * m_closeseg_len);
}


void LineBuffer::ResizeCloseSegArray(int n)
{
    if (n <= m_closeseg_len) // unnecessary at the very least
        return;

    // new close segment array
    int* tempCloseSeg = new int[n];

    // copy data
    if (m_closeseg)
        memcpy(tempCloseSeg, m_closeseg, sizeof(int)*(1+m_cur_closeseg));

    // cleanup
    delete[] m_closeseg;
    m_closeseg = tempCloseSeg;
    m_closeseg_len = n;
}


LineBuffer& LineBuffer::operator+=(LineBuffer& other)
{
    // only if there is something to copy
    if (other.point_count() == 0)
        return *this;

    // make sure there is room
    EnsurePoints(other.point_count());
    EnsureContours(other.cntr_count());
    EnsureArcsSpArray(other.arcs_sp_length());
    EnsureCloseSegArray(other.closeseg_length());

    // copy contours
    memcpy(m_cntrs+m_cur_cntr+1, other.m_cntrs, sizeof(int)*(1+other.m_cur_cntr));
    memcpy(m_csp+m_cur_cntr+1, other.m_csp, sizeof(int)*(1+other.m_cur_cntr));
    for (int i = m_cur_cntr + 1; i < m_cur_cntr + other.m_cur_cntr + 2; ++i)
        m_csp[i] += m_cur_types;
    m_cur_cntr += other.m_cur_cntr + 1; // example: add two single contour buffers new cur contour is index 1

    // copy point data
    memcpy(m_pts + m_cur_types, other.m_pts, sizeof(double)*(other.point_count() * 3));

    // copy arc start point indices
    memcpy(m_arcs_sp+m_cur_arcs_sp+1, other.m_arcs_sp, sizeof(int)*(1+other.m_cur_arcs_sp));
    for (int i = m_cur_arcs_sp + 1; i < m_cur_arcs_sp + other.m_cur_arcs_sp + 2; ++i)
        m_arcs_sp[i] += m_cur_types;
    m_cur_arcs_sp += other.m_cur_arcs_sp + 1;

    // copy close segment indices
    memcpy(m_closeseg+m_cur_closeseg+1, other.m_closeseg, sizeof(int)*(1+other.m_cur_closeseg));
    for (int i = m_cur_closeseg + 1; i < m_cur_closeseg + other.m_cur_closeseg + 2; ++i)
        m_closeseg[i] += m_cur_types;
    m_cur_closeseg += other.m_cur_closeseg + 1;

    // copy segment types
    memcpy(m_types + m_cur_types, other.m_types, other.m_cur_types);
    m_cur_types += other.m_cur_types;

    // copy geometry
    if (m_cur_geom + other.m_cur_geom + 2 > m_num_geomcntrs_len)
        ResizeNumGeomContours(m_cur_geom + other.m_cur_geom + 2);
    memcpy(m_num_geomcntrs + m_cur_geom + 1, other.m_num_geomcntrs, sizeof(int)*(1+other.m_cur_geom));
    m_cur_geom += other.m_cur_geom + 1; // follows same pattern as contour

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
////                          Curve tessellation
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void LineBuffer::CircularArcTo(double x1, double y1, double x2, double y2)
{
    _ASSERT(point_count() > 0);

    // NOTE: This is really an error condition.  The user has specified that
    // the LineBuffer should have Z, but is now calling the circular arc routine
    // with no Z value.  Assert and draw nothing.
    _ASSERT(!m_bProcessZ);

    if (!m_bProcessZ)
    {
        // get the start point
        double x0, y0, z0;
        last_point(x0, y0, z0);
        CircularArcTo2D(x0, y0, x1, y1, x2, y2);
    }
}


void LineBuffer::CircularArcTo(double x1, double y1, double z1, double x2, double y2, double z2)
{
    _ASSERT(point_count() > 0);

    // get the start point
    double x0, y0, z0;
    last_point(x0, y0, z0);
    if (m_bProcessZ)
        CircularArcTo3D(x0, y0, z0, x1, y1, z1, x2, y2, z2);
    else
        CircularArcTo2D(x0, y0, x1, y1, x2, y2);
}


// 3D circular arc tessellation.
// We transform the 3D circular arc into the XY Z=0 plane and then tessellate using
// the 2D circular arc tessellation algorithm.  Then we transform the points back
// into the original plane of the 3D circular arc.
void LineBuffer::CircularArcTo3D(double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2)
{
    Point3D start(x0, y0, z0);
    Point3D mid(x1, y1, z1);
    Point3D end(x2, y2, z2);

    // Avoid tessellation if two points are equal.  This is considered an error
    // condition because two points don't define a circular arc segment.  This
    // also means we can't draw full circles as there is no way to specify one.
    if (start == mid || mid == end || start == end)
    {
        // store off arc start point index
        EnsureArcsSpArray(2);
        m_arcs_sp[++m_cur_arcs_sp] = m_cur_types - 1;

        LineTo(x1, y1, z1);
        LineTo(x2, y2, z2);

        // store off arc end point index (want index of start point of last seg)
        m_arcs_sp[++m_cur_arcs_sp] = m_cur_types - 2;

        return;
    }

    // compute normal
    Vector3D p0p2(end.x-start.x, end.y-start.y, end.z-start.z);
    Vector3D p0p1(mid.x-start.x, mid.y-start.y, mid.z-start.z);
    Vector3D normal = p0p2.crossProduct(p0p1);

    // If normal has near zero length, then points are co-linear.  Avoid
    // tessellation and draw two line segments.
    if (normal.lengthSqrd() < 1.0e-12)
    {
        // store off arc start point index
        EnsureArcsSpArray(2);
        m_arcs_sp[++m_cur_arcs_sp] = m_cur_types - 1;

        LineTo(x1, y1, z1);
        LineTo(x2, y2, z2);

        // store off arc end point index (want index of start point of last seg)
        m_arcs_sp[++m_cur_arcs_sp] = m_cur_types - 2;

        return;
    }
    normal.normalize();

    // Create the coordinate system transform for the arc's coordinate system.
    // Do this by using p0p2 as the major axis, then compute minor axis
    // (third perpendicular) by taking cross product of normal and major axis.
    // Use p0 as the arbitrary origin of the coordinate system.
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

    // Tessellate arc in 2D and store in point list
    CircularArcTo2D(startPrime.x, startPrime.y, midPrime.x, midPrime.y, endPrime.x, endPrime.y);

    m_bTransform2DPoints = false;

    // ensure the final generated point exactly matches the input
    AdjustArcEndPoint(x2, y2, z2);
}


void LineBuffer::CircularArcTo2D(double x0, double y0, double x1, double y1, double x2, double y2)
{
    // now we can compute the circle that those 3 points describe
    double dx1 = x1 - x0;
    double dy1 = y1 - y0;
    double dx2 = x2 - x0;
    double dy2 = y2 - y0;

    double area = 0.5 * (dx1 * dy2 - dx2 * dy1);

    // if it's almost a straight line, avoid numeric instability
    // by just adding two lines instead of tessellating a curve
    double minx = rs_min(x0, rs_min(x1, x2));
    double maxx = rs_max(x0, rs_max(x1, x2));
    double miny = rs_min(y0, rs_min(y1, y2));
    double maxy = rs_max(y0, rs_max(y1, y2));

    double boxArea = (maxx - minx) * (maxy - miny);

    // TODO: in case the area of the bounding box is 0, we may need
    //       special handling for arcs that represent a full circle
    if (fabs(boxArea) > 1.0e-20)
    {
        double areaRatio = fabs(area) / boxArea;

        if (areaRatio < 1.0e-10)
        {
            // store off arc start point index
            EnsureArcsSpArray(2);
            m_arcs_sp[++m_cur_arcs_sp] = m_cur_types - 1;

            LineTo(x1, y1);
            LineTo(x2, y2);

            // store off arc end point index (want index of start point of last seg)
            m_arcs_sp[++m_cur_arcs_sp] = m_cur_types - 2;

            return;
        }
    }

    double sqLen10 = dx1 * dx1 + dy1 * dy1;
    double sqLen20 = dx2 * dx2 + dy2 * dy2;
    double invArea = 1.0 / area;

    // find center point and radius of circumscribing circle using
    // formulas from Geometric Tools (Schneider & Eberly)
    double cx = x0 + 0.25 * invArea * (dy2 * sqLen10 - dy1 * sqLen20);
    double cy = y0 + 0.25 * invArea * (dx1 * sqLen20 - dx2 * sqLen10);

    double rdx = cx - x0;
    double rdy = cy - y0;

    double r = sqrt(rdx*rdx + rdy*rdy);

    // start and end angles of circular arc, in radians
    double startAngle = atan2(y0 - cy, x0 - cx);
    double endAngle = atan2(y2 - cy, x2 - cx);

    // now fix the start and end angles so that they track the arc
    // in the correct direction
    if (area < 0.0) // CW
    {
        if (startAngle < endAngle)
            startAngle += 2.0 * M_PI;
    }
    else // CCW
    {
        if (startAngle > endAngle)
            endAngle += 2.0 * M_PI;

    }

    ArcTo(cx, cy, r, r, startAngle, endAngle);

    // ensure the final generated point exactly matches the input
    AdjustArcEndPoint(x2, y2);
}


void LineBuffer::ArcTo(double cx, double cy, double a, double b, double startRad, double endRad, double rotRad)
{
    // store off arc start point index
    EnsureArcsSpArray(2);
    m_arcs_sp[++m_cur_arcs_sp] = m_cur_types - 1;

    // arcs with no angular extent will result in one LineTo segment
    double extent = endRad - startRad;

    int nSegs = ARC_TESSELLATION_SEGMENTS;  // default
    if (m_drawingScale != 0.0)
    {
        // Compute the required angular separation between points which gives
        // the desired tolerance.  Just base it off the max radius and use the
        // formula for a circle with 0.25 pixel tolerance.  Note that we don't
        // need to take into account any transformation in the case of 3D arcs
        // (see CircularArcTo3D).  The case requiring the most segments is when
        // the plane of the 3D arc is normal to the Z axis.  Since CircularArcTo3D
        // does the tessellation in this plane, this already corresponds to the
        // worst case.
        double maxRadius = rs_max(a, b);
        double dRadReq = sqrt(2.0 * m_drawingScale / maxRadius);

        // using the angular separation we can compute the minimum number
        // of segments
        nSegs = 1 + (int)(fabs(extent) / dRadReq);

        if (nSegs < 0)
        {
            // can happen due to overflow
            nSegs = 10 * ARC_TESSELLATION_SEGMENTS;
        }
        else
        {
            // the number of segments can get big for large arcs displayed at
            // high zoom levels, so limit the number of segments
            nSegs = rs_min(nSegs, 10 * ARC_TESSELLATION_SEGMENTS);
        }
    }

    // get the angular separation corresponding to the number of segments
    double dRad = extent / nSegs;

    // account for the overall arc rotation
    double rcos = cos(rotRad);
    double rsin = sin(rotRad);

    // add the segments
    EnsurePoints(nSegs);
    for (int i=1; i<=nSegs; ++i)
    {
        double ang = startRad + i*dRad;
        double tx = a * cos(ang);
        double ty = b * sin(ang);

        double x = cx + tx*rcos - ty*rsin;
        double y = cy + ty*rcos + tx*rsin;

        LineTo(x, y);
    }

    // store off arc end point index (want index of start point of last seg)
    m_arcs_sp[++m_cur_arcs_sp] = m_cur_types - 2;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////                          AGF reader
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


// simple helper macro that reads x and y and (possibly) z and skips m and
// does coord sys transform
#define READ_POINT(x, y, z)         \
    x = *dreader++;                 \
    y = *dreader++;                 \
    if (m_bProcessZ)                \
    {                               \
        z = *dreader++;             \
        if (fabs(z) < 1.0e100)      \
        {                           \
            last_z = z;             \
            if (!have_bad_z)        \
                use_last_z = true;  \
            else if (!use_last_z)   \
                z = 0.0;            \
        }                           \
        else                        \
        {                           \
            have_bad_z = true;      \
            if (use_last_z)         \
                z = last_z;         \
            else                    \
                z = 0.0;            \
        }                           \
    }                               \
    else                            \
        z = 0.0;                    \
    dreader += skip;                \
    if (xformer)                    \
        xformer->TransformPoint(x, y);


void LineBuffer::LoadFromAgf(unsigned char* RESTRICT data, int /*sz*/, CSysTransformer* xformer)
{
    int* ireader = (int*)data;

    // the geometry type
    m_geom_type = (GeometryType)*ireader++;

    double last_z = 0.0;
    bool use_last_z = false;
    bool have_bad_z = false;

    switch (m_geom_type)
    {
        // all the linear types...
        case GeometryType_MultiLineString:
        case GeometryType_MultiPolygon:
        case GeometryType_MultiPoint:
        case GeometryType_LineString:
        case GeometryType_Polygon:
        case GeometryType_Point:
        {
            bool is_multi = (m_geom_type == GeometryType_MultiLineString)
                || (m_geom_type == GeometryType_MultiPolygon
                || (m_geom_type == GeometryType_MultiPoint));

            // the coordinate type
            int skip = 0; //0=XY, 1=XYZ or XYM, 2 = XYZM
            int num_geoms = 1;
            double x;
            double y;
            double z;

            // in case of multipolygon or multilinestring or multipoint,
            // read poly or linestring count
            if (is_multi)
                num_geoms = *ireader++;

            for (int q=0; q<num_geoms; ++q)
            {
                // skip past geometry type of subgeometry
                // we know it is LineString or Polygon or Point respectively
                if (is_multi)
                    *ireader++;

                // read cordinate typeB
                Dimensionality dim = (Dimensionality)*ireader++;

                // ensure that all dimensionalities of each geometry are the same
                _ASSERT(q==0 || m_dimensionality == (dim & ~Dimensionality_M));

                m_dimensionality = dim & ~Dimensionality_M; //LineBuffer doesn't support M
                m_bProcessZ = (m_dimensionality & Dimensionality_Z) && !m_bIgnoreZ;

                skip = 0;
                if ((dim & Dimensionality_Z) && m_bIgnoreZ) skip++;
                if (dim & Dimensionality_M) skip++;

                // the number of contours in current polygon/linestring
                int contour_count = 1; //for linestrings

                if (m_geom_type == GeometryType_Polygon ||
                    m_geom_type == GeometryType_MultiPolygon)
                    contour_count = *ireader++;

                for (int i=0; i<contour_count; ++i)
                {
                    int point_count = 1;

                    // point geoms do not have a point count, since
                    // each piece is just one point each
                    if (m_geom_type != GeometryType_MultiPoint &&
                        m_geom_type != GeometryType_Point)
                        point_count = *ireader++;

                    //*** ireader not valid from here down
                    double* dreader = (double*) ireader;

                    READ_POINT(x, y, z);

                    int num_pts_read = 1;

                    MoveTo(x, y, z);

                    if (point_count == 1)
                    {
                        // if current contour is just a point, add a second point
                        // for easier time in the line style algorithm
                        // but only do this for line and polygons, not points!!!!!
                        if (m_geom_type != GeometryType_MultiPoint &&
                            m_geom_type != GeometryType_Point)
                            LineTo(x, y, z);
                    }
                    else
                    {
                        while (num_pts_read  < point_count)
                        {
                            int block_size = point_count - num_pts_read;
                            if (block_size > MAX_POINT_BLOCK) block_size =  MAX_POINT_BLOCK;
                            double xarray[MAX_POINT_BLOCK];
                            double yarray[MAX_POINT_BLOCK];
                            double zarray[MAX_POINT_BLOCK];

                            for (int n_pt=0; n_pt<block_size; ++n_pt)
                            {
                                xarray[n_pt] = *dreader++;
                                yarray[n_pt] = *dreader++;
                                if (m_bProcessZ)
                                {
                                    zarray[n_pt] = *dreader++;

                                    if (fabs(zarray[n_pt]) < 1.0e100)
                                    {
                                        // z is good
                                        last_z = zarray[n_pt];
                                        if (!have_bad_z)
                                            use_last_z = true;
                                        else if (!use_last_z)
                                            zarray[n_pt] = 0.0;
                                    }
                                    else
                                    {
                                        // z is bad
                                        have_bad_z = true;

                                        if (use_last_z)
                                            zarray[n_pt] = last_z;
                                        else
                                            zarray[n_pt] = 0.0;
                                    }
                                }
                                else
                                    zarray[n_pt] = 0.0;
                                dreader += skip;
                            }

                            if (xformer)
                            {
                                xformer->TransformPoints(block_size, xarray, yarray);
                            }

                            for (int n_pt=0; n_pt<block_size; ++n_pt)
                            {
                                LineTo(xarray[n_pt], yarray[n_pt], zarray[n_pt]);
                            }

                            num_pts_read += block_size;
                        }
                    }

                    ireader = (int*)dreader;
                    //*** ireader valid again
                }
            }
            break;
        }

        // all the non-linear types...
        case GeometryType_CurveString:
        case GeometryType_CurvePolygon:
        case GeometryType_MultiCurveString:
        case GeometryType_MultiCurvePolygon:
        {
            bool is_multi = false;

            int real_geom_type = m_geom_type;

            // change geometry type over to flattened type
            switch (m_geom_type)
            {
                case GeometryType_CurveString:
                    m_geom_type = GeometryType_LineString;
                    break;
                case GeometryType_CurvePolygon:
                    m_geom_type = GeometryType_Polygon;
                    break;
                case GeometryType_MultiCurveString:
                    m_geom_type = GeometryType_MultiLineString;
                    is_multi = true;
                    break;
                case GeometryType_MultiCurvePolygon:
                    m_geom_type = GeometryType_MultiPolygon;
                    is_multi = true;
                    break;
            }

            // the coordinate type
            int skip = 0; //0=XY, 1=XYZ or XYM, 2 = XYZM
            int num_geoms = 1;
            double x;
            double y;
            double z;

            // in case of multicurvepolygon or multicurvestring
            // read poly or linestring count
            if (is_multi)
                num_geoms = *ireader++;

            for (int q=0; q<num_geoms; ++q)
            {
                // skip past geometry type of subgeometry
                // we know it is CurveString or CurvePolygon respectively
                if (is_multi)
                    *ireader++;

                // read cordinate typeB
                Dimensionality dim = (Dimensionality)*ireader++;

                // ensure that all dimensionalities of each geometry are the same
                _ASSERT(q==0 || m_dimensionality == (dim & ~Dimensionality_M));

                m_dimensionality = dim & ~Dimensionality_M; //LineBuffer doesn't support M
                m_bProcessZ = (m_dimensionality & Dimensionality_Z) && !m_bIgnoreZ;

                skip = 0;
                if ((dim & Dimensionality_Z) && m_bIgnoreZ) skip++;
                if (dim & Dimensionality_M) skip++;

                // the number of contours in current polygon/linestring
                int contour_count = 1; //for linestrings, no rings, just one

                if (real_geom_type == GeometryType_CurvePolygon ||
                    real_geom_type == GeometryType_MultiCurvePolygon)
                    contour_count = *ireader++; // # rings for polygons

                for (int i=0; i<contour_count; ++i)
                {
                    //*** ireader not valid from here down
                    double* dreader = (double*) ireader;

                    READ_POINT(x, y, z);

                    MoveTo(x, y, z); // first point is always a move

                    // ireader valid again
                    ireader = (int*)dreader;
                    int seg_count = *ireader++; // # curve segments

                    for (int j=0; j<seg_count; ++j)
                    {
                        int seg_type = *ireader++;

                        switch (seg_type)
                        {
                        case GeometryComponentType_CircularArcSegment:

                            // circular arc: read midpont and endpoint
                            // first point was either the start point or
                            // the last point of the previous segment
                            dreader = (double*)ireader;

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

                        case GeometryComponentType_LineStringSegment:

                            // line string segment - just read the points and do LineTos
                            int num_pts = *ireader++;
                            dreader = (double*)ireader;

                            for (int k=0; k<num_pts; ++k)
                            {
                                READ_POINT(x, y, z);
                                LineTo(x, y, z);
                            }

                            ireader = (int*)dreader;
                            break;
                        }
                    }

                    if (m_geom_type == GeometryType_Polygon ||
                        m_geom_type == GeometryType_MultiPolygon)
                    {
                        Close();
                    }
                }
            }
            break;
        }

        case GeometryType_MultiGeometry:
        {
            // can't do that yet
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


// AGF writer
void LineBuffer::ToAgf(RS_OutputStream* os)
{
    int ptindex = 0;
    int cntrindex = 0;

    switch (m_geom_type)
    {
        // all the linear types...
        case GeometryType_MultiLineString:
        case GeometryType_MultiPolygon:
        case GeometryType_MultiPoint:
        case GeometryType_LineString:
        case GeometryType_Polygon:
        case GeometryType_Point:
        {
            // write geometry type
            WRITE_INT(os, m_geom_type);

            bool is_multi = (m_geom_type == GeometryType_MultiLineString ||
                             m_geom_type == GeometryType_MultiPolygon ||
                             m_geom_type == GeometryType_MultiPoint);

            // the coordinate type
//          int skip = 0; //0=XY, 1=XYZ or XYM, 2 = XYZM
            double x;
            double y;
            double z;

            // in case of multipolygon or multilinestring or multipoint,
            // read poly or linestring count
            int num_geoms = m_cur_geom + 1;
            if (is_multi)
            {
                if (   m_geom_type == GeometryType_MultiPoint 
                    || m_geom_type == GeometryType_MultiLineString )
                {
                    //write number of geometries -- for multipoint/linestring the
                    //number of contours (i.e. MoveTos) is equal to the
                    //number of point geometries in the multipoint
                    WRITE_INT(os, m_num_geomcntrs[0]);
                    num_geoms = m_num_geomcntrs[0];
                }
                else
                {
                    //write number of geometries
                    WRITE_INT(os, num_geoms);
                }
            }

            for (int q=0; q<num_geoms; ++q)
            {
                // write geometry type of subgeometry
                // we know it is LineString or Polygon or Point respectively
                if (is_multi)
                {
                    switch (m_geom_type)
                    {
                    case GeometryType_MultiLineString:
                        WRITE_INT(os, GeometryType_LineString);
                        break;
                    case GeometryType_MultiPolygon:
                        WRITE_INT(os, GeometryType_Polygon);
                        break;
                    case GeometryType_MultiPoint:
                        WRITE_INT(os, GeometryType_Point);
                        break;
                    }
                }

                // write cordinate type
                // If user specifies ignored Z, then we don't have Z values
                // even though m_dimensionality indicates we do.
                // Write out Dimensionality_XY in this case.
                if (m_bProcessZ)
                {
                    WRITE_INT(os, m_dimensionality);
                }
                else
                {
                    WRITE_INT(os, Dimensionality_XY);
                }

                // the number of contours in current polygon/linestring
                int contour_count = 1; //for linestrings

                if (m_geom_type == GeometryType_MultiPolygon ||
                    m_geom_type == GeometryType_Polygon )
                {
                    contour_count = m_num_geomcntrs[q];
                    WRITE_INT(os, contour_count);
                }

                for (int i=cntrindex; i< cntrindex + contour_count; ++i)
                {
                    int point_count = 1;

                    // point geoms do not have a point count, since
                    // each piece is just one point each
                    if ((m_geom_type != GeometryType_MultiPoint)
                        && (m_geom_type != GeometryType_Point))
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

        case GeometryType_CurveString:
        case GeometryType_CurvePolygon:
        case GeometryType_MultiCurveString:
        case GeometryType_MultiCurvePolygon:
        {
            // TODO: the LineBuffer is already tessellated, so in case of these
            // we need to actually use a tessellated type, i.e. the code above
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


// WARNING: caller responsible for deleting resulting line buffer
LineBuffer* LineBuffer::Optimize(double drawingScale, LineBufferPool* lbp)
{
    // the minimum allowed separation
    double d2Min = OPTIMIZE_DISTANCE_SQ * drawingScale * drawingScale;

    // don't set any offset for new linebuffer, because it is already applied,
    // and MoveTo and LineTo would apply it again. Instead, set at end
    LineBuffer* ret = LineBufferPool::NewLineBuffer(lbp, m_cur_types, m_dimensionality, m_bIgnoreZ);
    std::auto_ptr<LineBuffer> spLB(ret);
    ret->SetGeometryType(geom_type());

    // optimization
    int index = 0;
    double x, y, z=0.0, lx, ly, lz=0.0;
    for (int i=0; i<=m_cur_cntr; ++i)
    {
        int numPoints = m_cntrs[i];

        // if not enough points, just add the entire contour
        if (numPoints < MIN_RING_SIZE_TO_OPTIMIZE)
        {
            x = m_pts[index][0];
            y = m_pts[index][1];
            if (m_bProcessZ)
                z = m_pts[index][2];
            ret->MoveTo(x, y, z);
            index++;

            for (int j=1; j<numPoints; ++j)
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
            // add first point
            lx = m_pts[index][0];
            ly = m_pts[index][1];
            if (m_bProcessZ)
                lz = m_pts[index][2];
            ret->MoveTo(lx, ly, lz);
            index++;
            int numAdded = 1;

            // middle points
            for (int j=1; j<numPoints-1; ++j)
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

            // add last point to ensure closure
            x = m_pts[index][0];
            y = m_pts[index][1];
            if (m_bProcessZ)
                z = m_pts[index][2];
            ret->LineTo(x, y, z);
            index++;
        }
    }

    return spLB.release();
}


// WARNING: caller is responsible for deleting resulting line buffer
// if return is equal to this pointer, no clipping was needed (geometry was
// fully inside given box), so no need to free it
// if return pointer is NULL, geometry was fully outside the clip box
LineBuffer* LineBuffer::Clip(RS_Bounds& b, GeomOperationType clipType, LineBufferPool* lbp)
{
    // We don't handle 3D clipping correctly yet
    // TODO: Implement 3D clipping if necessary
    if (hasZ())
        return this;

    // check if line buffer is fully contained in box
    if (   b.minx <= m_bounds.minx
        && b.maxx >= m_bounds.maxx
        && b.miny <= m_bounds.miny
        && b.maxy >= m_bounds.maxy)
        return this;

    // check if line buffer is completely outside box
    if (   m_bounds.minx > b.maxx
        || m_bounds.miny > b.maxy
        || m_bounds.maxx < b.minx
        || m_bounds.maxy < b.miny)
        return NULL;

    std::auto_ptr<LineBuffer> spLB(LineBufferPool::NewLineBuffer(lbp, m_cur_types, m_dimensionality, m_bIgnoreZ));

    if (clipType == ctArea)
    {
        ClipPolygon(b, spLB.get());
        return spLB.release();
    }
    else if (clipType == ctLine)
    {
        ClipPolyline(b, spLB.get());
        return spLB.release();
    }
    else if (clipType == ctPoint)
    {
        ClipPoints(b, spLB.get());
        return spLB.release();
    }
    else if (clipType == ctAGF)
    {
        switch (m_geom_type)
        {
            case GeometryType_MultiPolygon:
            case GeometryType_Polygon:
            {
                ClipPolygon(b, spLB.get());
                return spLB.release();
            }
            case GeometryType_MultiLineString:
            case GeometryType_LineString:
            {
                ClipPolyline(b, spLB.get());
                return spLB.release();
            }
            case GeometryType_Point:
            case GeometryType_MultiPoint:
            {
                ClipPoints(b, spLB.get());
                return spLB.release();
            }
            default:
            {
                LineBufferPool::FreeLineBuffer(lbp, spLB.release());
                return NULL;
            }
        }
    }
    else
    {
        LineBufferPool::FreeLineBuffer(lbp, spLB.release());
        return NULL;
    }
}


void LineBuffer::ClipPoints(RS_Bounds& b, LineBuffer* dst)
{
    dst->m_geom_type = m_geom_type;

    for (int i=0; i<point_count(); ++i)
    {
        double x = m_pts[i][0];
        double y = m_pts[i][1];

        if (x >= b.minx && y >= b.miny &&
            x <= b.maxx && y <= b.maxy)
        {
            dst->MoveTo(x, y);
        }
    }
}


void LineBuffer::ClipPolygon(RS_Bounds& b, LineBuffer* dst)
{
    _ASSERT(dst);

    // TODO: NOT Z AWARE
    // TODO: handle polygons that become multipolygons when clipped
    dst->m_geom_type = m_geom_type;

    // unlike polylines, here we don't need to expand the clip region
    // because we will still get a polygon point on the edge if we throw
    // away one which is just outside the edge (due to floating point precision)
    RS_Bounds clipRect(b.minx, b.miny, b.maxx, b.maxy);

    double xIn, xOut, yIn, yOut, tInX, tInY, tOutX, tOutY, tIn2, tOut1, tOut2;
    double aline[4];

    bool move = false;

    // loop over all segments, MoveTo's denote start of polygon
    // in a multipolygon
    for (int i=0; i<m_cur_types; ++i)
    {
        if (m_types[i] == (unsigned char)stMoveTo)
        {
            // if last segment added to ret was a move, roll it back
            // otherwise close the last contour
            if (i)
                dst->FinalizeContour();
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
            else if (clipRect.minx <= aline[0] && aline[0] <= clipRect.maxx)
                tOutX = std::numeric_limits<double>::infinity();
            else
                tOutX = -std::numeric_limits<double>::infinity();

            if (deltaY != 0.0)
                tOutY = (yOut - aline[1]) / deltaY;
            else if (clipRect.miny <= aline[1]&& aline[1] <= clipRect.maxy)
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
                    // if tOut1 > 0.0 and tOut1 <= 1.0 then line crosses over
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

                        AppendLBClipVertex(clipRect, xClip, yClip, dst, move);
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

                        AppendLBClipVertex(clipRect, xClip, yClip, dst, move);
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

                        AppendLBClipVertex(clipRect, xClip, yClip, dst, move);
                        move = false;
                    }

                    // else the line segment terminates inside the window
                    else
                    {
                        double xClip = aline[2];
                        double yClip = aline[3];
                        AppendLBClipVertex(clipRect, xClip, yClip, dst, move);
                        move = false;
                    }
                }

                // if 0.0 < tOut2 and tOut2 <= 1.0 then the line segment enters
                // into a final corner region - add the turning vertex
                if (tOut2 <= 1.0)
                {
                    double xClip = xOut;
                    double yClip = yOut;
                    AppendLBClipVertex(clipRect, xClip, yClip, dst, move);
                    move = false;
                }
            }
        }
    }

    // roll back or close the last contour
    dst->FinalizeContour();
}


// Called by the polygon clipper in order to weed out contours that are
// incomplete after clipping (i.e. they have a single point or two points).
void LineBuffer::FinalizeContour()
{
    // if last segment added to ret was a move, roll it back one point
    if (m_cur_types > 0 && m_types[m_cur_types-1] == (unsigned char)stMoveTo)
    {
        m_cur_types--; // roll back the MoveTo segment
        m_cur_cntr--;  // roll back the contour count by 1 also
        m_num_geomcntrs[m_cur_geom] -= 1;

        if (m_num_geomcntrs[m_cur_geom] == 0)
            m_cur_geom--;
    }
    // if last segment was only a line, take it out also
    else if (m_cur_types > 1 && m_types[m_cur_types-2] == (unsigned char)stMoveTo)
    {
        m_cur_types -= 2;   // roll back 2 segment types
        m_cur_cntr--;       // roll back the contour count by 1

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
//         clipRect  - passes a reference to an RS_Bounds structure; the
//                     structure pointed to contains the coordinates of the
//                     corners of the clip rectangle.
//         x         - passes the x coordinate of the clipped vertex.
//         y         - passes the y coordinate of the clipped vertex.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------
void LineBuffer::AppendLBClipVertex(RS_Bounds& clipRect, double x, double y, LineBuffer* lb, bool move)
{
    // If there is at least one edge in the output array, then determine if the
    // edge defined by the last two vertices in the array plus the new clip
    // vertex induce a degenerate edge.  The edge is considered degenerate if
    // it doubles back on the last edge in the output array, and lies along
    // one of the boundaries of the clip rectangle.

    // TODO: NOT Z AWARE
    bool degenerate = false;

    // only line segments can be degenerate - a move indicates the start of a
    // new polygon, so it is not degenerate
    int npts = move? 0 : lb->m_cntrs[lb->m_cur_cntr];

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

    // else if there is only one vertex, and the new vertex is identical,
    // then flag as degenerate
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


void LineBuffer::ClipPolyline(RS_Bounds& b, LineBuffer* dst)
{
    _ASSERT(dst);

    // TODO: NOT Z AWARE
    // TODO: handle polylines that become multipolylines when clipped
    dst->m_geom_type = m_geom_type;

    // expand clip region a little so that we don't throw
    // out points which lie on the edge of the clip region
    double sizex = b.width() * 1.0e-12;
    double sizey = b.height() * 1.0e-12;

    double aline[4];
    double bline[4];

    RS_Bounds clipRect(b.minx - sizex,
                       b.miny - sizey,
                       b.maxx + sizex,
                       b.maxy + sizey);

    bool move = false;

    for (int i=0; i<m_cur_types; ++i)
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

            if (res == 1) // second point was not clipped
            {
                if (move)
                    dst->MoveTo(bline[0], bline[1]);

                dst->LineTo(bline[2], bline[3]);

                move = false;
            }

            if (res == 2) // second point was clipped
            {
                if (move)
                    dst->MoveTo(bline[0], bline[1]);

                dst->LineTo(bline[2], bline[3]);

                move = true;
            }
        }
    }
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
        clipCode |= BOTTOM;
    else if (y > b.maxy)
        clipCode |= TOP;

    return clipCode;
}


//------------------------------------------------------------------------------
//
// FUNCTION: ClipLine().
//
// PURPOSE: Clips a line (given in application coordinates) against the
//          specified clip rectangle using the Cohen-Sutherland clipping
//          algorithm.  Note - the clipped segment is guaranteed to have
//          the same directon as the original.
//
// PARAMETERS:
//     clipRect - the rectangle to clip against
//     line     - an array of 4 doubles containing the x/y coordinates
//                of the line segment's start and end points
//     ret      - an array of 4 doubles which will contain the x/y
//                coordinates of the line's clipped start and end points
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
    for (int i=0; i<4; ++i)
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


void LineBuffer::Centroid(GeomOperationType type, double* x, double* y, double* slope) const
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


void LineBuffer::PolygonCentroid(int cntr, double* cx, double* cy) const
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

    // TODO: what do we do if none of the centroids are inside the polygon?
    *cx = std::numeric_limits<double>::quiet_NaN();
    *cy = std::numeric_limits<double>::quiet_NaN();
    return;
}


void LineBuffer::PolygonCentroidTAW(int cntr, double* cx, double* cy) const
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


void LineBuffer::PolygonCentroidBVM(int cntr, double* cx, double* cy) const
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


void LineBuffer::PolygonCentroidWMC(int cntr, double* cx, double* cy) const
{
    const int dpv = 3;  // triplets
    double* pts = m_pts[contour_start_point(cntr)];
    int len = cntr_size(cntr) - 1;  // dont' consider closing point
    double xSum = 0.0, ySum = 0.0, segLength, dx, dy;
    double totalLength = 0.0;

    for (int i=0, k=1; k<len; i+=dpv, ++k)
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
double LineBuffer::PolylineLength(int cntr) const
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
double LineBuffer::PolylineLengthSqr(int cntr) const
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

double LineBuffer::PolygonSignedArea(int cntr) const
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
    return 0.5*sum;
}


double LineBuffer::PolygonArea(int cntr) const
{
    return fabs(PolygonSignedArea(cntr));
}


void LineBuffer::MultiPolygonCentroid(double* cx, double* cy) const
{
    // Computes the centroid of a MultiPolygon.  This is defined to
    // be the centroid of the largest polygon (polygon whose outer
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
    for (int i=0; i<=m_cur_cntr; ++i)
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


void LineBuffer::MultiPolylineCentroid(double* cx, double* cy, double* slope) const
{
    // Computes the centroid of a MultiPolyline.  This is defined to
    // be the centroid of the longest polyline.

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
    for (int i=0; i<=m_cur_cntr; ++i)
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


void LineBuffer::PolylineCentroid(int cntr, double* cx, double* cy, double* slope) const
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


void LineBuffer::MultiPointCentroid(double* cx, double* cy) const
{
    int len = point_count();
    if (len == 0)
        return;

    // case of single point
    if (len == 1)
    {
        *cx = m_pts[0][0];
        *cy = m_pts[0][1];
        return;
    }

    double xSum = 0.0, ySum = 0.0;
    for (int i=0; i<point_count(); ++i)
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
bool LineBuffer::PointInPolygon(int contour, double& x, double& y) const
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

    for (int j=0; j<numPts; ++j)
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


// even-odd test for point containment
bool LineBuffer::PointInPolygon(double& x, double& y) const
{
    bool ret = false;

    for (int i=0; i<=m_cur_cntr; ++i)
    {
        if (LineBuffer::PointInPolygon(i, x, y))
            ret = !ret;
    }

    return ret;
}


bool LineBuffer::hasZ() const
{
    return m_bProcessZ;
}


bool LineBuffer::ignoreZ() const
{
    return m_bIgnoreZ;
}


int LineBuffer::dimensionality() const
{
    return m_dimensionality;
}


void LineBuffer::ComputeBounds(RS_Bounds& bounds)
{
    // update the bounds, if they're not already set
    if (!m_bounds.IsValid())
    {
        for (int i=0; i<point_count(); ++i)
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
// Pooling -- it's very basic
//--------------------------------------------------------

LineBufferPool::LineBufferPool()
{
}


LineBufferPool::~LineBufferPool()
{
    while (!m_pool.empty())
        delete m_pool.pop();
}


LineBuffer* LineBufferPool::NewLineBuffer(LineBufferPool* pool, int requestSize, int dimensionality, bool bIgnoreZ)
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
