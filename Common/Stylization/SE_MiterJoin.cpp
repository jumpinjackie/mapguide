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

#include "SE_MiterJoin.h"
#include "stdafx.h"

/* TODO: use vectors instead of sAng, eAng */
SE_MiterJoin::SE_MiterJoin
                         ( double limit, 
                           RS_Bounds& bounds, 
                           double vertexOffset, 
                           double sAng, 
                           double eAng, 
                           double dx, 
                           double dy, 
                           double chopOffset )
{
    double V;    /* The location of the vertex in symbol coordinates */
    double wn;   /* The height of the symbol below the line (inside the join) */
    double wp;   /* The height of the symbol above the line (outside the join) */
    double jn;   /* The length of the join above the line */
    double jp;   /* The length of the join below the line */
    double h;    /* The half-angle between the segments */
    double t;    /* The tangent of the half-angle */
    double c;    /* The cosine of the half-angle */
    double s;    /* The sine of the half-angle */
    double mmax; /* The length of the miter before the miter limit is applied */
    double mlen; /* The miter length (the miter limit times the outside line thickness) */
    double movr; /* The length of the portion of the miter that exceeds the miter length */

    m_sAng = sAng;
    m_eAng = eAng;

    wn = bounds.miny < 0.0 ? -bounds.miny : 0.0;
    wp = bounds.maxy > 0.0 ? bounds.maxy : 0.0;
    
    /* If t is negative, then jn will become the miter length and jp will become the inside length */
    h = (eAng - sAng)/2.0;
    s = sin(h);
    c = cos(h);
    t = s/c; /* The calculations will be stable even if the tangent is INF or 0 (assuming a reasonable miter limit) */
    V = bounds.minx - vertexOffset;
    
    /* Rotation should occur about the vertex */
    m_sRot.translateX(V);
    m_sRot.rotate(sAng);
    m_sRot.translate(dx - V, dy);
    m_eRot.translateX(V);
    m_eRot.rotate(eAng);
    m_eRot.translate(dx - V, dy);

    /* the tangent of the half angle is equal to the upper height over the upper join width */
    jn = wn/t;

    /* the tangent of the half angle is equal to the lower height over the lower join width */
    jp = wp/t;
 
    if (t < 0.0)
    { /* The positive portion of the symbol is inside the join */
        mmax = sqrt(wn*wn + jn*jn);
        mlen = wn*limit;
        if ((movr = mmax - mlen) > 0) /* the miter limit forces a bevel */
        {
            /* the cosine of the half angle is equal to the chopped length / the truncation of j */
            jn -= movr/c;
            jn = (jn < 0) ? 0.0 : jn;
        }
        m_y_max_len = jn;
        m_y_max = wn;
        m_y_min_len = jp;
        m_y_min = wp;
    }
    else
    { /* The negative portion of the symbol is inside the join */
        mmax = sqrt(wp*wp + jp*jp);
        mlen = wp*limit;
        if ((movr = mmax - mlen) > 0)
        {
            jp -= movr/c;
            jp = (jp < 0) ? 0.0 : jp;
        }
        m_y_max_len = jp;
        m_y_max = wp;
        m_y_min_len = jn;
        m_y_min = wn;
    }
    
    if (V + chopOffset < bounds.maxx)
        m_chop = V + chopOffset;
    else
        m_chop = DBL_MAX;

    /* The widest part of the transform is cosine of the half angle times the actual miter length.
     * If there is no bevel, m_y_mid will be equal to m_y_max.
     * Despite this mathemtical certainty, using an if ensures that m_y_mid == m_y_max in fp math */
    if (movr > 0)
    {
        m_y_mid_len = (mmax < mlen ? mmax : mlen)*c;
        m_y_mid = m_y_mid_len*t;
    }
    else
    {
        m_y_mid_len = m_y_max_len;
        m_y_mid = m_y_max;
    }
    m_x_vert = V;

    m_n_discontinuities = -1;
    m_n_identity_regions = -1;
    m_bounds = bounds;

    /* Factor used in miter calculations (see comments in Transform) */
    m_miter_scale = m_y_min_len*(m_y_mid_len + m_y_min_len)/(m_y_mid - m_y_min);
    m_bevel_scale = (m_y_max - m_y_mid)*(m_y_mid_len - m_y_max_len);

}

RS_F_Point* SE_MiterJoin::GetDiscontinuities(int &length)
{
    if (m_n_discontinuities == -1) /* Lazily initialize */
    {
        m_first_discontinuity = NULL;
        double minx = m_x_vert - m_y_min_len;
        double maxx = m_x_vert + m_y_min_len;
        if (m_bounds.minx < m_x_vert && m_y_mid != m_y_max)
        {
            m_first_discontinuity = m_discontinuities;
            m_n_discontinuities++;
            m_discontinuities[0].x = (minx > m_bounds.minx) ? minx : m_bounds.minx;
            m_discontinuities[0].y = m_y_mid;
            m_discontinuities[1].x = (m_bounds.maxx < m_x_vert) ? m_bounds.maxx : m_x_vert;
            m_discontinuities[1].y = m_y_mid;
        }

        if (m_bounds.minx < m_x_vert && m_bounds.maxx > m_x_vert)
        {
            if (m_first_discontinuity == NULL)
                m_first_discontinuity = m_discontinuities + 2;
            m_n_discontinuities++;
            m_discontinuities[2].x = m_x_vert;
            m_discontinuities[2].y = m_y_max;
            m_discontinuities[3].x = m_x_vert;
            m_discontinuities[3].y = m_y_min;
        }

        if (m_bounds.maxx > m_x_vert && m_y_mid != m_y_max)
        {
            if (m_first_discontinuity == NULL)
                m_first_discontinuity = m_discontinuities + 4;
            m_n_discontinuities++;
            m_discontinuities[4].x = (maxx < m_bounds.maxx) ? maxx : m_bounds.maxx;
            m_discontinuities[4].y = m_y_mid;
            m_discontinuities[5].x = (m_bounds.minx > m_x_vert) ? m_bounds.minx : m_x_vert;
            m_discontinuities[5].y = m_y_mid;
        }
    }

    length = m_n_discontinuities;
    return m_first_discontinuity;
}

RS_Bounds* SE_MiterJoin::GetIdentityRegions(int &length)
{
    if (m_n_identity_regions == -1) /* Lazily initialize */
    {
        m_first_identity_region = NULL;
        m_n_identity_regions = 0;

        if (m_bounds.minx < m_x_vert - m_y_min_len)
        {
            m_first_identity_region = m_identity_region;
            m_n_identity_regions++;
            m_identity_region[0].minx = m_bounds.minx;
            m_identity_region[0].maxx = m_x_vert - m_y_min_len;
            m_identity_region[0].miny = m_bounds.miny;
            m_identity_region[0].maxy = m_bounds.maxy;
        }

        if (m_bounds.maxx > m_x_vert + m_y_min_len && m_chop > m_x_vert + m_y_min_len)
        {
            if (m_first_identity_region == NULL)
                m_first_identity_region = m_identity_region + 1;
            m_n_identity_regions++;
            m_identity_region[1].minx = m_x_vert + m_y_min_len;
            m_identity_region[1].maxx = m_bounds.maxx > m_chop ? m_chop : m_bounds.maxx;
            m_identity_region[1].miny = m_bounds.miny;
            m_identity_region[1].maxy = m_bounds.maxy;
        }
    }

    length = m_n_identity_regions;
    return m_first_identity_region;
}

RS_Bounds* SE_MiterJoin::GetNonlinearRegions(int &length)
{
    length = 0;
    return NULL;
}

RS_Bounds& SE_MiterJoin::GetTransformedBounds()
{
    /* TODO: initialize, lazily */
    return m_xf_bounds;
}

double SE_MiterJoin::GetXChop(bool &chopEnd)
{
    chopEnd = true;
    return m_chop;
}

void SE_MiterJoin::Transform(double& x, double &y, SE_TransformInfo* info)
{
    /* By using these, max will always be > min */
    bool start = x < m_x_vert;
    double x_min = start ? m_x_vert - m_y_min_len : m_x_vert + m_y_min_len;
    double dXdx, dXdy, d2Xdxdy;

    if (x >= m_x_vert - m_y_min_len && x <= m_x_vert + m_y_min_len)
    {
        if (y > m_y_mid && y <= m_y_max)
        {
        /* The bevel transform can be described by the following function:
         * X(x,y) = x_min + (x - x_min) * (y_max_len + (y_max - y) * (y_mid_len - y_max_len) / (y_max - y_mid)
         *                                                          |------------- m_bevel_scale -------------|
         * where:
         * dX/dx = y_max_len + (y_max - y) * scale
         * dX/dy = - (x - x_min) * scale
         * d2X/dx2 = 0
         * d2X/dy2 = 0
         * d2X/dxdy = -scale
         * 
         * The transform is constant over Y, so
         * Y(x,y) = y
         */
            double dy = m_y_max - y;
            double dx = x - x_min;

            x = x_min + dx * (m_y_max_len + dy * m_bevel_scale);
            dXdx = m_y_max_len + dy * m_bevel_scale;
            dXdy = dx * m_bevel_scale;
            d2Xdxdy = -m_bevel_scale;
        }
        else if (y <= m_y_mid && y >= m_y_min)
        {

        /* The miter transform can be described by the following function:
         * X(x,y) = x_min + (x - x_min) * (y - y_min) * y_min_len * (y_mid_len + y_min_len) / (y_mid - y_min)
         *                                             |------------------- m_miter_scale -------------------|
         * where:
         * dX/dx = (y - y_min) * scale
         * dX/dy = (x - x_min) * scale
         * d2X/dx2 = 0
         * d2X/dy2 = 0
         * d2X/dxdy = scale
         * 
         * The transform is constant over Y, so
         * Y(x,y) = y
         */
            double dx = x - x_min;
            double dy = y - m_y_min;

            x = x_min + dx * dy * m_miter_scale;
            dXdx = dy * m_miter_scale;
            dXdy = dx * m_miter_scale;
            d2Xdxdy = m_miter_scale;
        }
        else
        {
            dXdx = 1.0;
            dXdy = 0.0;
            d2Xdxdy = 0.0;
        }
    }
    else
    {
        dXdx = 1.0;
        dXdy = 0.0;
        d2Xdxdy = 0.0;
    }

    if (info)
    {
        info->dXdx = dXdx;
        info->dXdy = dXdy;
        info->dYdx = 0.0;
        info->dYdy = 1.0;
        info->d2Xdx2 = 0.0;
        info->d2Ydx2 = 0.0;
        info->d2Xdy2 = 0.0;
        info->d2Ydy2 = 0.0;
        info->d2Xdxdy = d2Xdxdy;
        info->d2Ydxdy = 0.0;
    }

    if (start)
        m_sRot.transform(x,y);
    else
        m_eRot.transform(x, y);
}