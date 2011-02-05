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
/// Construct a MgCurveRingCollection objet
///</summary>
MgCurveRingCollection::MgCurveRingCollection()
{
    m_collection = new MgDisposableCollection();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// MgCurveRingCollection dtor
///</summary>
MgCurveRingCollection::~MgCurveRingCollection()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of curve rings in the collection.
//
INT32 MgCurveRingCollection::GetCount() const
{
    return m_collection->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the curve ring in the collection at the specified index. Throws
// an invalid argument exception if the index is out of range.
//
MgCurveRing* MgCurveRingCollection::GetItem(INT32 index) const
{
    return (MgCurveRing*)m_collection->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Sets the curve ring in the collection at the specified index to the
// specified value. Throws an invalid argument exception if the index is
// out of range.
//
void MgCurveRingCollection::SetItem(INT32 index, MgCurveRing* value)
{
    m_collection->SetItem(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified coodinate to the end of the collection.
//
void MgCurveRingCollection::Add(MgCurveRing* value)
{
    m_collection->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified curve ring at the specified index within the
// collection. Items following the insertion point are moved down to
// accommodate the new item. Throws an invalid argument exception if the
// specified index is out of range.
//
void MgCurveRingCollection::Insert(INT32 index, MgCurveRing* value)
{
    m_collection->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all curve rings from the collection.
//
void MgCurveRingCollection::Clear()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes the specified curve ring from the collection.
//
bool MgCurveRingCollection::Remove(const MgCurveRing* value)
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
// Removes the curve ring at the specified index from the collection.
// Throws an invalid argument exception if the index does not exist within
// the collection.
//
void MgCurveRingCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified curve ring, false
// otherwise.
//
bool MgCurveRingCollection::Contains(const MgCurveRing* value) const
{
    return m_collection->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified curve ring in the collection or -1
// if the curve ring does not exist.
//
INT32 MgCurveRingCollection::IndexOf(const MgCurveRing* value) const
{
    return m_collection->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this collection
//
MgCurveRingCollection* MgCurveRingCollection::Copy()
{
    MgCurveRingCollection* coll = new MgCurveRingCollection();
    for(INT32 i = 0; i < GetCount(); i++)
    {
        Ptr<MgCurveRing> segment = GetItem(i);
        coll->Add(segment);
    }
    return coll;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgCurveRingCollection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgCurveRingCollection::Dispose()
{
    delete this;
}

void MgCurveRingCollection::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    INT32 cnt = GetCount();
    for(INT32 i = 0; i < cnt; i++)
    {
        if (i > 0)
        {
            awktStr += L", ";
        }

        Ptr<MgCurveRing> segment = GetItem(i);
        segment->ToAwkt(awktStr, coordDim, is2dOnly);
    }
}
