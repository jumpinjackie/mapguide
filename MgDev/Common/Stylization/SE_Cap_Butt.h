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

#ifndef SE_CAP_BUTT_H
#define SE_CAP_BUTT_H

#include "SE_Cap.h"

template<class USER_DATA> class SE_Cap_Butt : public SE_Cap<USER_DATA>
{
public:
    SE_INLINE SE_Cap_Butt( SE_RenderLineStyle* style );
    
    virtual void Construct( const SE_SegmentInfo& seg, double& tolerance, bool isStart );
    virtual void Transform( SE_JoinTransform<USER_DATA>& joins );

protected:
    SE_Tuple    m_tan;
};

// Function Implementations

template<class USER_DATA> SE_Cap_Butt<USER_DATA>::SE_Cap_Butt( SE_RenderLineStyle* style ) :
    SE_Cap(style)
{
}

template<class USER_DATA> void
    SE_Cap_Butt<USER_DATA>::Construct( const SE_SegmentInfo& seg, double& tolerance, bool isStart )
{
    SE_Cap::Construct(seg, tolerance, isStart);

    m_tan = SE_Tuple(-m_seg->next.y, m_seg->next.x) * (m_cap_ext / m_seg->nextlen);
}

template<class USER_DATA> void 
    SE_Cap_Butt<USER_DATA>::Transform(SE_JoinTransform<USER_DATA>& joins)
{
    joins.StartJoin(true); /* TODO */

    if (m_start)
        joins.AddVertex( *m_seg->vertex + m_tan,
                         *m_seg->vertex,
                         *m_seg->vertex - m_tan,
                         m_seg->vertpos ); 
    else
    {
        SE_Tuple end = *m_seg->vertex + m_seg->next;
        joins.AddVertex( end + m_tan,
                         end,
                         end - m_tan,
                         m_seg->vertpos + m_seg->nextlen );
    }
}

#endif // SE_CAP_BUTT_H