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

#include "CoordinateSystemCache.h"

// Process-wide singleton
MgCoordinateSystemCache MgCoordinateSystemCache::sm_coordinateSystemCache;

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
/// \remarks
/// Assuming the MG Server can hande a reasonable number of coordinate systems,
/// the Remove method is not exposed/implemented.
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
/// Get pointer to a process-wide singleton.
///
MgCoordinateSystemCache* MgCoordinateSystemCache::GetInstance()
{
    return &MgCoordinateSystemCache::sm_coordinateSystemCache;
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
#ifdef _DEBUG
        if (NULL != i->second && 1 != i->second->GetRefCount())
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgCoordinateSystemCache::Clear - Memory leaks detected. Reference count = %d : %W\n"),
                i->second->GetRefCount(), i->first.c_str()));
        }
#endif
        SAFE_RELEASE(i->second);
    }

    m_coordinateSystemMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set an MgCoordinateSystem instance to the cache.
///
/// \param key
/// The OpenGIS WKT definition or the CS code.
///
/// \param value
/// The MgCoordinateSystem instance to be saved to the cache.
///
void MgCoordinateSystemCache::Set(CREFSTRING key, MgCoordinateSystem* value)
{
    CHECKARGUMENTEMPTYSTRING(key, L"MgCoordinateSystemCache.Set");
    CHECKARGUMENTNULL(value, L"MgCoordinateSystemCache.Set");

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    SAFE_RELEASE(m_coordinateSystemMap[key]);
    m_coordinateSystemMap[key] = SAFE_ADDREF(value);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Get an MgCoordinateSystem instance from the cache.
///
/// \remarks
/// The returned value could be NULL.
///
/// \param key
/// The OpenGIS WKT definition or the CS code.
///
/// \return
/// An MgCoordinateSystem instance that corresponds to the specified OpenGIS WKT
/// definition or the CS code.
///
MgCoordinateSystem* MgCoordinateSystemCache::Get(CREFSTRING key)
{
    CHECKARGUMENTEMPTYSTRING(key, L"MgCoordinateSystemCache.Get");

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    return SAFE_ADDREF(m_coordinateSystemMap[key]);
}
