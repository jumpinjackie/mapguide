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

/// \ingroup Geometry_Module

//------------------------------------------------------------------------------
//
// FILE: sedgaloc.h.
//
// PURPOSE: Declaration of the SweepEdgeAllocator class. The SweepEdgeAllocator
//          class is used to allocate SweepEdges. The lifetime of a SweepEdge
//          is identical to the lifetime of the SweepEdgeAllocator object
//          from which it was obtained.
//
//------------------------------------------------------------------------------

#ifndef _SEDGALOC_H_
#define _SEDGALOC_H_

class SweepEdgeAllocator : private BufferAllocator<SweepEdge> {
public:
    /// constructor/destructor

    SweepEdgeAllocator();
    virtual ~SweepEdgeAllocator();

    /// method to allocate and initialize a new SweepEdge

    SweepEdge *Allocate(WingedEdge *edge);
};

#endif
