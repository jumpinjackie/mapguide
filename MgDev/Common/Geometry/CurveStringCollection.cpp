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
/// Construct a MgCurveStringCollection objet
///</summary>
MgCurveStringCollection::MgCurveStringCollection()
{
    m_collection = new MgDisposableCollection();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// MgCurveStringCollection dtor
///</summary>
MgCurveStringCollection::~MgCurveStringCollection()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of curve strings in the collection.
//
INT32 MgCurveStringCollection::GetCount() const
{
    return m_collection->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the curve string in the collection at the specified index. Throws
// an invalid argument exception if the index is out of range.
//
MgCurveString* MgCurveStringCollection::GetItem(INT32 index) const
{
    return (MgCurveString*)m_collection->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Sets the curve string in the collection at the specified index to the
// specified value. Throws an invalid argument exception if the index is
// out of range.
//
void MgCurveStringCollection::SetItem(INT32 index, MgCurveString* value)
{
    m_collection->SetItem(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified coodinate to the end of the collection. Returns the
// index of the newly added curve string.
//
void MgCurveStringCollection::Add(MgCurveString* value)
{
    m_collection->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified curve string at the specified index within the
// collection. Items following the insertion point are moved down to
// accommodate the new item. Throws an invalid argument exception if the
// specified index is out of range.
//
void MgCurveStringCollection::Insert(INT32 index, MgCurveString* value)
{
    m_collection->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all curve strings from the collection.
//
void MgCurveStringCollection::Clear()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes the specified curve string from the collection. Throws an invalid
// argument exception if the item does not exist within the collection.
//
bool MgCurveStringCollection::Remove(const MgCurveString* value)
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
// Removes the curve string at the specified index from the collection.
// Throws an invalid argument exception if the index does not exist within
// the collection.
//
void MgCurveStringCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified curve string, false
// otherwise.
//
bool MgCurveStringCollection::Contains(const MgCurveString* value) const
{
    return m_collection->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified curve string in the collection or -1
// if the curve string does not exist.
//
INT32 MgCurveStringCollection::IndexOf(const MgCurveString* value) const
{
    return m_collection->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this collection
//
MgCurveStringCollection* MgCurveStringCollection::Copy()
{
    MgCurveStringCollection* coll = new MgCurveStringCollection();
    for(INT32 i = 0; i < GetCount(); i++)
    {
        Ptr<MgCurveString> segment = GetItem(i);
        coll->Add(segment);
    }
    return coll;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgCurveStringCollection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgCurveStringCollection::Dispose()
{
    delete this;
}

void MgCurveStringCollection::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    INT32 cnt = GetCount();
    for(INT32 i = 0; i < cnt; i++)
    {
        if (i > 0)
        {
            awktStr += L", ";
        }

        Ptr<MgCurveString> segment = GetItem(i);
        segment->ToAwkt(awktStr, coordDim, is2dOnly);
    }
}
