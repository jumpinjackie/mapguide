//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
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

#include "SE_AreaPositioning.h"
#include <float.h>


SE_AreaPositioning::SE_AreaPositioning(LineBuffer* geom, SE_RenderAreaStyle* style, double w2sAngleRad)
{
    SE_Tuple min = SE_Tuple(geom->bounds().minx, geom->bounds().miny);
    SE_Tuple max = SE_Tuple(geom->bounds().maxx, geom->bounds().maxy);

    if (style->originControl == SE_OriginControl_Centroid)
    {
        double slope;
        geom->Centroid(LineBuffer::ctArea, &m_base_pt.x, &m_base_pt.y, &slope);
    }
    else if (style->originControl == SE_OriginControl_Local)
    {
        // use parametric coordinates where [(0,0), (1,1)] is the extent of the polygon bounding box
        m_base_pt = min + (max - min) * SE_Tuple(style->origin[0], style->origin[1]);
    }
    else
    {
        // Global
        m_base_pt.x = style->origin[0];
        m_base_pt.y = style->origin[1];
    }

    m_angle_rad = style->angleRad;
    if (style->angleControl == SE_AngleControl_FromGeometry)
    {
        // angle is the same direction as longest segment
        double maxlensq = -1.0;
        SE_Tuple dir;

        for (int i = 0; i < geom->cntr_count(); ++i)
        {
            int start = geom->contour_start_point(i);
            int end = geom->contour_end_point(i);
            SE_Tuple last = SE_Tuple(geom->x_coord(start), geom->y_coord(start));

            for (int j = start + 1; j <= end; ++j)
            {
                SE_Tuple current = SE_Tuple(geom->x_coord(j), geom->y_coord(j));
                SE_Tuple seg = current - last;
                double curlensq = seg.lengthSquared();

                if (curlensq > maxlensq)
                {
                    maxlensq = curlensq;
                    dir = seg;
                }

                last = current;
            }
        }

        m_angle_rad = atan2(dir.y, dir.x);
    }
    else
    {
        // FromAngle

        // here we need to account for the viewport rotation
        m_angle_rad = style->angleRad + w2sAngleRad;
    }

    double cosa = cos(m_angle_rad);
    double sina = sin(m_angle_rad);

    m_h_vec = SE_Tuple( cosa, sina) * style->repeat[0];
    m_v_vec = SE_Tuple(-sina, cosa) * style->repeat[1];

    SE_Tuple sym_bnd_min, sym_bnd_max;
    sym_bnd_min.x = rs_min(rs_min(style->bounds[0].x, style->bounds[1].x),
                           rs_min(style->bounds[2].x, style->bounds[3].x));
    sym_bnd_min.y = rs_min(rs_min(style->bounds[0].y, style->bounds[1].y),
                           rs_min(style->bounds[2].y, style->bounds[3].y));
    sym_bnd_max.x = rs_max(rs_max(style->bounds[0].x, style->bounds[1].x),
                           rs_max(style->bounds[2].x, style->bounds[3].x));
    sym_bnd_max.y = rs_max(rs_max(style->bounds[0].y, style->bounds[1].y),
                           rs_max(style->bounds[2].y, style->bounds[3].y));

    /* 1 x---------x 2
     *   |         |
     *   |         |
     *   |         |
     * 0 x---------x 3
     */

    SE_Matrix irot;
    irot.rotate(-sina, cosa);

    SE_Tuple rbase;
    irot.transform(m_base_pt.x, m_base_pt.y, rbase.x, rbase.y);

    SE_Tuple outline[4];
    irot.transform(min.x, min.y, outline[0].x, outline[0].y);
    irot.transform(min.x, max.y, outline[1].x, outline[1].y);
    irot.transform(max.x, max.y, outline[2].x, outline[2].y);
    irot.transform(max.x, min.y, outline[3].x, outline[3].y);

    int least_i = 0, most_i = 0;

    for (int i = 1; i < 4; ++i)
    {
        if (outline[i].x < outline[least_i].x)
            least_i = i;
        if (outline[i].x > outline[most_i].x)
            most_i = i;
    }

    m_h_neg_pos = (int)ceil((outline[least_i].x - sym_bnd_max.x - rbase.x) / style->repeat[0]);
    m_h_pts = (int)floor((outline[most_i].x - sym_bnd_min.x - rbase.x) / style->repeat[0]) - m_h_neg_pos + 1;

    if(m_h_pts > m_k_buf_size)
        m_v_min = new int[2*m_h_pts];
    else
        m_v_min = m_v_buf;

    m_v_max = m_v_min + m_h_pts;

    double pos = rbase.x + m_h_neg_pos * style->repeat[0];
    for (int i = 0; i < m_h_pts; ++i)
    {
        double xmin = pos + sym_bnd_min.x, xmax = pos + sym_bnd_max.x;
        double ymin = DBL_MAX, ymax = -DBL_MAX;
        for (int j = 0; j < 4; ++j)
        {
            const SE_Tuple& p0 = outline[j];
            const SE_Tuple& p1 = outline[(j + 1) & 3];

            /* TODO: We could sort the segments and only check the valid ones, but there
             * are only four segments.  I doubt this will be a performance issue. */
            if ((p0.x <= xmax && p1.x >= xmin) || (p0.x >= xmin && p1.x <= xmax))
            {
                /* xmin or xman could overrun the current line segment, but that would only
                 * have the effect of increasing coverage, so it is acceptable */
                double y0, y1;

                if (p1.x == p0.x)
                {
                    y0 = p0.y;
                    y1 = p1.y;
                }
                else
                {
                    double f = (p1.y - p0.y) / (p1.x - p0.x);
                    y0 = p0.y + (xmin - p0.x) * f;
                    y1 = p0.y + (xmax - p0.x) * f;
                }

                if (y1 < y0)
                    std::swap(y0, y1);

                if (y0 < ymin)
                    ymin = y0;
                if (y1 > ymax)
                    ymax = y1;
            }
        }

        _ASSERT(ymax >= ymin);

        /* TODO: This seems like a clumsy way to round toward zero... */
        double yminsym = ymin - sym_bnd_max.y - rbase.y;
        double ymaxsym = ymax - sym_bnd_min.y - rbase.y;
        m_v_min[i] = (int)ceil(fabs(yminsym) / style->repeat[1]);
        m_v_max[i] = (int)floor(fabs(ymaxsym) / style->repeat[1]);
        if (yminsym < 0.0)
            m_v_min[i] = -m_v_min[i];
        if (ymaxsym < 0.0)
            m_v_max[i] = -m_v_max[i];

        _ASSERT(m_v_max[i] - m_v_min[i] >= -1);

        pos += style->repeat[0];
    }

    m_h_cur_pos = m_h_neg_pos;
    m_v_cur_pos = m_v_min[0];
}


SE_AreaPositioning::~SE_AreaPositioning()
{
    if (m_v_buf != m_v_min)
        delete[] m_v_min;
}


const double& SE_AreaPositioning::PatternRotation()
{
    return m_angle_rad;
}


const SE_Tuple* SE_AreaPositioning::NextLocation()
{
    if (m_v_cur_pos > m_v_max[m_h_cur_pos - m_h_neg_pos])
    {
        do
        {
            if (m_h_cur_pos + 1 >= m_h_neg_pos + m_h_pts)
                return NULL;
            m_v_cur_pos = m_v_min[++m_h_cur_pos - m_h_neg_pos];
        } while (m_v_cur_pos > m_v_max[m_h_cur_pos - m_h_neg_pos]);
    }

    m_cur_pt = m_base_pt + (m_h_vec * m_h_cur_pos) + (m_v_vec * m_v_cur_pos);

    ++m_v_cur_pos;

    return &m_cur_pt;
}
