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

//-----------------------------------------------------------------------------
//
// FILE: ortree.h.
//
// PURPOSE: Declaration of the OpsRTree class. The OpsRTree class implements
//          the basic functionality of a 3-4-5 memory resident R-Tree (insert-
//          ion, deletion, and searching). As such, additional functionality is
//          expected to be added via (private) inheritance. For example, the
//          OpsPolygonRTree derives from OpsRTree adding additional function-
//          ality required to support lasso pick operations.
//
//-----------------------------------------------------------------------------

#ifndef _ORTREE_H_
#define _ORTREE_H_

#include <float.h>


class OpsRTree : public OpsObject {
public:
    /// constructor/destructor

   OpsRTree();
   virtual ~OpsRTree();

    /// methods to insert and delete items from the R-Tree

    void Insert(const void *item, const OpsFloatExtent *itemExtent);
    BOOL Delete(const void *item, const OpsFloatExtent *itemExtent);

    /// methods to initialize a search of the R-Tree, and return pointers
    /// to items that intersect the search area

    void BeginSearch(const OpsFloatExtent *searchExt);
    const void *GetNextItem();

    /// classes and method to override the standard search behaviour (the
    /// IntersectsSearchPredicate implements the standard search behaviour,
    /// returning all items that intersect a search extent - it is implicitly
    /// used by the BeginSearch(const OpsFloatExtent *))

    class SearchPredicate {
    public:
        virtual BOOL Predicate(const OpsFloatExtent &itemExtent) const = 0;
    };

    class IntersectsSearchExtent : public SearchPredicate {
    public:
        void Initialize(const OpsFloatExtent &searchExtent);
        virtual BOOL Predicate(const OpsFloatExtent &nodeExtent) const;
    private:
        OpsFloatExtent m_searchExtent;
    };

    void BeginSearch(const SearchPredicate *predicate);

    /// method to (re)initialize the R-Tree to an empty state

    void Initialize();

    /// method to get the extent of all items inserted into the R-Tree

    OpsFloatExtent *GetExtent(OpsFloatExtent *extent) const;

protected:
    /// methods to find the minimum/maximum of two floats.

    float Min(float x1, float x2) const;
    float Max(float x1, float x2) const;

#if !defined(TESTAPP_ACCESS)  // test app given protected access to members
private:
#endif
    /// declaration of nested classes used in the R-Tree implementation

    struct ItemContainer {
        const void *m_item;           // pointer to the item
        OpsFloatExtent m_itemExtent;  // extent of the item
    };

    struct RTreeNode {
        enum {MaxChildren = 5, MinChildren = 3};

        int m_nodeLevel;              // level of the node
        OpsFloatExtent m_nodeExtent;  // node extent
        void *m_child[MaxChildren];   // pointers to child nodes/items
        int m_subTree;                // used to save search state

        void Initialize(int level);
        int GetNChildren() const;
        RTreeNode *GetChildNode(int index) const;
        ItemContainer *GetChildItem(int index) const;
        const OpsFloatExtent *GetChildExtent(int index) const;
        BOOL IsNodeFull() const;
        BOOL IsaLeafNode() const;
        void AddChild(void *child);
        void RemoveChild(int index);
        void RecomputeExtent();
        int GetChildIndex(void *childPtr) const;
    };

    template <class ObjType > class Allocator {
    public:
        Allocator(int blockSize);
        ~Allocator();

        void Initialize();

        ObjType *Allocate();
        void Free(ObjType *obj);

    private:
        ObjType **m_blockPool;      // pointers to blocks of objects
        int m_blockSize;            // size of blocks to allocate
        int m_nBlocks;              // number of blocks
        int m_maxBlocks;            // current size of m_blockPool
        int m_nextObjIndex;         // index of next free object
        ObjType *m_headFreeList;    // pointer to head of free list
        enum {PoolBlockSize = 16};  // size to grow m_blockPool by

        void InitializeMembers(int blockSize);
        void UnInitialize();
        void GetNewBlock();
    };

    typedef Allocator<RTreeNode> NodeAllocator;
    typedef Allocator<ItemContainer> ContainerAllocator;

    class NodePtrStack {
    public:
        NodePtrStack();
        ~NodePtrStack();

        void Push(RTreeNode *nodePtr);
        void Pop();
        RTreeNode *Top() const;
        RTreeNode *GetNodePtrAt(int index) const;
        void Reset();
        BOOL IsEmpty() const;
        int GetSize() const;

    private:
        int m_topOfStack;                // index of top node pointer
        RTreeNode **m_nodePtrArray;      // array of node pointers
        enum {NodePtrArraySize = 64};    // size of node pointer array
    };

    /// the following are needed so that the nested classes can reference
    /// one another

    friend struct RTreeNode;
    friend class Allocator<ItemContainer>;
    friend class NodePtrStack;
    friend class SearchStateStack;
    friend class Allocator<RTreeNode>;

    /// class members

    int m_rTreeHeight;                          // current height of the R-Tree
    RTreeNode *m_rootNode;                      // pointer to the root node
    NodePtrStack m_nodePtrStack;                // stack of pointers to nodes
    NodeAllocator m_nodeAllocator;              // node allocator
    ContainerAllocator m_containerAllocator;    // container allocator
    RTreeNode **m_eliminatedNodes;              // used by deletion methods
    const SearchPredicate *m_predicate;         // current search predicate
    IntersectsSearchExtent m_standardPredicate; // standard search predicate

    /// implementation methods

    void Insert(void *item, const OpsFloatExtent *itemExtent, int level);
    RTreeNode *ChooseNode(const OpsFloatExtent *itemExtent, int level);
    double GetNonZeroArea(double width, double height) const;
    int ChooseBestSubTree(const RTreeNode *node,
        const OpsFloatExtent *itemExtent) const;
    RTreeNode *SplitNode(RTreeNode *fullNode, void *childItem,
        const OpsFloatExtent *childExtent);
    const int *ChooseBestPartition(const OpsFloatExtent * const inExtents[5],
        OpsFloatExtent *outExtent1, OpsFloatExtent *outExtent2);
    void AdjustRTree(RTreeNode *newNode);
    int FindLeafNode(const void *item, const OpsFloatExtent *itemExtent);
    void CondenseRTree(int itemIndex);

    /// private to prevent access

    OpsRTree(const OpsRTree &);
    OpsRTree &operator=(const OpsRTree &);
};


//------------------------------------------------------------------------------
//
// METHOD: Min().
//
// PURPOSE: Determine the minimum of two floats.
//
// PARAMETERS:
//
//     Input:
//
//         x1, x2 - pass the two floats to be compared.
//
//     Output:
//
//         None.
//
// RETURNS: The minimum of x1 and x2.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline float OpsRTree::Min(float x1, float x2) const
{
    return x1 <= x2 ? x1 : x2;

} // end: Min()


//------------------------------------------------------------------------------
//
// METHOD: Max().
//
// PURPOSE: Determine the maximum of two floats.
//
// PARAMETERS:
//
//     Input:
//
//         x1, x2 - pass the two floats to be compared.
//
//     Output:
//
//         None.
//
// RETURNS: The maximum of x1 and x2.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline float OpsRTree::Max(float x1, float x2) const
{
     return x1 <= x2 ? x2 : x1;

} // end: Max()


//------------------------------------------------------------------------------
//
// METHOD: Initialize().
//
// PURPOSE: Initialize the node to an empty state.
//
// PARAMETERS:
//
//     Input:
//
//         level - passes the level of the node. Node levels are measured with
//                 respect to the R-Tree leaf nodes, with a leaf node being at
//                 level 0. Levels are used rather than heights since the latter
//                 change as the tree grows or shrinks in height.
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

inline void OpsRTree::RTreeNode::Initialize(int level)
{
    m_nodeLevel = level;

    for (int i = 0; i < MaxChildren; i++)
         m_child[i] = NULL;

    m_subTree = 0;

} // end: Initialize()


//------------------------------------------------------------------------------
//
// METHOD: GetChildNode().
//
// PURPOSE: Get a pointer to the specified child node.
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index of the child node pointer.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the specified child node.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline OpsRTree::RTreeNode *OpsRTree::RTreeNode::GetChildNode(int index) const
{
    /// check that index is in bounds, and that it is not a leaf node

    assert(index >= 0);
    assert(index < MaxChildren);
    assert(!IsaLeafNode());

    return reinterpret_cast<RTreeNode *>(m_child[index]);

} // end: GetChildNode()


//------------------------------------------------------------------------------
//
// METHOD: GetChildItem().
//
// PURPOSE: Get a pointer to the specified child item container.
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index of the child item container.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the specified child item container.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline OpsRTree::ItemContainer *OpsRTree::RTreeNode::GetChildItem(int index)
    const
{
    /// check that index is in bounds, and that it is a leaf node

    assert(index >= 0);
    assert(index < MaxChildren);
    assert(IsaLeafNode());

    return reinterpret_cast<ItemContainer *>(m_child[index]);

} // end: GetChildItem()


//------------------------------------------------------------------------------
//
// METHOD: GetChildExtent().
//
// PURPOSE: Get the extent of the specified child of the node.
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index of the child.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the child extent.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline const OpsFloatExtent *OpsRTree::RTreeNode::GetChildExtent(int index)
    const
{
    /// check that index is in bounds

    assert(index >= 0);
    assert(index < GetNChildren());

    if (IsaLeafNode())
        return &GetChildItem(index)->m_itemExtent;
    else
        return &GetChildNode(index)->m_nodeExtent;

} // end: GetChildExtent()


//------------------------------------------------------------------------------
//
// METHOD: IsNodeFull().
//
// PURPOSE: Determine if the node is full.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: TRUE if the node is full and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline BOOL OpsRTree::RTreeNode::IsNodeFull() const
{
    return m_child[MaxChildren-1] != NULL;

} // end: IsNodeFull()


//------------------------------------------------------------------------------
//
// METHOD: IsaLeafNode().
//
// PURPOSE: Determine if the node is a leaf node.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: TRUE if the node is a leaf and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline BOOL OpsRTree::RTreeNode::IsaLeafNode() const
{
    return m_nodeLevel == 0;

} // end: IsaLeafNode()


//------------------------------------------------------------------------------
//
// METHOD: Push().
//
// PURPOSE: Push a pointer to a node onto the stack.
//
// PARAMETERS:
//
//     Input:
//
//         nodePtr - passes a pointer to the node to be pushed.
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

inline void OpsRTree::NodePtrStack::Push(RTreeNode *nodePtr)
{
    assert(m_topOfStack < NodePtrArraySize - 1);
    m_nodePtrArray[++m_topOfStack] = nodePtr;

} // end: Push()


//------------------------------------------------------------------------------
//
// METHOD: Pop().
//
// PURPOSE: Pop the node pointer that is on top of the stack.
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

inline void OpsRTree::NodePtrStack::Pop()
{
    assert(m_topOfStack >= 0);
    m_topOfStack--;

} // end Pop()


//------------------------------------------------------------------------------
//
// METHOD: Top().
//
// PURPOSE: Get the node pointer that is on top of the stack.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: Returns the pointer that is on top of the stack (NULL if the stack
//          is empty).
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline OpsRTree::RTreeNode *OpsRTree::NodePtrStack::Top() const
{
    if (m_topOfStack >= 0)
        return m_nodePtrArray[m_topOfStack];
    else
        return NULL;

} // end: Top()


//------------------------------------------------------------------------------
//
// METHOD: GetNodePtrAt().
//
// PURPOSE: Get the node pointer that is at the indexed location.
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index of the node pointer (must be bewteen 0 and
//                 GetSize() - 1).
//
//     Output:
//
//         None.
//
// RETURNS: Returns the indexed node pointer (NULL if there is no such pointer).
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline OpsRTree::RTreeNode *OpsRTree::NodePtrStack::GetNodePtrAt(int index)
    const
{
    assert(index >= 0 && index <= m_topOfStack);

    if (index >= 0 && index <= m_topOfStack)
        return m_nodePtrArray[index];
    else
        return NULL;

} // end: GetNodePtrAt()


//------------------------------------------------------------------------------
//
// METHOD: IsEmpty().
//
// PURPOSE: Determine whether or not the stack is empty.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: TRUE if the stack is empty, and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline BOOL OpsRTree::NodePtrStack::IsEmpty() const
{
    return m_topOfStack < 0;

} // end: IsEmpty()


//------------------------------------------------------------------------------
//
// METHOD: GetNonZeroArea().
//
// PURPOSE: Compute the area of a rectangle, subject to the constraint that
//          the computed area be nonzero. For rectangles with zero width or
//          height, the perimeter is returned.
//
// PARAMETERS:
//
//     Input:
//
//         width  - passes the width of the rectangle.
//         height - passes the height of the rectangle.
//
//     Output:
//
//         None.
//
// RETURNS: Returns the area of the rectangle.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline double OpsRTree::GetNonZeroArea(double width, double height) const
{
    double area = width * height;

    if (area == 0.0) {
        if (width == 0.0)
            area = 2.0 * height;
        else
            area = 2.0 * width;

        if (area == 0.0)
            area = FLT_MIN;
    }

    return area;

} // end: GetNonZeroArea()

#endif
