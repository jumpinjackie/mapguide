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

#ifndef MGSERVERRESOURCESERVICE_H_
#define MGSERVERRESOURCESERVICE_H_

#include "ServerResourceDllExport.h"

#include <set>

class MgSecurityCache;
class MgPermissionCache;
class MgLibraryRepository;
class MgSessionRepository;
class MgSiteRepository;
class MgRepositoryManager;
class MgSystemRepositoryManager;
class MgApplicationRepositoryManager;

class MG_SERVER_RESOURCE_API MgServerResourceService : public MgResourceService
{
    DECLARE_CLASSNAME(MgServerResourceService)

/// Constructors/Destructor

public:

    explicit MgServerResourceService();
    virtual ~MgServerResourceService();
    DECLARE_CREATE_SERVICE()

private:

/// Methods

public:

    virtual void OpenRepositories();
    virtual void CloseRepositories();

    // Repository Management APIs

    virtual MgByteReader* EnumerateRepositories(CREFSTRING repositoryType);
    virtual void CreateRepository(MgResourceIdentifier* resource,
        MgByteReader* content, MgByteReader* header);
    virtual void DeleteRepository(MgResourceIdentifier* resource);
    virtual void UpdateRepository(MgResourceIdentifier* resource,
        MgByteReader* content, MgByteReader* header);
    virtual MgByteReader* GetRepositoryContent(
        MgResourceIdentifier* resource);
    virtual MgByteReader* GetRepositoryHeader(
        MgResourceIdentifier* resource);
    virtual void ApplyResourcePackage(MgByteReader* packageStream);
    virtual void LoadResourcePackage(CREFSTRING packagePathname, bool logActivities);
    virtual void MakeResourcePackage(MgResourceIdentifier* resource,
        CREFSTRING packagePathname, CREFSTRING packageDescription,
        bool logActivities);

    // Resource Management APIs

    virtual bool ResourceExists(MgResourceIdentifier* resource);
    virtual MgByteReader* EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, INT32 properties,
        CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren);
    virtual void SetResource(MgResourceIdentifier* resource,
        MgByteReader* content, MgByteReader* header);
    virtual void DeleteResource(MgResourceIdentifier* resource);
    virtual void MoveResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite);
    virtual void MoveResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite, bool cascade);
    virtual void CopyResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite);
    virtual MgByteReader* GetResourceContent(
        MgResourceIdentifier* resource, CREFSTRING preProcessTags);
    virtual MgStringCollection* GetResourceContents(MgStringCollection* resources,
        MgStringCollection* preProcessTags);
    virtual MgByteReader* GetResourceHeader(
        MgResourceIdentifier* resource);
    virtual MgDateTime* GetResourceModifiedDate(
        MgResourceIdentifier* resource);
    virtual MgByteReader* EnumerateReferences(
        MgResourceIdentifier* resource);
    virtual MgSerializableCollection* EnumerateParentMapDefinitions(
        MgSerializableCollection* resources);
    virtual STRING EnumerateResourceDocuments(MgStringCollection* resources,
        CREFSTRING type, INT32 properties);
    virtual void ChangeResourceOwner(MgResourceIdentifier* resource,
        CREFSTRING owner, bool includeDescendants);
    virtual void InheritPermissionsFrom(MgResourceIdentifier* resource);

    // Resource Data Management APIs

    virtual MgByteReader* EnumerateResourceData(
        MgResourceIdentifier* resource);
    virtual void SetResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data);
    virtual void DeleteResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName);
    virtual void RenameResourceData(MgResourceIdentifier* resource,
        CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite);
    virtual MgByteReader* GetResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName, CREFSTRING preProcessTags);

    // User Management APIs

    MgByteReader* EnumerateUsers(CREFSTRING group, CREFSTRING role,
        bool includePassword, bool includeGroups);
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

    MgByteReader* EnumerateGroups(CREFSTRING user, CREFSTRING role);
    void AddGroup(CREFSTRING group, CREFSTRING description);
    void DeleteGroups(MgStringCollection* groups);
    void UpdateGroup(CREFSTRING group, CREFSTRING newGroup,
        CREFSTRING newDescription);
    void GrantRoleMembershipsToGroups(MgStringCollection* roles,
        MgStringCollection* groups);
    void RevokeRoleMembershipsFromGroups(MgStringCollection* roles,
        MgStringCollection* groups);

    // Role Management APIs

    MgStringCollection* EnumerateRoles(CREFSTRING user, CREFSTRING group);

    // Unmanaged Data APIs

    virtual MgByteReader* EnumerateUnmanagedData(CREFSTRING path, bool recursive, CREFSTRING type, CREFSTRING filter);

    // Site Security related methods

    MgSecurityCache* CreateSecurityCache();

    // Resource Permission related methods

    MgPermissionCache* CreatePermissionCache();
    virtual void CheckPermission(MgResourceIdentifier* resource,
        CREFSTRING permission);
    virtual bool HasPermission(MgResourceIdentifier* resource,
        CREFSTRING permission);

    // Other internal methods

    MgApplicationRepositoryManager* CreateApplicationRepositoryManager(
        MgResourceIdentifier* resource);

    virtual void PerformRepositoryCheckpoints(UINT32 flags = 0);

    virtual MgSerializableCollection* GetChangedResources();
    virtual void UpdateChangedResources(MgSerializableCollection* resources);

    void SetConnectionProperties(MgConnectionProperties* connProp);

protected:

    void UpdateChangedResources(const set<STRING>& resources);

/// Data Members

private:

    static INT32 sm_retryAttempts;
    static ACE_Time_Value sm_retryInterval;

    static MgSiteRepository*    sm_siteRepository;
    static MgSessionRepository* sm_sessionRepository;
    static MgLibraryRepository* sm_libraryRepository;
    static std::map<STRING, MgSessionRepository* > sm_sessionRepositories;
    static INT32 sm_sessionRepositoriesLimit;

    static ACE_Recursive_Thread_Mutex sm_mutex;
    static set<STRING> sm_changedResources;
    static bool sm_bSingleSessionRepository;
};

#endif
