//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#include "CoordinateSystemCache.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgCoordinateSystemCache::MgCoordinateSystemCache()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgCoordinateSystemCache::~MgCoordinateSystemCache()
{
    Clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Get an MgCoordinateSystem instance from the cache, given a definition of
/// the coordinate system in OpenGIS Well-Known Text (WKT) format.
/// If it is not found in the cache, then create one and cache it.
///
/// \param srsWkt
/// A string defining the coordinate system in OpenGIS WKT format.
///
/// \returns
/// An MgCoordinateSystem instance that corresponds to the specified WKT
/// definition.
///
MgCoordinateSystem* MgCoordinateSystemCache::GetCoordinateSystem(CREFSTRING srsWkt)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    MgCoordinateSystem* coordinateSystem = m_coordinateSystemMap[srsWkt];

    if (NULL == coordinateSystem)
    {
        coordinateSystem = new MgCoordinateSystem(srsWkt);
        m_coordinateSystemMap[srsWkt] = coordinateSystem;
    }

    return SAFE_ADDREF(coordinateSystem);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Clear the cache.
///
void MgCoordinateSystemCache::Clear()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    for (MgCoordinateSystemMap::iterator i = m_coordinateSystemMap.begin();
        i != m_coordinateSystemMap.end(); ++i)
    {
        SAFE_RELEASE(i->second);
    }

    m_coordinateSystemMap.clear();
}
