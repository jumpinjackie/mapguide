//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

// Constructs an empty MgMapCollection object.
//
MgMapCollection::MgMapCollection()
{
    m_maps = new MgNamedCollection();
}

// Returns the number of items in the collection
//
INT32 MgMapCollection::GetCount()
{
    return m_maps->GetCount();
}

// Returns the item at the specified index
// Throws an out of range exception if the index is invalid
//
MgMapBase* MgMapCollection::GetItem(INT32 index)
{
    //returned value is addref'd by m_maps base class
    return (MgMapBase*)m_maps->GetItem(index);
}

// Returns the item with the specified name
// Throws an illegal argument exception if the name does not exist
//
MgMapBase* MgMapCollection::GetItem(CREFSTRING name)
{
    //returned value is addref'd by m_maps base class
    return (MgMapBase*)m_maps->GetItem(name);
}

// Sets the item in the collection at the specified index to the specified value
// Throws an out of range exception if the index is out of range.
//
void MgMapCollection::SetItem(INT32 index, MgMapBase* value)
{
    //value is addref'd by m_maps base class
    m_maps->SetItem(index, value);
}

// Adds the specified item to the end of the collection.
//
void MgMapCollection::Add(MgMapBase* value)
{
    //value is addref'd by m_maps base class
    m_maps->Add(value);
}

// Inserts the specified item at the specified index within the collection.
// Items following the insertion point are moved down to accommodate the new item.
// Throws an out of range argument exception if the specified index is out of range
//
void MgMapCollection::Insert(INT32 index, MgMapBase* value)
{
    //value is addref'd by m_maps base class
    m_maps->Insert(index, value);
}

/// <summary>
/// Removes all items from the collection
/// </summary>
void MgMapCollection::Clear()
{
    m_maps->Clear();
}

// Removes an item from the collection
// Throws an invalid argument exception if the item does not exist within the collection.
//
bool MgMapCollection::Remove(MgMapBase* value)
{
    bool removed = true;
    try
    {
        //value is released by m_maps base class
       removed = m_maps->Remove(value);
    }
    catch (MgException* e)
    {
        e->Release();
        removed = false;
    }
    return removed;
}

// Removes an item from the collection at the specified index
// Throws an out of range exception if the item does not exist within the collection.
//
void MgMapCollection::RemoveAt(INT32 index)
{
    //value at index is released by m_maps base class
    m_maps->RemoveAt(index);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgMapCollection::Contains(CREFSTRING name)
{
    return m_maps->Contains(name);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgMapCollection::Contains(MgMapBase* value)
{
    return m_maps->Contains(value);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgMapCollection::IndexOf(CREFSTRING name)
{
    return m_maps->IndexOf(name);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgMapCollection::IndexOf(MgMapBase* value)
{
    return m_maps->IndexOf(value);
}

// Destruct a MgMapCollection object
//
MgMapCollection::~MgMapCollection()
{
    SAFE_RELEASE(m_maps);
}
