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

#include "Foundation.h"


//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgDisposableCollection.  Collection is initially empty.
/// </summary>
MgDisposableCollection::MgDisposableCollection()
{
    m_capacity = INIT_CAPACITY;
    m_size = 0;
    m_list = new MgDisposable*[m_capacity];
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the collection and the releases the contained MgDisposable objects.
/// </summary>
MgDisposableCollection::~MgDisposableCollection()
{
    for(INT32 i = 0; i < m_size; i++)
    {
        MgDisposable* disp = dynamic_cast<MgDisposable*>(m_list[i]);
        SAFE_RELEASE(disp);
    }
    delete[] m_list;
    m_list = NULL;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgDisposableCollection::GetClassId()
{
    return m_cls_id;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns number of items in the collection.
/// </summary>
INT32 MgDisposableCollection::GetCount() const
{
    return m_size;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection at the specified index.
/// Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>
/// Returns the item in the collection at the specified index.
/// </returns>
MgDisposable* MgDisposableCollection::GetItem(INT32 index) const
{
    if (index >= m_size || index < 0)
        throw new MgIndexOutOfRangeException(L"GetItem",__LINE__, __WFILE__, NULL, L"", NULL);
    // EXC::Create(Exception::NLSGetMessage(_NLSID(_5_INDEXOUTOFBOUNDS)));

    return SAFE_ADDREF(m_list[index]);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the item in the collection at the specified index to the specified value.
/// Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <param name="value">Input value</param>
/// <returns>Returns nothing</returns>
void MgDisposableCollection::SetItem(INT32 index, MgDisposable* value)
{
    if (index < m_size && index >= 0)
    {
        SAFE_RELEASE(m_list[index]);
        m_list[index] = SAFE_ADDREF(value);
    }
    else
        throw new MgIndexOutOfRangeException(L"SetItem", __LINE__, __WFILE__, NULL, L"", NULL);
    // EXC::Create(GisException::NLSGetMessage(GIS_NLSID(GIS_5_INDEXOUTOFBOUNDS)));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns nothing.
/// </returns>
void MgDisposableCollection::Add(MgDisposable* value)
{
    if (m_size == m_capacity)
        resize();

    m_list[m_size] = SAFE_ADDREF(value);
    m_size++;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Inserts the specified item at the specified index within the collection.
/// Items following the insertion point are moved down to accommodate the new item.
/// Throws an invalid argument exception if the specified index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
void MgDisposableCollection::Insert(INT32 index, MgDisposable* value)
{
    INT32    i;
    if (m_size == m_capacity)
        resize();
    if (index <= m_size && index >= 0)
    {
        for (i = m_size; i > index; i--)
            m_list[i] = m_list[i-1];

        m_list[index] = SAFE_ADDREF(value);
        m_size++;
    }
    else
        throw new MgIndexOutOfRangeException(L"Insert", __LINE__, __WFILE__, NULL, L"", NULL);
    // EXC::Create(GisException::NLSGetMessage(GIS_NLSID(GIS_5_INDEXOUTOFBOUNDS)));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes all items from the collection.
/// </summary>
/// <returns>Returns nothing.</returns>
void MgDisposableCollection::Clear()
{
    INT32    i;
    for (i = 0; i < m_size; i++)
    {
        SAFE_RELEASE(m_list[i]);
        m_list[i] = NULL;
    }

    m_size = 0;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns true if successful</returns>
bool MgDisposableCollection::Remove(const MgDisposable* value)
{
    INT32    i;
    for (i = 0; i < m_size; i++)
    {
        if (m_list[i] == value)
            break;
    }

    if (i == m_size)
        return false;

    SAFE_RELEASE(m_list[i]);

    while (i < m_size - 1)
    {
        m_list[i] = m_list[i+1];
        i++;
    }

    m_list[--m_size] = NULL;
    return true;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection. Throws an invalid argument exception if the item does not exist within the collection.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>Returns nothing.</returns>
void MgDisposableCollection::RemoveAt(INT32 index)
{
    if (index < m_size && index >= 0)
    {
        INT32    i;

        SAFE_RELEASE(m_list[index]);    // also NULLs out m_list[index]

        for (i = index; i < m_size-1; i++)
            m_list[i] = m_list[i+1];

        m_list[--m_size] = NULL;
    }
    else
        throw new MgIndexOutOfRangeException(L"RemoveAt", __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgDisposableCollection::Contains(const MgDisposable* value) const
{
    INT32    i;
    bool ret = false;
    for (i = 0; i < m_size; i++)
    {
        if (m_list[i] == value)
        {
            ret = true;
            break;
        }
    }
    return ret;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </returns>
INT32 MgDisposableCollection::IndexOf(const MgDisposable* value) const
{
    INT32    i, index = -1;
    for (i = 0; i < m_size; i++)
    {
        if (m_list[i] == value)
        {
            index = i;
            break;
        }
    }
    return index;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates an XML document representing the collection.
/// </summary>
/// <returns>
/// Returns a pointer to an MgByteReader object.
/// </returns>
MgByteReader* MgDisposableCollection::ToXml()
{
    throw new MgNotImplementedException(L"MgDisposableCollection.ToXml", __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgDisposableCollection::Serialize(MgStream* stream)
{
    throw new MgNotImplementedException(L"MgDisposableCollection.Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgDisposableCollection::Deserialize(MgStream* stream)
{
    throw new MgNotImplementedException(L"MgDisposableCollection.Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes the object.
/// </summary>
void MgDisposableCollection::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// resize the array
void MgDisposableCollection::resize()
{
    INT32 i;
    INT32 old_capacity = m_capacity;
    m_capacity = (INT32)(m_capacity*(1.4));
    MgDisposable** newArray = new MgDisposable*[m_capacity];
    for (i = 0; i < old_capacity; i++)
    {
        newArray[i] = m_list[i];
    }

    delete[] m_list;
    m_list = newArray;
}
