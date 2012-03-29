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
// FILE: xordtree.cpp.
//
// PURPOSE: Implementation of the XOrderedTree class. The implementation is
//          based on an AVL binary tree implementation given in "Algorithms
//          and Data Structures", by Niklaus Wirth (Prentice Hall 1986).
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

// Declaration of an X-Ordered tree Node, and NodeAllocator classes

struct XOrderedTree::Node {
    short m_balFactor;    // AVL balance factor
    Node *m_parent;       // pointer to parent node
    Node *m_left;         // pointer to left child node
    Node *m_right;        // pointer to right child node
    Node *m_below;        // pointer to "below" node in the current order
    Node *m_above;        // pointer to "above" node in the current order
    SweepEdge *m_edge;    // pointer to the sweep edge stored at the node
};


class XOrderedTree::NodeAllocator : private BufferAllocator<Node> {
public:
    NodeAllocator();
    ~NodeAllocator();

    Node *Allocate(SweepEdge *edge, Node *parent, Node *below, Node *above);
    void Free(Node *node);

private:
    Node *m_headFreeList;
};


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an XOrderedTree object.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             avialable to complete initialization.
//
//------------------------------------------------------------------------------

XOrderedTree::XOrderedTree() :
    m_root(NULL),
    m_belowSentinel(NULL),
    m_aboveSentinel(NULL),
    m_abscissa(0.0),
    m_nodeAllocator(NULL)
{
    MG_TRY()

    m_nodeAllocator = new NodeAllocator;
    m_belowSentinel = m_nodeAllocator->Allocate(NULL, NULL, NULL, NULL);
    m_aboveSentinel = m_nodeAllocator->Allocate(NULL, NULL, NULL, NULL);

    MG_CATCH(L"XOrderedTree::XOrderedTree")

    if (mgException != 0) // mgException is defined in MG_TRY() macro
    {
        delete m_nodeAllocator;
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

XOrderedTree::~XOrderedTree()
{
    delete m_nodeAllocator;

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: SetAbscissa().
//
// PURPOSE: Set the abscissa for SweepEdge insertion and deletion. The abscissa
//          is the point on the x axis at which the X-Order of SweepEdges is
//          computed.
//
// PARAMETERS:
//
//     Input:
//
//         x - passes the value of the abscissa.
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

void XOrderedTree::SetAbscissa(double x)
{
    m_abscissa = x;

} // end: SetAbscissa()


//------------------------------------------------------------------------------
//
// METHOD: Insert().
//
// PURPOSE: Insert a SweepEdge into the XOrderedTree based on the current
//          abscissa.
//
// PARAMETERS:
//
//     Input:
//
//         edge - passes a pointer to the SweepEdge that is to be inserted.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to insert the edge.
//
//------------------------------------------------------------------------------

void XOrderedTree::Insert(SweepEdge *edge)
{
    // if tree is empty, then get a new node and assign it to the root pointer

    if (m_root == (Node *)NULL) {
        m_root = m_nodeAllocator->Allocate(edge, NULL, m_belowSentinel,
            m_aboveSentinel);
        m_belowSentinel->m_above = m_root;
        m_aboveSentinel->m_below = m_root;
    }

    // else call private recursive insertion method

    else
        InsertEdge(m_root, edge);

} // end: Insert()


//------------------------------------------------------------------------------
//
// METHOD: InsertEdge().
//
// PURPOSE: Recursively descend into the XOrderedTree finding the appropriate
//          leaf node for insertion. Rotations are performed as required along
//          the insertion path from the root to  maintain the AVL balance
//          conditions.
//
// PARAMETERS:
//
//     Input:
//
//         p    - reference to a pointer to a Node. On input, the referenced
//                pointer points to the current node on the insertion path from
//                the root.
//         edge - passes a pointer to the edge that is to be inserted.
//
//     Output:
//
//         p    - on output, the referenced pointer is updated to reflect any
//                rotations that may have occurred as a result of the insertion.
//
// RETURNS: TRUE if the height of the left or right subtree of p has increased
//          in height; otherwise returns FALSE.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to insert the edge.
//
//------------------------------------------------------------------------------

BOOL XOrderedTree::InsertEdge(Node *&p, SweepEdge *edge)
{
    BOOL hIncreased = FALSE;

    // if edge less than edge at current node, then descend into left subtree

    if (edge->XOrder(p->m_edge, m_abscissa) == -1) {
        // if not at the leaf level then descend

        if (p->m_left)
            hIncreased = InsertEdge(p->m_left, edge);

        // else allocate a new node, and link it into the current order

        else {
            p->m_left = m_nodeAllocator->Allocate(edge, p, p->m_below, p);
            p->m_below->m_above = p->m_left;
            p->m_below = p->m_left;
            hIncreased = TRUE;
        }

        // if height of the left subtree increased, then need to update
        // balance factors and (possibly) do a rotation

        if (hIncreased) {
           // only need to update balance factor if it is 0 or 1

           if (p->m_balFactor == 0)
               p->m_balFactor = -1;
           else if (p->m_balFactor == 1) {
               p->m_balFactor = 0;
               hIncreased = FALSE;
           }

           // else AVL condition has been violated at the current node, a
           // single or double rotation is required

           else { // p->m_balFactor == -1
               Node *p1, *p2;

               p1 = p->m_left;

               // if balFactor of left child is -1 then do a single left-left
               // rotation, and re-parent nodes affected by the rotation

               if (p1->m_balFactor == -1) {
                   p->m_left = p1->m_right; p1->m_right = p;
                   ReparentNodes(p, p1, p->m_left);
                   p->m_balFactor = 0;
                   p = p1;
               }

               // else need to do a double left-right rotation, and
               // re-parent nodes affected by the rotations

               else {
                   p2 = p1->m_right; p1->m_right = p2->m_left; p2->m_left = p1;
                   p->m_left = p2->m_right; p2->m_right = p;
                   ReparentNodes(p, p1, p2, p1->m_right, p->m_left);
                   p->m_balFactor = static_cast<short>(p2->m_balFactor == -1 ? 1 : 0);
                   p1->m_balFactor = static_cast<short>(p2->m_balFactor == 1 ? -1 : 0);
                   p = p2;
               }

               // AVL balance conditions have now been restored

               p->m_balFactor = 0;
               hIncreased = FALSE;
           }
        }
    }

    // else descend into the right subtree

    else {
        // if not at the leaf level then descend

        if (p->m_right)
            hIncreased = InsertEdge(p->m_right, edge);

        // else allocate a new node, and link it into the current order

        else {
            p->m_right = m_nodeAllocator->Allocate(edge, p, p, p->m_above);
            p->m_above->m_below = p->m_right;
            p->m_above = p->m_right;
            hIncreased = TRUE;
        }

        // if height of the right subtree increased, then need to update
        // balance factors and (possibly) do a rotation

        if (hIncreased) {
            // only need to update the balance factor if it is 0 or -1

            if (p->m_balFactor == 0)
                p->m_balFactor = 1;
            else if (p->m_balFactor == -1) {
                p->m_balFactor = 0;
                hIncreased = FALSE;
            }

            // else AVL condition has been violated at the current node, a
            // single or double rotation is required

            else {  // p->m_balFactor == 1
                Node *p1, *p2;

                p1 = p->m_right;

                // if balfactor of right child is 1 then do a single right-
                // right rotation, and re-parent nodes affected by the rotation

                if (p1->m_balFactor == 1) {
                    p->m_right = p1->m_left; p1->m_left = p;
                    ReparentNodes(p, p1, p->m_right);
                    p->m_balFactor = 0;
                    p = p1;
                }

                // else need to do a double right-left rotation, and
                // re-parent nodes affected by the rotations

                else {
                    p2 = p1->m_left; p1->m_left = p2->m_right; p2->m_right = p1;
                    p->m_right = p2->m_left; p2->m_left = p;
                    ReparentNodes(p, p1, p2, p1->m_left, p->m_right);
                    p->m_balFactor = static_cast<short>(p2->m_balFactor == 1 ? -1 : 0);
                    p1->m_balFactor = static_cast<short>(p2->m_balFactor == -1 ? 1 : 0);
                    p = p2;
                }

                // AVL balance conditions have now been restored

                p->m_balFactor = 0;
                hIncreased = FALSE;
            }
        }
    }

    // return height increased boolean flag

    return hIncreased;

} // end: InsertEdge()


//------------------------------------------------------------------------------
//
// METHOD: Delete().
//
// PURPOSE: Delete a SweepEdge from the current X-Order.
//
// PARAMETERS:
//
//     Input:
//
//         edge - passes a pointer to the edge that is to be deleted.
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

void XOrderedTree::Delete(SweepEdge *edge)
{
    // get a pointer to the Node corresponding to the edge

    Node *p = edge->GetXotNode();
    edge->SetXotNode(NULL);

    // if the node is a leaf then just remove it from the doubly-linked list

    if (p->m_left == NULL && p->m_right == NULL) {
        p->m_below->m_above = p->m_above;
        p->m_above->m_below = p->m_below;
    }

    // else if the node only has a right child, then swap the edge from
    // the right child

    else if (p->m_left == NULL) {
        p->m_edge = p->m_right->m_edge;
        p->m_edge->SetXotNode(p);
        p->m_above = p->m_right->m_above;
        p->m_above->m_below = p;
        p = p->m_right;
    }

    // else the node has an in-order predecessor node that is either a leaf
    // or has a left-child only

    else {
        p->m_edge = p->m_below->m_edge;
        p->m_edge->SetXotNode(p);
        Node *q = p->m_below;

        // if there is a left child then swap the edge from the left-child

        if (q->m_left != NULL) {
            q->m_edge = q->m_left->m_edge;
            q->m_edge->SetXotNode(q);
            q->m_below = q->m_left->m_below;
            q->m_below->m_above = q;
            p = q->m_left;
        }
        else {
             p->m_below = q->m_below;
             p->m_below->m_above = p;
             p = q;
        }
    }

    // p now points to the Node to be deleted; if p has a parent, then need
    // to walk up the tree to the root, restoring the balance as required

    if (p->m_parent != NULL) {
        BOOL hReduced = TRUE;
        BOOL leftSubtreeReduced = FALSE;

        // delete p, keeping track of whether it is a left or right child
        // of its parent

        if (p == p->m_parent->m_left) {
            p = p->m_parent;
            m_nodeAllocator->Free(p->m_left);
            p->m_left = NULL;
            leftSubtreeReduced = TRUE;
        }
        else {
            p = p->m_parent;
            m_nodeAllocator->Free(p->m_right);
            p->m_right = NULL;
        }

        // now walk up the tree from p to the root, restoring the AVL
        // balance conditions as required

        while (p != NULL && hReduced) {
            Node *parent = p->m_parent;
            Node **nodePtrPtr;
            BOOL leftChildOfParent = FALSE;

            if (parent == NULL)
                nodePtrPtr = &m_root;
            else if (p == parent->m_left) {
                nodePtrPtr = &parent->m_left;
                leftChildOfParent = TRUE;
            }
            else {
                nodePtrPtr = &parent->m_right;
                leftChildOfParent = FALSE;
            }

            if (leftSubtreeReduced)
                BalanceLeft(*nodePtrPtr, hReduced);
            else
                BalanceRight(*nodePtrPtr, hReduced);

            p = parent;
            leftSubtreeReduced = leftChildOfParent;
        }
    }

    // else node has no parent, so it must be the root

    else {
        m_nodeAllocator->Free(m_root);
        m_root = NULL;
    }

} // end: Delete()


//------------------------------------------------------------------------------
//
// METHOD: BalanceLeft().
//
// PURPOSE: This method is invoked by the delete method whenever the height of
//          the left subtree of a node has been reduced. Rotations are performed
//          if required to restore the AVL balance conditions at the node.
//
// PARAMETERS:
//
//     Input:
//
//         p         - reference to a pointer to a Node. On input, the
//                     referenced pointer points to the current node whose
//                     left subtree has been reduced in height.
//
//     Output:
//
//         p         - on output, the referenced pointer is updated to reflect
//                     any rotations that may have occurred.
//         hReduced  - reference to a boolean. The referenced boolean is set
//                     to FALSE if the height of the subtree rooted at p was
//                     not reduced; otherwise, the value is left unchanged.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void XOrderedTree::BalanceLeft(Node *&p, BOOL &hReduced)
{
    // only need to update balFactor if it is -1 or 0

    if (p->m_balFactor == -1)
        p->m_balFactor = 0;
    else if (p->m_balFactor == 0) {
        p->m_balFactor = 1;
        hReduced = FALSE;
    }

    // else need to do a rotation

    else {
        Node *p1, *p2;
        short b1, b2;

        p1 = p->m_right; b1 = p1->m_balFactor;

        // if balance factor of right subtree is >= 0 then need to do a
        // single right-right rotation, and re-parent rotated nodes

        if (b1 >= 0) {
            p->m_right = p1->m_left; p1->m_left = p;
            ReparentNodes(p, p1, p->m_right);

            if (b1 == 0) {
                p->m_balFactor = 1;
                p1->m_balFactor = -1;
                hReduced = FALSE;
            }
            else {
                p->m_balFactor = 0;
                p1->m_balFactor = 0;
            }
            p = p1;
        }

        // else need to do a double right-left rotation, and re-parent
        // rotated nodes

        else {
            p2 = p1->m_left; b2 = p2->m_balFactor;
            p1->m_left = p2->m_right; p2->m_right = p1;
            p->m_right = p2->m_left; p2->m_left = p;
            ReparentNodes(p, p1, p2, p1->m_left, p->m_right);
            p->m_balFactor = static_cast<short>(b2 == 1 ? -1 : 0);
            p1->m_balFactor = static_cast<short>(b2 == -1 ? 1 : 0);
            p = p2;
            p2->m_balFactor = 0;
        }
    }

} // end: BalanceLeft()


//------------------------------------------------------------------------------
//
// METHOD: BalanceRight().
//
// PURPOSE: This method is invoked by the delete method whenever the height of
//          the right subtree of a node has been reduced. Rotations are perform-
//          ed if required to restore the AVL balance conditions at the node.
//
// PARAMETERS:
//
//     Input:
//
//         p         - reference to a pointer to a Node. On input, the
//                     referenced pointer points to the current node whose
//                     right subtree has been reduced in height.
//
//     Output:
//
//         p         - on output, the referenced pointer is updated to reflect
//                     any rotations that may have occurred.
//         hReduced  - reference to a boolean. The referenced boolean is set
//                     to FALSE if the height of the subtree rooted at p was
//                     not reduced; otherwise, the value is left unchanged.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void XOrderedTree::BalanceRight(Node *&p, BOOL &hReduced)
{
    // only need to update balFactor if it is 1 or 0

    if (p->m_balFactor == 1)
        p->m_balFactor = 0;
    else if (p->m_balFactor == 0) {
        p->m_balFactor = -1;
        hReduced = FALSE;
    }

    // else need to do a rotation

    else {
        Node *p1, *p2;
        short b1, b2;

        p1 = p->m_left; b1 = p1->m_balFactor;

        // if balance factor of left subtree is <= 0 then need to do a
        // single left-left rotation, and re-parent affected nodes

        if (b1 <= 0) {
            p->m_left = p1->m_right; p1->m_right = p;
            ReparentNodes(p, p1, p->m_left);

            if (b1 == 0) {
                p->m_balFactor = -1;
                p1->m_balFactor = 1;
                hReduced = FALSE;
            }
            else {
                p->m_balFactor = 0;
                p1->m_balFactor = 0;
            }
            p = p1;
        }

        // else need to do a double left-right rotation, and re-parent
        // affected nodes

        else {
            p2 = p1->m_right; b2 = p2->m_balFactor;
            p1->m_right = p2->m_left; p2->m_left = p1;
            p->m_left = p2->m_right; p2->m_right = p;
            ReparentNodes(p, p1, p2, p1->m_right, p->m_left);
            p->m_balFactor = static_cast<short>(b2 == -1 ? 1 : 0);
            p1->m_balFactor = static_cast<short>(b2 == 1 ? -1 : 0);
            p = p2;
            p2->m_balFactor = 0;
        }
    }

} // end: BalanceRight()


//------------------------------------------------------------------------------
//
// METHOD: ReparentNodes().
//
// PURPOSE: Reparent the specified nodes after a left-left or right-right single
//          rotation.
//
// PARAMETERS:
//
//     Input:
//
//         p, p1, q - pass pointers to the nodes involved in the reparenting
//             operation. On input, p is the pivot node about which the rotation
//             occurred,  p1 is the node rotated into the position previously
//             occupied by p, and q depends on the type of rotation performed.
//
//     Output:
//
//         The parent pointers of the nodes are updated to reflect the rotation.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void XOrderedTree::ReparentNodes(Node *p, Node *p1, Node *q)
{
    p1->m_parent = p->m_parent;
    p->m_parent = p1;
    if (q != NULL)
        q->m_parent = p;

} // end: ReparentNodes()


//------------------------------------------------------------------------------
//
// METHOD: ReparentNodes().
//
// PURPOSE: Reparent the specified nodes after a left-right or right-left double
//          rotation.
//
// PARAMETERS:
//
//     Input:
//
//         p, p1, p2, q1, q2 - pass pointers to the nodes involved in the
//             reparenting operation. On input, p is the pivot node about
//             which the double rotation occurred, and p1, p2, q1, and q2
//             depend on the type of rotation that was performed.
//
//     Output:
//
//         The parent pointers of the nodes are updated to reflect the rotation.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void XOrderedTree::ReparentNodes(Node *p, Node *p1, Node *p2, Node *q1, Node *q2)
{
    p2->m_parent = p->m_parent;
    p1->m_parent = p2;
    p->m_parent = p2;
    if (q1 != NULL)
        q1->m_parent = p1;
    if (q2 != NULL)
        q2->m_parent = p;

} // end: ReparentNodes()


//------------------------------------------------------------------------------
//
// METHOD: Exchange().
//
// PURPOSE: Exchange edge and Above(edge) in the current order.
//
// PARAMETERS:
//
//     Input:
//
//         edge - passes a pointer to the SweepEdge that is to be exhanged
//                with the one above it.
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

void XOrderedTree::Exchange(SweepEdge *edge)
{
    Node *aboveNode = edge->GetXotNode()->m_above;
    SweepEdge *aboveEdge = aboveNode->m_edge;

    aboveNode->m_edge = edge;
    edge->GetXotNode()->m_edge = aboveEdge;

    aboveEdge->SetXotNode(edge->GetXotNode());
    edge->SetXotNode(aboveNode);

} // end: Exchange()


//------------------------------------------------------------------------------
//
// METHOD: Above().
//
// PURPOSE: Determine which edge is above the specified edge in the current
//          X-order.
//
// PARAMETERS:
//
//     Input:
//
//         edge - passes a pointer to the edge, that the 'above' edge is to
//                be determined for.
//
//     Output:
//
//         None.
//
// RETURNS: Pointer to the edge that is above the specified edge in the current
//          X-order. If there is no edge above the specified edge, then NULL is
//          returned (from aboveSentinel node).
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

SweepEdge *XOrderedTree::Above(const SweepEdge *edge) const
{
    return edge->GetXotNode()->m_above->m_edge;

} // end: Above()


//------------------------------------------------------------------------------
//
// METHOD: Below().
//
// PURPOSE: Determine which edge is below the specified edge in the current
//          X-order.
//
// PARAMETERS:
//
//     Input:
//
//         edge - passes a pointer to the edge, that the 'below' edge is to
//                be determined for.
//
//     Output:
//
// RETURNS: Pointer to the edge that is below the specified edge in the current
//          X-order. If there is no edge above the specified edge, then NULL is
//          returned (from belowSentinel node).
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

SweepEdge *XOrderedTree::Below(const SweepEdge *edge) const
{
    return edge->GetXotNode()->m_below->m_edge;

} // end: Below()


//------------------------------------------------------------------------------
//
// Implementation of the XOrderedTree::NodeAllocator class.
//
//------------------------------------------------------------------------------

static const int OrderedTreeNodeBlockSize = 256;

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Initialize an instance of the XOrderedTree::NodeAllocator class.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

XOrderedTree::NodeAllocator::NodeAllocator() :
    BufferAllocator<Node>(OrderedTreeNodeBlockSize),
    m_headFreeList(NULL)
{
} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Destroy an instance of the XOrderedTree::NodeAllocator class,
//          releasing all dynamically allocated resources.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

XOrderedTree::NodeAllocator::~NodeAllocator()
{
} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: Allocate().
//
// PURPOSE: Get the next available free node in the node pool, reallocating
//          the pool to a larger size if it has been exhausted.
//
// PARAMETERS:
//
//     Input:
//
//         edge   - passes a pointer to the SweepEdge that is to be stored at
//                  the node.
//         parent - passes a pointer to the parent node.
//         below  - passes a pointer to the node that is immediately below the
//                  new node in the current X-order.
//         above  - passes a pointer to the node that is immediately above the
//                  new node in the current X-order.
//
//     Output:
//
//         None.
//
// RETURNS: Returns a pointer to the new node.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to expand the node pool.
//
//------------------------------------------------------------------------------

XOrderedTree::Node *XOrderedTree::NodeAllocator::Allocate(SweepEdge *edge,
    Node *parent, Node *below, Node *above)
{
    // first check the linked list of free nodes. If the list is not empty,
    // then remove the head of the list

    Node *freeNode;

    if (m_headFreeList != NULL) {
        freeNode = m_headFreeList;
        m_headFreeList = m_headFreeList->m_left;
    }

    // else allocate a new node using services of the subclass

    else
        freeNode = GetNextObject();

    // initialize the node from the specified parameters and return

    freeNode->m_edge = edge;

    if (edge)
        edge->SetXotNode(freeNode);

    freeNode->m_balFactor = 0;
    freeNode->m_parent = parent;
    freeNode->m_left = freeNode->m_right = NULL;
    freeNode->m_below = below;
    freeNode->m_above = above;

    return freeNode;

} // end: Allocate()


//------------------------------------------------------------------------------
//
// METHOD: Free().
//
// PURPOSE: Free the specified XOrderedTree node, placing it back in the pool of
//          available nodes.
//
// PARAMETERS:
//
//     Input:
//
//         node - pointer to the node to be freed. The node pointed to is
//                assumed to have been obtained in a prior call to the
//                Allocate() method.
//
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

void XOrderedTree::NodeAllocator::Free(Node *node)
{
    // add to head of linked list of free nodes

    node->m_left = m_headFreeList;
    m_headFreeList = node;

} // end: Free()
