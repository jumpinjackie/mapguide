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
#include "PropertyDefinitionCollection.h"

MG_IMPL_DYNCREATE(MgPropertyDefinitionCollection);

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgPropertyDefinitionCollection.  Collection is initially empty.
/// </summary>
MgPropertyDefinitionCollection::MgPropertyDefinitionCollection()
{
    m_pCollection = new MgPropertyCollection();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgPropertyDefinitionCollection.  Collection is initially empty.
/// </summary>
/// <param name="allowDuplicate">Allow duplicate values if true</param>
/// <param name="caseSensitive">true - "Abc" will be different from "ABC"</param>
MgPropertyDefinitionCollection::MgPropertyDefinitionCollection( bool allowDuplicate, bool caseSensitive )
{
    m_pCollection = new MgPropertyCollection(allowDuplicate, caseSensitive);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the collection and the contained property definitions.
/// </summary>
MgPropertyDefinitionCollection::~MgPropertyDefinitionCollection()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgPropertyDefinitionCollection::GetClassId()
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
void MgPropertyDefinitionCollection::SetItem(INT32 index, MgPropertyDefinition* value)
{
    m_pCollection->SetItem(index, value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns nothing
/// </returns>
void MgPropertyDefinitionCollection::Add(MgPropertyDefinition* value)
{
    m_pCollection->Add(value);
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
void MgPropertyDefinitionCollection::Insert(INT32 index, MgPropertyDefinition* value)
{
    m_pCollection->Insert(index, value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns true if successful.</returns>
bool MgPropertyDefinitionCollection::Remove(MgPropertyDefinition* value)
{
    bool removed = true;
    try
    {
        removed = m_pCollection->Remove(value);
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
MgPropertyDefinition* MgPropertyDefinitionCollection::GetItem(INT32 index) const
{
    return ((MgPropertyDefinition*)m_pCollection->GetItem(index));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection with the specified name. Throws an exception if the item is not found.
/// </summary>
/// <param name="name">Input item name</param>
/// <returns>
/// Returns the item in the collection with the specified name.
/// </returns>
MgPropertyDefinition* MgPropertyDefinitionCollection::GetItem(CREFSTRING name)
{
    return ((MgPropertyDefinition*)m_pCollection->GetItem(name));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of items in the collection.
/// </summary>
/// <returns>
/// Returns number of items in the collection.
/// </returns>
INT32 MgPropertyDefinitionCollection::GetCount() const
{
    return m_pCollection->GetCount();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes all items from the collection.
/// </summary>
/// <returns>Returns nothing</returns>
void MgPropertyDefinitionCollection::Clear()
{
    m_pCollection->Clear();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection. Throws an invalid argument exception if the item does not exist within the collection.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>Returns nothing.</returns>
/// EXCEPTIONS:
/// MgIndexOutOfRangeException
void MgPropertyDefinitionCollection::RemoveAt(INT32 index)
{
    m_pCollection->RemoveAt(index);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgPropertyDefinitionCollection::Contains(const MgPropertyDefinition* value)
{
    return m_pCollection->Contains(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="name">Input the item name</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgPropertyDefinitionCollection::Contains(CREFSTRING name)
{
    return m_pCollection->Contains(name);
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
INT32 MgPropertyDefinitionCollection::IndexOf(const MgPropertyDefinition* value) const
{
    return m_pCollection->IndexOf(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the index of the specified item (by name) in the collection or -1 if the item does not exist.
/// </summary>
/// <param name="name">Input the item name</param>
/// <returns>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </returns>
INT32 MgPropertyDefinitionCollection::IndexOf(CREFSTRING name) const
{
    return m_pCollection->IndexOf(name);
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
MgPropertyDefinition* MgPropertyDefinitionCollection::FindItem(CREFSTRING name)
{
    return ((MgPropertyDefinition*)m_pCollection->FindItem(name));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates an XML document representing the collection.
/// </summary>
/// <returns>
/// Returns a pointer to an MgByteReader object.
/// </returns>
MgByteReader* MgPropertyDefinitionCollection::ToXml()
{
    string xmlStr;
    this->ToXml(xmlStr);
    return MgUtil::GetByteReader(xmlStr, (STRING*)&MgMimeType::Xml);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgPropertyDefinitionCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgPropertyDefinition> prop = this->GetItem(i);
        stream->WriteObject(prop);
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgPropertyDefinitionCollection::Deserialize(MgStream* stream)
{
    INT32 count = 0;
    stream->GetInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgPropertyDefinition> prop = (MgPropertyDefinition*)stream->GetObject();
        this->Add(prop);
    }
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes the object.
/// </summary>
void MgPropertyDefinitionCollection::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// Convert to XML
void MgPropertyDefinitionCollection::ToXml(string &xmlStr)
{
    xmlStr += "<PropertyDefinitions>";

    INT32 cnt = this->GetCount();
    for (int i=0; i < cnt; i++)
    {
        Ptr<MgPropertyDefinition> ptr = (MgPropertyDefinition*)m_pCollection->GetItem(i);
        if (ptr != NULL)
            ptr->ToXml(xmlStr);
    }

    xmlStr += "</PropertyDefinitions>";
}


//////////////////////////////////////////////////////////////////
/// Convert to Column
void MgPropertyDefinitionCollection::ToColumnDefinitions(string &xmlStr)
{
    xmlStr += "<ColumnDefinitions>";

    INT32 cnt = this->GetCount();
    for (int i=0; i < cnt; i++)
    {
        Ptr<MgPropertyDefinition> ptr = (MgPropertyDefinition*)m_pCollection->GetItem(i);
        if (ptr != NULL)
            ptr->ToColumnDefinition(xmlStr);
    }

    xmlStr += "</ColumnDefinitions>";
}
