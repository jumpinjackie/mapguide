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
#include "LineBuffer.h"

#include "CSysTransformer.h"
#include "Centroid.h"
#include "RS_OutputStream.h"

#include <limits>

//for point reduction loop -- point will be dropped if
//distance between them squared is more than 1.96 (i.e. 1.4 pixels)
//Dave said 1.4 is a good number.
#define OPTIMIZE_DISTANCE_SQ 1.96

//polygons and polylines below this # of points will not be victims of point reduction
#define MIN_RING_SIZE_TO_OPTIMIZE 6

//Cohen - Sutherland outcodes
#define LEFT   0x01
#define RIGHT  0x02
#define TOP    0x04
#define BOTTOM 0x08
#define INSIDE 0x00


LineBuffer::LineBuffer(int size)
: m_bounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX)
{
    m_types_len = size;
    m_types = new unsigned char[m_types_len];
    m_pts_len = m_types_len * 2;
    m_pts = new double[m_pts_len];
    m_cur_pts = 0;
    m_cur_types = 0;
    m_geom_type = 0;

    m_cntrs_len = 4;
    m_cntrs = new int[m_cntrs_len];
    m_cur_cntr = -1; //will increment with first MoveTo segment

    m_xformbuf = NULL;
    m_xformbuf_len = 0;
}

LineBuffer::~LineBuffer()
{
    delete[] m_types;
    delete[] m_pts;
    delete[] m_cntrs;

    if (m_xformbuf)
        delete [] m_xformbuf;
}

void LineBuffer::Reset()
{
    m_bounds = RS_Bounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);

    m_cur_pts = 0;
    m_cur_types = 0;
    m_cur_cntr = -1;//will increment with first MoveTo segment
}

void LineBuffer::SetGeometryType(int geomType)
{
    m_geom_type = geomType;
}

void LineBuffer::MoveTo(double x, double y)
{
    if (m_cur_types == m_types_len)
        Resize();

    m_types[m_cur_types++] = (unsigned char)stMoveTo;
    m_pts[m_cur_pts++] = x;
    m_pts[m_cur_pts++] = y;

    m_last_x = x;
    m_last_y = y;

    if (m_cur_cntr+1 == m_cntrs_len)
        ResizeContours();

    m_cntrs[++m_cur_cntr] = 1; //increment to next contour and count the point

    AddToBounds(x, y);
}

void LineBuffer::LineTo(double x, double y)
{
    if (m_cur_types == m_types_len)
        Resize();

    m_types[m_cur_types++] = (unsigned char)stLineTo;
    m_pts[m_cur_pts++] = x;
    m_pts[m_cur_pts++] = y;

    m_cntrs[m_cur_cntr] ++;

    AddToBounds(x, y);
}


void LineBuffer::Close()
{
    if (m_cur_pts == 0)
        return;

    if (m_types[m_cur_types-1] == (unsigned char)stMoveTo)
        return;

    if (   m_pts[m_cur_pts-2] == m_last_x
        && m_pts[m_cur_pts-1] == m_last_y)
        return;

    LineTo(m_last_x, m_last_y);
}

inline void LineBuffer::AddToBounds(double x, double y)
{
    if (x < m_bounds.minx)
        m_bounds.minx = x;

    if (x > m_bounds.maxx)
        m_bounds.maxx = x;

    if (y < m_bounds.miny)
        m_bounds.miny = y;

    if (y > m_bounds.maxy)
        m_bounds.maxy = y;
}


void LineBuffer::Resize()
{
    double* tempPts = new double[m_pts_len * 2];
    memcpy(tempPts, m_pts, sizeof(double)*m_pts_len);
    delete[] m_pts;
    m_pts = tempPts;
    m_pts_len *= 2;

    unsigned char* tempTypes = new unsigned char[m_types_len * 2];
    memcpy(tempTypes, m_types, sizeof(unsigned char) * m_types_len);
    delete[] m_types;
    m_types = tempTypes;
    m_types_len *= 2;
}

void LineBuffer::ResizeContours()
{
    int* tempCntrs = new int[m_cntrs_len * 2];
    memcpy(tempCntrs, m_cntrs, sizeof(int)*m_cntrs_len);
    delete[] m_cntrs;
    m_cntrs = tempCntrs;
    m_cntrs_len *= 2;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////
////                          Curve tesselation
////
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineBuffer::CircularArcTo(double x1, double y1, double x2, double y2)
{
    _ASSERT(point_count() > 0);

    //get the start point
    double x0 = points()[m_cur_pts-2];
    double y0 = points()[m_cur_pts-1];

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
    if (fabs(boxArea) > 1e-20)
    {
        double areaRatio = fabs(area) / boxArea;

        if (areaRatio < 1e-10)
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

    for (int i=0; i<num_segs; i++)
    {
        // move to start point
        if (i==0)
            LineTo(cx + a * ellx, cy + b * elly);

        double c1x = cx + a*(ellx - Ke * elly);
        double c1y = cy + b*(elly + Ke * ellx);

        angle += increment;
        ellx = cos(angle);
        elly = sin(angle);

        double c2x = cx + a * (ellx + Ke * elly);
        double c2y = cy + b * (elly - Ke * ellx);
        double c3x = cx + ellx * a;
        double c3y = cy + elly * b;

        TesselateCubicTo(c1x, c1y, c2x, c2y, c3x, c3y);
    }
}

void LineBuffer::TesselateCubicTo(double px2, double py2, double px3, double py3, double px4, double py4)
{
    //get the start point
    double px1 = points()[m_cur_pts-2];
    double py1 = points()[m_cur_pts-1];

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
    double dt = INV_TESSELATION_ITERATIONS; //= 1.0 / 100.0

    //double dt2 = dt*dt;
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

        //error += sqrt(dfx*dfx+dfy*dfy); //slow but accurate for flattening

        //faster but less accurste error estimate
        w = fabs(dfx);
        h = fabs(dfy);
        error += rs_max(w, h);
        if (error >= minSegLen)  //add segment only if we have reached treshold length
        {
            // line to current
            LineTo(fx, fy);
            error = 0.0;
        }
    }

    LineTo(px4, py4);
}

void LineBuffer::TesselateQuadTo(double px2, double py2, double px3, double py3)
{
    //get the start point
    double px1 = points()[m_cur_pts-2];
    double py1 = points()[m_cur_pts-1];

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
    double dt = max(1.0e-2, invscale / diff); // * m_invscale;
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
////
////                          AGF reader
////
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//simple helper macro that reads x and y and skip z and m and does coord sys transform
#define READ_POINT(x, y)            \
                x = *dreader++;     \
                y = *dreader++;     \
                switch(skip)        \
                {                   \
                case 2: *dreader++; \
                case 1: *dreader++; \
                }


void LineBuffer::LoadFromAgf(unsigned char* RESTRICT data, int /*sz*/, CSysTransformer* xformer)
{
    int* ireader = (int*)data;

    // the geometry type
    m_geom_type = (GisGeometryType)*ireader++;

    switch (m_geom_type)
    {
        //all the linear types...
    case GisGeometryType_MultiLineString :
    case GisGeometryType_MultiPolygon:
    case GisGeometryType_MultiPoint:
    case GisGeometryType_LineString:
    case GisGeometryType_Polygon:
    case GisGeometryType_Point:
        {
            bool is_multi = (m_geom_type == GisGeometryType_MultiLineString)
                || (m_geom_type == GisGeometryType_MultiPolygon
                || (m_geom_type == GisGeometryType_MultiPoint));

            // the coordinate type
            int skip = 0; //0=XY, 1=XYZ or XYM, 2 = XYZM
            int num_geoms = 1;
            double x;
            double y;

            //in case of multipolygon or multilinestring or multipoint,
            //read poly or linestring count
            if (is_multi) num_geoms = *ireader++;

            for (int q=0; q<num_geoms; q++)
            {
                //skip past geometry type of subgeometry
                //we know it is LineString or Polygon or Point respectively
                if (is_multi) *ireader++;

                //read cordinate typeB
                GisDimensionality dim = (GisDimensionality)*ireader++;

                skip = 0;
                if (dim & GisDimensionality_Z) skip++;
                if (dim & GisDimensionality_M) skip++;

                // the number of contours in current polygon/linestring
                int contour_count = 1; //for linestrings

                if ((m_geom_type == GisGeometryType_Polygon)
                    || (m_geom_type == GisGeometryType_MultiPolygon))
                    contour_count = *ireader++;

                for (int i=0; i<contour_count; i++)
                {
                    int point_count = 1;

                    //point geoms do not have a point count, since
                    //each piece is just one point each
                    if ((m_geom_type != GisGeometryType_MultiPoint)
                        && (m_geom_type != GisGeometryType_Point))
                        point_count = *ireader++;

                    //*** ireader not valid from here down
                    double* dreader = (double*) ireader;

                    READ_POINT(x, y);

                    int num_pts_read = 1;

                    MoveTo(x, y);

                    if (point_count == 1)
                    {
                        //if current contour is just a point, add a second point
                        //for easier time in the line style algorithm
                        //but only do this for line and polygons, not points!!!!!
                        if ((m_geom_type != GisGeometryType_MultiPoint)
                            && (m_geom_type != GisGeometryType_Point))
                            LineTo(x, y);
                    }
                    else
                    {
                        while (num_pts_read  < point_count)
                        {
                            READ_POINT(x, y);

                            LineTo(x, y);

                            num_pts_read++;
                        }
                    }

                    ireader = (int*)dreader;
                    //*** ireader valid again
                }
            }
        }
        break;

    case GisGeometryType_CurveString :
    case GisGeometryType_CurvePolygon :
    case GisGeometryType_MultiCurveString :
    case GisGeometryType_MultiCurvePolygon :
        {
            bool is_multi = false;

            int real_geom_type = m_geom_type;

            //change geometry type over to flattened type
            switch (m_geom_type)
            {
                case GisGeometryType_CurveString :
                    m_geom_type = GisGeometryType_LineString;
                    break;
                case GisGeometryType_CurvePolygon :
                    m_geom_type = GisGeometryType_Polygon;
                    break;
                case GisGeometryType_MultiCurveString :
                    m_geom_type = GisGeometryType_MultiLineString;
                    is_multi = true;
                    break;
                case GisGeometryType_MultiCurvePolygon :
                    m_geom_type = GisGeometryType_MultiPolygon;
                    is_multi = true;
                    break;
            }

            // the coordinate type
            int skip = 0; //0=XY, 1=XYZ or XYM, 2 = XYZM
            int num_geoms = 1;
            double x;
            double y;

            //in case of multicurvepolygon or multicurvestring
            //read poly or linestring count
            if (is_multi) num_geoms = *ireader++;

            for (int q=0; q<num_geoms; q++)
            {
                //skip past geometry type of subgeometry
                //we know it is CurveString or CurvePolygon respectively
                if (is_multi) *ireader++;

                //read cordinate typeB
                GisDimensionality dim = (GisDimensionality)*ireader++;

                skip = 0;
                if (dim & GisDimensionality_Z) skip++;
                if (dim & GisDimensionality_M) skip++;

                // the number of contours in current polygon/linestring
                int contour_count = 1; //for linestrings, no rings, just one

                if ((real_geom_type == GisGeometryType_CurvePolygon)
                    || (real_geom_type == GisGeometryType_MultiCurvePolygon))
                    contour_count = *ireader++; //#rings for polygons

                for (int i=0; i<contour_count; i++)
                {
                    //*** ireader not valid from here down
                    double* dreader = (double*) ireader;

                    READ_POINT(x, y);

                    MoveTo(x, y); //first point is always a move

                    ireader = (int*)dreader;
                    int seg_count = *ireader++; //# curve segments

                    for (int j = 0; j < seg_count; j++)
                    {
                        int seg_type = *ireader++;

                        switch (seg_type)
                        {
                        case GisGeometryComponentType_CircularArcSegment:
                            dreader = (double*)ireader;

                            //circular arc : read midpont and endpoint
                            //first point was either the start point or
                            //the last point if the previous segment

                            double midx;
                            double midy;

                            READ_POINT(midx, midy);

                            double endx;
                            double endy;

                            READ_POINT(endx, endy);

                            CircularArcTo(midx, midy, endx, endy);

                            ireader = (int*) dreader;

                            break;

                        case GisGeometryComponentType_LineStringSegment:

                            //line string segment -- just read the points
                            //and do LineTos

                            int num_pts = *ireader++;

                            dreader = (double*)ireader;

                            for (int k = 0; k < num_pts; k++)
                            {
                                READ_POINT(x, y);

                                LineTo(x, y);
                            }

                            ireader = (int*) dreader;
                            break;
                        }

                    } //seg count
                } //ring count of polygons
            }// geom count of multi
        }//case
        break;
    case GisGeometryType_MultiGeometry :
        //can't do that yet
        break;
    }

    //perform coordinate system transformation if needed
    if (xformer)
    {
        //we need to group the points in an x-array and y-array

        //check if our transform buffer is big enough and resize if needed
        if (m_xformbuf_len < m_pts_len)
        {
            if (m_xformbuf)
                delete [] m_xformbuf;

            m_xformbuf_len = m_pts_len;
            m_xformbuf = new double[m_xformbuf_len];
        }

        //group the coordinates into x and y arrays
        int ptcount = m_cur_pts/2;
        double * xv = m_xformbuf;
        double * yv = m_xformbuf + ptcount;
        double * pts = m_pts;

        for (int i=0; i<ptcount; i++)
        {
            *xv++ = *pts++;
            *yv++ = *pts++;
        }

        xv = m_xformbuf;
        yv = m_xformbuf + ptcount;
        pts = m_pts;

        //transform
        xformer->TransformPoints(ptcount, xv, yv);

        //copy back into the interleaved points array
        //and also recompute the feature extents
        m_bounds = RS_Bounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);

        for (int i=0; i<ptcount; i++)
        {
            AddToBounds(*xv, *yv);

            *pts++ = *xv++;
            *pts++ = *yv++;
        }

    }
}

#define WRITE_INT(os, val) { \
                             \
    int val2 = val;          \
    os->write(&val2, 4);   } \

#define WRITE_DOUBLE(os, val) { \
                                \
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
    case GisGeometryType_MultiLineString :
    case GisGeometryType_MultiPolygon:
    case GisGeometryType_MultiPoint:
    case GisGeometryType_LineString:
    case GisGeometryType_Polygon:
    case GisGeometryType_Point:
        {
            //write geometry type
            WRITE_INT(os, m_geom_type);

            bool is_multi = (m_geom_type == GisGeometryType_MultiLineString)
                || (m_geom_type == GisGeometryType_MultiPolygon
                || (m_geom_type == GisGeometryType_MultiPoint));

            // the coordinate type
            //int skip = 0; //0=XY, 1=XYZ or XYM, 2 = XYZM
            int num_geoms = 1;
            double x;
            double y;

            //in case of multipolygon or multilinestring or multipoint,
            //read poly or linestring count
            if (is_multi)
            {
                num_geoms = m_cur_cntr;
                //write number of geometries
                //TODO: this is not actually equal to the
                //number of contours but for now we will map
                //each contour to a geometry -- we need another
                //array which counts groups of contours per polygon
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
                    case GisGeometryType_MultiLineString:
                        WRITE_INT(os, GisGeometryType_LineString);
                        break;
                    case GisGeometryType_MultiPolygon:
                        WRITE_INT(os, GisGeometryType_Polygon);
                        break;
                    case GisGeometryType_MultiPoint:
                        WRITE_INT(os, GisGeometryType_Point);
                        break;
                    }
                }

                //write cordinate type
                //TODO: for now we only save XY
                WRITE_INT(os, GisDimensionality_XY);

                // the number of contours in current polygon/linestring
                int contour_count = 1; //for linestrings

                if ((m_geom_type == GisGeometryType_Polygon)
                    || (m_geom_type == GisGeometryType_MultiPolygon))
                {

                    contour_count = 1;//TODO: we need to remember contour count
                                      //per polygon in order to support this.
                    WRITE_INT(os, contour_count);
                }

                for (int i=cntrindex; i< cntrindex + contour_count; i++)
                {
                    int point_count = 1;

                    //point geoms do not have a point count, since
                    //each piece is just one point each
                    if ((m_geom_type != GisGeometryType_MultiPoint)
                        && (m_geom_type != GisGeometryType_Point))
                    {
                        point_count = m_cntrs[i];
                        WRITE_INT(os, point_count);
                    }

                    int num_pts_written = 0;

                    while (num_pts_written < point_count)
                    {
                        x = m_pts[2*ptindex];
                        y = m_pts[2*ptindex+1];
                        ptindex++;

                        WRITE_DOUBLE(os, x);
                        WRITE_DOUBLE(os, y);

                        num_pts_written++;
                    }
                }

                cntrindex += contour_count;
            }
        }
        break;

    case GisGeometryType_CurveString :
    case GisGeometryType_CurvePolygon :
    case GisGeometryType_MultiCurveString :
    case GisGeometryType_MultiCurvePolygon :
        //TODO: the LineBuffer is already tesselated, so in case of these
        //we need to actually use a tesselated type, i.e. the code above
        break;
    }
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////
////                          Geometric operations
////
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
    LineBuffer* ret = lbp->NewLineBuffer(m_cur_types);

    //optimization
    int index = 0;
    double x,y,lx,ly;
    for(int i=0; i< m_cur_cntr+1; i++)
    {
        int numPoints = m_cntrs[i];

        //if not enough points, just add the entire contour
        if(numPoints < MIN_RING_SIZE_TO_OPTIMIZE)
        {
            x = m_pts[index++];
            y = m_pts[index++];
            ret->MoveTo(x, y);

            for(int j=1; j< numPoints; j++)
            {
                x = m_pts[index++];
                y = m_pts[index++];
                ret->LineTo(x, y);
            }
        }
        else
        {
            //add first point
            lx = m_pts[index++];
            ly = m_pts[index++];
            ret->MoveTo(lx,ly);

            int numAdded = 1;

            //middle points
            for(int j=1; j< numPoints-1; j++)
            {
                x = m_pts[index++];
                y = m_pts[index++];

                // always ensure we add at least 2 middle points
                int numRemaining = numPoints - j - 1;
                int numRequired  = 3 - numAdded;
                if (numRequired >= numRemaining)
                {
                    ret->LineTo(x,y);
                    numAdded++;
                    lx = x;
                    ly = y;
                    continue;
                }

                double dx = x - lx;
                double dy = y - ly;
                double dist2 = dx*dx + dy*dy;

                if (dist2 >= d2Min)
                {
                    ret->LineTo(x,y);
                    numAdded++;
                    lx = x;
                    ly = y;
                }
            }

            //add last point for to ensure closure
            x = m_pts[index++];
            y = m_pts[index++];
            ret->LineTo(x, y);
        }
    }

    return ret;
}




//WARNING: caller responsible for deleting resulting line buffer
//if return is equal to this pointer, no clipping was needed (geometry was
//fully inside given box), so no need to free it
//if return pointer is NULL, geometry was fully outside the clip box
LineBuffer* LineBuffer::Clip(RS_Bounds& b, GeomOperationType clipType, LineBufferPool* lbp)
{
    //check if line buffer is fully contained in box
    if (   b.minx < m_bounds.minx
        && b.maxx > m_bounds.maxx
        && b.miny < m_bounds.miny
        && b.maxy > m_bounds.maxy)
        return this;

    //check if line buffer is completely outside box
    if (m_bounds.minx > b.maxx
        || m_bounds.miny > b.maxy
        || m_bounds.maxx < b.minx
        || m_bounds.maxy < b.miny)
        return NULL;

    LineBuffer* dest = lbp->NewLineBuffer(m_cur_types);

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
        case GisGeometryType_MultiPolygon:
        case GisGeometryType_Polygon:
            return ClipPolygon(b, dest);
        case GisGeometryType_MultiLineString :
        case GisGeometryType_LineString:
            return ClipPolyline(b, dest);
        case GisGeometryType_Point:
        case GisGeometryType_MultiPoint:
            return ClipPoints(b, dest);
        default:
            {
                lbp->FreeLineBuffer(dest);
                return NULL;
            }
        }
    }
    else
    {
        lbp->FreeLineBuffer(dest);
        return NULL;
    }
}

LineBuffer* LineBuffer::ClipPoints(RS_Bounds& b, LineBuffer* dst)
{
    dst->m_geom_type = m_geom_type;

    for (int i=0; i<point_count(); i++)
    {
        double x = points()[2*i];
        double y = points()[2*i+1];

        if (   x >= b.minx && y >= b.miny
            && x <= b.maxx && y <= b.maxy)
        {
            dst->MoveTo(x, y);
        }
    }

    return dst;
}


LineBuffer* LineBuffer::ClipPolygon(RS_Bounds& b, LineBuffer* dest)
{
    _ASSERT(dest);

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
            //one point
            if (ret->m_cur_types > 0 && ret->m_types[ret->m_cur_types-1] == (unsigned char)stMoveTo)
            {
                ret->m_cur_types --; //roll back the MoveTo segment
                ret->m_cur_pts -= 2; //roll back one point
                ret->m_cur_cntr --; //roll back the contour count by 1 also
                if (ret->m_cur_pts > 0)
                {
                    ret->m_last_x = ret->m_pts[ret->m_cur_pts-2];
                    ret->m_last_y = ret->m_pts[ret->m_cur_pts-1];
                }
            }
            //if last contour was only a line, take it out also
            else if (ret->m_cur_types > 1 && ret->m_types[ret->m_cur_types-2] == (unsigned char)stMoveTo)
            {
                ret->m_cur_types -= 2; //roll back 2 segment types
                ret->m_cur_pts -= 4; //roll back 4 coordinates (2 points)
                ret->m_cur_cntr --; //roll back the contour caount by 1
                if (ret->m_cur_pts > 0)
                {
                    ret->m_last_x = ret->m_pts[ret->m_cur_pts-2];
                    ret->m_last_y = ret->m_pts[ret->m_cur_pts-1];
                }
            }
            else if (i != 0)// && (ret.cur_types != 0) && (ret.types[ret.cur_types-1] == (byte)SegType.LineTo))
                ret->Close();

            move = true;
        }
        else if (m_types[i] == (unsigned char)stLineTo)
        {
            aline[0] = m_pts[2*(i-1)];
            aline[1] = m_pts[2*(i-1)+1];
            aline[2] = m_pts[2*i];
            aline[3] = m_pts[2*i+1];

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

    //explicitly close last contour
    ret->Close();

    return ret;
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

    bool degenerate = false;

    //only line segments can be degenerate -- a move indicates the start of a new
    //polygon, so it is not degenerate.
    if (!move)
    {
        if (lb->m_cur_types > 1)
        {
            double x1 = lb->m_pts[lb->m_cur_pts-2];
            double y1 = lb->m_pts[lb->m_cur_pts-1];
            double x2 = lb->m_pts[lb->m_cur_pts-4];
            double y2 = lb->m_pts[lb->m_cur_pts-3];

            degenerate = (x == x1 && x == x2 &&
                (x == clipRect.minx || x == clipRect.maxx) &&
                ((y <= y1 && y2 <= y1) || (y >= y1 && y2 >= y1))) ||
                (y == y1 && y == y2 &&
                (y == clipRect.miny || y == clipRect.maxy) &&
                ((x <= x1 && x2 <= x1) || (x >= x1 && x2 >= x1)));
        }

            // else if there is only one vertex, and the new vertex is identical, then
            // flag as degenerate

        else if (lb->m_cur_types == 1)
            degenerate = x == lb->m_pts[0] && y == lb->m_pts[1];

            // else not degenerate

        else
            degenerate = false;
    }

    // if the new vertex does not induce a degeneracy, then just append

    if (!degenerate)
    {
        if (lb->m_cur_types == 0 || move)
            lb->MoveTo(x, y);
        else
            lb->LineTo(x, y);
    }

    // else overwrite the last vertex with the new vertex - this will eliminate
    // the degeneracy

    else
    {
        lb->m_pts[lb->m_cur_pts-2] = x;
        lb->m_pts[lb->m_cur_pts-1] = y;
    }

} // end: AppendLBClipVertex()


LineBuffer* LineBuffer::ClipPolyline(RS_Bounds& b, LineBuffer* dest)
{
    _ASSERT(dest);

    //TODO: handle polylines that become multipolylines when clipped
    dest->m_geom_type = m_geom_type;

    // expand clip region a little so that we don't throw
    // out points which lie on the edge of the clip region
    double sizex = b.width() * 1e-12;
    double sizey = b.height() * 1e-12;

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
            //ret.MoveTo(this.pts[3*i], this.pts[3*i+1], this.pts[3*i+2]);
        else if (m_types[i] == (unsigned char)stLineTo)
        {
            aline[0] = m_pts[2*(i-1)];
            aline[1] = m_pts[2*(i-1)+1];
            aline[2] = m_pts[2*i];
            aline[3] = m_pts[2*i+1];

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

    // Compute the initial clip codes for the endpoints
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

        // trivially reject or accept the line segment ?

        if ((clipCode1 & clipCode2) != 0)
            return 0;
        else if (clipCode1 == clipCode2)
            return initClipCode2 == INSIDE ? 1 : 2;

        // use the clip code for an endpoint outside the clip rectangle

        int clipCode = ((clipCode1!=0) ? clipCode1 : clipCode2);

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

        // Set up for next pass through loop

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

} // end OpsClipLine()


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////
////                          Centroid
////
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void LineBuffer::Centroid(GeomOperationType type, double* x, double * y, double * slope)
{
    switch (type)
    {
    case ctArea:
        return MultiPolygonCentroid(x, y);
        break;
    case ctLine:
        return MultiPolylineCentroid(x, y, slope);
        break;
    case ctPoint:
        return MultiPointCentroid(x, y);
        break;
    default:
        break;

    }
}


void LineBuffer::PolygonCentroid(double* cx, double* cy)
{
    //for polygons, we just consider the outer ring and ignore
    //any holes

    int numpts = 0;

    //find number of points in outer ring
    if (types()[0] == LineBuffer::stMoveTo)
    {
          numpts = 1;
          while ((numpts < point_count()) && (types()[numpts] == LineBuffer::stLineTo))
              numpts++;
    }

    return Centroid::PolygonCentroid(points(), numpts, cx, cy);
}


void LineBuffer::MultiPolygonCentroid(double* cx, double* cy)
{
    // Computes the centroid of a MultiPolygon.  This is defined
    // to be the centroid of the largest polygon (polygon whose outer
    // ring has the largest area).

    if (point_count() == 0)
    {
        return;
    }
    else if ((point_count() == 1) || (point_count() == 2))
    {
        *cx = points()[0];
        *cy = points()[1];
    }

    double* cntr = NULL;
    int numpts = 0;
    int typeoffset1 = 0;
    int typeoffset2 = 0;

    double maxarea = -DBL_MAX;
    double* maxcntr = NULL;
    int maxnumpts = 0;

    //go over all contours and find the one with largest area
    while (typeoffset1 < point_count())
    {
        //remember beginning index (MoveTo) of contour and advance to LineTo segment
        typeoffset2 = typeoffset1++;

        //find end index of contour
        while ((typeoffset1 < point_count()) && (types()[typeoffset1] == LineBuffer::stLineTo))
            typeoffset1++;

        numpts = typeoffset1 - typeoffset2;
        cntr = points() + (typeoffset2 * 2); //pointer offset into controur data

        double area = Centroid::PolygonArea(cntr, numpts);

        if (area > maxarea)
        {
            maxarea = area;
            maxcntr = cntr;
            maxnumpts = numpts;
        }
    }

    _ASSERT(cntr);

    return Centroid::PolygonCentroid(maxcntr, maxnumpts, cx, cy);
}


void LineBuffer::MultiPolylineCentroid(double* cx, double* cy, double* slope)
{
    // Computes the centroid of a MultiPolygon.  This is defined
    // to be the centroid of the largest polygon (polygon whose outer
    // ring has the largest area).

    if (point_count() == 0)
    {
        *cx = std::numeric_limits<double>::quiet_NaN();
        *cy = std::numeric_limits<double>::quiet_NaN();
        return;
    }
    else if ((point_count() == 1))
    {
        *cx = points()[0];
        *cy = points()[1];
        return;
    }

    double* cntr = NULL;
    int numpts = 0;
    int typeoffset1 = 0;
    int typeoffset2 = 0;

    double maxlen = -DBL_MAX;
    double* maxcntr = NULL;
    int maxnumpts = 0;

    //go over all contours and find the one with largest area
    while (typeoffset1 < point_count())
    {
        //remember beginning index (MoveTo) of contour and advance to LineTo segment
        typeoffset2 = typeoffset1++;

        //find end index of contour
        while ((typeoffset1 < point_count()) && (types()[typeoffset1] == LineBuffer::stLineTo))
            typeoffset1++;

        numpts = typeoffset1 - typeoffset2;
        cntr = points() + (typeoffset2 * 2); //pointer offset into controur data

        double len = Centroid::PolylineLength(cntr, numpts);

        if (len > maxlen)
        {
            maxlen = len;
            maxcntr = cntr;
            maxnumpts = numpts;
        }
    }

    _ASSERT(cntr);

    return Centroid::PolylineCentroid(maxcntr, maxnumpts, cx, cy, slope);
}

void LineBuffer::PolylineCentroid(double* cx, double* cy, double* slope)
{
    int numpts = 0;

    //find number of points in line string
    if (types()[0] == LineBuffer::stMoveTo)
    {
          numpts = 1;
          while ((numpts < point_count()) && (types()[numpts] == LineBuffer::stLineTo))
              numpts++;
    }
    else
    {
        _ASSERT(false);
        return;
    }

    return Centroid::PolylineCentroid(points(), numpts, cx, cy, slope);
}


void LineBuffer::MultiPointCentroid(double* cx, double* cy)
{
    int len = point_count();

    if (len == 0)
        return;

    //case of single point
    if (len == 1)
    {
        *cx = points()[0];
        *cy = points()[1];
        return;
    }

    double xSum = 0.0, ySum = 0.0;
    for (int i=0; i<len*2; i+=2)
    {
        xSum += points()[i  ];
        ySum += points()[i+1];
    }

    if (len > 0.0)
    {
        *cx = xSum / len;
        *cy = ySum / len;
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////
////                          LineBuffer object pool
////
////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------
// Pooling -- it's very basic.
//--------------------------------------------------------
LineBufferPool::~LineBufferPool()
{
    while (!m_pool.empty())
        delete m_pool.pop();
}

LineBuffer* LineBufferPool::NewLineBuffer(int requestSize)
{
    if (!m_pool.empty())
    {
        LineBuffer* lb = m_pool.pop();
        lb->Reset();
        return lb;
    }
    else
        return new LineBuffer(requestSize);
}

void LineBufferPool::FreeLineBuffer(LineBuffer* lb)
{
    m_pool.push(lb);
}
