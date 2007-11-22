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

#ifndef SE_CAP_ROUND_H
#define SE_CAP_ROUND_H

#include "SE_Cap_Square.h"
#include "SE_Matrix.h"

template<class USER_DATA> class SE_Cap_Round : public SE_Cap_Square<USER_DATA>
{
protected:
/* Using declarations to make the linux build happy */
using SE_Cap<USER_DATA>::m_base_pos;
using SE_Cap<USER_DATA>::m_base_pt;
using SE_Cap<USER_DATA>::m_cap_ext;
using SE_Cap<USER_DATA>::m_cw_nml;
using SE_Cap<USER_DATA>::m_tolerance;
using SE_Cap_Square<USER_DATA>::m_ext_pt;
using SE_Cap_Square<USER_DATA>::m_ext_pos;

double    m_cos_arc_inc;   /* Cosine of arc segment span */
double    m_sin_arc_inc;   /* Sine of arc segment span */
int       m_quad_segs;     /* Number of segments either above or below the centerline */

public:
    SE_INLINE SE_Cap_Round( SE_RenderLineStyle* style );

    virtual void Construct( const SE_SegmentInfo& seg,
                            double& tolerance,
                            bool isStart );
    virtual void Transform( SE_JoinTransform<USER_DATA>& joins );
};


// Function Implementations

template<class USER_DATA>
SE_Cap_Round<USER_DATA>::SE_Cap_Round( SE_RenderLineStyle* style )
    : SE_Cap_Square<USER_DATA>(style)
{
}


template<class USER_DATA>
void SE_Cap_Round<USER_DATA>::Construct( const SE_SegmentInfo& seg,
                                         double& tolerance,
                                         bool isStart )
{
    SE_Cap_Square<USER_DATA>::Construct(seg, tolerance, isStart);

    double max_span;

    if (*m_tolerance >= m_cap_ext)
    {
        max_span = M_PI;
        *m_tolerance -= m_cap_ext;
    }
    else
    {
        max_span = 2.0 * acos(1.0 - *m_tolerance / m_cap_ext);
        /* TODO: be more accurate about this */
       *m_tolerance = 0.0;
    }

    m_quad_segs = (int)ceil(M_PI / (2.0 * max_span));
    double inc = M_PI / (double)(2 * m_quad_segs);
    m_cos_arc_inc = cos(inc);
    m_sin_arc_inc = sin(inc);
}


template<class USER_DATA>
void SE_Cap_Round<USER_DATA>::Transform( SE_JoinTransform<USER_DATA>& joins )
{
    /* The outer point is on the cw side, as in a ccw join */
    joins.StartJoin(false);

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

#endif // SE_CAP_SQUARE_H
