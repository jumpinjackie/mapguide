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

/// \cond INTERNAL

//-----------------------------------------------------------------------------
//
// FILE: oarray.h.
//
// PURPOSE: Declaration of the OpsArray template. OpsArray defines a template
//          that implements an object managing an array of elements of a spec-
//          ified type. Clients of OpsArray request arrays of a certain min-
//          imum size, and the OpsArray object resizes the internally managed
//          array if necessary before returning the array pointer to the
//          client. OpsArray is meant for internal use by OPS++, and should
//          not be viewed as a generically useful resizable array (it isn't).
//
//-----------------------------------------------------------------------------

#ifndef _OARRAY_H_
#define _OARRAY_H_

template<class ElementType> class OpsArray : public OpsObject {
public:
    /// constructor/destructor

    OpsArray(int initSize = 0);
    virtual ~OpsArray();

    /// implementation method to reallocate the array

    ElementType *ResizeArray(int size);

    /// method to access the currently allocated array, and its size

    operator const ElementType *() const;
    ElementType *GetArray() const;
    int GetMaxSize() const;

    /// overloaded subscripting operators

    ElementType &operator[](int index);
    const ElementType &operator[](int index) const;

private:
    int m_currSize;         // current size
    ElementType *m_array;   // current array
};

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Initialize an OpsArray<ElementType> object.
//
// PARAMETERS:
//
//     Input:
//
//       initSize - specifies the initial size of the array.
//
//     Output:
//
//        None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template<class ElementType> OpsArray<ElementType>::OpsArray(int initSize) :
    m_currSize(0),
    m_array(NULL)
{
    assert(initSize >= 0);

    if (initSize > 0) {
        ResizeArray(initSize);
    }

} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Delete the internally managed array.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template<class ElementType> OpsArray<ElementType>::~OpsArray()
{
    delete [] m_array;

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: ResizeArray().
//
// PURPOSE: Ensure that the array is at least the specified size, resizing and
//          copying the existing contents if necessary.
//
// PARAMETERS:
//
//     Input:
//
//       size - specifies the minimum size of the array.
//
//     Output:
//
//        None.
//
// RETURNS: A pointer to the array.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available on an attempt to resize.
//
//------------------------------------------------------------------------------

template<class ElementType> ElementType *OpsArray<ElementType>::ResizeArray(
    int size)
{
    if (size > m_currSize) {
        int newSize;

        if (m_currSize == 0) {
            newSize = size;
        }
        else {
            for (newSize = m_currSize; newSize < size; newSize *= 2)
                ;
        }

        ElementType *newArray = new ElementType[newSize];

        if (m_currSize > 0) {
            ::memcpy(newArray, m_array, m_currSize * sizeof(ElementType));
        }

        delete [] m_array;

        m_array = newArray;
        m_currSize = newSize;
    }

    return m_array;

} // end: ResizeArray()


//------------------------------------------------------------------------------
//
// METHOD: operator ElementType *().
//
// PURPOSE: Overloaded typecast operator. The overloaded operator allows the
//          OpsArray to be use where a pointer to ElementType is expected.
//
// PARAMETERS:
//
//     None.:
//
// RETURNS: A pointer to the internally managed array.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template<class ElementType>
OpsArray<ElementType>::operator const ElementType * () const
{
    return GetArray();

} // end: operator ElementType *()


//------------------------------------------------------------------------------
//
// METHOD: operator []().
//
// PURPOSE: Overloaded subsripting operator. The overloaded operator accesses
//          the indexed element of the array.
//
// PARAMETERS:
//
//     None.:
//
// RETURNS: A reference to the indexed array element.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template<class ElementType>
inline ElementType &OpsArray<ElementType>::operator[](int index)
{
    assert(index >= 0);
    assert(index < m_currSize);

    return m_array[index];

} // end: operator[]()


//------------------------------------------------------------------------------
//
// METHOD: operator []().
//
// PURPOSE: Overloaded subsripting operator operator. The overloaded operator
//          accesses the indexed element of the array.
//
// PARAMETERS:
//
//     None.:
//
// RETURNS: A reference to the indexed array element.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template<class ElementType>
inline const ElementType &OpsArray<ElementType>::operator[](int index) const
{
    assert(index >= 0);
    assert(index < m_currSize);

    return m_array[index];

} // end: operator[]()


//------------------------------------------------------------------------------
//
// METHOD: GetArray().
//
// PURPOSE: Get a pointer to the interally managed array. No resizing is done.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A pointer to the internally managed array. Note - if the array has
//          not been given an initial nonzero size, NULL may be returned.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template<class ElementType> ElementType *OpsArray<ElementType>::GetArray() const
{
    return m_array;

} // end: GetArray()


//------------------------------------------------------------------------------
//
// METHOD: GetMaxSize().
//
// PURPOSE: Get the current (maximum) size of the internally managed array.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The current size of the array.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

template<class ElementType> int OpsArray<ElementType>::GetMaxSize() const
{
    return m_currSize;

} // end: GetMaxSize()


// typedefs for commonly used types of OpsArray's

typedef OpsArray<OpsFloatPoint> OpsFloatPointArray ;
//typedef OpsArray<OpsIntegerPoint> OpsIntegerPointArray;
typedef OpsArray<int> OpsIntArray;

#endif

/// \endcond
