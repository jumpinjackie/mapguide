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

#include "SE_LineStorage.h"
#include "SE_BufferPool.h"
#include "stdafx.h"


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
        m_cross_x = chopStart ? m_chop_start : m_chop_end;
        m_chopped = true;
        return;
    }
    else
        m_chopped = false;

    m_types[m_cur_types++] = (unsigned char)stMoveTo;
    m_pts[m_cur_pts++] = x;
    m_pts[m_cur_pts++] = y;

    m_last_x = x;
    m_last_y = y;

    m_cntrs[++m_cur_cntr] = 1;
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

            double lastx, lasty;
            lastx = m_pts[m_cur_pts-2];
            lasty = m_pts[m_cur_pts-1];
            m_cross_x = chopStart ? m_chop_start : m_chop_end;
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
                m_types[m_cur_types++] = (unsigned char)stLineTo;
                m_pts[m_cur_pts++] = m_cross_x;
                m_pts[m_cur_pts++] = sy;
                m_cntrs[m_cur_cntr]++;
            }
            m_chopped = false;
            if (x == m_cross_x)
                return;
            EnsurePoints(1);
        }
    }

    m_types[m_cur_types++] = (unsigned char)stLineTo;
    m_pts[m_cur_pts++] = x;
    m_pts[m_cur_pts++] = y;

    m_cntrs[m_cur_cntr]++;
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
    m_do_chop = src->m_do_chop;
    m_chopped = src->m_chopped;
    m_close_chops = src->m_close_chops;
    m_crossed = src->m_crossed;
    m_chop_start = src->m_chop_start;
    m_chop_end = src->m_chop_end;
    m_cross_x = src->m_cross_x;
    m_cross_y = src->m_cross_y;
    m_chop_x = src->m_chop_x;
    m_chop_y = src->m_chop_y;
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


void SE_LineStorage::Transform(const SE_Matrix& xform)
{
    double* cur = m_pts;
    double* end = m_pts + m_cur_pts;
    double *x, *y;

    while (cur < end)
    {
        x = cur++;
        y = cur++;
        xform.transform(*x, *y);
    }
}


void SE_LineStorage::_ResizePoints(int n)
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


void SE_LineStorage::_ResizeContours(int n)
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


void SE_LineStorage::Append(SE_LineStorage* srcls)
{
    int n_pts = srcls->point_count();
    int n_cntrs = srcls->cntr_count();
    int* contours = srcls->cntrs();
    
    EnsurePoints(n_pts);
    EnsureContours(n_cntrs);

    double* src = srcls->points();

    for (int i = 0; i < n_cntrs; i++)
    {
        double sx, sy;
        int types = contours[i] - 1;
        int pts = 2*types;
        sx = *src++;
        sy = *src++;
        _MoveToNoChop(sx, sy);
        memcpy(m_pts + m_cur_pts, src, pts*sizeof(double));
        m_cur_pts += pts;
        m_cntrs[m_cur_cntr] += types;
        memset(m_types + m_cur_types, stLineTo, types);
        m_cur_types += types;
        src += pts;
    }
}


void SE_LineStorage::Free()
{
    m_pool->FreeLineStorage(this);
}
