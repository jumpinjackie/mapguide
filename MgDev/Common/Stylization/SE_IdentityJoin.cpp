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
#include "SE_IdentityJoin.h"
#include "SE_LineStorage.h"


SE_IdentityJoin::SE_IdentityJoin(RS_Bounds& bounds,
                                 double offset,
                                 const RS_F_Point& pt0,
                                 const RS_F_Point& pt1,
                                 double prechop,
                                 double postchop) :
    m_xf_bounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX),
    m_xform()
{
    RS_F_Point dp(pt1.x - pt0.x, pt1.y - pt0.y);
    double len = sqrt(dp.x*dp.x + dp.y*dp.y);
    double invlen = 1.0/len;
    dp.x *= invlen;
    dp.y *= invlen;

    /* Rotate around leading edge */
    m_xform.translateX(-bounds.minx);
    m_xform.rotate(dp.y, dp.x);
    dp.x *= offset;
    dp.y *= offset;
    /* Position left edge offset pixels along the line from pt0 */
    m_xform.translate(pt0.x + dp.x, pt0.y + dp.y);

    m_bounds = bounds;
    m_chop_start = bounds.minx + prechop - offset;
    m_chop_end = bounds.minx + postchop - offset;
}


RS_F_Point* SE_IdentityJoin::GetDiscontinuities(int &length)
{
    length = 0;
    return NULL;
}


RS_Bounds& SE_IdentityJoin::GetTransformedBounds()
{
    if (!m_xf_bounds.IsValid() && m_bounds.IsValid())
    {
        RS_F_Point xfpt;

        xfpt.x = m_bounds.minx;
        xfpt.y = m_bounds.miny;
        m_xform.transform(xfpt.x, xfpt.y);
        m_xf_bounds.add_point(xfpt);

        xfpt.x = m_bounds.minx;
        xfpt.y = m_bounds.maxy;
        m_xform.transform(xfpt.x, xfpt.y);
        m_xf_bounds.add_point(xfpt);

        xfpt.x = m_bounds.maxx;
        xfpt.y = m_bounds.miny;
        m_xform.transform(xfpt.x, xfpt.y);
        m_xf_bounds.add_point(xfpt);

        xfpt.x = m_bounds.maxx;
        xfpt.y = m_bounds.maxy;
        m_xform.transform(xfpt.x, xfpt.y);
        m_xf_bounds.add_point(xfpt);
    }

    return m_xf_bounds;
}


void SE_IdentityJoin::GetXChop(double& startx, double& endx)
{
    startx = m_chop_start;
    endx = m_chop_end;
}


void SE_IdentityJoin::GetXRadius(double& pre, double& post)
{
    pre = post = 0.0;
}


void SE_IdentityJoin::ApplyPreTransform(SE_Matrix& prexf)
{
    m_xform.postmultiply(prexf);
}


void SE_IdentityJoin::Transform(SE_LineStorage* source, SE_LineStorage* dest, int contour, int ncntrs, bool closed)
{
    const RS_Bounds& bounds = source->bounds();

    if (bounds.minx < m_chop_start || bounds.maxx > m_chop_end)
    { /* Chopping may be required */
        double saved_chop_start, saved_chop_end;
        bool saved_chop_closed;
        dest->GetChopInfo(saved_chop_start, saved_chop_end, saved_chop_closed);
        dest->SetChopInfo(m_chop_start, m_chop_end, closed);

        int orig_point_cnt = dest->point_count();

        for (int i = contour; i < ncntrs; i++)
        {
            double x, y;
            int src = source->contour_start_point(i);
            int last = source->contour_end_point(i);
            source->get_point(src++, x, y);
            dest->EnsureContours(1);
            dest->EnsurePoints(1);
            dest->_MoveTo(x, y);

            while (src <= last)
            {
                source->get_point(src++, x, y);
                dest->EnsurePoints(1);
                dest->_LineTo(x, y);
            }
        }
        dest->SetChopInfo(saved_chop_start, saved_chop_end, saved_chop_closed);

        // transform newly added points
        int n = orig_point_cnt;

        while (n < dest->point_count())
        {
            m_xform.transform(dest->x_coord(n), dest->y_coord(n));
            n++;
        }
    }
    else
    {

        for (int i = contour; i < ncntrs; i++)
        {
            double x, y;
            int src = source->contour_start_point(i);
            int last = source->contour_end_point(i);
            source->get_point(src++, x, y);
            m_xform.transform(x, y);
            dest->EnsureContours(1);
            dest->EnsurePoints(1);
            dest->_MoveTo(x, y);

            while (src <= last)
            {
                source->get_point(src++, x, y);
                m_xform.transform(x, y);
                dest->EnsurePoints(1);
                dest->_LineTo(x, y);
            }
        }
    }
}
