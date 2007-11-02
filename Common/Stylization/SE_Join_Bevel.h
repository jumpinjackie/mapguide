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

#ifndef SE_JOIN_BEVEL_H
#define SE_JOIN_BEVEL_H

#include "SE_Join_Miter.h"

template<class USER_DATA> class SE_Join_Bevel : public SE_Join_Miter<USER_DATA>
{
protected:
/* Using declarations to make the linux build happy */
using SE_Join<USER_DATA>::m_width;
using SE_Join<USER_DATA>::m_join_ext;
using SE_Join<USER_DATA>::m_lead;
using SE_Join<USER_DATA>::m_tail;
using SE_Join_Miter<USER_DATA>::m_sin_ha;
using SE_Join_Miter<USER_DATA>::m_miter;
using SE_Join_Miter<USER_DATA>::m_tolerance;
using SE_Join_Miter<USER_DATA>::m_clockwise;
using SE_Join_Miter<USER_DATA>::m_lead_nml;
using SE_Join_Miter<USER_DATA>::m_tail_nml;

public:
    SE_INLINE SE_Join_Bevel( SE_RenderLineStyle* style );

    virtual void Construct( const SE_SegmentInfo& lead,
                            const SE_SegmentInfo& tail,
                            double& tolerance );
    virtual void Transform( SE_JoinTransform<USER_DATA>& joins );

private:
    double m_top_width;    /* Top width (0 for minimal bevel, m_width for unbeveled miter) */
    double m_miter_limit;
};

// Function Implementations

template<class USER_DATA>
SE_Join_Bevel<USER_DATA>::SE_Join_Bevel( SE_RenderLineStyle* style ) :
    SE_Join_Miter<USER_DATA>(style),
    m_miter_limit(style->vertexMiterLimit)
{
}


template<class USER_DATA>
void SE_Join_Bevel<USER_DATA>::Construct( const SE_SegmentInfo& lead,
                                          const SE_SegmentInfo& tail,
                                          double& tolerance )
{
    SE_Join_Miter<USER_DATA>::Construct(lead, tail, tolerance);

    double mmin;        /* The minimum miter limit (at which a flat bevel is possible) */
    double mlen;        /* The effective miter length */

    /* Derivation of mmin:
     *
     * Let alpha be the angle between the two line segments.
     * Consider the icoceles triangle formed by the vertex, and outer endpoint of each line segment
     * before any join is applied.  This is the area filled in by the minimum bevel.  The triangle
     * of the additional area filled by the miter has one angle alpha, and two angles (pi - alpha)/2,
     * which share vertices with the bevel triangle. Because the nonshared edges of the bevel triangle
     * are perpendicular to the line, the corresponding angles of the bevel triangle must be alpha/2,
     * leaving (pi - alpha) for the angle at the vertex.  If this triangle is bisected, we are left
     * with two right triangles, with angles (pi - alpha)/2, alpha/2, and pi/2, so
     * sin(alpha/2) = minlen/m_height.
     */
    mmin = m_sin_ha;
    mlen = m_join_ext * (m_miter_limit < mmin ? mmin : m_miter_limit);

    /* If coverting a bevel to a miter is within the tolerance, we should do it, since miter
     * is simpler to calculate. */
    if (m_miter > mlen && ((m_miter - mlen) > *m_tolerance))
    {
        double minlen = mmin * m_join_ext;
        m_top_width = m_width*((mlen - minlen) / (m_miter - minlen));
    }
    else
    {
        m_top_width = m_width;
        if (m_miter > mlen)
            *m_tolerance -= mlen - m_miter;
    }
 }


template<class USER_DATA>
void SE_Join_Bevel<USER_DATA>::Transform( SE_JoinTransform<USER_DATA>& joins )
{
    if (m_top_width == m_width)
        return SE_Join_Miter<USER_DATA>::Transform(joins);

    joins.StartJoin(m_clockwise);

    SE_Tuple v_out = (m_lead->next - m_tail->next).normalize() * m_miter;
    SE_Tuple outer_join = *m_tail->vertex + v_out;

    double lost_width = m_width - m_top_width;
    SE_Tuple lead_bevel = outer_join - (m_lead_nml * lost_width);
    SE_Tuple tail_bevel = outer_join + (m_tail_nml * lost_width);

    /* Calculate the correct position in the case of closed contours */
    bool open = m_tail->vertpos >= m_lead->vertpos;
    bool ending = joins.LastPosition() < m_lead->vertpos + m_lead->nextlen;
    double position =  !open && ending ? m_lead->vertpos + m_lead->nextlen : m_tail->vertpos;

    if (open || ending)
        joins.AddOutsidePoint(lead_bevel);
    joins.AddVertex( (lead_bevel + tail_bevel) * 0.5,
                     *m_tail->vertex,
                     *m_tail->vertex - v_out,
                     position );
    if (open || !ending)
        joins.AddOutsidePoint(tail_bevel);
}

#endif // SE_JOIN_BEVEL_H
