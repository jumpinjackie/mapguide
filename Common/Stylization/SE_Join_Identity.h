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

public:
    SE_Join_Identity( SE_RenderLineStyle* style );

    virtual void Construct( const SE_SegmentInfo& lead,
                            const SE_SegmentInfo& tail,
                            double& tolerance );
    virtual void Transform( SE_JoinTransform<USER_DATA>& joins );
    
protected:
    bool m_clockwise;

    SE_Tuple m_lead_out;
    SE_Tuple m_tail_out;
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

    m_lead_out = lead.next * (1.0 / lead.nextlen);
    m_tail_out = tail.next * (1.0 / tail.nextlen);

    m_clockwise = m_lead_out.cross(m_tail_out) > 0;


    /* If the join is cw (resp. ccw), the outer normal will be ccw (resp. cw) */
    std::swap(m_lead_out.x, m_lead_out.y);
    std::swap(m_tail_out.x, m_tail_out.y);
    if (m_clockwise)
    {
        m_lead_out.y = -m_lead_out.y;
        m_tail_out.y = -m_tail_out.y;
    }
    else
    {
        m_lead_out.x = -m_lead_out.x;
        m_tail_out.x = -m_tail_out.x;
    }

    m_lead_out *= m_join_ext;
    m_tail_out *= m_join_ext;

    m_width = 0.0;
}


template<class USER_DATA> 
void SE_Join_Identity<USER_DATA>::Transform(SE_JoinTransform<USER_DATA>& joins)
{
    joins.StartJoin(m_clockwise);

    joins.AddVertex( *m_tail->vertex + m_lead_out,
                     *m_tail->vertex,
                     *m_tail->vertex - m_lead_out,
                     m_tail->vertpos );

    joins.AddVertex( *m_tail->vertex + m_tail_out,
                     *m_tail->vertex,
                     *m_tail->vertex - m_tail_out,
                     m_tail->vertpos );
}

#endif // SE_JOIN_IDENTITY_H
