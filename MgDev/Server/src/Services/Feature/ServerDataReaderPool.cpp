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

#include "MapGuideCommon.h"
#include "ServerFeatureServiceDefs.h"
#include "ServerDataReaderPool.h"

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
    for (DataReaderCollection::iterator iterator = m_drCollection.begin();iterator != m_drCollection.end(); iterator++)
    {
        MgServerDataReader* reader = iterator->second;
        if(reader)
        {
            reader->Release();
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

STRING MgServerDataReaderPool::Add(MgServerDataReader* dataReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    if(NULL == dataReader)
    {
        throw new MgNullArgumentException(L"MgServerDataReaderPool.Add",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get a unique ID
    STRING uid = L"";
    MgUtil::GenerateUuid(uid);

    // Increment the reference count
    dataReader->AddRef();

    // Add it to the collection
    m_drCollection.insert(DataReaderCacheEntry_Pair(uid, dataReader));

    return uid;
}

bool MgServerDataReaderPool::Remove(STRING dataReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    bool bResult = false;

    DataReaderCollection::iterator iterator = m_drCollection.find(dataReader);
    if(m_drCollection.end() != iterator)
    {
        // Release resources
        MgServerDataReader* reader = iterator->second;
        if(reader)
        {
            reader->Release();
        }

        // Remove the reader
        m_drCollection.erase(iterator);
        bResult = true;
    }

    return bResult;
}

MgServerDataReader* MgServerDataReaderPool::GetReader(STRING dataReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    MgServerDataReader* reader = NULL;

    DataReaderCollection::iterator iterator = m_drCollection.find(dataReader);
    if(m_drCollection.end() != iterator)
    {
        // Found it
        reader = iterator->second;

        // Add a reference to it
        reader->AddRef();
    }

    return reader;
}

STRING MgServerDataReaderPool::GetReaderId(MgServerDataReader* dataReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    STRING readerId = L"";

    for (DataReaderCollection::iterator iterator = m_drCollection.begin();iterator != m_drCollection.end(); iterator++)
    {
        if(iterator->second == dataReader)
        {
            // Found it
            readerId = iterator->first;
            break;
        }
    }

    return readerId;
}
