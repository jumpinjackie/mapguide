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
// FILE: worklist.cpp.
//
// PURPOSE: Implementation of the WorkListArray class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

// declaration of the Node and NodeAllocator classes

struct WorkListArray::Node {
    SweepEdge *m_edge;
    int m_interval;
    Node *m_prev;
    Node *m_next;
};


class WorkListArray::NodeAllocator : private BufferAllocator<Node> {
public:
    NodeAllocator();
    ~NodeAllocator();

    Node *Allocate();
    void Free(Node *node);

private:
    Node *m_headFreeList;
};


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize a WorkList object based on the specified
//          number of event intervals.
//
// PARAMETERS:
//
//     Input:
//
//         nEventIntervals - passes the number of event intervals involved in
//                           the plane sweep.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to initialize the work list object.
//
//------------------------------------------------------------------------------

WorkListArray::WorkListArray(int nEventIntervals) :
    m_workList(NULL),
    m_nodeAllocator(NULL)
{
    assert(nEventIntervals > 0);

    MG_TRY()

    m_workList = new Node *[nEventIntervals];
    ::memset(m_workList, 0, nEventIntervals * sizeof(Node *));
    m_nodeAllocator = new NodeAllocator;

    MG_CATCH(L"WorkListArray.WorkListArray")

    if (mgException != 0) // mgException is defined in MG_TRY() macro
    {
        Cleanup();
    }

    MG_THROW()
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

WorkListArray::~WorkListArray()
{
    Cleanup();

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: Push().
//
// PURPOSE: Push the specified edge onto the work list corresponding to the
//          specified event interval.
//
// PARAMETERS:
//
//     Input:
//
//         edge     - passes a pointer to the sweep edge that is to be pushed
//                    onto the work list.
//         interval - passes the index of the event interval the edge is to be
//                    pushed for.
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

void WorkListArray::Push(SweepEdge *edge, int interval)
{
    Node *node = m_nodeAllocator->Allocate();

    node->m_edge = edge;
    edge->SetWorkListNode(node);
    node->m_interval = interval;
    node->m_prev = NULL;
    node->m_next = (*this)[interval];

    (*this)[interval] = node;

    if (node->m_next != NULL)
        node->m_next->m_prev = node;

} // end: Push()


//------------------------------------------------------------------------------
//
// METHOD: Pop().
//
// PURPOSE: Pop the next edge (if any) from the work list corresponding to the
//          specified event interval.
//
// PARAMETERS:
//
//     Input:
//
//         interval - passes the index of the event interval the edge is to be
//                    popped for.
//
//     Output:
//
//         None.
//
// RETURNS: Returns the SweepEdge that was popped or NULL if there was no more
//          sweep edges for the specified interval.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

SweepEdge *WorkListArray::Pop(int interval)
{
    SweepEdge *edge = NULL;
    Node *node = (*this)[interval];

    if (node != NULL) {
        edge = node->m_edge;
        edge->SetWorkListNode(NULL);
        (*this)[interval] = node->m_next;
        if (node->m_next)
            node->m_next->m_prev = NULL;
        m_nodeAllocator->Free(node);
    }

    return edge;

} // end: Pop()


//------------------------------------------------------------------------------
//
// METHOD: Remove().
//
// PURPOSE: Remove the specified edge from the work list containing it (if any).
//
// PARAMETERS:
//
//     Input:
//
//         edge - passes a pointer to the SweepEdge.
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

void WorkListArray::Remove(SweepEdge *edge)
{
    Node *node = edge->GetWorkListNode();

    if (node != NULL) {
        edge->SetWorkListNode(NULL);
        if (node == (*this)[node->m_interval])
            (*this)[node->m_interval] = node->m_next;
        if (node->m_prev)
            node->m_prev->m_next = node->m_next;
        if (node->m_next)
            node->m_next->m_prev = node->m_prev;
        m_nodeAllocator->Free(node);
    }

} // end: Remove()


//------------------------------------------------------------------------------
//
// METHOD: operator[]().
//
// PURPOSE: Overloaded subscripting operator used to return a reference to the
//          indexed WorkListArray::Node object (actually a reference to a Work-
//          ListArray::Node pointer).
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index of the WorkListArray::Node object.
//
//     Output:
//
//         None.
//
// RETURNS: A reference to the indexed WorkListArray::Node object.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

WorkListArray::Node *&WorkListArray::operator[](int index) const
{
    return m_workList[index];

} // end: operator[]()


//------------------------------------------------------------------------------
//
// METHOD: Cleanup().
//
// PURPOSE: Release all dynamically allocated memory.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void WorkListArray::Cleanup()
{
    delete [] m_workList;
    delete m_nodeAllocator;

} // end: Cleanup()


//------------------------------------------------------------------------------
//
// Implementation of the WorkListArray::NodeAllocator class.
//
//------------------------------------------------------------------------------

static const int WorkListNodeBlockSize = 256;


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Initialize an instance of the WorkListArray::NodeAllocator class.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

WorkListArray::NodeAllocator::NodeAllocator() :
    BufferAllocator<Node>(WorkListNodeBlockSize),
    m_headFreeList(NULL)
{
} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Destroy an instance of the WorkListArray::NodeAllocator class,
//          releasing all dynamically allocated resources.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

WorkListArray::NodeAllocator::~NodeAllocator()
{
} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: Allocate().
//
// PURPOSE: Return the next available free node in the node pool, reallocating
//          the pool to a larger size if it has been exhausted.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A pointer to the new node.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to expand the node pool.
//
//------------------------------------------------------------------------------

WorkListArray::Node *WorkListArray::NodeAllocator::Allocate()
{
    // first check the linked list of free nodes. If the list is not empty,
    // then remove the head of the list

    Node *freeNode;

    if (m_headFreeList != NULL) {
        freeNode = m_headFreeList;
        m_headFreeList = m_headFreeList->m_next;
    }

    // else get the allocate a new node

    else
        freeNode = GetNextObject();

    return freeNode;

} // end: Allocate()


//------------------------------------------------------------------------------
//
// METHOD: Free().
//
// PURPOSE: Free the specified WorkListArray node, placing it back in the pool
//          of available nodes.
//
// PARAMETERS:
//
//     Input:
//
//         node - pointer to the node to be freed. The node pointed to is
//                assumed to have been obtained in a prior call to the
//                Allocate() method.
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

void WorkListArray::NodeAllocator::Free(Node *node)
{
    // add to head of linked list of free nodes

    node->m_next = m_headFreeList;
    m_headFreeList = node;

} // end: Free()
