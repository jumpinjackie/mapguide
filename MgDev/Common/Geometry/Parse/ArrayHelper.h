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

#ifndef _ARRAYHELPER_H_
#define _ARRAYHELPER_H_

#ifdef _WIN32
#pragma once
#endif

#define GIS_API

/// \cond INTERNAL
///////////////////////////////////////////////////////////
/* Class to do allocations for the GisArray template.
 * Allocations and freeing must be done in this class in order to
 * protect against various callers that have overridden
 * the "new" and "delete" operators.
 */
class GisArrayHelper
{
public:
    /// Metadata for an array.  This precedes the actually array data in any allocation.
    struct Metadata
    {
        INT32 refCount;  // Reference count
        INT32 size;      // # bytes used in array data
        INT32 alloc;     // # bytes allocated for array data
        void SetSizes(INT32 rc, INT32 s, INT32 a) { refCount=rc; size=s; alloc=a; }
    };

    /// Declare generic array and element types.
    struct GenericArray     // An array is a single heap allocation whose first bytes are a GenericArray.
    {
        Metadata m_metadata;
        unsigned char* GetData() { return (m_metadata.alloc<=0) ? 0 : (((unsigned char*)this) + sizeof(Metadata)); }
    };

    GIS_API static GenericArray* Append(GenericArray* array, INT32 numElements, unsigned char* elements, INT32 elementSize);
    GIS_API static GenericArray* SetSize(GenericArray* array, INT32 numElements, INT32 elementSize);
    GIS_API static GenericArray* SetAlloc(GenericArray* array, INT32 numElements, INT32 elementSize);
    GIS_API static GenericArray* AllocMore(GenericArray* array, INT32 atLeastThisMuch, bool exactly, INT32 elementSize);
    GIS_API static void DisposeOfArray(GenericArray* array, INT32 elementSize);

protected:

    /// Constructor (makes sure that this class cannot be instantiated -- it consists of all static methods.
    GIS_API GisArrayHelper() {};
};
/// \endcond

#endif
