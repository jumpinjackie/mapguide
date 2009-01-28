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

#include "WebApp.h"

///////////////////////////////////////////////////////////////////////////
// Returns the number of items in the collection
//
INT32 MgWebCommandCollection::GetCount()
{
    return m_commands->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Returns the item at the specified index
// Throws an out of range exception if the index is invalid
//
MgWebCommand* MgWebCommandCollection::GetItem(INT32 index)
{
    //returned value is addref'd by m_commands base class
    return (MgWebCommand*)m_commands->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns the item with the specified name
// Throws an illegal argument exception if the name does not exist
//
MgWebCommand* MgWebCommandCollection::GetItem(CREFSTRING name)
{
    //returned value is addref'd by m_commands base class
    return (MgWebCommand*)m_commands->GetItem(name);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified item, false otherwise
//
bool MgWebCommandCollection::Contains(CREFSTRING name)
{
    return m_commands->Contains(name);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified item, false otherwise
//
bool MgWebCommandCollection::Contains(MgWebCommand* value)
{
    return m_commands->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgWebCommandCollection::IndexOf(CREFSTRING name)
{
    return m_commands->IndexOf(name);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgWebCommandCollection::IndexOf(MgWebCommand* value)
{
    return m_commands->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Constructs an empty MgWebCommandCollection object.
//
MgWebCommandCollection::MgWebCommandCollection()
{
    m_commands = new MgNamedCollection();
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified item to the end of the collection.
//
void MgWebCommandCollection::Add(MgWebCommand* value)
{
    //value is addref'd by m_commands base class
    m_commands->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified item at the specified index within the collection.
// Items following the insertion point are moved down to accommodate the new item.
// Throws an out of range argument exception if the specified index is out of range
//
void MgWebCommandCollection::Insert(INT32 index, MgWebCommand* value)
{
    //value is addref'd by m_commands base class
    m_commands->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all items from the collection
//
void MgWebCommandCollection::Clear()
{
    m_commands->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes an item from the collection
// Throws an invalid argument exception if the item does not exist within the collection.
//
bool MgWebCommandCollection::Remove(MgWebCommand* value)
{
    bool removed = true;
    try
    {
        //value is released by m_commands base class
        m_commands->Remove(value);
    }
    catch (MgException* e)
    {
        e->Release();
        removed = false;
    }
    return removed;
}

///////////////////////////////////////////////////////////////////////////
// Removes an item from the collection at the specified index
// Throws an out of range exception if the item does not exist within the collection.
//
void MgWebCommandCollection::RemoveAt(INT32 index)
{
    //value at index is released by m_commands base class
    m_commands->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebCommandCollection::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebCommandCollection::Dispose()
{
    delete this;
}
