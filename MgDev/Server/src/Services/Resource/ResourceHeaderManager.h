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

#ifndef MGRESOURCEHEADERMANAGER_H_
#define MGRESOURCEHEADERMANAGER_H_

#include "ResourceDefinitionManager.h"
#include "LibraryRepositoryManager.h"
#include "PermissionManager.h"
#include "ResourceHeader.h"
#include "ResourcePackageMaker.h"

typedef std::map<STRING, STRING> MgResourcePermissionMap;

class MgResourceHeaderManager : public MgResourceDefinitionManager
{
/// Constructors/Destructor

public:

    explicit MgResourceHeaderManager(
        MgLibraryRepositoryManager& repositoryMan);
    virtual ~MgResourceHeaderManager();

private:

    // Unimplemented Constructors/Methods

    MgResourceHeaderManager();
    MgResourceHeaderManager(const MgResourceHeaderManager&);
    MgResourceHeaderManager& operator=(const MgResourceHeaderManager&);

/// Methods

public:

    // Repository Management APIs

    virtual void AddRepository(MgResourceInfo& resourceInfo,
        const string& document);

    // Resource Management APIs

    MgByteReader* EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, INT32 properties,
        CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren);
    virtual void AddResource(MgResourceInfo& resourceInfo,
        const string& document);
    virtual void MoveResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite);
    virtual void CopyResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite);
    void AddParentResources(MgResourceInfo& resourceInfo,
        const string& document);
    MgDateTime* GetResourceModifiedDate(MgResourceIdentifier* resource);
    STRING EnumerateResourceDocuments(MgStringCollection* resources,
        CREFSTRING type, INT32 properties);
    void ChangeResourceOwner(MgResourceIdentifier* resource,
        CREFSTRING owner, bool includeDescendants);
    void InheritPermissionsFrom(MgResourceIdentifier* resource);

    // Resource Permission related methods

    MgPermissionCache* CreatePermissionCache();
    virtual bool CheckPermission(MgResourceIdentifier& resource,
        CREFSTRING permission, bool strict = true);
    virtual bool CheckParentPermission(MgResourceIdentifier& resource,
        CREFSTRING permission, bool strict = true);

protected:

    bool CheckOwnership(const string& user, XmlDocument& xmlDoc,
        string& owner, bool strict = true);
    bool CheckOwnership(XmlDocument& xmlDoc, bool strict = true);
    bool CheckPermission(CREFSTRING resource, CREFSTRING permission);

    virtual void ValidateDocument(MgResourceIdentifier& resource,
        CREFSTRING rootName, CREFSTRING schemaName);

    virtual XmlDocument GetDocument(MgResourceIdentifier& resource,
        CREFSTRING permission = MgResourcePermission::ReadOnly);
    virtual void PutDocument(MgResourceIdentifier& resource,
        XmlDocument& xmlDoc, XmlUpdateContext& updateContext);
    virtual void TransformDocument(const string& xmlStr,
        XmlDocument& xmlDoc);
    virtual void UpdateDocument(MgResourceIdentifier& resource,
        XmlDocument& xmlDoc, XmlUpdateContext& updateContext,
        UINT32 opId = MgResourceService::opIdSetResource);
    virtual void DeleteDocument(MgResourceIdentifier& resource,
        XmlDocument& xmlDoc, XmlUpdateContext& updateContext);

    void PackageResource(MgResourceIdentifier& resource,
        MgResourcePackageMaker& packageMaker);

private:

    void SetUpPermissionQuery(
        XmlQueryContext& queryContext,
        XmlQueryExpression& selectInheritedFlags,
        XmlQueryExpression& selectUserNames,
        XmlQueryExpression& selectUserPermissions,
        XmlQueryExpression& selectGroupNames,
        XmlQueryExpression& selectGroupPermissions);

    const string& GetDefaultHeader(
        const MgResourceIdentifier& resource);
    MgPermissionInfo* GetDefaultPermissionInfo(CREFSTRING resource) const;

    MgPermissionInfo* CreatePermissionInfo(
        XmlDocument& xmlDoc, XmlQueryContext& queryContext,
        XmlQueryExpression& selectInheritedFlags,
        XmlQueryExpression& selectUserNames,
        XmlQueryExpression& selectUserPermissions,
        XmlQueryExpression& selectGroupNames,
        XmlQueryExpression& selectGroupPermissions);
    const MgPermissionInfo* GetPermissionInfo(CREFSTRING resource,
        bool& local) const;

    INT32 GetParentResources(MgResourceIdentifier& resource,
        XmlResults& docResults, bool inclusive);
    void GetParentResources(MgResourceIdentifier& resource,
        MgResourceHeaderMap& resourceHeaderMap, INT32 properties,
        bool inclusive);

    void UpdatePermissionMaps(CREFSTRING resource, CREFSTRING permission,
        MgPermissionInfo::PermissionStatus status);

    MgPermissionInfo* UpdatePermissionCache(
        XmlDocument& xmlDoc, XmlQueryContext& queryContext,
        XmlQueryExpression& selectInheritedFlags,
        XmlQueryExpression& selectUserNames,
        XmlQueryExpression& selectUserPermissions,
        XmlQueryExpression& selectGroupNames,
        XmlQueryExpression& selectGroupPermissions,
        MgPermissionCache* permissionCache);
    MgPermissionInfo* UpdatePermissionCache(
        XmlDocument& xmlDoc, MgPermissionCache* permissionCache);
    void UpdatePermissionCache(UINT32 opId, CREFSTRING resource,
        CREFSTRING permission, XmlDocument& xmlDoc, bool permissionChanged);
    void UpdatePermissionCache(XmlResults& docResults, INT32 maxSize,
        MgPermissionCache* permissionCache);
    void UpdatePermissionCache(
        const MgPermissionInfoMap& permissionInfoMap);
    void UpdatePermissionCache();

    void UpdateResourceModifiedDates(const set<STRING>& resources);

    void BeginWriteResourceList(string& list);
    void WriteResourceList(string& list, const string& resourcePathname,
        const MgResourceHeaderMap* resourceHeaderMap, MgResourceHeader& resourceHeader,
        INT32 properties, INT32 requiredDepth, INT32 maxDepth);
    void EndWriteResourceList(string& list);

/// Data Members

private:

    friend class MgLibraryRepositoryManager;

    auto_ptr<MgPermissionManager> m_permissionMan;
    Ptr<MgPermissionCache> m_permissionCache;

    string m_defaultRootHeader;
    string m_defaultFolderHeader;
    string m_defaultDocumentHeader;

    MgResourcePermissionMap m_accessibleResources;
    MgResourcePermissionMap m_inaccessibleResources;
};

/// Inline Methods

#endif
