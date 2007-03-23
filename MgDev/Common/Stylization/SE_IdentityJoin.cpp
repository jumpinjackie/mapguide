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

#include "SE_IdentityJoin.h"
#include "SE_LineStorage.h"
#include <float.h>

SE_IdentityJoin::SE_IdentityJoin(RS_Bounds& bounds, double offset, const RS_F_Point& pt0, const RS_F_Point& pt1) :
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
    double underreach = len - bounds.width() - offset;
    if (offset < 0)
    {
        m_bounds.minx -= offset;
        m_chop_start = m_bounds.minx;
    }
    if (underreach < 0)
    {
        m_bounds.maxx += underreach;
        m_chop_end = m_bounds.maxx;
    }
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

RS_F_Point* SE_IdentityJoin::Transform(const RS_F_Point& pt0, const RS_F_Point& pt1, int& length)
{
    length = 2;
    m_xform.transform(pt0.x, pt0.y, m_points[0].x, m_points[0].y);
    m_xform.transform(pt1.x, pt1.y, m_points[1].x, m_points[1].y);
    return m_points;
}

void SE_IdentityJoin::Transform(SE_LineStorage* source, SE_LineStorage* dest, bool closed)
{

    const RS_Bounds& bounds = source->bounds();
    if (bounds.minx < m_chop_start || bounds.maxx > m_chop_end)
    {   /* Chopping may be required */
        dest->SetChopInfo(m_chop_start, m_chop_end, closed);
        
        int n_cntrs = source->cntr_count();
        int* contours = source->cntrs();
        double* src = source->points();

        for (int i = 0; i < n_cntrs; i++)
        {
            double x, y;
            double* last = src + 2*contours[i];
            x = *src++;
            y = *src++;
            dest->EnsureContours(1);
            dest->EnsurePoints(1);
            dest->_MoveTo(x, y);

            while (src < last)
            {
                x = *src++;
                y = *src++;
                dest->EnsurePoints(1);
                dest->_LineTo(x, y);
            }
        }
        dest->Transform(m_xform);
    }
    else
        dest->SetToTransform(m_xform, source);
}
