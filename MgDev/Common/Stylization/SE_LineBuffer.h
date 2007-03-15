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
class SE_LineStorage;

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

    STYLIZATION_API void MoveTo(double x, double y);
    STYLIZATION_API void LineTo(double x, double y);
    STYLIZATION_API void EllipticalArcTo(double cx, double cy, double rx, double ry, double sAng, double eAng, double rotation);
    STYLIZATION_API void Close();
    STYLIZATION_API bool Empty();
    STYLIZATION_API void Free();

    STYLIZATION_API LineBuffer* Transform(const SE_Matrix& xform, double weight = 0.0, double tolerance = .25);
    STYLIZATION_API LineBuffer* TransformInstance(const SE_Matrix& xform);

    STYLIZATION_API SE_INLINE bool& compute_bounds() { return m_compute_bounds; }
    STYLIZATION_API SE_INLINE LineBuffer* xf_buffer() { return (LineBuffer*)m_xf_buf; }
    STYLIZATION_API SE_INLINE LineBuffer* inst_buffer() { return (LineBuffer*)m_inst_buf; }
    STYLIZATION_API SE_INLINE SE_Bounds* xf_bounds() { return m_xf_bounds; }
    STYLIZATION_API SE_INLINE SE_Bounds* inst_bounds() { return m_inst_bounds; }

    STYLIZATION_API SE_LineBuffer* Clone();

private:
    void Reset();
    void ResizeBuffer(void** buffer, int unitsize, int mininc, int cur_pts, int& max_pts);
    void TessellateCubicTo(SE_LineStorage* pts, double px2, double py2, double px3, double py3, double px4, double py4, int steps);
    SE_Bounds* ComputeConvexHull(double* pnts, int* cntrs, int ncntrs, double weight);

    SE_LineBufferPool* m_pool;

    double* m_pts;
    SE_LB_SegType* m_segs;
    int m_npts;
    int m_nsegs;
    int m_max_pts;
    int m_max_segs;

    double m_start[2];
    double m_last[2];

    bool m_compute_bounds;

    SE_Matrix m_xf;
    double m_xf_tol;
    double m_xf_weight;
    SE_Bounds* m_xf_bounds;
    SE_LineStorage* m_xf_buf;

    SE_Bounds* m_inst_bounds;
    SE_LineStorage* m_inst_buf;

    /* TODO: write a stack based allocator for this, or replace it */
    std::set<std::pair<double, double>, PointLess> m_ch_ptbuf;
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
