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

#ifndef MGSECURITYMANAGER_H_
#define MGSECURITYMANAGER_H_

#include "SecurityCache.h"

class MG_SERVER_MANAGER_API MgSecurityManager
{
    DECLARE_CLASSNAME(MgSecurityManager)

/// Constructors/Destructor

public:

    MgSecurityManager();
    virtual ~MgSecurityManager();

private:

    // Unimplemented copy constructor and assignment operator.

    MgSecurityManager(const MgSecurityManager&);
    MgSecurityManager& operator=(const MgSecurityManager&);

/// Methods

public:

    static void RefreshSecurityCache(MgSecurityCache* securityCache);
    static void SetUser(CREFSTRING user, CREFSTRING password);
    static void SetGroup(CREFSTRING group);

    static MgUserInformation* CreateSystemCredentials();
    static void Authenticate(MgUserInformation* userInformation);
    static MgStringCollection* Authenticate(MgUserInformation* userInformation,
        MgStringCollection* requiredRoles, bool returnAssignedRoles);

    static bool IsSystemUser(CREFSTRING user);

    bool IsUserAnAdministrator(CREFSTRING user) const;
    bool IsUserAnAuthor(CREFSTRING user) const;

    bool IsUserInGroup(CREFSTRING user, CREFSTRING group) const;
    bool IsUserInGroups(CREFSTRING user, MgStringCollection* groups) const;

    bool IsUserInRole(CREFSTRING user, CREFSTRING role) const;
    bool IsUserInRoles(CREFSTRING user, MgStringCollection* roles) const;

    STRING GetUserName(CREFSTRING session) const;
    STRING GetPassword(CREFSTRING user) const;

    bool FindUser(CREFSTRING user) const;
    bool FindGroup(CREFSTRING group) const;

/// Data Members

private:

    static ACE_Recursive_Thread_Mutex sm_mutex;
    static Ptr<MgSecurityCache> sm_securityCache;

    Ptr<MgSecurityCache> m_securityCache;
};

/// Inline Methods

#endif
