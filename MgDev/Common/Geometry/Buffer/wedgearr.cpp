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
// FILE: wedgearr.cpp.
//
// PURPOSE: Implementation of the WingedEdgeArray object.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"
#include "wedgaloc.h"

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an WingedEdgeArray object.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to initialize the object.
//
//------------------------------------------------------------------------------

WingedEdgeArray::WingedEdgeArray() :
    m_edgeArray(NULL),
    m_nEdges(0),
    m_maxEdges(0)
{
    // allocate array of pointers to its initial size

    m_maxEdges = 32;
    m_edgeArray = new WingedEdge*[m_maxEdges];

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

WingedEdgeArray::~WingedEdgeArray()
{
    delete [] m_edgeArray;

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: AddEdge().
//
// PURPOSE: Add a WingedEdge pointer to the array (provided it is not already
//          contained in the array); the array is resized if necessary to
//          accomodate the pointer.
//
// PARAMETERS:
//
//     Input:
//
//         edge - passes a pointer to the WingedEdge.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available.
//
//------------------------------------------------------------------------------

void WingedEdgeArray::AddEdge(WingedEdge *edge)
{
    // check if the pointer is already contained in the array

    for (int i = 0; i < m_nEdges; i++) {
        if (m_edgeArray[i] == edge)
            return;
    }

    // resize the array if necessary

    if (m_nEdges >= m_maxEdges) {
        WingedEdge **temp = new WingedEdge*[m_maxEdges + m_maxEdges];

        WingedEdgeAllocator *wingedEdgeAlloc = new WingedEdgeAllocator;
        for (int i = 0; i < m_nEdges; i++)
        {
            temp[i] = wingedEdgeAlloc->Allocate();
            temp[i]->m_id = m_edgeArray[i]->m_id;
            temp[i] = m_edgeArray[i];
        }
        delete wingedEdgeAlloc;
        delete [] m_edgeArray;

        m_edgeArray = temp;
        m_maxEdges += m_maxEdges;
    }

    // add the pointer to the array

    m_edgeArray[m_nEdges++] = edge;

} // end: AddEdge()


//------------------------------------------------------------------------------
//
// METHOD: Reset().
//
// PURPOSE: Reset the array to an empty state.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void WingedEdgeArray::Reset()
{
    m_nEdges = 0;

} // end: Reset()
