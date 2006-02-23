//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "CoordinateSystemCommon.h"

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs and intializes a coordinate system factory.
///</summary>
MgCoordinateSystemFactory::MgCoordinateSystemFactory()
{
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Destructor.
///</summary>
MgCoordinateSystemFactory::~MgCoordinateSystemFactory()
{
    //release cached coord systems
    for (std::map<STRING, MgCoordinateSystem*>::iterator iter = m_hCSCache.begin();
        iter != m_hCSCache.end(); iter++)
    {
        if (NULL != iter->second)
            (iter->second)->Release();
    }

    m_hCSCache.clear();
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Creates a coordinate system instance given a definition of the
/// coordinate system in OpenGIS Well-Known Text (WKT) format.
///</summary>
///<param name="srsWkt">
/// A string defining the coordinate system in OpenGIS WKT format.
///</param>
///<returns>
/// An MgCoordinateSystem instance that cooresponds to the specified WKT
/// definition.
///</returns>
MgCoordinateSystem* MgCoordinateSystemFactory::Create(CREFSTRING srsWkt)
{
    //see if we have this coordinate system in the cache
    MgCoordinateSystem* pCoordinateSystem = m_hCSCache[srsWkt];

    //if not, create one and also cache it
    if (NULL == pCoordinateSystem)
    {
        //lock all threads while we create the coordinate system
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

        //we have to check for NULL again because we might have been locked out by
        //another thread that just finished initializing the coordinate system
        pCoordinateSystem = m_hCSCache[srsWkt];

        if (NULL == pCoordinateSystem)
        {
            pCoordinateSystem = new MgCoordinateSystem(srsWkt);
            m_hCSCache[srsWkt] = pCoordinateSystem;
        }
    }

    return SAFE_ADDREF(pCoordinateSystem);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgCoordinateSystemFactory::GetClassId()
{
    return m_cls_id;
}

// <summary>Dispose this object.</summary>
// <returns>Returns nothing</returns>
void MgCoordinateSystemFactory::Dispose()
{
    delete this;
}
