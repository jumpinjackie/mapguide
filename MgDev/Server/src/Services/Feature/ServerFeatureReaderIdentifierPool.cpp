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
}

/// <summary>
/// Destructor
/// </summary>
MgServerFeatureReaderIdentifierPool::~MgServerFeatureReaderIdentifierPool()
{
    for (FeatureProcessorCollection::iterator iterator = m_fridCollection.begin();iterator != m_fridCollection.end(); iterator++)
    {
        MgServerFeatureProcessor* processor = iterator->second;
        if(processor)
        {
            processor->Release();
        }
    }

    m_fridCollection.clear();
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

STRING MgServerFeatureReaderIdentifierPool::Add(MgServerFeatureProcessor* processor)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    // Get a unique ID
    STRING uid = L"";
    MgUtil::GenerateUuid(uid);

    if(NULL == processor)
    {
        throw new MgNullArgumentException(L"MgServerFeatureReaderIdentifierPool.Add",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Increment the reference count
    processor->AddRef();

    // Add it to the collection
    m_fridCollection.insert(FeatureProcessorCacheEntry_Pair(uid, processor));

    return uid;
}

bool MgServerFeatureReaderIdentifierPool::Remove(STRING featureReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    bool bResult = false;
    INT32 ref = 0;

    FeatureProcessorCollection::iterator iterator = m_fridCollection.find(featureReader);
    if(m_fridCollection.end() != iterator)
    {
        // Release resources
        MgServerFeatureProcessor* processor = iterator->second;
        if(processor)
        {
            ref = processor->Release();
        }

        // Remove the processor
        m_fridCollection.erase(iterator);
        bResult = true;
    }

    return bResult;
}

MgServerFeatureProcessor* MgServerFeatureReaderIdentifierPool::GetProcessor(STRING featureReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    MgServerFeatureProcessor* processor = NULL;

    FeatureProcessorCollection::iterator iterator = m_fridCollection.find(featureReader);
    if(m_fridCollection.end() != iterator)
    {
        // Found it
        processor = iterator->second;
        
        // Add a reference to it
        processor->AddRef();
    }

    return processor;
}

STRING MgServerFeatureReaderIdentifierPool::GetReaderId(MgServerFeatureProcessor* processor)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    STRING readerId = L"";

    for (FeatureProcessorCollection::iterator iterator = m_fridCollection.begin();iterator != m_fridCollection.end(); iterator++)
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
