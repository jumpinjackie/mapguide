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

#include "SE_MiterJoin.h"
#include "SE_IdentityJoin.h"

#include <algorithm>

#define GROWTH_FACTOR 1.5

#define ENSURE_POINT_BUFFER(points) \
    if ((m_npts + (points)) > m_max_pts) \
        ResizeBuffer<double>(&m_pts, (points), m_npts, m_max_pts);

#define ENSURE_SEG_BUFFER(points) \
    if ((m_nsegs + (points)) > m_max_segs) \
        ResizeBuffer<SE_LB_SegType>(&m_segs, (points), m_nsegs, m_max_segs);

inline void SineCosineMax(double sAng, double sSine, double sCosine, double eAng, double eSine, double eCosine, double &maxSine, double &maxCosine)
{
    int quadrants = ((int)(sAng*2.0/M_PI) << 2) | (int)(eAng*2.0/M_PI);

    switch(quadrants)
    {
    case 0: /* Q1 -> Q1, sine increasing, cosine decreasing */
        maxSine = eSine;
        maxCosine = sCosine;
        return;
    case 1: /* Q1 -> Q2, sine local maximum, cosine local minimum */
        maxSine = 1.0;
        maxCosine = (sCosine > eCosine ? sCosine : eCosine);
        return;
    case 2: /* Q1 -> Q3, sine local maximum, cosine local maximum */
        maxSine = 1.0;
        maxCosine = 1.0;
        return;
    case 3: /* Q1 -> Q4, sine local maximum, cosine local maximum */
        maxSine = 1.0;
        maxCosine = 1.0;
        return;
    /* case 4: Q2 -> Q1 */
    case 5: /* Q2 -> Q2, sine decreasing, cosine increasing */
        maxSine = sSine;
        maxCosine = eCosine;
        return;
    case 6: /* Q2 -> Q3, sine decreasing, increasing, cosine local maximum */
        maxSine = (sSine > eSine ? sSine : eSine);
        maxCosine = 1.0;
        return;
    case 7: /* Q2 -> Q4, sine local maximum, cosine local maximum */
        maxSine = 1.0;
        maxCosine = 1.0;
        return;
    /* case 8:  Q3 -> Q1 */
    /* case 9:  Q3 -> Q2 */
    case 10: /* Q3 -> Q3, sine increasing, cosine decreasing */
        maxSine = eSine;
        maxCosine = sCosine;
        return;
    case 11: /* Q3 -> Q4, sine local maximum, cosine local minimum */
        maxSine = 1.0;
        maxCosine = (sCosine > eCosine ? sCosine : eCosine);
        return;
    /* case 12: Q4 -> Q1 */
    /* case 13: Q4 -> Q2 */
    /* case 14: Q4 -> Q3 */
    case 15: /* Q4 -> Q4, sine decreasing, cosine increasing */
        maxSine = sSine;
        maxCosine = eCosine;
        return;
    default:
        maxSine = 1.0;
        maxCosine = 1.0;
        return;
    }
}

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

SE_Bounds* SE_LineBuffer::ComputeConvexHull(double* pnts, int* cntrs, int ncntrs)
{
    if (ncntrs == 0)
        return NULL;

    // There are linear time algorithms, but only for simple (nonintersecting) paths, 
    // which we cannot guarantee.
    // TODO: In the unlikely event that this becomes a performance issue, figure out whether geometry 
    // is simple (once) at parse-time, and use the faster algorithm in the cases where 
    // it is (probably most cases).

    double* cur = pnts;
    double* last;
    m_ch_ptbuf.clear();
    for (int i = 0; i < ncntrs; i++)
    {
        last = cur + cntrs[i]*2;

        double x, y;
        while(cur < last)
        {
            x = *cur++;
            y = *cur++;
            m_ch_ptbuf.push_back(std::pair<double,double>(x, y));
        }
    }

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

void SE_LineBuffer::EllipticalArcTo(double cx, double cy, double rx, double ry, double sAng, double eAng, double rotation)
{
    ENSURE_POINT_BUFFER(7);
    ENSURE_SEG_BUFFER(1);

    double* cur_pt = m_pts + m_npts;
    double sx, sy, ex, ey, dsx, dsy, dex, dey;

    sx = cx + rx*cos(sAng);
    sy = cy + ry*sin(sAng);
    ex = cx + rx*cos(eAng);
    ey = cy + ry*sin(eAng);

    if (rotation != 0)
    {
        double rs = sin(rotation);
        double rc = cos(rotation);
        double tx, ty;
        tx = sx; ty = sy;
        sx = rc*sx + rs*sy;
        sy = rc*sy - rs*sx;
        tx = ex; ty = ey;
        ex = rc*ex + rs*ey;
        ey = rc*ey - rs*ex;
    }

    dsx = m_last[0] - sx;
    dsy = m_last[1] - sy;
    dex = m_last[0] - ex;
    dey = m_last[1] - ey;

    if (dsx*dsx + dsy*dsy > dex*dex + dey*dey)
    {
        /* End angle is actually the current line position (i.e. In this case, the arc begins
         * at the end angle, and is CW, not CCW). */
        double t = sAng;
        sAng = eAng;
        eAng = t;
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
    *cur_pt++ = sAng;
    *cur_pt++ = eAng;
    *cur_pt = rotation;

    m_npts += 7;
    m_segs[m_nsegs++] = SegType_EllipticalArc;
}

void SE_LineBuffer::SetGeometry(LineBuffer* srclb)
{
    if (m_npts != 0)
        Reset();
    int n_cntrs = srclb->cntr_count();
    int* contours = srclb->cntrs();
    double* src = srclb->points();

    for (int i = 0; i < n_cntrs; i++)
    {
        double x, y;
        double* last = src + 2*contours[i];
        x = *src++;
        y = *src++;
        SE_LineBuffer::MoveTo(x, y);

        while (src < last)
        {
            x = *src++;
            y = *src++;
            SE_LineBuffer::LineTo(x, y);
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
    while(curseg != endseg)
    {
        switch(*curseg++)
        {
        case SegType_MoveTo:
            m_xf_buf->EnsureContours(1);
            m_xf_buf->EnsurePoints(1);
            m_xf.transform(m_pts[src_idx], m_pts[src_idx+1], x, y);
            m_xf_buf->_MoveTo(x,y);
            src_idx += 2;
            break;
        case SegType_LineTo:
            m_xf_buf->EnsurePoints(1);
            m_xf.transform(m_pts[src_idx], m_pts[src_idx+1], x, y);
            m_xf_buf->_LineTo(x,y);
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

                /* TODO: change the # of segments based on the tolerance?...up to 6 for now */
                int nsegs = (int)(4.0*fabs(eAng - sAng)/(2*M_PI)) + 1; // eAng - sAng is (0, 2pi), so this is {1,2,3,4}.
                double span = (eAng - sAng)/(double)nsegs;
                double aspan = fabs(span);
                
                double sec = 1.0/cos(aspan/2.0);
                double alpha = sin(aspan)*(sqrt(1.0 + 3.0/sec/sec) - 1)/3.0;
                double rcos = cos(rot), rsin = sin(rot);
                
                double ex, ey, sx, sy;
                double scos, ssin, ecos, esin;
                double sa, ea;

                ecos = cos(sAng);
                esin = sin(sAng);
                ex = rx*ecos;
                ey = ry*esin;
                ea = sAng;

                double cw = sAng > eAng ? -1.0 : 1.0;

                for (int i = 0; i < nsegs; i++)
                {
                    sa = ea;
                    ea = sa + span;

                    scos = ecos;
                    ssin = esin;
                    ecos = cos(ea);
                    esin = sin(ea);

                    sx = ex;
                    sy = ey;
                    ex = rx*ecos;
                    ey = ry*esin;

                    double c1x, c1y, c2x, c2y;

                    c1x = sx - cw*alpha*rx*ssin;
                    c1y = sy + cw*alpha*ry*scos;
                    c2x = ex + cw*alpha*rx*esin;
                    c2y = ey - cw*alpha*ry*ecos;

                    /* Here, we find the maximum 2nd derivative over the interval in question, and
                       calculate the number of segments, assuming a deviation of the segment length *
                       the second derivative must be within the tolerance */
                    double mx, my;
                    SineCosineMax(sa, ssin, scos, ea, esin, ecos, my, mx);
                    mx *= sqrt(m_xf.x0*m_xf.x0 + m_xf.y0*m_xf.y0)*rx;
                    my *= sqrt(m_xf.x1*m_xf.x1 + m_xf.y1*m_xf.y1)*ry;
                    int steps = (int)((aspan/m_xf_tol)*sqrt(mx*mx + my*my)) + 1;
                    m_xf_buf->EnsurePoints(steps + 1);

                    if (rot != 0.0)
                    {
                        double tx, ty;
                        tx = ex, ty = ey;
                        ex = tx*rcos + ty*rsin;
                        ey = ty*rcos - tx*rsin;
                        tx = c1x, ty = c1y;
                        c1x = tx*rcos + ty*rsin;
                        c1y = ty*rcos - tx*rsin;
                        tx = c2x, ty = c2y;
                        c2x = tx*rcos + ty*rsin;
                        c2y = ty*rcos - tx*rsin;
                        tx = mx, ty = my;
                    }

                    c1x += cx;
                    c1y += cy;
                    c2x += cx;
                    c2y += cy;

                    TessellateCubicTo(m_xf_buf, c1x, c1y, c2x, c2y, ex + cx, ey + cy, steps);
                }

                break;
            }
        }
    }
}

void SE_LineBuffer::PopulateXFWeightBuffer()
{
    int n_cntrs = m_xf_buf->cntr_count();
    int* contours = m_xf_buf->cntrs();
    double* src = m_xf_buf->points();
    double hweight = m_xf_weight/2.0;

    SE_LineStorage* srcseg = m_pool->NewLineStorage(5);
    SE_LineStorage* lastseg = m_pool->NewLineStorage(5);
    SE_LineStorage* nextseg = m_pool->NewLineStorage(5);

    RS_F_Point prev, vert, next;
    RS_Bounds mbounds(0.0, -hweight, 0.0, hweight);
    double *maxx[2]; /* The points in segbuf that change with seg width */
    double dx, dy, len = 1.0;

    SE_MiterJoin mj;
    SE_IdentityJoin idj;

    /* Save a rectangle contour covering x [0, len] y [-hweight, hweight] in srcseg */
    srcseg->EnsureContours(1);
    srcseg->EnsurePoints(5);
    srcseg->_MoveTo(0, hweight);
    maxx[0] = srcseg->points() + 2;
    srcseg->_LineTo(len, hweight);
    maxx[1] = maxx[0] + 2;
    srcseg->_LineTo(len, -hweight);
    srcseg->_LineTo(0, -hweight);
    srcseg->_LineTo(0, hweight);

    for (int i = 0; i < n_cntrs; i++)
    {
        double* last = src + 2*contours[i];
        double* first = src;

        prev.x = last[-4];
        prev.y = last[-3];
        vert.x = *src++;
        vert.y = *src++;
        next.x = *src++;
        next.y = *src++;
        dx = next.x - vert.x;
        dy = next.y - vert.y;
        len = sqrt(dx*dx + dy*dy);
        mbounds.minx = 0.0;
        mbounds.maxx = len;
        *maxx[0] = *maxx[1] = len;
        
        bool closed = vert.x == last[-2] && vert.y == last[-1] && (contours[i] > 2);
        if (closed)
        { /* Closed */ /* TODO: stitching joins (requires repeat interval for argument) */
            mj = SE_MiterJoin(m_xf_miter_limit, mbounds, 0.0, prev, vert, next, false);
            mj.Transform(srcseg, lastseg, 0, 1, true);
        }
        else
        { /* Open */
            // Todo: startcap
            idj = SE_IdentityJoin(mbounds, 0, vert, next, false);
            idj.Transform(srcseg, lastseg, 0, 1, true);
        }

        while (src < last)
        {
            prev.x = vert.x;
            prev.y = vert.y;
            vert.x = next.x;
            vert.y = next.y;
            next.x = *src++;
            next.y = *src++;
            /* Update segment bounds */
            dx = next.x - vert.x;
            dy = next.y - vert.y;
            len = sqrt(dx*dx + dy*dy);
            *maxx[0] = *maxx[1] = len;

            switch (m_xf_join)
            {
            case SE_LineJoin_Bevel:
            case SE_LineJoin_Miter:
                {
                    idj = SE_IdentityJoin(mbounds, len, prev, vert, false);
                    mj = SE_MiterJoin(m_xf_miter_limit, mbounds, -len, prev, vert, next, true);

                    mj.Transform(lastseg, m_xf_wt_buf, 0, 1, true);
                    lastseg->Reset();
                    idj.Transform(srcseg, nextseg, 0, 1, true);
                    mj.Transform(nextseg, lastseg, 0, 1, true);
                    nextseg->Reset();
                }
                break;
            case SE_LineJoin_None:
            case SE_LineJoin_Round: // TODO: implement Round joins
            default:
                {
                    idj = SE_IdentityJoin(mbounds, 0, vert, next, false);
                    m_xf_wt_buf->Append(lastseg);
                    lastseg->Reset();
                    idj.Transform(srcseg, lastseg, 0, 1, true);
                }
                break;
            }
        }
 
        if (closed)
        {
            prev.x = first[2];
            prev.y = first[3];
            mj = SE_MiterJoin(m_xf_miter_limit, mbounds, 0.0, vert, next, prev, true);
            mj.Transform(lastseg, m_xf_wt_buf, 0, 1, true);            
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
            m_xf_bounds = ComputeConvexHull(m_xf_wt_buf->points(), m_xf_wt_buf->cntrs(), 
                m_xf_wt_buf->cntr_count());
            m_xf_buf->SetBounds(m_xf_bounds);
            m_xf_wt_buf->SetBounds(m_xf_bounds);
        }
        else
        {
            m_xf_bounds = ComputeConvexHull(m_xf_buf->points(), m_xf_buf->cntrs(), 
                m_xf_buf->cntr_count());
            m_xf_buf->SetBounds(m_xf_bounds);
        }
    }

    return m_xf_buf;
}

SE_LineStorage* SE_LineBuffer::TransformInstance(SE_PiecewiseTransform** ppxf, int xflen, bool closed)
{
    if (xflen == 0)
        return m_xf_wt_buf;

    SE_LineStorage* source = m_xf_weight > 0.0 ? m_xf_wt_buf : m_xf_buf;
    SE_LineStorage* instance = m_pool->NewLineStorage(source->point_count());
 
    ppxf[0]->Transform(source, instance, 0, source->cntr_count(), closed);

    for (int i = 1; i < xflen; i++)
    {
        source = instance;
        instance = m_pool->NewLineStorage(source->point_count());
        ppxf[i]->Transform(source, instance, 0, source->cntr_count(), closed);
        source->Free();
    }

    /* Todo: bounds */

    return instance;
}

bool SE_LineBuffer::Empty()
{
    return m_npts == 0;
}

void SE_LineBuffer::Free()
{
    m_pool->FreeLineBuffer(this);
}

void SE_LineBuffer::TessellateCubicTo(SE_LineStorage* lb, double px2, double py2, double px3, double py3, double px4, double py4, int steps)
{
    //get the start point
    double* pts = lb->points() + 2*lb->point_count();
    double px1 = pts[-2];
    double py1 = pts[-1];

    m_xf.transform(px2, py2);
    m_xf.transform(px3, py3);
    m_xf.transform(px4, py4);

    double dt = 1.0/(double)steps;

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

    // forward differencing loop
    while(steps--)
    {
        fx   += dfx;
        fy   += dfy;
        dfx  += ddfx;
        dfy  += ddfy;
        ddfy += dddfy;
        ddfx += dddfx;

        lb->_LineTo(fx, fy);
    }

    lb->_LineTo(px4, py4);
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