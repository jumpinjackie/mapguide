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
#include "ServerDataReaderPool.h"
#include "ServerDataProcessor.h"

// Process-wide MgServerDataReaderPool
Ptr<MgServerDataReaderPool> MgServerDataReaderPool::m_drPool = MgServerDataReaderPool::GetInstance();

/// <summary>
/// Constructor
/// </summary>
MgServerDataReaderPool::MgServerDataReaderPool()
{
}

/// <summary>
/// Destructor
/// </summary>
MgServerDataReaderPool::~MgServerDataReaderPool()
{
    for (DataProcessorCollection::iterator iterator = m_drCollection.begin();iterator != m_drCollection.end(); iterator++)
    {
        MgServerDataProcessor* processor = iterator->second;
        if(processor)
        {
            processor->Release();
        }
    }

    m_drCollection.clear();
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

STRING MgServerDataReaderPool::Add(MgServerDataProcessor* processor)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    // Get a unique ID
    STRING uid = L"";
    MgUtil::GenerateUuid(uid);

    if(NULL == processor)
    {
        throw new MgNullArgumentException(L"MgServerDataReaderPool.Add",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Increment the reference count
    processor->AddRef();

    // Add it to the collection
    m_drCollection.insert(DataProcessorCacheEntry_Pair(uid, processor));

    return uid;
}

bool MgServerDataReaderPool::Remove(STRING dataReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    bool bResult = false;

    DataProcessorCollection::iterator iterator = m_drCollection.find(dataReader);
    if(m_drCollection.end() != iterator)
    {
        // Release resources
        MgServerDataProcessor* processor = iterator->second;
        if(processor)
        {
            processor->Release();
        }

        // Remove the processor
        m_drCollection.erase(iterator);
        bResult = true;
    }

    return bResult;
}

MgServerDataProcessor* MgServerDataReaderPool::GetProcessor(STRING dataReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    MgServerDataProcessor* processor = NULL;

    DataProcessorCollection::iterator iterator = m_drCollection.find(dataReader);
    if(m_drCollection.end() != iterator)
    {
        // Found it
        processor = iterator->second;

        // Add a reference to it
        processor->AddRef();
    }

    return processor;
}

STRING MgServerDataReaderPool::GetReaderId(MgServerDataProcessor* processor)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    STRING readerId = L"";

    for (DataProcessorCollection::iterator iterator = m_drCollection.begin();iterator != m_drCollection.end(); iterator++)
    {
        if(iterator->second == processor)
        {
            // Found it
            readerId = iterator->first;
            break;
        }
    }

    return readerId;
}
