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

#include "ResourceServiceDefs.h"
#include "SessionResourceContentManager.h"
#include "SessionRepositoryManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgSessionResourceContentManager::MgSessionResourceContentManager(
    MgSessionRepositoryManager& repositoryManager) :
    MgApplicationResourceContentManager(repositoryManager)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSessionResourceContentManager::~MgSessionResourceContentManager()
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

MgByteReader* MgSessionResourceContentManager::EnumerateRepositories()
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/*[dbxml:metadata('Metadata:Depth')=xs:double(0)]";

    // Execute the XQuery.

    XmlManager& xmlManager = m_container.getManager();
    XmlQueryContext queryContext = xmlManager.createQueryContext();
    queryContext.setNamespace(MgResourceInfo::sm_metadataPrefix,
        MgResourceInfo::sm_metadataUri);
    XmlResults results = IsTransacted() ?
        xmlManager.query(GetXmlTxn(), query, queryContext, 0) :
        xmlManager.query(query, queryContext, 0);

    // Get the resource and populate the repository list.

    // this XML follows the RepositoryList-1.0.0.xsd schema
    XmlValue xmlValue;
    string list;

    while (results.next(xmlValue)) // TODO: Need an XML writer
    {
        const XmlDocument& xmlDoc = xmlValue.asDocument();
        STRING resourcePathname;
        MgUtil::MultiByteToWideChar(xmlDoc.getName(), resourcePathname);

        MgResourceIdentifier resource(resourcePathname);

        string repositoryName;
        MgUtil::WideCharToMultiByte(resource.GetRepositoryName(),
            repositoryName);

        string content;
        xmlDoc.getContent(content);

        size_t index = content.find("<RepositoryContent");

        if (string::npos != index)
        {
            content.erase(0, index);
        }
        else
        {
            assert(false);
        }

        list += "\t<Repository>\n";

        list += "\t\t<Name>";
        list += repositoryName;
        list += "</Name>\n";

        list += content;

        list += "\t</Repository>\n";
    }

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSessionResourceContentManager.EnumerateRepositories")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes a repository from the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgSessionResourceContentManager::DeleteRepository(
    MgResourceIdentifier* resource)
{
    assert(NULL != resource && resource->IsRoot());

    MG_RESOURCE_SERVICE_TRY()

    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource->ToString(), resourcePathname);

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
    query += resourcePathname;
    query += "')]";

    // Execute the XQuery.

    XmlManager& xmlManager = m_container.getManager();
    XmlQueryContext queryContext = xmlManager.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlManager.query(GetXmlTxn(), query, queryContext, 0) :
        xmlManager.query(query, queryContext, 0);

    if (0 == results.size())
    {
        m_repositoryMan.ThrowResourceNotFoundException(*resource,
            L"MgSessionResourceContentManager.DeleteRepository",
            __LINE__, __WFILE__);
    }

    // Delete the repository.

    MgResourceIdentifier currResource;
    XmlUpdateContext updateContext = xmlManager.createUpdateContext();
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        XmlDocument xmlDoc = xmlValue.asDocument();

        currResource.SetResource(MgUtil::MultiByteToWideChar(xmlDoc.getName()));
        DeleteDocument(currResource, xmlDoc, updateContext);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSessionResourceContentManager.DeleteRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// specified resource.
/// </summary>
///
/// <exceptions>
/// MgPermissionDeniedException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgSessionResourceContentManager::CheckPermission(
    MgResourceIdentifier& resource, CREFSTRING permission, bool strict)
{
    // Do nothing if the current user is an administrator.

    if (m_repositoryMan.m_currUserIsAdmin)
    {
        return true;
    }

    bool permitted = false;

    MG_RESOURCE_SERVICE_TRY()

    permitted = (m_repositoryMan.m_currUserInfo->GetMgSessionId() ==
        resource.GetRepositoryName());

    if (!permitted && strict)
    {
        MG_LOG_AUTHENTICATION_ENTRY(MgResources::PermissionDenied.c_str());

        MgStringCollection arguments;
        arguments.Add(resource.ToString());

        throw new MgPermissionDeniedException(
            L"MgSessionResourceContentManager.CheckPermission",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSessionRepositoryManager.CheckPermission")

    return permitted;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// parent of the specified resource.
/// </summary>
///
/// <exceptions>
/// MgPermissionDeniedException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgSessionResourceContentManager::CheckParentPermission(
    MgResourceIdentifier& resource, CREFSTRING permission, bool strict)
{
    // The resource and its parent have the same permission.

    return CheckPermission(resource, permission, strict);
}
