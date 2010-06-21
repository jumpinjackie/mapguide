//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "ServerFeatureReaderPool.h"

// Process-wide MgServerFeatureReaderPool
Ptr<MgServerFeatureReaderPool> MgServerFeatureReaderPool::m_frPool =
    (MgServerFeatureReaderPool*)NULL;

/// <summary>
/// Constructor
/// </summary>
MgServerFeatureReaderPool::MgServerFeatureReaderPool()
{
}

/// <summary>
/// Destructor
/// </summary>
MgServerFeatureReaderPool::~MgServerFeatureReaderPool()
{
    for (FeatureReaderCollection::iterator iterator = m_frCollection.begin();iterator != m_frCollection.end(); iterator++)
    {
        MgFeatureReader* reader = iterator->second;
        if(reader)
        {
            reader->Release();
        }
    }

    m_frCollection.clear();
}

/// <summary>
/// Self Destructor
/// </summary>
void MgServerFeatureReaderPool::Dispose()
{
    delete this;
}

/// <summary>
/// Get pointer to a process-wide MgServerFeatureReaderPool.
/// </summary>
MgServerFeatureReaderPool* MgServerFeatureReaderPool::GetInstance()
{
    MG_CONFIGURATION_TRY()

    ACE_TRACE ("MgServerFeatureReaderPool::GetInstance");

    if (MgServerFeatureReaderPool::m_frPool == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgServerFeatureReaderPool::m_frPool == NULL)
        {
            MgServerFeatureReaderPool::m_frPool = new MgServerFeatureReaderPool;
        }
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgServerFeatureReaderPool.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgServerFeatureReaderPool::m_frPool;
}

STRING MgServerFeatureReaderPool::Add(MgFeatureReader* featureReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    if(NULL == featureReader)
    {
        throw new MgNullArgumentException(L"MgServerFeatureReaderPool.Add",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get a unique ID
    STRING uid = L"";
    MgUtil::GenerateUuid(uid);

    // Increment the reference count
    featureReader->AddRef();

    // Add it to the collection
    m_frCollection.insert(FeatureReaderCacheEntry_Pair(uid, featureReader));

    return uid;
}

bool MgServerFeatureReaderPool::Remove(STRING featureReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    bool bResult = false;
    INT32 ref = 0;

    FeatureReaderCollection::iterator iterator = m_frCollection.find(featureReader);
    if(m_frCollection.end() != iterator)
    {
        // Release resources
        MgFeatureReader* reader = iterator->second;
        if(reader)
        {
            ref = reader->Release();
        }

        // Remove the reader
        m_frCollection.erase(iterator);
        bResult = true;
    }

    return bResult;
}

MgFeatureReader* MgServerFeatureReaderPool::GetReader(STRING featureReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    MgFeatureReader* reader = NULL;

    FeatureReaderCollection::iterator iterator = m_frCollection.find(featureReader);
    if(m_frCollection.end() != iterator)
    {
        // Found it
        reader = iterator->second;

        // Add a reference to it
        reader->AddRef();
    }

    return reader;
}

STRING MgServerFeatureReaderPool::GetReaderId(MgFeatureReader* featureReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    STRING readerId = L"";

    for (FeatureReaderCollection::iterator iterator = m_frCollection.begin();iterator != m_frCollection.end(); iterator++)
    {
        if(iterator->second == featureReader)
        {
            // Found it
            readerId = iterator->first;
            break;
        }
    }

    return readerId;
}
