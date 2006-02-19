//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
    MG_RESOURCE_SERVICE_TRY()

    throw new MgInvalidOperationException(
        L"MgResourceContentManager.EnumerateRepositories",
        __LINE__, __WFILE__, NULL, L"", NULL);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.EnumerateRepositories")

    return NULL;
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

    // TODO: Enforce versioning in future release.
    if (STRING::npos != schemaName.find(rootName))
    {
        if (resource.IsRoot())
        {
            valid = (L"RepositoryContent" == rootName);
        }
        else if (!resource.IsFolder())
        {
            valid = (resource.GetResourceType() == rootName);
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

    DeleteResourceData(xmlDoc);

    // Delete the resource.

    MgResourceDefinitionManager::DeleteDocument(xmlDoc, updateContext);

    // Update the changed resource set.

    m_repositoryMan.UpdateChangedResourceSet(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceContentManager.DeleteDocument")
}
