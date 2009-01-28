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
#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureReaderIdentifier.h"
#include "ServerFeatureReaderIdentifierPool.h"
#include "ServerFeatureProcessor.h"

// Process-wide MgServerFeatureReaderIdentifierPool
Ptr<MgServerFeatureReaderIdentifierPool> MgServerFeatureReaderIdentifierPool::m_fridPool =
    (MgServerFeatureReaderIdentifierPool*)NULL;

/// <summary>
/// Constructor
/// </summary>
MgServerFeatureReaderIdentifierPool::MgServerFeatureReaderIdentifierPool()
{
    m_fridCollection = new MgDisposableCollection();
}

/// <summary>
/// Destructor
/// </summary>
MgServerFeatureReaderIdentifierPool::~MgServerFeatureReaderIdentifierPool()
{
}

/// <summary>
/// Self Destructor
/// </summary>
void MgServerFeatureReaderIdentifierPool::Dispose()
{
    delete this;
}

/// <summary>
/// Get pointer to a process-wide MgServerFeatureReaderIdentifierPool.
/// </summary>
MgServerFeatureReaderIdentifierPool* MgServerFeatureReaderIdentifierPool::GetInstance()
{
    MG_CONFIGURATION_TRY()

    ACE_TRACE ("MgServerFeatureReaderIdentifierPool::GetInstance");

    if (MgServerFeatureReaderIdentifierPool::m_fridPool == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgServerFeatureReaderIdentifierPool::m_fridPool == NULL)
        {
            MgServerFeatureReaderIdentifierPool::m_fridPool = new MgServerFeatureReaderIdentifierPool;
        }
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgServerFeatureReaderIdentifierPool.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgServerFeatureReaderIdentifierPool::m_fridPool;
}

void MgServerFeatureReaderIdentifierPool::Add(MgServerFeatureProcessor* featId)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_fridCollection->Add(featId);
}

void MgServerFeatureReaderIdentifierPool::Remove(MgServerFeatureProcessor* featId)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_fridCollection->Remove(featId);
}

bool MgServerFeatureReaderIdentifierPool::Contains(MgServerFeatureProcessor* featId)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    return m_fridCollection->Contains(featId);
}
