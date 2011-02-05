//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "stdafx.h"
#include "SimpleOverpost.h"


SimpleOverpost::~SimpleOverpost()
{
    m_excludes.clear();
}


bool SimpleOverpost::Overlaps(RS_F_Point* pts, int npts)
{
    RS_Bounds b;
    ComputeBounds(pts, npts, b);

    //look if the bounds overlaps current overpost regions
    for (size_t i=0; i<m_excludes.size(); ++i)
    {
        RS_Bounds res = RS_Bounds::Intersect(b, m_excludes[i]);
        if (res.IsValid())
            return true;
    }

    return false;
}


void SimpleOverpost::AddRegion(RS_F_Point* pts, int npts)
{
    RS_Bounds b;
    ComputeBounds(pts, npts, b);

    m_excludes.push_back(b);
}


void SimpleOverpost::AddRegions(SimpleOverpost& mgr)
{
    m_excludes.insert(m_excludes.end(), mgr.m_excludes.begin(), mgr.m_excludes.end());
}


void SimpleOverpost::Clear()
{
    m_excludes.clear();
}


//////////////////////////////////////////////////////////////////////////////
//computes axis aligned bounds of a point set
void SimpleOverpost::ComputeBounds(RS_F_Point* RESTRICT pts, int npts, RS_Bounds& b)
{
    if (!npts)
        return;

    b.minx = b.maxx = pts[0].x;
    b.miny = b.maxy = pts[0].y;
    b.minz = b.maxz = 0;

    for (int i=1; i<npts; ++i)
        b.add_point(pts[i]);
}
