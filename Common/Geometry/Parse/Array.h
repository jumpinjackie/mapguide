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

/// ALL THESE ARE INTERNAL ONLY

#ifndef _ARRAY_H_
#define _ARRAY_H_

#ifdef _WIN32
#pragma once
#endif
#include "ArrayHelper.h"

// Template for dynamic array.  It supports:
//  - shared ownership, via reference counting
//  - single allocation on heap for entire structure (no internal indirection)
//  - dynamic growth with infrequent reallocation
//  - hiding "new" and "delete" from callers that may have redefined these operators
//
// This class integrates its own needed member variables within
// the array itself.  This approach is very space efficient, as
// it avoids the overhead of a level of indirection; there is no
// pointer to an internal, owned array.
//
// The tradeoffs to get these features are:
//  - any method call that could cause re-allocation is static,
//    returning a (possibly new) location for the array object
//  - one cannot call re-allocating methods if the object is shared
//  - this class must never contain virtual methods (thus does its own reference counting)
//  - the allocation mechanism delegates to a helper class
//  - it is not typesafe for objects that cannot safely be moved around
//    in memory or which have non-empty destructors (it is safe for basic
//    types and most structs).
//
// Example:
//      GisIntArray* ints = GisIntArray::Create();
//      ints = GisIntArray::Append(ints, 7);
//      INT32 moreInts [] = { 8, 9 };
//      INT32 numMoreInts = sizeof(moreInts)/sizeof(moreInts[0]);
//      ints = GisIntArray::Append(ints, numMoreInts, moreInts);
//      int element = (*ints)[1];
//      cout << "The second element is " << element << endl;
//      GIS_SAFE_RELEASE(ints);


/// \brief
/// Abstract template class for defining dynamic arrays.
///
/// \remarks
/// WARNING: This array uses a single memory allocation to store array elements
/// and may re-allocate that memory when the array size needs to change.
/// Therefore, do not keep pointers to array elements and avoid using
/// for objects which cannot be safely moved around in memory or which have
/// non-empty destructors.
template <typename T> class GisArray // TODO : public GisIDisposable
{
public:
    /// constructors
    //
    /// \brief
    /// Create an empty array
    ///
    /// \return
    /// Returns a GisArray pointer
    ///
    static GisArray<T> * Create();
    /// \brief
    /// Create a dynamic array preallocated to a specific size
    ///
    /// \param initialAlloc
    /// Input initial size of the array
    ///
    /// \return
    /// Returns a GisArray pointer
    ///
    static GisArray<T> * Create(INT32 initialAlloc);
    /// \brief
    /// Create a dynamic array from a pointer to a fixed array
    ///
    /// \param elements
    /// Input pointer to array of elements
    /// \param initialNumElements
    /// Input number of elements to be added
    ///
    /// \return
    /// Returns a GisArray pointer
    ///
    static GisArray<T> * Create(const T* elements, INT32 initialNumElements);

    /// GisIDisposable simulation
    //
    /// \brief
    /// Increase the reference count.
    ///
    /// \return
    /// Returns the new reference count. For debugging use only.
    ///
    inline INT32 AddRef() { return ++((GisArrayHelper::GenericArray*)this)->m_metadata.refCount; }
    /// \brief
    /// Decrease the reference count.
    ///
    /// \return
    /// Returns the new reference count. For debugging use only.
    ///
    inline INT32 Release() { if (0 != --((GisArrayHelper::GenericArray*)this)->m_metadata.refCount ) return ((GisArrayHelper::GenericArray*)this)->m_metadata.refCount; Dispose(); return 0; }

    /// simple access (count and data)
    //
    /// \brief
    /// Gets the number of items in the array.
    ///
    /// \return
    /// Returns number of items in the array
    ///
    INT32 GetCount() const { return ((GisArrayHelper::GenericArray*)this)->m_metadata.size; }
    /// \brief
    /// Get a pointer to the data in the array.
    ///
    /// \return
    /// Returns a pointer to the data
    ///
    T* GetData() { return (T*)(((GisArrayHelper::GenericArray*)this)->GetData()); }
    /// \brief
    /// Get a pointer to the data in the array.
    ///
    /// \return
    /// Returns a pointer to the data
    ///
    const T* GetData() const {return (const T*)(((GisArrayHelper::GenericArray*)this)->GetData()); }

    /// \brief
    ///  Resize the array.
    ///
    /// \return
    /// Returns the new array pointer.
    ///
    static GisArray<T> * SetSize(GisArray<T> * array, INT32 numElements);

    /// \brief
    /// Adds the specified array of items to the end of the array. Returns a new array pointer.
    ///
    /// \param array
    /// Input array pointer
    /// \param numElements
    /// Input number of items to be added
    /// \param elements
    /// Input pointer to array to append
    ///
    /// \return
    /// Returns the new array pointer.
    ///
    static GisArray<T> * Append(GisArray<T> * array, INT32 numElements, T * elements);

    /// \brief
    /// Adds the specified item to the end of the array. Returns a new array pointer.
    ///
    /// \param array
    /// Input array pointer
    /// \param element
    /// Input item to be appended to array
    ///
    /// \return
    /// Returns the new array pointer.
    ///
    static GisArray<T> * Append(GisArray<T> * array, T element);

    /// \brief
    /// Removes all items from the array.
    ///
    /// \return
    /// Returns nothing
    ///
    void Clear();

    /// overloaded operators
    //
    /// \brief
    /// Provide index operator [] access to array members.
    ///
    /// \param i
    /// Input index to access
    ///
    /// \return
    /// Returns the value of the specified item in the array.
    ///
    /// note: need to deference array pointer to use (*ints)[i]
    const T& operator[](int i) const { return GetValue(i); }

    /// \brief
    /// Provide index operator [] access to array members.
    ///
    /// \param i
    /// Input index to access
    ///
    /// \return
    /// Returns the value of the specified item in the array.
    ///
    T& operator[](int i) { return GetValue(i); }

protected:
    /// Constructor
    GisArray() {}

    static GisArray<T> * AllocMore(GisArray<T> * array, INT32 atLeastThisMuch, bool exactly);
    static GisArray<T> * SetAlloc(GisArray<T> * array, INT32 numElements);

    /// Access elements
    const T& GetValue(INT32 i) const;
    T& GetValue(INT32 i);
    void SetValue(INT32 i, T newElement);

    /// the rest of GisIDisposable
    void Dispose();
};

template <class T>
inline
GisArray<T> * GisArray<T>::Create()
{
    return AllocMore(0, 0, true);
}

template <class T>
inline
GisArray<T> * GisArray<T>::Create(INT32 initialAlloc)
{
    return AllocMore(0, initialAlloc, true);
}

template <class T>
inline
GisArray<T> * GisArray<T>::Create(const T* elements, INT32 initialNumElements)
{
    GisArray<T> * array = AllocMore(0, initialNumElements, true);
    if (array != NULL)
        array = (GisArray<T> *)GisArrayHelper::Append((GisArrayHelper::GenericArray*) array, initialNumElements, (unsigned char*) elements, sizeof(T));
    return array;
}

template <class T>
inline
GisArray<T> * GisArray<T>::Append(GisArray<T> * array, INT32 numElements, T * elements)
{
    array = (GisArray<T> *)GisArrayHelper::Append((GisArrayHelper::GenericArray*) array, numElements, (unsigned char*) elements, sizeof(T));
    return array;
}

template <class T>
inline
GisArray<T> * GisArray<T>::Append(GisArray<T> * array, T element)
{
    if ( ((GisArrayHelper::GenericArray*)array)->m_metadata.size == ((GisArrayHelper::GenericArray*)array)->m_metadata.alloc )
    {
        array = (GisArray<T> *)GisArrayHelper::AllocMore((GisArrayHelper::GenericArray*) array, 1, false /*exactly*/, sizeof(T));
    }
    array->GetValue((((GisArrayHelper::GenericArray*)array)->m_metadata.size)++) = element;
    return array;
}

template <class T>
inline
void GisArray<T>::Clear()
{
    /// since no memory reallocation we can avoid input/output of array class pointer
    SetSize(this, 0);
}

template <class T>
inline
GisArray<T> * GisArray<T>::SetSize(GisArray<T> * array, INT32 numElements)
{
    array = (GisArray<T> *)GisArrayHelper::SetSize((GisArrayHelper::GenericArray*) array, numElements, sizeof(T));
    return array;
}

template <class T>
inline
GisArray<T> * GisArray<T>::SetAlloc(GisArray<T> * array, INT32 numElements)
{
    array = (GisArray<T> *)GisArrayHelper::SetAlloc((GisArrayHelper::GenericArray*) array, numElements, sizeof(T));
    return array;
}

//
// Allocate some more memory for the array.
// If the given array is NULL, a new array of zero size
// but the requested allocation is returned.
// The returned array will always have at least one new
// free element allocated. Thus, passing atLeastThisMuch < 1,
// while not an error, still results in some new storage.
//
template <class T>
inline
GisArray<T> * GisArray<T>::AllocMore(GisArray<T> * array, INT32 atLeastThisMuch, bool exactly)
{
    array = (GisArray<T> *)GisArrayHelper::AllocMore((GisArrayHelper::GenericArray*) array, atLeastThisMuch, exactly, sizeof(T));
    return array;
}

template <class T>
inline
void GisArray<T>::Dispose()
{
    GisArrayHelper::DisposeOfArray((GisArrayHelper::GenericArray*) this, sizeof(T));
}

template <class T>
inline
const T& GisArray<T>::GetValue(INT32 i) const
{
    if (i >= GetCount() || i < 0)
    {
        throw new MgIndexOutOfRangeException(L"GisArray<T>.GetValue", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return *(this->GetData()+i);
}

template <class T>
inline
T& GisArray<T>::GetValue(INT32 i)
{
    if (i >= GetCount() || i < 0)
        throw new MgIndexOutOfRangeException(L"GisArray<T>.GetValue", __LINE__, __WFILE__, NULL, L"", NULL);

    return *(this->GetData()+i);
}

template <class T>
inline
void GisArray<T>::SetValue(INT32 i, T newElement)
{
    if (i >= GetCount() || i < 0)
        throw new MgIndexOutOfRangeException(L"GisArray<T>.SetValue", __LINE__, __WFILE__, NULL, L"", NULL);

    *(this->GetData()+i) = newElement;
}

/// \brief
/// Dynamic array of bytes (unsigned char).
typedef GisArray<unsigned char>   GisByteArray;

/// \brief
/// Dynamic array of doubles (double).
typedef GisArray<double> GisDoubleArray;

/// \brief
/// Dynamic array of integers (INT32).
typedef GisArray<INT32>  GisIntArray;

//typedef GisArray<GisString> GisStringArray;
#endif

