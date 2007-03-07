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

#ifndef SE_LINEBUFFER_H
#define SE_LINEBUFFER_H

#include "LineBuffer.h"
#include "SE_Matrix.h"
#include <set>

struct SE_Bounds;
class SE_LineBufferPool;

struct PointLess : std::binary_function<std::pair<double, double>&, std::pair<double, double>&, bool>
{
public:
    bool operator( ) (const std::pair<double, double>& a, const std::pair<double, double>& b) const
    {
        return (a.first < b.first) || (a.first == b.first && a.second < b.second);
    }
};

//---------------------------------------------
//---------------------------------------------

struct SE_Geometry
{
    SE_INLINE SE_Geometry() :
        points(NULL),
        n_pts(0),
        contours(NULL),
        n_cntrs(0),
        geom_type(0)
    {
    }

    SE_INLINE SE_Geometry(LineBuffer* srclb)
    {
        points = srclb->points();
        n_pts = srclb->point_count();
        contours = srclb->cntrs();
        n_cntrs = srclb->cntr_count();
        geom_type = srclb->geom_type();
    }

    double* points;
    int n_pts;
    int* contours;
    int n_cntrs;
    int geom_type;
};

//---------------------------------------------
//---------------------------------------------

class SE_LineBuffer
{
friend class SE_LineBufferPool;
private:
    SE_LineBuffer(int size);
    ~SE_LineBuffer();

public:
    enum SE_LB_SegType
    {
        SegType_MoveTo,
        SegType_LineTo,
        SegType_EllipticalArc
    };

    STYLIZATION_API void SetToTransform(LineBuffer* lb, const SE_Matrix& xform);

    STYLIZATION_API void MoveTo(double x, double y);
    STYLIZATION_API void LineTo(double x, double y);
    STYLIZATION_API void EllipticalArcTo(double cx, double cy, double rx, double ry, double sAng, double eAng, double rotation);
    STYLIZATION_API void Close();
    STYLIZATION_API void Transform(const SE_Matrix& xform, double weight = 0.0, double tolerance = .25);
    STYLIZATION_API void InstanceTransform(const SE_Matrix& xform, SE_Geometry& geom, SE_Bounds** bounds);
    STYLIZATION_API bool Empty();

    STYLIZATION_API SE_Bounds* xf_bounds();
    STYLIZATION_API void xf_longest_edge(double& x0, double& y0, double& x1, double& y1);
    STYLIZATION_API void Centroid(LineBuffer::GeomOperationType type, double* x, double * y, double * slope);
    STYLIZATION_API int xf_geom_type();
    SE_INLINE double* xf_points() { return m_xf_pts; }
    SE_INLINE int xf_point_cnt() { return m_xf_npts; }
    SE_INLINE int* xf_cntrs() { return m_xf_cntrs; }
    SE_INLINE int xf_cntr_cnt() { return m_xf_ncntrs; }

private:
    void Reset();
    void ResizeBuffer(void** buffer, int unitsize, int mininc, int cur_pts, int& max_pts);
    void TessellateCubicTo(double* pts, double px2, double py2, double px3, double py3, double px4, double py4, int steps);
    SE_Bounds* ComputeConvexHull(double* pnts, int* cntrs, int ncntrs, double weight);

    SE_LineBufferPool* m_pool;
    LineBuffer* m_src_lb;

    double* m_pts;
    double* m_xf_pts;
    double* m_inst_pts;
    int* m_xf_cntrs;
    int* m_inst_cntrs;
    SE_LB_SegType* m_segs;

    /* TODO: maintaining this buffer for every instance is wasteful */
    std::set<std::pair<double, double>, PointLess> m_ch_ptbuf;

    SE_Matrix m_xf;
    SE_Bounds* m_xf_bounds;
    SE_Bounds* m_inst_bounds;
    double m_xf_tol;
    double m_xf_weight;

    double m_start[2];
    double m_last[2];

    int m_npts;
    int m_xf_npts;
    int m_inst_npts;
    int m_xf_ncntrs;
    int m_inst_ncntrs;
    int m_nsegs;

    int m_max_xf_cntrs;
    int m_max_inst_cntrs;
    int m_max_pts;
    int m_max_xf_pts;
    int m_max_inst_pts;
    int m_max_segs;
};

//---------------------------------------------
// Object pool for line buffers
//---------------------------------------------

class SE_LineBufferPool
{
public:
    STYLIZATION_API virtual ~SE_LineBufferPool();
    STYLIZATION_API SE_LineBuffer* NewLineBuffer(int requestSize);
    STYLIZATION_API SE_Bounds* NewBounds(int size);
    STYLIZATION_API void FreeBounds(SE_Bounds* bounds);
    STYLIZATION_API void FreeLineBuffer(SE_LineBuffer*);

private:
    DataValueStack<SE_LineBuffer> m_lb_pool;
    DataValueStack<SE_Bounds> m_bnd_pool;
};

#endif // SE_LINEBUFFER_H
