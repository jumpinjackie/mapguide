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

#ifndef MGSITERESOURCECONTENTMANAGER_H_
#define MGSITERESOURCECONTENTMANAGER_H_

#include "SystemResourceContentManager.h"
#include "SecurityCache.h"

class MgSiteRepositoryManager;

class MgSiteResourceContentManager : public MgSystemResourceContentManager
{
/// Constructors/Destructor

public:

    explicit MgSiteResourceContentManager(
        MgSiteRepositoryManager& repositoryMan);
    virtual ~MgSiteResourceContentManager();

private:

    // Unimplemented Constructors/Methods

    MgSiteResourceContentManager();
    MgSiteResourceContentManager(const MgSiteResourceContentManager&);
    MgSiteResourceContentManager& operator=(const MgSiteResourceContentManager&);

/// Methods

public:

    // User Management APIs

    MgByteReader* EnumerateAllUsers(bool includePassword);
    MgByteReader* EnumerateUsersInGroup(CREFSTRING group, bool includePassword);
    MgByteReader* EnumerateUsersInRole(CREFSTRING role, bool includePassword,
        bool includeGroups);
    void AddUser(CREFSTRING userId, CREFSTRING username,
        CREFSTRING password, CREFSTRING description);
    void DeleteUsers(MgStringCollection* users);
    void UpdateUser(CREFSTRING userId, CREFSTRING newUserId,
        CREFSTRING newUsername, CREFSTRING newPassword,
        CREFSTRING newDescription);
    void GrantGroupMembershipsToUsers(MgStringCollection* groups,
        MgStringCollection* users);
    void GrantRoleMembershipsToUsers(MgStringCollection* roles,
        MgStringCollection* users);
    void RevokeGroupMembershipsFromUsers(MgStringCollection* groups,
        MgStringCollection* users);
    void RevokeRoleMembershipsFromUsers(MgStringCollection* roles,
        MgStringCollection* users);

    // Group Management APIs

    MgByteReader* EnumerateAllGroups();
    MgByteReader* EnumerateGroupsByUser(CREFSTRING user);
    MgByteReader* EnumerateGroupsByRole(CREFSTRING role);
    void AddGroup(CREFSTRING group, CREFSTRING description);
    void DeleteGroups(MgStringCollection* groups);
    void UpdateGroup(CREFSTRING group, CREFSTRING newGroup,
        CREFSTRING newDescription);
    void GrantRoleMembershipsToGroups(MgStringCollection* roles,
        MgStringCollection* groups);
    void RevokeRoleMembershipsFromGroups(MgStringCollection* roles,
        MgStringCollection* groups);

    // Role Management APIs

    MgStringCollection* EnumerateAllRoles();
    MgStringCollection* EnumerateRolesOfUser(CREFSTRING user);
    MgStringCollection* EnumerateRolesOfGroup(CREFSTRING group);

    // Site Security APIs

    MgSecurityCache* CreateSecurityCache();

protected:

    // Resource Permission related methods

    virtual bool CheckPermission(MgResourceIdentifier& resource,
        CREFSTRING permission, bool strict = true);
    virtual bool CheckParentPermission(MgResourceIdentifier& resource,
        CREFSTRING permission, bool strict = true);

    // Writer Methods

    void WriteUserList(const XmlDocument& userDoc, bool includePassword,
        string& list);
    void WriteGroupList(const XmlDocument* groupDoc, string& list);

private :

    // Utility Methods

    void RemoveUserFromGroup(MgResourceIdentifier * const groupRes,
        STRING userId);
    MgStringCollection* RemoveUserFromAllGroups(STRING userId);
    void RemoveUserFromRole(MgResourceIdentifier * const roleRes,
        STRING userId);
    MgStringCollection* RemoveUserFromAllRoles(STRING userId);
    void RemoveGroupFromRole(MgResourceIdentifier * const roleRes,
        STRING groupId);
    MgStringCollection* RemoveGroupFromAllRoles(STRING groupId);

/// Data Members

private:

};

#endif
