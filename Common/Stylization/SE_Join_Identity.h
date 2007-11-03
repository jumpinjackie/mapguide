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

#ifndef SE_JOIN_IDENTITY_H
#define SE_JOIN_IDENTITY_H

#include "SE_Join.h"

template<class USER_DATA> class SE_Join_Identity : public SE_Join<USER_DATA>
{
protected:
/* Using declarations to make the linux build happy */
using SE_Join<USER_DATA>::m_width;
using SE_Join<USER_DATA>::m_join_ext;
using SE_Join<USER_DATA>::m_lead;
using SE_Join<USER_DATA>::m_tail;
using SE_Join<USER_DATA>::m_lead_nml;
using SE_Join<USER_DATA>::m_tail_nml;
using SE_Join<USER_DATA>::m_lxt;
using SE_Join<USER_DATA>::m_colinear;

public:
    SE_Join_Identity( SE_RenderLineStyle* style );

    virtual void Construct( const SE_SegmentInfo& lead,
                            const SE_SegmentInfo& tail,
                            double& tolerance );
    virtual void Transform( SE_JoinTransform<USER_DATA>& joins );

protected:
    bool m_clockwise;
};

// Function Implementations

template<class USER_DATA>
SE_Join_Identity<USER_DATA>::SE_Join_Identity(SE_RenderLineStyle* style)
    : SE_Join<USER_DATA>(style)
{
}


template<class USER_DATA>
void SE_Join_Identity<USER_DATA>::Construct( const SE_SegmentInfo& lead,
                                             const SE_SegmentInfo& tail,
                                             double& tolerance)
{
    SE_Join<USER_DATA>::Construct(lead, tail, tolerance);

    if (m_colinear)
        return;

    m_clockwise = m_lxt > 0;

    /* If the join is cw (resp. ccw), the outer normal will be ccw (resp. cw) */
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


template<class USER_DATA>
void SE_Join_Identity<USER_DATA>::Transform(SE_JoinTransform<USER_DATA>& joins)
{
    if (m_colinear)
        return;

    joins.StartJoin(m_clockwise);

    /* Calculate the correct position in the case of closed contours */
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

#endif // SE_JOIN_IDENTITY_H
