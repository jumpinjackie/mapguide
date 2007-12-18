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

#ifndef SE_LINEBUFFER_H_
#define SE_LINEBUFFER_H_

#include "LineBuffer.h"
#include "SE_Matrix.h"
#include "SE_RendererStyles.h"
#include <set>

struct SE_Bounds;
class SE_BufferPool;
struct SE_RenderPolyline;
struct SE_RenderLineStyle;


struct PointLess : std::binary_function<std::pair<double, double>&, std::pair<double, double>&, bool>
{
public:
    bool operator( ) (const std::pair<double, double>& a, const std::pair<double, double>& b) const
    {
        return (a.first < b.first) || (a.first == b.first && a.second < b.second);
    }
};

typedef std::vector<std::pair<double, double> > PointList;

//---------------------------------------------

class SE_LineBuffer
{
friend class SE_BufferPool;
private:
    SE_LineBuffer(int size, SE_BufferPool* pool);
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
    STYLIZATION_API void EllipticalArcTo(double cx, double cy, double rx, double ry, double sAngRad, double eAngRad, double rotRad);
    STYLIZATION_API void SetGeometry(LineBuffer* srclb);
    STYLIZATION_API void Close();
    STYLIZATION_API bool Empty();
    STYLIZATION_API void Free();
    STYLIZATION_API void Reset();

    // caller doesn't free
    STYLIZATION_API LineBuffer* Transform(const SE_Matrix& xform, double tolerance, SE_RenderPolyline* rp = NULL);

    STYLIZATION_API SE_INLINE bool& compute_bounds()
    {
        return m_compute_bounds;
    }

    // the line buffer representing the transformed/tesselated path
    STYLIZATION_API SE_INLINE LineBuffer* area_buffer()
    {
        return m_area_buf;
    }

    // the line buffer representing the transformed/tesselated path, with line style applied
    // (i.e. the buffer will contain a polygon for a polyline path in some cases)
    STYLIZATION_API SE_INLINE LineBuffer* outline_buffer()
    {
        return m_outline_buf;
    }

    STYLIZATION_API SE_INLINE SE_Bounds* xf_bounds()
    {
        return m_xf_bounds;
    }

    STYLIZATION_API SE_LineBuffer* Clone(bool keepPool = true);

private:
    SE_Bounds* ComputeConvexHull(LineBuffer* plb);
    SE_Bounds* GetSEBounds(RS_Bounds& bounds);
    void PopulateXFBuffer();

    SE_BufferPool* m_pool;

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

    SE_LineStroke m_xf_lineStroke;

    SE_RenderLineStyle* m_xf_style;
    SE_Bounds* m_xf_bounds;
    LineBuffer* m_area_buf;
    LineBuffer* m_outline_buf;

    // TODO: write a stack based allocator for this, or replace it
    PointList m_ch_ptbuf;
};

#endif
