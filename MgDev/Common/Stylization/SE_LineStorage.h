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

#ifndef SE_LINESTORAGE_H_
#define SE_LINESTORAGE_H_

#include "LineBuffer.h"
#include "SE_Matrix.h"
#include "SE_Bounds.h"

class SE_LineStorage : public LineBuffer
{
friend class SE_BufferPool;

private:
    SE_LineStorage(int size, SE_BufferPool* pool);

public:
    STYLIZATION_API SE_LineStorage& operator=(const SE_LineStorage& src);

    SE_INLINE void SetBounds(double minx, double miny, double maxx, double maxy);
    SE_INLINE void SetBounds(SE_Bounds* bounds);
    SE_INLINE void SetChopInfo(double startx, double endx, bool closeChops);
    SE_INLINE void GetChopInfo(double& startx, double& endx, bool& closeChops);
    SE_INLINE void Reset();

    SE_INLINE void _MoveTo(double x, double y);
    SE_INLINE void _LineTo(double x, double y);

    /* Both of these methods invalidate the bounds.  SetBounds must be called manually to restore them. */
    STYLIZATION_API void SetToTransform(const SE_Matrix& xform, LineBuffer* src);
    STYLIZATION_API void SetToCopy(SE_LineStorage* src);
    STYLIZATION_API void Transform(const SE_Matrix& xform);
    STYLIZATION_API void Append(SE_LineStorage* src);
    STYLIZATION_API void Free();

private:
    void _MoveToChop(double x, double y);
    void _LineToChop(double x, double y);

    bool m_do_chop;
    bool m_chopped;

    bool m_close_chops;
    bool m_crossed;
    double m_chop_start;
    double m_chop_end;
    double m_cross_x;
    double m_cross_y;
    double m_chop_x;
    double m_chop_y;

    SE_BufferPool* m_pool;
};


/* Inline Functions */


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


void SE_LineStorage::SetChopInfo(double startx, double endx, bool closeChops)
{
    m_chop_start = startx;
    m_chop_end = endx;
    m_close_chops = closeChops;
    m_chopped = false;
    m_crossed = false;
    m_do_chop = startx < endx; /* No chopping if info is invalid */
}


void SE_LineStorage::GetChopInfo(double& startx, double& endx, bool& closeChops)
{
    startx = m_chop_start;
    endx = m_chop_end;
    closeChops = m_close_chops;
}


void SE_LineStorage::Reset()
{
    m_do_chop = false;
    m_chopped = false;
    m_crossed = false;
    LineBuffer::Reset();
}


void SE_LineStorage::_MoveTo(double x, double y)
{
    if (m_do_chop)
        return _MoveToChop(x, y);

    double z = 0.0;
    append_segment(stMoveTo, x, y, z);
    cache_contour_start(x, y, z);
    increment_contour();
}


void SE_LineStorage::_LineTo(double x, double y)
{
    if (m_do_chop)
        return _LineToChop(x, y);

    double z = 0.0;
    append_segment(stLineTo, x, y, z);
    increment_contour_pts();
}

#endif
