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
#include "IntCollection.h"

MG_IMPL_DYNCREATE(MgIntCollection);

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgIntCollection.  Collection is initially empty.
/// </summary>
MgIntCollection::MgIntCollection()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes the collection.
/// </summary>
MgIntCollection::~MgIntCollection()
{
    m_intProperty.clear();
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgIntCollection::GetClassId()
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
INT32 MgIntCollection::GetCount() const
{
    return (INT32)m_intProperty.size();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the item in the collection at the specified index. Throws an invalid argument exception if the index is out of range.
/// </summary>
/// <param name="index">Input index</param>
/// <returns>
/// Returns the item in the collection at the specified index.
/// </returns>
INT32 MgIntCollection::GetItem(INT32 index) const
{
    ValidateIndex(index);
    return m_intProperty[index];
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
void MgIntCollection::SetItem(INT32 index, INT32 value)
{
    ValidateIndex(index);
    m_intProperty[index] = value;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified item to the end of the collection. Returns the index of the newly added item.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns the index of the newly added item.
/// </returns>
void MgIntCollection::Add(INT32 value)
{
    m_intProperty.push_back(value);
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
void MgIntCollection::Insert(INT32 index, INT32 value)
{
    ValidateIndex(index);
    m_intProperty.insert( m_intProperty.begin( ) + index, value );
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes all items from the collection.
/// </summary>
/// <returns>Returns nothing.</returns>
void MgIntCollection::Clear()
{
    m_intProperty.clear();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified item from the collection.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>Returns nothing.</returns>
/// EXCEPTIONS:
/// InvalidArgument if the item does not exist within the collection.
bool MgIntCollection::Remove(INT32 value)
{
    bool removed = true;
    try
    {
        INT32 index = this->IndexOf(value);
        ValidateIndex(index);
        m_intProperty.erase( m_intProperty.begin( ) + index );
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
void MgIntCollection::RemoveAt(INT32 index)
{
    ValidateIndex(index);
    m_intProperty.erase( m_intProperty.begin() + index );
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the collection contains the specified item, false otherwise.
/// </summary>
/// <param name="value">Input value</param>
/// <returns>
/// Returns true if the collection contains the specified item, false otherwise.
/// </returns>
bool MgIntCollection::Contains(INT32 value)
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
INT32 MgIntCollection::IndexOf(INT32 value)
{
    INT32 cnt = (INT32)m_intProperty.size();
    INT32 index = -1;

    for (INT32 i = 0; i < cnt; i++)
    {
        if (m_intProperty[i] == value)
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
MgByteReader* MgIntCollection::ToXml()
{
    // TODO: define a schema for this XML
    string xmlStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    xmlStr += "<IntCollection>";

    INT32 cnt = this->GetCount();
    for (int i=0; i < cnt; i++)
    {
        xmlStr += "<Item>";
        INT32 value = this->GetItem(i);

        char buff[64]; buff[0] = 0;
        sprintf(buff, "%d", (int)value);

        xmlStr += string(buff);
        xmlStr += "</Item>";
    }

    xmlStr += "</IntCollection>";

    return MgUtil::GetByteReader(xmlStr);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgIntCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    for (INT32 i = 0; i < count; i++)
        stream->WriteInt32(this->GetItem(i));
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream
/// </summary>
/// <param name="stream">Stream</param>
/// <returns>Returns nothing.</returns>
void MgIntCollection::Deserialize(MgStream* stream)
{
    INT32 count = 0;
    stream->GetInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        INT32 value;
        stream->GetInt32(value);
        this->Add(value);
    }
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes the object.
/// </summary>
void MgIntCollection::Dispose()
{
    delete this;
}


/// performs index validation
void MgIntCollection::ValidateIndex(INT32 index) const
{
    INT32 cnt = (INT32)m_intProperty.size();
    if (index >= cnt)
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgIntCollection.ValidateIndex",
            __LINE__, __WFILE__, &arguments, L"MgInvalidValueTooBig", NULL);
    }
    else if (index < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgIntCollection.ValidateIndex",
            __LINE__, __WFILE__, &arguments, L"MgInvalidValueTooSmall", NULL);
    }
}
