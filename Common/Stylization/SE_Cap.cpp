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

#include "stdafx.h"
#include "SE_Cap.h"


SE_Cap::SE_Cap(SE_RenderLineStyle* style) :
    m_width(0.0),
    m_cap_ext(0.001)
{
    double t;
    for (int i=0; i<4; ++i)
    {
        if ((t = fabs(style->bounds[i].y)) > m_cap_ext)
            m_cap_ext = t;
    }
}


void SE_Cap::Construct(const SE_SegmentInfo &seg,
                       double &tolerance,
                       bool isStart)
{
    m_tolerance = &tolerance;
    m_cw_nml = SE_Tuple(-seg.next.y, seg.next.x) * (m_cap_ext / seg.nextlen);

    if (isStart)
    {
        m_base_pt = *seg.vertex;
        m_base_pos = seg.vertpos;
    }
    else
    {
        m_base_pt = *seg.vertex + seg.next;
        m_base_pos = seg.vertpos + seg.nextlen;
    }
}


// SE_Cap_Butt

void SE_Cap_Butt::Construct(const SE_SegmentInfo& seg,
                            double& tolerance,
                            bool isStart)
{
    SE_Cap::Construct(seg, tolerance, isStart);
}


void SE_Cap_Butt::Transform(SE_JoinTransform& joins, const LocalJoinInfo& data)
{
    // the outer point is on the cw side, as in a ccw join
    joins.StartJoin(false, data);

    joins.AddVertex( m_base_pt + m_cw_nml,
                     m_base_pt,
                     m_base_pt - m_cw_nml,
                     m_base_pos );
}


// SE_Cap_Round

void SE_Cap_Round::Construct(const SE_SegmentInfo& seg,
                             double& tolerance,
                             bool isStart)
{
    SE_Cap_Square::Construct(seg, tolerance, isStart);

    double max_span;
    double max_tol = *m_tolerance * CAP_ERROR_FRACTION;

    if (max_tol >= m_cap_ext)
    {
        max_span = M_PI;
        *m_tolerance -= m_cap_ext;
    }
    else
    {
        max_span = 2.0 * acos(1.0 - max_tol / m_cap_ext);

        // TODO: be more accurate about this
        *m_tolerance -= max_tol;
    }

    m_quad_segs = (int)ceil(M_PI / (2.0 * max_span));
    double inc = M_PI / (double)(2 * m_quad_segs);
    m_cos_arc_inc = cos(inc);
    m_sin_arc_inc = sin(inc);
}


void SE_Cap_Round::Transform(SE_JoinTransform& joins, const LocalJoinInfo& data)
{
    // the outer point is on the cw side, as in a ccw join
    joins.StartJoin(false, data);

    if (m_ext_pos > m_base_pos)
    {
        SE_Tuple pos_vec = m_cw_nml;
        SE_Tuple neg_vec = - m_cw_nml;
        joins.AddOutsidePoint(m_base_pt + pos_vec);
        joins.AddInsidePoint(m_base_pt + neg_vec);
        for (int i = 1; i < m_quad_segs; ++i)
        {
            pos_vec = SE_Tuple(pos_vec.x * m_cos_arc_inc + pos_vec.y * m_sin_arc_inc,
                               pos_vec.y * m_cos_arc_inc - pos_vec.x * m_sin_arc_inc);
            neg_vec = SE_Tuple(neg_vec.x * m_cos_arc_inc - neg_vec.y * m_sin_arc_inc,
                               neg_vec.y * m_cos_arc_inc + neg_vec.x * m_sin_arc_inc);
            joins.AddOutsidePoint(m_base_pt + pos_vec);
            joins.AddInsidePoint(m_base_pt + neg_vec);
        }

        joins.AddVertex( m_ext_pt,
                         m_ext_pt,
                         m_ext_pt,
                         m_ext_pos );
    }
    else
    {
        joins.AddVertex( m_ext_pt,
                         m_ext_pt,
                         m_ext_pt,
                         m_ext_pos );

        SE_Tuple pos_vec = m_ext_pt - m_base_pt;
        SE_Tuple neg_vec = pos_vec;
        for (int i = 0; i < m_quad_segs; ++i)
        {
            pos_vec = SE_Tuple(pos_vec.x * m_cos_arc_inc + pos_vec.y * m_sin_arc_inc,
                               pos_vec.y * m_cos_arc_inc - pos_vec.x * m_sin_arc_inc);
            neg_vec = SE_Tuple(neg_vec.x * m_cos_arc_inc - neg_vec.y * m_sin_arc_inc,
                               neg_vec.y * m_cos_arc_inc + neg_vec.x * m_sin_arc_inc);
            joins.AddOutsidePoint(m_base_pt + pos_vec);
            joins.AddInsidePoint(m_base_pt + neg_vec);
        }
    }
}


// SE_Cap_Square

void SE_Cap_Square::Construct(const SE_SegmentInfo& seg,
                              double& tolerance,
                              bool isStart)
{
    SE_Cap::Construct(seg, tolerance, isStart);

    m_width = m_cap_ext;
    m_ext_pt = seg.next * (m_width / seg.nextlen);
    if (isStart)
    {
        m_ext_pt.x = -m_ext_pt.x;
        m_ext_pt.y = -m_ext_pt.y;
        m_ext_pos = m_base_pos - m_width;
    }
    else
        m_ext_pos = m_base_pos + m_width;
    m_ext_pt += m_base_pt;
}


void SE_Cap_Square::Transform(SE_JoinTransform& joins, const LocalJoinInfo& data)
{
    // the outer point is on the cw side, as in a ccw join
    joins.StartJoin(false, data);

    if (m_ext_pos > m_base_pos)
    {
        joins.AddVertex( m_base_pt + m_cw_nml,
                         m_base_pt,
                         m_base_pt - m_cw_nml,
                         m_base_pos );

        joins.AddVertex( m_ext_pt + m_cw_nml,
                         m_ext_pt,
                         m_ext_pt - m_cw_nml,
                         m_ext_pos );
    }
    else
    {
        joins.AddVertex( m_ext_pt + m_cw_nml,
                         m_ext_pt,
                         m_ext_pt - m_cw_nml,
                         m_ext_pos );

        joins.AddVertex( m_base_pt + m_cw_nml,
                         m_base_pt,
                         m_base_pt - m_cw_nml,
                         m_base_pos );
    }
}


// SE_Cap_Triangle

void SE_Cap_Triangle::Transform(SE_JoinTransform& joins, const LocalJoinInfo& data)
{
    // the outer point is on the cw side, as in a ccw join
    joins.StartJoin(false, data);

    if (m_ext_pos > m_base_pos)
    {
        joins.AddVertex( m_base_pt + m_cw_nml,
                         m_base_pt,
                         m_base_pt - m_cw_nml,
                         m_base_pos );

        joins.AddVertex( m_ext_pt,
                         m_ext_pt,
                         m_ext_pt,
                         m_ext_pos );
    }
    else
    {
        joins.AddVertex( m_ext_pt,
                         m_ext_pt,
                         m_ext_pt,
                         m_ext_pos );

        joins.AddVertex( m_base_pt + m_cw_nml,
                         m_base_pt,
                         m_base_pt - m_cw_nml,
                         m_base_pos );
    }
}
