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

#ifndef SE_CAP_TRIANGLE_H
#define SE_CAP_TRIANGLE_H

#include "SE_Cap_Square.h"

template<class USER_DATA> class SE_Cap_Triangle : public SE_Cap_Square<USER_DATA>
{
protected:
/* Using declarations to make the linux build happy */
using SE_Cap<USER_DATA>::m_base_pos;
using SE_Cap<USER_DATA>::m_base_pt;
using SE_Cap<USER_DATA>::m_cap_ext;
using SE_Cap<USER_DATA>::m_cw_nml;
using SE_Cap_Square<USER_DATA>::m_ext_pt;
using SE_Cap_Square<USER_DATA>::m_ext_pos;

public:
    SE_INLINE SE_Cap_Triangle(SE_RenderLineStyle* style);

    virtual void Transform(SE_JoinTransform<USER_DATA>& joins);
};


// Function Implementations

template<class USER_DATA>
SE_Cap_Square<USER_DATA>::SE_Cap_Triangle(SE_RenderLineStyle* style) :
    SE_Cap_Square<USER_DATA>(style)
{
}


template<class USER_DATA>
void SE_Cap_Triangle<USER_DATA>::Transform(SE_JoinTransform<USER_DATA>& joins)
{
    /* The outer point is on the cw side, as in a ccw join */
    joins.StartJoin(false);

    if (m_ext_pos > m_base_pos)
    {
        joins.AddVertex( m_base_pt + m_cw_nml,
                         m_base_pt,
                         m_base_pt - m_cw_nml,
                         m_base_pos );

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

        joins.AddVertex( m_base_pt + m_cw_nml,
                         m_base_pt,
                         m_base_pt - m_cw_nml,
                         m_base_pos );
    }
}

#endif // SE_CAP_TRIANGLE_H
