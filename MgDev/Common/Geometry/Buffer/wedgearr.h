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
// FILE: wedgearr.h.
//
// PURPOSE: Declaration of the WingedEdgeArray class. The WingedEdgeArray
//          class implements an array type object that stores a list of
//          unique pointers WingedEdge objects. The number of pointers that
//          can be stored is not fixed, with dynamic resizing taking place
//          as required.
//
//------------------------------------------------------------------------------

#ifndef _WEDGEARR_H_
#define _WEDGEARR_H_

class WingedEdgeArray {
public:
    /// constructor/destructor

    WingedEdgeArray();
    virtual ~WingedEdgeArray();

    /// methods to add WingedEdge pointers and to reset array to an empty state

    void AddEdge(WingedEdge *edge);
    void Reset();

    /// methods to access the WingedEdge pointers

    WingedEdge *operator[](int edgeIndex) const;
    int GetNEdges() const;

private:
    WingedEdge **m_edgeArray;
    int m_nEdges;
    int m_maxEdges;
};


//------------------------------------------------------------------------------
//
// METHOD: operator[]().
//
// PURPOSE: Overloaded subscripting operator to access the indexed WingedEdge
//          pointer.
//
// PARAMETERS:
//
//     Input:
//
//         edgeIndex - passes the index of the WingedEdge pointer.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the indexed WingedEdge.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline WingedEdge *WingedEdgeArray::operator[](int edgeIndex) const
{
    assert(edgeIndex >= 0);
    assert(edgeIndex < m_nEdges);

    return m_edgeArray[edgeIndex];

} // end: operator[]()


//------------------------------------------------------------------------------
//
// METHOD: GetNEdges().
//
// PURPOSE: Get the number of pointers to WingedEdge objects currently stored
//          in the array.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The number of pointers stored in the array.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline int WingedEdgeArray::GetNEdges() const
{
    return m_nEdges;

} // end: GetNEdges()

#endif
