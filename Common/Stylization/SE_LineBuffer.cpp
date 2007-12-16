//
//  Copyright (C) 2007 by Autodesk, Inc.
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
#include "SE_ConvexHull.h"
#include "SE_RenderProxies.h"
//#include "SE_JoinProcessor_Null.h"

#include <algorithm>

#define GROWTH_FACTOR 1.5

#define ENSURE_POINT_BUFFER(points) \
    if ((m_npts + (points)) > m_max_pts) \
        ResizeBuffer<double>(&m_pts, (points), m_npts, m_max_pts);

#define ENSURE_SEG_BUFFER(points) \
    if ((m_nsegs + (points)) > m_max_segs) \
        ResizeBuffer<SE_LB_SegType>(&m_segs, (points), m_nsegs, m_max_segs);


inline void TransformScale(SE_Matrix& xform, double &sx, double &sy)
{
    sx = sqrt(xform.x0*xform.x0 + xform.x1*xform.x1);
    sy = sqrt(xform.y0*xform.y0 + xform.y1*xform.y1);
}


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


struct PointUtil
{
    SE_INLINE double x(PointList::iterator iter)
    {
        return (*iter).first;
    }

    SE_INLINE double y(PointList::iterator iter)
    {
        return (*iter).second;
    }

    SE_INLINE bool equal(PointList::iterator a, PointList::iterator b)
    {
        return ((*a).first == (*b).first) && ((*a).second == (*b).second);
    }
};


SE_LineBuffer::SE_LineBuffer(int size, SE_BufferPool* pool) :
    m_npts(0),
    m_nsegs(0),
    m_max_pts(2*size),
    m_max_segs(size),
    m_xf_tol(-1.0),
    m_xf_weight(-1.0),
    m_xf_bounds(NULL),
    m_compute_bounds(true),
    m_xf_join(SE_LineJoin_None),
    m_xf_cap(SE_LineCap_None),
    m_xf_miter_limit(0.0),
    m_pool(pool)
{
    m_pts = new double[size*2];
    m_segs = new SE_LB_SegType[size];
    m_area_buf = LineBufferPool::NewLineBuffer(pool, size);
    m_outline_buf = NULL;
    m_xf_style = new SE_RenderLineStyle();
}


SE_LineBuffer::~SE_LineBuffer()
{
    delete[] m_pts;
    delete[] m_segs;
    delete m_xf_style;
    if (m_area_buf != m_outline_buf && m_outline_buf)
        LineBufferPool::FreeLineBuffer(m_pool, m_outline_buf);
    if (m_area_buf)
        LineBufferPool::FreeLineBuffer(m_pool, m_area_buf);
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

    for (int i = 0; i < n_cntrs; i++)
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
    m_xf_tol = m_xf_weight = -1.0;
    m_xf.setIdentity();
    if (m_area_buf)
        m_area_buf->Reset();
    if (m_area_buf != m_outline_buf && m_outline_buf)
        LineBufferPool::FreeLineBuffer(m_pool, m_outline_buf);
    m_outline_buf = NULL;
    m_xf_join = SE_LineJoin_None;
    m_xf_cap = SE_LineCap_None;
    m_xf_miter_limit = 0.0;
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


SE_Bounds* SE_LineBuffer::ComputeConvexHull(LineBuffer* plb)
{
    if (plb->cntr_count() == 0)
        return NULL;

    // There are linear time algorithms, but only for simple (nonintersecting) paths,
    // which we cannot guarantee.
    // TODO: In the unlikely event that this becomes a performance issue, figure out whether geometry
    // is simple (once) at parse-time, and use the faster algorithm in the cases where
    // it is (probably most cases).

    for (int i = 0; i< plb->point_count(); i++)
        m_ch_ptbuf.push_back(std::pair<double, double>(plb->x_coord(i), plb->y_coord(i)));

    std::sort(m_ch_ptbuf.begin(), m_ch_ptbuf.end(), PointLess());
    std::unique(m_ch_ptbuf.begin(), m_ch_ptbuf.end());

    PointList::iterator end = m_ch_ptbuf.end();
    PointList::iterator iter = m_ch_ptbuf.begin();

    if (m_ch_ptbuf.size() < 2)
        return NULL;
    return AndrewHull<PointList::iterator, PointUtil>(iter, --end, (int)m_ch_ptbuf.size(), m_pool);
}


void SE_LineBuffer::PopulateXFBuffer()
{
    SE_LB_SegType* endseg = m_segs + m_nsegs;
    SE_LB_SegType* curseg = m_segs;
    int src_idx = 0;
    double x, y;

    m_area_buf->Reset();
    if (m_outline_buf && m_area_buf != m_outline_buf)
        LineBufferPool::FreeLineBuffer(m_pool, m_outline_buf);
    m_outline_buf = NULL;

    while (curseg != endseg)
    {
        switch (*curseg++)
        {
        case SegType_MoveTo:
            m_area_buf->EnsureContours(1);
            m_area_buf->EnsurePoints(1);
            m_xf.transform(m_pts[src_idx], m_pts[src_idx+1], x, y);
            m_area_buf->UnsafeMoveTo(x, y);
            src_idx += 2;
            break;

        case SegType_LineTo:
            m_area_buf->EnsurePoints(1);
            m_xf.transform(m_pts[src_idx], m_pts[src_idx+1], x, y);
            m_area_buf->UnsafeLineTo(x, y);
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
                // of segments, and then we recompute the actual angular separation
                // corresponding to that number of segments
                int nSegs = 1 + (int)(fabs(eAng - sAng) / dAng);
                dAng = (eAng - sAng) / nSegs;

                // add the segments
                m_area_buf->EnsurePoints(nSegs);
                for (int i=1; i<=nSegs; ++i)
                {
                    double ang = sAng + i*dAng;
                    double tx = rx * cos(ang);
                    double ty = ry * sin(ang);

                    double x = cx + tx*rcos - ty*rsin;
                    double y = cy + ty*rcos + tx*rsin;

                    m_xf.transform(x, y);

                    m_area_buf->UnsafeLineTo(x, y);
                }

                break;
            }
        }
    }

    m_area_buf->SetGeometryType(FdoGeometryType_LineString);

    //This code converts thick polylines to polygons (taking int account cap and join styles).
    //This results in slow processing, so instead of this conversion to polygon, we will rely on
    //the underlying renderer to draw thick lines with correct joins.
    /*
    if (m_xf_weight >= 2.0)
    {
        double ext = m_xf_weight * 0.5;
        m_xf_style->bounds[0] = RS_F_Point(0.0, -ext);
        m_xf_style->bounds[1] = RS_F_Point(0.0,  ext);
        m_xf_style->bounds[2] = RS_F_Point(0.0,  ext);
        m_xf_style->bounds[3] = RS_F_Point(0.0, -ext);
        m_xf_style->startOffset = m_xf_style->endOffset = 0.0;
        m_xf_style->vertexMiterLimit = m_xf_miter_limit;

        m_outline_buf = LineBufferPool::NewLineBuffer(m_pool, m_area_buf->point_count());

        for (int i = 0; i < m_area_buf->cntr_count(); ++i)
        {
            SE_JoinProcessor_Null processor(m_xf_join, m_xf_cap, m_area_buf, i, m_xf_style);
            processor.AppendOutline(m_outline_buf);
        }
    }
    else
    */
        m_outline_buf = m_area_buf;
}


LineBuffer* SE_LineBuffer::Transform(const SE_Matrix& xform, double tolerance, SE_RenderPolyline* rp)
{
    if ( m_xf == xform &&
         m_xf_weight == rp->weight &&
         m_xf_cap == rp->cap &&
         m_xf_join == rp->join &&
         m_xf_miter_limit == rp->miterLimit )
        return m_outline_buf;

    if (m_xf_bounds)
    {
        m_xf_bounds->Free();
        m_xf_bounds = NULL;
    }

    m_xf = xform;
    m_xf_weight = rp->weight;
    m_xf_cap = rp->cap;
    m_xf_join = rp->join;
    m_xf_miter_limit = rp->miterLimit;
    m_xf_tol = tolerance;

    PopulateXFBuffer();

    if (m_compute_bounds)
    {
        RS_Bounds bounds;
        m_outline_buf->ComputeBounds(bounds);
        // don't need a convex hull for now
//      m_xf_bounds = ComputeConvexHull(m_xf_buf);
        m_xf_bounds = GetSEBounds(bounds);

        if (m_area_buf != m_outline_buf)
            m_area_buf->ComputeBounds(bounds);
    }

    return m_outline_buf;
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
    clone->m_last[0] = m_last[0];
    clone->m_last[1] = m_last[1];
    clone->m_compute_bounds = m_compute_bounds;
    clone->m_xf = m_xf;
    clone->m_xf_tol = m_xf_tol;
    clone->m_xf_weight = m_xf_weight;

    // clear the pool if requested
    if (!keepPool)
        clone->m_pool = NULL;

    // clone any bounds
    if (clone->m_xf_bounds)
        clone->m_xf_bounds->Free();
    clone->m_xf_bounds = m_xf_bounds? m_xf_bounds->Clone(keepPool) : NULL;

    // clone any line buffer
    if (m_outline_buf)
    {
        if (!clone->m_outline_buf)
            clone->m_outline_buf = LineBufferPool::NewLineBuffer(m_pool, m_outline_buf->point_count());
        *clone->m_outline_buf = *m_outline_buf;
    }
    else
    {
        if (clone->m_outline_buf)
            LineBufferPool::FreeLineBuffer(m_pool, clone->m_outline_buf);
        clone->m_outline_buf = NULL;
    }
    if (m_area_buf)
    {
        if (m_area_buf == m_outline_buf)
            clone->m_area_buf = clone->m_outline_buf;
        else
        {
            if (!clone->m_area_buf)
                clone->m_area_buf = LineBufferPool::NewLineBuffer(m_pool, m_area_buf->point_count());
            *clone->m_area_buf = *m_area_buf;
        }
    }
    else
    {
        if (clone->m_area_buf)
            LineBufferPool::FreeLineBuffer(m_pool, clone->m_area_buf);
        clone->m_area_buf = NULL;
    }

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
