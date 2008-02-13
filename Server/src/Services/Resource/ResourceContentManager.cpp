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

#include "ResourceServiceDefs.h"
#include "ResourceContentManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceContentManager::MgResourceContentManager(
    MgRepositoryManager& repositoryMan) :
    MgResourceDefinitionManager(repositoryMan,
        *repositoryMan.GetRepository().GetResourceContentContainer())
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceContentManager::~MgResourceContentManager()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Enumerates the roots for this container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgResourceContentManager::EnumerateRepositories()
{
    throw new MgNotImplementedException(
        L"MgResourceContentManager.EnumerateRepositories",
        __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL; // to make some compiler happy
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a repository content to the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceContentManager::AddRepository(MgResourceInfo& resourceInfo,
    const string& document)
{
    assert(resourceInfo.GetIdentifier().IsRoot());

    MG_RESOURCE_SERVICE_TRY()

    MgResourceIdentifier& resource = resourceInfo.GetIdentifier();
    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource.ToString(), resourcePathname);

    // Set the repository metadata.

    XmlManager& xmlMan = m_container.getManager();
    XmlDocument xmlDoc = xmlMan.createDocument();

    xmlDoc.setName(resourcePathname);
    xmlDoc.setMetaData(
        MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::Depth],
        resourceInfo.GetMetadata(MgResourceInfo::Depth));
    xmlDoc.setMetaData(
        MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::Tags],
        resourceInfo.GetMetadata(MgResourceInfo::Tags));

    // Use a default repository content if it is not specified.

    if (document.empty()) // TODO: Need an XML writer
    {
        string defaultDoc;

        // this XML follows the RepositoryContent-1.0.0.xsd schema
        defaultDoc  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        defaultDoc += "<RepositoryContent xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"RepositoryContent-1.0.0.xsd\">\n";
        defaultDoc += "</RepositoryContent>";

        xmlDoc.setContent(defaultDoc);
    }
    else
    {
        xmlDoc.setContent(document);
    }

    // Add the repository content.

    XmlUpdateContext updateContext = xmlMan.createUpdateContext();

    PutDocument(resource, xmlDoc, updateContext);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.AddRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a resource content to the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceContentManager::AddResource(MgResourceInfo& resourceInfo,
    const string& document)
{
    assert(!resourceInfo.GetIdentifier().IsFolder());

    MG_RESOURCE_SERVICE_TRY()

    // Check if the resource content is empty.

    if (document.empty())
    {
        throw new MgNullArgumentException(
            L"MgResourceContentManager.AddResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

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
        MgResourceInfo::sm_metadataNames[MgResourceInfo::Tags],
        resourceInfo.GetMetadata(MgResourceInfo::Tags));

    xmlDoc.setContent(document);

    // Add the resource content.

    XmlUpdateContext updateContext = xmlMan.createUpdateContext();

    PutDocument(resource, xmlDoc, updateContext);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.AddResource")
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Renames a resource and/or moves it from one location to another.
///
void MgResourceContentManager::MoveResource(
    MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource,
    bool overwrite)
{
    assert(NULL != sourceResource && NULL != destResource);

    MG_RESOURCE_SERVICE_TRY()

    // Check whether or not the destination resource already exists.

    if (!overwrite && FindResource(destResource->ToString()))
    {
        m_repositoryMan.ThrowDuplicateResourceException(*destResource,
            L"MgResourceContentManager.MoveResource",
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
        if (sourceResourceIsFolder)
        {
            return;
        }
        else
        {
            m_repositoryMan.ThrowResourceNotFoundException(*sourceResource,
                L"MgResourceContentManager.MoveResource",
                __LINE__, __WFILE__);
        }
    }

    // Move/rename the resources.

    XmlUpdateContext updateContext = xmlManager.createUpdateContext();
    const size_t sourcePathLength = sourcePathname.length();
    XmlValue xmlValue;

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

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.MoveResource")
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Copies an existing resource to another location.
///
void MgResourceContentManager::CopyResource(
    MgResourceIdentifier* sourceResource, MgResourceIdentifier* destResource,
    bool overwrite)
{
    assert(NULL != sourceResource && NULL != destResource);

    MG_RESOURCE_SERVICE_TRY()

    // Check whether or not the destination resource already exists.

    if (!overwrite && FindResource(destResource->ToString()))
    {
        m_repositoryMan.ThrowDuplicateResourceException(*destResource,
            L"MgResourceContentManager.CopyResource",
            __LINE__, __WFILE__);
    }

    // Get the source MgResourceContentManager.

    MgApplicationRepositoryManager* sourceRepositoryMan =
        dynamic_cast<MgApplicationRepositoryManager*>(
        ((MgApplicationRepositoryManager&)m_repositoryMan).GetSourceRepositoryManager());
    ACE_ASSERT(NULL != sourceRepositoryMan);
    MgResourceContentManager* sourceResourceContentMan =
        sourceRepositoryMan->GetResourceContentManager();

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
        query += sourceResourceContentMan->m_container.getName();
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
        query += sourceResourceContentMan->m_container.getName();
        query += "')";
        query += "/*[dbxml:metadata('dbxml:name')='";
        query += sourcePathname;
        query += "']";
    }

    // Execute the XQuery.

    XmlManager& sourceXmlMan = sourceResourceContentMan->m_container.getManager();
    XmlQueryContext queryContext = sourceXmlMan.createQueryContext();
    XmlResults results = sourceResourceContentMan->IsTransacted() ?
        sourceXmlMan.query(sourceResourceContentMan->GetXmlTxn(), query, queryContext, 0) :
        sourceXmlMan.query(query, queryContext, 0);

    if (0 == results.size())
    {
        if (sourceResourceIsFolder)
        {
            return;
        }
        else
        {
            sourceRepositoryMan->ThrowResourceNotFoundException(*sourceResource,
                L"MgResourceContentManager.CopyResource",
                __LINE__, __WFILE__);
        }
    }

    // Get the source and destination MgResourceHeaderManager's.

    MgResourceHeaderManager* sourceResourceHeaderMan =
        sourceRepositoryMan->GetResourceHeaderManager();
    MgResourceHeaderManager* destResourceHeaderMan =
        m_repositoryMan.GetResourceHeaderManager();
    bool addResourceHeaders = (NULL == sourceResourceHeaderMan && NULL != destResourceHeaderMan);

    // Copy the resources.

    XmlManager& destXmlMan = m_container.getManager();
    XmlUpdateContext updateContext = destXmlMan.createUpdateContext();
    const size_t sourcePathLength = sourcePathname.length();
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        const XmlDocument& currDoc = xmlValue.asDocument();
        string currPathname = currDoc.getName();
        MgResourceIdentifier currResource(MgUtil::MultiByteToWideChar(currPathname));

        sourceResourceContentMan->CheckPermission(currResource, MgResourcePermission::ReadOnly);

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

        if (overwrite || addResourceHeaders)
        {
            XmlDocument oldDoc;

            if (MgResourceDefinitionManager::GetDocument(currPathname, oldDoc))
            {
                if (overwrite)
                {
                    DeleteDocument(currResource, oldDoc, updateContext);
                    overwritten = true;
                }
                else
                {
                    m_repositoryMan.ThrowDuplicateResourceException(currResource,
                        L"MgResourceContentManager.CopyResource",
                        __LINE__, __WFILE__);
                }
            }
            else if (addResourceHeaders && currPathname != destPathname)
            {
                MgResourceInfo resourceInfo(currResource,
                    m_repositoryMan.m_currUserInfo, m_repositoryMan.m_accessedTime);
                string defaultDoc;

                destResourceHeaderMan->AddParentResources(resourceInfo, defaultDoc);
                destResourceHeaderMan->AddResource(resourceInfo, defaultDoc);
            }
        }

        XmlDocument newDoc = destXmlMan.createDocument();

        newDoc.setName(currPathname);
        newDoc.setMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::Depth],
            XmlValue(static_cast<double>(currResource.GetDepth())));

        XmlValue tagValue;
        STRING destResourceTags;

        if (currDoc.getMetaData(MgResourceInfo::sm_metadataUri,
                MgResourceInfo::sm_metadataNames[MgResourceInfo::Tags],
                tagValue))
        {
            STRING sourceResourceTags;
            MgUtil::MultiByteToWideChar(tagValue.asString(), sourceResourceTags);

            m_repositoryMan.CopyResourceData(sourceResourceTags,
                destResourceTags, overwrite);
        }

        tagValue = XmlValue(MgUtil::WideCharToMultiByte(destResourceTags));
        newDoc.setMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::Tags],
            tagValue);

        newDoc.setContentAsXmlInputStream(currDoc.getContentAsXmlInputStream());

        PutDocument(currResource, newDoc, updateContext);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.CopyResource")
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

void MgResourceContentManager::ValidateDocument(MgResourceIdentifier& resource,
    CREFSTRING rootName, CREFSTRING schemaName)
{
    MG_RESOURCE_SERVICE_TRY()

    bool valid = false;

    // The symbol definition schema contains two root elements whose
    // names don't exactly match the schema name and resource type.
    // We therefore require special handling for symbol definition
    // resources.
    STRING resourceType = resource.GetResourceType();
    if (resourceType == MgResourceType::SymbolDefinition)
    {
        // verify the schema name matches the resource type
        if (STRING::npos != schemaName.find(resourceType))
        {
            // verify the rootname is one of the symbol definition elements
            if (STRING::npos != rootName.find(resourceType))
            {
                valid = true;
            }
        }
    }
    else
    {
        // TODO: Enforce versioning in future release.
        if (STRING::npos != schemaName.find(rootName))
        {
            if (resource.IsRoot())
            {
                valid = (L"RepositoryContent" == rootName);
            }
            else if (!resource.IsFolder())
            {
                valid = (resourceType == rootName);
            }
        }
    }

    if (!valid)
    {
        // TODO: Throw a more specific exception?
        throw new MgXmlParserException(L"MgResourceContentManager.ValidateDocument",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.ValidateDocument")
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

XmlDocument MgResourceContentManager::GetDocument(
    MgResourceIdentifier& resource, CREFSTRING permission)
{
    XmlDocument xmlDoc;

    MG_RESOURCE_SERVICE_TRY()

    // Get the resource.

    if (MgResourceDefinitionManager::GetDocument(resource, xmlDoc, true))
    {
        // Check if the current user is allowed to perform the current operation.

        CheckPermission(resource, permission);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.GetDocument")

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

void MgResourceContentManager::PutDocument(MgResourceIdentifier& resource,
    XmlDocument& xmlDoc, XmlUpdateContext& updateContext)
{
    MG_RESOURCE_SERVICE_TRY()

    // Check if the current user is allowed to perform the current operation.

    CheckParentPermission(resource, MgResourcePermission::ReadWrite);

    // Insert the resource.

    MgResourceDefinitionManager::PutDocument(xmlDoc, updateContext);

    // Update the changed resource set.

    m_repositoryMan.UpdateChangedResourceSet(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.PutDocument")
}

///----------------------------------------------------------------------------
/// <summary>
/// Converts an XML string into an XML document.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceContentManager::TransformDocument(const string& xmlStr,
    XmlDocument& xmlDoc)
{
    xmlDoc.setContent(xmlStr);
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

void MgResourceContentManager::UpdateDocument(MgResourceIdentifier& resource,
    XmlDocument& xmlDoc, XmlUpdateContext& updateContext, UINT32 opId)
{
    MG_RESOURCE_SERVICE_TRY()

    // Check if the current user is allowed to perform the current operation.

    CheckPermission(resource, MgResourcePermission::ReadWrite);

    // Update the resource.

    MgResourceDefinitionManager::UpdateDocument(xmlDoc, updateContext);

    // Update the changed resource set.

    m_repositoryMan.UpdateChangedResourceSet(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.UpdateDocument")
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

void MgResourceContentManager::DeleteDocument(MgResourceIdentifier& resource,
    XmlDocument& xmlDoc, XmlUpdateContext& updateContext)
{
    MG_RESOURCE_SERVICE_TRY()

    // Check if the current user is allowed to perform the current operation.

    CheckParentPermission(resource, MgResourcePermission::ReadWrite);

    // Delete the resource data.

    DeleteResourceData(&resource, xmlDoc);

    // Delete the resource.

    MgResourceDefinitionManager::DeleteDocument(xmlDoc, updateContext);

    // Update the changed resource set.

    m_repositoryMan.UpdateChangedResourceSet(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.DeleteDocument")
}
