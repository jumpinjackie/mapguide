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

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The MgCoordinateIterator class supports iteration over the
/// coordinates of a geometry or path instruction. MgCoordinateIterator only
/// supports read access to the underlying coordinates. Initially, the
/// iterator is positioned before the first coordinate. Reset also brings
/// the iterator back to this position. At this position, calling
/// GetCurrent throws an exception. Therefore, you must call MoveNext to
/// advance the iterator to the first coordinate before calling GetCurrent.
///</summary>

#include "GeometryCommon.h"

//////////////////////////////////////////////////////////////////
// Construct an MgCoordinateIterator object based on a collection
//
MgCoordinateIterator::MgCoordinateIterator(MgCoordinateCollection* collection)
{
    // Null is not allowed
    CHECKARGUMENTNULL(collection, L"MgCoordinateIterator.MgCoordinateIterator");

    // Increment the counter on collection
    m_coordinateCollection = SAFE_ADDREF(collection);
    m_currPos = -1;
}

//////////////////////////////////////////////////////////////////
// Construct an empty object of type MgCoordinateIterator (for Ptr<>)
//
MgCoordinateIterator::MgCoordinateIterator()
{
}

//////////////////////////////////////////////////////////////////
// MgCoordinateIterator destructor
//
MgCoordinateIterator::~MgCoordinateIterator()
{
}

//////////////////////////////////////////////////////////////////
// Returns the element at the current position of this iterator
//
MgCoordinate* MgCoordinateIterator::GetCurrent()
{
    // Null is not allowed
    CHECKARGUMENTNULL((MgCoordinateCollection*)m_coordinateCollection, L"MgCoordinateIterator.GetCurrent");

    return (MgCoordinate*)m_coordinateCollection->GetItem(m_currPos);
}

//////////////////////////////////////////////////////////////////
// Move this iterator position to the next element
//
bool MgCoordinateIterator::MoveNext()
{
    // Null is not allowed
    CHECKNULL((MgCoordinateCollection*)m_coordinateCollection, L"MgCoordinateIterator.MoveNext");

    bool allowNextFetch = false;

    INT32 cnt = m_coordinateCollection->GetCount();

    if (cnt > 0 && m_currPos < (cnt-1))
    {
        m_currPos++;
        allowNextFetch = true;
    }

    return allowNextFetch;
}

//////////////////////////////////////////////////////////////////
// Reset this iterator position to before the first element
//
void MgCoordinateIterator::Reset()
{
    m_currPos = -1;
}

//////////////////////////////////////////////////////////////////
// Return the class id
//
INT32 MgCoordinateIterator::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgCoordinateIterator::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Return the number of elements in the associated collection
//
INT32 MgCoordinateIterator::GetCount()
{
    return m_coordinateCollection->GetCount();
}
