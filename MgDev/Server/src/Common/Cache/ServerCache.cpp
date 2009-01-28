//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "ServerCache.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgServerCache::MgServerCache() :
    m_size(0),
    m_timeLimit(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgServerCache::~MgServerCache()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Dispose the object.
///
void MgServerCache::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initialize the object.
///
void MgServerCache::Initialize(INT32 size, INT32 timeLimit)
{
    if (size <= 0 || timeLimit <=0)
    {
        throw new MgArgumentOutOfRangeException(L"MgServerCache.Initialize",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_size = size;
    m_timeLimit = ACE_Time_Value(timeLimit);
}
