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
// FILE: wedgaloc.h.
//
// PURPOSE: Declaration of the WingedEdgeAllocator class. The WindowEdge-
//          Allocator class is used to allocate WingedEdges. The lifetime of a
//          WingedEdge is identical to the lifetime of the WingedEdgeAllocator
//          object from which it was obtained.
//
//------------------------------------------------------------------------------

#ifndef _WEDGALOC_H_
#define _WEDGALOC_H_

class WingedEdge;

class WingedEdgeAllocator : public BufferAllocator<WingedEdge> {
public:
    /// constructor/destructor

    WingedEdgeAllocator();
    virtual ~WingedEdgeAllocator();

    /// methods to allocate and initialize a new WingedEdge

    WingedEdge *Allocate(OpsDoublePoint *vert[2]);
    WingedEdge *Allocate();
};

#endif
