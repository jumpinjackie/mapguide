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

#ifndef MGRESOURCECONTENTMANAGER_H_
#define MGRESOURCECONTENTMANAGER_H_

#include "ResourceDefinitionManager.h"

class MgResourceContentManager : public MgResourceDefinitionManager
{
/// Constructors/Destructor

public:

    explicit MgResourceContentManager(
        MgRepositoryManager& repositoryMan);
    virtual ~MgResourceContentManager();

private:

    // Unimplemented Constructors/Methods

    MgResourceContentManager();
    MgResourceContentManager(const MgResourceContentManager&);
    MgResourceContentManager& operator=(const MgResourceContentManager&);

/// Methods

public:

    // Repository Management APIs

    virtual MgByteReader* EnumerateRepositories();
    virtual void AddRepository(MgResourceInfo& resourceInfo,
        const string& document);

    // Resource Management APIs

    virtual void AddResource(MgResourceInfo& resourceInfo,
        const string& document);
    virtual void MoveResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite);
    virtual void CopyResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite);

protected:

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

    virtual void DeleteResourceData(MgResourceIdentifier* resource, XmlDocument& xmlDoc) {};

/// Data Members

private:

};

#endif
