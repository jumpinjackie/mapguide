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

#include "SE_IdentityJoin.h"
#include <float.h>

SE_IdentityJoin::SE_IdentityJoin(RS_Bounds& bounds, double chop, bool end) :
    m_chop(chop),
    m_end(end),
    m_identity_region(),
    m_xf_bounds()
{
    if (end)
    {
        m_identity_region.minx = -DBL_MAX;
        m_identity_region.maxx = bounds.maxx - chop;
        m_xf_bounds.maxx = m_identity_region.maxx;
        m_xf_bounds.minx = bounds.minx;
    }
    else
    {
        m_identity_region.maxx = DBL_MAX;
        m_identity_region.minx = bounds.minx + chop;
        m_xf_bounds.minx = m_identity_region.minx;
        m_xf_bounds.maxx = bounds.maxx;
    }

    m_identity_region.miny = -DBL_MAX;
    m_identity_region.maxy = DBL_MAX;
    m_xf_bounds.miny = bounds.miny;
    m_xf_bounds.maxy = bounds.maxy;
}

RS_Bounds* SE_IdentityJoin::GetIdentityRegions(int &length)
{
    length = 1;
    return &m_identity_region;
}

RS_F_Point* SE_IdentityJoin::GetDiscontinuities(int &length)
{
    length = 0;
    return NULL;
}

RS_Bounds* SE_IdentityJoin::GetNonlinearRegions(int &length)
{
    length = 0;
    return NULL;
}

RS_Bounds& SE_IdentityJoin::GetTransformedBounds()
{
    return m_xf_bounds;
}

double SE_IdentityJoin::GetXChop(bool &chopEnd)
{
    chopEnd = m_end;
    return m_chop;
}

void SE_IdentityJoin::Transform(double& /*x*/, double& /*y*/, SE_TransformInfo* info)
{
    if (info)
    {
        info->dXdx = 1.0;
        info->dYdy = 1.0;

        info->dYdx = 0.0;
        info->dXdy = 0.0;
        info->d2Xdx2 = 0.0;
        info->d2Ydx2 = 0.0;
        info->d2Xdy2 = 0.0;
        info->d2Ydy2 = 0.0;
        info->d2Xdxdy = 0.0;
        info->d2Ydxdy = 0.0;
    }
}
