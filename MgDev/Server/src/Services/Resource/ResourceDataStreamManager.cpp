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

#include "ResourceServiceDefs.h"
#include "ResourceDataStreamManager.h"
#include "ApplicationRepository.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceDataStreamManager::MgResourceDataStreamManager(
    MgApplicationRepositoryManager& repositoryMan) :
    MgResourceDataManager(repositoryMan),
    m_database(((MgApplicationRepository&)repositoryMan.GetRepository()).
        GetResourceDataStreamDatabase()->GetDb())
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceDataStreamManager::~MgResourceDataStreamManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds resource data to the database if it does not exists, or updates it
/// otherwise.
/// </summary>
///
/// <exceptions>
/// MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDataStreamManager::SetResourceData(const string& dataKey,
    CREFSTRING dataName, MgByteReader* byteReader)
{
    assert(NULL != byteReader);

    MG_RESOURCE_SERVICE_TRY()

    // Ensure the current operation is transactionally protected.
    m_repositoryMan.ValidateTransaction();

    // Create a byte sink.

    MgByteSink byteSink(byteReader);
    Ptr<MgByte> buffer = byteSink.ToBuffer();
    u_int32_t dataSize = buffer->GetLength();
    unsigned char* dataBuffer = (unsigned char*) buffer->Bytes();

    if (NULL == dataBuffer || 0 == dataSize)
    {
        throw new MgNullArgumentException(
            L"MgResourceDataStreamManager.SetResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Add the resource data.

    Dbt key, data;

    ::memset(&key, 0, sizeof(key));
    ::memset(&data, 0, sizeof(data));

    key.set_data(reinterpret_cast<void*>(const_cast<char*>(dataKey.c_str())));
    key.set_size(static_cast<u_int32_t>(dataKey.length() + 1));
    data.set_data(reinterpret_cast<void*>(dataBuffer));
    data.set_size(dataSize);

    m_database.put(GetDbTxn(), &key, &data, 0);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceDataStreamManager.SetResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes resource data from the database.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDataStreamManager::DeleteResourceData(const string& dataKey,
    CREFSTRING dataName, bool strict)
{
    MG_RESOURCE_SERVICE_TRY()

    // Ensure the current operation is transactionally protected.
    m_repositoryMan.ValidateTransaction();

    // Delete the resource data.

    Dbt key;

    ::memset(&key, 0, sizeof(key));

    key.set_data(reinterpret_cast<void*>(const_cast<char*>(dataKey.c_str())));
    key.set_size(static_cast<u_int32_t>(dataKey.length() + 1));

    if (DB_NOTFOUND == m_database.del(GetDbTxn(), &key, 0) && strict)
    {
        MgStringCollection arguments;
        arguments.Add(dataName);

        throw new MgResourceDataNotFoundException(
            L"MgResourceDataStreamManager.DeleteResourceData",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceDataStreamManager.DeleteResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Copies resource data in the database from one location to another.
/// </summary>
///
/// <exceptions>
/// MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDataStreamManager::CopyResourceData(const string& sourceDataKey,
    const string& destDataKey, CREFSTRING dataName)
{
    Dbt data;

    MG_RESOURCE_SERVICE_TRY()

    // Ensure the current operation is transactionally protected.
    m_repositoryMan.ValidateTransaction();

    // Get the source MgResourceDataStreamManager.
    MgApplicationRepositoryManager* sourceRepositoryMan =
        dynamic_cast<MgApplicationRepositoryManager*>(
        ((MgApplicationRepositoryManager&)m_repositoryMan).GetSourceRepositoryManager());
    ACE_ASSERT(NULL != sourceRepositoryMan);
    MgResourceDataStreamManager* sourceResourceDataStreamMan =
        sourceRepositoryMan->GetResourceDataStreamManager();
    ACE_ASSERT(NULL != sourceResourceDataStreamMan);

    // Get the resource data.

    Dbt key;
    ::memset(&key, 0, sizeof(key));
    ::memset(&data, 0, sizeof(data));

    key.set_data(reinterpret_cast<void*>(const_cast<char*>(sourceDataKey.c_str())));
    key.set_size(static_cast<u_int32_t>(sourceDataKey.length() + 1));
    data.set_flags(DB_DBT_MALLOC);

    if (DB_NOTFOUND == sourceResourceDataStreamMan->m_database.get(
        sourceResourceDataStreamMan->GetDbTxn(), &key, &data, 0))
    {
        MgStringCollection arguments;
        arguments.Add(dataName);

        throw new MgResourceDataNotFoundException(
            L"MgResourceDataStreamManager.CopyResourceData",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Copy the resource data.

    ::memset(&key, 0, sizeof(key));

    key.set_data(reinterpret_cast<void*>(const_cast<char*>(destDataKey.c_str())));
    key.set_size(static_cast<u_int32_t>(destDataKey.length() + 1));

    m_database.put(GetDbTxn(), &key, &data, 0);

    MG_RESOURCE_SERVICE_CATCH(L"MgResourceDataStreamManager.CopyResourceData")

    ::free(data.get_data());

    MG_RESOURCE_SERVICE_THROW()
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets resource data from the database.
/// </summary>
///
/// <exceptions>
/// MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgResourceDataStreamManager::GetResourceData(
    const string& dataKey, CREFSTRING dataName, CREFSTRING mimeType)
{
    Ptr<MgByteReader> byteReader;
    void* buf = 0;

    MG_RESOURCE_SERVICE_TRY()

    // Get the resource data.

    Dbt key, data;

    ::memset(&key, 0, sizeof(key));
    ::memset(&data, 0, sizeof(data));

    key.set_data(reinterpret_cast<void*>(const_cast<char*>(dataKey.c_str())));
    key.set_size(static_cast<u_int32_t>(dataKey.length() + 1));
    data.set_flags(DB_DBT_MALLOC);

    if (DB_NOTFOUND == m_database.get(GetDbTxn(), &key, &data, 0))
    {
        MgStringCollection arguments;
        arguments.Add(dataName);

        throw new MgResourceDataNotFoundException(
            L"MgResourceDataStreamManager.GetResourceData",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Create a byte source.

    buf = data.get_data();
    assert(NULL != buf);
    Ptr<MgByteSource> byteSource = new MgByteSource( // TODO: Optimize this by using direct memory data.
        reinterpret_cast<unsigned char*>(buf), data.get_size());

    byteSource->SetMimeType(mimeType);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH(L"MgResourceDataStreamManager.GetResourceData")

    ::free(buf);

    MG_RESOURCE_SERVICE_THROW()

    return SAFE_ADDREF((MgByteReader*)byteReader);
}
