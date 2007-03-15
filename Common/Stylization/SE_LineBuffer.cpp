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
#include "SE_ConvexHull.h"
#include "SE_Bounds.h"

#define GROWTH_FACTOR 1.5

#define ENSURE_POINT_BUFFER(points) \
    if ((m_npts + (points)) > m_max_pts) \
        ResizeBuffer((void**)&m_pts, sizeof(double), (points), m_npts, m_max_pts);

#define ENSURE_SEG_BUFFER(points) \
    if ((m_nsegs + (points)) > m_max_segs) \
        ResizeBuffer((void**)&m_segs, sizeof(SE_LB_SegType), (points), m_nsegs, m_max_segs);

class SE_LineStorage : public LineBuffer
{
public:
    SE_INLINE SE_LineStorage(int size);
    SE_INLINE void EnsurePoints(int n);
    SE_INLINE void EnsureContours(int n);
    SE_INLINE void _MoveTo(double x, double y);
    SE_INLINE void _LineTo(double x, double y);
    SE_INLINE void SetBounds(double minx, double miny, double maxx, double maxy);
    SE_INLINE void SetBounds(SE_Bounds* bounds);

    void SetToTransform(const SE_Matrix& xform, LineBuffer* src);
    void SetToCopy(SE_LineStorage* src);
private:
    SE_INLINE double& _LastX() { return m_last_x; }
    SE_INLINE double& _LastY() { return m_last_y; }
};

SE_LineStorage::SE_LineStorage(int size) :
    LineBuffer(size)
{
}

void SE_LineStorage::_MoveTo(double x, double y)
{
    m_types[m_cur_types++] = (unsigned char)stMoveTo;
    m_pts[m_cur_pts++] = x;
    m_pts[m_cur_pts++] = y;

    m_last_x = x;
    m_last_y = y;

    m_cntrs[++m_cur_cntr] = 1; //increment to next contour and count the point
}


void SE_LineStorage::_LineTo(double x, double y)
{
    m_types[m_cur_types++] = (unsigned char)stLineTo;
    m_pts[m_cur_pts++] = x;
    m_pts[m_cur_pts++] = y;

    m_cntrs[m_cur_cntr]++;
}

void SE_LineStorage::EnsurePoints(int n)
{
    if (m_cur_pts + 2*n >= m_pts_len)
    {
        int len = 2*m_pts_len;
        if (len - m_pts_len < 2*n)
            len += 2*n;

        double* tempPts = new double[len];
        memcpy(tempPts, m_pts, sizeof(double)*m_pts_len);
        delete[] m_pts;
        m_pts = tempPts;
        m_pts_len = len;

        len /= 2;
        unsigned char* tempTypes = new unsigned char[len];
        memcpy(tempTypes, m_types, sizeof(unsigned char) * m_types_len);
        delete[] m_types;
        m_types = tempTypes;
        m_types_len = len;
    }
}

void SE_LineStorage::EnsureContours(int n)
{
    if (m_cur_cntr + 2 + n > m_cntrs_len)
    {
        int len = 2*m_cntrs_len;
        if (len - m_pts_len < n)
            len += n;

        int* tempCntrs = new int[len];
        memcpy(tempCntrs, m_cntrs, sizeof(int)*m_cntrs_len);
        delete[] m_cntrs;
        m_cntrs = tempCntrs;
        m_cntrs_len = len;
    }
}

void SE_LineStorage::SetBounds(double minx, double miny, double maxx, double maxy)
{
    m_bounds.minx = minx;
    m_bounds.miny = miny;
    m_bounds.maxx = maxx;
    m_bounds.maxy = maxy;
}

void SE_LineStorage::SetBounds(SE_Bounds* bounds)
{
    m_bounds.minx = bounds->min[0];
    m_bounds.miny = bounds->min[1];
    m_bounds.maxx = bounds->max[0];
    m_bounds.maxy = bounds->max[1];
}

void SE_LineStorage::SetToTransform(const SE_Matrix& xform, LineBuffer* lb)
{
    EnsurePoints(lb->point_count());
    EnsureContours(lb->cntr_count());

    int n_cntrs = lb->cntr_count();
    int* contours = lb->cntrs();
    double* src = lb->points();
    double* dst = m_pts;

    for (int i = 0; i < n_cntrs; i++)
    {
        double sx, sy, *tx, *ty;
        double* last = src + 2*contours[i];
        sx = *src++;
        sy = *src++;
        xform.transform(sx, sy, m_last_x, m_last_y);
        *dst++ = m_last_x;
        *dst++ = m_last_y;

        while (src < last)
        {
            sx = *src++;
            sy = *src++;
            tx = dst++;
            ty = dst++;
            xform.transform(sx, sy, *tx, *ty);
        }
    }

    memcpy(m_types, lb->types(), lb->point_count());
    memcpy(m_cntrs, lb->cntrs(), sizeof(int)*lb->cntr_count());

    m_cur_types = lb->point_count();
    m_cur_pts = m_cur_types*2;
    m_cur_cntr = lb->cntr_count() - 1;

    m_geom_type = lb->geom_type();
    xform.transform(m_last_x, m_last_y);
}

void SE_LineStorage::SetToCopy(SE_LineStorage* src)
{
    m_cur_pts = m_cur_types = 0;
    m_cur_cntr = -1;
    m_last_x = src->_LastX();
    m_last_y = src->_LastY();
    m_bounds = src->bounds();
    m_geom_type = src->geom_type();
    int grow_types = src->point_count() - m_types_len;
    if (grow_types > 0)
        EnsurePoints(m_types_len + grow_types);
    int grow_cntrs = src->cntr_count() - m_cntrs_len;
    if (grow_cntrs > 0)
        EnsureContours(m_cntrs_len + grow_cntrs);
    memcpy(m_pts, src->points(), sizeof(double)*2*src->point_count());
    memcpy(m_types, src->types(), src->point_count());
    memcpy(m_cntrs, src->cntrs(), sizeof(int)*src->cntr_count());
    m_cur_pts = src->point_count()*2;
    m_cur_types = src->point_count();
    m_cur_cntr = src->cntr_count() - 1;
}

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

inline void LineExt(double dx, double dy, double hweight, double &vx, double &vy)
{
    double scale = hweight/sqrt(dx*dx + dy*dy);
    vx = dy*scale;
    vy = -dy*scale;
}

struct PointUtil
{
    SE_INLINE double x(std::set<std::pair<double,double>, PointLess>::iterator iter)
    {
        return (*iter).first;
    }

    SE_INLINE double y(std::set<std::pair<double,double>, PointLess>::iterator iter)
    {
        return (*iter).second;
    }

    SE_INLINE bool equal(std::set<std::pair<double,double>, PointLess>::iterator a,
                         std::set<std::pair<double,double>, PointLess>::iterator b)
    {
        return ((*a).first == (*b).first) && ((*a).second == (*b).second);
    }
};

SE_Bounds* SE_LineBuffer::ComputeConvexHull(double* pnts, int* cntrs, int ncntrs, double weight)
{
    if (ncntrs == 0)
        return NULL;
    // Monotone Chain Convex Hull Algorithm (Andrew, 1979)
    m_ch_ptbuf.clear();

    double hweight = weight/2.0;
    double* cur = pnts;
    double* last;
    for (int i = 0; i < ncntrs; i++)
    {
        last = cur + cntrs[i]*2;

        if (weight > 0)
        {
            double x, y, lx, ly, vx, vy;
            x = *cur++;
            y = *cur++;
            lx = *cur++;
            ly = *cur++;
            LineExt(lx-x, ly-y, hweight, vx, vy);
            /* For each segment, add the bounding box of the rectangle formed by applying line weight */
            /* TODO: this result will be correct if the line is drawn with bevel joins...Map and MG,
               however, appear to be using round joins at the moment. */
            m_ch_ptbuf.insert(std::pair<double,double>(x + vx, y + vy));
            m_ch_ptbuf.insert(std::pair<double,double>(x - vx, y - vy));
            m_ch_ptbuf.insert(std::pair<double,double>(lx + vx, ly + vy));
            m_ch_ptbuf.insert(std::pair<double,double>(lx - vx, ly - vy));

            while(cur < last)
            {
                x = *cur++;
                y = *cur++;
                LineExt(x-lx, y-ly, hweight, vx, vy);
                m_ch_ptbuf.insert(std::pair<double,double>(x + vx, y + vy));
                m_ch_ptbuf.insert(std::pair<double,double>(x - vx, y - vy));
                m_ch_ptbuf.insert(std::pair<double,double>(lx + vx, ly + vy));
                m_ch_ptbuf.insert(std::pair<double,double>(lx - vx, ly - vy));
                lx = x;
                ly = y;
            }
        }
        else /* Weight might be zero...such as if the buffer holds pure geometry from a LineBuffer */
        {
            double x, y;
            while(cur < last)
            {
                x = *cur++;
                y = *cur++;
                m_ch_ptbuf.insert(std::pair<double,double>(x, y));
            }
        }
    }

    std::set<std::pair<double,double>, PointLess>::iterator end = m_ch_ptbuf.end();
    std::set<std::pair<double,double>, PointLess>::iterator iter = m_ch_ptbuf.begin();

    if (m_ch_ptbuf.size() < 2)
        return NULL;
    return AndrewHull<std::set<std::pair<double,double>, PointLess>::iterator, PointUtil>
        (iter, --end, (int)m_ch_ptbuf.size(), m_pool);
}

SE_LineBuffer::SE_LineBuffer(int size) :
    m_npts(0),
    m_nsegs(0),
    m_max_pts(2*size),
    m_max_segs(size),
    m_xf_tol(-1.0),
    m_xf_weight(-1.0),
    m_src_lb(NULL),
    m_xf_bounds(NULL),
    m_inst_bounds(NULL),
    m_compute_bounds(true)
{
    m_pts = new double[size*2];
    m_segs = new SE_LB_SegType[size];
    m_xf_buf = new SE_LineStorage(size);
    m_inst_buf = new SE_LineStorage(size);
}

SE_LineBuffer::~SE_LineBuffer()
{
    delete[] m_pts;
    delete[] m_segs;
    delete m_xf_buf;
    delete m_inst_buf;
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

    /* End angle is actually the current line position */
    if (dsx*dsx + dsy*dsy > dex*dex + dey*dey)
    {
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
    if (m_inst_bounds)
    {
        m_inst_bounds->Free();
        m_inst_bounds = NULL;
    }
    m_src_lb = NULL;
    m_xf_tol = m_xf_weight = -1.0;
    m_xf.setIdentity();
    m_xf_buf->Reset();
    m_inst_buf->Reset();
}

void SE_LineBuffer::ResizeBuffer(void** buffer, int unitsize, int mininc, int cur_pts, int& max_pts)
{
    int max_newpts = (int)(GROWTH_FACTOR*max_pts) + 1;
    if (max_newpts - max_pts < mininc)
        max_newpts += mininc;

    void* newbuf = new char[unitsize*max_newpts];
    memcpy(newbuf, *buffer, cur_pts*unitsize);
    delete[] *buffer;
    *buffer = newbuf;
    max_pts = max_newpts;
}

LineBuffer* SE_LineBuffer::Transform(const SE_Matrix& xform, double weight, double tolerance)
{
    if (m_xf == xform && m_xf_tol == tolerance && m_xf_weight == weight)
        return m_xf_buf;

    if (m_xf_bounds)
    {
        m_xf_bounds->Free();
        m_xf_bounds = NULL;
    }

    if (m_src_lb)
        return Transform(m_src_lb, xform, weight);

    m_xf = xform;
    m_xf_tol = tolerance;
    m_xf_weight = weight;
    m_xf_buf->Reset();

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
            xform.transform(m_pts[src_idx], m_pts[src_idx+1], x, y);
            m_xf_buf->_MoveTo(x,y);
            src_idx += 2;
            break;
        case SegType_LineTo:
            m_xf_buf->EnsurePoints(1);
            xform.transform(m_pts[src_idx], m_pts[src_idx+1], x, y);
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

                    /* Here, we fine the maximum 2nd derivative over the interval in question, and
                       calculate the number of segments, assuming a deviation of the segment length *
                       the second derivative must be within the tolerance */
                    double mx, my;
                    SineCosineMax(sa, ssin, scos, ea, esin, ecos, my, mx);
                    mx *= rx;
                    my *= ry;
                    int steps = (int)((aspan/tolerance)*sqrt(mx*mx + my*my)) + 1;
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

    if (m_compute_bounds)
        m_xf_bounds = ComputeConvexHull(m_xf_buf->points(), m_xf_buf->cntrs(), m_xf_buf->cntr_count(), m_xf_weight);
    if (m_xf_bounds)
        m_xf_buf->SetBounds(m_xf_bounds);

    return m_xf_buf;
}

LineBuffer* SE_LineBuffer::Transform(LineBuffer* lb, const SE_Matrix& xform, double weight)
{
    m_src_lb = lb;
    m_xf = xform;
    m_xf_tol = 0.0;
    m_xf_weight = weight;

    if (m_xf_bounds)
    {
        m_xf_bounds->Free();
        m_xf_bounds = NULL;
    }

    m_xf_buf->SetToTransform(xform, lb);
    if (m_compute_bounds)
        m_xf_bounds = ComputeConvexHull(m_xf_buf->points(), m_xf_buf->cntrs(), m_xf_buf->cntr_count(), m_xf_weight);
    if (m_xf_bounds)
        m_xf_buf->SetBounds(m_xf_bounds);

    return m_xf_buf;
}

LineBuffer* SE_LineBuffer::TransformInstance(const SE_Matrix& xform)
{
    m_inst_buf->SetToTransform(xform, m_xf_buf);

    if (m_inst_bounds)
    {
        m_inst_bounds->Free();
        m_inst_bounds = NULL;
    }

    if (m_xf_bounds && m_compute_bounds)
    {
        m_inst_bounds = m_pool->NewBounds(m_xf_bounds->size);
        m_inst_bounds->Transform(xform, m_xf_bounds);
        m_inst_buf->SetBounds(m_inst_bounds);
    }

    return m_inst_buf;
}

bool SE_LineBuffer::Empty()
{
    if (m_src_lb)
        return m_src_lb->point_count() == 0;
    else
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

void SE_LineBuffer::LongestEdge(LineBuffer* lb, double& x0, double& y0, double& x1, double& y1)
{
    int* contours = lb->cntrs();
    int* cntr_end = contours + lb->cntr_count();
    double* points = lb->points();

    while(contours < cntr_end)
    {
        double* pnt_end = points + 2*(*contours++);
        double maxlensq = -1.0, lastx, lasty;
        lastx = x0 = x1 = *points++;
        lasty = y0 = y1 = *points++;

        while (points < pnt_end)
        {
            double x = *points++;
            double y = *points++;
            double lensq = (x - lastx)*(x - lastx) + (y - lasty)*(y - lasty);
            if (lensq > maxlensq)
            {
                maxlensq = lensq;
                x0 = lastx; y0 = lasty;
                x1 = x; y1 = y;
            }
            lastx = x;
            lasty = y;
        }
    }
}

SE_LineBuffer* SE_LineBuffer::Clone()
{
    SE_LineBuffer* clone = m_pool->NewLineBuffer(m_npts);
    clone->m_src_lb = m_src_lb;
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
    if (m_inst_bounds)
        clone->m_inst_bounds = m_inst_bounds->Clone();
    clone->m_xf_buf->SetToCopy(m_xf_buf);
    clone->m_inst_buf->SetToCopy(m_inst_buf);
    int grow_segs = m_nsegs - clone->m_max_segs;
    if (grow_segs > 0)
        ResizeBuffer((void**)&clone->m_segs, sizeof(double), grow_segs, clone->m_nsegs, clone->m_max_segs);
    int grow_pts = m_npts - clone->m_max_pts;
    if (grow_pts > 0)
        ResizeBuffer((void**)&clone->m_pts, sizeof(double), grow_pts, clone->m_npts, clone->m_max_pts);
    memcpy(clone->m_pts, m_pts, sizeof(double)*m_npts);
    memcpy(clone->m_segs, m_segs, sizeof(SE_LB_SegType)*m_nsegs);
    clone->m_nsegs = m_nsegs;
    clone->m_npts = m_npts;
    return clone;
}

SE_LineBufferPool::~SE_LineBufferPool()
{
    while (!m_lb_pool.empty())
        delete m_lb_pool.pop();
    while (!m_bnd_pool.empty())
        free(m_bnd_pool.pop());
}

SE_LineBuffer* SE_LineBufferPool::NewLineBuffer(int requestSize)
{
    if (!m_lb_pool.empty())
    {
        SE_LineBuffer* lb = m_lb_pool.pop();
        return lb;
    }
    else
    {
        SE_LineBuffer* buf = new SE_LineBuffer(requestSize);
        buf->m_pool = this;
        return buf;
    }
}

void SE_LineBufferPool::FreeLineBuffer(SE_LineBuffer* lb)
{
    lb->Reset();
    m_lb_pool.push(lb);
}

SE_Bounds* SE_LineBufferPool::NewBounds(int size)
{
    if (!m_bnd_pool.empty())
    {
        SE_Bounds* bounds = m_bnd_pool.pop();
        if (bounds->capacity >= size)
        {
            bounds->size = 0;
            bounds->min[0] = bounds->min[1] = DBL_MAX;
            bounds->max[0] = bounds->max[1] = -DBL_MAX;
            return bounds;
        }
        free(bounds);
    }

    SE_Bounds* bounds = (SE_Bounds*)malloc(sizeof(SE_Bounds) + 2*size*sizeof(double));
    bounds->hull = (double*)((char*)(bounds) + sizeof(SE_Bounds));
    bounds->capacity = size;
    bounds->size = 0;
    bounds->min[0] = bounds->min[1] = DBL_MAX;
    bounds->max[0] = bounds->max[1] = -DBL_MAX;
    bounds->pool = this;
    return bounds;
}

void SE_LineBufferPool::FreeBounds(SE_Bounds* bounds)
{
    m_bnd_pool.push(bounds);
}
