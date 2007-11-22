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

#ifndef SE_JOIN_ROUND_H
#define SE_JOIN_ROUND_H

#include "SE_Join_Miter.h"
#include "SE_Matrix.h"

template<class USER_DATA> class SE_Join_Round : public SE_Join_Miter<USER_DATA>
{
protected:
/* Using declarations to make the linux build happy */
using SE_Join<USER_DATA>::m_width;
using SE_Join<USER_DATA>::m_join_ext;
using SE_Join<USER_DATA>::m_lead;
using SE_Join<USER_DATA>::m_tail;
using SE_Join<USER_DATA>::m_lxt;
using SE_Join<USER_DATA>::m_colinear;
using SE_Join_Miter<USER_DATA>::m_clockwise;
using SE_Join_Miter<USER_DATA>::m_cos_a;
using SE_Join_Miter<USER_DATA>::m_miter;
using SE_Join_Miter<USER_DATA>::m_tolerance;
using SE_Join_Miter<USER_DATA>::m_lead_nml;
using SE_Join_Miter<USER_DATA>::m_tail_nml;

public:
    SE_INLINE SE_Join_Round( SE_RenderLineStyle* style );

    virtual void Construct( const SE_SegmentInfo& lead,
                            const SE_SegmentInfo& tail,
                            double& tolerance );
    virtual void Transform( SE_JoinTransform<USER_DATA>& joins );

private:
    unsigned int m_verts;
    SE_Matrix m_vert_rot;
};

// Function Implementations

template<class USER_DATA> SE_Join_Round<USER_DATA>::SE_Join_Round( SE_RenderLineStyle* style ) :
SE_Join_Miter<USER_DATA>(style)
{
}


template<class USER_DATA>
void SE_Join_Round<USER_DATA>::Construct( const SE_SegmentInfo& lead,
                                          const SE_SegmentInfo& tail,
                                          double& tolerance )
{
    SE_Join_Miter<USER_DATA>::Construct(lead, tail, tolerance);

    if (m_colinear)
        return;

    /* Is the circular join appreciably different from a miter join? */
    if (m_miter - m_join_ext > *m_tolerance)
    {
       /*
        * Consider a circular arc of angle beta, and the chord connecting the endpoints.
        * The center of the chord is the farthest point from the arc, measuring radially
        * (proof of this?).  Then, consider the isoceles triangle formed by the chord, and
        * two radii connected to the endpoints of the arc.  The radius of the circle -
        * the height of the triangle bounds the error from the approximation of the arc
        * with the chord.  The height of the triangle will be r * cos(beta/2), and
        * the error is thus bounded: 0 <= e <= r * (1 - cos(beta/2).
        */

        double max_span;
        /* Thus, e has an absolute upper bound of r */
        if (*m_tolerance >= m_join_ext)
        {
            max_span = M_PI;
            *m_tolerance -= m_join_ext;
        }
        else
        {
            /* TODO: investigate whether an alternative method based on geomemetric identities
             * and subdivision instead of inverse transcendental functions might be faster */
            max_span = 2.0 * acos(1.0 - *m_tolerance / m_join_ext);
            /* TODO: bound can be slightly tighter */
            *m_tolerance = 0.0;
        }

        /* The outside angle of the join is the supplement of the inside angle */
        double arc_span = M_PI - acos(m_cos_a);

        /* The arc in the circular join covers a wedge of (pi - alpha) radians */
        double fsegs = ceil(arc_span / max_span);
        m_verts = (unsigned)fsegs - 1;
        /* Even number of interpolated points -- centerpoint would be interpolated over */
        if ((m_verts & 1) == 0)
            m_verts++, fsegs += 1.0;
        double ainc = arc_span / fsegs;

        m_vert_rot.setIdentity();
        m_vert_rot.translate(-m_tail->vertex->x, -m_tail->vertex->y);
        if (m_clockwise)
            m_vert_rot.rotate(sin(ainc), cos(ainc));
        else
            m_vert_rot.rotate(-sin(ainc), cos(ainc));
        m_vert_rot.translate(m_tail->vertex->x, m_tail->vertex->y);
    }
    else
    {
        /* TODO: is this error estimate may be unnecessarily loose? */
        *m_tolerance -= m_miter - m_join_ext;
        m_verts = 0;
    }
}


template<class USER_DATA>
void SE_Join_Round<USER_DATA>::Transform( SE_JoinTransform<USER_DATA>& joins )
{
    if (m_verts == 0 || m_colinear)
        return SE_Join_Miter<USER_DATA>::Transform(joins);

    SE_Tuple v_in = (m_tail_nml - m_lead_nml).normalize() * m_miter;
    SE_Tuple inner_join = *m_tail->vertex + v_in;
    unsigned int hverts = m_verts / 2;

    joins.StartJoin(m_clockwise);

    /* Calculate the correct position in the case of closed contours */
    bool open = m_tail->vertpos >= m_lead->vertpos;
    bool ending = joins.LastPosition() < m_lead->vertpos + m_lead->nextlen;
    double position =  !open && ending ? m_lead->vertpos + m_lead->nextlen : m_tail->vertpos;

    /* m_tail->vertex - v_in is the point of the miter;
     * m_lead_nml * m_width is (height) is the vector from the point (height) away
     * from the vertex along the leading segment's normal, to the point of the miter. */
    SE_Tuple prev_arc;
    if (open || ending)
    {
        prev_arc = *m_tail->vertex - v_in - (m_lead_nml * m_width);

        for (unsigned int i = 0; i <= hverts; i++)
        {
            joins.AddOutsidePoint(prev_arc);
            m_vert_rot.transform(prev_arc.x, prev_arc.y);
        }
    }
    else
    {
        prev_arc = *m_tail->vertex - (v_in * (m_join_ext / m_miter));
    }

    joins.AddVertex( prev_arc,
                     *m_tail->vertex,
                     inner_join,
                     position );

    if (open || !ending)
    {
        for (unsigned int i = 0; i <= hverts; i++)
        {
            m_vert_rot.transform(prev_arc.x, prev_arc.y);
            joins.AddOutsidePoint(prev_arc);
        }
    }
}

#endif // SE_JOIN_ROUND_H
