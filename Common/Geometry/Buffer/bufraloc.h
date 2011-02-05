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
// FILE: bufraloc.h.
//
// PURPOSE: Definition of the BufferAllocator template. The BufferAllocator
//          template defines generic allocation behaviour required by many
//          of the classes the in the buffering library. Objects of a specified
//          type are allocated in blocks of a given size, and methods are
//          defined to access the next free object, determine the number of
//          objects allocated, and access an allocated object using array style
//          indexing. NOTE that BufferAllocator does not provide generic
//          memory allocation. It is specific to the needs of various classes
//          in the buffering library.
//
//------------------------------------------------------------------------------

#ifndef _BUFRALOC_H_
#define _BUFRALOC_H_

template <class ObjType> class BufferAllocator {
public:
    /// constructor/destructor

    BufferAllocator(int blockSize);
    virtual ~BufferAllocator();

    /// method to get the next available object

    ObjType *GetNextObject();

    /// methods to determine the number of objects that have been allocated, and
    /// to access a specific allocated object

    int GetNObjects() const;
    ObjType *operator[](int index) const;

private:
    int m_blockSize;                 // size of the block to allocate
    int m_nBlocks;                   // number of blocks allocated
    int m_blockTableSize;            // current size of m_objBlockTable
    int m_nextObjectIndex;           // index of next object
    int m_nObjectsAllocated;         // number of allocated objects
    ObjType **m_objectBlockTable;    // array of pointers to allocated blocks
    enum {InitBlockTableSize = 32};  // initial size of block table

    /// method to get a new block of objects

    void GetNewBlock();
};


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize a BufferAllocator<ObjType> object.
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

template <class ObjType> BufferAllocator<ObjType>::BufferAllocator(
    int blockSize) :
    m_blockSize(blockSize),
    m_nBlocks(0),
    m_blockTableSize(0),
    m_nextObjectIndex(blockSize),
    m_nObjectsAllocated(0),
    m_objectBlockTable(NULL)
{
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

template <class ObjType> BufferAllocator<ObjType>::~BufferAllocator()
{
    for (int i = 0; i < m_nBlocks; i++)
        delete [] m_objectBlockTable[i];

    delete [] m_objectBlockTable;

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: GetNextObject().
//
// PURPOSE: Get the next available object.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A pointer to the next available object.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available.
//
//------------------------------------------------------------------------------

template <class ObjType> ObjType *BufferAllocator<ObjType>::GetNextObject()
{
    /// allocate a new block of objects if necessary

    if (m_nextObjectIndex >= m_blockSize) {
        GetNewBlock();
        m_nextObjectIndex = 0;
    }

    /// update counter of the number of objects that have been returned, and
    /// return a pointer to the new object

    m_nObjectsAllocated++;

    return &m_objectBlockTable[m_nBlocks - 1][m_nextObjectIndex++];

} // end: GetNextObject()


//------------------------------------------------------------------------------
//
// METHOD: GetNObjects().
//
// PURPOSE: Get the number of objects that have been allocated.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: An integer specifying the number of objects returned from calls
//          to GetNextObject.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template <class ObjType> inline int BufferAllocator<ObjType>::GetNObjects()
    const
{
    return m_nObjectsAllocated;

} // end: GetNObjects()


//------------------------------------------------------------------------------
//
// METHOD: operator[]().
//
// PURPOSE: Get a pointer to the indexed object.
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index of the object. This value must be between 0
//                 and GetNObjects() - 1.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the indexed object.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template <class ObjType> inline ObjType *BufferAllocator<ObjType>::operator[](
    int index) const
{
    /// check that the index is in range

    assert(index >= 0);
    assert(index < m_nObjectsAllocated);

    /// compute the index of the block containing the object, and the index of
    /// the object within the block

    int blockIndex = int(index / m_blockSize);
    int indexWithinBlock = int(index & (m_blockSize - 1));

    /// return a pointer to the object

    return &m_objectBlockTable[blockIndex][indexWithinBlock];

} // end: operator[]()


//------------------------------------------------------------------------------
//
// METHOD: GetNewBlock().
//
// PURPOSE: Allocate a new block of objects.
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

template <class ObjType> void BufferAllocator<ObjType>::GetNewBlock()
{
    /// reallocate the block table to a larger size if necessary by doubling
    /// its size

    if (m_nBlocks >= m_blockTableSize) {
    /// double of the size of the block table (or if this is the first time
    /// allocated, use the initial block table size)

        int newBlockTableSize;

        if (m_blockTableSize > 0)
            newBlockTableSize = m_blockTableSize + m_blockTableSize;
        else
            newBlockTableSize = InitBlockTableSize;

        ObjType **temp = new ObjType*[newBlockTableSize];

    /// save pointers to existing blocks, and setup to use larger block table

        if (m_nBlocks > 0) {
            size_t nBytesToCopy = m_nBlocks * sizeof(ObjType *);
            ::memcpy(temp, m_objectBlockTable, nBytesToCopy);
        }

        delete [] m_objectBlockTable;
        m_objectBlockTable = temp;
        m_blockTableSize = newBlockTableSize;
    }

    /// allocate the new block of objects and update the block count

    m_objectBlockTable[m_nBlocks] = new ObjType[m_blockSize];
    m_nBlocks++;

} // end: GetNewBlock()

#endif
