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

#ifndef SIMPLEOVERPOST_H_
#define SIMPLEOVERPOST_H_

#include "Bounds.h"

//Interface definition for label overpost region maintenance object
//It holds on to lists of overposts regions and can also check if
//a given region overlaps existing overpost/exclusion regions
class SimpleOverpost
{
public:
    ~SimpleOverpost();

    bool Overlaps(RS_F_Point* pts, int npts);
    void AddRegion(RS_F_Point* pts, int npts);
    void AddRegions(SimpleOverpost& mgr);
    void Clear();

private:
    void ComputeBounds(RS_F_Point* RESTRICT pts, int npts, RS_Bounds& b);

    std::vector<RS_Bounds> m_excludes;
};

#endif
