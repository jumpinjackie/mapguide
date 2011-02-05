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
// Construct a MgGeometryCollection objet
//
MgGeometryCollection::MgGeometryCollection()
{
    m_collection = new MgDisposableCollection();
}

///////////////////////////////////////////////////////////////////////////
// MgGeometryCollection dtor
//
MgGeometryCollection::~MgGeometryCollection()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of points in the collection.
//
INT32 MgGeometryCollection::GetCount() const
{
    return m_collection->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometry in the collection at the specified index. Throws
// an invalid argument exception if the index is out of range.
//
MgGeometry* MgGeometryCollection::GetItem(INT32 index) const
{
    return (MgGeometry*)m_collection->GetItem(index);
}


///////////////////////////////////////////////////////////////////////////
// Sets the geometry in the collection at the specified index to the
// specified value. Throws an invalid argument exception if the index is
// out of range.
//
void MgGeometryCollection::SetItem(INT32 index, MgGeometry* value)
{
    m_collection->SetItem(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified geometry to the end of the collection.
//
void MgGeometryCollection::Add(MgGeometry* value)
{
    m_collection->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified geometry at the specified index within the
// collection. Items following the insertion point are moved down to
// accommodate the new item. Throws an invalid argument exception if the
// specified index is out of range.
//
void MgGeometryCollection::Insert(INT32 index, MgGeometry* value)
{
    m_collection->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all geometries from the collection.
//
void MgGeometryCollection::Clear()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes the specified geometry from the collection.
//
bool MgGeometryCollection::Remove(const MgGeometry* value)
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
// Removes the geometry at the specified index from the collection.
// Throws an invalid argument exception if the index does not exist within
// the collection.
//
void MgGeometryCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified geometry, false
// otherwise.
//
bool MgGeometryCollection::Contains(const MgGeometry* value) const
{
    return m_collection->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified geometry in the collection or -1
// if the point does not exist.
//
INT32 MgGeometryCollection::IndexOf(const MgGeometry* value) const
{
    return m_collection->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this collection
//
MgGeometryCollection* MgGeometryCollection::Copy()
{
    MgGeometryCollection* coll = new MgGeometryCollection();
    for(INT32 i = 0; i < GetCount(); i++)
    {
        Ptr<MgGeometry> segment = GetItem(i);
        coll->Add(segment);
    }
    return coll;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgGeometryCollection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgGeometryCollection::Dispose()
{
    delete this;
}

void MgGeometryCollection::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    INT32 cnt = GetCount();
    for(INT32 i = 0; i < cnt; i++)
    {
        if (i > 0)
        {
            awktStr += L", ";
        }

        Ptr<MgGeometry> geom = GetItem(i);
        awktStr += geom->ToAwkt(is2dOnly);
    }
}
