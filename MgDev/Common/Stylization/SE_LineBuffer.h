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

#ifndef SE_LINEBUFFER_H_
#define SE_LINEBUFFER_H_

#include "LineBuffer.h"
#include "SE_Matrix.h"
#include <set>

struct SE_Bounds;
class SE_BufferPool;


// Ellipse definition
struct SE_EllipseDef
{
    double cx;    // X center
    double cy;    // Y center
    double rx;    // major radius
    double ry;    // minor radius
    double sAng;  // start angle
    double eAng;  // start angle
    double rot;   // rotation
    SE_Matrix xf; // scale matrix
};


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
    STYLIZATION_API void Transform(const SE_Matrix& xform, double tolerance);

    STYLIZATION_API SE_INLINE bool& compute_bounds()
    {
        return m_compute_bounds;
    }

    // the line buffer representing the transformed/tesselated path
    STYLIZATION_API SE_INLINE LineBuffer* xf_buffer()
    {
        return m_xf_buf;
    }

    STYLIZATION_API SE_INLINE SE_Bounds* xf_bounds()
    {
        return m_xf_bounds;
    }

    STYLIZATION_API SE_LineBuffer* Clone(bool keepPool = true);

    // Retrieve the elliptical arc parameters
    //   index - the index of the arc in the symbol, zero-based
    //
    // E.g. for a buffer with:
    //     M L L M A0 M A1
    //     M=MoveTo, L=LineTo, A=ArcTo
    // GetEllipseDefinition would return arc segment A0 for index=0,
    // and arc segment A1 for index=1.
    //
    // Caller has to check for NULL and delete the returned object.
    STYLIZATION_API SE_EllipseDef* GetEllipseDefinition(int index);

private:
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

    SE_Bounds* m_xf_bounds;
    LineBuffer* m_xf_buf;
};

#endif
