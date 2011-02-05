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
/// Construct a MgCurveSegmentCollection objet
///</summary>
MgCurveSegmentCollection::MgCurveSegmentCollection()
{
    m_collection = new MgDisposableCollection();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// MgCurveSegmentCollection dtor
///</summary>
MgCurveSegmentCollection::~MgCurveSegmentCollection()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of curve segments in the collection.
//
INT32 MgCurveSegmentCollection::GetCount() const
{
    return m_collection->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the curve segment in the collection at the specified index. Throws
// an invalid argument exception if the index is out of range.
//
MgCurveSegment* MgCurveSegmentCollection::GetItem(INT32 index) const
{
    return (MgCurveSegment*)m_collection->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Sets the curve segment in the collection at the specified index to the
// specified value. Throws an invalid argument exception if the index is
// out of range.
//
void MgCurveSegmentCollection::SetItem(INT32 index, MgCurveSegment* value)
{
    m_collection->SetItem(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified coodinate to the end of the collection.
//
void MgCurveSegmentCollection::Add(MgCurveSegment* value)
{
    m_collection->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified curve segment at the specified index within the
// collection. Items following the insertion point are moved down to
// accommodate the new item. Throws an invalid argument exception if the
// specified index is out of range.
//
void MgCurveSegmentCollection::Insert(INT32 index, MgCurveSegment* value)
{
    m_collection->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all curve segments from the collection.
//
void MgCurveSegmentCollection::Clear()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes the specified curve segment from the collection.
//
bool MgCurveSegmentCollection::Remove(const MgCurveSegment* value)
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
// Removes the curve segment at the specified index from the collection.
// Throws an invalid argument exception if the index does not exist within
// the collection.
//
void MgCurveSegmentCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified curve segment, false
// otherwise.
//
bool MgCurveSegmentCollection::Contains(const MgCurveSegment* value) const
{
    return m_collection->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified curve segment in the collection or -1
// if the curve segment does not exist.
//
INT32 MgCurveSegmentCollection::IndexOf(const MgCurveSegment* value) const
{
    return m_collection->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this collection
//
MgCurveSegmentCollection* MgCurveSegmentCollection::Copy()
{
    MgCurveSegmentCollection* coll = new MgCurveSegmentCollection();
    for(INT32 i = 0; i < GetCount(); i++)
    {
        Ptr<MgCurveSegment> segment = (MgCurveSegment*)(Ptr<MgCurveSegment>(GetItem(i)))->Copy();
        coll->Add(segment);
    }
    return coll;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgCurveSegmentCollection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgCurveSegmentCollection::Dispose()
{
    delete this;
}

void MgCurveSegmentCollection::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    INT32 cnt = this->GetCount();

    for(INT32 i = 0; i < cnt; i++)
    {
        if (i > 0)
        {
            awktStr += L", ";
        }

        Ptr<MgCurveSegment> segment = (MgCurveSegment*)GetItem(i);
        if (i == 0) // first segement, add start position
        {
            Ptr<MgCoordinate> startcoord = segment->GetStartCoordinate();
            startcoord->ToAwkt(awktStr, coordDim, is2dOnly);
            awktStr += L" (";
        }
        segment->ToAwkt(awktStr, coordDim, is2dOnly);

        if (i == (cnt-1)) // Last segment, add end brace
        {
            awktStr += L")";
        }
    }
}
