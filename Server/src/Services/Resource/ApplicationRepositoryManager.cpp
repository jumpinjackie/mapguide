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

#include "ResourceServiceDefs.h"
#include "ApplicationRepositoryManager.h"
#include "ApplicationResourceContentManager.h"
#include "FdoConnectionManager.h"
#include "ResourceHeaderManager.h"
#include "ResourceDataStreamManager.h"
#include "ResourceDataFileManager.h"
#include "TagManager.h"
#include "ResourceInfo.h"
#include "CryptographyUtil.h"
#include "LogManager.h"
#include "CacheManager.h"
#include "ServiceManager.h"
#include "ServerResourceService.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgApplicationRepositoryManager::MgApplicationRepositoryManager(
    MgApplicationRepository& repository) :
    MgRepositoryManager(repository),
    m_dataStreamMan(NULL),
    m_dataFileMan(NULL)
{
    MG_RESOURCE_SERVICE_TRY()

    m_securityMan.reset(new MgSecurityManager());

    // If the current user name and/or password are not given, then look them
    // up based on the current session ID.

    m_currUserInfo = new MgUserInformation;
    Ptr<MgUserInformation> currUserInfo = MgUserInformation::GetCurrentUserInfo();
    assert(NULL != currUserInfo);
    STRING sessionId = currUserInfo->GetMgSessionId();
    STRING userName = currUserInfo->GetUserName();
    STRING password = currUserInfo->GetPassword();

    if (!sessionId.empty())
    {
        m_currUserInfo->SetMgSessionId(sessionId);

        if (userName.empty())
        {
            userName = m_securityMan->GetUserName(sessionId);
            password = m_securityMan->GetPassword(userName);
        }
    }

    if (userName.empty())
    {
        MG_LOG_AUTHENTICATION_ENTRY(MgResources::UnauthorizedAccess.c_str());

        throw new MgUnauthorizedAccessException(
            L"MgApplicationRepositoryManager.MgApplicationRepositoryManager",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_currUserInfo->SetMgUsernamePassword(userName, password);

    m_currUserIsAdmin = m_securityMan->IsUserAnAdministrator(userName);
    m_currUserIsAuthor = m_securityMan->IsUserAnAuthor(userName);

    m_dataStreamMan = new MgResourceDataStreamManager(*this);
    m_dataFileMan = new MgResourceDataFileManager(*this);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.MgApplicationRepositoryManager")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgApplicationRepositoryManager::~MgApplicationRepositoryManager()
{
    delete m_dataStreamMan;
    delete m_dataFileMan;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates an application repository manager.
///
MgApplicationRepositoryManager* MgApplicationRepositoryManager::Create(
    MgResourceIdentifier* resource)
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    ACE_ASSERT(NULL != serviceManager);
    Ptr<MgService> service = serviceManager->RequestService(MgServiceType::ResourceService);
    MgServerResourceService* resourceService = dynamic_cast<MgServerResourceService*>(service.p);

    if (NULL == resourceService)
    {
        throw new MgServiceNotAvailableException(
            L"MgApplicationRepositoryManager.Create",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return resourceService->CreateApplicationRepositoryManager(resource);
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the application resource content manager.
///
MgApplicationResourceContentManager* MgApplicationRepositoryManager::GetApplicationResourceContentManager()
{
    MgApplicationResourceContentManager* appResourceContentMan =
        dynamic_cast<MgApplicationResourceContentManager*>(
            GetResourceContentManager());

    if (NULL == appResourceContentMan)
    {
        throw new MgInvalidCastException(
            L"MgApplicationResourceContentManager.GetApplicationResourceContentManager",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return appResourceContentMan;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Notify the applicable caches of the changed resource.
///
void MgApplicationRepositoryManager::NotifyResourceChanged(
    MgResourceIdentifier& resource)
{
    MgCacheManager* cacheManager = MgCacheManager::GetInstance();

    if (cacheManager->IsResourceChangeNotificationNeeded(&resource))
    {
        // Request the cache manager to release the lock on the resource/data
        // file that may be currently opened by the FDO connection manager.
        cacheManager->NotifyResourceChanged(&resource);

        // Insert the specified resource into the changed resource set.
        m_changedResources.insert(resource.ToString());
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates an existing repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgApplicationRepositoryManager::UpdateRepository(
    MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (!resource->IsRoot())
    {
        throw new MgInvalidResourceTypeException(
            L"MgApplicationRepositoryManager.UpdateRepository",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Initialize resource information.

    MgResourceInfo resourceInfo(*resource, m_currUserInfo, m_accessedTime);

    // Update the header document.

    string headerDoc;

    if (NULL != header)
    {
        header->ToStringUtf8(headerDoc);
    }

    if (headerDoc.empty())
    {
        UpdateDateModifiedResourceSet(resource->ToString());
    }
    else
    {
        MgResourceHeaderManager* resourceHeaderMan = GetResourceHeaderManager();

        if (NULL == resourceHeaderMan)
        {
            throw new MgNullReferenceException(
                L"MgApplicationRepositoryManager.UpdateRepository",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            resourceHeaderMan->UpdateRepository(resourceInfo, headerDoc);
        }
    }

    // Update the content document.

    string contentDoc;

    if (NULL != content)
    {
        content->ToStringUtf8(contentDoc);
    }

    if (!contentDoc.empty())
    {
        MgResourceContentManager* resourceContentMan = GetResourceContentManager();

        resourceContentMan->UpdateRepository(resourceInfo, contentDoc);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.UpdateRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the content of the specified repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgApplicationRepositoryManager::GetRepositoryContent(
    MgResourceIdentifier* resource)
{
    assert(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (!resource->IsRoot())
    {
        throw new MgInvalidResourceTypeException(
            L"MgApplicationRepositoryManager.GetRepositoryContent",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Perform the operation.

    MgResourceContentManager* resourceContentMan = GetResourceContentManager();

    byteReader = resourceContentMan->GetRepository(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.GetRepositoryContent")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the header associated with the specified repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgApplicationRepositoryManager::GetRepositoryHeader(
    MgResourceIdentifier* resource)
{
    throw new MgNotImplementedException(
        L"MgApplicationRepositoryManager.GetRepositoryHeader",
        __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL; // to make some compiler happy
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates the resources in the opened resource repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgApplicationRepositoryManager::EnumerateResources(
    MgResourceIdentifier* resource, INT32 depth, CREFSTRING type,
    INT32 properties, CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren)
{
    throw new MgNotImplementedException(
        L"MgApplicationRepositoryManager.EnumerateResources",
        __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL; // to make some compiler happy
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the content of the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgApplicationRepositoryManager::GetResourceContent(
    MgResourceIdentifier* resource, CREFSTRING preProcessTags)
{
    assert(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (resource->IsFolder())
    {
        throw new MgInvalidResourceTypeException(
            L"MgApplicationRepositoryManager.GetResourceContent",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Perform the operation.

    MgResourceContentManager* resourceContentMan = GetResourceContentManager();

    if (preProcessTags.empty())
    {
        byteReader = resourceContentMan->GetResource(resource);
    }
    else if (MgResourcePreProcessingType::Substitution == preProcessTags)
    {
        MgDataBindingInfo dataBindingInfo;

        dataBindingInfo.SetPreProcessTags(preProcessTags);
        dataBindingInfo.SetResourceDataFilePath(
            m_dataFileMan->GetResourceDataFilePath());

        dataBindingInfo.SetLoginUsername(m_currUserInfo->GetUserName());
        dataBindingInfo.SetLoginPassword(m_currUserInfo->GetPassword());

        // turn on substitute unmanaged data mappings
        dataBindingInfo.SetSubstituteUnmanagedDataMappings(
            resource->IsResourceTypeOf(MgResourceType::FeatureSource));

        byteReader = resourceContentMan->GetResource(resource,
            &dataBindingInfo);
    }
    else
    {
        throw new MgInvalidResourcePreProcessingTypeException(
            L"MgApplicationRepositoryManager.GetResourceContent",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.GetResourceContent")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the header associated with the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgApplicationRepositoryManager::GetResourceHeader(
    MgResourceIdentifier* resource)
{
    throw new MgNotImplementedException(
        L"MgApplicationRepositoryManager.GetResourceHeader",
        __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL; // to make some compiler happy
}

///----------------------------------------------------------------------------
/// <summary>
/// Renames a resource and/or moves it from one location to another.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgApplicationRepositoryManager::MoveResource(
    MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource,
    bool overwrite, bool cascade)
{
    assert(NULL != sourceResource && NULL != destResource) ;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (sourceResource->IsRoot())
    {
        // Source is root
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(sourceResource->ToString());

        throw new MgInvalidArgumentException(L"MgApplicationRepositoryManager.MoveResource",
            __LINE__, __WFILE__, &arguments, L"MgInvalidResourceCannotBeRoot", NULL);
    }

    if (destResource->IsRoot())
    {
        // Destination is root
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(destResource->ToString());

        throw new MgInvalidArgumentException(L"MgApplicationRepositoryManager.MoveResource",
            __LINE__, __WFILE__, &arguments, L"MgInvalidResourceCannotBeRoot", NULL);
    }

    if (sourceResource->GetRootPath() != destResource->GetRootPath())
    {
        // Root paths are different
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(sourceResource->ToString());
        arguments.Add(L"2");
        arguments.Add(destResource->ToString());

        throw new MgInvalidArgumentException(L"MgApplicationRepositoryManager.MoveResource",
            __LINE__, __WFILE__, &arguments, L"MgResourceRootPathsDifferent", NULL);
    }

    if (sourceResource->GetResourceType() != destResource->GetResourceType())
    {
        // Resource types are different
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(sourceResource->ToString());
        arguments.Add(L"2");
        arguments.Add(destResource->ToString());

        throw new MgInvalidArgumentException(L"MgApplicationRepositoryManager.MoveResource",
            __LINE__, __WFILE__, &arguments, L"MgResourceTypesDifferent", NULL);
    }

    if (sourceResource->ToString() == destResource->ToString())
    {
        // Resources cannot be the same.
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(sourceResource->ToString());
        arguments.Add(L"2");
        arguments.Add(destResource->ToString());

        throw new MgInvalidArgumentException(L"MgApplicationRepositoryManager.MoveResource",
            __LINE__, __WFILE__, &arguments, L"MgResourcesIdentical", NULL);
    }

    // If cascade update referencing resources, get the references and check permission.
    // An MgUnauthorizedAccessException will throw if current user doesn't have write permission to any referencing resource.
    Ptr<MgStringCollection> referencingIds = NULL;
    if(cascade)
    {
        MgApplicationResourceContentManager* resourceContentMan = GetApplicationResourceContentManager();
        referencingIds = resourceContentMan->EnumerateAllReferences(sourceResource, MgResourcePermission::ReadWrite);
    }

    // Move the resource and all of its children.

    MgResourceHeaderManager* resourceHeaderMan = GetResourceHeaderManager();

    if (NULL != resourceHeaderMan)
    {
        MgResourceInfo resourceInfo(*destResource, m_currUserInfo, m_accessedTime);
        string defaultDoc;

        resourceHeaderMan->AddParentResources(resourceInfo, defaultDoc);
        resourceHeaderMan->MoveResource(sourceResource, destResource, overwrite);
    }

    MgResourceContentManager* resourceContentMan = GetResourceContentManager();
    ACE_ASSERT(NULL != resourceContentMan);

    resourceContentMan->MoveResource(sourceResource, destResource, overwrite);

    // If cascade update referencing resources, update referencing resources' content,
    // setting ResourceId from sourceResource to destResource
    if(cascade && referencingIds != NULL &&  referencingIds->GetCount() != 0)
    {
        STRING source = sourceResource->ToString();
        STRING dest = destResource->ToString();
        STRING prefix = L"<ResourceId>";
        for(int i = 0; i < referencingIds->GetCount(); i ++)
        {
            STRING referencingId = referencingIds->GetItem(i);
            if(referencingId.find(source) != STRING::npos)
            {
                referencingId = referencingId.replace(0, source.size(), dest);
            }
            Ptr<MgResourceIdentifier> referenceResource = new MgResourceIdentifier(referencingId);
            Ptr<MgByteReader> contentReader = GetResourceContent(referenceResource, L"");
            STRING mimeType = contentReader->GetByteSource()->GetMimeType();
            STRING content = contentReader->ToString();
            size_t pos = content.find(prefix + source);
            if(pos != STRING::npos)
            {
                content = content.replace(pos + prefix.size(), source.size(), dest);
            }
            string utf8Text = MgUtil::WideCharToMultiByte(content);
            Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)utf8Text.c_str(), (INT32)utf8Text.length());
            byteSource->SetMimeType(mimeType);
            contentReader = byteSource->GetReader();
            Ptr<MgByteReader> headerReader = GetResourceHeader(referenceResource);
            SetResource(referenceResource, contentReader, headerReader);
        }
    }

    UpdateDateModifiedResourceSet(sourceResource->GetFullPath(true));
    UpdateDateModifiedResourceSet(destResource->GetFullPath(true));

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.MoveResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Copies an existing resource to another location.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgApplicationRepositoryManager::CopyResource(
    MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource,
    bool overwrite)
{
    assert(NULL != sourceResource && NULL != destResource) ;
    auto_ptr<MgApplicationRepositoryManager> sourceRepositoryMan;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (sourceResource->IsRoot())
    {
        // Source is root
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(sourceResource->ToString());

        throw new MgInvalidArgumentException(L"MgApplicationRepositoryManager.CopyResource",
            __LINE__, __WFILE__, &arguments, L"MgInvalidResourceCannotBeRoot", NULL);
    }

    if (destResource->IsRoot())
    {
        // Destination is root
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(destResource->ToString());

        throw new MgInvalidArgumentException(L"MgApplicationRepositoryManager.CopyResource",
            __LINE__, __WFILE__, &arguments, L"MgInvalidResourceCannotBeRoot", NULL);
    }

    if (sourceResource->GetResourceType() != destResource->GetResourceType())
    {
        // Resource types are different
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(sourceResource->ToString());
        arguments.Add(L"2");
        arguments.Add(destResource->ToString());

        throw new MgInvalidArgumentException(L"MgApplicationRepositoryManager.CopyResource",
            __LINE__, __WFILE__, &arguments, L"MgResourceTypesDifferent", NULL);
    }

    if (sourceResource->ToString() == destResource->ToString())
    {
        // Resources cannot be the same.
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(sourceResource->ToString());
        arguments.Add(L"2");
        arguments.Add(destResource->ToString());

        throw new MgInvalidArgumentException(L"MgApplicationRepositoryManager.CopyResource",
            __LINE__, __WFILE__, &arguments, L"MgResourcesIdentical", NULL);
    }

    // Initialize the source MgApplicationRepositoryManager.

    if (sourceResource->GetRootPath() == destResource->GetRootPath())
    {
        m_sourceRepositoryMan = this;
    }
    else
    {
        sourceRepositoryMan.reset(Create(sourceResource));
        m_sourceRepositoryMan = sourceRepositoryMan.get();
    }

    // Copy the resource and all of its children.

    MgResourceHeaderManager* resourceHeaderMan = GetResourceHeaderManager();

    if (NULL != resourceHeaderMan)
    {
        MgResourceInfo resourceInfo(*destResource, m_currUserInfo, m_accessedTime);
        string defaultDoc;

        resourceHeaderMan->AddParentResources(resourceInfo, defaultDoc);
        resourceHeaderMan->CopyResource(sourceResource, destResource, overwrite);
    }

    MgResourceContentManager* resourceContentMan = GetResourceContentManager();
    ACE_ASSERT(NULL != resourceContentMan);

    resourceContentMan->CopyResource(sourceResource, destResource, overwrite);

    UpdateDateModifiedResourceSet(destResource->GetFullPath(true));

    MG_RESOURCE_SERVICE_CATCH(L"MgApplicationRepositoryManager.CopyResource")

    // Set the source and destination repository managers are the same by default.
    m_sourceRepositoryMan = this;

    MG_RESOURCE_SERVICE_THROW()
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates all resources which reference the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgApplicationRepositoryManager::EnumerateReferences(
    MgResourceIdentifier* resource)
{
    assert(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Perform the operation.

    MgApplicationResourceContentManager* resourceContentMan =
        GetApplicationResourceContentManager();

    byteReader = resourceContentMan->EnumerateReferences(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.EnumerateReferences")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerate all the parent Map Definition resources of the specified
/// resources.
///
/// Note that child list will also be updated with additional members during
/// the search.
///
void MgApplicationRepositoryManager::EnumerateParentMapDefinitions(
    set<string>& childResources, set<STRING>& parentResources)
{
    MG_RESOURCE_SERVICE_TRY()

    // If the child list contains some Map Definition resources, then
    // insert them into the parent list.

    for (set<string>::const_iterator i = childResources.begin();
        i != childResources.end( ); ++i)
    {
        string mbResourcePathname = *i;
        STRING wcResourcePathname;
        MgUtil::MultiByteToWideChar(mbResourcePathname, wcResourcePathname);
        MgResourceIdentifier resource(wcResourcePathname);

        if (resource.IsResourceTypeOf(MgResourceType::MapDefinition))
        {
            parentResources.insert(wcResourcePathname);
        }
    }

    MgApplicationResourceContentManager* resourceContentMan =
        GetApplicationResourceContentManager();

    // Scan through the repository tree to search for all the parent Map
    // Definition resources that reference the specified child resources.

    set<string> *prevSearchResources, *currSearchResources, *nextSearchResources;
    set<string> inputResources, outputResources;

    inputResources = childResources;
    currSearchResources = &inputResources;
    nextSearchResources = &outputResources;

    while (!currSearchResources->empty())
    {
        resourceContentMan->EnumerateParentMapDefinitions(*currSearchResources,
            *nextSearchResources, childResources, parentResources);
        prevSearchResources = currSearchResources;
        currSearchResources = nextSearchResources;
        nextSearchResources = prevSearchResources;
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.EnumerateParentMapDefinitions")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates tagged data for the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgApplicationRepositoryManager::EnumerateResourceData(
    MgResourceIdentifier* resource)
{
    assert(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (resource->IsFolder())
    {
        throw new MgInvalidResourceTypeException(
            L"MgApplicationRepositoryManager.EnumerateResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Perform the operation.

    MgApplicationResourceContentManager* resourceContentMan =
        GetApplicationResourceContentManager();

    byteReader = resourceContentMan->EnumerateResourceData(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.EnumerateResourceData")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets tagged data of a specific type to the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgApplicationRepositoryManager::SetResourceData(
    MgResourceIdentifier* resource,
    CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data)
{
    assert(NULL != resource && NULL != data);

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (resource->IsFolder())
    {
        throw new MgInvalidResourceTypeException(
            L"MgApplicationRepositoryManager.SetResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Validate the resource tag.

    MgTagManager::ValidateTag(dataName, dataType);

    // Get the resource tags.

    MgApplicationResourceContentManager* resourceContentMan =
        GetApplicationResourceContentManager();
    STRING resourceTags;
    XmlDocument xmlDoc = resourceContentMan->GetResourceTags(resource,
        MgResourcePermission::ReadWrite, resourceTags);

    // Set the resource data.

    MgTagManager tagMan(resourceTags);
    STRING mimeType = data->GetMimeType();
    STRING dataValue;

    if (MgResourceDataType::File == dataType)
    {
        // Notify the applicable caches of the changed resource.
        NotifyResourceChanged(*resource);

        // Set the tag first to ensure the data name/type is unique within
        // the resource.
        tagMan.SetTag(dataName, dataType, dataValue, mimeType);

        STRING pathname = m_dataFileMan->GetResourceDataFilePath();
        MgTagInfo filePathTag;

        if (tagMan.GetTag(MgResourceTag::DataFilePath, filePathTag, false))
        {
            pathname += filePathTag.GetAttribute(MgTagInfo::TokenValue);
        }
        else // Add the File Path tag.
        {
            STRING dirName;
            MgUtil::GenerateUuid(dirName);

            pathname += dirName;
            MgFileUtil::CreateDirectory(pathname);

            tagMan.SetTag(MgResourceTag::DataFilePath,
                MgResourceDataType::String, dirName, MgMimeType::Text);
        }

        pathname += L"/";
        pathname += dataName;

        m_dataFileMan->SetResourceData(pathname, data);
    }
    else if (MgResourceDataType::Stream == dataType)
    {
        string dataKey;
        MgTagInfo tagInfo;

        if (tagMan.GetTag(dataName, tagInfo, false))
        {
            if (tagInfo.GetAttribute(MgTagInfo::StorageType) != dataType)
            {
                MgStringCollection arguments;
                arguments.Add(dataName);

                throw new MgDuplicateResourceDataException(
                    L"MgApplicationRepositoryManager.SetResourceData",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }

            dataValue = tagInfo.GetAttribute(MgTagInfo::TokenValue);
            MgUtil::WideCharToMultiByte(dataValue, dataKey);
        }
        else
        {
            MgUtil::GenerateUuid(dataKey);
            MgUtil::MultiByteToWideChar(dataKey, dataValue);

            tagMan.SetTag(dataName, dataType, dataValue, mimeType);
        }

        m_dataStreamMan->SetResourceData(dataKey, dataName, data);
    }
    else if (MgResourceDataType::String == dataType)
    {
        dataValue = data->ToString();

        if (MgResourceDataName::UserCredentials == dataName)
        {
            MgCryptographyManager cryptoManager;

            if (!cryptoManager.IsStringEncrypted(dataValue))
            {
                throw new MgDecryptionException(
                    L"MgApplicationRepositoryManager.SetResourceData",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }

        tagMan.SetTag(dataName, dataType, dataValue, mimeType);
    }
    else
    {
        throw new MgInvalidResourceDataTypeException(
            L"MgApplicationRepositoryManager.SetResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Update the resource with the new tag.

    resourceTags = tagMan.GetTags();
    resourceContentMan->SetResourceTags(*resource, resourceTags, xmlDoc);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.SetResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes tagged data from the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgApplicationRepositoryManager::DeleteResourceData(
    MgResourceIdentifier* resource, CREFSTRING dataName)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (resource->IsFolder())
    {
        throw new MgInvalidResourceTypeException(
            L"MgApplicationRepositoryManager.DeleteResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get the resource tags.

    MgApplicationResourceContentManager* resourceContentMan =
        GetApplicationResourceContentManager();
    STRING resourceTags;
    XmlDocument xmlDoc = resourceContentMan->GetResourceTags(resource,
        MgResourcePermission::ReadWrite, resourceTags);

    // Delete the resource tag.

    MgTagManager tagMan(resourceTags);
    MgTagInfo tagInfo;

    tagMan.DeleteTag(dataName, tagInfo);

    // Delete the resource data.

    CREFSTRING dataType = tagInfo.GetAttribute(MgTagInfo::StorageType);

    if (MgResourceDataType::File == dataType)
    {
        // Notify the applicable caches of the changed resource.
        NotifyResourceChanged(*resource);

        MgTagInfo filePathTag;
        tagMan.GetTag(MgResourceTag::DataFilePath, filePathTag);

        STRING pathname = m_dataFileMan->GetResourceDataFilePath();
        pathname += filePathTag.GetAttribute(MgTagInfo::TokenValue);
        pathname += L"/";
        pathname += dataName;

        m_dataFileMan->DeleteResourceData(pathname);
    }
    else if (MgResourceDataType::Stream == dataType)
    {
        string dataKey;
        MgUtil::WideCharToMultiByte(
            tagInfo.GetAttribute(MgTagInfo::TokenValue), dataKey);

        m_dataStreamMan->DeleteResourceData(dataKey, dataName);
    }
    else if (MgResourceDataType::String != dataType)
    {
        throw new MgInvalidResourceDataTypeException(
            L"MgApplicationRepositoryManager.DeleteResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Update the resource with the new tag.

    resourceTags = tagMan.GetTags();
    resourceContentMan->SetResourceTags(*resource, resourceTags, xmlDoc);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.DeleteResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Renames tagged data for the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgApplicationRepositoryManager::RenameResourceData(
    MgResourceIdentifier* resource,
    CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (oldDataName == newDataName)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(oldDataName);
        arguments.Add(L"2");
        arguments.Add(newDataName);

        throw new MgInvalidArgumentException(L"MgApplicationRepositoryManager.RenameResourceData",
            __LINE__, __WFILE__, &arguments, L"MgFilenamesIdentical", NULL);
    }

    if (resource->IsFolder())
    {
        throw new MgInvalidResourceTypeException(
            L"MgApplicationRepositoryManager.RenameResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get the resource tags.

    MgApplicationResourceContentManager* resourceContentMan =
        GetApplicationResourceContentManager();
    STRING resourceTags;
    XmlDocument xmlDoc = resourceContentMan->GetResourceTags(resource,
        MgResourcePermission::ReadWrite, resourceTags);

    MgTagManager tagMan(resourceTags);
    MgTagInfo tagInfo;
    tagMan.GetTag(oldDataName, tagInfo);

    // Rename the resource data.

    CREFSTRING dataType = tagInfo.GetAttribute(MgTagInfo::StorageType);

    // Validate the resource tag.

    MgTagManager::ValidateTag(newDataName, dataType);

    if (MgResourceDataType::File == dataType)
    {
        // Notify the applicable caches of the changed resource.
        NotifyResourceChanged(*resource);

        MgTagInfo filePathTag;
        tagMan.GetTag(MgResourceTag::DataFilePath, filePathTag);

        STRING path = m_dataFileMan->GetResourceDataFilePath();
        path += filePathTag.GetAttribute(MgTagInfo::TokenValue);
        path += L"/";

        tagMan.RenameTag(oldDataName, newDataName);
        m_dataFileMan->RenameResourceData(path, oldDataName, newDataName, overwrite);
    }
    else if (MgResourceDataType::Stream == dataType)
    {
        tagMan.RenameTag(oldDataName, newDataName);
    }
    else
    {
        throw new MgInvalidResourceDataTypeException(
            L"MgApplicationRepositoryManager.RenameResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Update the resource with the new tag.

    resourceTags = tagMan.GetTags();
    resourceContentMan->SetResourceTags(*resource, resourceTags, xmlDoc);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.RenameResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets tagged data for the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgApplicationRepositoryManager::GetResourceData(
    MgResourceIdentifier* resource, CREFSTRING dataName,
    CREFSTRING preProcessTags)
{
    assert(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (resource->IsFolder())
    {
        throw new MgInvalidResourceTypeException(
            L"MgApplicationRepositoryManager.RenameResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get the resource tags.

    MgApplicationResourceContentManager* resourceContentMan =
        GetApplicationResourceContentManager();
    STRING resourceTags;

    resourceContentMan->GetResourceTags(resource,
        MgResourcePermission::ReadOnly, resourceTags);

    MgTagManager tagMan(resourceTags);
    MgTagInfo tagInfo;
    tagMan.GetTag(dataName, tagInfo);

    // Get the resource data.

    CREFSTRING mimeType = tagInfo.GetAttribute(MgTagInfo::MimeType);
    CREFSTRING dataType = tagInfo.GetAttribute(MgTagInfo::StorageType);

    if (MgResourceDataType::File == dataType)
    {
        MgTagInfo filePathTag;
        tagMan.GetTag(MgResourceTag::DataFilePath, filePathTag);

        STRING pathname = m_dataFileMan->GetResourceDataFilePath();
        pathname += filePathTag.GetAttribute(MgTagInfo::TokenValue);
        pathname += L"/";
        pathname += dataName;

        byteReader = m_dataFileMan->GetResourceData(pathname, mimeType);
    }
    else if (MgResourceDataType::Stream == dataType)
    {
        string dataKey;
        MgUtil::WideCharToMultiByte(
            tagInfo.GetAttribute(MgTagInfo::TokenValue), dataKey);

        byteReader = m_dataStreamMan->GetResourceData(dataKey, dataName,
            mimeType);
    }
    else if (MgResourceDataType::String == dataType)
    {
        string data;

        if (MgResourceDataName::UserCredentials == dataName)
        {
            // For security purpose, only the decrypted username
            // (no decrypted password) is returned.

            MG_CRYPTOGRAPHY_TRY()

            MgCryptographyUtil cryptoUtil;
            string password;

            cryptoUtil.DecryptCredentials(MgUtil::WideCharToMultiByte(
                tagInfo.GetAttribute(MgTagInfo::TokenValue)), data, password);

            MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgApplicationRepositoryManager.GetResourceData")
        }
        else
        {
            MgUtil::WideCharToMultiByte(
                tagInfo.GetAttribute(MgTagInfo::TokenValue), data);
        }

        Ptr<MgByteSource> byteSource = new MgByteSource(
            (BYTE_ARRAY_IN)data.c_str(), (INT32)data.length());

        byteSource->SetMimeType(mimeType);
        byteReader = byteSource->GetReader();
    }
    else
    {
        throw new MgInvalidResourceDataTypeException(
            L"MgApplicationRepositoryManager.GetResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Bind the data if required.

    if (MgResourcePreProcessingType::Substitution == preProcessTags)
    {
        MgDataBindingInfo dataBindingInfo;

        dataBindingInfo.SetPreProcessTags(preProcessTags);
        dataBindingInfo.SetResourceDataFilePath(
            m_dataFileMan->GetResourceDataFilePath());

        dataBindingInfo.SetLoginUsername(m_currUserInfo->GetUserName());
        dataBindingInfo.SetLoginPassword(m_currUserInfo->GetPassword());

        // Substitute all the tags.

        string data;

        byteReader->ToStringUtf8(data);
        tagMan.SubstituteTags(dataBindingInfo, data);

        // Re-create a byte reader.

        Ptr<MgByteSource> byteSource = new MgByteSource(
            (BYTE_ARRAY_IN)data.c_str(), (INT32)data.length());

        byteSource->SetMimeType(mimeType);
        byteReader = byteSource->GetReader();
    }
    else if (!preProcessTags.empty())
    {
        throw new MgInvalidResourcePreProcessingTypeException(
            L"MgApplicationRepositoryManager.GetResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.GetResourceData")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes all the resource data referenced in the specified resource tags.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgApplicationRepositoryManager::DeleteResourceData(
    CREFSTRING resourceTags,
    MgResourceIdentifier* resource)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Do nothing if there is no resource tag.

    if (resourceTags.empty())
    {
        return;
    }

    MgTagManager tagMan(resourceTags);
    STRING filePath;
    MgTagInfo filePathTag;

    if (tagMan.GetTag(MgResourceTag::DataFilePath, filePathTag, false))
    {
        filePath = m_dataFileMan->GetResourceDataFilePath();
        filePath += filePathTag.GetAttribute(MgTagInfo::TokenValue);
        filePath += L"/";
    }

    MgTagMap& tagMap = tagMan.GetTagMap();
    MgTagMap::const_iterator i;

    for (i = tagMap.begin(); i != tagMap.end(); ++i)
    {
        CREFSTRING dataName = (*i).first;
        const MgTagInfo& tagInfo = (*i).second;
        CREFSTRING dataType = tagInfo.GetAttribute(MgTagInfo::StorageType);

        if (MgResourceDataType::File == dataType)
        {
            // Notify the applicable caches of the changed resource.
            NotifyResourceChanged(*resource);

            assert(!filePath.empty());
            STRING pathname = filePath;
            pathname += dataName;

            m_dataFileMan->DeleteResourceData(pathname);
        }
        else if (MgResourceDataType::Stream == dataType)
        {
            string dataKey;
            MgUtil::WideCharToMultiByte(
                tagInfo.GetAttribute(MgTagInfo::TokenValue), dataKey);

            m_dataStreamMan->DeleteResourceData(dataKey, dataName);
        }
        else if (MgResourceDataType::String != dataType)
        {
            assert(false);
        }
    }

    if (!filePath.empty())
    {
        try
        {
            MgFileUtil::DeleteDirectory(filePath); // TODO: Optimize this by moving data files to a Mg recycle bin.
        }
        catch (MgException* e) // DataFiles directory has been hacked?
        {
            assert(false);
            SAFE_RELEASE(e);
        }
        catch (...)
        {
            assert(false);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.DeleteResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Copies all the resource data referenced in the source resource tags, and
/// returns destination resource tags with new references.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgApplicationRepositoryManager::CopyResourceData(
    CREFSTRING sourceResourceTags, REFSTRING destResourceTags, bool overwrite)
{
    ACE_ASSERT(NULL != m_sourceRepositoryMan);

    MG_RESOURCE_SERVICE_TRY()

    destResourceTags = L"";

    // Do nothing if there is no resource tag.

    if (sourceResourceTags.empty())
    {
        return;
    }

    MgTagManager sourceTagMan(sourceResourceTags), destTagMan;
    STRING sourcePath, destPath;
    MgTagInfo filePathTag;

    if (sourceTagMan.GetTag(MgResourceTag::DataFilePath, filePathTag, false))
    {
        // Intialize the source path.
        sourcePath = ((MgApplicationRepositoryManager*)m_sourceRepositoryMan)->m_dataFileMan->GetResourceDataFilePath();
        sourcePath += filePathTag.GetAttribute(MgTagInfo::TokenValue);
        sourcePath += L"/";

        // Intialize the destination path.
        destPath = m_dataFileMan->GetResourceDataFilePath();

        STRING dirName;
        MgUtil::GenerateUuid(dirName);

        destPath += dirName;
        MgFileUtil::CreateDirectory(destPath);

        destTagMan.SetTag(MgResourceTag::DataFilePath,
            MgResourceDataType::String, dirName, MgMimeType::Text);

        destPath += L"/";
    }

    MgTagMap& tagMap = sourceTagMan.GetTagMap();
    MgTagMap::const_iterator i;

    for (i = tagMap.begin(); i != tagMap.end(); ++i)
    {
        CREFSTRING dataName = (*i).first;
        const MgTagInfo& tagInfo = (*i).second;
        CREFSTRING dataType = tagInfo.GetAttribute(MgTagInfo::StorageType);
        STRING dataValue = tagInfo.GetAttribute(MgTagInfo::TokenValue);
        CREFSTRING mimeType = tagInfo.GetAttribute(MgTagInfo::MimeType);

        if (MgResourceDataType::File == dataType)
        {
            assert(!sourcePath.empty() && !destPath.empty() && dataValue.empty());
            STRING sourcePathname = sourcePath, destPathname = destPath;
            sourcePathname += dataName;
            destPathname += dataName;

            destTagMan.SetTag(dataName, dataType, dataValue, mimeType);
            m_dataFileMan->CopyResourceData(sourcePathname, destPathname, overwrite);
        }
        else if (MgResourceDataType::Stream == dataType)
        {
            string sourceDataKey, destDataKey;

            MgUtil::WideCharToMultiByte(dataValue, sourceDataKey);
            MgUtil::GenerateUuid(dataValue);
            MgUtil::WideCharToMultiByte(dataValue, destDataKey);

            destTagMan.SetTag(dataName, dataType, dataValue, mimeType);
            m_dataStreamMan->CopyResourceData(sourceDataKey, destDataKey, dataName);
        }
        else if (MgResourceDataType::String == dataType)
        {
            if (!sourceTagMan.IsReservedTag(dataName))
            {
                destTagMan.SetTag(dataName, dataType, dataValue, mimeType);
            }
        }
        else
        {
            assert(false);
        }
    }

    // Update the destination resource tags with new references.

    destResourceTags = destTagMan.GetTags();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationRepositoryManager.CopyResourceData")
}
