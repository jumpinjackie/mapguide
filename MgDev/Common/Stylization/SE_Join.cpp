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
#include "SE_Join.h"


// SE_Join

SE_Join::SE_Join(SE_RenderLineStyle* style) :
    m_join_ext(0.001)
{
    double t;
    for (int i=0; i<4; ++i)
    {
        if ((t = fabs(style->bounds[i].y)) > m_join_ext)
            m_join_ext = t;
    }
}


void SE_Join::Construct(const SE_SegmentInfo& lead,
                        const SE_SegmentInfo& tail,
                        double& tolerance)
{
    m_lead = &lead;
    m_tail = &tail;
    m_tolerance = &tolerance;

    m_lead_nml = lead.next * (1.0 / lead.nextlen);
    m_tail_nml = tail.next * (1.0 / tail.nextlen);

    m_lxt = m_lead_nml.cross(m_tail_nml);
    m_colinear = fabs(m_lxt) < COLINEAR_THRESHOLD;
}


// SE_Join_Identity

void SE_Join_Identity::Construct(const SE_SegmentInfo& lead,
                                 const SE_SegmentInfo& tail,
                                 double& tolerance)
{
    SE_Join::Construct(lead, tail, tolerance);

    m_clockwise = m_lxt > 0;

    // if the join is cw (resp. ccw), the outer normal will be ccw (resp. cw)
    std::swap(m_lead_nml.x, m_lead_nml.y);
    std::swap(m_tail_nml.x, m_tail_nml.y);
    if (m_clockwise)
    {
        m_lead_nml.y = -m_lead_nml.y;
        m_tail_nml.y = -m_tail_nml.y;
    }
    else
    {
        m_lead_nml.x = -m_lead_nml.x;
        m_tail_nml.x = -m_tail_nml.x;
    }

    m_lead_nml *= m_join_ext;
    m_tail_nml *= m_join_ext;

    m_width = 0.0;
}


void SE_Join_Identity::Transform(SE_JoinTransform& joins, const LocalJoinInfo& data)
{
    joins.StartJoin(m_clockwise, data);

    // calculate the correct position in the case of closed contours
    bool open = m_tail->vertpos >= m_lead->vertpos;
    bool ending = joins.LastPosition() < m_lead->vertpos + m_lead->nextlen;
    double position =  !open && ending ? m_lead->vertpos + m_lead->nextlen : m_tail->vertpos;

    if (open || ending)
        joins.AddVertex( *m_tail->vertex + m_lead_nml,
                         *m_tail->vertex,
                         *m_tail->vertex - m_lead_nml,
                         position );

    if (open || !ending)
        joins.AddVertex( *m_tail->vertex + m_tail_nml,
                         *m_tail->vertex,
                         *m_tail->vertex - m_tail_nml,
                         position );
}


// SE_Join_Miter

void SE_Join_Miter::Construct(const SE_SegmentInfo& lead,
                              const SE_SegmentInfo& tail,
                              double& tolerance)
{
    SE_Join::Construct(lead, tail, tolerance);

    if (m_colinear)
        return;

    // we default conceptually to y-down
    m_clockwise = (m_lxt > 0.0);

    m_cos_a = -m_lead_nml.dot(m_tail_nml);
    m_sin_a = fabs(m_lxt);
    m_tan_ha = (1.0 - m_cos_a) / m_sin_a;   // half-angle identity for tangent
    m_cos_ha = sqrt((1.0 + m_cos_a) * 0.5); // half-angle identity for cosine
    m_sin_ha = m_tan_ha * m_cos_ha;

    m_width = m_join_ext / m_tan_ha;
    m_miter = m_join_ext / m_sin_ha;

    // If the miter length is extremely long, the inside point (the point where the line segments
    // would separate, if they were infinitely long) might be extremely far away from the line.
    // It is undesirable for highly acute angles to cast thin rays far away from their location,
    // so we will constrain the inside point to a point along the bisector of the angle that is
    // also at the intersection of a normal of the lead segment with a normal of the tail segment.

    // The outer and inner miter points lie on the bisector of the angle;  Thus, if the inner point
    // is m_miter distant from the vertex, then the base point of the normal to lead (resp. tail)
    // that intersects lead (resp. tail) and the inner point will be d = cos(alpha/2) * m_miter
    // away from the vertex, where alpha is the angle of the join.

    // If the inner join extends beyond one of the segments, we must move the inner point so
    // that d does not exceed min(lead.nextlen, tail.nextlen).
    m_inside = std::min<double>(m_miter, (std::min<double>(lead.nextlen, tail.nextlen) / m_cos_ha));
}


void SE_Join_Miter::Transform(SE_JoinTransform& joins, const LocalJoinInfo& data)
{
    // calculate the correct position in the case of closed contours
    bool open = m_tail->vertpos >= m_lead->vertpos;

    // the start and end of the line will have the same lead/tail segments, so we determine
    // which one is active by checking against the last vertex position
    bool ending = joins.LastPosition() < m_lead->vertpos + m_lead->nextlen;
    double position =  !open && ending ? m_lead->vertpos + m_lead->nextlen : m_tail->vertpos;

    if (m_colinear)
    {
        if (!open)
        {
            joins.StartJoin(false, data);
            SE_Tuple cw_nml = SE_Tuple(-m_tail->next.y, m_tail->next.x) * (m_join_ext / m_tail->nextlen);
            joins.AddVertex( *m_tail->vertex + cw_nml,
                             *m_tail->vertex,
                             *m_tail->vertex - cw_nml,
                             position );
        }
        return;
    }

    joins.StartJoin(m_clockwise, data);

    SE_Tuple v = (m_lead_nml - m_tail_nml).normalize();

    joins.AddVertex( *m_tail->vertex + (v * m_miter),
                     *m_tail->vertex,
                     *m_tail->vertex - (v * m_inside),
                     position );
}


// SE_Join_Round

void SE_Join_Round::Construct(const SE_SegmentInfo& lead,
                              const SE_SegmentInfo& tail,
                              double& tolerance)
{
    SE_Join_Miter::Construct(lead, tail, tolerance);

    if (m_colinear)
        return;

    double max_tol = JOIN_ERROR_FRACTION * *m_tolerance;

    // Is the circular join appreciably different from a miter join?
    if (m_miter - m_join_ext > max_tol)
    {
        // Consider a circular arc of angle beta, and the chord connecting the endpoints.
        // The center of the chord is the farthest point from the arc, measuring radially
        // (proof of this?).  Then, consider the isoceles triangle formed by the chord, and
        // two radii connected to the endpoints of the arc.  The radius of the circle -
        // the height of the triangle bounds the error from the approximation of the arc
        // with the chord.  The height of the triangle will be r * cos(beta/2), and
        // the error is thus bounded: 0 <= e <= r * (1 - cos(beta/2).

        // thus, e has an absolute upper bound of r
        double max_span;
        if (max_tol >= m_join_ext)
        {
            max_span = M_PI;
            *m_tolerance -= m_join_ext;
        }
        else
        {
            // TODO: investigate whether an alternative method based on geomemetric identities
            //       and subdivision instead of inverse transcendental functions might be faster
            max_span = 2.0 * acos(1.0 - max_tol / m_join_ext);

            // TODO: bound can be slightly tighter
            *m_tolerance -= max_tol;
        }

        // the outside angle of the join is the supplement of the inside angle
        double arc_span = M_PI - acos(m_cos_a);

        // the arc in the circular join covers a wedge of (pi - alpha) radians
        double fsegs = ceil(arc_span / max_span);
        m_verts = (unsigned)fsegs - 1;

        // even number of interpolated points - centerpoint would be interpolated over
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
        // TODO: is this error estimate unnecessarily loose?
        *m_tolerance -= m_miter - m_join_ext;
        m_verts = 0;
    }
}


void SE_Join_Round::Transform(SE_JoinTransform& joins, const LocalJoinInfo& data)
{
    if (m_verts == 0 || m_colinear)
        return SE_Join_Miter::Transform(joins, data);

    SE_Tuple v_in = (m_tail_nml - m_lead_nml).normalize() * m_miter;
    unsigned int hverts = m_verts / 2;

    joins.StartJoin(m_clockwise, data);

    // calculate the correct position in the case of closed contours
    bool open = m_tail->vertpos >= m_lead->vertpos;
    bool ending = joins.LastPosition() < m_lead->vertpos + m_lead->nextlen;
    double position =  !open && ending ? m_lead->vertpos + m_lead->nextlen : m_tail->vertpos;

    // m_tail->vertex - v_in is the point of the miter
    // m_lead_nml * m_width is (height) is the vector from the point (height) away
    // from the vertex along the leading segment's normal, to the point of the miter
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
                     *m_tail->vertex + (v_in * (m_inside / m_miter)),
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


// SE_Join_Bevel

void SE_Join_Bevel::Construct(const SE_SegmentInfo& lead,
                              const SE_SegmentInfo& tail,
                              double& tolerance)
{
    SE_Join_Miter::Construct(lead, tail, tolerance);

    if (m_colinear)
        return;

    double mmin;    // the minimum miter limit (at which a flat bevel is possible)
    double mlen;    // the effective miter length

    // Derivation of mmin:
    //
    // Let alpha be the angle between the two line segments.
    // Consider the isoceles triangle formed by the vertex, and outer endpoint of each line segment
    // before any join is applied.  This is the area filled in by the minimum bevel.  The triangle
    // of the additional area filled by the miter has one angle alpha, and two angles (pi - alpha)/2,
    // which share vertices with the bevel triangle. Because the nonshared edges of the bevel triangle
    // are perpendicular to the line, the corresponding angles of the bevel triangle must be alpha/2,
    // leaving (pi - alpha) for the angle at the vertex.  If this triangle is bisected, we are left
    // with two right triangles, with angles (pi - alpha)/2, alpha/2, and pi/2, so
    // sin(alpha/2) = minlen/m_height.
    mmin = m_sin_ha;
    mlen = m_join_ext * (m_miter_limit < mmin ? mmin : m_miter_limit);

    // if converting a bevel to a miter is within the tolerance, we should do it, since miter
    // is simpler to calculate
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


void SE_Join_Bevel::Transform(SE_JoinTransform& joins, const LocalJoinInfo& data)
{
    if (m_top_width == m_width || m_colinear)
        return SE_Join_Miter::Transform(joins, data);

    joins.StartJoin(m_clockwise, data);

    SE_Tuple v_out = (m_lead->next - m_tail->next).normalize() * m_inside;
    SE_Tuple outer_join = *m_tail->vertex + v_out;

    double lost_width = m_width - m_top_width;
    SE_Tuple lead_bevel = outer_join - (m_lead_nml * lost_width);
    SE_Tuple tail_bevel = outer_join + (m_tail_nml * lost_width);

    // calculate the correct position in the case of closed contours
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
