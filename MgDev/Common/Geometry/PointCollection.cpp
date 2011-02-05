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
/// Construct a MgPointCollection objet
///</summary>
MgPointCollection::MgPointCollection()
{
    m_collection = new MgDisposableCollection();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// MgPointCollection dtor
///</summary>
MgPointCollection::~MgPointCollection()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of points in the collection.
//
INT32 MgPointCollection::GetCount() const
{
    return m_collection->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the point in the collection at the specified index. Throws
// an invalid argument exception if the index is out of range.
//
MgPoint* MgPointCollection::GetItem(INT32 index) const
{
    return (MgPoint*)m_collection->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Sets the point in the collection at the specified index to the
// specified value. Throws an invalid argument exception if the index is
// out of range.
//
void MgPointCollection::SetItem(INT32 index, MgPoint* value)
{
    m_collection->SetItem(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified point to the end of the collection. Returns the
// index of the newly added point.
//
void MgPointCollection::Add(MgPoint* value)
{
    m_collection->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified point at the specified index within the
// collection. Items following the insertion point are moved down to
// accommodate the new item. Throws an invalid argument exception if the
// specified index is out of range.
//
void MgPointCollection::Insert(INT32 index, MgPoint* value)
{
    m_collection->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all points from the collection.
//
void MgPointCollection::Clear()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes the specified point from the collection. Throws an invalid
// argument exception if the item does not exist within the collection.
//
bool MgPointCollection::Remove(const MgPoint* value)
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
// Removes the point at the specified index from the collection.
// Throws an invalid argument exception if the index does not exist within
// the collection.
//
void MgPointCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified point, false
// otherwise.
//
bool MgPointCollection::Contains(const MgPoint* value) const
{
    return m_collection->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified point in the collection or -1
// if the point does not exist.
//
INT32 MgPointCollection::IndexOf(const MgPoint* value) const
{
    return m_collection->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this collection
//
MgPointCollection* MgPointCollection::Copy()
{
    MgPointCollection* coll = new MgPointCollection();
    for(INT32 i = 0; i < GetCount(); i++)
    {
        Ptr<MgPoint> segment = GetItem(i);
        coll->Add(segment);
    }
    return coll;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgPointCollection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgPointCollection::Dispose()
{
    delete this;
}

void MgPointCollection::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    INT32 cnt = GetCount();
    for(INT32 i = 0; i < cnt; i++)
    {
        if (i > 0)
        {
            awktStr += L", ";
        }

        Ptr<MgPoint> geom = GetItem(i);
        geom->ToAwkt(awktStr, coordDim, is2dOnly);
    }
}
