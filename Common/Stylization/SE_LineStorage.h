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

#ifndef SE_LINESTORAGE_H
#define SE_LINESTORAGE_H

#include "LineBuffer.h"
#include "SE_Matrix.h"
#include "SE_Bounds.h"

class SE_LineStorage : public LineBuffer
{
public:
    SE_INLINE SE_LineStorage(int size);
    SE_INLINE void EnsurePoints(int n);
    SE_INLINE void EnsureContours(int n);
    SE_INLINE void SetBounds(double minx, double miny, double maxx, double maxy);
    SE_INLINE void SetBounds(SE_Bounds* bounds);
    SE_INLINE void SetChopInfo(double startx, double endx, bool closeChops);
    SE_INLINE void Reset();
    
    SE_INLINE void _MoveToNoChop(double x, double y);
    SE_INLINE void _LineToNoChop(double x, double y);
    
    void _MoveTo(double x, double y);
    void _LineTo(double x, double y);
    
    /* Both of these methods invalidate the bounds.  SetBounds must be called manually to restore them. */
    void SetToTransform(const SE_Matrix& xform, LineBuffer* src);
    void SetToCopy(SE_LineStorage* src);
    void Transform(const SE_Matrix& xform);

private:
    SE_INLINE double& _LastX() { return m_last_x; }
    SE_INLINE double& _LastY() { return m_last_y; }
    void _ResizePoints(int n);
    void _ResizeContours(int n);

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
};

/* Inline Functions */

SE_LineStorage::SE_LineStorage(int size) : 
    LineBuffer(size),
    m_do_chop(false),
    m_chopped(false),
    m_crossed(false)
{ 
}

void SE_LineStorage::EnsurePoints(int n)
{
    if (m_cur_pts + 2*n >= m_pts_len)
        _ResizePoints(n);
}

void SE_LineStorage::EnsureContours(int n)
{
    if (m_cur_cntr + 2 + n > m_cntrs_len)
        _ResizeContours(n);
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

void SE_LineStorage::SetChopInfo(double startx, double endx, bool closeChops)
{
    m_do_chop = true;
    m_chop_start = startx;
    m_chop_end = endx;
    m_close_chops = closeChops;
}

void SE_LineStorage::Reset()
{
    m_do_chop = false;
    m_chopped = false;
    m_crossed = false;
    LineBuffer::Reset();
}

void SE_LineStorage::_MoveToNoChop(double x, double y)
{
    m_types[m_cur_types++] = (unsigned char)stMoveTo;
    m_pts[m_cur_pts++] = x;
    m_pts[m_cur_pts++] = y;

    m_last_x = x;
    m_last_y = y;

    m_cntrs[++m_cur_cntr] = 1;
}

void SE_LineStorage::_LineToNoChop(double x, double y)
{
    m_types[m_cur_types++] = (unsigned char)stLineTo;
    m_pts[m_cur_pts++] = x;
    m_pts[m_cur_pts++] = y;

    m_cntrs[m_cur_cntr]++;
}

#endif // SE_LINESTORAGE_H
