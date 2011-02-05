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

#include "PlatformBase.h"

class MgTestResourceService :
    public MgResourceService
{
public:
    MgTestResourceService();

public:
    virtual ~MgTestResourceService();
    MgByteReader* EnumerateRepositories(CREFSTRING repositoryType);
    void CreateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header);
    void DeleteRepository(MgResourceIdentifier* resource);
    void UpdateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header);
    void ApplyResourcePackage(MgByteReader* resourcePackage);
    MgByteReader* EnumerateResources(MgResourceIdentifier* resource, INT32 depth, CREFSTRING type);
    void SetResource(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header);
    void DeleteResource(MgResourceIdentifier* resource);
    void MoveResource(MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource, bool overwrite);
    void CopyResource(MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource, bool overwrite);
    MgByteReader* GetResourceContent(MgResourceIdentifier* resource);
    virtual MgByteReader* GetResourceHeader(MgResourceIdentifier* resource);
    virtual void ChangeResourceOwner(MgResourceIdentifier* resource, CREFSTRING owner, bool includeDescendants);
    virtual void InheritPermissionsFrom(MgResourceIdentifier* resource);
    virtual void SetResourceData(MgResourceIdentifier* resource, CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data);
    virtual void DeleteResourceData(MgResourceIdentifier* resource, CREFSTRING dataName);
    virtual void RenameResourceData(MgResourceIdentifier* resource, CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite);
    MgByteReader* GetResourceData(MgResourceIdentifier* resource, CREFSTRING dataName);
    virtual MgByteReader* EnumerateResourceData(MgResourceIdentifier* resource);
    virtual MgByteReader* GetRepositoryContent(MgResourceIdentifier* resource);
    virtual MgByteReader* GetRepositoryHeader(MgResourceIdentifier* resource);
    virtual MgByteReader* EnumerateReferences(MgResourceIdentifier* resource);
    virtual MgByteReader* EnumerateResources(MgResourceIdentifier* resource, INT32 depth, CREFSTRING type, INT32 properties, CREFSTRING fromDate, CREFSTRING toDate);
    virtual MgByteReader* GetResourceContent(MgResourceIdentifier* resource, CREFSTRING preProcessTags);
    virtual MgByteReader* GetResourceData(MgResourceIdentifier* resource, CREFSTRING dataName, CREFSTRING preProcessTags);
    virtual MgDateTime* GetResourceModifiedDate(MgResourceIdentifier* resource);
    virtual MgSerializableCollection* EnumerateParentMapDefinitions(MgSerializableCollection* resources);
    bool ResourceExists(MgResourceIdentifier* resource);

    DECLARE_CREATE_SERVICE()

protected:
    virtual void Dispose();
};

