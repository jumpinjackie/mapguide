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

#include <memory.h>

#include "Foundation.h"
#include "ArrayHelper.h"

#define MIN_ALLOC 1
#define GROWTH_FACTOR 1     /* Proportion of array size to grow when needed. */

GisArrayHelper::GenericArray* GisArrayHelper::Append(GenericArray* array, INT32 numElements, unsigned char* elements, INT32 elementSize)
{
    // Disallow possible re-allocation if object is shared.
    if (array->m_metadata.refCount > 1)
        throw new MgInvalidOperationException(L"GisArrayHelper.Append", __LINE__, __WFILE__, NULL, L"", NULL);

    if ( array->m_metadata.size + numElements > array->m_metadata.alloc )
        array = AllocMore(array, numElements, false /*exactly*/, elementSize);   // We could calculate the difference here, allocating less.

    unsigned char* appendHere = (array->GetData() + (array->m_metadata.size * elementSize));
    (void) memcpy( appendHere, elements, numElements * elementSize );

    array->m_metadata.size += numElements;

    return array;
}

GisArrayHelper::GenericArray* GisArrayHelper::SetSize(GenericArray* array, INT32 numElements, INT32 elementSize)
{
    // Disallow possible re-allocation if object is shared.
    if (array->m_metadata.refCount > 1)
        throw new MgInvalidOperationException(L"GisArrayHelper.SetSize", __LINE__, __WFILE__, NULL, L"", NULL);

    if ( numElements > array->m_metadata.alloc )
    {
        INT32 oldSize = array->m_metadata.size;
        INT32 diff = numElements - oldSize;

        array = AllocMore(array, diff, true /*exactly*/, elementSize);

        // Zero out the new storage.
        (void) memset( array->GetData() + (oldSize * elementSize), 0, diff * elementSize );
    }

    // reset size/count downward without changing allocation
    array->m_metadata.size = numElements;

    return array;
}

GisArrayHelper::GenericArray* GisArrayHelper::SetAlloc(GenericArray* array, INT32 numElements, INT32 elementSize)
{
    // Disallow possible re-allocation if object is shared.
    if (array->m_metadata.refCount > 1)
        throw new MgInvalidOperationException(L"GisArrayHelper.SetAlloc", __LINE__, __WFILE__, NULL, L"", NULL);

    if (numElements < array->m_metadata.size || numElements < MIN_ALLOC)
        throw new MgInvalidOperationException(L"GisArrayHelper.SetAlloc", __LINE__, __WFILE__, NULL, L"", NULL);

    if (array->m_metadata.alloc != numElements)
    {
        // Force new allocation of exactly the requested size.
        GenericArray* newArray = AllocMore(0, numElements, true /*exactly*/, elementSize);

        // Copy data from old array.
        newArray->m_metadata = array->m_metadata;
        newArray->m_metadata.alloc = numElements;
        if (array->m_metadata.size > 0)
        {
            // Copy data from old array.
            (void) memcpy( newArray->GetData(), array->GetData(), array->m_metadata.size * elementSize );
        }
        delete [] ((unsigned char*)array);
        array = 0;  // Just to be obvious that the pointer is stale.
        array = newArray;
    }

    return array;
}

/*
 * Allocate some more memory for the array.
 * If the given array is NULL, a new array of zero size
 * but the requested allocation is returned.
 * The returned array will always have at least one new
 * free element allocated. Thus, passing atLeastThisMuch < 1,
 * while not an error, still results in some new storage.
 */
GisArrayHelper::GenericArray* GisArrayHelper::AllocMore(GenericArray* array, INT32 atLeastThisMuch, bool exactly, INT32 elementSize)
{
    INT32 oldSize = (0==array) ? 0 : array->m_metadata.size;
    INT32 hopefulNewSize = oldSize + atLeastThisMuch;
    INT32 oldAlloc = (0==array) ? 0 : array->m_metadata.alloc;
    INT32 newAlloc = oldAlloc < MIN_ALLOC ? MIN_ALLOC : oldAlloc;    // There's always some room.

    if (exactly)
    {
        newAlloc = ( newAlloc > hopefulNewSize ) ? newAlloc : hopefulNewSize;
    }
    else // Infrequent-reallocation policy: increase the array size when needed
    {
        while ( newAlloc < hopefulNewSize )
            newAlloc *= 1 + GROWTH_FACTOR;
    }

    INT32 newAllocBytes = (newAlloc*elementSize) + sizeof(Metadata);

    /*
     * The main trick: GenericArray's only real "member" data is the metadata.
     * The rest of the allocated space is actually used for the array content itself.
     * This is why GetData() doesn't look for some member data, but simply
     * looks into "this" and skips over the metadata.  Because Metadata is a real
     * struct, skipping past its "sizeof" gives correct alignment in case the
     * array content is also composed of struct or class types.
     */

    GenericArray* newArray = (GenericArray*) new unsigned char[newAllocBytes];
    if (0==newArray)
        throw new MgOutOfMemoryException(L"GisArrayHelper.AllocMore", __LINE__, __WFILE__, NULL, L"", NULL);

    // Copy data from old array.
    if (0!=array)
    {
        newArray->m_metadata = array->m_metadata;
        if (oldSize > 0)
            (void) memcpy( newArray->GetData(), array->GetData(), oldSize*elementSize );
        // Make sure that stale pointers cannot do much.
        (void) memset(array, 0xfd, sizeof(Metadata) + (array->m_metadata.alloc*elementSize));
        delete [] ((unsigned char*) array);
        array = 0;
    }
    else
    {
        newArray->m_metadata.SetSizes(1,0,0);
    }

    newArray->m_metadata.alloc = newAlloc;

    return newArray;
}

void GisArrayHelper::DisposeOfArray(GenericArray* array, INT32 elementSize)
{
    // Make sure that stale pointers cannot do much.
    (void) memset(array, 0xff, sizeof(Metadata)+ (array->m_metadata.alloc*elementSize));
    delete [] ((unsigned char*)array);
}
