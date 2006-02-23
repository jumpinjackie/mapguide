//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "DbEnvironment.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgDbEnvironment::MgDbEnvironment(CREFSTRING repositoryType, const string& home,
    bool transacted) :
    m_transacted(transacted),
    m_opened(false),
    m_dbEnv(0)
{
    assert(!home.empty());

    MG_RESOURCE_SERVICE_TRY()

#ifdef _DEBUG
    m_dbEnv.set_error_stream(&std::cerr);
#endif

    m_dbEnv.set_cachesize(0, MG_CACHE_SIZE, 1);
    m_dbEnv.set_lg_bsize(MG_LOG_BUF_SIZE);

    u_int32_t containerFlags = DB_CREATE|DB_THREAD;
    u_int32_t environmentFlags = DB_CREATE|DB_THREAD|DB_INIT_MPOOL;

    if (m_transacted)
    {
        // Note that if set_lk_detect is not used, we need to have a worker
        // thread to run a deadlock detector every N seconds.
        // e.g. m_dbEnv.lock_detect(0, DB_LOCK_DEFAULT, 0);

        m_dbEnv.set_lk_detect(DB_LOCK_DEFAULT);

        // Configure the database environment to perform recovery if necessary.
        // Note that the server must start with a cold backup using DB_RECOVER
        // which takes much shorter time than using DB_RECOVER_FATAL.
        // For a hot backup, the user must recover it using our Repository
        // Admin script where the required DB_RECOVER_FATAL must be used.

        environmentFlags |= DB_INIT_TXN|DB_INIT_LOCK|DB_INIT_LOG|DB_RECOVER;

        if (MgRepositoryType::Session == repositoryType)
        {
            m_dbEnv.set_flags(DB_LOG_INMEMORY, 1);
            environmentFlags |= DB_PRIVATE;
        }

        containerFlags |= DBXML_TRANSACTIONAL;
    }
    else
    {
        m_dbEnv.set_flags(DB_CDB_ALLDB, 1);
        environmentFlags |= DB_INIT_CDB;
    }

    m_dbEnv.open(home.c_str(), environmentFlags, 0);
    m_opened = true;

    // Initialize the XML manager.

    m_xmlMan = XmlManager(&m_dbEnv, DBXML_ALLOW_EXTERNAL_ACCESS);
    m_xmlMan.setDefaultPageSize(MG_DBXML_PAGE_SIZE);
    m_xmlMan.setDefaultContainerType(XmlContainer::WholedocContainer);

    if (m_schemaResolver.IsValidationEnabled())
    {
        containerFlags |= DBXML_ALLOW_VALIDATION;
        m_xmlMan.registerResolver(m_schemaResolver);
    }

    m_xmlMan.setDefaultContainerFlags(containerFlags);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgDbEnvironment.MgDbEnvironment")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgDbEnvironment::~MgDbEnvironment()
{
    if (m_opened)
    {
        try
        {
            m_dbEnv.close(0);
        }
        catch (...)
        {
            assert(false);
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Flushes any cached information to disk.
/// </summary>
///
/// <exceptions>
/// MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgDbEnvironment::PerformCheckpoint(UINT32 flags)
{
    // Note that DbEnv::memp_sync(0), Db::sync(0), and XmlContainer::sync
    // get called internally.

    if (m_opened)
    {
        m_dbEnv.txn_checkpoint(0, 0, flags);
    }
}
