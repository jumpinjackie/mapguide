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

#include "SE_JoinTransform.h"
#include "SE_ButtCap.h"
#include "SE_RenderProxies.h"

template<class USER_DATA>
    SE_ButtCap<USER_DATA>::SE_ButtCap( SE_SegmentInfo& cap, bool isStart ) :
    m_seg(cap),
    m_start(isStart)
{
    m_tan = SE_Tuple(-m_seg.next.y, m_seg.next.x) * 
        (std::max(abs(m_data.ymax), abs(m_data.ymin)) / m_seg.nextlen);
}

template<class USER_DATA> void 
    SE_ButtCap<USER_DATA>::Transform(SE_JoinBuffer<USER_DATA>& joins)
{
    joins.StartJoin(true); /* TODO */

    if (m_start)
        joins.AddVertex( *m_seg.vertex + m_tan,
                         *m_seg.vertex,
                         *m_seg.vertex - m_tan,
                         m_seg.vertpos ); 
    else
    {
        SE_Tuple end = *m_seg.vertex + m_seg.next;
        joins.AddVertex( end + m_tan,
                         end,
                         end - m_tan,
                         m_seg.vertpos + m_seg.nextlen );
    }
}