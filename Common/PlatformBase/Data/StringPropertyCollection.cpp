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

MG_IMPL_DYNCREATE(MgStringPropertyCollection)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgStringPropertyCollection.  Collection is initially empty.
/// </summary>
MgStringPropertyCollection::MgStringPropertyCollection()
{
    m_strProperty = new MgPropertyCollection();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgStringPropertyCollection.  Collection is initially empty.
/// </summary>
MgStringPropertyCollection::MgStringPropertyCollection(bool allowDuplicate, bool caseSensitive)
{
    m_strProperty = new MgPropertyCollection(allowDuplicate, caseSensitive);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the collection and the contained string properties.
/// </summary>
MgStringPropertyCollection::~MgStringPropertyCollection()
{
    m_strProperty->Clear();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgStringPropertyCollection::GetClassId()
{
    return m_cls_id;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of items in the collection.
/// </summary>
/// <returns>
/// Returns number of items in the collection.
/// </returns>
INT32 MgStringPropertyCollection::GetCount() const
{
    return m_strProperty->GetCount();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection at the specified index. Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>
/// Returns the item in the collection at the specified index.
/// </returns>
STRING MgStringPropertyCollection::GetName(INT32 index) const
{
    STRING str;
    Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->GetItem(index);
    if (strPtr != NULL)
    str = strPtr->GetName();
    return str;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection at the specified index. Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>
/// Returns the item in the collection at the specified index.
/// </returns>
STRING MgStringPropertyCollection::GetValue(INT32 index) const
{
    STRING str;
    Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->GetItem(index);
    if (strPtr != NULL)
         str = strPtr->GetValue();
    return str;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection at the specified index. Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>
/// Returns the item in the collection at the specified index.
/// </returns>
STRING MgStringPropertyCollection::GetValue(CREFSTRING keyname) const
{
    STRING str;
    Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->GetItem(keyname);
    if (strPtr != NULL)
    str = strPtr->GetValue();
    return str;
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
void MgStringPropertyCollection::SetValue(CREFSTRING keyname, CREFSTRING value)
{
    Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->GetItem(keyname);
    if (strPtr != NULL)
    strPtr->SetValue(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection.
/// </summary>
/// <param name="keyname">Input key</param>
/// <param name="value">Input value</param>
/// <returns>
/// Returns nothing.
/// </returns>
void MgStringPropertyCollection::Add(CREFSTRING keyname, CREFSTRING value)
{
    Ptr<MgStringProperty> strPtr = new MgStringProperty(keyname, value);
    m_strProperty->Add(strPtr);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection.
/// </summary>
/// <param name="keyname">Input key</param>
/// <param name="value">Input value</param>
/// <returns>
/// Returns nothing.
/// </returns>
void MgStringPropertyCollection::Add(MgStringProperty* value)
{
    m_strProperty->Add(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection at the specified index. Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>
/// Returns the item in the collection at the specified index.
/// </returns>
MgStringProperty* MgStringPropertyCollection::GetItem(INT32 index)
{
    Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->GetItem(index);
    return SAFE_ADDREF((MgStringProperty*)strPtr);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the item in the collection at the specified index. Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <param name="value">Value to set</param>
/// <returns>
/// Returns nothing.
/// </returns>
void MgStringPropertyCollection::SetItem(INT32 index, MgStringProperty* value)
{
    if (NULL != value)
    {
        m_strProperty->SetItem(index, value);
    }
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
void MgStringPropertyCollection::Insert(INT32 index, CREFSTRING keyname, CREFSTRING value)
{
    Ptr<MgStringProperty> strPtr = new MgStringProperty(keyname, value);
    m_strProperty->Insert(index, strPtr);
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
void MgStringPropertyCollection::Insert(INT32 index, MgStringProperty* value)
{
    m_strProperty->Insert(index, value);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes all items from the collection.
/// </summary>
/// <returns>Returns nothing.</returns>
void MgStringPropertyCollection::Clear()
{
    m_strProperty->Clear();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
bool MgStringPropertyCollection::Remove(CREFSTRING keyname)
{
    bool removed = false;
    try
    {
        Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->FindItem(keyname);
        if (strPtr != NULL)
        {
            m_strProperty->Remove(strPtr);
            removed = true;
        }
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
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
bool MgStringPropertyCollection::Remove(MgStringProperty* value)
{
    bool removed = true;
    try
    {
        removed = m_strProperty->Remove(value);
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
/// Removes the specified item from the collection.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>Returns nothing.</returns>
/// EXCEPTIONS:
/// InvalidArgument if the item does not exist within the collection.
void MgStringPropertyCollection::RemoveAt(INT32 index)
{
    m_strProperty->RemoveAt(index);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgStringPropertyCollection::Contains(CREFSTRING keyname)
{
    return m_strProperty->Contains(keyname);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgStringPropertyCollection::Contains(MgStringProperty* value)
{
    if (NULL == value)
    {
        return false;
    }
    return m_strProperty->Contains(value->GetName());
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </returns>
INT32 MgStringPropertyCollection::IndexOf(CREFSTRING keyname)
{
    return m_strProperty->IndexOf(keyname);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </returns>
INT32 MgStringPropertyCollection::IndexOf(MgStringProperty* value)
{
    if (NULL == value)
    {
        return -1;
    }
    return m_strProperty->IndexOf(value->GetName());
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates an XML document representing the collection.
/// </summary>
/// <returns>
/// Returns a pointer to an MgByteReader object.
/// </returns>
MgByteReader* MgStringPropertyCollection::ToXml()
{
    string xmlStr = "<StringPropertyCollection>";

    INT32 cnt = this->GetCount();
    for (int i=0; i < cnt; i++)
    {
        Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->GetItem(i);
        if (strPtr != NULL)
            strPtr->ToXml(xmlStr, false, false);
    }

    xmlStr += "</StringPropertyCollection>";

    return MgUtil::GetByteReader(xmlStr);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes the object.
/// </summary>
void MgStringPropertyCollection::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgStringPropertyCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    for (INT32 i = 0; i < count; i++)
        stream->WriteObject(Ptr<MgStringProperty>(this->GetItem(i)));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgStringPropertyCollection::Deserialize(MgStream* stream)
{
    INT32 count = 0;
    stream->GetInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgStringProperty> prop;
        prop = (MgStringProperty*)stream->GetObject();
        this->Add(prop);
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property collection this string collection is based upon.
/// </summary>
MgPropertyCollection* MgStringPropertyCollection::GetPropertyCollection()
{
    return SAFE_ADDREF((MgPropertyCollection*)m_strProperty);
}

STRING MgStringPropertyCollection::GetLogString()
{
    STRING tmp = L"";
    if (m_strProperty->GetCount() == 0) return tmp;

    tmp = L"{";
    for (int i=0; i < m_strProperty->GetCount(); i++)
    {
        Ptr<MgStringProperty> prop = dynamic_cast<MgStringProperty*>(m_strProperty->GetItem(i));
        ACE_ASSERT(NULL != prop.p);
        tmp += L"{";
        tmp += prop->GetName();
        tmp += L"=";
        tmp += prop->GetValue();
        tmp += L"}";
    }
    tmp += L"}";
    return tmp;
}
