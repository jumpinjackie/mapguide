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
#include "PrintLayoutElementCollection.h"

// Constructs an empty MgPrintLayoutElementCollection object.
//
MgPrintLayoutElementCollection::MgPrintLayoutElementCollection(MgPrintLayoutBase* owner)
{
    m_elements = new MgNamedCollection();
    m_owner = SAFE_ADDREF(owner);
}

MgPrintLayoutElementCollection::MgPrintLayoutElementCollection()
{
    m_elements = new MgNamedCollection();
    m_owner = NULL;
}

// Returns the number of items in the collection
//
INT32 MgPrintLayoutElementCollection::GetCount()
{
    return m_elements->GetCount();
}

// Returns the item at the specified index
// Throws an out of range exception if the index is invalid
//
MgPrintLayoutElementBase* MgPrintLayoutElementCollection::GetItem(INT32 index)
{
    //returned value is addref'd by m_elements base class
    return (MgPrintLayoutElementBase*)m_elements->GetItem(index);
}

// Returns the item with the specified name
// Throws an illegal argument exception if the name does not exist
//
MgPrintLayoutElementBase* MgPrintLayoutElementCollection::GetItem(CREFSTRING name)
{
    //returned value is addref'd by m_groups base class
    return (MgPrintLayoutElementBase*)m_elements->GetItem(name);
}

// Sets the item in the collection at the specified index to the specified value
// Throws an out of range exception if the index is out of range.
//
void MgPrintLayoutElementCollection::SetItem(INT32 index, MgPrintLayoutElementBase* value)
{
    //notify the removal of the current print layout element at this position
    Ptr<MgPrintLayoutElementBase> prevElement = (MgPrintLayoutElementBase*)m_elements->GetItem(index);
    if (m_owner)
        m_owner->OnPrintLayoutElementRemoved(prevElement);

    //value is addref'd by m_elements base class
    m_elements->SetItem(index, value);

    if (m_owner)
        m_owner->OnPrintLayoutElementAdded(value);
}

// Adds the specified item to the end of the collection.
//
void MgPrintLayoutElementCollection::Add(MgPrintLayoutElementBase* value)
{
    //value is addref'd by m_elements base class
    m_elements->Add(value);

    if (m_owner)
        m_owner->OnPrintLayoutElementAdded(value);
}

// Inserts the specified item at the specified index within the collection.
// Items following the insertion point are moved down to accommodate the new item.
// Throws an out of range argument exception if the specified index is out of range
//
void MgPrintLayoutElementCollection::Insert(INT32 index, MgPrintLayoutElementBase* value)
{
    //value is addref'd by m_elements base class
    m_elements->Insert(index, value);

    if (m_owner)
        m_owner->OnPrintLayoutElementAdded(value);
}

// Removes all items from the collection
//
void MgPrintLayoutElementCollection::Clear()
{
    if (m_owner)
        m_owner->OnPrintLayoutElementRemoved(NULL);

    m_elements->Clear();
}

// Removes an item from the collection
// Returns true if successful.
//
bool MgPrintLayoutElementCollection::Remove(MgPrintLayoutElementBase* value)
{
    bool removed = true;
    try
    {
        Ptr<MgPrintLayoutElementBase> element = SAFE_ADDREF(value);

        //value is released by m_elements base class
        m_elements->Remove(value);

        if (m_owner)
            m_owner->OnPrintLayoutElementRemoved(element);
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
void MgPrintLayoutElementCollection::RemoveAt(INT32 index)
{
    Ptr<MgPrintLayoutElementBase> element = (MgPrintLayoutElementBase*)m_elements->GetItem(index);

    //value at index is released by m_elements base class
    m_elements->RemoveAt(index);

    if (m_owner)
        m_owner->OnPrintLayoutElementRemoved(element);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgPrintLayoutElementCollection::Contains(CREFSTRING name)
{
    return m_elements->Contains(name);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgPrintLayoutElementCollection::Contains(MgPrintLayoutElementBase* value)
{
    return m_elements->Contains(value);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgPrintLayoutElementCollection::IndexOf(CREFSTRING name)
{
    return m_elements->IndexOf(name);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgPrintLayoutElementCollection::IndexOf(MgPrintLayoutElementBase* value)
{
    return m_elements->IndexOf(value);
}

// Destruct a MgPrintLayoutElementCollection object
//
MgPrintLayoutElementCollection::~MgPrintLayoutElementCollection()
{
}

// Gets the print layout owning this collection
//
MgPrintLayoutBase* MgPrintLayoutElementCollection::GetPrintLayout()
{
    return SAFE_ADDREF(m_owner.p);
}
