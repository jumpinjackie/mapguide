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
/// Construct a MgPolygonCollection objet
///</summary>
MgPolygonCollection::MgPolygonCollection()
{
    m_collection = new MgDisposableCollection();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// MgPolygonCollection dtor
///</summary>
MgPolygonCollection::~MgPolygonCollection()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of polygons in the collection.
//
INT32 MgPolygonCollection::GetCount() const
{
    return m_collection->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the polygon in the collection at the specified index. Throws
// an invalid argument exception if the index is out of range.
//
MgPolygon* MgPolygonCollection::GetItem(INT32 index) const
{
    return (MgPolygon*)m_collection->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Sets the polygon in the collection at the specified index to the
// specified value. Throws an invalid argument exception if the index is
// out of range.
//
void MgPolygonCollection::SetItem(INT32 index, MgPolygon* value)
{
    m_collection->SetItem(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified polygon to the end of the collection. Returns the
// index of the newly added polygon.
//
void MgPolygonCollection::Add(MgPolygon* value)
{
    m_collection->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified polygon at the specified index within the
// collection. Items following the insertion point are moved down to
// accommodate the new item. Throws an invalid argument exception if the
// specified index is out of range.
//
void MgPolygonCollection::Insert(INT32 index, MgPolygon* value)
{
    m_collection->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all polygons from the collection.
//
void MgPolygonCollection::Clear()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes the specified polygon from the collection. Throws an invalid
// argument exception if the item does not exist within the collection.
//
bool MgPolygonCollection::Remove(const MgPolygon* value)
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
// Removes the polygon at the specified index from the collection.
// Throws an invalid argument exception if the index does not exist within
// the collection.
//
void MgPolygonCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified polygon, false
// otherwise.
//
bool MgPolygonCollection::Contains(const MgPolygon* value) const
{
    return m_collection->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified polygon in the collection or -1
// if the polygon does not exist.
//
INT32 MgPolygonCollection::IndexOf(const MgPolygon* value) const
{
    return m_collection->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this collection
//
MgPolygonCollection* MgPolygonCollection::Copy()
{
    MgPolygonCollection* coll = new MgPolygonCollection();
    for(INT32 i = 0; i < GetCount(); i++)
    {
        Ptr<MgPolygon> segment = GetItem(i);
        coll->Add(segment);
    }
    return coll;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgPolygonCollection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgPolygonCollection::Dispose()
{
    delete this;
}

void MgPolygonCollection::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    INT32 cnt = GetCount();
    for(INT32 i = 0; i < cnt; i++)
    {
        if (i > 0)
        {
            awktStr += L", ";
        }

        Ptr<MgPolygon> geom = GetItem(i);
        geom->ToAwkt(awktStr, coordDim, is2dOnly);
    }
}
