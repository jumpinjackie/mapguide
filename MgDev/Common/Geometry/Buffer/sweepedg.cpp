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
// FILE: sweepedg.cpp.
//
// PURPOSE: Implementation of the SweepEdge class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an instance of the SweepEdge class.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

SweepEdge::SweepEdge() :
    m_slope(0.0),
    m_wingedEdge(NULL),
    m_workListNode(NULL),
    m_xotNode(NULL)
{
} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Initialize().
//
// PURPOSE: Initialize a SweepEdge based on the specified WingedEdge object.

//
// PARAMETERS:
//
//     Input:
//
//         edge - passes a pointer to the WingedEdge object to initialize with.
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

void SweepEdge::Initialize(WingedEdge *edge)
{
    m_wingedEdge = edge;

    if (edge->m_vert[0]->x != edge->m_vert[1]->x)
        m_slope = (edge->m_vert[1]->y - edge->m_vert[0]->y) /
                  (edge->m_vert[1]->x - edge->m_vert[0]->x);
    else
        m_slope = HUGE_VAL;

} // end: Initialize()
