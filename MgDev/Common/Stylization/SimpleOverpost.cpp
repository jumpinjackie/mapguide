//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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


bool SimpleOverpost::Overlaps(RS_Bounds& b)
{
    //look if the bounds overlaps current overpost regions
    for (size_t i=0; i<m_excludes.size(); i++)
    {
        RS_Bounds res = RS_Bounds::Intersect(b, m_excludes[i]);
        if (res.IsValid())
            return true;
    }

    return false;
}


void SimpleOverpost::AddRegion(RS_Bounds& b)
{
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
