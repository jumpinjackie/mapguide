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

#ifndef SE_CAP_H
#define SE_CAP_H

#include "StylizationDefs.h"
#include "SE_JoinTransform.h"
#include "SE_RenderProxies.h"

/* Interface implemented by all cap classes */
template<class USER_DATA> class SE_NOVTABLE SE_Cap
{
public:
    SE_INLINE const double& cap_width() const { return m_width; }
    SE_INLINE const double& cap_height() const { return m_cap_ext; }
    SE_INLINE const SE_SegmentInfo& cap_seg() const { return m_seg; }

    virtual void Construct( const SE_SegmentInfo& seg, double& tolerance, bool isStart );
    virtual void Transform( SE_JoinTransform<USER_DATA>& joins ) = 0;

protected:
    SE_INLINE SE_Cap( SE_RenderLineStyle* style );

    bool                    m_start;
    double                  m_width;
    double                  m_cap_ext;
    double*                 m_tolerance;
    const SE_SegmentInfo*   m_seg;
};

// Function Implementations

template<class USER_DATA> SE_Cap<USER_DATA>::SE_Cap(SE_RenderLineStyle* style)
{
    m_cap_ext = 0.0;

    double t;
    for (int i = 0; i < 4; i++)
    {
        if ((t = fabs(style->bounds[i].y)) > m_cap_ext)
            m_cap_ext = t;
    }
}


template<class USER_DATA> void
    SE_Cap<USER_DATA>::Construct( const SE_SegmentInfo &seg, double &tolerance, bool isStart)
{
    m_start = isStart;
    m_tolerance = &tolerance;
    m_seg = &seg;
}


#endif // SE_CAP_H