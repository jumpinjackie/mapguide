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
// FILE: xordtree.h.
//
// PURPOSE: Declaration of the XOrderedTree class. The XOrderedTree class impl-
//          ements the balanced binary tree required in Meyers plane sweep
//          algorithm for storing the current X-order of edges. The tree is
//          implemented as an AVL binary tree, and as Meyer suggests, the
//          current order is explicitly threaded as a doubly-linked list.
//
//------------------------------------------------------------------------------

#ifndef _XORDTREE_H_
#define _XORDTREE_H_


class XOrderedTree {
public:
    /// constructor/destructor

    XOrderedTree();
    virtual ~XOrderedTree();

    /// methods required in the plane sweep algorithm

    void SetAbscissa(double x);
    void Insert(SweepEdge *edge);
    void Delete(SweepEdge *edge);
    void Exchange(SweepEdge *edge);
    SweepEdge *Below(const SweepEdge *edge) const;
    SweepEdge *Above(const SweepEdge *edge) const;

    /// opaque forward declaration for XOrderedTree::Node

    struct Node;

private:
    /// opaque forward declaration of the NodeAllocator class

    class NodeAllocator;

    Node *m_root;                   // root node of the XOrderedTree
    Node *m_belowSentinel;          // sentinels used to simplify
    Node *m_aboveSentinel;          //     insertion/deletion, etc
    double  m_abscissa;             // current abscissa (x) value
    NodeAllocator *m_nodeAllocator; // used to allocate nodes

    BOOL InsertEdge(Node *&p, SweepEdge *edge);
    void BalanceLeft(Node *&p, BOOL &hReduced);
    void BalanceRight(Node *&p, BOOL &hReduced);
    void ReparentNodes(Node *p, Node *p1, Node *q);
    void ReparentNodes(Node *p, Node *p1, Node *p2, Node *q1, Node *q2);
};

#endif
