//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

MG_IMPL_DYNCREATE(MgClassDefinitionCollection);

//////////////////////////////////////////////////////////
///<summary>
///Constructs a ClassDefinitionCollection.  The collection is initially empty.
///</summary>
MgClassDefinitionCollection::MgClassDefinitionCollection()
{
    m_dCollection = new MgDisposableCollection();
}

//////////////////////////////////////////////////////////
///<summary>
///Deletes the collection and the properties contained in it.
///</summary>
MgClassDefinitionCollection::~MgClassDefinitionCollection()
{
}

///<summary>Gets the number of items in the collection.</summary>
/// <returns>Returns the number of items in the collection.</returns>
INT32 MgClassDefinitionCollection::GetCount() const
{
    return m_dCollection->GetCount();
}

///<summary>Gets the item in the collection at the specified index.</summary>
/// <param name="index">Input index</param>
/// <returns>Returns the item in the collection at the specified index.</returns>
///<exception>
///InvalidArgument if index is out of range.</exception>
MgClassDefinition* MgClassDefinitionCollection::GetItem(INT32 index) const
{
    return ((MgClassDefinition*)m_dCollection->GetItem(index));
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
void MgClassDefinitionCollection::SetItem(INT32 index, MgClassDefinition* value)
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
void MgClassDefinitionCollection::Add(MgClassDefinition* value)
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
void MgClassDefinitionCollection::Insert(INT32 index, MgClassDefinition* value)
{
    m_dCollection->Insert(index, value);
}


///<summary>Removes all items from the collection.</summary>
/// <returns>Returns nothing.</returns>
void MgClassDefinitionCollection::Clear()
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
bool MgClassDefinitionCollection::Remove(MgClassDefinition* value)
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
void MgClassDefinitionCollection::RemoveAt(INT32 index)
{
    m_dCollection->RemoveAt(index);
}

///<summary>Returns true if the collection contains the specified item, false otherwise.</summary>
/// <param name="value">Input value</param>
/// <returns>Returns true if the collection contains the specified item, false otherwise.</returns>
bool MgClassDefinitionCollection::Contains(const MgClassDefinition* value) const
{
    return m_dCollection->Contains(value);
}

///<summary>Returns the index of the specified item in the collection or -1 if the item does not exist.</summary>
/// <param name="value">Input value</param>
/// <returns>Returns the index of the specified item in the collection or -1 if the item does not exist.</returns>
/// <exception>
/// MgInvalidArgumentException</exception>
INT32 MgClassDefinitionCollection::IndexOf(const MgClassDefinition* value) const
{
    return m_dCollection->IndexOf(value);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serializes data to a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgClassDefinitionCollection::Serialize(MgStream* stream)
{
    INT32 count = this->GetCount();
    stream->WriteInt32(count);
    for (INT32 i = 0; i < count; i++)
        stream->WriteObject(Ptr<MgClassDefinition>(this->GetItem(i)));
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserializes data from a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgClassDefinitionCollection::Deserialize(MgStream* stream)
{
    INT32 count = 0;
    stream->GetInt32(count);
    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgClassDefinition> prop;
        prop = (MgClassDefinition*)stream->GetObject();
        this->Add(prop);
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgClassDefinitionCollection::GetClassId()
{
    return m_cls_id;
}

void MgClassDefinitionCollection::Dispose()
{
    delete this;
}

MgByteReader* MgClassDefinitionCollection::ToXml()
{
    throw new MgNotImplementedException(L"ClassDefinitionCollection.ToXml", __LINE__, __WFILE__, NULL, L"", NULL);
}
