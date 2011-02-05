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

#include "GeometryCommon.h"

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgLineStringCollection objet
///</summary>
MgLineStringCollection::MgLineStringCollection()
{
    m_collection = new MgDisposableCollection();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// MgLineStringCollection dtor
///</summary>
MgLineStringCollection::~MgLineStringCollection()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of line strings in the collection.
//
INT32 MgLineStringCollection::GetCount() const
{
    return m_collection->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the line string in the collection at the specified index. Throws
// an invalid argument exception if the index is out of range.
//
MgLineString* MgLineStringCollection::GetItem(INT32 index) const
{
    return (MgLineString*)m_collection->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Sets the line string in the collection at the specified index to the
// specified value. Throws an invalid argument exception if the index is
// out of range.
//
void MgLineStringCollection::SetItem(INT32 index, MgLineString* value)
{
    m_collection->SetItem(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified line string to the end of the collection. Returns the
// index of the newly added line string.
//
void MgLineStringCollection::Add(MgLineString* value)
{
    m_collection->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified line string at the specified index within the
// collection. Items following the insertion point are moved down to
// accommodate the new item. Throws an invalid argument exception if the
// specified index is out of range.
//
void MgLineStringCollection::Insert(INT32 index, MgLineString* value)
{
    m_collection->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all line strings from the collection.
//
void MgLineStringCollection::Clear()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes the specified line string from the collection. Throws an invalid
// argument exception if the item does not exist within the collection.
//
bool MgLineStringCollection::Remove(const MgLineString* value)
{
    bool removed = true;
    try
    {
        m_collection->Remove(value);
    }
    catch (MgException* e)
    {
        e->Release();
        removed = false;
    }
    return removed;
}

///////////////////////////////////////////////////////////////////////////
// Removes the line string at the specified index from the collection.
// Throws an invalid argument exception if the index does not exist within
// the collection.
//
void MgLineStringCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified line string, false
// otherwise.
//
bool MgLineStringCollection::Contains(const MgLineString* value) const
{
    return m_collection->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified line string in the collection or -1
// if the line string does not exist.
//
INT32 MgLineStringCollection::IndexOf(const MgLineString* value) const
{
    return m_collection->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this collection
//
MgLineStringCollection* MgLineStringCollection::Copy()
{
    MgLineStringCollection* coll = new MgLineStringCollection();
    for(INT32 i = 0; i < GetCount(); i++)
    {
        Ptr<MgLineString> segment = GetItem(i);
        coll->Add(segment);
    }
    return coll;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgLineStringCollection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgLineStringCollection::Dispose()
{
    delete this;
}

void MgLineStringCollection::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    for(INT32 i = 0; i < GetCount(); i++)
    {
        if (i > 0)
        {
            awktStr += L", ";
        }
        Ptr<MgLineString> segment = GetItem(i);
        segment->ToAwkt(awktStr, coordDim, is2dOnly);
    }
}
