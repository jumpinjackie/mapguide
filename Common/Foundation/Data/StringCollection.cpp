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

MG_IMPL_DYNCREATE(MgStringCollection);

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgStringCollection.  Collection is initially empty.
/// </summary>
MgStringCollection::MgStringCollection()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the collection and the contained strings.
/// </summary>
MgStringCollection::~MgStringCollection()
{
    m_strProperty.clear();
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
    return (INT32)m_strProperty.size();
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
    ValidateIndex(index);
    return m_strProperty[index];
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the item in the collection at the specified index to the specified value.
/// </summary>
/// <param name="index">Input index</param>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
/// EXCEPTIONS:
/// \exception MgIndexOutOfRangeException if the index is out of range.
void MgStringCollection::SetItem(INT32 index, CREFSTRING value)
{
    ValidateIndex(index);
    m_strProperty[index] = value;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns nothing.
/// </returns>
void MgStringCollection::Add(CREFSTRING value)
{
    m_strProperty.push_back(value);
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
/// exception MgIndexOutOfRangeException if the index is out of range.
void MgStringCollection::Insert(INT32 index, CREFSTRING value)
{
    ValidateIndex(index);
    m_strProperty.insert( m_strProperty.begin( ) + index, value );
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes all items from the collection.
/// </summary>
/// <returns>Returns nothing.</returns>
void MgStringCollection::Clear()
{
    m_strProperty.clear();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns true if successful</returns>
///
bool MgStringCollection::Remove(CREFSTRING value)
{
    bool removed = true;
    try
    {
        INT32 index = this->IndexOf(value);
        ValidateIndex(index);
        m_strProperty.erase( m_strProperty.begin( ) + index );
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
/// exception MgIndexOutOfRangeException if the index is out of range.
void MgStringCollection::RemoveAt(INT32 index)
{
    ValidateIndex(index);
    m_strProperty.erase( m_strProperty.begin() + index );
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
    bool contains = false;
    if (this->IndexOf(value) > -1)
        contains = true;

    return contains;
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
    INT32 cnt = (INT32)m_strProperty.size();
    INT32 index = -1;

    for (INT32 i = 0; i < cnt; i++)
    {
        if (m_strProperty[i] == value)
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
MgByteReader* MgStringCollection::ToXml()
{
    // TODO: define a schema for this XML
    string xmlStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    xmlStr += "<StringCollection>";

    INT32 cnt = this->GetCount();
    for (int i=0; i < cnt; i++)
    {
        xmlStr += "<Item>";
        xmlStr += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(m_strProperty[i]));
        xmlStr += "</Item>";
    }

    xmlStr += "</StringCollection>";

    return MgUtil::GetByteReader(xmlStr, (STRING*)&MgMimeType::Xml);
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
/// Disposes the object.
/// </summary>
void MgStringCollection::Dispose()
{
    delete this;
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

    MgStringCollection* c = new MgStringCollection();
    if (c == NULL)
        return NULL;

    STRING separator = delim.length() == 0? L",": delim;
    size_t delimPos, currPos = 0;
    bool done = false;
    STRING elem;
    while(!done)
    {
        delimPos = collection.find_first_of(separator, currPos);
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


/// performs index validation
void MgStringCollection::ValidateIndex(INT32 index) const
{
    INT32 cnt = (INT32)m_strProperty.size();
    if (index >= cnt)
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgIndexOutOfRangeException(L"MgStringCollection.ValidateIndex",
            __LINE__, __WFILE__, &arguments, L"MgInvalidValueTooBig", NULL);
    }
    else if (index < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgIndexOutOfRangeException(L"MgStringCollection.ValidateIndex",
            __LINE__, __WFILE__, &arguments, L"MgInvalidValueTooSmall", NULL);
    }
}

STRING MgStringCollection::GetLogString()
{
    STRING tmp = L"";
    if (m_strProperty.size() == 0) return tmp;

    tmp = L"{";;
    for (INT32 i=0; i < (INT32) m_strProperty.size(); i++)
    {
        tmp += L"{";
        tmp += m_strProperty[i];
        tmp += L"}";
    }
    tmp += L"}";
    return tmp;
}

