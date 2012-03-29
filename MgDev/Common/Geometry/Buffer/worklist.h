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
// FILE: worklist.h.
//
// PURPOSE: Declaration of the WorkListArray class. The WorkListArray class
//          implements the worklist array and associated functions required
//          in Meyer's plane sweep algorithm.
//
//------------------------------------------------------------------------------

#ifndef _WORKLIST_H_
#define _WORKLIST_H_

class SweepEdge;

class WorkListArray {
public:
    /// constructor/destructor

    WorkListArray(int nEventIntervals);
    virtual ~WorkListArray();

    /// methods to manipulate nodes in the worklist

    void Push(SweepEdge *edge, int interval);
    SweepEdge *Pop(int interval);
    void Remove(SweepEdge *edge);

    /// opaque forward declaration for the WorkList::Node

    struct Node;

private:
    /// opaque forward declaration of the NodeAllocator class

    class NodeAllocator;

    Node **m_workList;
    NodeAllocator *m_nodeAllocator;

    Node *&operator[](int index) const;
    void Cleanup();
};

#endif
