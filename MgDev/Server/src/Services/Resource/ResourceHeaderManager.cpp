//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "ResourceHeaderManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceHeaderManager::MgResourceHeaderManager(
    MgLibraryRepositoryManager& repositoryMan) :
    MgResourceDefinitionManager(repositoryMan,
        *((MgLibraryRepository&)repositoryMan.GetRepository()).
            GetResourceHeaderContainer())
{
    assert(NULL != repositoryMan.m_securityMan.get());

    m_permissionMan.reset(new MgPermissionManager(*repositoryMan.m_securityMan));
    m_permissionCache = Ptr<MgPermissionCache>(new MgPermissionCache);
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceHeaderManager::~MgResourceHeaderManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a repository header to the container.
/// </summary>
///
/// <returns>
/// false if the repository already exists, and true otherwise.
/// </returns>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::AddRepository(MgResourceInfo& resourceInfo,
    const string& document)
{
    assert(resourceInfo.GetIdentifier().IsRoot());

    AddResource(resourceInfo, document);
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates the resources in this container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgResourceHeaderManager::EnumerateResources(
    MgResourceIdentifier* resource, INT32 depth, CREFSTRING type,
    INT32 properties, CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren)
{
    assert(NULL != resource);
    Ptr<MgByteReader> byteReader;
    MgResourceHeaderMap resourceHeaderMap;

    MG_RESOURCE_SERVICE_TRY()

    // Get the specified resource and all of its parents.

    vector<string> resourceIdVector;
    string tmpStr;
    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource->ToString(), resourcePathname);
    INT32 minDepth = resource->GetDepth();
    INT32 maxDepth = 0;
    INT32 requiredDepth = -1;

    if (resource->IsFolder())
    {
        GetParentResources(*resource, resourceHeaderMap, properties, false);

        // Set up an XQuery.

        XmlManager& xmlMan = m_container.getManager();
        XmlQueryContext queryContext = xmlMan.createQueryContext();
        queryContext.setNamespace(MgResourceInfo::sm_metadataPrefix,
            MgResourceInfo::sm_metadataUri);
        string query;

        if (m_repositoryMan.m_currUserIsAdmin)
        {
            queryContext.setEvaluationType(XmlQueryContext::Lazy);
            query = "collection('";
        }
        else
        {
            queryContext.setEvaluationType(XmlQueryContext::Eager);
            query = "for $i in collection('";
        }

        query += m_container.getName();
        query += "')/*[";

        if (depth < 0)
        {
            query += "starts-with(dbxml:metadata('dbxml:name'),'";
            query += resourcePathname;
            query += "')";
        }
        else
        {
            if (0 == depth)
            {
                maxDepth = minDepth;
            }
            else // if (depth > 0)
            {
                maxDepth = minDepth + depth;
                ++minDepth;
            }

            if (!resource->IsRoot() || resource->IsRepositoryTypeOf(MgRepositoryType::Session))
            {
                query += "starts-with(dbxml:metadata('dbxml:name'),'";
                query += resourcePathname;
                query += "')";
                query += " and ";
            }

            // Note that when required, an extra level of depth is used to
            // compute the number of children for the leaf folders at the
            // maximum level of depth.

            requiredDepth = maxDepth;

            if (computeChildren && (type.empty() || MgResourceType::Folder == type))
            {
                ++requiredDepth;
            }

            query += "dbxml:metadata('Metadata:Depth')<=xs:double(";
            MgUtil::Int32ToString(requiredDepth, tmpStr);
            query += tmpStr;
            query += ")";
        }

        if (MgResourceType::Folder == type)
        {
            query += " and ends-with(dbxml:metadata('dbxml:name'),'/')";
        }
        else if (!type.empty() || !fromDate.empty() || !toDate.empty())
        {
            query += " and (ends-with(dbxml:metadata('dbxml:name'),'/')";
            query += " or (";

            if (!type.empty())
            {
                query += "ends-with(dbxml:metadata('dbxml:name'),'.";
                query += MgUtil::WideCharToMultiByte(type);
                query += "')";
                
                if (MgResourceHeaderProperties::Metadata == properties
                    && (MgResourceType::LayerDefinition == type
                        || MgResourceType::FeatureSource == type))
                {
                    query += " and //*/Metadata=*";
                }
            }

            if (!fromDate.empty() || !toDate.empty())
            {
                if (!type.empty())
                {
                    query += " and ";
                }

                if (fromDate == toDate)
                {
                    query += "dbxml:metadata('Metadata:ModifiedDate')=xs:dateTime('";
                    query += MgUtil::WideCharToMultiByte(fromDate);
                    query += "')";
                }
                else
                {
                    if (!fromDate.empty())
                    {
                        query += "dbxml:metadata('Metadata:ModifiedDate')>=xs:dateTime('";
                        query += MgUtil::WideCharToMultiByte(fromDate);
                        query += "')";
                    }

                    if (!toDate.empty())
                    {
                        if (!fromDate.empty())
                        {
                            query += " and ";
                        }

                        query += "dbxml:metadata('Metadata:ModifiedDate')<=xs:dateTime('";
                        query += MgUtil::WideCharToMultiByte(toDate);
                        query += "')";
                    }
                }
            }

            query += "))";
        }

        if (m_repositoryMan.m_currUserIsAdmin)
        {
            query += "]";
        }
        else
        {
            query += "] order by dbxml:metadata('dbxml:name', $i) return $i";
        }

        // Execute the XQuery.

        XmlResults results = IsTransacted() ?
            xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
            xmlMan.query(query, queryContext, 0);

        // Get the resources.

        size_t size = (XmlQueryContext::Lazy == queryContext.getEvaluationType()) ?
            m_permissionMan->GetPermissionInfoCacheSize() : results.size();

        if (size > 0)
        {
            resourceIdVector.reserve(size);
        }

        XmlValue xmlValue;

        while (results.next(xmlValue))
        {
            const XmlDocument& xmlDoc = xmlValue.asDocument();
            string mbResourceId = xmlDoc.getName();
            STRING wcResourceId;
            MgUtil::MultiByteToWideChar(mbResourceId, wcResourceId);
            MgResourceIdentifier currResource(wcResourceId);
            STRING wcParentId = currResource.GetFullPath(true);
            string mbParentId;
            MgUtil::WideCharToMultiByte(wcParentId, mbParentId);

            // Check if the current user has read access to the current resource.

            MgResourceHeader* resourceHeader;
            MgResourceHeaderMap::const_iterator i = resourceHeaderMap.find(
                mbResourceId);

            if (resourceHeaderMap.end() == i)
            {
                if (!m_repositoryMan.m_currUserIsAdmin)
                {
                    if (!CheckParentPermission(currResource,
                        MgResourcePermission::ReadOnly, false))
                    {
                        continue;
                    }
                }

                resourceHeader = new MgResourceHeader();
                resourceHeaderMap.insert(MgResourceHeaderMap::value_type(
                    mbResourceId, resourceHeader));
            }
            else
            {
                resourceHeader = (*i).second;
            }

            // Associate the current resource with a header.

            if (m_repositoryMan.m_currUserIsAdmin)
            {
                resourceHeader->Initialize(currResource, xmlDoc, properties);
            }
            else
            {
                // Update the local permission cache if necessary.

                bool local;
                const MgPermissionInfo* permissionInfo =
                    GetPermissionInfo(wcResourceId, local);

                if (NULL == permissionInfo)
                {
                    MgPermissionInfo* newPermissionInfo = new MgPermissionInfo(m_repositoryMan.m_accessedTime);
                    resourceHeader->Initialize(currResource, xmlDoc, properties, newPermissionInfo);
                    m_permissionCache->SetPermissionInfo(wcResourceId, newPermissionInfo);
                }
                else
                {
                    resourceHeader->Initialize(currResource, xmlDoc, properties);
                }
            }

            // Associate the parent folder of the current resource with a header.

            if (!currResource.IsRoot())
            {
                MgResourceHeader* parentHeader;

                i = resourceHeaderMap.find(mbParentId);

                if (resourceHeaderMap.end() == i)
                {
                    parentHeader = new MgResourceHeader();
                    resourceHeaderMap.insert(MgResourceHeaderMap::value_type(
                        mbParentId, parentHeader));
                }
                else
                {
                    parentHeader = (*i).second;
                }

                // Update the number of childrens contained in the parent folder.

                if (currResource.IsFolder())
                {
                    parentHeader->IncrementNumberOfFolders();
                }
                else
                {
                    parentHeader->IncrementNumberOfDocuments();
                }
            }

            // Filter resources based on depth.

            if (depth >= 0)
            {
                INT32 currDepth = currResource.GetDepth();

                if (currDepth < minDepth || currDepth > maxDepth)
                {
                    continue;
                }
            }

            // Filter resources based on type.

            if (!type.empty() && !currResource.IsResourceTypeOf(type))
            {
                continue;
            }

            // Filter resources based on modified date.

            if (currResource.IsFolder() && (!fromDate.empty() || !toDate.empty()))
            {
                STRING modifiedDate = MgUtil::MultiByteToWideChar(
                    resourceHeader->GetMetadata(MgResourceInfo::ModifiedDate).asString());
                assert(!modifiedDate.empty());

                if ((!fromDate.empty() && modifiedDate < fromDate) ||
                    (!toDate.empty()   && modifiedDate > toDate))
                {
                    continue;
                }
            }

            // Add the ID of the qualified resource.

            resourceIdVector.push_back(mbResourceId);
        }
    }
    else
    {
        bool qualified = true;

        GetParentResources(*resource, resourceHeaderMap, properties, true);

        // Check if the current user has read access to the current resource.

        CheckParentPermission(*resource, MgResourcePermission::ReadOnly);

        // Filter resources based on modified date.

        if (!fromDate.empty() || !toDate.empty())
        {
            MgResourceHeader* resourceHeader = NULL;
            MgResourceHeaderMap::const_iterator i = resourceHeaderMap.find(
                resourcePathname);

            if (resourceHeaderMap.end() != i)
            {
                resourceHeader = (*i).second;

                STRING modifiedDate = MgUtil::MultiByteToWideChar(
                    resourceHeader->GetMetadata(MgResourceInfo::ModifiedDate).asString());
                assert(!modifiedDate.empty());

                if ((!fromDate.empty() && modifiedDate < fromDate) ||
                    (!toDate.empty()   && modifiedDate > toDate))
                {
                    qualified = false;
                }
            }
            else
            {
                assert(false);
                qualified = false;
            }
        }

        // Add the ID of the qualified resource.

        if (qualified)
        {
            resourceIdVector.push_back(resourcePathname);
        }
    }

    // Populate the resource list.

    string resourceList;

    BeginWriteResourceList(resourceList);

    for (size_t index = 0; index < resourceIdVector.size(); ++index)
    {
        const string& resourceId = resourceIdVector[index];
        MgResourceHeader* resourceHeader = 0;
        MgResourceHeaderMap::const_iterator i = resourceHeaderMap.find(
            resourceId);

        if (resourceHeaderMap.end() != i)
        {
            resourceHeader = (*i).second;
        }
        else
        {
            MgResourceIdentifier currResource(
                MgUtil::MultiByteToWideChar(resourceId));

            m_repositoryMan.ThrowResourceNotFoundException(currResource,
                L"MgResourceHeaderManager.EnumerateResources",
                __LINE__, __WFILE__);
        }

        WriteResourceList(resourceList, resourceId, &resourceHeaderMap,
            *resourceHeader, properties, requiredDepth, maxDepth);
    }

    EndWriteResourceList(resourceList);

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)resourceList.c_str(), (INT32)resourceList.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH(L"MgResourceHeaderManager.EnumerateResources")

    // Clean up.

    MgResourceHeaderMap::const_iterator i;

    for (i = resourceHeaderMap.begin(); i != resourceHeaderMap.end(); ++i)
    {
        delete (*i).second;
    }

    resourceHeaderMap.clear();

    MG_RESOURCE_SERVICE_THROW()

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a resource header to the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::AddResource(MgResourceInfo& resourceInfo,
    const string& document)
{
    MG_RESOURCE_SERVICE_TRY()

    MgResourceIdentifier& resource = resourceInfo.GetIdentifier();
    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource.ToString(), resourcePathname);

    // Set the resource metadata.

    XmlManager& xmlMan = m_container.getManager();
    XmlDocument xmlDoc = xmlMan.createDocument();

    xmlDoc.setName(resourcePathname);
    xmlDoc.setMetaData(
        MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::Depth],
        resourceInfo.GetMetadata(MgResourceInfo::Depth));
    xmlDoc.setMetaData(
        MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::Owner],
        resourceInfo.GetMetadata(MgResourceInfo::Owner));
    xmlDoc.setMetaData(
        MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::CreatedDate],
        resourceInfo.GetMetadata(MgResourceInfo::CreatedDate));
    xmlDoc.setMetaData(
        MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
        resourceInfo.GetMetadata(MgResourceInfo::ModifiedDate));

    // Use a default resource header if it is not specified.

    if (document.empty())
    {
        xmlDoc.setContent(GetDefaultHeader(resource));
    }
    else
    {
        xmlDoc.setContent(document);
    }

    // Add the resource header.

    XmlUpdateContext updateContext = xmlMan.createUpdateContext();

    PutDocument(resource, xmlDoc, updateContext);

    if (!resource.IsRoot())
    {
        m_repositoryMan.UpdateDateModifiedResourceSet(resource.GetFullPath(true));
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.AddResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds non-existing parents of the specifed resource to the container.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::AddParentResources(MgResourceInfo& resourceInfo,
    const string& document)
{
    MG_RESOURCE_SERVICE_TRY()

    // Create new folders if required.

    MgResourceIdentifier& resource = resourceInfo.GetIdentifier();
    STRING parentPathname = resource.GetFullPath(true);
    const INT32 depth = resource.GetDepth();
    vector<STRING> parentIds;

    parentIds.reserve(depth);

    for (INT32 i = depth - 1; i > 0; --i)
    {
        size_t index = parentPathname.rfind(L'/');

        if (index == STRING::npos)
        {
            MgStringCollection arguments;
            arguments.Add(resource.ToString());

            throw new MgInvalidResourceNameException(
                L"MgResourceHeaderManager.AddParentResources",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
        {
            parentPathname.erase(index + 1);

            if (ResourceExists(parentPathname))
            {
                break;
            }
            else
            {
                parentIds.push_back(parentPathname);
            }

            parentPathname.erase(index);
        }
    }

    const INT32 size = (INT32)parentIds.size();

    if (size > 0)
    {
        MgResourceInfo parentInfo(resourceInfo);
        string defaultDoc;
        const string& headerDoc = resource.IsFolder() ? document : defaultDoc;

        for (INT32 i = size - 1; i >= 0; --i)
        {
            parentInfo.SetIdentifier(parentIds[i]);
            AddResource(parentInfo, headerDoc);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.AddParentResources")
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Renames a resource and/or moves it from one location to another.
///
void MgResourceHeaderManager::MoveResource(
    MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource,
    bool overwrite)
{
    assert(NULL != sourceResource && NULL != destResource);

    MG_RESOURCE_SERVICE_TRY()

    // Check whether or not the destination resource already exists.

    if (!overwrite && ResourceExists(destResource->ToString()))
    {
        m_repositoryMan.ThrowDuplicateResourceException(*destResource,
            L"MgResourceHeaderManager.MoveResource",
            __LINE__, __WFILE__);
    }

    // Set up an XQuery.

    string sourcePathname, destPathname;

    MgUtil::WideCharToMultiByte(sourceResource->ToString(), sourcePathname);
    MgUtil::WideCharToMultiByte(destResource->ToString(), destPathname);

    bool sourceResourceIsFolder = sourceResource->IsFolder();
    string query;

    if (sourceResourceIsFolder)
    {
        assert(destResource->IsFolder());

        query  = "for $i in collection('";
        query += m_container.getName();
        query += "')";
        query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
        query += sourcePathname;
        query += "')]";
        query += " order by dbxml:metadata('dbxml:name', $i) return $i";
    }
    else
    {
        assert(!destResource->IsFolder());

        query  = "collection('";
        query += m_container.getName();
        query += "')";
        query += "/*[dbxml:metadata('dbxml:name')='";
        query += sourcePathname;
        query += "']";
    }

    // Execute the XQuery.

    XmlManager& xmlManager = m_container.getManager();
    XmlQueryContext queryContext = xmlManager.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlManager.query(GetXmlTxn(), query, queryContext, 0) :
        xmlManager.query(query, queryContext, 0);

    if (0 == results.size())
    {
        m_repositoryMan.ThrowResourceNotFoundException(*sourceResource,
            L"MgResourceHeaderManager.MoveResource",
            __LINE__, __WFILE__);
    }

    // Move/rename the resources.

    XmlUpdateContext updateContext = xmlManager.createUpdateContext();
    const size_t sourcePathLength = sourcePathname.length();
    const XmlValue modifiedDate = m_repositoryMan.GetAccessedTime();
    XmlValue xmlValue, createdDate;

    while (results.next(xmlValue))
    {
        XmlDocument currDoc = xmlValue.asDocument();
        string currPathname = currDoc.getName();
        MgResourceIdentifier currResource(MgUtil::MultiByteToWideChar(currPathname));

        CheckParentPermission(currResource, MgResourcePermission::ReadWrite);

        if (sourceResourceIsFolder)
        {
            currPathname.replace(0, sourcePathLength, destPathname);
            currResource.SetResource(MgUtil::MultiByteToWideChar(currPathname));
        }
        else
        {
            assert(1 == results.size());

            currPathname = destPathname;
            currResource = *destResource;
        }

        if (overwrite)
        {
            XmlDocument oldDoc;

            if (MgResourceDefinitionManager::GetDocument(currPathname, oldDoc))
            {
                oldDoc.getMetaData(
                    MgResourceInfo::sm_metadataUri,
                    MgResourceInfo::sm_metadataNames[MgResourceInfo::CreatedDate],
                    createdDate);
                currDoc.setMetaData(
                    MgResourceInfo::sm_metadataUri,
                    MgResourceInfo::sm_metadataNames[MgResourceInfo::CreatedDate],
                    createdDate);
                currDoc.setMetaData(
                    MgResourceInfo::sm_metadataUri,
                    MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
                    modifiedDate);

                DeleteDocument(currResource, oldDoc, updateContext);
            }
        }

        currDoc.setName(currPathname);
        currDoc.setMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::Depth],
            XmlValue(static_cast<double>(currResource.GetDepth())));

        UpdateDocument(currResource, currDoc, updateContext,
            MgResourceService::opIdMoveResource);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.MoveResource")
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Copies an existing resource to another location.
///
void MgResourceHeaderManager::CopyResource(
    MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource,
    bool overwrite)
{
    assert(NULL != sourceResource && NULL != destResource);

    MG_RESOURCE_SERVICE_TRY()

    // Get the source MgResourceHeaderManager.
    // If it does not exist, then add default headers to the destination repository.

    MgApplicationRepositoryManager* sourceRepositoryMan =
        dynamic_cast<MgApplicationRepositoryManager*>(
        ((MgApplicationRepositoryManager&)m_repositoryMan).GetSourceRepositoryManager());
    ACE_ASSERT(NULL != sourceRepositoryMan);
    MgResourceHeaderManager* sourceResourceHeaderMan =
        sourceRepositoryMan->GetResourceHeaderManager();

    if (NULL == sourceResourceHeaderMan)
    {
        if (!ResourceExists(destResource->ToString()))
        {
            MgResourceInfo resourceInfo(*destResource,
                m_repositoryMan.m_currUserInfo, m_repositoryMan.m_accessedTime);
            string defaultDoc;

            AddParentResources(resourceInfo, defaultDoc);
            AddResource(resourceInfo, defaultDoc);
        }

        return;
    }

    // Check whether or not the destination resource already exists.

    if (!overwrite && ResourceExists(destResource->ToString()))
    {
        m_repositoryMan.ThrowDuplicateResourceException(*destResource,
            L"MgResourceHeaderManager.CopyResource",
            __LINE__, __WFILE__);
    }

    // Set up an XQuery.

    string sourcePathname, destPathname;

    MgUtil::WideCharToMultiByte(sourceResource->ToString(), sourcePathname);
    MgUtil::WideCharToMultiByte(destResource->ToString(), destPathname);

    bool sourceResourceIsFolder = sourceResource->IsFolder();
    string query;

    if (sourceResourceIsFolder)
    {
        assert(destResource->IsFolder());

        query  = "for $i in collection('";
        query += sourceResourceHeaderMan->m_container.getName();
        query += "')";
        query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
        query += sourcePathname;
        query += "')]";
        query += " order by dbxml:metadata('dbxml:name', $i) return $i";
    }
    else
    {
        assert(!destResource->IsFolder());

        query  = "collection('";
        query += sourceResourceHeaderMan->m_container.getName();
        query += "')";
        query += "/*[dbxml:metadata('dbxml:name')='";
        query += sourcePathname;
        query += "']";
    }

    // Execute the XQuery.

    XmlManager& sourceXmlMan = sourceResourceHeaderMan->m_container.getManager();
    XmlQueryContext queryContext = sourceXmlMan.createQueryContext();
    XmlResults results = sourceResourceHeaderMan->IsTransacted() ?
        sourceXmlMan.query(sourceResourceHeaderMan->GetXmlTxn(), query, queryContext, 0) :
        sourceXmlMan.query(query, queryContext, 0);

    if (0 == results.size())
    {
        sourceRepositoryMan->ThrowResourceNotFoundException(*sourceResource,
            L"MgResourceHeaderManager.CopyResource",
            __LINE__, __WFILE__);
    }

    // Get the current user information.

    string owner;
    MgUtil::WideCharToMultiByte(m_repositoryMan.m_currUserInfo->GetUserName(), owner);

    // Copy the resources.

    XmlManager& destXmlMan = m_container.getManager();
    XmlUpdateContext updateContext = destXmlMan.createUpdateContext();
    const size_t sourcePathLength = sourcePathname.length();
    const XmlValue accessedTime = m_repositoryMan.GetAccessedTime();
    XmlValue xmlValue, createdDate, modifiedDate;

    while (results.next(xmlValue))
    {
        const XmlDocument& currDoc = xmlValue.asDocument();
        string currPathname = currDoc.getName();
        MgResourceIdentifier currResource(MgUtil::MultiByteToWideChar(currPathname));

        sourceResourceHeaderMan->CheckPermission(currResource, MgResourcePermission::ReadOnly);

        if (sourceResourceIsFolder)
        {
            currPathname.replace(0, sourcePathLength, destPathname);
            currResource.SetResource(MgUtil::MultiByteToWideChar(currPathname));
        }
        else
        {
            assert(1 == results.size());

            currPathname = destPathname;
            currResource = *destResource;
        }

        bool overwritten = false;

        if (overwrite)
        {
            XmlDocument oldDoc;

            if (MgResourceDefinitionManager::GetDocument(currPathname, oldDoc))
            {
                oldDoc.getMetaData(
                    MgResourceInfo::sm_metadataUri,
                    MgResourceInfo::sm_metadataNames[MgResourceInfo::CreatedDate],
                    createdDate);
                modifiedDate = accessedTime;

                DeleteDocument(currResource, oldDoc, updateContext);
                overwritten = true;
            }
        }

        XmlDocument newDoc = destXmlMan.createDocument();

        newDoc.setName(currPathname);
        newDoc.setMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::Depth],
            XmlValue(static_cast<double>(currResource.GetDepth())));

        if (!overwritten)
        {
            createdDate = accessedTime;
            currDoc.getMetaData(
                MgResourceInfo::sm_metadataUri,
                MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
                modifiedDate);
        }

        newDoc.setMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::Owner],
            XmlValue(owner));
        newDoc.setMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::CreatedDate],
            createdDate);
        newDoc.setMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
            modifiedDate);

        newDoc.setContentAsXmlInputStream(currDoc.getContentAsXmlInputStream());

        PutDocument(currResource, newDoc, updateContext);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.CopyResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the modified date of the specified resource.
/// </summary>
///
/// <exceptions>
/// MgInvalidRepositoryTypeException
/// MgInvalidResourceTypeException
/// </exceptions>
///----------------------------------------------------------------------------

MgDateTime* MgResourceHeaderManager::GetResourceModifiedDate(
    MgResourceIdentifier* resource)
{
    assert(NULL != resource);
    Ptr<MgDateTime> dateTime;

    MG_RESOURCE_SERVICE_TRY()

    // Get the resource.

    XmlValue modifiedDate;
    XmlDocument xmlDoc = GetDocument(*resource);

    if (xmlDoc.getMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
            modifiedDate))
    {
        dateTime = new MgDateTime(modifiedDate.asString());
    }
    else
    {
        // TODO: Throw a more specific exception?
        throw new MgXmlParserException(
            L"MgResourceHeaderManager.GetResourceModifiedDate",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.GetResourceModifiedDate")

    return dateTime.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerate the resource documents in the specified repository.
///
STRING MgResourceHeaderManager::EnumerateResourceDocuments(
    MgStringCollection* resources, CREFSTRING type, INT32 properties)
{
    string resourceList;

    MG_RESOURCE_SERVICE_TRY()

    INT32 resourceCount = (NULL == resources) ? 0 : resources->GetCount();

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')/*[";

    if (resourceCount > 0)
    {
        query += "dbxml:metadata('dbxml:name')[";

        for (INT32 i = 0; i < resourceCount; ++i)
        {
            if (0 != i)
            {
                query += " or ";
            }

            query += ".='";
            query += MgUtil::WideCharToMultiByte(resources->GetItem(i));
            query += "'";
        }

        query += "]";
    }
    else
    {
        query += "ends-with(dbxml:metadata('dbxml:name'),'";
        query += MgUtil::WideCharToMultiByte(type);
        query += "')";
                
        if (MgResourceHeaderProperties::Metadata == properties
            && (MgResourceType::LayerDefinition == type
                || MgResourceType::FeatureSource == type))
        {
            query += " and //*/Metadata=*";
        }
    }

    query += "]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    queryContext.setEvaluationType(XmlQueryContext::Lazy);
    queryContext.setNamespace(MgResourceInfo::sm_metadataPrefix,
        MgResourceInfo::sm_metadataUri);
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Populate the resource list.

    XmlValue xmlValue;

    BeginWriteResourceList(resourceList);

    while (results.next(xmlValue))
    {
        // Get the resource.

        const XmlDocument& xmlDoc = xmlValue.asDocument();
        string mbResourceId = xmlDoc.getName();
        STRING wcResourceId;
        MgUtil::MultiByteToWideChar(mbResourceId, wcResourceId);
        MgResourceIdentifier currResource(wcResourceId);

        // Check the permission on the parent resource.

        if (!m_repositoryMan.m_currUserIsAdmin
            && !CheckParentPermission(
                currResource, MgResourcePermission::ReadOnly, false))
        {
            continue;
        }

        // Write the resource list.

        auto_ptr<MgResourceHeader> resourceHeader;

        resourceHeader.reset(new MgResourceHeader());
        resourceHeader->Initialize(currResource, xmlDoc, properties);

        WriteResourceList(resourceList, mbResourceId, NULL,
            *resourceHeader.get(), properties, -1, -1);
    }

    EndWriteResourceList(resourceList);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.EnumerateResourceDocuments")

    return MgUtil::MultiByteToWideChar(resourceList);
}

///----------------------------------------------------------------------------
/// <summary>
/// Changes the owner of an existing resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::ChangeResourceOwner(
    MgResourceIdentifier* resource, CREFSTRING owner, bool includeDescendants)
{
    assert(NULL != resource && !owner.empty());

    MG_RESOURCE_SERVICE_TRY()

    // Ensure the new owner is an existing user.

    if (!m_repositoryMan.m_securityMan->FindUser(owner))
    {
        MgStringCollection arguments;
        arguments.Add(owner);

        throw new MgUserNotFoundException(
            L"MgResourceHeaderManager.ChangeResourceOwner",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource->ToString(), resourcePathname);

    // Set up an XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    queryContext.setNamespace(MgResourceInfo::sm_metadataPrefix,
        MgResourceInfo::sm_metadataUri);
    string query;

    // Allow changing the ownership only if the current user is
    // either the administrator or the owner.

    if (resource->IsFolder() && includeDescendants)
    {
        if (m_repositoryMan.m_currUserIsAdmin)
        {
            queryContext.setEvaluationType(XmlQueryContext::Lazy);

            query  = "collection('";
            query += m_container.getName();
            query += "')";
            query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
            query += resourcePathname;
            query += "')]";
        }
        else
        {
            queryContext.setEvaluationType(XmlQueryContext::Eager);

            query = "for $i in collection('";
            query += m_container.getName();
            query += "')";
            query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
            query += resourcePathname;
            query += "')";
            query += " and dbxml:metadata('Metadata:Owner')='";
            query += MgUtil::WideCharToMultiByte(m_repositoryMan.m_currUserInfo->GetUserName());
            query += "']";
            query += " order by dbxml:metadata('dbxml:name', $i) return $i";
        }
    }
    else
    {
        query  = "collection('";
        query += m_container.getName();
        query += "')";
        query += "/*[dbxml:metadata('dbxml:name')='";
        query += resourcePathname;

        if (m_repositoryMan.m_currUserIsAdmin)
        {
            query += "']";
        }
        else
        {
            query += "'";
            query += " and dbxml:metadata('Metadata:Owner')='";
            query += MgUtil::WideCharToMultiByte(m_repositoryMan.m_currUserInfo->GetUserName());
            query += "']";
        }
    }

    // Execute the XQuery.

    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Change the resource owner.

    MgResourceIdentifier currResource;
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();
    const XmlValue newOwner(MgUtil::WideCharToMultiByte(owner));
    const XmlValue modifiedDate = m_repositoryMan.GetAccessedTime();
    XmlValue xmlValue;
    INT32 count = 0;

    while (results.next(xmlValue))
    {
        ++count;
        XmlDocument xmlDoc = xmlValue.asDocument();
        XmlValue oldOwner;

        if (xmlDoc.getMetaData(
                MgResourceInfo::sm_metadataUri,
                MgResourceInfo::sm_metadataNames[MgResourceInfo::Owner],
                oldOwner)
            && !(oldOwner == newOwner))
        {
            xmlDoc.setMetaData(
                MgResourceInfo::sm_metadataUri,
                MgResourceInfo::sm_metadataNames[MgResourceInfo::Owner],
                XmlValue(newOwner));
            xmlDoc.setMetaData(
                MgResourceInfo::sm_metadataUri,
                MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
                modifiedDate);

            currResource.SetResource(MgUtil::MultiByteToWideChar(
                xmlDoc.getName()));

            UpdateDocument(currResource, xmlDoc, updateContext,
                MgResourceService::opIdChangeResourceOwner);
        }
    }

    // Throw an exception if the specified resource does not exists.

    if (0 == count)
    {
        if (ResourceExists(resource->ToString()))
        {
            MG_LOG_AUTHENTICATION_ENTRY(MgResources::PermissionDenied.c_str());

            MgStringCollection arguments;
            arguments.Add(resource->ToString());

            throw new MgPermissionDeniedException(
                L"MgResourceHeaderManager.ChangeResourceOwner",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
        {
            m_repositoryMan.ThrowResourceNotFoundException(*resource,
                L"MgResourceHeaderManager.ChangeResourceOwner",
                __LINE__, __WFILE__);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.ChangeResourceOwner")
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the permissions for all descendants to be inherited from the
/// specified resource.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::InheritPermissionsFrom(
    MgResourceIdentifier* resource)
{
    assert(NULL != resource && resource->IsFolder());

    MG_RESOURCE_SERVICE_TRY()

    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource->ToString(), resourcePathname);

    // Set up an XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    queryContext.setNamespace(MgResourceInfo::sm_metadataPrefix,
        MgResourceInfo::sm_metadataUri);
    string query;

    // Allow changing the permission inheritance only if the current user is
    // either the administrator or the owner.

    if (m_repositoryMan.m_currUserIsAdmin)
    {
        queryContext.setEvaluationType(XmlQueryContext::Lazy);

        query  = "collection('";
        query += m_container.getName();
        query += "')";
        query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
        query += resourcePathname;
        query += "')]";
    }
    else
    {
        queryContext.setEvaluationType(XmlQueryContext::Eager);

        query = "for $i in collection('";
        query += m_container.getName();
        query += "')";
        query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
        query += resourcePathname;
        query += "')";
        query += " and dbxml:metadata('Metadata:Owner')='";
        query += MgUtil::WideCharToMultiByte(m_repositoryMan.m_currUserInfo->GetUserName());
        query += "']";
        query += " order by dbxml:metadata('dbxml:name', $i) return $i";
    }

    // Execute the XQuery.

    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Get the resources and change the permissions.

    MgResourceIdentifier currResource;
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();
    const XmlValue modifiedDate = m_repositoryMan.GetAccessedTime();
    XmlValue xmlValue;
    INT32 count = 0;

    while (results.next(xmlValue))
    {
        ++count;
        XmlDocument xmlDoc = xmlValue.asDocument();

        if (resourcePathname != xmlDoc.getName())
        {
            string xmlContent;
            MgXmlUtil xmlUtil(xmlDoc.getContent(xmlContent));
            DOMElement* rootNode = xmlUtil.GetRootNode();
            DOMNode* securityNode = xmlUtil.GetElementNode(rootNode, "Security");

            if (xmlUtil.SetElementValue(
                reinterpret_cast<DOMElement*>(securityNode), "Inherited", L"true"))
            {
                string document;
                xmlUtil.ToStringUtf8(document);

                // Update the document.

                xmlDoc.setContent(document);
                xmlDoc.setMetaData(
                    MgResourceInfo::sm_metadataUri,
                    MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
                    modifiedDate);

                currResource.SetResource(MgUtil::MultiByteToWideChar(
                    xmlDoc.getName()));

                UpdateDocument(currResource, xmlDoc, updateContext,
                    MgResourceService::opIdInheritPermissionsFrom);
            }
        }
    }

    // Throw an exception if the specified resource does not exists.

    if (0 == count)
    {
        if (ResourceExists(resource->ToString()))
        {
            MG_LOG_AUTHENTICATION_ENTRY(MgResources::PermissionDenied.c_str());

            MgStringCollection arguments;
            arguments.Add(resource->ToString());

            throw new MgPermissionDeniedException(
                L"MgResourceHeaderManager.InheritPermissionsFrom",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
        {
            m_repositoryMan.ThrowResourceNotFoundException(*resource,
                L"MgResourceHeaderManager.InheritPermissionsFrom",
                __LINE__, __WFILE__);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.InheritPermissionsFrom")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the default header for the specified resource.
/// </summary>
///----------------------------------------------------------------------------

const string& MgResourceHeaderManager::GetDefaultHeader(
    const MgResourceIdentifier& resource)
{
    if (!resource.IsFolder())
    {
        if (m_defaultDocumentHeader.empty())
        {
            // this XML follows the ResourceDocumentHeader-1.0.0.xsd schema
            m_defaultDocumentHeader = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            m_defaultDocumentHeader += "<ResourceDocumentHeader xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ResourceDocumentHeader-1.0.0.xsd\">\n";
            m_defaultDocumentHeader += "\t<Security xsi:noNamespaceSchemaLocation=\"ResourceSecurity-1.0.0.xsd\">\n";
            m_defaultDocumentHeader += "\t\t<Inherited>true</Inherited>\n";
            m_defaultDocumentHeader += "\t</Security>\n";
            m_defaultDocumentHeader += "</ResourceDocumentHeader>";
        }

        return m_defaultDocumentHeader;
    }
    else if (resource.IsRoot())
    {
        if (m_defaultRootHeader.empty())
        {
            // this XML follows the ResourceFolderHeader-1.0.0.xsd schema
            m_defaultRootHeader = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            m_defaultRootHeader += "<ResourceFolderHeader xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ResourceFolderHeader-1.0.0.xsd\">\n";
            m_defaultRootHeader += "\t<Security xsi:noNamespaceSchemaLocation=\"ResourceSecurity-1.0.0.xsd\">\n";
            m_defaultRootHeader += "\t\t<Inherited>false</Inherited>\n";
            m_defaultRootHeader += "\t\t<Groups>\n";
            m_defaultRootHeader += "\t\t\t<Group>\n";
            m_defaultRootHeader += "\t\t\t\t<Name>Everyone</Name>\n";
            m_defaultRootHeader += "\t\t\t\t<Permissions>r,w</Permissions>\n";
            m_defaultRootHeader += "\t\t\t</Group>\n";
            m_defaultRootHeader += "\t\t</Groups>\n";
            m_defaultRootHeader += "\t</Security>\n";
            m_defaultRootHeader += "</ResourceFolderHeader>";
        }

        return m_defaultRootHeader;
    }
    else
    {
        if (m_defaultFolderHeader.empty())
        {
            // this XML follows the ResourceFolderHeader-1.0.0.xsd schema
            m_defaultFolderHeader = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            m_defaultFolderHeader += "<ResourceFolderHeader xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ResourceFolderHeader-1.0.0.xsd\">\n";
            m_defaultFolderHeader += "\t<Security xsi:noNamespaceSchemaLocation=\"ResourceSecurity-1.0.0.xsd\">\n";
            m_defaultFolderHeader += "\t\t<Inherited>true</Inherited>\n";
            m_defaultFolderHeader += "\t</Security>\n";
            m_defaultFolderHeader += "</ResourceFolderHeader>";
        }

        return m_defaultFolderHeader;
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets up a permission query.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::SetUpPermissionQuery(
    XmlQueryContext& queryContext,
    XmlQueryExpression& selectInheritedFlags,
    XmlQueryExpression& selectUserNames,
    XmlQueryExpression& selectUserPermissions,
    XmlQueryExpression& selectGroupNames,
    XmlQueryExpression& selectGroupPermissions)
{
    XmlManager& xmlMan = m_container.getManager();
    queryContext.setReturnType(XmlQueryContext::DeadValues);

    string xpathInheritedFlags = "/*/Security/Inherited/text()";
    selectInheritedFlags = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), xpathInheritedFlags, queryContext) :
        xmlMan.prepare(xpathInheritedFlags, queryContext);

    string xpathUserNames = "/*/Security/Users/User/Name/text()";
    selectUserNames = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), xpathUserNames, queryContext) :
        xmlMan.prepare(xpathUserNames, queryContext);

    string xpathUserPermissions = "/*/Security/Users/User/Permissions/text()";
    selectUserPermissions = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), xpathUserPermissions, queryContext) :
        xmlMan.prepare(xpathUserPermissions, queryContext);

    string xpathGroupNames = "/*/Security/Groups/Group/Name/text()";
    selectGroupNames = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), xpathGroupNames, queryContext) :
        xmlMan.prepare(xpathGroupNames, queryContext);

    string xpathGroupPermissions = "/*/Security/Groups/Group/Permissions/text()";
    selectGroupPermissions = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), xpathGroupPermissions, queryContext) :
        xmlMan.prepare(xpathGroupPermissions, queryContext);
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the default permission information for the specified resource.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionInfo* MgResourceHeaderManager::GetDefaultPermissionInfo(
    CREFSTRING resource) const
{
    MgPermissionInfo* permissionInfo = new MgPermissionInfo(m_repositoryMan.m_accessedTime);

    permissionInfo->SetOwner(m_repositoryMan.m_currUserInfo->GetUserName());

    if ((resource.length() - 2) == resource.find(L"//")) // Root folder
    {
        permissionInfo->SetInherited(false);
        permissionInfo->SetGroupPermission(
            MgGroup::Everyone, MgResourcePermission::ReadWrite);
    }
    else // Sub-folder or document
    {
        permissionInfo->SetInherited(true);
    }

    return permissionInfo;
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates an MgPermissionInfo object from the specified document.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionInfo* MgResourceHeaderManager::CreatePermissionInfo(
    XmlDocument& xmlDoc, XmlQueryContext& queryContext,
    XmlQueryExpression& selectInheritedFlags,
    XmlQueryExpression& selectUserNames,
    XmlQueryExpression& selectUserPermissions,
    XmlQueryExpression& selectGroupNames,
    XmlQueryExpression& selectGroupPermissions)
{
    auto_ptr<MgPermissionInfo> permissionInfo;

    MG_RESOURCE_SERVICE_TRY()

    // Read the resource permission information.

    permissionInfo.reset(new MgPermissionInfo(m_repositoryMan.m_accessedTime));
    STRING owner, name, permission;
    bool inherited;
    XmlValue ownerValue, inheritedValue, nameValue, permissionValue;
    XmlResults inheritedFlagResults, nameResults, permissionResults;

    // Get the owner.

    if (xmlDoc.getMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::Owner],
            ownerValue))
    {
        MgUtil::MultiByteToWideChar(ownerValue.asString(), owner);
        permissionInfo->SetOwner(owner);
    }
    else
    {
        // TODO: Throw a more specific exception?
        throw new MgXmlParserException(
            L"MgResourceHeaderManager.CreatePermissionInfo",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get the inherited flag.

    inheritedFlagResults = IsTransacted() ?
        selectInheritedFlags.execute(GetXmlTxn(), xmlDoc, queryContext, 0) :
        selectInheritedFlags.execute(xmlDoc, queryContext, 0);

    if (inheritedFlagResults.next(inheritedValue))
    {
        assert(1 == inheritedFlagResults.size());
        inherited = ("true" == inheritedValue.asString());
        permissionInfo->SetInherited(inherited);
    }
    else
    {
        throw new MgXmlParserException(
            L"MgResourceHeaderManager.CreatePermissionInfo",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!inherited)
    {
        // Get user permissions.

        nameResults = IsTransacted() ?
            selectUserNames.execute(GetXmlTxn(), xmlDoc, queryContext, 0) :
            selectUserNames.execute(xmlDoc, queryContext, 0);
        permissionResults = IsTransacted() ?
            selectUserPermissions.execute(GetXmlTxn(), xmlDoc, queryContext, 0) :
            selectUserPermissions.execute(xmlDoc, queryContext, 0);

        if (nameResults.size() != permissionResults.size())
        {
            throw new MgXmlParserException(
                L"MgResourceHeaderManager.CreatePermissionInfo",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        while (nameResults.next(nameValue) && permissionResults.next(permissionValue))
        {
            MgUtil::MultiByteToWideChar(nameValue.asString(), name);
            MgUtil::MultiByteToWideChar(permissionValue.asString(), permission);
            permissionInfo->SetUserPermission(name, permission);
        }

        // Get group permissions.

        nameResults = IsTransacted() ?
            selectGroupNames.execute(GetXmlTxn(), xmlDoc, queryContext, 0) :
            selectGroupNames.execute(xmlDoc, queryContext, 0);
        permissionResults = IsTransacted() ?
            selectGroupPermissions.execute(GetXmlTxn(), xmlDoc, queryContext, 0) :
            selectGroupPermissions.execute(xmlDoc, queryContext, 0);

        if (nameResults.size() != permissionResults.size())
        {
            throw new MgXmlParserException(
                L"MgResourceHeaderManager.CreatePermissionInfo",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        while (nameResults.next(nameValue) && permissionResults.next(permissionValue))
        {
            MgUtil::MultiByteToWideChar(nameValue.asString(), name);
            MgUtil::MultiByteToWideChar(permissionValue.asString(), permission);
            permissionInfo->SetGroupPermission(name, permission);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.CreatePermissionInfo")

    return permissionInfo.release();
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the explicit permission information on the specified resource.
/// The returned permission is NOT effectively inherited from a parent.
/// </summary>
///----------------------------------------------------------------------------

const MgPermissionInfo* MgResourceHeaderManager::GetPermissionInfo(
    CREFSTRING resource, bool& local) const
{
    // Get the permission information from the local permission cache.

    const MgPermissionInfo* permissionInfo =
        m_permissionCache->GetPermissionInfo(resource);

    // Get the permission information from the global permission cache.

    if (NULL == permissionInfo)
    {
        permissionInfo = m_permissionMan->GetPermissionInfo(resource);
        local = (NULL == permissionInfo);
    }
    else
    {
        local = true;
    }

    return permissionInfo;
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets all the parents of the specified resource, including or excluding
/// itself.
/// </summary>
///----------------------------------------------------------------------------

INT32 MgResourceHeaderManager::GetParentResources(MgResourceIdentifier& resource,
    XmlResults& docResults, bool inclusive)
{
    INT32 numDocs = 0;

    MG_RESOURCE_SERVICE_TRY()

    INT32 depth = resource.GetDepth();

    if (0 == depth && !inclusive)
    {
        return 0;
    }

    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource.ToString(), resourcePathname);

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/*[dbxml:metadata('dbxml:name')='";
    query += resourcePathname;
    query += "'";

    if (resource.IsFolder() && !resource.IsRoot())
    {
        resourcePathname.erase(resourcePathname.length() - 1);
    }

    while (depth > 0)
    {
        string::size_type index = resourcePathname.rfind('/');

        if (string::npos == index)
        {
            MgStringCollection arguments;
            arguments.Add(resource.ToString());

            throw new MgInvalidResourceNameException(
                L"MgResourceHeaderManager.GetParentResources",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
        {
            query += " or dbxml:metadata('dbxml:name')='";
            query += resourcePathname.erase(index + 1);
            query += "'";

            resourcePathname.erase(index);
        }

        --depth;
    }

    query += "]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();

    docResults = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    numDocs = (INT32)docResults.size();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.GetParentResources")

    return numDocs;
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets all the parents of the specified resource, including or excluding
/// itself.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::GetParentResources(MgResourceIdentifier& resource,
    MgResourceHeaderMap& resourceHeaderMap, INT32 properties, bool inclusive)
{
    MG_RESOURCE_SERVICE_TRY()

    // Get the resources.

    XmlResults docResults;

    if (GetParentResources(resource, docResults, inclusive) <= 0)
    {
        return;
    }

    // Initialize the resource header map.

    bool found = false;
    INT32 depth = resource.GetDepth();
    XmlValue xmlValue;

    while (docResults.next(xmlValue))
    {
        const XmlDocument& xmlDoc = xmlValue.asDocument();
        bool addResourceToMap = true;
        string mbResourceId = xmlDoc.getName();
        STRING wcResourceId;
        MgUtil::MultiByteToWideChar(mbResourceId, wcResourceId);
        MgResourceIdentifier currResource(wcResourceId);

        if (!found &&
            currResource.GetDepth() == depth &&
            currResource.ToString() == resource.ToString())
        {
            found = true;
            addResourceToMap = inclusive;
        }

        // Associate the current resource with a header.

        if (addResourceToMap)
        {
            MgResourceHeader* resourceHeader = new MgResourceHeader();

            resourceHeaderMap.insert(MgResourceHeaderMap::value_type(
                mbResourceId, resourceHeader));

            if (m_repositoryMan.m_currUserIsAdmin)
            {
                resourceHeader->Initialize(currResource, xmlDoc, properties);
            }
            else
            {
                // Update the local permission cache if necessary.

                bool local;
                const MgPermissionInfo* permissionInfo =
                    GetPermissionInfo(wcResourceId, local);

                if (NULL == permissionInfo)
                {
                    MgPermissionInfo* newPermissionInfo = new MgPermissionInfo(m_repositoryMan.m_accessedTime);
                    resourceHeader->Initialize(currResource, xmlDoc, properties, newPermissionInfo);
                    m_permissionCache->SetPermissionInfo(wcResourceId, newPermissionInfo);
                }
                else
                {
                    resourceHeader->Initialize(currResource, xmlDoc, properties);
                }
            }
        }
    }

    if (!found)
    {
        m_repositoryMan.ThrowResourceNotFoundException(resource,
            L"MgResourceHeaderManager.GetParentResources",
            __LINE__, __WFILE__);
    }

    assert(docResults.size() == depth + 1);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.GetParentResources")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the local resource permission maps.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::UpdatePermissionMaps(CREFSTRING resource,
    CREFSTRING permission, MgPermissionInfo::PermissionStatus status)
{
    if (resource.empty())
    {
        throw new MgNullArgumentException(
            L"MgResourceHeaderManager.UpdatePermissionMaps",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgResourcePermissionMap::iterator i;

    if (MgPermissionInfo::Permitted == status)
    {
        if (L'/' == resource[resource.length()-1])
        {
            for (i = m_inaccessibleResources.begin();
                i != m_inaccessibleResources.end();)
            {
                if (STRING::npos == (*i).first.find(resource))
                {
                    ++i;
                }
                else
                {
                    m_inaccessibleResources.erase(i++);
                }
            }
        }
        else
        {
            i = m_inaccessibleResources.find(resource);

            if (m_inaccessibleResources.end() != i)
            {
                m_inaccessibleResources.erase(i);
            }
        }

        if (m_accessibleResources[resource].empty() ||
            MgResourcePermission::ReadWrite == permission)
        {
            m_accessibleResources[resource] = permission;
        }
    }
    else if (MgPermissionInfo::Denied == status)
    {
        if (L'/' == resource[resource.length()-1])
        {
            for (i = m_accessibleResources.begin();
                i != m_accessibleResources.end();)
            {
                if (STRING::npos == (*i).first.find(resource))
                {
                    ++i;
                }
                else
                {
                    m_accessibleResources.erase(i++);
                }
            }
        }
        else
        {
            i = m_accessibleResources.find(resource);

            if (m_accessibleResources.end() != i)
            {
                m_accessibleResources.erase(i);
            }
        }

        if (m_inaccessibleResources[resource].empty() ||
            MgResourcePermission::ReadOnly == permission)
        {
            m_inaccessibleResources[resource] = permission;
        }
    }
    else
    {
        assert(MgPermissionInfo::Unknown == status);

        if (L'/' == resource[resource.length()-1])
        {
            for (i = m_inaccessibleResources.begin();
                i != m_inaccessibleResources.end();)
            {
                if (STRING::npos == (*i).first.find(resource))
                {
                    ++i;
                }
                else
                {
                    m_inaccessibleResources.erase(i++);
                }
            }

            for (i = m_accessibleResources.begin();
                i != m_accessibleResources.end();)
            {
                if (STRING::npos == (*i).first.find(resource))
                {
                    ++i;
                }
                else
                {
                    m_accessibleResources.erase(i++);
                }
            }
        }
        else
        {
            i = m_accessibleResources.find(resource);

            if (m_accessibleResources.end() != i)
            {
                m_accessibleResources.erase(i);
            }

            i =  m_inaccessibleResources.find(resource);

            if (m_inaccessibleResources.end() != i)
            {
                m_inaccessibleResources.erase(i);
            }
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Reads the permission information on the specified document then writes
/// them to the specified permission cache.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionInfo* MgResourceHeaderManager::UpdatePermissionCache(
    XmlDocument& xmlDoc, XmlQueryContext& queryContext,
    XmlQueryExpression& selectInheritedFlags,
    XmlQueryExpression& selectUserNames,
    XmlQueryExpression& selectUserPermissions,
    XmlQueryExpression& selectGroupNames,
    XmlQueryExpression& selectGroupPermissions,
    MgPermissionCache* permissionCache)
{
    assert(NULL != permissionCache);
    auto_ptr<MgPermissionInfo> permissionInfo;

    MG_RESOURCE_SERVICE_TRY()

    permissionInfo.reset(CreatePermissionInfo(xmlDoc, queryContext,
                                              selectInheritedFlags,
                                              selectUserNames, selectUserPermissions,
                                              selectGroupNames, selectGroupPermissions));

    // Get the resource pathname.

    STRING resource;

    MgUtil::MultiByteToWideChar(xmlDoc.getName(), resource);

    // Write the resource permission information to the cache.

    permissionCache->SetPermissionInfo(resource, permissionInfo.get());

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.UpdatePermissionCache")

    return permissionInfo.release();
}

///----------------------------------------------------------------------------
/// <summary>
/// Reads the permission information on the specified document then writes
/// them to the specified permission cache.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionInfo* MgResourceHeaderManager::UpdatePermissionCache(
    XmlDocument& xmlDoc, MgPermissionCache* permissionCache)
{
    // Set up an XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlQueryExpression selectInheritedFlags;
    XmlQueryExpression selectUserNames;
    XmlQueryExpression selectUserPermissions;
    XmlQueryExpression selectGroupNames;
    XmlQueryExpression selectGroupPermissions;

    SetUpPermissionQuery(queryContext,
        selectInheritedFlags,
        selectUserNames, selectUserPermissions,
        selectGroupNames, selectGroupPermissions);

    // Update the permission cache.

    return UpdatePermissionCache(xmlDoc, queryContext,
        selectInheritedFlags,
        selectUserNames, selectUserPermissions,
        selectGroupNames, selectGroupPermissions,
        permissionCache);
}

///----------------------------------------------------------------------------
/// <summary>
/// Reads the permission information on the specified document then writes
/// them to the local permission cache.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::UpdatePermissionCache(UINT32 opId,
    CREFSTRING resource, CREFSTRING permission,
    XmlDocument& xmlDoc, bool permissionChanged)
{
    MG_RESOURCE_SERVICE_TRY()

    MgPermissionInfo* newPermissionInfo = NULL;

    if (permissionChanged)
    {
        newPermissionInfo = UpdatePermissionCache(xmlDoc, m_permissionCache);

        if (MgResourceService::opIdChangeResourceOwner == opId)
        {
            XmlValue xmlValue;

            if (xmlDoc.getMetaData(
                    MgResourceInfo::sm_metadataUri,
                    MgResourceInfo::sm_metadataNames[MgResourceInfo::Owner],
                    xmlValue))
            {
                STRING owner;
                MgUtil::MultiByteToWideChar(xmlValue.asString(), owner);

                newPermissionInfo->SetOwner(owner);
            }
        }
        else if (MgResourceService::opIdInheritPermissionsFrom == opId)
        {
            newPermissionInfo->SetInherited(true);
        }

        UpdatePermissionMaps(resource, permission, MgPermissionInfo::Unknown);
    }
    else
    {
        bool local;

        newPermissionInfo = const_cast<MgPermissionInfo*>(
            GetPermissionInfo(resource, local));

        if (NULL == newPermissionInfo)
        {
            newPermissionInfo = UpdatePermissionCache(xmlDoc, m_permissionCache);
        }
        else if (!local)
        {
            MgPermissionInfo* permissionInfo = new MgPermissionInfo(*newPermissionInfo);
            m_permissionCache->SetPermissionInfo(resource, permissionInfo);
            newPermissionInfo = permissionInfo;
        }

        if (MgResourceService::opIdDeleteResource == opId)
        {
            // Reset the accessed time to mark the resource as deleted.
            newPermissionInfo->SetAccessedTime(0);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.UpdatePermissionCache")
}

///----------------------------------------------------------------------------
/// <summary>
/// Reads the permission information on the specified documents then writes
/// them to the specified permission cache.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::UpdatePermissionCache(XmlResults& docResults,
    INT32 maxSize, MgPermissionCache* permissionCache)
{
    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlQueryExpression selectInheritedFlags;
    XmlQueryExpression selectUserNames;
    XmlQueryExpression selectUserPermissions;
    XmlQueryExpression selectGroupNames;
    XmlQueryExpression selectGroupPermissions;

    SetUpPermissionQuery(queryContext,
        selectInheritedFlags,
        selectUserNames, selectUserPermissions,
        selectGroupNames, selectGroupPermissions);

    // Update the permission cache.

    INT32 count = 0;
    XmlDocument xmlDoc;

    while (count < maxSize && docResults.next(xmlDoc))
    {
        UpdatePermissionCache(xmlDoc, queryContext,
            selectInheritedFlags,
            selectUserNames, selectUserPermissions,
            selectGroupNames, selectGroupPermissions,
            permissionCache);
        ++count;
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.UpdatePermissionCache")
}

///----------------------------------------------------------------------------
/// <summary>
/// Reads the permission information on the specified resources then writes
/// them to the local permission cache.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::UpdatePermissionCache(
    const MgPermissionInfoMap& permissionInfoMap)
{
    MG_RESOURCE_SERVICE_TRY()

    if (permissionInfoMap.empty())
    {
        return;
    }

    // Set up an XQuery.

    int count = 0;
    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/*[";

    for (MgPermissionInfoMap::const_iterator
        i = permissionInfoMap.begin();
        i != permissionInfoMap.end(); ++i)
    {
        if (NULL == (*i).second)
        {
            if (count > 0)
            {
                query += " or ";
            }

            query += "dbxml:metadata('dbxml:name')='";
            query += MgUtil::WideCharToMultiByte((*i).first);
            query += "'";

            ++count;
        }
    }

    query += "]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults docResults = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Update the local pemission cache.

    if (docResults.size() > 0)
    {
        UpdatePermissionCache(docResults, (INT32)docResults.size(), m_permissionCache);
    }
    else
    {
        for (MgPermissionInfoMap::const_iterator
            i = permissionInfoMap.begin();
            i != permissionInfoMap.end(); ++i)
        {
            if (NULL == (*i).second)
            {
                CREFSTRING resource = (*i).first;
                MgPermissionInfo* permissionInfo = GetDefaultPermissionInfo(resource);
                m_permissionCache->SetPermissionInfo(resource, permissionInfo);
            }
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.UpdatePermissionCache")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the global permission cache with the local one.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::UpdatePermissionCache()
{
    MG_TRY()

    MgPermissionManager::UpdatePermissionCache(m_permissionCache);

    MG_CATCH(L"MgResourceHeaderManager.UpdatePermissionCache")
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a new global permission cache.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionCache* MgResourceHeaderManager::CreatePermissionCache()
{
    Ptr<MgPermissionCache> permissionCache;

    MG_RESOURCE_SERVICE_TRY()

    // Create the pemission cache.

    permissionCache = Ptr<MgPermissionCache>(new MgPermissionCache);

    // Execute the XQuery.

    XmlResults docResults = IsTransacted() ?
        m_container.getAllDocuments(GetXmlTxn(), DBXML_LAZY_DOCS) :
        m_container.getAllDocuments(DBXML_LAZY_DOCS);

    // Update the pemission cache.

    INT32 halfCacheSize = m_permissionMan->GetPermissionInfoCacheSize() / 2;

    UpdatePermissionCache(docResults, halfCacheSize, permissionCache);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.CreatePermissionCache")

    return permissionCache.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user owns the specified document.
/// </summary>
///
/// <exceptions>
/// MgPermissionDeniedException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgResourceHeaderManager::CheckOwnership(const string& user,
    XmlDocument& xmlDoc, string& owner, bool strict)
{
    // Do nothing if the current user is an administrator.

    if (m_repositoryMan.m_currUserIsAdmin)
    {
        return true;
    }

    bool userIsOwner = false;

    MG_RESOURCE_SERVICE_TRY()

    // Get the owner.

    XmlValue xmlValue;

    if (xmlDoc.getMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::Owner],
            xmlValue))
    {
        owner = xmlValue.asString();
    }
    else
    {
        owner.clear();
    }

    // Throw an exception if the current user does not own the document.

    userIsOwner = (user == owner);

    if (!userIsOwner && strict)
    {
        MG_LOG_AUTHENTICATION_ENTRY(MgResources::PermissionDenied.c_str());

        MgStringCollection arguments;
        arguments.Add(MgUtil::MultiByteToWideChar(xmlDoc.getName()));

        throw new MgPermissionDeniedException(
            L"MgResourceHeaderManager.CheckOwnership",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.CheckOwnership")

    return userIsOwner;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user owns the specified document.
/// </summary>
///
/// <exceptions>
/// MgPermissionDeniedException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgResourceHeaderManager::CheckOwnership(XmlDocument& xmlDoc, bool strict)
{
    string user, owner;

    MgUtil::WideCharToMultiByte(m_repositoryMan.m_currUserInfo->GetUserName(),
        user);

    return CheckOwnership(user, xmlDoc, owner, strict);
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks against the local resource permission maps to see whether or not
/// the current user has the specified permission on the specified resource.
/// </summary>
///----------------------------------------------------------------------------

bool MgResourceHeaderManager::CheckPermission(CREFSTRING resource,
    CREFSTRING permission)
{
    // Check against the inaccessible resource permission map.

    MgResourcePermissionMap::const_iterator i =
        m_inaccessibleResources.find(resource);

    if (m_inaccessibleResources.end() != i
        && STRING::npos != permission.find((*i).second))
    {
        return false;
    }

    // Check against the accessible resource permission map.

    i = m_accessibleResources.find(resource);

    if (m_accessibleResources.end() != i
        && STRING::npos != (*i).second.find(permission))
    {
        return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// specified resource.
/// </summary>
///
/// <exceptions>
/// MgUnauthorizedAccessException
/// MgPermissionDeniedException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgResourceHeaderManager::CheckPermission(
    MgResourceIdentifier& resource, CREFSTRING permission, bool strict)
{
    // Do nothing if the current user is an administrator.

    if (m_repositoryMan.m_currUserIsAdmin)
    {
        return true;
    }

    bool permitted = false;

    MG_RESOURCE_SERVICE_TRY()

    // Only allow the author to have write access.

    if (MgResourcePermission::ReadWrite == permission
        && !m_repositoryMan.m_currUserIsAuthor)
    {
        if (strict)
        {
            MG_LOG_AUTHENTICATION_ENTRY(MgResources::UnauthorizedAccess.c_str());

            throw new MgUnauthorizedAccessException(
                L"MgResourceHeaderManager.CheckPermission",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            return false;
        }
    }

    // Do nothing if the permission has already been checked for the resource.

    CREFSTRING resourcePathname = resource.ToString();

    if (CheckPermission(resourcePathname, permission))
    {
        return true;
    }

    // Skip permission checks on parent folders if possible.

    STRING::size_type index = 0;
    INT32 startDepth = 0;
    const INT32 depth = resource.GetDepth();

    if (depth > 0)
    {
        CREFSTRING parentPathname = resource.GetFullPath(true);

        if (CheckPermission(parentPathname, permission))
        {
            index = parentPathname.length();
            startDepth = depth;
        }
    }

    // Check the current user's permission.

    CREFSTRING currUser = m_repositoryMan.m_currUserInfo->GetUserName();
    MgPermissionInfoMap permissionInfoMap;

    for (INT32 i = startDepth; i <= depth; ++i)
    {
        if (0 == i)
        {
            index = resourcePathname.find(L"//", index) + 1;
        }
        else
        {
            index = resourcePathname.find(L'/', index);
        }

        STRING currPathname;

        if (STRING::npos == index)
        {
            assert(i == depth);
            currPathname = resourcePathname;
        }
        else
        {
            currPathname = resourcePathname.substr(0, ++index);
        }

        // Check against the local resource permission maps.

        if (!CheckPermission(currPathname, permission))
        {
            // Retrieve permission information from the local/global permission caches.

            bool local;
            const MgPermissionInfo* permissionInfo = GetPermissionInfo(currPathname, local);

            if (NULL == permissionInfo || !permissionInfoMap.empty())
            {
                permissionInfoMap.insert(MgPermissionInfoMap::value_type(
                    currPathname, const_cast<MgPermissionInfo*>(permissionInfo)));
            }
            else
            {
                // Check against the local/global permission caches.

                bool isParent = (i != depth);

                if (m_permissionMan->CheckPermission(currUser, permission,
                    permissionInfo, isParent))
                {
                    UpdatePermissionMaps(currPathname, permission,
                        MgPermissionInfo::Permitted);
                }
                else
                {
                    UpdatePermissionMaps(currPathname, permission,
                        MgPermissionInfo::Denied);

                    if (strict)
                    {
                        MG_LOG_AUTHENTICATION_ENTRY(MgResources::PermissionDenied.c_str());

                        MgStringCollection arguments;
                        arguments.Add(currPathname);

                        throw new MgPermissionDeniedException(
                            L"MgResourceHeaderManager.CheckPermission",
                            __LINE__, __WFILE__, &arguments, L"", NULL);
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }

    if (!permissionInfoMap.empty())
    {
        // Update the local permission cache.

        UpdatePermissionCache(permissionInfoMap);

        for (MgPermissionInfoMap::const_iterator
            i = permissionInfoMap.begin();
            i != permissionInfoMap.end(); ++i)
        {
            CREFSTRING currPathname = (*i).first;
            const MgPermissionInfo* permissionInfo = (*i).second;

            if (NULL == permissionInfo)
            {
                bool local;

                permissionInfo = GetPermissionInfo(currPathname, local);
            }

            // Check against the local/global permission caches.

            bool isParent = (currPathname.length() == resourcePathname.length());

            if (m_permissionMan->CheckPermission(currUser, permission,
                permissionInfo, isParent))
            {
                UpdatePermissionMaps(currPathname, permission,
                    MgPermissionInfo::Permitted);
            }
            else
            {
                UpdatePermissionMaps(currPathname, permission,
                    MgPermissionInfo::Denied);

                if (strict)
                {
                    MG_LOG_AUTHENTICATION_ENTRY(MgResources::PermissionDenied.c_str());

                    MgStringCollection arguments;
                    arguments.Add(currPathname);

                    throw new MgPermissionDeniedException(
                        L"MgResourceHeaderManager.CheckPermission",
                        __LINE__, __WFILE__, &arguments, L"", NULL);
                }
                else
                {
                    return false;
                }
            }
        }
    }

    permitted = true;

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.CheckPermission")

    return permitted;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// parent of the specified resource.
/// </summary>
///
/// <exceptions>
/// MgUnauthorizedAccessException
/// MgPermissionDeniedException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgResourceHeaderManager::CheckParentPermission(
    MgResourceIdentifier& resource, CREFSTRING permission, bool strict)
{
    MgResourceIdentifier parentResource(resource.GetFullPath(true));

    return CheckPermission(parentResource, permission, strict);
}

///----------------------------------------------------------------------------
/// <summary>
/// Validates the document based on the specified resource information.
/// </summary>
///
/// <exceptions>
/// MgInvalidSchemaNameException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::ValidateDocument(MgResourceIdentifier& resource,
    CREFSTRING rootName, CREFSTRING schemaName)
{
    MG_RESOURCE_SERVICE_TRY()

    bool valid = false;

    // TODO: Enforce versioning in future release.
    if (STRING::npos != schemaName.find(rootName))
    {
        if (resource.IsFolder())
        {
            valid = (L"ResourceFolderHeader" == rootName);
        }
        else
        {
            valid = (L"ResourceDocumentHeader" == rootName);
        }
    }

    if (!valid)
    {
        // TODO: Throw a more specific exception?
        throw new MgXmlParserException(L"MgResourceHeaderManager.ValidateDocument",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.ValidateDocument")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets a document from the container.
/// The current user's permission is also checked.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgRepositoryNotFoundException, MgResourceNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------

XmlDocument MgResourceHeaderManager::GetDocument(
    MgResourceIdentifier& resource, CREFSTRING permission)
{
    XmlDocument xmlDoc;

    MG_RESOURCE_SERVICE_TRY()

    // Get the resource.

    if (MgResourceDefinitionManager::GetDocument(resource, xmlDoc, true))
    {
        // Update the local permission cache.

        UpdatePermissionCache(MgResourceService::opIdGetResourceHeader,
            resource.ToString(), permission, xmlDoc, false);

        // Check if the current user is allowed to perform the current operation.

        CheckPermission(resource, permission);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.GetDocument")

    return xmlDoc;
}

///----------------------------------------------------------------------------
/// <summary>
/// Inserts a document into the container.
/// The current user's permission is also checked.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgDuplicateRepositoryException, MgDuplicateResourceException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::PutDocument(MgResourceIdentifier& resource,
    XmlDocument& xmlDoc, XmlUpdateContext& updateContext)
{
    MG_RESOURCE_SERVICE_TRY()

    // Check if the current user is allowed to perform the current operation.

    CheckParentPermission(resource, MgResourcePermission::ReadWrite);

    // Insert the resource.

    MgResourceDefinitionManager::PutDocument(xmlDoc, updateContext);

    // Update the local permission cache.

    UpdatePermissionCache(MgResourceService::opIdSetResource,
        resource.ToString(), MgResourcePermission::ReadWrite, xmlDoc, true);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.PutDocument")
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts an XML string into an XML document.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::TransformDocument(const string& xmlStr,
    XmlDocument& xmlDoc)
{
    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlQueryExpression selectInheritedFlags;
    XmlQueryExpression selectUserNames;
    XmlQueryExpression selectUserPermissions;
    XmlQueryExpression selectGroupNames;
    XmlQueryExpression selectGroupPermissions;

    SetUpPermissionQuery(queryContext,
        selectInheritedFlags,
        selectUserNames, selectUserPermissions,
        selectGroupNames, selectGroupPermissions);

    // Allow only the owner or the Administrator to change the permission
    // information.

    auto_ptr<MgPermissionInfo> oldPermissionInfo(CreatePermissionInfo(
        xmlDoc, queryContext,
        selectInheritedFlags,
        selectUserNames, selectUserPermissions,
        selectGroupNames, selectGroupPermissions));
    assert(NULL != oldPermissionInfo.get());

    xmlDoc.setContent(xmlStr);

    auto_ptr<MgPermissionInfo> newPermissionInfo(CreatePermissionInfo(
        xmlDoc, queryContext,
        selectInheritedFlags,
        selectUserNames, selectUserPermissions,
        selectGroupNames, selectGroupPermissions));
    assert(NULL != newPermissionInfo.get());

    if (*oldPermissionInfo.get() != *newPermissionInfo.get())
    {
        CheckOwnership(xmlDoc);
    }

    const XmlValue modifiedDate = m_repositoryMan.GetAccessedTime();

    xmlDoc.setMetaData(
        MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
        modifiedDate);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.TransformDocument")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates a document in the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgRepositoryNotFoundException, MgResourceNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::UpdateDocument(MgResourceIdentifier& resource,
    XmlDocument& xmlDoc, XmlUpdateContext& updateContext, UINT32 opId)
{
    MG_RESOURCE_SERVICE_TRY()

    // Check if the current user is allowed to perform the current operation.

    if (MgResourceService::opIdMoveResource == opId)
    {
        CheckParentPermission(resource, MgResourcePermission::ReadWrite);
    }
    else
    {
        CheckPermission(resource, MgResourcePermission::ReadWrite);
    }

    // Update the resource.

    MgResourceDefinitionManager::UpdateDocument(xmlDoc, updateContext);

    // Update the local permission cache.

    UpdatePermissionCache(opId, resource.ToString(),
        MgResourcePermission::ReadWrite, xmlDoc, true);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.UpdateDocument")
}

///----------------------------------------------------------------------------
/// <summary>
/// Removes a document from the container.
/// The current user's permission is also checked.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgRepositoryNotFoundException, MgResourceNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::DeleteDocument(MgResourceIdentifier& resource,
    XmlDocument& xmlDoc, XmlUpdateContext& updateContext)
{
    MG_RESOURCE_SERVICE_TRY()

    // Check if the current user is allowed to perform the current operation.

    CheckParentPermission(resource, MgResourcePermission::ReadWrite);

    // Delete the resource.

    MgResourceDefinitionManager::DeleteDocument(xmlDoc, updateContext);

    // Update the local permission cache.

    UpdatePermissionCache(MgResourceService::opIdDeleteResource,
        resource.ToString(), MgResourcePermission::ReadWrite, xmlDoc, false);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.DeleteDocument")
}

///----------------------------------------------------------------------------
/// <summary>
/// Update the modified dates for the specified resources.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeaderManager::UpdateResourceModifiedDates(
    const set<STRING>& resources)
{
    if (resources.empty())
    {
        return;
    }

    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/*[";

    for (set<STRING>::const_iterator i = resources.begin();
        i != resources.end(); ++i)
    {
        if (i != resources.begin())
        {
            query += " or ";
        }

        query += "dbxml:metadata('dbxml:name')='";
        query += MgUtil::WideCharToMultiByte(*i);
        query += "'";
    }

    query += "]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();

    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Update modified dates.

    MgResourceIdentifier currResource;
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();
    const XmlValue accessedTime = m_repositoryMan.GetAccessedTime();
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        XmlDocument xmlDoc = xmlValue.asDocument();
        XmlValue modifiedDate;

        if (xmlDoc.getMetaData(
                MgResourceInfo::sm_metadataUri,
                MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
                modifiedDate)
            && !(modifiedDate == accessedTime))
        {
            xmlDoc.setMetaData(
                MgResourceInfo::sm_metadataUri,
                MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
                accessedTime);

            currResource.SetResource(MgUtil::MultiByteToWideChar(
                xmlDoc.getName()));

            UpdateDocument(currResource, xmlDoc, updateContext,
                MgResourceService::opIdChangeResourceOwner);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.UpdateResourceModifiedDates")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Packages the specified resource.
///
void MgResourceHeaderManager::PackageResource(MgResourceIdentifier& resource,
    MgResourcePackageMaker& packageMaker)
{
    ACE_ASSERT(resource.IsFolder());

    MG_RESOURCE_SERVICE_TRY()

    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource.ToString(), resourcePathname);

    // Set up an XQuery.
    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    string query;

    if (m_repositoryMan.m_currUserIsAdmin)
    {
        queryContext.setEvaluationType(XmlQueryContext::Lazy);
        query = "collection('";
    }
    else
    {
        queryContext.setEvaluationType(XmlQueryContext::Eager);
        query = "for $i in collection('";
    }

    query += m_container.getName();
    query += "')";
    query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
    query += resourcePathname;

    if (m_repositoryMan.m_currUserIsAdmin)
    {
        query += "')]";
    }
    else
    {
        query += "')] order by dbxml:metadata('dbxml:name', $i) return $i";
    }

    // Execute the XQuery.
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Get the resources
    MgResourceIdentifier currResource;
    XmlValue xmlValue;
    INT32 resourceCount = 0;

    while (results.next(xmlValue))
    {
        ++resourceCount;

        const XmlDocument& xmlDoc = xmlValue.asDocument();
        currResource.SetResource(MgUtil::MultiByteToWideChar(xmlDoc.getName()));

        // Check if the current user has a read permission.
        if (CheckPermission(currResource,
            MgResourcePermission::ReadOnly, false))
        {
            // Package the resource header.
            packageMaker.PackageResourceHeader(currResource, xmlDoc);
        }
    }

    if (0 == resourceCount)
    {
        m_repositoryMan.ThrowResourceNotFoundException(resource,
            L"MgResourceHeaderManager.PackageResource",
            __LINE__, __WFILE__);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeaderManager.PackageResource")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Helper methods to write the resource list.
///
void MgResourceHeaderManager::BeginWriteResourceList(string& list)
{
    // This XML follows the ResourceList-1.0.0.xsd schema.

    list += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    list += "<ResourceList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ResourceList-1.0.0.xsd\">\n";
}

void MgResourceHeaderManager::WriteResourceList(string& list, const string& resourcePathname,
    const MgResourceHeaderMap* resourceHeaderMap, MgResourceHeader& resourceHeader,
    INT32 properties, INT32 requiredDepth, INT32 maxDepth)
{
    MgResourceInfo& resourceInfo = resourceHeader.GetResourceInfo();
    MgResourceIdentifier& resource = resourceInfo.GetIdentifier();

    if (resource.IsFolder())
    {
        list += "\t<ResourceFolder>\n";

        list += "\t\t<ResourceId>";
        list += resourcePathname;
        list += "</ResourceId>\n";

        list += "\t\t<Depth>";
        list += resourceHeader.GetMetadata(MgResourceInfo::Depth).asString();
        list += "</Depth>\n";

        list += "\t\t<Owner>";
        list += resourceHeader.GetMetadata(MgResourceInfo::Owner).asString();
        list += "</Owner>\n";

        list += "\t\t<CreatedDate>";
        list += resourceHeader.GetMetadata(MgResourceInfo::CreatedDate).asString();
        list += "</CreatedDate>\n";

        list += "\t\t<ModifiedDate>";
        list += resourceHeader.GetMetadata(MgResourceInfo::ModifiedDate).asString();
        list += "</ModifiedDate>\n";

        INT32 numFolders = resourceHeader.GetNumberOfFolders();
        INT32 numDocuments = resourceHeader.GetNumberOfDocuments();

        if (0 == numFolders && 0 == numDocuments)
        {
            if (NULL == resourceHeaderMap)
            {
                numFolders = numDocuments = -1;
            }
            else if (requiredDepth == maxDepth)
            {
                INT32 currDepth = resource.GetDepth();

                if (currDepth == maxDepth)
                {
                    numFolders = numDocuments = -1;
                }
            }
        }

        string tmpStr;

        list += "\t\t<NumberOfFolders>";
        MgUtil::Int32ToString(numFolders, tmpStr);
        list += tmpStr;
        list += "</NumberOfFolders>\n";

        list += "\t\t<NumberOfDocuments>";
        MgUtil::Int32ToString(numDocuments, tmpStr);
        list += tmpStr;
        list += "</NumberOfDocuments>\n";

        if (0 != properties)
        {
            string header;

            resourceHeader.GetDocument(resourceHeaderMap, header);
            list += header;
        }

        list += "\t</ResourceFolder>\n";
    }
    else
    {
        list += "\t<ResourceDocument>\n";

        list += "\t\t<ResourceId>";
        list += resourcePathname;
        list += "</ResourceId>\n";

        list += "\t\t<Depth>";
        list += resourceHeader.GetMetadata(MgResourceInfo::Depth).asString();
        list += "</Depth>\n";

        list += "\t\t<Owner>";
        list += resourceHeader.GetMetadata(MgResourceInfo::Owner).asString();
        list += "</Owner>\n";

        list += "\t\t<CreatedDate>";
        list += resourceHeader.GetMetadata(MgResourceInfo::CreatedDate).asString();
        list += "</CreatedDate>\n";

        list += "\t\t<ModifiedDate>";
        list += resourceHeader.GetMetadata(MgResourceInfo::ModifiedDate).asString();
        list += "</ModifiedDate>\n";

        if (0 != properties)
        {
            string header;

            resourceHeader.GetDocument(resourceHeaderMap, header);
            list += header;
        }

        list += "\t</ResourceDocument>\n";
    }
}

void MgResourceHeaderManager::EndWriteResourceList(string& list)
{
    list += "</ResourceList>";
}
