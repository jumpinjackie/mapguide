//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

// Constructs an empty MgLayerCollection object.
//
MgLayerCollection::MgLayerCollection(MgMapBase* owner)
{
    m_layers = new MgNamedCollection();
    m_owner = owner;        //no need for reference counting. MgMap wil always outlive this collection
}

MgLayerCollection::MgLayerCollection()
{
    m_layers = new MgNamedCollection();
    m_owner = NULL;
}

// Returns the number of items in the collection
//
INT32 MgLayerCollection::GetCount()
{
    return m_layers->GetCount();
}

// Returns the item at the specified index
// Throws an out of range exception if the index is invalid
//
MgLayerBase* MgLayerCollection::GetItem(INT32 index)
{
    //returned value is addref'd by m_layers base class
    return (MgLayerBase*)m_layers->GetItem(index);
}

// Returns the item with the specified name
// Throws an illegal argument exception if the name does not exist
//
MgLayerBase* MgLayerCollection::GetItem(CREFSTRING name)
{
    //returned value is addref'd by m_groups base class
    return (MgLayerBase*)m_layers->GetItem(name);
}

// Sets the item in the collection at the specified index to the specified value
// Throws an out of range exception if the index is out of range.
//
void MgLayerCollection::SetItem(INT32 index, MgLayerBase* value)
{
    //notify the removal of the current layer at this position
    Ptr<MgLayerBase> prevLayer = (MgLayerBase*)m_layers->GetItem(index);
    if (m_owner)
        m_owner->OnLayerRemoved(prevLayer);

    //new layer inherit zorder of the layer it replaces
    value->SetDisplayOrder(prevLayer->GetDisplayOrder());

    //value is addref'd by m_layers base class
    m_layers->SetItem(index, value);

    if (m_owner)
        m_owner->OnLayerAdded(value);
}

// Adds the specified item to the end of the collection.
//
void MgLayerCollection::Add(MgLayerBase* value)
{
    //calculate and set the zorder for the new layer
    Ptr<MgLayerBase> prevLayer = (m_layers->GetCount() == 0? NULL: (MgLayerBase*)m_layers->GetItem(m_layers->GetCount() - 1));
    double zOrder = prevLayer == NULL? LAYER_ZORDER_TOP: prevLayer->GetDisplayOrder() + LAYER_ZORDER_INCR;
    value->SetDisplayOrder(zOrder);

    //value is addref'd by m_layers base class
    m_layers->Add(value);

    if (m_owner)
        m_owner->OnLayerAdded(value);
}

// Inserts the specified item at the specified index within the collection.
// Items following the insertion point are moved down to accommodate the new item.
// Throws an out of range argument exception if the specified index is out of range
//
void MgLayerCollection::Insert(INT32 index, MgLayerBase* value)
{
    //calculate zorder for the new layer
    double zOrderLow, zOrderHigh;
    Ptr<MgLayerBase> layer;

    if(index == 0)
    {
        zOrderLow = 0;
        layer = m_layers->GetCount() > 0? (MgLayerBase*)m_layers->GetItem(index): NULL;
        if(layer != NULL)
            zOrderHigh = layer->GetDisplayOrder();
        else
            zOrderHigh = 2. * LAYER_ZORDER_INCR;
    }
    else
    {
        layer = (MgLayerBase*)m_layers->GetItem(index - 1);
        zOrderLow = layer->GetDisplayOrder();
        layer = m_layers->GetCount() > index? (MgLayerBase*)m_layers->GetItem(index): NULL;
        zOrderHigh = layer != NULL? layer->GetDisplayOrder() : zOrderLow + 2. * LAYER_ZORDER_INCR;
    }
    value->SetDisplayOrder(zOrderLow + (zOrderHigh - zOrderLow) / 2.);

    //value is addref'd by m_layers base class
    m_layers->Insert(index, value);

    if (m_owner)
        m_owner->OnLayerAdded(value);
}

// Removes all items from the collection
//
void MgLayerCollection::Clear()
{
    if (m_owner)
        m_owner->OnLayerRemoved(NULL);

    m_layers->Clear();
}

// Removes an item from the collection
// Returns true if successful.
//
bool MgLayerCollection::Remove(MgLayerBase* value)
{
    bool removed = true;
    try
    {
        Ptr<MgLayerBase> layer = SAFE_ADDREF(value);

        //value is released by m_layers base class
        m_layers->Remove(value);

        if (m_owner)
            m_owner->OnLayerRemoved(layer);
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
void MgLayerCollection::RemoveAt(INT32 index)
{
    Ptr<MgLayerBase> layer = (MgLayerBase*)m_layers->GetItem(index);

    //value at index is released by m_layers base class
    m_layers->RemoveAt(index);

    if (m_owner)
        m_owner->OnLayerRemoved(layer);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgLayerCollection::Contains(CREFSTRING name)
{
    return m_layers->Contains(name);
}

// Returns true if the collection contains the specified item, false otherwise
//
bool MgLayerCollection::Contains(MgLayerBase* value)
{
    return m_layers->Contains(value);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgLayerCollection::IndexOf(CREFSTRING name)
{
    return m_layers->IndexOf(name);
}

// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgLayerCollection::IndexOf(MgLayerBase* value)
{
    return m_layers->IndexOf(value);
}

// Destruct a MgLayerCollection object
//
MgLayerCollection::~MgLayerCollection()
{
    SAFE_RELEASE(m_layers);
}

// Gets the map owning this collection
//
MgMapBase* MgLayerCollection::GetMap()
{
    return m_owner;
}

void MgLayerCollection::SetCheckForDuplicates(bool bDup)
{
    m_layers->SetCheckForDuplicates(bDup);
}
