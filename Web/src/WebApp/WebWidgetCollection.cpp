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

#include "WebApp.h"

///////////////////////////////////////////////////////////////////////////
// Constructs an empty MgWebWidgetCollection object.
//
MgWebWidgetCollection::MgWebWidgetCollection()
{
    m_widgets = new MgDisposableCollection();
}

///////////////////////////////////////////////////////////////////////////
// Returns the number of items in the collection
//
INT32 MgWebWidgetCollection::GetCount()
{
    return m_widgets->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Returns the item at the specified index
// Throws an out of range exception if the index is invalid
//
MgWebWidget* MgWebWidgetCollection::GetWidget(INT32 index)
{
    //returned value is addref'd by m_widgets base class
    return (MgWebWidget*)m_widgets->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified item, false otherwise
//
bool MgWebWidgetCollection::Contains(MgWebWidget* value)
{
    return m_widgets->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified item in the collection or -1 if the item does not exist.
//
INT32 MgWebWidgetCollection::IndexOf(MgWebWidget* value)
{
    return m_widgets->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified item to the end of the collection.
//
void MgWebWidgetCollection::Add(MgWebWidget* value)
{
    //value is addref'd by m_widgets base class
    m_widgets->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified item at the specified index within the collection.
// Items following the insertion point are moved down to accommodate the new item.
// Throws an out of range argument exception if the specified index is out of range
//
void MgWebWidgetCollection::Insert(INT32 index, MgWebWidget* value)
{
    //value is addref'd by m_widgets base class
    m_widgets->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all items from the collection
//
void MgWebWidgetCollection::Clear()
{
    m_widgets->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes an item from the collection
//
bool MgWebWidgetCollection::Remove(MgWebWidget* value)
{
    bool removed = true;
    try
    {
        //value is released by m_commands base class
        m_widgets->Remove(value);
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
void MgWebWidgetCollection::RemoveAt(INT32 index)
{
    //value at index is released by m_widgets base class
    m_widgets->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebWidgetCollection::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebWidgetCollection::Dispose()
{
    delete this;
}
