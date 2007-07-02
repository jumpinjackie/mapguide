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
#include "SE_LineStorage.h"
#include "SE_BufferPool.h"


SE_LineStorage::SE_LineStorage(int size, SE_BufferPool* pool) :
    LineBuffer(size),
    m_do_chop(false),
    m_chopped(false),
    m_crossed(false),
    m_pool(pool)
{
}


void SE_LineStorage::_MoveTo(double x, double y)
{
    bool chopStart = x < m_chop_start;
    if (m_do_chop && (chopStart || x > m_chop_end))
    {
        m_chop_x = x;
        m_chop_y = y;
        m_cross_x = chopStart? m_chop_start : m_chop_end;
        m_chopped = true;
        return;
    }
    else
        m_chopped = false;

    _MoveToNoChop(x, y);
}


void SE_LineStorage::_LineTo(double x, double y)
{
    if (m_do_chop)
    {
        bool chopStart = x < m_chop_start;
        if (chopStart || x > m_chop_end)
        {
            if (m_chopped)
            {
                /* Handle the cases where we jump across both clipping lines */
                if ((m_chop_x < m_chop_start && !chopStart) ||
                    (m_chop_x > m_chop_end && chopStart))
                {
                    double cy0 = m_chop_y + (y - m_chop_y)*(m_chop_start - m_chop_x)/(x - m_chop_x);
                    double cy1 = m_chop_y + (y - m_chop_y)*(m_chop_end - m_chop_x)/(x - m_chop_x);

                    if (chopStart)
                    {
                        _LineTo(m_chop_end, cy1);
                        _LineToNoChop(m_chop_start, cy0);
                    }
                    else
                    {
                        _LineTo(m_chop_start, cy0);
                        _LineToNoChop(m_chop_end, cy1);
                    }
                }
                else
                {
                    m_chop_x = x;
                    m_chop_y = y;
                    return;
                }
            }

            m_chop_x = x;
            m_chop_y = y;

            double lastx, lasty, lastz;
            last_point(lastx, lasty, lastz);
            m_cross_x = chopStart? m_chop_start : m_chop_end;
            m_cross_y = lasty + (y - lasty)*(m_cross_x - lastx)/(x - lastx);
            m_chopped = m_crossed = true;
            x = m_cross_x;
            y = m_cross_y;
        }
        else if (m_chopped)
        {
            double sy = m_chop_y + (y - m_chop_y)*(m_cross_x - m_chop_x)/(x - m_chop_x);
            if (!m_close_chops || !m_crossed)
            {
                if (m_crossed)
                    EnsureContours(1);
                _MoveTo(m_cross_x, sy);
            }
            else
            {
                _LineToNoChop(m_cross_x, sy);
            }
            m_chopped = false;
            if (x == m_cross_x)
                return;
            EnsurePoints(1);
        }
    }

    _LineToNoChop(x, y);
}


void SE_LineStorage::SetToTransform(const SE_Matrix& xform, LineBuffer* lb)
{
    // call base class
    LineBuffer::operator=(*lb);
    Transform(xform);
}


void SE_LineStorage::SetToCopy(SE_LineStorage* src)
{
    *this = *src;
}


void SE_LineStorage::Transform(const SE_Matrix& xform)
{
    for (int ix = 0; ix < point_count(); ix++)
        xform.transform(x_coord(ix), y_coord(ix));

    xform.transform(m_contour_start_x, m_contour_start_y);
}


SE_LineStorage& SE_LineStorage::operator=(const SE_LineStorage& src)
{
    // call base class
    LineBuffer::operator=(src);
    m_do_chop = src.m_do_chop;
    m_chopped = src.m_chopped;
    m_close_chops = src.m_close_chops;
    m_crossed = src.m_crossed;
    m_chop_start = src.m_chop_start;
    m_chop_end = src.m_chop_end;
    m_cross_x = src.m_cross_x;
    m_cross_y = src.m_cross_y;
    m_chop_x = src.m_chop_x;
    m_chop_y = src.m_chop_y;
    //m_pool does not get assigned
    return *this;
}


void SE_LineStorage::Append(SE_LineStorage* srcls)
{
    operator+=(*srcls);
}


void SE_LineStorage::Free()
{
    m_pool->FreeLineStorage(this);
}
