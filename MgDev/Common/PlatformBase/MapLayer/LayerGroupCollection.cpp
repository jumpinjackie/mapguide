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
#include <assert.h>

// Constructs an empty MgLayerGroupCollection object.
//
MgLayerGroupCollection::MgLayerGroupCollection(MgMapBase* owner)
{
    m_groups = new MgNamedCollection();
    m_owner = owner;        //no need for reference counting. MgMap wil always outlive this collection
}

MgLayerGroupCollection::MgLayerGroupCollection()
{
    m_groups = new MgNamedCollection();
    m_owner = NULL;
}

// Returns the number of items in the collection
//
INT32 MgLayerGroupCollection::GetCount()
{
    return m_groups->GetCount();
}

// Returns the item at the specified index
// Throws an out of range exception if the index is invalid
//
MgLayerGroup* MgLayerGroupCollection::GetItem(INT32 index)
{
    //returned value is addref'd by m_groups base class
    return (MgLayerGroup*)m_groups->GetItem(index);
}

// Returns the item with the specified name
// Throws an illegal argument exception if the name does not exist
//
MgLayerGroup* MgLayerGroupCollection::GetItem(CREFSTRING name)
{
    //returned value is addref'd by m_groups base class
    return (MgLayerGroup*)m_groups->GetItem(name);
}

// Sets the item in the collection at the specified index to the specified value
// Throws an out of range exception if the index is out of range.
//
void MgLayerGroupCollection::SetItem(INT32 index, MgLayerGroup* value)
{
    //notify the removal of the current layer group at this position
    Ptr<MgLayerGroup> prevGroup = (MgLayerGroup*)m_groups->GetItem(index);
    if (m_owner)
        m_owner->OnGroupRemoved(prevGroup);

    //value is addref'd by m_groups base class
    m_groups->SetItem(index, value);

    if (m_owner)
        m_owner->OnGroupAdded(value);
}

// Adds the specified item to the end of the collection.
//
void MgLayerGroupCollection::Add(MgLayerGroup* value)
{
    //value is addref'd by m_groups base class
    m_groups->Add(value);

    if (m_owner)
        m_owner->OnGroupAdded(value);
}

// Inserts the specified item at the specified index within the collection.
// Items following the insertion point are moved down to accommodate the new item.
// Throws an out of range argument exception if the specified index is out of range
//
void MgLayerGroupCollection::Insert(INT32 index, MgLayerGroup* value)
{
    //value is addref'd by m_groups base class
    m_groups->Insert(index, value);

    if (m_owner)
        m_owner->OnGroupAdded(value);
}

// Removes all items from the collection
//
void MgLayerGroupCollection::Clear()
{
    //remove top level groups. children group will be automatically removed
    for(INT32 i = 0; i < m_groups->GetCount(); i++)
    {
        Ptr<MgLayerGroup> group = (MgLayerGroup*)m_groups->GetItem(i);
        Ptr<MgLayerGroup> groupParent = group->GetGroup();
        if(groupParent == NULL)
        {
            m_groups->RemoveAt(i);
            if (m_owner)
                m_owner->OnGroupRemoved(group);
            i = -1; //needs to start all over as groups might have been removed from anywhere in the collection
        }
    }

    assert(m_groups->GetCount() == 0);  //all top + children groups must have been removed
}

// Removes an item from the collection
// Returns true if successful.
//
bool MgLayerGroupCollection::Remove(MgLayerGroup* value)
{
    bool removed = true;
    try
    {
        Ptr<MgLayerGroup> group = SAFE_ADDREF(value);

        //value is released by m_groups base class
        m_groups->Remove(value);

        if (m_owner)
            m_owner->OnGroupRemoved(group);
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
void MgLayerGroupCollection::RemoveAt(INT32 index)
{
    Ptr<MgLayerGroup> group = (MgLayerGroup*)m_groups->GetItem(index);

    //value at index is released by m_groups base class
    m_groups->RemoveAt(index);

    if (m_owner)
        m_owner->OnGroupRemoved(group);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgLayerGroupCollection::Contains(CREFSTRING name)
{
    return m_groups->Contains(name);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgLayerGroupCollection::Contains(MgLayerGroup* value)
{
    return m_groups->Contains(value);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgLayerGroupCollection::IndexOf(CREFSTRING name)
{
    return m_groups->IndexOf(name);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgLayerGroupCollection::IndexOf(MgLayerGroup* value)
{
    return m_groups->IndexOf(value);
}

// Destruct a MgLayerGroupCollection object
//
MgLayerGroupCollection::~MgLayerGroupCollection()
{
    SAFE_RELEASE(m_groups);
}

// Gets the map owning this collection
//
MgMapBase* MgLayerGroupCollection::GetMap()
{
    return m_owner;
}
