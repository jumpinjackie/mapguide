//
//  Copyright (C) 2007 Autodesk, Inc.
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

#include "SE_LineBuffer.h"
#include "SE_ConvexHull.h"
#include "SE_Bounds.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

#define GROWTH_FACTOR 1.5

#define ENSURE_POINT_BUFFER(points) \
    if ((m_npts + (points)) > m_max_pts) \
        ResizeBuffer((void**)&m_pts, 2*sizeof(double), (points), m_npts, m_max_pts);

#define ENSURE_XF_POINT_BUFFER(points) \
    if ((m_xf_npts + (points)) > m_max_xf_pts) \
        ResizeBuffer((void**)&m_xf_pts, 2*sizeof(double), (points), m_xf_npts, m_max_xf_pts);

#define ENSURE_INST_POINT_BUFFER(points) \
    if ((m_inst_npts + (points)) > m_max_inst_pts) \
        ResizeBuffer((void**)&m_inst_pts, 2*sizeof(double), (points), m_inst_npts, m_max_inst_pts);

#define ENSURE_XF_CNTR_BUFFER(points) \
    if ((m_xf_ncntrs + (points)) > m_max_xf_cntrs) \
        ResizeBuffer((void**)&m_xf_cntrs, sizeof(int), (points), m_xf_ncntrs, m_max_xf_cntrs);

#define ENSURE_INST_CNTR_BUFFER(points) \
    if ((m_inst_ncntrs + (points)) > m_max_inst_cntrs) \
        ResizeBuffer((void**)&m_inst_cntrs, sizeof(int), (points), m_inst_ncntrs, m_max_inst_cntrs);

#define ENSURE_SEG_BUFFER(points) \
    if ((m_nsegs + (points)) > m_max_segs) \
        ResizeBuffer((void**)&m_segs, sizeof(SE_LB_SegType), (points), m_nsegs, m_max_segs);

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

    return AndrewHull<std::set<std::pair<double,double>, PointLess>::iterator, PointUtil>
        (iter, --end, (int)m_ch_ptbuf.size(), m_pool);
}

SE_LineBuffer::SE_LineBuffer(int size) :
    m_npts(0),
    m_nsegs(0),
    m_xf_ncntrs(0),
    m_inst_ncntrs(0),
    m_xf_npts(0),
    m_inst_npts(0),
    m_max_xf_pts(size),
    m_max_inst_pts(size),
    m_max_xf_cntrs(size),
    m_max_inst_cntrs(size),
    m_max_pts(size),
    m_max_segs(size),
    m_xf_tol(-1.0),
    m_xf_weight(-1.0),
    m_src_lb(NULL),
    m_xf_bounds(NULL),
    m_inst_bounds(NULL)
{
    m_pts = new double[size*2];
    m_xf_pts = new double[size*2];
    m_inst_pts = new double[size*2];
    m_xf_cntrs = new int[size];
    m_inst_cntrs = new int[size];
    m_segs = new SE_LB_SegType[size];
}

SE_LineBuffer::~SE_LineBuffer()
{
    delete[] m_pts;
    delete[] m_xf_pts;
    delete[] m_inst_pts;
    delete[] m_xf_cntrs;
    delete[] m_inst_cntrs;
    delete[] m_segs;
}

void SE_LineBuffer::MoveTo(double x, double y)
{
    ENSURE_POINT_BUFFER(1);
    ENSURE_SEG_BUFFER(1);

    double* cur_pt = m_pts + 2*m_npts++;
    *cur_pt++ = x;
    *cur_pt = y;

    m_start[0] = x;
    m_start[1] = y;

    m_segs[m_nsegs++] = SegType_MoveTo;
}

void SE_LineBuffer::LineTo(double x, double y)
{
    ENSURE_POINT_BUFFER(1);
    ENSURE_SEG_BUFFER(1);

    double* cur_pt = m_pts + 2*m_npts++;
    *cur_pt++ = x;
    *cur_pt = y;

    m_segs[m_nsegs++] = SegType_LineTo;
}

void SE_LineBuffer::EllipticalArcTo(double cx, double cy, double rx, double ry, double sAng, double eAng, double rotation)
{
    ENSURE_POINT_BUFFER(7);
    ENSURE_SEG_BUFFER(1);

    double* cur_pt = m_pts + 2*m_npts;
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
    m_npts = m_xf_npts = m_xf_ncntrs = m_nsegs = m_inst_npts = m_inst_ncntrs = 0;
    if (m_xf_bounds)
    {
        m_pool->FreeBounds(m_xf_bounds);
        m_xf_bounds = NULL;
    }
    if (m_inst_bounds)
    {
        m_pool->FreeBounds(m_inst_bounds);
        m_inst_bounds = NULL;
    }
    m_src_lb = NULL;
    m_xf_tol = m_xf_weight = -1.0;
    m_xf.setIdentity();
}

void SE_LineBuffer::ResizeBuffer(void** buffer, int unitsize, int mininc, int cur_pts, int& max_pts)
{
    int max_newpts = (int)(max_pts*GROWTH_FACTOR) + 1;
    if (max_newpts - max_pts < mininc)
        max_newpts += mininc;
     
    void* newbuf = new char[unitsize*max_newpts];
    memcpy(newbuf, *buffer, cur_pts*unitsize);
    delete[] *buffer;
    *buffer = newbuf;
    max_pts = max_newpts;
}

void SE_LineBuffer::Transform(const SE_Matrix& xform, double weight, double tolerance)
{
    if (m_xf == xform && m_xf_tol == tolerance && m_xf_weight == weight)
        return;

    if (m_xf_bounds)
    {
        m_pool->FreeBounds(m_xf_bounds);
        m_xf_bounds = NULL;
    }

    if (m_src_lb)
    {
        SetToTransform(m_src_lb, xform);
        return;
    }

    m_xf = xform;
    m_xf_tol = tolerance;
    m_xf_weight = weight;

    SE_LB_SegType* endseg = m_segs + m_nsegs;
    SE_LB_SegType* curseg = m_segs;
    int src_idx = 0;
    int dst_idx = 0;
    while(curseg != endseg)
    {
        switch(*curseg++)
        {
        case SegType_MoveTo:
            ENSURE_XF_CNTR_BUFFER(1);
            m_xf_cntrs[m_xf_ncntrs++] = 0;
        case SegType_LineTo:
            ENSURE_XF_POINT_BUFFER(1);
            xform.transform(m_pts[src_idx], m_pts[src_idx+1], m_xf_pts[dst_idx], m_xf_pts[dst_idx+1]);
            src_idx += 2;
            dst_idx += 2;
            m_xf_cntrs[m_xf_ncntrs-1]++;
            m_xf_npts++;
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

                /* TODO: change the # of segments based on the tolerance?...up to 4 for now */
                int nsegs = (int)(4.0*(eAng - sAng)/(2*M_PI)) + 1; // eAng - sAng is (0, 2pi), so this is {1,2,3,4}.
                double span = (eAng - sAng)/(double)nsegs;
                
                double sec = 1.0/cos(span/2.0);
                double alpha = sin(span)*(sqrt(1.0 + 3.0/sec/sec) - 1)/3.0;
                double rcos = cos(rot), rsin = sin(rot);
                
                double ex, ey, sx, sy;
                double scos, ssin, ecos, esin;
                double sa, ea;

                ecos = cos(sAng);
                esin = sin(sAng);
                ex = rx*ecos;
                ey = ry*esin;
                ea = sAng;

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

                    double c1x = sx - alpha*rx*ssin;
                    double c1y = sy + alpha*ry*scos;
                    double c2x = ex + alpha*rx*esin;
                    double c2y = ey - alpha*ry*ecos;

                    /* Here, we fine the maximum 2nd derivative over the interval in question, and
                       calculate the number of segments, assuming a deviation of the segment length *
                       the second derivative must be within the tolerance */
                    double mx, my;
                    SineCosineMax(sa, ssin, scos, ea, esin, ecos, my, mx);
                    mx *= rx;
                    my *= ry;
                    int steps = (int)((span/tolerance)*sqrt(mx*mx + my*my)) + 1;
                    ENSURE_XF_POINT_BUFFER(steps + 1);
                    m_xf_cntrs[m_xf_ncntrs-1] += steps + 1;
                    m_xf_npts += steps + 1;

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

                    ex += cx;
                    ey += cy;
                    c1x += cx;
                    c1y += cy;
                    c2x += cx;
                    c2y += cy;

                    TessellateCubicTo(m_xf_pts + dst_idx, c1x, c1y, c2x, c2y, ex, ey, steps);
                    dst_idx += steps + 1;
                }

                break;
            }
        }
    }
}

void SE_LineBuffer::InstanceTransform(const SE_Matrix& xform, SE_Geometry& geom, SE_Bounds** bounds)
{
    m_inst_npts = 0;
    m_inst_ncntrs = 0;
    ENSURE_INST_POINT_BUFFER(m_xf_npts);
    //ENSURE_INST_CNTR_BUFFER(m_inst_ncntrs); For this transform, the same number of points
    double* src = m_xf_pts;
    double* dst = m_inst_pts;
    double* last = m_xf_pts + m_xf_npts*2;
    while (src < last)
    {
        double sx = *src++;
        double sy = *src++;
        double* ix = dst++;
        double* iy = dst++;
        xform.transform(sx, sy, *ix, *iy);
    }
    m_inst_npts = m_xf_npts;
    geom.points = m_inst_pts;
    geom.n_pts = m_inst_npts;
    geom.contours = m_xf_cntrs;
    geom.n_cntrs = m_xf_ncntrs;

    geom.geom_type = xf_geom_type();

    if (bounds)
    {
        SE_Bounds* srcbounds = xf_bounds();
        SE_Bounds* dstbounds = *bounds;
        dstbounds = m_pool->NewBounds(srcbounds->size);
        for(int i = 0; i < srcbounds->size; i++)
            xform.transform(srcbounds->hull[2*i], srcbounds->hull[2*i+1], dstbounds->hull[2*i], dstbounds->hull[2*i+1]);
        dstbounds->size = srcbounds->size;
        dstbounds->max[0] = srcbounds->max[0];
        dstbounds->max[1] = srcbounds->max[1];
        dstbounds->min[0] = srcbounds->min[0];
        dstbounds->min[1] = srcbounds->min[1];
    }
}

bool SE_LineBuffer::Empty()
{
    if (m_src_lb)
        return m_src_lb->point_count() == 0;
    else
        return m_npts == 0;
}

void SE_LineBuffer::TessellateCubicTo(double* pts, double px2, double py2, double px3, double py3, double px4, double py4, int steps)
{
    //get the start point
    double px1 = pts[-2];
    double py1 = pts[-1];

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

        m_xf.transform(fx, fy, pts[0], pts[1]);
        pts += 2;
    }

    *pts++ = px4;
    *pts++ = py4;
}

SE_Bounds* SE_LineBuffer::xf_bounds()
{
    if (m_xf_bounds == NULL)
        m_xf_bounds = ComputeConvexHull(m_xf_pts, m_xf_cntrs, m_xf_ncntrs, m_xf_weight);

    return m_xf_bounds;
}

void SE_LineBuffer::xf_longest_edge(double& x0, double& y0, double& x1, double& y1)
{
    int* cntr_end = m_xf_cntrs + m_xf_ncntrs;
    int* contours = m_xf_cntrs;
    double* points = m_xf_pts;

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

int SE_LineBuffer::xf_geom_type()
{
    if (m_src_lb)
        return m_src_lb->geom_type();
    return -1;
}

void SE_LineBuffer::Centroid(LineBuffer::GeomOperationType type, double* x, double * y, double * slope)
{
    if (m_src_lb)
    {
        double dx = 1.0;
        m_src_lb->Centroid(type, x, y, slope);
        m_xf.transform(*x, *y);
        m_xf.transformVector(*slope, dx);
        *slope /= dx;
    }
}

void SE_LineBuffer::SetToTransform(LineBuffer* lb, const SE_Matrix& xform)
{
    m_src_lb = lb;
    m_xf = xform;
    m_xf_tol = 0.0;
    m_xf_weight = 0.0;

    ENSURE_XF_POINT_BUFFER(lb->point_count());
    ENSURE_XF_CNTR_BUFFER(lb->cntr_count());

    m_xf_ncntrs = lb->cntr_count();
    memcpy(m_xf_cntrs, lb->cntrs(), sizeof(int)*m_xf_ncntrs);
    
    m_xf_npts = lb->point_count();
    double *srcpts = lb->points();

    for (int i = 0; i < 2*m_xf_npts; i += 2)
        xform.transform(srcpts[i], srcpts[i+1], m_xf_pts[i], m_xf_pts[i+1]);
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
        lb->Reset();
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