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

//------------------------------------------------------------------------------
//
// FILE: vertaloc.cpp.
//
// PURPOSE: Implementation of the VertexAllocator class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

static const int VertexBlockSize = 1024;


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an instance of a VertexAllocator object.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

VertexAllocator::VertexAllocator() :
    BufferAllocator<OpsDoublePoint>(VertexBlockSize)
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

VertexAllocator::~VertexAllocator()
{
} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: Allocate().
//
// PURPOSE: Get and initialize the next available vertex.
//
// PARAMETERS:
//
//     Input:
//
//         x, y - pass the coordinates to initialize the vertex with.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the initialized vertex.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available.
//
//------------------------------------------------------------------------------

OpsDoublePoint *VertexAllocator::Allocate(double x, double y)
{
    // get a pointer to the next available vertex, initialize and return

    OpsDoublePoint *vertex = GetNextObject();
    vertex->x = x;
    vertex->y = y;

    return vertex;

} // end: Allocate()
