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
protected:
/* Using declarations to make the linux build happy */
using SE_Cap<USER_DATA>::m_base_pos;
using SE_Cap<USER_DATA>::m_base_pt;
using SE_Cap<USER_DATA>::m_cap_ext;
using SE_Cap<USER_DATA>::m_cw_nml;

public:
    SE_INLINE SE_Cap_Butt( SE_RenderLineStyle* style );

    virtual void Construct( const SE_SegmentInfo& seg,
                            double& tolerance,
                            bool isStart );
    virtual void Transform( SE_JoinTransform<USER_DATA>& joins );
};


// Function Implementations

template<class USER_DATA>
SE_Cap_Butt<USER_DATA>::SE_Cap_Butt( SE_RenderLineStyle* style ) : 
    SE_Cap<USER_DATA>(style)
{
}


template<class USER_DATA>
void SE_Cap_Butt<USER_DATA>::Construct( const SE_SegmentInfo& seg,
                                        double& tolerance,
                                        bool isStart )
{
    SE_Cap<USER_DATA>::Construct(seg, tolerance, isStart);
}


template<class USER_DATA>
void SE_Cap_Butt<USER_DATA>::Transform( SE_JoinTransform<USER_DATA>& joins )
{
    /* The outer point is on the cw side, as in a ccw join */
    joins.StartJoin(false);

    joins.AddVertex( m_base_pt + m_cw_nml,
                     m_base_pt,
                     m_base_pt - m_cw_nml,
                     m_base_pos );
}

#endif // SE_CAP_BUTT_H
