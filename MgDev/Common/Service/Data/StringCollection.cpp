//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "AceCommon.h"

IMPLEMENT_DYNCREATE(MgStringCollection);

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgStringCollection.  Collection is initially empty.
/// </summary>
MgStringCollection::MgStringCollection()
{
    m_strProperty = new MgPropertyCollection(true,true);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the collection and the contained strings.
/// </summary>
MgStringCollection::~MgStringCollection()
{
    m_strProperty->Clear();
    delete m_strProperty;
    m_strProperty = NULL;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgStringCollection::GetClassId()
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
INT32 MgStringCollection::GetCount() const
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
STRING MgStringCollection::GetItem(INT32 index) const
{
    STRING str;
    Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->GetItem(index);
    if (strPtr != NULL)
        str = strPtr->GetName();
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
void MgStringCollection::SetItem(INT32 index, CREFSTRING value)
{
    Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->GetItem(index);
    if (strPtr != NULL)
        strPtr->SetName(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection. Returns the index of the newly added item.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns the index of the newly added item.
/// </returns>
INT32 MgStringCollection::Add(CREFSTRING value)
{
    Ptr<MgStringProperty> strPtr = new MgStringProperty(value, L"");
    INT32 cnt = m_strProperty->Add(strPtr);
    return cnt;
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
void MgStringCollection::Insert(INT32 index, CREFSTRING value)
{
    Ptr<MgStringProperty> strPtr = new MgStringProperty(value,L"");
    m_strProperty->Insert(index, strPtr);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes all items from the collection.
/// </summary>
/// <returns>Returns nothing.</returns>
void MgStringCollection::Clear()
{
    m_strProperty->Clear();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
/// EXCEPTIONS:
/// InvalidArgument if the item does not exist within the collection.
void MgStringCollection::Remove(CREFSTRING value)
{
    Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->FindItem(value);
    if (strPtr != NULL)
        m_strProperty->Remove(strPtr);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>Returns nothing.</returns>
/// EXCEPTIONS:
/// InvalidArgument if the item does not exist within the collection.
void MgStringCollection::RemoveAt(INT32 index)
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
bool MgStringCollection::Contains(CREFSTRING value)
{
    return m_strProperty->Contains(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns the index of the specified item in the collection or -1 if the item does not exist.
/// </returns>
INT32 MgStringCollection::IndexOf(CREFSTRING value)
{
    return m_strProperty->IndexOf(value);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates an XML document representing the collection.
/// </summary>
/// <returns>
/// A pointer to an MgByteReader object
/// </returns>
MgByteReader* MgStringCollection::ToXml()
{
    // TODO: define a schema for this XML
    string xmlStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    xmlStr += "<StringCollection>";

    INT32 cnt = this->GetCount();
    for (int i=0; i < cnt; i++)
    {
        Ptr<MgStringProperty> strPtr = (MgStringProperty*)m_strProperty->GetItem(i);
        if (strPtr != NULL)
            strPtr->ToXml(xmlStr, true, false);
    }

    xmlStr += "</StringCollection>";

    return MgUtil::GetByteReader(xmlStr);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes the object.
/// </summary>
void MgStringCollection::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgStringCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    for (INT32 i = 0; i < count; i++)
        stream->WriteString(this->GetItem(i));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgStringCollection::Deserialize(MgStream* stream)
{
    INT32 count = 0;
    stream->GetInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        STRING str;
        stream->GetString(str);
        this->Add(str);
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates a string collection from parsing a string
/// Each element in the string is separated with the specified
/// delimiter. If no delimiter is specified, comma is used
/// as separator
/// </summary>
/// <param name="coll">String encoded collection</param>
/// <param name="delim">String delimiter. Comma is used if delim is empty</param>
/// <returns>
/// A collection object.
/// </returns>
MgStringCollection* MgStringCollection::ParseCollection(CREFSTRING coll, CREFSTRING delim)
{
    STRING collection = MgUtil::Trim(coll);
    if(collection.length() == 0)
        return NULL;

    MgStringCollection* c = new MgStringCollection;
    if(c == NULL)
        return NULL;

    STRING separator = delim.length() == 0? L",": delim;
    unsigned int delimPos, currPos = 0;
    bool done = false;
    STRING elem;
    while(!done)
    {
        delimPos = (int)collection.find_first_of(separator, currPos);
        if(delimPos == wstring::npos)
        {
            elem = collection.substr(currPos);
            done = true;
        }
        else
        {
            elem = collection.substr(currPos, delimPos - currPos);
            currPos = delimPos + 1;
        }

        elem = MgUtil::Trim(elem);
        if(elem.length() > 0)
            c->Add(elem);
    }

    return c;
}
