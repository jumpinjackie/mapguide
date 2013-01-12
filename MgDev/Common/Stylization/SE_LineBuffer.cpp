//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
#include "SE_LineBuffer.h"
#include "SE_Bounds.h"
#include "SE_BufferPool.h"
#include "SE_RenderProxies.h"

#include <algorithm>

const double GROWTH_FACTOR = 1.5;

#define ENSURE_POINT_BUFFER(points) \
    if ((m_npts + (points)) > m_max_pts) \
        ResizeBuffer<double>(&m_pts, (points), m_npts, m_max_pts);

#define ENSURE_SEG_BUFFER(points) \
    if ((m_nsegs + (points)) > m_max_segs) \
        ResizeBuffer<SE_LB_SegType>(&m_segs, (points), m_nsegs, m_max_segs);


template<class BUFTYPE> void ResizeBuffer(BUFTYPE** buffer, int mininc, int cur_pts, int& max_pts)
{
    int max_newpts = (int)(GROWTH_FACTOR*max_pts) + 1;
    if (max_newpts - max_pts < mininc)
        max_newpts += mininc;

    BUFTYPE* newbuf = new BUFTYPE[max_newpts];
    memcpy(newbuf, *buffer, cur_pts*sizeof(BUFTYPE));
    delete[] *buffer;
    *buffer = newbuf;
    max_pts = max_newpts;
}


SE_LineBuffer::SE_LineBuffer(int size, SE_BufferPool* pool) :
    m_npts(0),
    m_nsegs(0),
    m_max_pts(2*size),
    m_max_segs(size),
    m_xf_tol(-1.0),
    m_xf_bounds(NULL),
    m_compute_bounds(true),
    m_pool(pool)
{
    m_pts = new double[size*2];
    m_segs = new SE_LB_SegType[size];
    m_xf_buf = LineBufferPool::NewLineBuffer(pool, size);
}


SE_LineBuffer::~SE_LineBuffer()
{
    delete[] m_pts;
    delete[] m_segs;
    LineBufferPool::FreeLineBuffer(m_pool, m_xf_buf);
    if (m_xf_bounds)
        m_xf_bounds->Free();
}


void SE_LineBuffer::MoveTo(double x, double y)
{
    ENSURE_POINT_BUFFER(2);
    ENSURE_SEG_BUFFER(1);

    double* cur_pt = m_pts + m_npts;
    *cur_pt++ = x;
    *cur_pt = y;

    m_start[0] = m_last[0] = x;
    m_start[1] = m_last[1] = y;

    m_npts += 2;
    m_segs[m_nsegs++] = SegType_MoveTo;
}


void SE_LineBuffer::LineTo(double x, double y)
{
    ENSURE_POINT_BUFFER(2);
    ENSURE_SEG_BUFFER(1);

    double* cur_pt = m_pts + m_npts;
    *cur_pt++ = x;
    *cur_pt = y;

    m_last[0] = x;
    m_last[1] = y;

    m_npts += 2;
    m_segs[m_nsegs++] = SegType_LineTo;
}


void SE_LineBuffer::EllipticalArcTo(double cx, double cy, double rx, double ry, double sAngRad, double eAngRad, double rotRad)
{
    ENSURE_POINT_BUFFER(7);
    ENSURE_SEG_BUFFER(1);

    double* cur_pt = m_pts + m_npts;

    *cur_pt++ = cx;
    *cur_pt++ = cy;
    *cur_pt++ = rx;
    *cur_pt++ = ry;
    *cur_pt++ = sAngRad;
    *cur_pt++ = eAngRad;
    *cur_pt = rotRad;

    // compute end position
    double ex = rx*cos(eAngRad);
    double ey = ry*sin(eAngRad);

    if (rotRad != 0.0)
    {
        double rcos = cos(rotRad);
        double rsin = sin(rotRad);

        double tx = ex;
        double ty = ey;
        ex = tx*rcos - ty*rsin;
        ey = ty*rcos + tx*rsin;
    }

    ex += cx;
    ey += cy;

    m_last[0] = ex;
    m_last[1] = ey;

    m_npts += 7;
    m_segs[m_nsegs++] = SegType_EllipticalArc;
}


void SE_LineBuffer::SetGeometry(LineBuffer* srclb)
{
    if (m_npts != 0)
        Reset();

    int n_cntrs = srclb->cntr_count();
    for (int i=0; i<n_cntrs; ++i)
    {
        int j = srclb->contour_start_point(i);
        int k = srclb->contour_end_point(i);
        SE_LineBuffer::MoveTo(srclb->x_coord(j), srclb->y_coord(j));

        while (j < k)
        {
            j++;
            SE_LineBuffer::LineTo(srclb->x_coord(j), srclb->y_coord(j));
        }
    }
}


void SE_LineBuffer::Close()
{
    LineTo(m_start[0], m_start[1]);
}


void SE_LineBuffer::Reset()
{
    m_compute_bounds = true;
    m_npts = m_nsegs = 0;
    if (m_xf_bounds)
    {
        m_xf_bounds->Free();
        m_xf_bounds = NULL;
    }
    m_xf_tol = -1.0;
    m_xf.setIdentity();
    m_xf_buf->Reset();
}


SE_Bounds* SE_LineBuffer::GetSEBounds(RS_Bounds& bounds)
{
    if (!bounds.IsValid())
        return NULL;

    SE_Bounds* seBounds = SE_BufferPool::NewBounds(m_pool, 0);
    seBounds->min[0] = bounds.minx;
    seBounds->min[1] = bounds.miny;
    seBounds->max[0] = bounds.maxx;
    seBounds->max[1] = bounds.maxy;

    return seBounds;
}


void SE_LineBuffer::PopulateXFBuffer()
{
    SE_LB_SegType* endseg = m_segs + m_nsegs;
    SE_LB_SegType* curseg = m_segs;
    int src_idx = 0;
    double x, y;

    m_xf_buf->Reset();

    while (curseg != endseg)
    {
        switch (*curseg++)
        {
        case SegType_MoveTo:
            m_xf_buf->EnsureContours(1);
            m_xf_buf->EnsurePoints(1);
            m_xf.transform(m_pts[src_idx], m_pts[src_idx+1], x, y);
            m_xf_buf->UnsafeMoveTo(x, y);
            src_idx += 2;
            break;

        case SegType_LineTo:
            m_xf_buf->EnsurePoints(1);
            m_xf.transform(m_pts[src_idx], m_pts[src_idx+1], x, y);
            m_xf_buf->UnsafeLineTo(x, y);
            src_idx += 2;
            break;

        case SegType_EllipticalArc:
            {
                double cx = m_pts[src_idx++];
                double cy = m_pts[src_idx++];
                double rx = m_pts[src_idx++];
                double ry = m_pts[src_idx++];
                double sAng = m_pts[src_idx++];
                double eAng = m_pts[src_idx++];
                double rot = m_pts[src_idx++];

                double rcos = cos(rot);
                double rsin = sin(rot);

                // compute the required angular separation between points which
                // gives the desired tolerance - just base it off the max radius
                // and use the formula for a circle
                double maxRadius = rs_max(rx, ry);
                double dAng = sqrt(8.0 * m_xf_tol / maxRadius / m_xf.x0);

                // using the angular separation we can compute the minimum number
                // of segments
                int nSegs = 1 + (int)(fabs(eAng - sAng) / dAng);

                if (nSegs < 0)
                {
                    // can happen due to overflow
                    nSegs = 10 * ARC_TESSELLATION_SEGMENTS;
                }
                else
                {
                    // the number of segments can get big for large arcs, so limit
                    // the number of segments
                    nSegs = rs_min(nSegs, 10 * ARC_TESSELLATION_SEGMENTS);
                }

                // recompute the actual angular separation corresponding to the number
                // of segments
                dAng = (eAng - sAng) / nSegs;

                // add the segments
                m_xf_buf->EnsurePoints(nSegs);

                // store off arc start point index
                m_xf_buf->StoreArcStartIndex();

                for (int i=1; i<=nSegs; ++i)
                {
                    double ang = sAng + i*dAng;
                    double tx = rx * cos(ang);
                    double ty = ry * sin(ang);

                    double x = cx + tx*rcos - ty*rsin;
                    double y = cy + ty*rcos + tx*rsin;

                    m_xf.transform(x, y);

                    m_xf_buf->UnsafeLineTo(x, y);
                }

                // store off arc end point index
                m_xf_buf->StoreArcEndIndex();

                break;
            }
        }
    }

    m_xf_buf->SetGeometryType(LineBuffer::GeometryType_LineString);
}


SE_EllipseDef* SE_LineBuffer::GetEllipseDefinition(int index)
{
    SE_LB_SegType* endseg = m_segs + m_nsegs;
    SE_LB_SegType* curseg = m_segs;

    SE_EllipseDef*  arc_def = NULL; // output
    int             arc_idx = 0;    // elliptical arcs counter

    int src_idx = 0;

    // Locate the arc start in the array of points
    while (curseg != endseg && arc_idx <= index)
    {
        switch (*curseg++)
        {
        case SegType_MoveTo:
        case SegType_LineTo:
            src_idx += 2;
            break;

        case SegType_EllipticalArc:
            {
                if (arc_idx == index)
                {
                    // Arc found
                    arc_def = new SE_EllipseDef;

                    arc_def->cx = m_pts[src_idx++];
                    arc_def->cy = m_pts[src_idx++];
                    arc_def->rx = m_pts[src_idx++];
                    arc_def->ry = m_pts[src_idx++];
                    arc_def->sAng = m_pts[src_idx++];
                    arc_def->eAng = m_pts[src_idx++];
                    arc_def->rot = m_pts[src_idx++];
                    arc_def->xf = m_xf;
                }
                else
                    src_idx += 7;   // continue

                arc_idx++;
            }
            break;
        }
    }

    return arc_def;
}


void SE_LineBuffer::Transform(const SE_Matrix& xform, double tolerance)
{
    if (m_xf_bounds)
    {
        m_xf_bounds->Free();
        m_xf_bounds = NULL;
    }

    m_xf     = xform;
    m_xf_tol = tolerance;

    PopulateXFBuffer();

    if (m_compute_bounds)
    {
        RS_Bounds bounds;
        m_xf_buf->ComputeBounds(bounds);
        m_xf_bounds = GetSEBounds(bounds);
    }
}


bool SE_LineBuffer::Empty()
{
    return m_npts == 0;
}


void SE_LineBuffer::Free()
{
    SE_BufferPool::FreeSELineBuffer(m_pool, this);
}


SE_LineBuffer* SE_LineBuffer::Clone(bool keepPool)
{
    SE_LineBuffer* clone = SE_BufferPool::NewSELineBuffer(m_pool, m_npts);
    clone->m_start[0] = m_start[0];
    clone->m_start[1] = m_start[1];
    clone->m_last[0]  = m_last[0];
    clone->m_last[1]  = m_last[1];
    clone->m_xf             = m_xf;
    clone->m_xf_tol         = m_xf_tol;
    clone->m_compute_bounds = m_compute_bounds;

    // clear the pool if requested
    if (!keepPool)
        clone->m_pool = NULL;

    // clone any bounds
    if (clone->m_xf_bounds)
        clone->m_xf_bounds->Free();
    clone->m_xf_bounds = m_xf_bounds? m_xf_bounds->Clone(keepPool) : NULL;

    // clone area line buffer
    *clone->m_xf_buf = *m_xf_buf;

    // clone remaining items
    int grow_segs = m_nsegs - clone->m_max_segs;
    if (grow_segs > 0)
        ResizeBuffer<SE_LB_SegType>(&clone->m_segs, grow_segs, clone->m_nsegs, clone->m_max_segs);
    int grow_pts = m_npts - clone->m_max_pts;
    if (grow_pts > 0)
        ResizeBuffer<double>(&clone->m_pts, grow_pts, clone->m_npts, clone->m_max_pts);
    memcpy(clone->m_pts, m_pts, sizeof(double)*m_npts);
    memcpy(clone->m_segs, m_segs, sizeof(SE_LB_SegType)*m_nsegs);
    clone->m_nsegs = m_nsegs;
    clone->m_npts = m_npts;
    return clone;
}
