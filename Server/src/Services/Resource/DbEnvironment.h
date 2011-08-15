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

#ifndef MGDBENVIRONMENT_H_
#define MGDBENVIRONMENT_H_

#include "XmlSchemaResolver.h"

// All sizes must be in powers-of-two.
// TODO: Determine the best sizes to improve the Server performance.
static const u_int32_t MG_KB                   = 1024;
static const u_int32_t MG_MB                   = 1048576; // 1024^2
static const u_int32_t MG_MICROSECOND          = 1000000;

class MgDatabase;

class MgDbEnvironment
{
/// Constructors/Destructor

    public:

        MgDbEnvironment(CREFSTRING repositoryType, const string& home,
            bool transacted = true);
        virtual ~MgDbEnvironment();

    private:

        // Unimplemented Constructors/Methods

        MgDbEnvironment();
        MgDbEnvironment(const MgDbEnvironment&);
        MgDbEnvironment& operator=(const MgDbEnvironment&);

/// Methods

    public:

        bool IsTransacted() const;

        DbEnv& GetDbEnv();
        XmlManager& GetXmlManager();
        MgXmlSchemaResolver& GetXmlSchemaResolver();

        void PerformCheckpoint(UINT32 flags = 0);
        void ResetDatabase(MgDatabase& database);

        INT32 getDBPageSize();
        INT32 getSessionDBPageSize();

/// Data Members

    private:

        const bool m_transacted;
        bool m_opened;
        DbEnv m_dbEnv;
        XmlManager m_xmlMan;
        MgXmlSchemaResolver m_schemaResolver;

        //configurable settings
        INT32 MG_LIBRARY_CACHE_SIZE; //32 * MG_MB by default
        INT32 MG_SESSION_CACHE_SIZE; //2 * MG_MB by default
        INT32 MG_DB_PAGE_SIZE;  //32 * MG_KB by default
        INT32 MG_DBXML_PAGE_SIZE;  //32 * MG_KB by default
        INT32 MG_LIBRARY_LOG_BUF_SIZE;  //12 * MG_MB by default
        INT32 MG_SESSION_LOG_BUF_SIZE;  //1 * MG_MB by default
        INT32 MG_MAX_TRANSACTIONS;  //1000 by default
        INT32 MG_SESS_DB_PAGE_SIZE;  //2 * MG_KB by default
        double MG_SESS_DBXML_PAGE_SIZE;   //512 by default
        double MG_DB_ENV_TIMEOUT;  //0.2 * MG_MICROSECOND by default
        INT32 MG_MAX_LOCKS;   //1000 by default
};

/// Inline Methods

inline bool MgDbEnvironment::IsTransacted() const
{
    return m_transacted;
}

inline DbEnv& MgDbEnvironment::GetDbEnv()
{
    return m_dbEnv;
}

inline XmlManager& MgDbEnvironment::GetXmlManager()
{
    return m_xmlMan;
}

inline MgXmlSchemaResolver& MgDbEnvironment::GetXmlSchemaResolver()
{
    return m_schemaResolver;
}

INT32 MgDbEnvironment::getDBPageSize()
{
    return MG_DB_PAGE_SIZE;
}
        
INT32 MgDbEnvironment::getSessionDBPageSize()
{
    return MG_SESS_DB_PAGE_SIZE;
}

#endif
