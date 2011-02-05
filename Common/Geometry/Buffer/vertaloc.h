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
// FILE: vertaloc.h.
//
// PURPOSE: Declaration of the VertexAllocator class. The VertexAllocator class
//          is used to allocate the vertices of WingedEdge objects. The lifetime
//          of a vertex is identical to the lifetime of the VertexAllocator
//          object from which it was obtained.
//
//------------------------------------------------------------------------------

#ifndef _VERTALOC_H_
#define _VERTALOC_H_

class VertexAllocator : private BufferAllocator<OpsDoublePoint> {
public:
    /// constructor/destructor

    VertexAllocator();
    virtual ~VertexAllocator();

    /// method to allocate and initialize a new vertex

    OpsDoublePoint *Allocate(double x, double y);
};

#endif
