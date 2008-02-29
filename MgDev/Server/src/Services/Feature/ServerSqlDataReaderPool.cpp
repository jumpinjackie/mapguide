//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureReaderIdentifier.h"
#include "ServerSqlDataReaderPool.h"
#include "ServerSqlProcessor.h"

// Process-wide MgServerSqlDataReaderPool
Ptr<MgServerSqlDataReaderPool> MgServerSqlDataReaderPool::m_drPool = MgServerSqlDataReaderPool::GetInstance();

/// <summary>
/// Constructor
/// </summary>
MgServerSqlDataReaderPool::MgServerSqlDataReaderPool()
{
    m_drCollection = new MgDisposableCollection();
}

/// <summary>
/// Destructor
/// </summary>
MgServerSqlDataReaderPool::~MgServerSqlDataReaderPool()
{
    if (m_drCollection != NULL)
    {
        m_drCollection->Clear();
    }
}

/// <summary>
/// Self Destructor
/// </summary>
void MgServerSqlDataReaderPool::Dispose()
{
    delete this;
}

/// <summary>
/// Get pointer to a process-wide MgServerSqlDataReaderPool.
/// </summary>
MgServerSqlDataReaderPool* MgServerSqlDataReaderPool::GetInstance()
{
    MG_CONFIGURATION_TRY()

    ACE_TRACE ("MgServerSqlDataReaderPool::GetInstance");

    if (MgServerSqlDataReaderPool::m_drPool == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgServerSqlDataReaderPool::m_drPool == NULL)
        {
            MgServerSqlDataReaderPool::m_drPool = new MgServerSqlDataReaderPool;
        }
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgServerSqlDataReaderPool.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgServerSqlDataReaderPool::m_drPool;
}

void MgServerSqlDataReaderPool::Add(MgServerSqlProcessor* sqlReader)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_drCollection->Add(sqlReader);
}

void MgServerSqlDataReaderPool::Remove(MgServerSqlProcessor* sqlReader)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_drCollection->Remove(sqlReader);
}

bool MgServerSqlDataReaderPool::Contains(MgServerSqlProcessor* sqlReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    return m_drCollection->Contains(sqlReader);
}
