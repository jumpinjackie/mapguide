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
#include "ServerDataReaderPool.h"
#include "ServerDataProcessor.h"

// Process-wide MgServerDataReaderPool
Ptr<MgServerDataReaderPool> MgServerDataReaderPool::m_drPool = MgServerDataReaderPool::GetInstance();

/// <summary>
/// Constructor
/// </summary>
MgServerDataReaderPool::MgServerDataReaderPool()
{
    m_drCollection = new MgDisposableCollection();
}

/// <summary>
/// Destructor
/// </summary>
MgServerDataReaderPool::~MgServerDataReaderPool()
{
    if (m_drCollection != NULL)
    {
        m_drCollection->Clear();
    }
}

/// <summary>
/// Self Destructor
/// </summary>
void MgServerDataReaderPool::Dispose()
{
    delete this;
}

/// <summary>
/// Get pointer to a process-wide MgServerDataReaderPool.
/// </summary>
MgServerDataReaderPool* MgServerDataReaderPool::GetInstance()
{
    MG_CONFIGURATION_TRY()

    ACE_TRACE ("MgServerDataReaderPool::GetInstance");

    if (MgServerDataReaderPool::m_drPool == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgServerDataReaderPool::m_drPool == NULL)
        {
            MgServerDataReaderPool::m_drPool = new MgServerDataReaderPool;
        }
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgServerDataReaderPool.GetInstance")

    // To avoid overheads and maintain thread safety.
    // do not assign this returned static singleton to a Ptr object.
    return MgServerDataReaderPool::m_drPool;
}

void MgServerDataReaderPool::Add(MgServerDataProcessor* dataReader)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_drCollection->Add(dataReader);
}

void MgServerDataReaderPool::Remove(MgServerDataProcessor* dataReader)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_drCollection->Remove(dataReader);
}

bool MgServerDataReaderPool::Contains(MgServerDataProcessor* dataReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    return m_drCollection->Contains(dataReader);
}
