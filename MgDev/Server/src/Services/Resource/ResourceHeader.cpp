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
#include "ResourceHeader.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceHeader::MgResourceHeader() :
    m_properties(0),
    m_numFolders(0),
    m_numDocuments(0),
    m_inherited(false),
    m_securityInfoFromParent(false)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceHeader::~MgResourceHeader()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Initializes the resource header. This method must be called once and before
/// most of other methods.
/// </summary>
///
/// <exceptions>
/// MgInvalidOperationException
/// MgXmlParserException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeader::Initialize(
    const MgResourceIdentifier& resourceIdentifier, const XmlDocument& xmlDoc,
    INT32 properties, MgPermissionInfo* permissionInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    if (NULL != m_xmlUtil.get() || NULL != m_resourceInfo.get())
    {
        throw new MgInvalidOperationException(L"MgResourceHeader.Initialize",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Set the flag specifying which properties of the resource header should be filtered.
    // It may be set to 0 or by bitwise inclusively OR'ing together one or
    // more of the MgResourceHeaderProperties values.

    m_properties = properties;

    // Load the XML document.

    string xmlContent;

    m_resourceInfo.reset(new MgResourceInfo(resourceIdentifier, xmlDoc));
    m_xmlUtil.reset(new MgXmlUtil(xmlDoc.getContent(xmlContent)));

    // Get the Inherited flag.

    DOMElement* rootNode = m_xmlUtil->GetRootNode();
    DOMNode* securityNode = m_xmlUtil->GetElementNode(rootNode, "Security");

    m_xmlUtil->GetElementValue(securityNode, "Inherited", m_inherited);

    // Initialize the permission information if required.

    if (NULL != permissionInfo)
    {
        STRING owner;

        MgUtil::MultiByteToWideChar(m_resourceInfo->GetMetadata(
            MgResourceInfo::Owner).asString(), owner);

        permissionInfo->SetOwner(owner);
        permissionInfo->SetInherited(m_inherited);

        if (!m_inherited)
        {
            DOMNodeList* userNodes = m_xmlUtil->GetNodeList(securityNode, "User");

            if (NULL != userNodes)
            {
                for (XMLSize_t i = 0; i < userNodes->getLength(); ++i)
                {
                    STRING name, permissions;
                    DOMNode* userNode = userNodes->item(i);

                    m_xmlUtil->GetElementValue(userNode, "Name", name);
                    m_xmlUtil->GetElementValue(userNode, "Permissions", permissions);

                    permissionInfo->SetUserPermission(name, permissions);
                }
            }

            DOMNodeList* groupNodes = m_xmlUtil->GetNodeList(securityNode, "Group");

            if (NULL != groupNodes)
            {
                for (XMLSize_t i = 0; i < groupNodes->getLength(); ++i)
                {
                    STRING name, permissions;
                    DOMNode* groupNode = groupNodes->item(i);

                    m_xmlUtil->GetElementValue(groupNode, "Name", name);
                    m_xmlUtil->GetElementValue(groupNode, "Permissions", permissions);

                    permissionInfo->SetGroupPermission(name, permissions);
                }
            }
        }
    }

    // Remove unwanted properties appropriately.

    if (!IsFilteredOn(MgResourceHeaderProperties::General))
    {
        DOMNode* generalNode = m_xmlUtil->GetElementNode(rootNode, "General", false);

        if (NULL != generalNode)
        {
            generalNode = rootNode->removeChild(generalNode);
        }
    }

    if (!IsFilteredOn(MgResourceHeaderProperties::Security))
    {
        assert(NULL != securityNode);
        securityNode = rootNode->removeChild(securityNode);
    }

    if (!IsFilteredOn(MgResourceHeaderProperties::Metadata))
    {
        DOMNode* metadataNode = m_xmlUtil->GetElementNode(rootNode, "Metadata", false);

        if (NULL != metadataNode)
        {
            metadataNode = rootNode->removeChild(metadataNode);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeader.Initialize")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return the resource info.
///
MgResourceInfo& MgResourceHeader::GetResourceInfo()
{
    if (NULL == m_resourceInfo.get())
    {
        throw new MgNullReferenceException(L"MgResourceHeader.GetResourceInfo",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return *m_resourceInfo.get();
}

///----------------------------------------------------------------------------
/// <summary>
/// Increments the number of sub-folders contained in the resource folder.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeader::IncrementNumberOfFolders()
{
    ++m_numFolders;
}

///----------------------------------------------------------------------------
/// <summary>
/// Increments the number of documents contained in the resource folder.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceHeader::IncrementNumberOfDocuments()
{
    ++m_numDocuments;
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the specified metadata.
/// </summary>
///
/// <exceptions>
/// MgInvalidOperationException
/// MgDbXmlException
/// MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

const XmlValue& MgResourceHeader::GetMetadata(
    MgResourceInfo::Attribute attr) const
{
    CheckInitialization();

    return m_resourceInfo->GetMetadata(attr);
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the document that will be updated if its security information is inherited.
/// </summary>
///
/// <exceptions>
/// MgInvalidOperationException
/// MgDbXmlException
/// MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeader::GetDocument(const MgResourceHeaderMap* resourceHeaderMap,
    string& document)
{
    MG_RESOURCE_SERVICE_TRY()

    CheckInitialization();

    // Update the document if its security information is inherited.

    if (NULL != resourceHeaderMap
        && IsFilteredOn(MgResourceHeaderProperties::Security))
    {
        MgResourceIdentifier resourceIdentifier(m_resourceInfo->GetIdentifier());
        MgResourceHeader* resourceHeader = this;
        vector<MgResourceHeader*> resourceHeaderVector;

        resourceHeaderVector.reserve(resourceIdentifier.GetDepth() + 1);

        while (resourceHeader->IsSecurityInfoInherited()
            && !resourceHeader->IsSecurityInfoFromParent()
            && !resourceIdentifier.IsRoot())
        {
            resourceHeaderVector.push_back(resourceHeader);

            STRING wcParentId = resourceIdentifier.GetFullPath(true);
            string mbParentId;

            MgUtil::WideCharToMultiByte(wcParentId, mbParentId);
            resourceIdentifier.SetResource(wcParentId);

            MgResourceHeaderMap::const_iterator i = resourceHeaderMap->find(
                mbParentId);

            if (resourceHeaderMap->end() != i)
            {
                resourceHeader = (*i).second;
            }
            else
            {
                MgStringCollection arguments;
                arguments.Add(wcParentId);

                throw new MgResourceNotFoundException(
                    L"MgResourceHeader.GetDocument",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }

        if (resourceHeaderVector.size() > 0)
        {
            const DOMElement* securityNode = resourceHeader->GetSecurity();

            for (size_t index = 0; index < resourceHeaderVector.size(); ++index)
            {
                resourceHeaderVector[index]->SetSecurity(securityNode);
            }
        }
    }

    // Get the document.

    m_xmlUtil->ToStringUtf8(document);
    size_t index = document.find("<Resource");

    if (string::npos != index)
    {
        document.erase(0, index);
    }
    else
    {
        throw new MgXmlParserException(L"MgResourceHeader.GetDocument",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeader.GetDocument")
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks if the resource header has been initialized.
/// </summary>
///
/// <exceptions>
/// MgNullReferenceException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeader::CheckInitialization() const
{
    if (NULL == m_xmlUtil.get() || NULL == m_resourceInfo.get())
    {
        throw new MgNullReferenceException(
            L"MgResourceHeader.CheckInitialization", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Extracts the security snippet from the resource header.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

const DOMElement* MgResourceHeader::GetSecurity() const
{
    CheckInitialization();

    if (!IsFilteredOn(MgResourceHeaderProperties::Security))
    {
        throw new MgInvalidOperationException(L"MgResourceHeader.GetSecurity",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return m_xmlUtil->GetElementNode(m_xmlUtil->GetRootNode(), "Security");
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the resource header with the specified security snippet.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceHeader::SetSecurity(const DOMElement* securityNode)
{
    assert(NULL != securityNode);

    MG_RESOURCE_SERVICE_TRY()

    CheckInitialization();

    if (!IsFilteredOn(MgResourceHeaderProperties::Security))
    {
        throw new MgInvalidOperationException(L"MgResourceHeader.SetSecurity",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Clone a new Security node from the parent.

    DOMDocument* domDoc = m_xmlUtil->GetDocument();
    assert(NULL != domDoc);

    DOMNode* oldNode = (DOMNode*)GetSecurity();
    assert(NULL != oldNode);
    DOMNode* newNode = domDoc->importNode((DOMNode*)securityNode, true);
    assert(NULL != newNode);

    // Toggle the Inherited flag.

    assert(m_inherited);
    m_xmlUtil->SetElementValue(newNode, "Inherited", L"true");

    // Replace the old Security node with the new one.

    DOMElement* rootNode = m_xmlUtil->GetRootNode();
    assert(NULL != rootNode);

    rootNode->replaceChild(newNode, oldNode);
    oldNode->release();

    m_securityInfoFromParent = true;

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceHeader.SetSecurity")
}
