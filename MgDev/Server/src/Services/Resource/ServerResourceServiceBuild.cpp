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

// ignore warnings about exception definitions
#pragma warning(disable: 4290)

#ifdef _WIN32
#include "Dll.cpp"
#endif
#include "ApplicationRepository.cpp"
#include "ApplicationRepositoryManager.cpp"
#include "ApplicationResourceContentManager.cpp"
#include "ByteSourceDwfInputStreamImpl.cpp"
#include "Database.cpp"
#include "DataBindingInfo.cpp"
#include "DbEnvironment.cpp"
#include "LibraryRepository.cpp"
#include "LibraryRepositoryManager.cpp"
#include "LibraryResourceContentManager.cpp"
#include "OpApplyResourcePackage.cpp"
#include "OpChangeResourceOwner.cpp"
#include "OpCopyResource.cpp"
#include "OpCreateRepository.cpp"
#include "OpDeleteRepository.cpp"
#include "OpDeleteResource.cpp"
#include "OpDeleteResourceData.cpp"
#include "OpEnumerateParentMapDefinitions.cpp"
#include "OpEnumerateRepositories.cpp"
#include "OpEnumerateResourceData.cpp"
#include "OpEnumerateResourceDocuments.cpp"
#include "OpEnumerateResourceReferences.cpp"
#include "OpEnumerateResources.cpp"
#include "OperationInfo.cpp"
#include "OperationParameter.cpp"
#include "OpGetRepositoryContent.cpp"
#include "OpGetRepositoryHeader.cpp"
#include "OpGetResourceContent.cpp"
#include "OpGetResourceContents.cpp"
#include "OpGetResourceData.cpp"
#include "OpGetResourceHeader.cpp"
#include "OpGetResourceModifiedDate.cpp"
#include "OpInheritPermissionsFrom.cpp"
#include "OpMoveResource.cpp"
#include "OpRenameResourceData.cpp"
#include "OpResourceExists.cpp"
#include "OpSetResource.cpp"
#include "OpSetResourceData.cpp"
#include "OpUpdateRepository.cpp"
#include "OpEnumerateUnmanagedData.cpp"
#include "Repository.cpp"
#include "RepositoryManager.cpp"
#include "ResourceContainer.cpp"
#include "ResourceContentManager.cpp"
#include "ResourceDatabase.cpp"
#include "ResourceDataFileManager.cpp"
#include "ResourceDataManager.cpp"
#include "ResourceDataStreamManager.cpp"
#include "ResourceDefinitionManager.cpp"
#include "ResourceHeader.cpp"
#include "ResourceHeaderManager.cpp"
#include "ResourceInfo.cpp"
#include "ResourceManager.cpp"
#include "ResourceOperation.cpp"
#include "ResourceOperationFactory.cpp"
#include "ResourcePackageHandler.cpp"
#include "ResourcePackageLoader.cpp"
#include "ResourcePackageMaker.cpp"
#include "ResourcePackageManifestHandler.cpp"
#include "ResourcePackageManifestParser.cpp"
#include "ResourcePackageManifestSerializer.cpp"
#include "ResourceServiceHandler.cpp"
#include "ServerResourceService.cpp"
#include "SessionRepository.cpp"
#include "SessionRepositoryManager.cpp"
#include "SessionResourceContentManager.cpp"
#include "SiteRepository.cpp"
#include "SiteRepositoryManager.cpp"
#include "SiteResourceContentManager.cpp"
#include "SystemRepository.cpp"
#include "SystemRepositoryManager.cpp"
#include "SystemResourceContentManager.cpp"
#include "TagInfo.cpp"
#include "TagManager.cpp"
#include "XmlSchemaResolver.cpp"
#include "ZipFileHandler.cpp"
#include "ZipFileReader.cpp"
#include "ZipFileWriter.cpp"
