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

#ifndef SE_BUTTCAP_H
#define SE_BUTTCAP_H

#include "StylizationDefs.h"

template<class USER_DATA> class SE_ButtCap
{
public:
    typedef struct IdentityData CAP_DATA_TYPE;

    SE_ButtCap( SE_SegmentInfo& cap, bool isStart );

    SE_INLINE const double& cap_width() const { return 0.0; }
    SE_INLINE const double& cap_error() const { return 0.0; }
    SE_INLINE const SE_SegmentInfo& cap_seg() const { return m_seg; }

    void Transform(SE_JoinBuffer<USER_DATA>& joins);
    static void SetCapData(const CAP_DATA_TYPE& data) { m_data = data; }

protected:
    bool                    m_start;
    const SE_SegmentInfo&   m_seg;
    SE_Tuple                m_tan;

    SE_THREAD static CAP_DATA_TYPE m_data;
};

#endif // SE_BUTTCAP_H
