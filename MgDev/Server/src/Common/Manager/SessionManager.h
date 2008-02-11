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

#ifndef MGSESSIONMANAGER_H_
#define MGSESSIONMANAGER_H_

#include "SessionCache.h"

class MG_SERVER_MANAGER_API MgSessionManager
{
    DECLARE_CLASSNAME(MgSessionManager)

/// Constructors/Destructor

public:

    MgSessionManager();
    virtual ~MgSessionManager();

private:

    // Unimplemented copy constructor and assignment operator.

    MgSessionManager(const MgSessionManager&);
    MgSessionManager& operator=(const MgSessionManager&);

/// Methods

public:

    static void CleanUpSessions(INT32 sessionTimeout,
        MgStringCollection* expiredSessions);

    static void AddSession(CREFSTRING session, CREFSTRING user);
    static void RemoveSession(CREFSTRING session);

    static STRING GetUserName(CREFSTRING session);
    static STRING UpdateLastAccessedTime(CREFSTRING session);
    static void UpdateCurrentOperationInfo(const MgConnection& connection);

/// Data Members

private:

    static ACE_Recursive_Thread_Mutex sm_mutex;
    static Ptr<MgSessionCache> sm_sessionCache;
};

#endif
