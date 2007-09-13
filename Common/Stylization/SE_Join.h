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

#ifndef SE_JOIN_H
#define SE_JOIN_H

#include "StylizationDefs.h"
#include "SE_JoinTransform.h"
#include "SE_RenderProxies.h"

/* Interface implemented by all join classes */
template<class USER_DATA> class SE_NOVTABLE SE_Join
{
public:
    /* Initializes the state of the join using two segments.
     * Implementors should call the base function. */
    virtual void Construct(const SE_SegmentInfo& lead,
                           const SE_SegmentInfo& tail,
                           double& tolerance);

    virtual void Transform(SE_JoinTransform<USER_DATA>& joins) = 0;

    /* The distance along the line from the inside of the join to the vertex */
    SE_INLINE const double& join_width() const { return m_width; }
    /* The vertical extent of the join above and below the centerline */
    SE_INLINE const double& join_height() const { return m_join_ext; }
    SE_INLINE const SE_SegmentInfo& lead_seg() const { return m_lead; }
    SE_INLINE const SE_SegmentInfo& tail_seg() const { return m_tail; }

protected:
    SE_INLINE SE_Join(SE_RenderLineStyle* style);

    const SE_SegmentInfo* m_lead;
    const SE_SegmentInfo* m_tail;
    /* The available (remaining) error for the join transformation */
    double* m_tolerance;

    double  m_width;
    double  m_join_ext;
};


// Implementation

template<class USER_DATA>
SE_Join<USER_DATA>::SE_Join(SE_RenderLineStyle* style)
{
    m_join_ext = 0.0;

    for (int i = 0; i < 4; i++)
    {
        double t;
        if ((t = fabs(style->bounds[i].y)) > m_join_ext)
            m_join_ext = t;
    }
}

template<class USER_DATA>
void SE_Join<USER_DATA>::Construct(const SE_SegmentInfo& lead,
                                   const SE_SegmentInfo& tail,
                                   double& tolerance)
{
    m_lead = &lead;
    m_tail = &tail;
    m_tolerance = &tolerance;
}

#endif // SE_JOIN_H
