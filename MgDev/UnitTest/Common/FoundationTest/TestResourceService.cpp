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

#include "TestResourceService.h"

MgTestResourceService::MgTestResourceService()
{
}

MgTestResourceService::~MgTestResourceService()
{
}


MgByteReader* MgTestResourceService::EnumerateRepositories(CREFSTRING repositoryType)
{
    printf("Hello there\n");
    return NULL;
}

void MgTestResourceService::CreateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header)
{
}

void MgTestResourceService::DeleteRepository(MgResourceIdentifier* resource)
{
}

void MgTestResourceService::UpdateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header)
{
}

void MgTestResourceService::ApplyResourcePackage(MgByteReader* resourcePackage)
{
}

MgByteReader* MgTestResourceService::EnumerateResources(MgResourceIdentifier* resource, INT32 depth, CREFSTRING type)
{
    return NULL;
}

void MgTestResourceService::SetResource(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header)
{
}

void MgTestResourceService::DeleteResource(MgResourceIdentifier* resource)
{
}

void MgTestResourceService::MoveResource(MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource, bool overwrite)
{
}

void MgTestResourceService::CopyResource(MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource, bool overwrite)
{
}

MgByteReader* MgTestResourceService::GetResourceContent(MgResourceIdentifier* resource)
{
    return NULL;
}

MgByteReader* MgTestResourceService::GetResourceHeader(MgResourceIdentifier* resource)
{
    return NULL;
}

void MgTestResourceService::ChangeResourceOwner(MgResourceIdentifier* resource, CREFSTRING owner, bool includeDescendants)
{
}

void MgTestResourceService::InheritPermissionsFrom(MgResourceIdentifier* resource)
{
}

void MgTestResourceService::SetResourceData(MgResourceIdentifier* resource, CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data)
{
}

void MgTestResourceService::DeleteResourceData(MgResourceIdentifier* resource, CREFSTRING dataName)
{
}

void MgTestResourceService::RenameResourceData(MgResourceIdentifier* resource, CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite)
{
}


MgByteReader* MgTestResourceService::GetResourceData(MgResourceIdentifier* resource, CREFSTRING dataName)
{
    return NULL;
}

MgByteReader* MgTestResourceService::EnumerateResourceData(MgResourceIdentifier* resource)
{
    return NULL;
}

MgByteReader* MgTestResourceService::GetRepositoryContent(MgResourceIdentifier* resource)
{
    return NULL;
}

MgByteReader* MgTestResourceService::GetRepositoryHeader(MgResourceIdentifier* resource)
{
    return NULL;
}

MgByteReader* MgTestResourceService::EnumerateReferences(MgResourceIdentifier* resource)
{
    return NULL;
}

MgByteReader* MgTestResourceService::EnumerateResources(MgResourceIdentifier* resource, INT32 depth, CREFSTRING type, INT32 properties, CREFSTRING fromDate, CREFSTRING toDate)
{
    return NULL;
}

bool MgTestResourceService::ResourceExists(MgResourceIdentifier* resource)
{
    return false;
}

MgByteReader* MgTestResourceService::GetResourceContent(MgResourceIdentifier* resource, CREFSTRING preProcessTags)
{
    return NULL;
}

MgByteReader* MgTestResourceService::GetResourceData(MgResourceIdentifier* resource, CREFSTRING dataName, CREFSTRING preProcessTags)
{
    return NULL;
}

MgDateTime* MgTestResourceService::GetResourceModifiedDate(MgResourceIdentifier* resource)
{
    return NULL;
}

MgSerializableCollection* MgTestResourceService::EnumerateParentMapDefinitions(MgSerializableCollection* resources)
{
    return NULL;
}

void MgTestResourceService::Dispose()
{
    delete this;
}

IMPLEMENT_CREATE_SERVICE(MgTestResourceService)

