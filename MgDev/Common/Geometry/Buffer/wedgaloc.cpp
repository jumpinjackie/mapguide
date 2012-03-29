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

//-------------------------------------------------------------------------
//
// FILE: wedgaloc.cpp.
//
// PURPOSE:  Implementation of the WingedEdgeAllocator class.
//
//-------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

static const int WingedEdgeBlockSize = 512;

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an instance of the WingedEdgeAllocator
//          object.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

WingedEdgeAllocator::WingedEdgeAllocator() :
   BufferAllocator<WingedEdge>(WingedEdgeBlockSize)
{
} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Release all dynamically allocated resources.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

WingedEdgeAllocator::~WingedEdgeAllocator()
{
} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: Allocate().
//
// PURPOSE: Allocate and initialize a WingedEdge object.
//
// PARAMETERS:
//
//     Input:
//
//         verts - passes an array of 2 pointers to OpsDoublePoints. The two
//                 OpsDoublePoint objects pointed are used to initialize the
//                 vertices of the WingedEdge object. The interior (of the
//                 polygon) is assumed to lie to the left of the edge.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the initialized WingedEdge object.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to allocate the WingedEdge.
//
//------------------------------------------------------------------------------

WingedEdge *WingedEdgeAllocator::Allocate(OpsDoublePoint *vert[2])
{
    WingedEdge *edge = GetNextObject();
    edge->Initialize(vert);
    edge->m_id = this->GetNObjects() - 1;
    return edge;

} // end: Allocate()


//------------------------------------------------------------------------------
//
// METHOD: Allocate().
//
// PURPOSE: Allocate and initialize a WingedEdge object. The vertex pointers of
//          the allocated edge are set to NULL.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A pointer to the initialized WingedEdge object.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to allocate the WingedEdge.
//
//------------------------------------------------------------------------------

WingedEdge *WingedEdgeAllocator::Allocate()
{
    WingedEdge *edge = GetNextObject();
    edge->m_id = this->GetNObjects() - 1;
    return edge;

} // end: Allocate()
