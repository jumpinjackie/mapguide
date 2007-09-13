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
#include "SE_LineStorage.h"
#include "SE_Bounds.h"
#include "SE_BufferPool.h"
#include "SE_ConvexHull.h"

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
    m_xf_buf = pool->NewLineStorage(size);
    m_xf_wt_buf = pool->NewLineStorage(size);
}


SE_LineBuffer::~SE_LineBuffer()
{
    delete[] m_pts;
    delete[] m_segs;
    m_xf_buf->Free();
    m_xf_wt_buf->Free();
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

    double sx = rx*cos(sAngRad);
    double sy = ry*sin(sAngRad);
    double ex = rx*cos(eAngRad);
    double ey = ry*sin(eAngRad);

    if (rotRad != 0)
    {
        double rcos = cos(rotRad);
        double rsin = sin(rotRad);

        double tx = sx;
        double ty = sy;
        sx = tx*rcos - ty*rsin;
        sy = ty*rcos + tx*rsin;

        tx = ex;
        ty = ey;
        ex = tx*rcos - ty*rsin;
        ey = ty*rcos + tx*rsin;
    }

    sx += cx;
    sy += cy;
    ex += cx;
    ey += cy;

    double dsx = m_last[0] - sx;
    double dsy = m_last[1] - sy;
    double dex = m_last[0] - ex;
    double dey = m_last[1] - ey;

    if (dsx*dsx + dsy*dsy > dex*dex + dey*dey)
    {
        // end angle is actually the current line position (i.e. in this case,
        // the arc begins at the end angle, and is CW not CCW)
        double t = sAngRad;
        sAngRad = eAngRad;
        eAngRad = t;
        m_last[0] = sx;
        m_last[1] = sy;
    }
    else
    {
        m_last[0] = ex;
        m_last[1] = ey;
    }

    *cur_pt++ = cx;
    *cur_pt++ = cy;
    *cur_pt++ = rx;
    *cur_pt++ = ry;
    *cur_pt++ = sAngRad;
    *cur_pt++ = eAngRad;
    *cur_pt = rotRad;

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
    m_xf_buf->Reset();
    m_xf_wt_buf->Reset();
    m_xf_join = SE_LineJoin_None;
    m_xf_cap = SE_LineCap_None;
    m_xf_miter_limit = 0.0;
}


void SE_LineBuffer::PopulateXFBuffer()
{
    SE_LB_SegType* endseg = m_segs + m_nsegs;
    SE_LB_SegType* curseg = m_segs;
    int src_idx = 0;
    double x, y;
    while (curseg != endseg)
    {
        switch (*curseg++)
        {
        case SegType_MoveTo:
            m_xf_buf->EnsureContours(1);
            m_xf_buf->EnsurePoints(1);
            m_xf.transform(m_pts[src_idx], m_pts[src_idx+1], x, y);
            m_xf_buf->_MoveTo(x, y);
            src_idx += 2;
            break;

        case SegType_LineTo:
            m_xf_buf->EnsurePoints(1);
            m_xf.transform(m_pts[src_idx], m_pts[src_idx+1], x, y);
            m_xf_buf->_LineTo(x, y);
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
                int nSegs = 1 + (int)(abs(eAng - sAng) / dAng);
                dAng = (eAng - sAng) / nSegs;

                // add the segments
                m_xf_buf->EnsurePoints(nSegs);
                for (int i=1; i<=nSegs; ++i)
                {
                    double ang = sAng + i*dAng;
                    double tx = rx * cos(ang);
                    double ty = ry * sin(ang);

                    double x = cx + tx*rcos - ty*rsin;
                    double y = cy + ty*rcos + tx*rsin;

                    m_xf.transform(x, y);

                    m_xf_buf->_LineTo(x, y);
                }

                break;
            }
        }
    }
}


void SE_LineBuffer::PopulateXFWeightBuffer()
{
    int n_cntrs = m_xf_buf->cntr_count();
    double hweight = m_xf_weight/2.0;

    SE_LineStorage* srcseg = m_pool->NewLineStorage(5);
    SE_LineStorage* lastseg = m_pool->NewLineStorage(5);
    SE_LineStorage* nextseg = m_pool->NewLineStorage(5);

    RS_F_Point prev, vert, next;
    RS_Bounds mbounds(0.0, -hweight, 0.0, hweight);
    double dx, dy, len = 1.0;

    //SE_MiterJoin mj;
    //SE_IdentityJoin idj;

    /* Save a rectangle contour covering x [0, len] y [-hweight, hweight] in srcseg */
    srcseg->EnsureContours(1);
    srcseg->EnsurePoints(5);
    srcseg->_MoveTo(0, hweight);
    srcseg->_LineTo(len, hweight);
    srcseg->_LineTo(len, -hweight);
    srcseg->_LineTo(0, -hweight);
    srcseg->_LineTo(0, hweight);

    for (int i = 0; i < n_cntrs; i++)
    {
        int end = m_xf_buf->contour_end_point(i);
        int pt = m_xf_buf->contour_start_point(i);

        // get penultimate contour point
        m_xf_buf->get_point(m_xf_buf->contour_end_point(i)-1, prev.x, prev.y);

        // get first and second point of contour
        m_xf_buf->get_point(pt++, vert.x, vert.y);
        m_xf_buf->get_point(pt++, next.x, next.y);
        dx = next.x - vert.x;
        dy = next.y - vert.y;
        len = sqrt(dx*dx + dy*dy);
        mbounds.minx = 0.0;
        mbounds.maxx = len;
        srcseg->x_coord(1) = srcseg->x_coord(2) = len;

        // detect contour closure - get ultimate contour point
        bool closed = (m_xf_buf->cntr_size(i) > 2) && m_xf_buf->contour_closed(i);
        //if (closed)
        //{ /* Closed */ /* TODO: stitching joins (requires repeat interval for argument) */
        //    mj = SE_MiterJoin(m_xf_miter_limit, mbounds, 0.0, prev, vert, next, false);
        //    mj.Transform(srcseg, lastseg, 0, 1, true);
        //}
        //else
        //{ /* Open */
        //    // TODO: startcap
        //    idj = SE_IdentityJoin(mbounds, 0, vert, next, false);
        //    idj.Transform(srcseg, lastseg, 0, 1, true);
        //}

        while (pt <= end)
        {
            prev.x = vert.x;
            prev.y = vert.y;
            vert.x = next.x;
            vert.y = next.y;
            m_xf_buf->get_point(pt++, next.x, next.y);

            // update segment bounds
            dx = next.x - vert.x;
            dy = next.y - vert.y;
            len = sqrt(dx*dx + dy*dy);
            srcseg->x_coord(1) = srcseg->x_coord(2) = len;

            switch (m_xf_join)
            {
            case SE_LineJoin_Bevel:
            case SE_LineJoin_Miter:
                {
                    //idj = SE_IdentityJoin(mbounds, len, prev, vert, false);
                    //mj = SE_MiterJoin(m_xf_miter_limit, mbounds, -len, prev, vert, next, true);

                    //mj.Transform(lastseg, m_xf_wt_buf, 0, 1, true);
                    lastseg->Reset();
                    //idj.Transform(srcseg, nextseg, 0, 1, true);
                    //mj.Transform(nextseg, lastseg, 0, 1, true);
                    nextseg->Reset();
                }
                break;
            case SE_LineJoin_None:
            case SE_LineJoin_Round: // TODO: implement Round joins
            default:
                {
                    //idj = SE_IdentityJoin(mbounds, 0, vert, next, false);
                    m_xf_wt_buf->Append(lastseg);
                    lastseg->Reset();
                    //idj.Transform(srcseg, lastseg, 0, 1, true);
                }
                break;
            }
        }

        if (closed)
        {
            m_xf_buf->get_point(1+m_xf_buf->contour_start_point(i), prev.x, prev.y);
            //mj = SE_MiterJoin(m_xf_miter_limit, mbounds, 0.0, vert, next, prev, true);
            //mj.Transform(lastseg, m_xf_wt_buf, 0, 1, true);
        }
        else
        {
            // TODO: Endcap
            m_xf_wt_buf->Append(lastseg);
        }

        lastseg->Reset();
        nextseg->Reset();
        srcseg->Reset();
    }
    lastseg->Free();
    nextseg->Free();
    srcseg->Free();
}


SE_LineStorage* SE_LineBuffer::Transform(const SE_Matrix& xform,
                                         double weight,
                                         SE_LineCap cap,
                                         SE_LineJoin join,
                                         double miterLimit,
                                         double tolerance)
{
    /* TODO: Weight is 0.0 for the moment, pending better rendering for the exploded lines */
    weight = 0.0;
    if (join == SE_LineJoin_Bevel)
    { /* ...There is no bevel... */
        join = SE_LineJoin_Miter;
        miterLimit = 0.0;
    }
    if (m_xf == xform && m_xf_tol == tolerance &&
        m_xf_weight == weight && m_xf_join == join &&
        m_xf_cap == cap && m_xf_miter_limit == miterLimit)
        return m_xf_buf;

    if (m_xf_bounds)
    {
        m_xf_bounds->Free();
        m_xf_bounds = NULL;
    }

    m_xf = xform;
    m_xf_tol = tolerance;
    m_xf_weight = weight;
    m_xf_cap = cap;
    m_xf_join = join;
    m_xf_miter_limit = miterLimit;
    m_xf_buf->Reset();
    m_xf_wt_buf->Reset();

    PopulateXFBuffer();
    if (m_xf_weight > 0.0)
        PopulateXFWeightBuffer();

    if (m_compute_bounds)
    {
        if (m_xf_weight > 0.0)
        {
            m_xf_bounds = ComputeConvexHull(m_xf_wt_buf);
            m_xf_buf->SetBounds(m_xf_bounds);
            m_xf_wt_buf->SetBounds(m_xf_bounds);
        }
        else
        {
            m_xf_bounds = ComputeConvexHull(m_xf_buf);
            m_xf_buf->SetBounds(m_xf_bounds);
        }
    }

    return m_xf_buf;
}


bool SE_LineBuffer::Empty()
{
    return m_npts == 0;
}


void SE_LineBuffer::Free()
{
    m_pool->FreeLineBuffer(this);
}


SE_LineBuffer* SE_LineBuffer::Clone()
{
    SE_LineBuffer* clone = m_pool->NewLineBuffer(m_npts);
    clone->m_start[0] = m_start[0];
    clone->m_start[1] = m_start[1];
    clone->m_last[0] = m_last[0];
    clone->m_last[1] = m_last[1];
    clone->m_compute_bounds = m_compute_bounds;
    clone->m_xf = m_xf;
    clone->m_xf_tol = m_xf_tol;
    clone->m_xf_weight = m_xf_weight;
    if (m_xf_bounds)
        clone->m_xf_bounds = m_xf_bounds->Clone();
    clone->m_xf_buf->SetToCopy(m_xf_buf);
    clone->m_xf_wt_buf->SetToCopy(m_xf_wt_buf);
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
