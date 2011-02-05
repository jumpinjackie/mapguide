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

MG_IMPL_DYNCREATE(MgBatchPropertyCollection);

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgBatchPropertyCollection.  The collection is initially empty.
/// </summary>
MgBatchPropertyCollection::MgBatchPropertyCollection()
{
    m_dCollection = new MgDisposableCollection();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the collection and the batch properties contained in it.
/// </summary>
MgBatchPropertyCollection::~MgBatchPropertyCollection()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgBatchPropertyCollection::GetClassId()
{
    return m_cls_id;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the item in the collection at the specified index to the specified value.
/// </summary>
/// <param name="index">Input index</param>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
/// <exception>InvalidArgument if index is out of range.</exception>
void MgBatchPropertyCollection::SetItem(INT32 index, MgPropertyCollection* value)
{
    m_dCollection->SetItem(index, value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns the index of the newly added item.
/// </returns>
void MgBatchPropertyCollection::Add(MgPropertyCollection* value)
{
    m_dCollection->Add(value);
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
void MgBatchPropertyCollection::Insert(INT32 index, MgPropertyCollection* value)
{
    m_dCollection->Insert(index, value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
/// <exception>InvalidArgument if the item does not exist within the collection.</exception>
bool MgBatchPropertyCollection::Remove(MgPropertyCollection* value)
{
    bool removed = true;
    try
    {
        m_dCollection->Remove(value);
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
/// Gets the item in the collection at the specified index.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>
/// Returns the item in the collection at the specified index.
/// </returns>
/// <exception>InvalidArgument if index is out of range.</exception>
MgPropertyCollection* MgBatchPropertyCollection::GetItem(INT32 index) const
{
    return ((MgPropertyCollection*)m_dCollection->GetItem(index));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of items in the collection.
/// </summary>
/// <returns>
/// Returns the number of items in the collection.
/// </returns>
INT32 MgBatchPropertyCollection::GetCount() const
{
    return m_dCollection->GetCount();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes all items from the collection.
/// </summary>
/// <returns>Returns nothing.</returns>
void MgBatchPropertyCollection::Clear()
{
    m_dCollection->Clear();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>Returns nothing.</returns>
/// <exception>MgIndexOutOfRangeException</exception>
void MgBatchPropertyCollection::RemoveAt(INT32 index)
{
    m_dCollection->RemoveAt(index);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgBatchPropertyCollection::Contains(const MgPropertyDefinition* value)
{
    return m_dCollection->Contains(value);
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
INT32 MgBatchPropertyCollection::IndexOf(const MgPropertyDefinition* value) const
{
    return m_dCollection->IndexOf(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates an XML document representing the collection.
/// </summary>
/// <returns>
/// Returns a pointer to an MgByteReader object.
/// </returns>
MgByteReader* MgBatchPropertyCollection::ToXml()
{
    string xmlStr = "<BatchPropertyCollection>";

    INT32 cnt = this->GetCount();
    for (int i=0; i < cnt; i++)
    {
        Ptr<MgPropertyCollection> ptr = (MgPropertyCollection*)m_dCollection->GetItem(i);
        if (ptr != NULL)
            ptr->ToXml(xmlStr);
    }

    xmlStr += "</BatchPropertyCollection>";

    return MgUtil::GetByteReader(xmlStr, (STRING*)&MgMimeType::Xml);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes data to a TCP/IP stream.
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgBatchPropertyCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    for (INT32 i = 0; i < count; i++)
        stream->WriteObject(Ptr<MgPropertyCollection>(this->GetItem(i)));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserializes data from a TCP/IP stream.
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgBatchPropertyCollection::Deserialize(MgStream* stream)
{
    INT32 count = 0;
    stream->GetInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgPropertyCollection> prop;
        prop = (MgPropertyCollection*)stream->GetObject();
        this->Add(prop);
    }
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes the object.
/// </summary>
void MgBatchPropertyCollection::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
// Convert to XML
void MgBatchPropertyCollection::ToXml(string& str)
{
    INT32 count = this->GetCount();
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgPropertyCollection> propDefCol = this->GetItem(i);
        propDefCol->ToXml(str);
    }
}
