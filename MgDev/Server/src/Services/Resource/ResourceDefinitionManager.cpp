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
#include "ResourceDefinitionManager.h"
#include "ApplicationRepositoryManager.h"
#include "ResourceContentManager.h"
#include "TagManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceDefinitionManager::MgResourceDefinitionManager(
    MgRepositoryManager& repositoryManager,
    MgResourceContainer& resourceContainer) :
    MgResourceManager(repositoryManager),
    m_container(resourceContainer.GetXmlContainer())
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceDefinitionManager::~MgResourceDefinitionManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines if this is a resource content manager.
/// </summary>
///----------------------------------------------------------------------------

bool MgResourceDefinitionManager::IsResourceContentManager()
{
    return (NULL != dynamic_cast<MgResourceContentManager*>(this));
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

void MgResourceDefinitionManager::DeleteRepository(MgResourceIdentifier* resource)
{
    throw new MgNotImplementedException(
        L"MgResourceDefinitionManager.DeleteRepository",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates an existing repository.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDefinitionManager::UpdateRepository(
    MgResourceInfo& resourceInfo, const string& document)
{
    assert(resourceInfo.GetIdentifier().IsRoot());

    UpdateResource(resourceInfo, document);
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the content or header of the specified repository.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgResourceDefinitionManager::GetRepository(
    MgResourceIdentifier* resource)
{
    assert(NULL != resource && resource->IsRoot());
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Get the resource.

    XmlDocument xmlDoc = GetDocument(*resource);

    // Create a byte reader.

    string document;
    xmlDoc.getContent(document); // TODO: Improve performance by using getContentAsXmlInputStream?

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)document.c_str(), (INT32)document.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_CONTAINER_CATCH_AND_THROW(L"MgResourceDefinitionManager.GetRepository")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes a resource from the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDefinitionManager::DeleteResource(
    MgResourceIdentifier* resource, bool strict)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource->ToString(), resourcePathname);

    // Set up an XQuery.

    string query;

    if (resource->IsFolder())
    {
        query  = "for $i in collection('";
        query += m_container.getName();
        query += "')";
        query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
        query += resourcePathname;
        query += "')]";
        query += " order by dbxml:metadata('dbxml:name', $i) descending return $i";
    }
    else
    {
        query  = "collection('";
        query += m_container.getName();
        query += "')";
        query += "/*[dbxml:metadata('dbxml:name')='";
        query += resourcePathname;
        query += "']";
    }

    // Execute the XQuery.

    XmlManager& xmlManager = m_container.getManager();
    XmlQueryContext queryContext = xmlManager.createQueryContext();
    queryContext.setNamespace(MgResourceInfo::sm_metadataPrefix,
        MgResourceInfo::sm_metadataUri);
    XmlResults results = IsTransacted() ?
        xmlManager.query(GetXmlTxn(), query, queryContext, 0) :
        xmlManager.query(query, queryContext, 0);

    if (0 == results.size())
    {
        if (!strict || (IsResourceContentManager() && resource->IsFolder()))
        {
            return;
        }
        else
        {
            m_repositoryMan.ThrowResourceNotFoundException(*resource,
                L"MgResourceDefinitionManager.DeleteResource",
                __LINE__, __WFILE__);
        }
    }

    // Delete the resources.

    MgResourceIdentifier currResource;
    XmlUpdateContext updateContext = xmlManager.createUpdateContext();
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        XmlDocument xmlDoc = xmlValue.asDocument();

        currResource.SetResource(MgUtil::MultiByteToWideChar(xmlDoc.getName()));

        if (!currResource.IsRoot())
        {
            DeleteDocument(currResource, xmlDoc, updateContext);
        }
    }

    if (!resource->IsRoot())
    {
        m_repositoryMan.UpdateDateModifiedResourceSet(resource->GetFullPath(true));
    }

    MG_RESOURCE_CONTAINER_CATCH_AND_THROW(L"MgResourceDefinitionManager.DeleteResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates a resource in the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDefinitionManager::UpdateResource(
    MgResourceInfo& resourceInfo, const string& document)
{
    MG_RESOURCE_SERVICE_TRY()

    // Get the document.

    MgResourceIdentifier& resource = resourceInfo.GetIdentifier();
    XmlDocument xmlDoc = GetDocument(resource, MgResourcePermission::ReadWrite);

    // Convert an XML string into an XML document.

    TransformDocument(document, xmlDoc);

    // Update the document.

    XmlManager& xmlManager = m_container.getManager();
    XmlUpdateContext updateContext = xmlManager.createUpdateContext();

    UpdateDocument(resource, xmlDoc, updateContext);

    MG_RESOURCE_CONTAINER_CATCH_AND_THROW(L"MgResourceDefinitionManager.UpdateResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets a resource content or header from the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgResourceDefinitionManager::GetResource(
    MgResourceIdentifier* resource, const MgDataBindingInfo* dataBindingInfo)
{
    assert(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Get the resource.

    XmlDocument xmlDoc = GetDocument(*resource);
    string document;

    xmlDoc.getContent(document);

    // Bind the data if required.

    if (NULL != dataBindingInfo)
    {
        // Substitute all the tags if necessary.

        XmlValue tagValue;

        if (xmlDoc.getMetaData(MgResourceInfo::sm_metadataUri,
                MgResourceInfo::sm_metadataNames[MgResourceInfo::Tags],
                tagValue))
        {
            STRING resourceTags;
            MgUtil::MultiByteToWideChar(tagValue.asString(), resourceTags);

            // If there is no resource tag, then just do the substitution for
            // the login username and password.

            MgTagManager tagManager(resourceTags);
            tagManager.SubstituteTags(*dataBindingInfo, document);
        }
    }

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)document.c_str(), (INT32)document.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_CONTAINER_CATCH_AND_THROW(L"MgResourceDefinitionManager.GetResource")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines if the specified resource exists.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgResourceDefinitionManager::FindResource(const string& mbResourcePathname)
{
    bool found = false;

    MG_RESOURCE_SERVICE_TRY()

    XmlManager& xmlManager = m_container.getManager();
    XmlQueryContext queryContext = xmlManager.createQueryContext();
    XmlResults results = IsTransacted() ?
        m_container.lookupIndex(GetXmlTxn(), queryContext,
            DbXml::metaDataNamespace_uri, DbXml::metaDataName_name,
            "unique-node-metadata-equality-string",
            XmlValue(mbResourcePathname), 0) :
        m_container.lookupIndex(queryContext,
            DbXml::metaDataNamespace_uri, DbXml::metaDataName_name,
            "unique-node-metadata-equality-string",
            XmlValue(mbResourcePathname), 0);

    if (results.size() > 0)
    {
        assert(1 == results.size());
        found = true;
    }

    MG_RESOURCE_CONTAINER_CATCH_AND_THROW(L"MgResourceDefinitionManager.FindResource")

    return found;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines if the specified resource exists.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgResourceDefinitionManager::FindResource(CREFSTRING wcResourcePathname)
{
    string mbResourcePathname;
    MgUtil::WideCharToMultiByte(wcResourcePathname, mbResourcePathname);

    return FindResource(mbResourcePathname);
}


///----------------------------------------------------------------------------
/// <summary>
/// Validates the specified document.
/// </summary>
///
/// <exceptions>
/// MgInvalidSchemaNameException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDefinitionManager::ValidateDocument(XmlDocument& xmlDoc)
{
    MG_RESOURCE_SERVICE_TRY()

    MgResourceIdentifier resource(MgUtil::MultiByteToWideChar(xmlDoc.getName()));

    // Skip XML schema validation on runtime resources.

    if (!resource.IsRuntimeResource())
    {
        DOMDocument* domDoc = xmlDoc.getContentAsDOM();
        assert(NULL != domDoc);
        MgXmlUtil xmlUtil(domDoc, false);
        DOMElement* rootNode = xmlUtil.GetRootNode();
        assert(NULL != rootNode);

        STRING rootName;
        const XMLCh* tag = rootNode->getTagName();

        if (NULL != tag)
        {
            rootName = X2W(tag);
            assert(!rootName.empty());
        }

        STRING schemaName;
        const XMLCh* attr = rootNode->getAttribute(X("xsi:noNamespaceSchemaLocation"));

        if (NULL != attr)
        {
            schemaName = X2W(attr);
        }

        ValidateDocument(resource, rootName, schemaName);
    }

    MG_RESOURCE_CONTAINER_CATCH_AND_THROW(L"MgResourceDefinitionManager.ValidateDocument")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets a document from the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgRepositoryNotFoundException, MgResourceNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgResourceDefinitionManager::GetDocument(const string& resourcePathname,
    XmlDocument& xmlDoc, bool strict)
{
    bool found = false;

    MG_RESOURCE_SERVICE_TRY()

    try
    {
        // Get the resource.

        if (IsTransacted())
        {
            xmlDoc = m_container.getDocument(GetXmlTxn(), resourcePathname, 0);
        }
        else
        {
            xmlDoc = m_container.getDocument(resourcePathname, 0);
        }

        found = true;
    }
    catch (XmlException& e)
    {
        if (XmlException::DOCUMENT_NOT_FOUND == e.getExceptionCode())
        {
            if (strict)
            {
                MgResourceIdentifier resource(
                    MgUtil::MultiByteToWideChar(resourcePathname));

                m_repositoryMan.ThrowResourceNotFoundException(resource,
                    L"MgResourceDefinitionManager.GetDocument",
                    __LINE__, __WFILE__);
            }
        }
        else
        {
            throw e;
        }
    }

    MG_RESOURCE_CONTAINER_CATCH_AND_THROW(L"MgResourceDefinitionManager.GetDocument")

    return found;
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets a document from the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgRepositoryNotFoundException, MgResourceNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgResourceDefinitionManager::GetDocument(MgResourceIdentifier& resource,
    XmlDocument& xmlDoc, bool strict)
{
    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource.ToString(), resourcePathname);

    return GetDocument(resourcePathname, xmlDoc, strict);
}

///----------------------------------------------------------------------------
/// <summary>
/// Inserts a document into the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgDuplicateRepositoryException, MgDuplicateResourceException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDefinitionManager::PutDocument(XmlDocument& xmlDoc,
    XmlUpdateContext& updateContext)
{
    MG_RESOURCE_SERVICE_TRY()

    try
    {
        // Ensure the current operation is transactionally protected.
        m_repositoryMan.ValidateTransaction();

        // Perform the XML validation.
        ValidateDocument(xmlDoc);

        // Insert the resource.
        m_container.putDocument(GetXmlTxn(), xmlDoc, updateContext, 0);
    }
    catch (XmlException& e)
    {
        if (XmlException::UNIQUE_ERROR == e.getExceptionCode())
        {
            MgResourceIdentifier resource(
                MgUtil::MultiByteToWideChar(xmlDoc.getName()));

            m_repositoryMan.ThrowDuplicateResourceException(resource,
                L"MgResourceDefinitionManager.PutDocument",
                __LINE__, __WFILE__);
        }
        else
        {
            throw e;
        }
    }

    MG_RESOURCE_CONTAINER_CATCH_AND_THROW(L"MgResourceDefinitionManager.PutDocument")
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

void MgResourceDefinitionManager::UpdateDocument(XmlDocument& xmlDoc,
    XmlUpdateContext& updateContext)
{
    MG_RESOURCE_SERVICE_TRY()

    try
    {
        // Ensure the current operation is transactionally protected.
        m_repositoryMan.ValidateTransaction();

        // Perform the XML validation.
        ValidateDocument(xmlDoc);

        // Update the resource.
        m_container.updateDocument(GetXmlTxn(), xmlDoc, updateContext);
    }
    catch (XmlException& e)
    {
        if (XmlException::DOCUMENT_NOT_FOUND == e.getExceptionCode())
        {
            MgResourceIdentifier resource(
                MgUtil::MultiByteToWideChar(xmlDoc.getName()));

            m_repositoryMan.ThrowResourceNotFoundException(resource,
                L"MgResourceDefinitionManager.UpdateDocument",
                __LINE__, __WFILE__);
        }
        else
        {
            throw e;
        }
    }

    MG_RESOURCE_CONTAINER_CATCH_AND_THROW(L"MgResourceDefinitionManager.UpdateDocument")
}

///----------------------------------------------------------------------------
/// <summary>
/// Removes a document from the container.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgRepositoryNotFoundException, MgResourceNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDefinitionManager::DeleteDocument(XmlDocument& xmlDoc,
    XmlUpdateContext& updateContext)
{
    MG_RESOURCE_SERVICE_TRY()

    try
    {
        // Ensure the current operation is transactionally protected.
        m_repositoryMan.ValidateTransaction();

        // Delete the resource.
        m_container.deleteDocument(GetXmlTxn(), xmlDoc, updateContext);
    }
    catch (XmlException& e)
    {
        if (XmlException::DOCUMENT_NOT_FOUND == e.getExceptionCode())
        {
            MgResourceIdentifier resource(
                MgUtil::MultiByteToWideChar(xmlDoc.getName()));

            m_repositoryMan.ThrowResourceNotFoundException(resource,
                L"MgResourceDefinitionManager.DeleteDocument",
                __LINE__, __WFILE__);
        }
        else
        {
            throw e;
        }
    }

    MG_RESOURCE_CONTAINER_CATCH_AND_THROW(L"MgResourceDefinitionManager.DeleteDocument")
}


///----------------------------------------------------------------------------
/// <summary>
/// Formats the method name to include the repository type.
/// </summary>
///
///----------------------------------------------------------------------------

STRING MgResourceDefinitionManager::FormatMethodName(CREFSTRING methodName)
{
    STRING errorText = methodName;
    errorText.append(L" (");
    errorText.append(MgUtil::MultiByteToWideChar(m_container.getName()));
    errorText.append(L")");
    return errorText;
}
