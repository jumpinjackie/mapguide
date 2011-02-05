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

#include "PlatformBase.h"

MG_IMPL_DYNCREATE(MgPropertyMappingCollection);

//////////////////////////////////////////////////////////
///<summary>
///Constructs a PropertyMappingCollection.  The collection is initially empty.
///</summary>
MgPropertyMappingCollection::MgPropertyMappingCollection()
{
    m_dCollection = new MgDisposableCollection();
}

//////////////////////////////////////////////////////////
///<summary>
///Deletes the collection and the properties contained in it.
///</summary>
MgPropertyMappingCollection::~MgPropertyMappingCollection()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgPropertyMappingCollection::GetClassId()
{
    return m_cls_id;
}

void MgPropertyMappingCollection::Dispose()
{
    delete this;
}
//////////////////////////////////////////////////////////////////
///<summary>
/// Serializes data to a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgPropertyMappingCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    for (INT32 i = 0; i < count; i++)
        stream->WriteObject(Ptr<MgPropertyMapping>(this->GetItem(i)));
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserializes data from a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgPropertyMappingCollection::Deserialize(MgStream* stream)
{
    INT32 count = 0;
    stream->GetInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgPropertyMapping> prop;
        prop = (MgPropertyMapping*)stream->GetObject();
        this->Add(prop);
    }
}

///<summary>Gets the number of items in the collection.</summary>
/// <returns>Returns the number of items in the collection.</returns>
INT32 MgPropertyMappingCollection::GetCount() const
{
    return m_dCollection->GetCount();
}

///<summary>Gets the item in the collection at the specified index.</summary>
/// <param name="index">Input index</param>
/// <returns>Returns the item in the collection at the specified index.</returns>
///<exception>
///InvalidArgument if index is out of range.</exception>
MgPropertyMapping* MgPropertyMappingCollection::GetItem(INT32 index) const
{
    return ((MgPropertyMapping*)m_dCollection->GetItem(index));
}

//////////////////////////////////////////////////////////
///<summary>
///Sets the item in the collection at the specified index to the specified value.
///</summary>
///<param name="index">Input index</param>
///<param name="value">Input value</param>
///<returns>
///Returns nothing.
///</returns>
///<exception>
///InvalidArgument if index is out of range.</exception>
void MgPropertyMappingCollection::SetItem(INT32 index, MgPropertyMapping* value)
{
    m_dCollection->SetItem(index, value);
}

//////////////////////////////////////////////////////////
///<summary>
///Adds the specified item to the end of the collection.
///</summary>
///<param name="value">Input value</param>
///<returns>
///Returns nothing
///</returns>
void MgPropertyMappingCollection::Add(MgPropertyMapping* value)
{
    m_dCollection->Add(value);
}

//////////////////////////////////////////////////////////
///<summary>
///Inserts the specified item at the specified index within the collection.
///Items following the insertion point are moved down to accommodate the new item.
///</summary>
///<param name="index">Input index</param>
///<param name="value">Input value</param>
///<returns>
///Returns nothing.
///</returns>
///<exception>
///InvalidArgument if index is out of range.</exception>
void MgPropertyMappingCollection::Insert(INT32 index, MgPropertyMapping* value)
{
    m_dCollection->Insert(index, value);
}


///<summary>Removes all items from the collection.</summary>
/// <returns>Returns nothing.</returns>
void MgPropertyMappingCollection::Clear()
{
    m_dCollection->Clear();
}


//////////////////////////////////////////////////////////
///<summary>
///Removes the specified item from the collection.
///</summary>
///<param name="value">Input value</param>
///<returns>
///Returns nothing.
///</returns>
///<exception>
///InvalidArgument if the item does not exist within the collection.</exception>
bool MgPropertyMappingCollection::Remove(MgPropertyMapping* value)
{
    bool removed = true;
    try
    {
       removed = m_dCollection->Remove(value);
    }
    catch (MgException* e)
    {
        e->Release();
        removed = false;
    }
    return removed;
}


///<summary>Removes the specified item from the collection.</summary>
/// <param name="index">Input index</param>
/// <returns>Returns nothing.</returns>
/// <exception>
/// MgIndexOutOfRangeException</exception>
void MgPropertyMappingCollection::RemoveAt(INT32 index)
{
    m_dCollection->RemoveAt(index);
}

///<summary>Returns true if the collection contains the specified item, false otherwise.</summary>
/// <param name="value">Input value</param>
/// <returns>Returns true if the collection contains the specified item, false otherwise.</returns>
bool MgPropertyMappingCollection::Contains(const MgPropertyMapping* value) const
{
    return m_dCollection->Contains(value);
}

///<summary>Returns the index of the specified item in the collection or -1 if the item does not exist.</summary>
/// <param name="value">Input value</param>
/// <returns>Returns the index of the specified item in the collection or -1 if the item does not exist.</returns>
/// <exception>
/// MgInvalidArgumentException</exception>
INT32 MgPropertyMappingCollection::IndexOf(const MgPropertyMapping* value) const
{
    return m_dCollection->IndexOf(value);
}

MgByteReader* MgPropertyMappingCollection::ToXml()
{
    throw new MgNotImplementedException(L"PropertyMappingCollection.ToXml", __LINE__, __WFILE__, NULL, L"", NULL);
}

