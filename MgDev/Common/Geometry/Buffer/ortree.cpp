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
// FILE: ortree.cpp.
//
// PURPOSE: Implementation of the OpsRTree class. This source file implements
//          a 3-4-5 memory resident variant of the R-Tree data type. The R-Tree
//          was originally described in:
//
//          Guttman A., "R-Trees: A Dynamic Index Structure For Spatial
//          Searching", ACM SIGMOD Conference On Management Of Data, Boston,
//          June 1984.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"
#include <math.h>

//------------------------------------------------------------------------------
//
// Implementation of the OpsRTree::Allocator<> class template.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Initialize an instance of the OpsRTree::Allocator<> class.
//
// PARAMETERS:
//
//     Input:
//
//         blockSize - passes the size of the blocks to allocate.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template <class ObjType> OpsRTree::Allocator<ObjType>::Allocator(int blockSize)
{
    InitializeMembers(blockSize);

} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Destroy an instance of the OpsRTree::Allocator class, releasing all
//          dynamically allocated resources.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template <class ObjType> OpsRTree::Allocator<ObjType>::~Allocator()
{
    UnInitialize();

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: Allocate().
//
// PURPOSE: Return the next available free object in the object pool, reallocat-
//          ing the pool to a larger size if it has been exhausted.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: Returns a pointer to the new object.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to expand the object pool.
//
//------------------------------------------------------------------------------

template <class ObjType> ObjType *OpsRTree::Allocator<ObjType>::Allocate()
{
    ObjType *obj;

    // first check the linked list of free objects. If the list is not empty,
    // then remove the head of the list

    if (m_headFreeList != NULL) {
        obj = m_headFreeList;
        m_headFreeList = *reinterpret_cast<ObjType **>(m_headFreeList);
    }

    // else get the next free object from the object pool

    else {
        // get a new block of object if necessary

        if (m_nextObjIndex >= m_blockSize)
            GetNewBlock();

        obj = &m_blockPool[m_nBlocks-1][m_nextObjIndex++];
    }

    return obj;

} // end: Allocate()


//------------------------------------------------------------------------------
//
// METHOD: Free().
//
// PURPOSE: Free the specified object, placing it back in the pool of available
//          objects.
//
// PARAMETERS:
//
//     Input:
//
//         obj - pointer to the object. The object pointed to is assumed to have
//               been obtained in a prior call to the Allocate() method.
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

template <class ObjType> void OpsRTree::Allocator<ObjType>::Free(ObjType *obj)
{
    // add to head of linked list of free objects

    *reinterpret_cast<ObjType **>(obj) = m_headFreeList;
    m_headFreeList = obj;

} // end: Free()


//------------------------------------------------------------------------------
//
// METHOD: Initialize().
//
// PURPOSE: Initialize (or reinitialize) the allocator.
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

template <class ObjType> void OpsRTree::Allocator<ObjType>::Initialize()
{
    // free up any previously allocated blocks of objects, and reinitialize
    // member variables

    UnInitialize();
    InitializeMembers(m_blockSize);

} // end: Initialize()


//------------------------------------------------------------------------------
//
// METHOD: InitializeMembers().
//
// PURPOSE: Initialize member variables to their default values.
//
//     Input:
//
//         blockSize - passes the size of the blocks to allocate.
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

template <class ObjType> void OpsRTree::Allocator<ObjType>::InitializeMembers(
    int blockSize)
{
    m_blockPool = NULL;
    m_blockSize = blockSize;
    m_nBlocks = 0;
    m_maxBlocks = 0;
    m_nextObjIndex = m_blockSize;
    m_headFreeList = NULL;

} // end: InitializeMembers()


//------------------------------------------------------------------------------
//
// METHOD: UnInitialize().
//
// PURPOSE: Free up all previously allocated blocks of objects.
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

template <class ObjType> void OpsRTree::Allocator<ObjType>::UnInitialize()
{
    if (m_nBlocks > 0) {
        for (int i = 0; i < m_nBlocks; i++)
            OpsObject::FreeMem(m_blockPool[i]);

        OpsObject::FreeMem(m_blockPool);
    }

} // end: UnInitialize()


//------------------------------------------------------------------------------
//
// METHOD: GetNewBlock().
//
// PURPOSE: Internal method used to allocate a new block of objects.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to allocate the new block of objects.
//
//------------------------------------------------------------------------------

template <class ObjType> void OpsRTree::Allocator<ObjType>::GetNewBlock()
{
    // reallocate the pool of pointers to blocks to a larger size if necessary
    // by adding PoolBlockSize more entries to it

    size_t nBytes;

    if (m_nBlocks >= m_maxBlocks) {
        size_t nBytes = (m_maxBlocks + PoolBlockSize) * sizeof(ObjType *);
        m_blockPool = reinterpret_cast<ObjType **>(
            OpsObject::ReallocMem(m_blockPool, nBytes));
        m_maxBlocks += PoolBlockSize;
    }

    // allocate the new block of objects

    nBytes = m_blockSize * sizeof(ObjType);
    m_blockPool[m_nBlocks] = reinterpret_cast<ObjType *>(
        OpsObject::AllocMem(nBytes));
    m_nBlocks++;
    m_nextObjIndex = 0;

} // end: GetNewBlock()


//------------------------------------------------------------------------------
//
// Implementation of the OpsRTree class.
//
//------------------------------------------------------------------------------

static const int TreeNodeBlockSize = 2048;
static const int ContainerBlockSize = 1024;
static const int MaxEliminatedNodes = 64;

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Create and initialize an instance of the OpsRTree class.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory available to construct and initialize the object.
//
//------------------------------------------------------------------------------

OpsRTree::OpsRTree() :
    m_rTreeHeight(0),
    m_rootNode(NULL),
    m_nodeAllocator(TreeNodeBlockSize),
    m_containerAllocator(ContainerBlockSize),
    m_predicate(NULL)
{
    m_eliminatedNodes = new RTreeNode*[MaxEliminatedNodes];

} // end: Constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Release all dynamically acquired resources.
//
//------------------------------------------------------------------------------

OpsRTree::~OpsRTree()
{
    delete [] m_eliminatedNodes;

} // end: Destructor


//------------------------------------------------------------------------------
//
// METHOD: Insert().
//
// PURPOSE: Insert an item into the R-Tree.
//
// PARAMETERS:
//
//     Input:
//
//         item       - pointer to the item to be inserted.
//         itemExtent - pointer to an OpsFloatExtent structure containing the
//                      coordinates of the item extent.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to insert the item.
//
//------------------------------------------------------------------------------

void OpsRTree::Insert(const void *item, const OpsFloatExtent *itemExtent)
{
    // allocate an item container, and initialize

    ItemContainer *itemContainer = m_containerAllocator.Allocate();
    itemContainer->m_item = item;
    itemContainer->m_itemExtent = *itemExtent;

    // if the R-Tree is empty, then allocate a new root node, and insert the
    // item directly

    if (m_rootNode == NULL) {
        m_rootNode = m_nodeAllocator.Allocate();
        m_rootNode->Initialize(0);
        m_rootNode->m_child[0] = itemContainer;
        m_rootNode->m_nodeExtent = *itemExtent;
        m_rTreeHeight = 1;
    }

    // else call internal method to perform the insertion at the leaf level

    else
        Insert(itemContainer, itemExtent, 0);

} // end: Insert()


//------------------------------------------------------------------------------
//
// METHOD: Delete().
//
// PURPOSE: Delete an item from the R-Tree.
//
// PARAMETERS:
//
//     Input:
//
//         item       - pointer to the item to be deleted.
//         itemExtent - pointer to an OpsFloatExtent structure containing the
//                      coorindates of the item extent.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the item was located and deleted, and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OpsRTree::Delete(const void *item, const OpsFloatExtent *itemExtent)
{
    // find the leaf node containing the item, and the index of the item within
    // the leaf node

    int itemIndex = FindLeafNode(item, itemExtent);

    // if the item was located, then call CondenseRTree() to remove it from the
    // leaf node, and patch the R-Tree on the search path from the root

    if (itemIndex >= 0) {
        CondenseRTree(itemIndex);
        return TRUE;
    }
    else
        return FALSE;

} // end: Delete()


//------------------------------------------------------------------------------
//
// METHOD: BeginSearch().
//
// PURPOSE: Initialize a search of the R-Tree. Items intersecting the specified
//          search area can be iteratively retrieved by calling GetNextItem().
//
// PARAMETERS:
//
//     Input:
//
//         searchExt - pointer to an OpsFloatExtent structure containing the
//                     coorindates of the search extent.
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

void OpsRTree::BeginSearch(const OpsFloatExtent *searchExt)
{
    // intialize the standard precidate with the search extent, and
    // call the overloaded version of BeginSearch() with the standard
    // predicate

    m_standardPredicate.Initialize(*searchExt);
    BeginSearch(&m_standardPredicate);

} // end: BeginSearch()


//------------------------------------------------------------------------------
//
// METHOD: GetNextItem().
//
// PURPOSE: Locate and return a pointer to the next item that intersects
//          the search area established by a call to BeginSearch().
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A pointer to the next item or NULL if no more items were found to
//          intersect the search extent.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

const void *OpsRTree::GetNextItem()
{
    // safety check to ensure a search predicate has been specified

    assert(m_predicate != NULL);

    if (m_predicate == NULL)
        m_predicate = &m_standardPredicate;

    // descend into all subtrees whose extents intersect the search extent

    while (!m_nodePtrStack.IsEmpty()) {
        RTreeNode *node = m_nodePtrStack.Top();
        BOOL descending = FALSE;

        // if at the leaf level, then return the first child item whose extent
        // satisfies the search predicate

        if (node->IsaLeafNode()) {
            for (int i = node->m_subTree; i < RTreeNode::MaxChildren; i++) {
                ItemContainer *container = node->GetChildItem(i);

                if (container == NULL)
                    break;
                else if (m_predicate->Predicate(container->m_itemExtent)) {
                    node->m_subTree = i + 1;
                    return container->m_item;
                }
            }
        }

        // else must be at an internal node - descend into the first subtree
        // satisfies the search predicate

        else {
            for (int i = node->m_subTree; i < RTreeNode::MaxChildren; i++) {
                RTreeNode *childNode = node->GetChildNode(i);

                if (childNode == NULL)
                    break;
                else if (m_predicate->Predicate(childNode->m_nodeExtent)) {
                    node->m_subTree = i + 1;
                    childNode->m_subTree = 0;
                    m_nodePtrStack.Push(childNode);
                    descending = TRUE;
                    break;
                }
            }
        }

        // if no possibility for descent from the current node, then pop it

        if (!descending)
            m_nodePtrStack.Pop();
    }

    // no more items satisfying the search predicate, set the search predicate
    // to NULL, and return NULL

    m_predicate = NULL;
    return NULL;

} // end: GetNextItem()


//------------------------------------------------------------------------------
//
// METHOD: BeginSearch().
//
// PURPOSE: Initialize a search of the R-Tree based on the specified search
//          predicate. Items that satisfu the specified predicate can be
//          iteratively retrieved by calling GetNextItem().
//
// PARAMETERS:
//
//     Input:
//
//         searchExt - pointer to an OpsFloatExtent structure containing the
//                     coorindates of the search extent.
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

void OpsRTree::BeginSearch(const SearchPredicate *predicate)
{
    // set the search predicate, and push the root node onto the stack

    assert(predicate != NULL);
    m_predicate = predicate;

    m_nodePtrStack.Reset();
    if (m_rootNode != NULL && predicate->Predicate(m_rootNode->m_nodeExtent)) {
        m_rootNode->m_subTree = 0;
        m_nodePtrStack.Push(m_rootNode);
    }

} // end: BeginSearch()


//------------------------------------------------------------------------------
//
// METHOD: Initialize().
//
// PURPOSE: Initialize (reinitialize) the R-Tree.
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

void OpsRTree::Initialize()
{
    m_rTreeHeight = 0;
    m_rootNode = NULL;
    m_nodePtrStack.Reset();
    m_nodeAllocator.Initialize();
    m_containerAllocator.Initialize();

} // end: Initialize()


//------------------------------------------------------------------------------
//
// METHOD: GetExtent.
//
// PURPOSE: Returns the extent of all items that have been inserted into the
//          R-Tree.
//
// PARAMETERS:
//
//     Input:
//
//         None.
//
//     Output:
//
//         extent - pointer to an OpsFloatExtent structure; the extent of the
//                  R-Tree is copied to the location pointed to.
//
// RETURNS: Returns the output parameter 'extent'.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsFloatExtent *OpsRTree::GetExtent(OpsFloatExtent *extent) const
{
    if (m_rootNode != NULL)
        *extent = m_rootNode->m_nodeExtent;
    else
        extent->xMin = extent->xMax = extent->yMin = extent->yMax = 0.0f;

    return extent;

} // end: GetExtent()


//------------------------------------------------------------------------------
//
// METHOD: Insert().
//
// PURPOSE: Insert an item into the R-Tree at the specified level.
//
// PARAMETERS:
//
//     Input:
//
//         item       - pointer to the item to be inserted.
//         itemExtent - pointer to an OpsFloatExtent containing the extent
//                      of the item.
//         level      - an integer specifying the level in the R-Tree where
//                      the item is to be inserted.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to insert the item.
//
//------------------------------------------------------------------------------

void OpsRTree::Insert(void *item, const OpsFloatExtent *itemExtent, int level)
{
    // choose the best node at the specified level to insert into

    RTreeNode *node = ChooseNode(itemExtent, level);

    // add the item to the node, splitting it, if it is already full

    RTreeNode *newNode;

    if (node->IsNodeFull())
        newNode = SplitNode(node, item, itemExtent);
    else {
        newNode = NULL;
        node->AddChild(item);
        node->m_nodeExtent.UnionWith(itemExtent);
    }

    // adjust the nodes at the search path from the root

    AdjustRTree(newNode);

} // end: Insert()


//------------------------------------------------------------------------------
//
// METHOD: ChooseNode().
//
// PURPOSE: Choose the best node (at the specified level) for insertion. On
//          return, the node pointer stack contains the nodes on the path from
//          the root to the node selected for insertion.
//
// PARAMETERS:
//
//     Input:
//
//         itemExtent - pointer to an OpsFloatExtent containing the extent
//                      of the item that is to be inserted.
//         level      - an integer specifying the level in the R-Tree where
//                      the item is to be inserted.
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

OpsRTree::RTreeNode *OpsRTree::ChooseNode(const OpsFloatExtent *itemExtent,
    int level)
{
    // push the root node onto the stack, and then iteratively descend into
    // the R-Tree looking for the best node to insert into

    m_nodePtrStack.Reset();
    assert(m_rootNode != NULL);
    m_nodePtrStack.Push(m_rootNode);

    for (int nodeHeight = 1; m_rTreeHeight-nodeHeight != level; nodeHeight++) {
        RTreeNode *topNode = m_nodePtrStack.Top();
        int subtreeIndex = ChooseBestSubTree(topNode, itemExtent);
        m_nodePtrStack.Push(topNode->GetChildNode(subtreeIndex));
    }

    return m_nodePtrStack.Top();

} // end: ChooseNode()



//------------------------------------------------------------------------------
//
// METHOD: ChooseBestSubtree().
//
// PURPOSE: Given an R-Tree node, choose the best subtree of this node
//          to insert a new item into; the subtree chosen is the one whose
//          extent will require the least amount of areal enlargement.
//
// PARAMETERS:
//
//     Input:
//
//         node       - pointer to the R-Tree node the subtree will be
//                      selected from (this node must NOT be a leaf node).
//         itemExtent - passes the extent of the item.
//
//     Output:
//
//         None.
//
// RETURNS: Returns the index of the subtree node whose extent will require
//          the least amount of enlargement as a result of insertion of the
//          item.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OpsRTree::ChooseBestSubTree(const RTreeNode *node, const OpsFloatExtent *itemExtent) const
{
    // must not be a leaf node

    assert(!node->IsaLeafNode());

    // initialize ...

    int subTreeIndex = 0;
    double nodeExtentArea = HUGE_VAL;
    double minAreaIncrease = HUGE_VAL;

    // check each child looking for the one whose extent will require
    // the least amount of enlargement

    for (int i = 0; i < RTreeNode::MaxChildren && node->m_child[i] != NULL; i++) {
        const OpsFloatExtent &nodeExtent = node->GetChildNode(i)->m_nodeExtent;

        double width = Max(nodeExtent.xMax, itemExtent->xMax) -
                       Min(nodeExtent.xMin, itemExtent->xMin);

        double height = Max(nodeExtent.yMax, itemExtent->yMax) -
                        Min(nodeExtent.yMin, itemExtent->yMin);

        double newExtentArea = GetNonZeroArea(width, height);

        width = nodeExtent.Width();
        height = nodeExtent.Height();

        double oldExtentArea = GetNonZeroArea(width, height);

        double areaIncrease = newExtentArea - oldExtentArea;

        if (areaIncrease < minAreaIncrease ||
           (areaIncrease == minAreaIncrease && oldExtentArea<nodeExtentArea)) {
            minAreaIncrease = areaIncrease;
            nodeExtentArea = oldExtentArea;
            subTreeIndex = i;
        }
    }

    return subTreeIndex;

} // end: ChooseBestSubTree()


//------------------------------------------------------------------------------
//
// METHOD: SplitNode().
//
// PURPOSE: Split a node by grouping the children (5 from the full node plus the
//          new child) into two nodes in such a way that the sum of the areas
//          for the extents of the two new nodes is minimized.
//
// PARAMETERS:
//
//     Input:
//
//         fullNode    - passes a pointer to the node that needs to be split.
//         childItem   - passes a pointer to the child item that caused over-
//                       flow.
//         childExtent - passes a pointer to an OpsFloatExtent structure that
//                       contains the coordinates of the extent of the child
//                       item.
//
//     Output:
//
//         fullNode    - the entries in fullNode are updated upon return as a
//                       result of the node split.
//
// RETURNS: Returns a pointer to the new node resulting from the split.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory to
//             allocate the new node.
//
//------------------------------------------------------------------------------

OpsRTree::RTreeNode *OpsRTree::SplitNode(RTreeNode *fullNode, void *childItem,
    const OpsFloatExtent *childExtent)
{
     // allocate a new node

    RTreeNode *newNode = m_nodeAllocator.Allocate();
    newNode->Initialize(fullNode->m_nodeLevel);

    // make sure the maximum number of children an R-Tree node can have
    // is 5 - the method assumes this to be the case

    assert(RTreeNode::MaxChildren == 5);

    // set up array of child extents, and save an array of pointers to the
    // children

    const OpsFloatExtent *childExtents[RTreeNode::MaxChildren + 1];

    childExtents[0] = fullNode->GetChildExtent(0);
    childExtents[1] = fullNode->GetChildExtent(1);
    childExtents[2] = fullNode->GetChildExtent(2);
    childExtents[3] = fullNode->GetChildExtent(3);
    childExtents[4] = fullNode->GetChildExtent(4);
    childExtents[5] = childExtent;

    void *children[RTreeNode::MaxChildren + 1];

    children[0] = fullNode->m_child[0];
    children[1] = fullNode->m_child[1];
    children[2] = fullNode->m_child[2];
    children[3] = fullNode->m_child[3];
    children[4] = fullNode->m_child[4];
    children[5] = childItem;

    // determine the best partition of the children

    const int *partition = ChooseBestPartition(childExtents,
        &fullNode->m_nodeExtent, &newNode->m_nodeExtent);

    // assign the children to the two nodes according to the partition

    fullNode->m_child[0] = children[partition[0]];
    fullNode->m_child[1] = children[partition[1]];
    fullNode->m_child[2] = children[partition[2]];
    fullNode->m_child[3] = NULL;
    fullNode->m_child[4] = NULL;

    newNode->m_child[0] = children[partition[3]];
    newNode->m_child[1] = children[partition[4]];
    newNode->m_child[2] = children[partition[5]];
    newNode->m_child[3] = NULL;
    newNode->m_child[4] = NULL;

    // return a pointer to the new node

    return newNode;

} // end: SplitNode()


//------------------------------------------------------------------------------
//
// METHOD: ChooseBestPartition().
//
// PURPOSE: Partition a set of 6 input extents into two groups of three in such
//          a way that the sum of the areas of the extents for the two groups is
//          minimized (this is the exhaustive split strategy described by
//          Guttman).
//
// PARAMETERS:
//
//     Input:
//
//         inExtents  - passes an array of pointers to the six input extents.
//
//     Output:
//
//         outExtent1 - returns the minimal extent for the first set.
//         outExtent2 - returns the minimal extent for the second set.
//
// RETURNS: Returns a pointer to an array of integers specifying the
//          best partition; the parition is interpreted as:
//          {inExtents[partition[0]],inExtents[partition[1]],
//           inExtents[partition[2]]}, and
//          {inExtents[partition[3]],inExtents[partition[4]],
//           inExtents[partition[5]]}.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

const int *OpsRTree::ChooseBestPartition(const OpsFloatExtent * const inExtents[6],
    OpsFloatExtent *outExtent1, OpsFloatExtent *outExtent2)
{
    // static array of all possible splits of 6 extents into two groups of 3

    static int partitions[10][6] =
        {{0,1,2,3,4,5},{0,1,3,2,4,5},{0,1,4,2,3,5},{0,1,5,2,3,4},{0,2,3,1,4,5},
         {0,2,4,1,3,5},{0,2,5,1,3,4},{0,3,4,1,2,5},{0,3,5,1,2,4},{0,4,5,1,2,3}};

    // try the 10 possible partitions ...

    OpsFloatExtent extentGroup1[10];
    OpsFloatExtent extentGroup2[10];
    int bestPartition = 0;
    double minSumOfAreas = HUGE_VAL;

    for (int i = 0; i < 10; i++) {
        const OpsFloatExtent *ext0 = inExtents[partitions[i][0]];
        const OpsFloatExtent *ext1 = inExtents[partitions[i][1]];
        const OpsFloatExtent *ext2 = inExtents[partitions[i][2]];
        const OpsFloatExtent *ext3 = inExtents[partitions[i][3]];
        const OpsFloatExtent *ext4 = inExtents[partitions[i][4]];
        const OpsFloatExtent *ext5 = inExtents[partitions[i][5]];

        // derive the extent of the first group

        extentGroup1[i].xMin = Min(ext0->xMin, Min(ext1->xMin, ext2->xMin));
        extentGroup1[i].xMax = Max(ext0->xMax, Max(ext1->xMax, ext2->xMax));
        extentGroup1[i].yMin = Min(ext0->yMin, Min(ext1->yMin, ext2->yMin));
        extentGroup1[i].yMax = Max(ext0->yMax, Max(ext1->yMax, ext2->yMax));

        // derive the extent of the second group

        extentGroup2[i].xMin = Min(ext3->xMin, Min(ext4->xMin, ext5->xMin));
        extentGroup2[i].xMax = Max(ext3->xMax, Max(ext4->xMax, ext5->xMax));
        extentGroup2[i].yMin = Min(ext3->yMin, Min(ext4->yMin, ext5->yMin));
        extentGroup2[i].yMax = Max(ext3->yMax, Max(ext4->yMax, ext5->yMax));

        // compute the width and height of the two extents

        double width1 = extentGroup1[i].Width();
        double height1 = extentGroup1[i].Height();

        double width2  = extentGroup2[i].Width();
        double height2 = extentGroup2[i].Height();

        // compute the sum of the areas and the best partition so far

        double sumOfAreas = GetNonZeroArea(width1, height1) +
                            GetNonZeroArea(width2, height2);

        if (sumOfAreas < minSumOfAreas) {
            minSumOfAreas = sumOfAreas;
            bestPartition = i;
        }
    }

    // assign the output extents and return the best partition

    *outExtent1 = extentGroup1[bestPartition];
    *outExtent2 = extentGroup2[bestPartition];

    return partitions[bestPartition];

} // end: ChooseBestPartition()


//------------------------------------------------------------------------------
//
// METHOD: AdjustRTree().
//
// PURPOSE: Walk back up the search path from the leaf node (where an insertion
//          was performed) to the root, adjusting extents, and splitting nodes
//          where required.
//
// PARAMETERS:
//
//     Input:
//
//         newNode - passes a pointer to the new leaf node (if any) that res-
//                   ulted from the insertion leaf node being split.
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

void OpsRTree::AdjustRTree(RTreeNode *newNode)
{
    // get the pointer to the node that is on top of the stack

    RTreeNode *node = m_nodePtrStack.Top();
    assert(node != NULL);
    m_nodePtrStack.Pop();

    // walk through the node pointers on the stack, adjusting node extents,
    // and splitting nodes where required

    while (!m_nodePtrStack.IsEmpty()) {
        RTreeNode *parent = m_nodePtrStack.Top();

        if (newNode == NULL)
            parent->m_nodeExtent.UnionWith(&node->m_nodeExtent);
        else if (parent->IsNodeFull())
            newNode = SplitNode(parent, newNode, &newNode->m_nodeExtent);
        else {
            parent->m_nodeExtent.UnionWith(&node->m_nodeExtent);
            parent->AddChild(newNode);
            parent->m_nodeExtent.UnionWith(&newNode->m_nodeExtent);
            newNode = NULL;
        }

        node = parent;
        m_nodePtrStack.Pop();
    }

    // if the root node was split, then create a new one, growing the tree
    // in height by 1

    if (newNode != NULL) {
        assert(node == m_rootNode);
        m_rootNode = m_nodeAllocator.Allocate();
        m_rootNode->Initialize(node->m_nodeLevel + 1);
        m_rootNode->m_child[0] = node;
        m_rootNode->m_nodeExtent = node->m_nodeExtent;
        m_rootNode->m_child[1] = newNode;
        m_rootNode->m_nodeExtent.UnionWith(&newNode->m_nodeExtent);
        m_rTreeHeight++;
    }

} // end: AdjustRTree()


//------------------------------------------------------------------------------
//
// METHOD: FindLeafNode().
//
// PURPOSE: Find the leaf node containing the specified item. Upon return, the
//          leaf node containing the item (if any) is on the top of the stack.
//
// PARAMETERS:
//
//     Input:
//
//         item       - pointer to the item to be located.
//         itemExtent - pointer to an OpsFloatExtent structure containing the
//                      coordinates of the item extent.
//
//     Output:
//
//         None.
//
// RETURNS: The index of the item within the leaf node, or -1 if the item was
//          not located.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OpsRTree::FindLeafNode(const void *item, const OpsFloatExtent *itemExtent)
{
    // check for an empty R-Tree

    if (m_rootNode == NULL)
        return -1;

    // push the root node onto the stack, and iteratively descend into the
    // R-Tree looking for the item

    m_nodePtrStack.Reset();
    m_nodePtrStack.Push(m_rootNode);
    m_rootNode->m_subTree = 0;

    while (!m_nodePtrStack.IsEmpty()) {
        RTreeNode *topNode = m_nodePtrStack.Top();
        BOOL descending = FALSE;

        // if we have a leaf node, then check each item in the node to see
        // if it is the one we are looking for

        if (topNode->IsaLeafNode()) {
            for (int i = 0; i < RTreeNode::MaxChildren; i++) {
                ItemContainer *container = topNode->GetChildItem(i);

                if (container == NULL)
                    break;
                else if (container->m_item == item)
                    return i;
            }
        }

        // else have an internal node - descend into all children whose extent
        // contains the specified item extent

        else {
            for (int i = topNode->m_subTree; i < RTreeNode::MaxChildren; i++) {
                RTreeNode *childNode = topNode->GetChildNode(i);

                if (childNode == NULL)
                    break;
                else if (childNode->m_nodeExtent.Contains(itemExtent)) {
                    topNode->m_subTree = i + 1;
                    childNode->m_subTree = 0;
                    m_nodePtrStack.Push(childNode);
                    descending = TRUE;
                    break;
                }
            }
        }

        // if not descending then pop the node

        if (!descending)
            m_nodePtrStack.Pop();
    }

    // if we got here, then the item must not be in the R-Tree

    return -1;

} // end: FindLeafNode()


//------------------------------------------------------------------------------
//
// METHOD: CondenseRTree().
//
// PURPOSE: Remove the specified item from the leaf node that is on top of the
//          stack, then walk back up the search path from the root to the leaf
//          node, adjusting node extents, and eliminating underfilled nodes.
//
// PARAMETERS:
//
//     Input:
//
//         itemIndex - index of the item to be removed.
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

void OpsRTree::CondenseRTree(int itemIndex)
{
    // pop the leaf node on top of the stack, and remove the specified item

    RTreeNode *node = m_nodePtrStack.Top();
    m_nodePtrStack.Pop();
    assert(node->IsaLeafNode());
    m_containerAllocator.Free(node->GetChildItem(itemIndex));
    node->RemoveChild(itemIndex);

    // walk back up the search path, adjusting node extents, and eliminating
    // underfilled nodes

    int nEliminatedNodes = 0;

    while (node != m_rootNode) {
        RTreeNode *parent = m_nodePtrStack.Top();

        if (node->GetNChildren() < RTreeNode::MinChildren) {
            assert(nEliminatedNodes < MaxEliminatedNodes);
            m_eliminatedNodes[nEliminatedNodes++] = node;
            int childIndex = parent->GetChildIndex(node);
            parent->RemoveChild(childIndex);
        }
        else
            node->RecomputeExtent();

        node = parent;
        m_nodePtrStack.Pop();
    }

    // adjust the extent of the root node (provided it is not empty)

    if (m_rootNode->GetNChildren() > 0)
        m_rootNode->RecomputeExtent();

    // reinsert all orphaned nodes/items

    for (int i = 0; i < nEliminatedNodes; i++) {
        RTreeNode *node = m_eliminatedNodes[i];
        assert(node->GetNChildren() > 0);
        assert(node->GetNChildren() < RTreeNode::MinChildren);

        int nChildren = node->GetNChildren();

        for (int j = 0; j < nChildren; j++)
            Insert(node->m_child[j], node->GetChildExtent(j), node->m_nodeLevel);
    }

    // if the R-Tree is now empty then free up the root node

    int nChildren = m_rootNode->GetNChildren();

    if (nChildren == 0) {
        m_nodeAllocator.Free(m_rootNode);
        m_rootNode = NULL;
        m_rTreeHeight = 0;
    }

    // else if the root node now has only one child, and it is not a leaf node,
    // then make the child the new root node

    else if (!m_rootNode->IsaLeafNode() && nChildren == 1) {
        RTreeNode *temp = m_rootNode;
        m_rootNode = m_rootNode->GetChildNode(0);
        m_nodeAllocator.Free(temp);
        m_rTreeHeight--;
    }

} // end: CondenseRTree()


//------------------------------------------------------------------------------
//
// Implementation of the OpsRTree::IntersectsSearchPredicate class.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// METHOD: Initialize().
//
// PURPOSE: Initialize a standard search predicate object by setting the
//          search extent.
//
// PARAMETERS:
//
//     Input:
//
//         searchExtent - passes a reference to the search extent. A copy is
//                        made.
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

void OpsRTree::IntersectsSearchExtent::Initialize(const OpsFloatExtent &searchExtent)
{
    m_searchExtent = searchExtent;

} // end: Initialize()


//------------------------------------------------------------------------------
//
// METHOD: Predicate().
//
// PURPOSE: Implementation of the predicate. The method returns TRUE if the
//          node extent intersects the search extent set in the last call to
//          Initialize().
//
// PARAMETERS:
//
//     Input:
//
//         nodeExtent - passes a reference to the node (or item) extent to test
//                      for intersection with the search extent.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the node extent intersects the search extent.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OpsRTree::IntersectsSearchExtent::Predicate(const OpsFloatExtent &nodeExtent) const
{
    return m_searchExtent.Intersects(&nodeExtent);

} // end: Predicate()


//------------------------------------------------------------------------------
//
// Implementation of the OpsRTree::RTreeNode structure.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// METHOD: GetNChildren().
//
// PURPOSE: Determine the number of children the node has.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The number of children (0 to 5).
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OpsRTree::RTreeNode::GetNChildren() const
{
    int nChildren = 0;

    for (int i = MaxChildren - 1; i >= 0; i--) {
        if (m_child[i] != NULL) {
            nChildren = i + 1;
            break;
        }
    }

    return nChildren;

} // end: GetNChildren()


//------------------------------------------------------------------------------
//
// METHOD: AddChild().
//
// PURPOSE: Add a child to the node. The node is assumed to already contain
//          at least one child.
//
// PARAMETERS:
//
//     Input:
//
//         childPtr - pointer to the child.
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

void OpsRTree::RTreeNode::AddChild(void *childPtr)
{
    assert(!IsNodeFull());
    assert(m_child[0] != NULL);

    for (int i = 1; i < MaxChildren; i++) {
        if (m_child[i] == NULL) {
            m_child[i] = childPtr;
            break;
        }
    }

} // end: AddChild()


//------------------------------------------------------------------------------
//
// METHOD: RemoveChild().
//
// PURPOSE: Remove a child from the node.
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index of the child to be removed.
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

void OpsRTree::RTreeNode::RemoveChild(int index)
{
    assert(index >= 0);
    assert(index < GetNChildren());

    for (int i = index; i < MaxChildren - 1; i++)
        m_child[i] = m_child[i+1];

    m_child[MaxChildren - 1] = NULL;

} // end: RemoveChild()


//------------------------------------------------------------------------------
//
// METHOD: RecomputeExtent().
//
// PURPOSE: Recompute the extent of the node based on the extents of all its
//          children.
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

void OpsRTree::RTreeNode::RecomputeExtent()
{
    assert(GetNChildren() > 0);

    m_nodeExtent = *GetChildExtent(0);

    for (int i = 0; i < MaxChildren && m_child[i] != NULL; i++)
        m_nodeExtent.UnionWith(GetChildExtent(i));

} // end: RecomputeExtent()


//------------------------------------------------------------------------------
//
// METHOD: GetChildIndex().
//
// PURPOSE: Get the index of the specified child.
//
// PARAMETERS:
//
//     Input:
//
//         childPtr - pointer to the child.
//
//     Output:
//
//         None.
//
// RETURNS: The index of the specified child, or -1 if node does not point
//          to the child.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OpsRTree::RTreeNode::GetChildIndex(void *childPtr) const
{
    if (IsaLeafNode()) {
        for (int i = 0; i < MaxChildren && m_child[i] != NULL; i++) {
             if (GetChildItem(i)->m_item == childPtr)
                return i;
        }
    }
    else {
        for (int i = 0; i < MaxChildren && m_child[i] != NULL; i++) {
            if (m_child[i] == childPtr)
                return i;
        }
    }

    return -1;

} // end: GetChildIndex()


//------------------------------------------------------------------------------
//
// Implementation of the OpsRTree::NodePtrStack class.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Initialize an instance of the OpsRTree::NodePtrStack class.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available.
//
//------------------------------------------------------------------------------

OpsRTree::NodePtrStack::NodePtrStack() :
    m_topOfStack(-1),
    m_nodePtrArray(NULL)
{
    m_nodePtrArray = new OpsRTree::RTreeNode*[NodePtrArraySize];

} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Destroy an instance of the OpsRTree::NodePtrStack class, releasing
//          all dynamically allocated resources.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsRTree::NodePtrStack::~NodePtrStack()
{
    delete [] m_nodePtrArray;

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: Reset().
//
// PURPOSE: Reset the stack to an empty state.
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

void OpsRTree::NodePtrStack::Reset()
{
    m_topOfStack = -1;

} // end: Reset()


//------------------------------------------------------------------------------
//
// METHOD: GetSize().
//
// PURPOSE: Get the current size (number of elements) of the stack.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The number of elements currently pushed onto the stack.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OpsRTree::NodePtrStack::GetSize() const
{
    return m_topOfStack + 1;

} // end: GetSize()
