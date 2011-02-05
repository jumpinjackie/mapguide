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
/// Construct a MgCurvePolygonCollection objet
///</summary>
MgCurvePolygonCollection::MgCurvePolygonCollection()
{
    m_collection = new MgDisposableCollection();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// MgCurvePolygonCollection dtor
///</summary>
MgCurvePolygonCollection::~MgCurvePolygonCollection()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of curve polygons in the collection.
//
INT32 MgCurvePolygonCollection::GetCount() const
{
    return m_collection->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the curve polygon in the collection at the specified index. Throws
// an invalid argument exception if the index is out of range.
//
MgCurvePolygon* MgCurvePolygonCollection::GetItem(INT32 index) const
{
    return (MgCurvePolygon*)m_collection->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Sets the curve polygon in the collection at the specified index to the
// specified value. Throws an invalid argument exception if the index is
// out of range.
//
void MgCurvePolygonCollection::SetItem(INT32 index, MgCurvePolygon* value)
{
    m_collection->SetItem(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified curve polygon to the end of the collection.
//
void MgCurvePolygonCollection::Add(MgCurvePolygon* value)
{
    m_collection->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified curve polygon at the specified index within the
// collection. Items following the insertion point are moved down to
// accommodate the new item. Throws an invalid argument exception if the
// specified index is out of range.
//
void MgCurvePolygonCollection::Insert(INT32 index, MgCurvePolygon* value)
{
    m_collection->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all curve polygons from the collection.
//
void MgCurvePolygonCollection::Clear()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes the specified curve polygon from the collection.
//
bool MgCurvePolygonCollection::Remove(const MgCurvePolygon* value)
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
// Removes the curve polygon at the specified index from the collection.
// Throws an invalid argument exception if the index does not exist within
// the collection.
//
void MgCurvePolygonCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified curve polygon, false
// otherwise.
//
bool MgCurvePolygonCollection::Contains(const MgCurvePolygon* value) const
{
    return m_collection->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified curve polygon in the collection or -1
// if the curve polygon does not exist.
//
INT32 MgCurvePolygonCollection::IndexOf(const MgCurvePolygon* value) const
{
    return m_collection->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this collection
//
MgCurvePolygonCollection* MgCurvePolygonCollection::Copy()
{
    MgCurvePolygonCollection* coll = new MgCurvePolygonCollection();
    for(INT32 i = 0; i < GetCount(); i++)
    {
        Ptr<MgCurvePolygon> segment = GetItem(i);
        coll->Add(segment);
    }
    return coll;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgCurvePolygonCollection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgCurvePolygonCollection::Dispose()
{
    delete this;
}

void MgCurvePolygonCollection::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    INT32 cnt = GetCount();
    for(INT32 i = 0; i < cnt; i++)
    {
        if (i > 0)
        {
            awktStr += L", ";
        }

        Ptr<MgCurvePolygon> segment = GetItem(i);
        segment->ToAwkt(awktStr, coordDim, is2dOnly);
    }
}
