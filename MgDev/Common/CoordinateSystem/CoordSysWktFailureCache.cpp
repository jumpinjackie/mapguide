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
#include "CoordSysCommon.h"
#include "CoordSysWktFailureCache.h"

using namespace CSLibrary;

// Process-wide singleton
CCoordinateSystemWktFailureCache CCoordinateSystemWktFailureCache::sm_wktFailureCache;

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
/// \remarks
/// Assuming the host can hande a reasonable number of WKT strings, the Remove
/// method is not exposed/implemented.  Since this is a performance only
/// feature, executing a Clear () method call at judicious times is the way to
/// reduce resource usage.
///
CCoordinateSystemWktFailureCache::CCoordinateSystemWktFailureCache()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
CCoordinateSystemWktFailureCache::~CCoordinateSystemWktFailureCache()
{
    Clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Get pointer to a process-wide singleton.
///
CCoordinateSystemWktFailureCache* CCoordinateSystemWktFailureCache::GetInstance()
{
    return &CCoordinateSystemWktFailureCache::sm_wktFailureCache;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Clear the cache.
///
void CCoordinateSystemWktFailureCache::Clear()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    for (CWktExceptionMap::iterator i = m_wktExceptionMap.begin();
        i != m_wktExceptionMap.end(); ++i)
    {
#ifdef _DEBUG
        if (NULL != i->second && 1 != i->second->GetRefCount())
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) CCoordinateSystemWktFailureCache::Clear - Memory leaks detected. Reference count = %d : %W\n"),
                i->second->GetRefCount(), i->first.c_str()));
        }
#endif
        SAFE_RELEASE(i->second);
    }
    m_wktExceptionMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set an MgException instance to the cache.
///
/// \param key
/// The OpenGIS WKT definition which caused the provided MgException to be
/// generated.
///
/// \param value
/// The MgException instance to be saved to the cache.
///
void CCoordinateSystemWktFailureCache::Set(CREFSTRING key, MgException* mgException)
{
    CHECKARGUMENTEMPTYSTRING(key, L"CCoordinateSystemWktFailureCache.Set");
    CHECKARGUMENTNULL(mgException, L"CCoordinateSystemWktFailureCache.Set");

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    SAFE_RELEASE(m_wktExceptionMap[key]);
    m_wktExceptionMap[key] = SAFE_ADDREF(mgException);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns true if the specified key exists in the map.
///
/// \remarks
/// Does not cause an entry with a NULL exception to be added to the map.
///
/// \param key
/// The WKT definition
///
/// \return
/// true if an entry in the map with the provided key exists;
/// otherwise returns false.
///
bool CCoordinateSystemWktFailureCache::Has(CREFSTRING key)
{
    bool hasKey;

    CHECKARGUMENTEMPTYSTRING(key, L"CCoordinateSystemWktFailureCache.Has");

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    CWktExceptionMap::iterator mapItr = m_wktExceptionMap.find (key);
    hasKey = (mapItr != m_wktExceptionMap.end ());
    return hasKey;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Get an MgCoordinateSystem instance from the cache.
///
/// \remarks
/// The returned value could be NULL.
///
/// \param key
/// The WKT definition
///
/// \return
/// The MgException instance generated when the provided WKT string failed.
/// If the WKT string has not generated a processing failure, the NULL
/// pointer is returned.
///
MgException* CCoordinateSystemWktFailureCache::Get(CREFSTRING key)
{
    CHECKARGUMENTEMPTYSTRING(key, L"CCoordinateSystemWktFailureCache.Get");

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    return SAFE_ADDREF(m_wktExceptionMap[key]);
}
