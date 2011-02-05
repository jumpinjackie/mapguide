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
#include "FeatureService.h"

MG_IMPL_DYNCREATE(MgFeatureSchemaCollection);


///<summary>Gets the number of items in the collection.</summary>
/// <returns>Returns the number of items in the collection.</returns>
INT32 MgFeatureSchemaCollection::GetCount() const
{
    return m_dCollection->GetCount();
}

///<summary>Gets the item in the collection at the specified index.</summary>
/// <param name="index">Input index</param>
/// <returns>Returns the item in the collection at the specified index.</returns>
///<exception>
///InvalidArgument if index is out of range.</exception>
MgFeatureSchema* MgFeatureSchemaCollection::GetItem(INT32 index) const
{
    return ((MgFeatureSchema*)m_dCollection->GetItem(index));
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
void MgFeatureSchemaCollection::SetItem(INT32 index, MgFeatureSchema* value)
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
void MgFeatureSchemaCollection::Add(MgFeatureSchema* value)
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
void MgFeatureSchemaCollection::Insert(INT32 index, MgFeatureSchema* value)
{
    m_dCollection->Insert(index, value);
}


///<summary>Removes all items from the collection.</summary>
/// <returns>Returns nothing.</returns>
void MgFeatureSchemaCollection::Clear()
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
bool MgFeatureSchemaCollection::Remove(MgFeatureSchema* value)
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
void MgFeatureSchemaCollection::RemoveAt(INT32 index)
{
    m_dCollection->RemoveAt(index);
}

///<summary>Returns true if the collection contains the specified item, false otherwise.</summary>
/// <param name="value">Input value</param>
/// <returns>Returns true if the collection contains the specified item, false otherwise.</returns>
bool MgFeatureSchemaCollection::Contains(const MgFeatureSchema* value) const
{
    return m_dCollection->Contains(value);
}

///<summary>Returns the index of the specified item in the collection or -1 if the item does not exist.</summary>
/// <param name="value">Input value</param>
/// <returns>Returns the index of the specified item in the collection or -1 if the item does not exist.</returns>
/// <exception>
/// MgInvalidArgumentException</exception>
INT32 MgFeatureSchemaCollection::IndexOf(const MgFeatureSchema* value) const
{
    return m_dCollection->IndexOf(value);
}

//////////////////////////////////////////////////////////
///<summary>
///Constructs a PropertyCollection.  The collection is initially empty.
///</summary>
MgFeatureSchemaCollection::MgFeatureSchemaCollection()
{
    m_dCollection = new MgDisposableCollection();
}

//////////////////////////////////////////////////////////
///<summary>
///Deletes the collection and the properties contained in it.
///</summary>
MgFeatureSchemaCollection::~MgFeatureSchemaCollection()
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serializes data to a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgFeatureSchemaCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    for (INT32 i = 0; i < count; i++)
        stream->WriteObject(Ptr<MgFeatureSchema>(this->GetItem(i)));
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserializes data from a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgFeatureSchemaCollection::Deserialize(MgStream* stream)
{
    INT32 count = 0;
    stream->GetInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgFeatureSchema> prop;
        prop = (MgFeatureSchema*)stream->GetObject();
        this->Add(prop);
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgFeatureSchemaCollection::GetClassId()
{
    return m_cls_id;
}

void MgFeatureSchemaCollection::Dispose()
{
    delete this;
}

MgByteReader* MgFeatureSchemaCollection::ToXml()
{
    throw new MgNotImplementedException(L"FeatureSchemaCollection.ToXml", __LINE__, __WFILE__, NULL, L"", NULL);
}
