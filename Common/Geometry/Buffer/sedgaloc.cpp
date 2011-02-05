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
// FILE: sedgaloc.cpp.
//
// PURPOSE: Implementation of the SweepEdgeAllocator class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

static const int SweepEdgeBlockSize = 512;


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an instance of the SweepEdgeAllocator
//          object.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

SweepEdgeAllocator::SweepEdgeAllocator() :
    BufferAllocator<SweepEdge>(SweepEdgeBlockSize)
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

SweepEdgeAllocator::~SweepEdgeAllocator()
{
} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: Allocate().
//
// PURPOSE: Allocate and initialize a SweepEdge.
//
// PARAMETERS:
//
//     Input:
//
//         wingedEdge - passes a pointer to the WingedEdge object that the
//                      newly allocated SweepEdge object is to be based on.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the initialized SweepEdge.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to allocate the SweepEdge.
//
//------------------------------------------------------------------------------

SweepEdge *SweepEdgeAllocator::Allocate(WingedEdge *wingedEdge)
{
    // get a pointer to the new edge, initialize and return

    SweepEdge *sweepEdge = GetNextObject();
    sweepEdge->Initialize(wingedEdge);

    return sweepEdge;

} // end: Allocate()
