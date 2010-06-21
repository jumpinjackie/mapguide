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
#include "ServerSqlDataReaderPool.h"

// Process-wide MgServerSqlDataReaderPool
Ptr<MgServerSqlDataReaderPool> MgServerSqlDataReaderPool::m_drPool = MgServerSqlDataReaderPool::GetInstance();

/// <summary>
/// Constructor
/// </summary>
MgServerSqlDataReaderPool::MgServerSqlDataReaderPool()
{
}

/// <summary>
/// Destructor
/// </summary>
MgServerSqlDataReaderPool::~MgServerSqlDataReaderPool()
{
    for (SqlDataReaderCollection::iterator iterator = m_drCollection.begin();iterator != m_drCollection.end(); iterator++)
    {
        MgServerSqlDataReader* reader = iterator->second;
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

STRING MgServerSqlDataReaderPool::Add(MgServerSqlDataReader* sqlReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    if(NULL == sqlReader)
    {
        throw new MgNullArgumentException(L"MgServerSqlDataReaderPool.Add",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get a unique ID
    STRING uid = L"";
    MgUtil::GenerateUuid(uid);

    // Increment the reference count
    sqlReader->AddRef();

    // Add it to the collection
    m_drCollection.insert(SqlDataReaderCacheEntry_Pair(uid, sqlReader));

    return uid;
}

bool MgServerSqlDataReaderPool::Remove(STRING sqlReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    bool bResult = false;

    SqlDataReaderCollection::iterator iterator = m_drCollection.find(sqlReader);
    if(m_drCollection.end() != iterator)
    {
        // Release resources
        MgServerSqlDataReader* reader = iterator->second;
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

MgServerSqlDataReader* MgServerSqlDataReaderPool::GetReader(STRING sqlReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    MgServerSqlDataReader* reader = NULL;

    SqlDataReaderCollection::iterator iterator = m_drCollection.find(sqlReader);
    if(m_drCollection.end() != iterator)
    {
        // Found it
        reader = iterator->second;

        // Add a reference to it
        reader->AddRef();
    }

    return reader;
}

STRING MgServerSqlDataReaderPool::GetReaderId(MgServerSqlDataReader* sqlReader)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    STRING readerId = L"";

    for (SqlDataReaderCollection::iterator iterator = m_drCollection.begin();iterator != m_drCollection.end(); iterator++)
    {
        if(iterator->second == sqlReader)
        {
            // Found it
            readerId = iterator->first;
            break;
        }
    }

    return readerId;
}
