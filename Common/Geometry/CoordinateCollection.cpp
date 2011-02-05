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
// Construct an MgCoordinateCollection object
//
MgCoordinateCollection::MgCoordinateCollection()
{
    m_collection = new MgDisposableCollection();
}

///////////////////////////////////////////////////////////////////////////
// MgCoordinateCollection destructor
//
MgCoordinateCollection::~MgCoordinateCollection()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of coordinates in the collection.
//
INT32 MgCoordinateCollection::GetCount() const
{
    return m_collection->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the coordinate in the collection at the specified index. Throws
// an invalid argument exception if the index is out of range.
//
MgCoordinate* MgCoordinateCollection::GetItem(INT32 index) const
{
    return (MgCoordinate*)m_collection->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Sets the coordinate in the collection at the specified index to the
// specified value. Throws an invalid argument exception if the index is
// out of range.
//
void MgCoordinateCollection::SetItem(INT32 index, MgCoordinate* value)
{
    m_collection->SetItem(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Adds the specified coodinate to the end of the collection.
//
void MgCoordinateCollection::Add(MgCoordinate* value)
{
    m_collection->Add(value);
}

///////////////////////////////////////////////////////////////////////////
// Inserts the specified coordinate at the specified index within the
// collection. Items following the insertion point are moved down to
// accommodate the new item. Throws an invalid argument exception if the
// specified index is out of range.
//
void MgCoordinateCollection::Insert(INT32 index, MgCoordinate* value)
{
    m_collection->Insert(index, value);
}

///////////////////////////////////////////////////////////////////////////
// Removes all coordinates from the collection.
//
void MgCoordinateCollection::Clear()
{
    m_collection->Clear();
}

///////////////////////////////////////////////////////////////////////////
// Removes the specified coordinate from the collection. Throws an invalid
// argument exception if the item does not exist within the collection.
//
bool MgCoordinateCollection::Remove(const MgCoordinate* value)
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
// Removes the coordinate at the specified index from the collection.
// Throws an invalid argument exception if the index does not exist within
// the collection.
//
void MgCoordinateCollection::RemoveAt(INT32 index)
{
    m_collection->RemoveAt(index);
}

///////////////////////////////////////////////////////////////////////////
// Returns true if the collection contains the specified coordinate, false
// otherwise.
//
bool MgCoordinateCollection::Contains(const MgCoordinate* value) const
{
    return m_collection->Contains(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns the index of the specified coordinate in the collection or -1
// if the coordinate does not exist.
//
INT32 MgCoordinateCollection::IndexOf(const MgCoordinate* value) const
{
    return m_collection->IndexOf(value);
}

///////////////////////////////////////////////////////////////////////////
// Returns an iterator for this collection
//
MgCoordinateIterator* MgCoordinateCollection::GetIterator()
{
    return new MgCoordinateIterator(this);
}

//////////////////////////////////////////////////////////////////
// Returns a copy of this collection
//
MgCoordinateCollection* MgCoordinateCollection::Copy()
{
    MgCoordinateCollection* coll = new MgCoordinateCollection();
    for(INT32 i = 0; i < GetCount(); i++)
    {
        Ptr<MgCoordinate> coord = (Ptr<MgCoordinate>(GetItem(i)))->Copy();
        coll->Add(coord);
    }
    return coll;
}

void MgCoordinateCollection::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    INT32 cnt = this->GetCount();

    for (INT32 i = 0;  i < cnt;  i++ )
    {
        if ( i > 0 )
        {
            awktStr += POINT_SEPARATOR;
        }
        Ptr<MgCoordinate> coord = this->GetItem(i);
        coord->ToAwkt(awktStr, coordDim, is2dOnly);
    }
}
