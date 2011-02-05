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
// FILE: oobject.cpp
//
// PURPOSE: Implementation of the OpsObject type.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

//------------------------------------------------------------------------------
//
// METHOD: AllocMem().
//
// PURPOSE: Allocate a block of memory.
//
// PARAMETERS
//
//     Input:
//
//         nBytes - passes the number of bytes in the block of memory.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the newly allocated block of memory.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory available.
//
//------------------------------------------------------------------------------

void *OpsObject::AllocMem(size_t nBytes)
{
    void *memPtr = ::malloc(nBytes);

    if (!memPtr)
    {
        throw new MgOutOfMemoryException(L"OpsObject.AllocMem", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return memPtr;

} // end: AllocMem()


//------------------------------------------------------------------------------
//
// METHOD: ReallocMem().
//
// PURPOSE: Reallocate a block of memory to a different size. The block
//          to be reallocated is assumed to have been acquired from
//          OpsObject::AllocMem() or OpsObject::ReallocMem().
//
// PARAMETERS
//
//     Input:
//
//         memPtr - passes a pointer to the block to reallocate.
//         nBytes - passes the number of bytes in the block of memory.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the reallocated block of memory.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory available.
//
//------------------------------------------------------------------------------

void *OpsObject::ReallocMem(void *memPtr, size_t nBytes)
{
    void *newMemPtr = ::realloc(memPtr, nBytes);
    if (!newMemPtr && nBytes != 0)
    {
        throw new MgOutOfMemoryException(L"OpsObject.ReallocMem", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    return newMemPtr;

} // end: AllocMem()

//------------------------------------------------------------------------------
//
// METHOD: FreeMem().
//
// PURPOSE: Free a block of memory that was acquired from OpsObject::-
//          AllocMem() or OpsObject::ReallocMem()
//
// PARAMETERS
//
//     Input:
//
//         memPtr - passes a pointer to the block that is to be freed.
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

void OpsObject::FreeMem(void *memPtr)
{
    ::free(memPtr);

} // end: AllocMem()
