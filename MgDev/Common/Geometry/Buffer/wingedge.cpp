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
// FILE: wingedge.cpp.
//
// PURPOSE: Implementation of the WingedEdge class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize a WingedEdge object. The wings are initial-
//          ized so that the edge loops back on itself.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

WingedEdge::WingedEdge() :
    m_flags(0)
{
    m_vert[0] = m_vert[1] = NULL;
    m_ccwEdge[0] = m_ccwEdge[1] = this;
    m_cwEdge[0] = m_cwEdge[1] = this;

} // end : constructor


//------------------------------------------------------------------------------
//
// METHOD: Initialize().
//
// PURPOSE: Initialize a WingedEdge object with the specified vertices. Note
//          that the interior of the polygonal region bounded by the edge is
//          assumed to lie to the left of the edge.
//
// PARAMETERS:
//
//     Input:
//
//         vertices - passes an array of 2 pointers to OpsDoublePoints. The
//                    OpsDoublePoints pointed to become the vertices of the
//                    edge. Note the lifetime of the vertices pointed to must
//                    be at least as long as that of the WingedEdge object.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void WingedEdge::Initialize(OpsDoublePoint *vertices[2])
{
    // order the vertices and set the edge side-in flags

    if ( vertices[0]->x < vertices[1]->x ||
         (vertices[0]->x == vertices[1]->x &&
          vertices[0]->y < vertices[1]->y)) {
        m_vert[0] = vertices[0];
        m_vert[1] = vertices[1];
        m_flags = MaxVertSideIn;
    }
    else {
        m_vert[0] = vertices[1];
        m_vert[1] = vertices[0];
        m_flags = MinVertSideIn;
    }

} // end: Initialize()
