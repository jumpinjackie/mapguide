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

#include "ResourceServiceDefs.h"
#include "ResourceDatabase.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceDatabase::MgResourceDatabase(MgDbEnvironment& environment,
    const string& fileName) :
    MgDatabase(environment),
    m_db(&environment.GetDbEnv(), 0)
{
    assert(!fileName.empty());
    DbTxn* dbTxn = NULL;

    MG_RESOURCE_SERVICE_TRY()

#ifdef _DEBUG
    m_db.set_error_stream(&std::cerr);
#endif

    m_db.set_pagesize(MG_DB_PAGE_SIZE);

    if (m_environment.IsTransacted())
    {
        m_environment.GetDbEnv().txn_begin(0, &dbTxn, 0);
        assert(NULL != dbTxn);
    }

    u_int32_t flags = DB_CREATE|DB_THREAD;

    m_db.open(dbTxn, fileName.c_str(), 0, DB_BTREE, flags, 0);
    m_opened = true;

    if (NULL != dbTxn)
    {
        dbTxn->commit(0);
        dbTxn = NULL;
    }

    Reset();

    MG_RESOURCE_SERVICE_CATCH(L"MgResourceDatabase.MgResourceDatabase")

    if (mgException != NULL)
    {
        try
        {
            if (NULL != dbTxn)
            {
                dbTxn->abort();
            }
        }
        catch (...)
        {
            assert(false);
        }
    }

    MG_RESOURCE_SERVICE_THROW()
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceDatabase::~MgResourceDatabase()
{
    if (m_opened)
    {
        try
        {
            Reset();
            m_db.close(0);
        }
        catch (...)
        {
            assert(false);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return the name of the database.
///
string MgResourceDatabase::GetName()
{
    string name;

    if (m_opened)
    {
        const char *fileName = NULL;
        const char *dbName = NULL;

        m_db.get_dbname(&fileName, &dbName);

        if (NULL == fileName)
        {
            throw new MgNullReferenceException(L"MgResourceDatabase.GetName",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        name = fileName;
    }

    return name;
}
