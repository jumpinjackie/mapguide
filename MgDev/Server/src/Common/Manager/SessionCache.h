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

#ifndef MGSESSIONCACHE_H_
#define MGSESSIONCACHE_H_

#include "SessionInfo.h"

typedef std::map<STRING, MgSessionInfo*> MgSessionInfoMap;

class MG_SERVER_MANAGER_API MgSessionCache : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgSessionCache)

/// Constructors/Destructor

public:

    MgSessionCache();
    virtual ~MgSessionCache();

private:

    // Unimplemented copy constructor and assignment operator.

    MgSessionCache(const MgSessionCache&);
    MgSessionCache& operator=(const MgSessionCache&);

/// Methods

public:

    void CleanUpSessions(INT32 sessionTimeout,
        MgStringCollection* expiredSessions);

    void AddSession(CREFSTRING session, CREFSTRING user);
    void RemoveSession(CREFSTRING session);

    bool IsUserInSession(CREFSTRING user, CREFSTRING session) const;

    MgSessionInfoMap& GetSessionInfoMap();
    MgSessionInfo* GetSessionInfo(CREFSTRING session,
        bool strict = true) const;

protected:

    virtual void Dispose();

/// Data Members

private:

    MgSessionInfoMap m_sessionInfoMap;
};

/// Inline Methods

inline MgSessionInfoMap& MgSessionCache::GetSessionInfoMap()
{
    return m_sessionInfoMap;
}

#endif
