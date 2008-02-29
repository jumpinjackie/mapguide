//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "PlatformBase.h"
#include "SerializableCollection.h"

MG_IMPL_DYNCREATE(MgSerializableCollection);

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an empty collection of serializable objects.
/// </summary>
MgSerializableCollection::MgSerializableCollection()
{
    m_collection = new MgDisposableCollection();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the collection and the objects contained in it.
/// </summary>
MgSerializableCollection::~MgSerializableCollection()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes the object.
/// </summary>
void MgSerializableCollection::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the class ID.
/// </summary>
INT32 MgSerializableCollection::GetClassId()
{
    return m_cls_id;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns nothing.
/// </returns>
void MgSerializableCollection::Add(MgSerializable* value)
{
    m_collection->Add(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Inserts the specified item at the specified index within the collection.
/// Items following the insertion point are moved down to accommodate the new item.
/// </summary>
/// <param name="index">Input index</param>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
/// <exception>InvalidArgument if index is out of range.</exception>
void MgSerializableCollection::Insert(INT32 index, MgSerializable* value)
{
    m_collection->Insert(index, value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns true if successful.</returns>
bool MgSerializableCollection::Remove(MgSerializable* value)
{
    bool removed = true;
    try
    {
        m_collection->Remove(value);
    }
    catch (MgException* e)
    {
        e->Release();
        removed = false;
    }
    return removed;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the item in the collection at the specified index to the specified value.
/// </summary>
/// <param name="index">Input index</param>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
/// <exception>InvalidArgument if index is out of range.</exception>
void MgSerializableCollection::SetItem(INT32 index, MgSerializable* value)
{
    m_collection->SetItem(index, value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection at the specified index.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>
/// Returns the item in the collection at the specified index.
/// </returns>
/// <exception>InvalidArgument if index is out of range.</exception>
MgSerializable* MgSerializableCollection::GetItem(INT32 index) const
{
    return ((MgSerializable*)m_collection->GetItem(index));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of items in the collection.
/// </summary>
/// <returns>
/// Returns the number of items in the collection.
/// </returns>
INT32 MgSerializableCollection::GetCount() const
{
    return m_collection->GetCount();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes all items from the collection.
/// </summary>
/// <returns>Returns nothing.</returns>
void MgSerializableCollection::Clear()
{
    m_collection->Clear();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>Returns nothing.</returns>
/// EXCEPTIONS:
/// MgIndexOutOfRangeException
void MgSerializableCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgSerializableCollection::Contains(const MgSerializable* value)
{
    return m_collection->Contains(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </returns>
/// <exception>MgInvalidArgumentException</exception>
INT32 MgSerializableCollection::IndexOf(const MgSerializable* value) const
{
    return m_collection->IndexOf(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates an XML document representing the collection.
/// </summary>
/// <returns>
/// Returns a pointer to an MgByteReader object.
/// </returns>
MgByteReader* MgSerializableCollection::ToXml()
{
    throw new MgNotImplementedException(L"MgSerializableCollection.ToXml", __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgSerializableCollection::Serialize(MgStream* stream)
{
    INT32 size = GetCount();
    stream->WriteInt32(size);

    for (INT32 i = 0; i < size; ++i)
    {
        Ptr<MgSerializable> obj = GetItem(i);
        stream->WriteObject(obj);
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgSerializableCollection::Deserialize(MgStream* stream)
{
    INT32 size = 0;
    stream->GetInt32(size);

    for (INT32 i = 0; i < size; ++i)
    {
        Ptr<MgSerializable> obj = (MgSerializable*)stream->GetObject();
        Add(obj);
    }
}
