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

MG_IMPL_DYNCREATE(MgPropertyCollection);

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgPropertyCollection.  Collection is initially empty.
/// </summary>
MgPropertyCollection::MgPropertyCollection()
{
    m_nCollection = new MgNamedCollection();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgPropertyCollection.  Collection is initially empty.
/// </summary>
/// <param name="allowDuplicate">Allow duplicate values if true</param>
/// <param name="caseSensitive">true - "Abc" will be different from "ABC"</param>
MgPropertyCollection::MgPropertyCollection( bool allowDuplicate, bool caseSensitive )
{
    m_nCollection = new MgNamedCollection(allowDuplicate, caseSensitive);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the collection and the contained properties.
/// </summary>
MgPropertyCollection::~MgPropertyCollection()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgPropertyCollection::GetClassId()
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
/// EXCEPTIONS:
/// InvalidArgument if index is out of range
void MgPropertyCollection::SetItem(INT32 index, MgProperty* value)
{
    m_nCollection->SetItem(index, value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns nothing
/// </returns>
void MgPropertyCollection::Add(MgProperty* value)
{
    m_nCollection->Add(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Inserts the specified item at the specified index within the collection.
/// Items following the insertion point are moved down to accommodate the new item.
/// </summary>
/// <param name="index">Input index</param>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
/// EXCEPTIONS:
/// InvalidArgument if index is out of range
void MgPropertyCollection::Insert(INT32 index, MgProperty* value)
{
    m_nCollection->Insert(index, value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns true if successful</returns>
///
bool MgPropertyCollection::Remove(MgProperty* value)
{
    bool removed = true;
    try
    {
        removed = m_nCollection->Remove(value);
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
/// Gets the item in the collection at the specified index. Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>
/// Returns the item in the collection at the specified index.
/// </returns>
MgProperty* MgPropertyCollection::GetItem(INT32 index) const
{
    return ((MgProperty*)m_nCollection->GetItem(index));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection with the specified name. Throws an exception if the item is not found.
/// </summary>
/// <param name="name">Input item name</param>
/// <returns>
/// Returns the item in the collection with the specified name.
/// </returns>
MgProperty* MgPropertyCollection::GetItem(CREFSTRING name)
{
    return ((MgProperty*)m_nCollection->GetItem(name));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Finds the item in the collection with the specified name.
/// </summary>
/// <param name="name">Input item name</param>
/// <returns>
/// Returns the item in the collection with the specified name.
/// Returns NULL if the item was not found.
/// </returns>
MgProperty* MgPropertyCollection::FindItem(CREFSTRING name)
{
    return ((MgProperty*)m_nCollection->FindItem(name));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of items in the collection.
/// </summary>
/// <returns>
/// Returns number of items in the collection.
/// </returns>
INT32 MgPropertyCollection::GetCount() const
{
    return m_nCollection->GetCount();
}


//////////////////////////////////////////////////////////
/// <summary>
/// Removes all items from the collection.
/// </summary>
/// <returns>Returns nothing.</returns>
void MgPropertyCollection::Clear()
{
    m_nCollection->Clear();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>Returns nothing.</returns>
/// EXCEPTIONS:
/// MgIndexOutOfRangeException
void MgPropertyCollection::RemoveAt(INT32 index)
{
    m_nCollection->RemoveAt(index);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgPropertyCollection::Contains(const MgProperty* value)
{
    return m_nCollection->Contains(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="name">Input the item name</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgPropertyCollection::Contains(CREFSTRING name)
{
    return m_nCollection->Contains(name);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </returns>
/// EXCEPTIONS:
/// MgInvalidArgumentException
INT32 MgPropertyCollection::IndexOf(const MgProperty* value) const
{
    return m_nCollection->IndexOf(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the index of the specified item (by name) in the collection or -1 if the item does not exist.
/// </summary>
/// <param name="name">Input the item name</param>
/// <returns>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </returns>
INT32 MgPropertyCollection::IndexOf(CREFSTRING name) const
{
    return m_nCollection->IndexOf(name);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates an XML document representing the collection.
/// </summary>
/// <returns>
/// Returns a pointer to an MgByteReader object.
/// </returns>
MgByteReader* MgPropertyCollection::ToXml()
{
    string xmlStr;
    ToXml(xmlStr);
    return MgUtil::GetByteReader(xmlStr);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgPropertyCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    bool bAllowDuplicate = m_nCollection->GetAllowDuplicate();
    stream->WriteBoolean(bAllowDuplicate);
    bool bCaseSensitive = m_nCollection->GetCaseSenstive();
    stream->WriteBoolean(bCaseSensitive);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgProperty> prop = this->GetItem(i);
        stream->WriteObject(prop);
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgPropertyCollection::Deserialize(MgStream* stream)
{
    INT32 count = 0;
    stream->GetInt32(count);
    bool bAllowDuplicate = false;
    stream->GetBoolean(bAllowDuplicate);
    bool bCaseSensitive = false;
    stream->GetBoolean(bCaseSensitive);

    if (m_nCollection)
    {
        m_nCollection = NULL;
    }

    m_nCollection = new MgNamedCollection(bAllowDuplicate, bCaseSensitive);

    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgProperty> prop = (MgProperty*)stream->GetObject();
        this->Add(prop);
    }
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes the object.
/// </summary>
void MgPropertyCollection::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
// Convert to XML
void MgPropertyCollection::ToXml(string& xmlStr, bool includeType, string rootElmName)
{
    INT32 cnt = this->GetCount();
    for (int i=0; i < cnt; i++)
    {
        Ptr<MgProperty> ptr = (MgProperty*)m_nCollection->GetItem(i);
        if (ptr != NULL)
            ptr->ToXml(xmlStr, includeType, rootElmName);
    }
}


//////////////////////////////////////////////////////////////////
// Convert to XML
void MgPropertyCollection::ToXml(string& xmlStr)
{
    xmlStr += "<PropertyCollection>";

    this->ToXml(xmlStr, true);

    xmlStr += "</PropertyCollection>";
}


//////////////////////////////////////////////////////////////////
// Convert to Row
void MgPropertyCollection::ToRow(string& xmlStr)
{
    xmlStr += "<Row>";

    this->ToXml(xmlStr, false, "Column");

    xmlStr += "</Row>";
}


//////////////////////////////////////////////////////////////////
// Convert to Feature
void MgPropertyCollection::ToFeature(string& xmlStr)
{
    xmlStr += "<Feature>";

    this->ToXml(xmlStr, false);

    xmlStr += "</Feature>";
}
