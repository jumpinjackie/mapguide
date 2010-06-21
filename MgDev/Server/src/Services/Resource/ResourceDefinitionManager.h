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

#ifndef MGRESOURCEDEFINITIONMANAGER_H_
#define MGRESOURCEDEFINITIONMANAGER_H_

#include "ResourceManager.h"
#include "ResourceContainer.h"
#include "ResourceInfo.h"

class MgApplicationRepositoryManager;
class MgDataBindingInfo;

class MgResourceDefinitionManager : public MgResourceManager
{
/// Constructors/Destructor

public:

    explicit MgResourceDefinitionManager(
        MgRepositoryManager& repositoryManager,
        MgResourceContainer& resourceContainer);
    virtual ~MgResourceDefinitionManager();

private:

    // Unimplemented Constructors/Methods

    MgResourceDefinitionManager();
    MgResourceDefinitionManager(const MgResourceDefinitionManager&);
    MgResourceDefinitionManager& operator=(const MgResourceDefinitionManager&);

/// Methods

public:

    bool IsResourceContentManager();

    // Repository Management APIs

    virtual void AddRepository(MgResourceInfo& resourceInfo,
        const string& document) = 0;
    void DeleteRepository(MgResourceIdentifier* resource);
    void UpdateRepository(MgResourceInfo& resourceInfo,
        const string& document);
    MgByteReader* GetRepository(MgResourceIdentifier* resource);

    // Resource Management APIs

    bool ResourceExists(const string& mbResourcePathname);
    bool ResourceExists(CREFSTRING wcResourcePathname);
    virtual void AddResource(MgResourceInfo& resourceInfo,
        const string& document) = 0;
    void DeleteResource(MgResourceIdentifier* resource,
        bool strict = false);
    void UpdateResource(MgResourceInfo& resourceInfo,
        const string& document);
    virtual void MoveResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite) = 0;
    virtual void CopyResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite) = 0;
    MgByteReader* GetResource(MgResourceIdentifier* resource,
        const MgDataBindingInfo* dataBindingInfo = NULL);

    // Helper Methods

    STRING FormatMethodName(CREFSTRING methodName);

    // Resource Permission related methods

    virtual bool CheckPermission(MgResourceIdentifier& resource,
        CREFSTRING permission, bool strict = true) = 0;

protected:

    virtual void ValidateDocument(MgResourceIdentifier& resource,
        CREFSTRING rootName, CREFSTRING schemaName) = 0;

    virtual XmlDocument GetDocument(MgResourceIdentifier& resource,
        CREFSTRING permission = MgResourcePermission::ReadOnly) = 0;
    virtual void PutDocument(MgResourceIdentifier& resource,
        XmlDocument& xmlDoc, XmlUpdateContext& updateContext) = 0;
    virtual void TransformDocument(const string& xmlStr,
        XmlDocument& xmlDoc) = 0;
    virtual void UpdateDocument(MgResourceIdentifier& resource,
        XmlDocument& xmlDoc, XmlUpdateContext& updateContext,
        UINT32 opId = MgResourceService::opIdSetResource) = 0;
    virtual void DeleteDocument(MgResourceIdentifier& resource,
        XmlDocument& xmlDoc, XmlUpdateContext& updateContext) = 0;

    bool GetDocument(const string& resourcePathname,
        XmlDocument& xmlDoc, bool strict = false);
    bool GetDocument(MgResourceIdentifier& resource,
        XmlDocument& xmlDoc, bool strict = false);
    void PutDocument(XmlDocument& xmlDoc,
        XmlUpdateContext& updateContext);
    void UpdateDocument(XmlDocument& xmlDoc,
        XmlUpdateContext& updateContext);
    void DeleteDocument(XmlDocument& xmlDoc,
        XmlUpdateContext& updateContext);

    virtual bool CheckParentPermission(MgResourceIdentifier& resource,
        CREFSTRING permission, bool strict = true) = 0;

private:

    virtual void ValidateDocument(XmlDocument& xmlDoc);

/// Data Members

protected:

    XmlContainer& m_container;

private:

    friend class MgResourceHeaderManager;
    friend class MgResourceContentManager;
};

#endif
