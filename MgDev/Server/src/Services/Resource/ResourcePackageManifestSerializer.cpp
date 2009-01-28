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
#include "ResourcePackageManifestSerializer.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgResourcePackageManifestSerializer::MgResourcePackageManifestSerializer() :
    m_rootNode(NULL),
    m_opsNode(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgResourcePackageManifestSerializer::~MgResourcePackageManifestSerializer()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes the specified resource package manifest.
///
void MgResourcePackageManifestSerializer::Initialize(CREFSTRING packageDescription)
{
    MG_RESOURCE_SERVICE_TRY()

    m_description = packageDescription;

    m_xmlUtil.reset(new MgXmlUtil("ResourcePackageManifest"));
    m_rootNode = m_xmlUtil->GetRootNode();
    ACE_ASSERT(NULL != m_rootNode);

    m_xmlUtil->SetAttribute(m_rootNode, "xmlns:xsi",
        L"http://www.w3.org/2001/XMLSchema-instance");
    m_xmlUtil->SetAttribute(m_rootNode, "xsi:noNamespaceSchemaLocation",
        L"ResourcePackageManifest-1.0.0.xsd");

    if (!packageDescription.empty())
    {
        m_xmlUtil->AddTextNode(m_rootNode, "Description", packageDescription.c_str());
    }

    m_opsNode = m_xmlUtil->AddChildNode(m_rootNode, "Operations");
    ACE_ASSERT(NULL != m_opsNode);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManifestSerializer.Initialize")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes the specified resource package manifest.
///
void MgResourcePackageManifestSerializer::Serialize(const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    ACE_ASSERT(NULL != m_opsNode);
    DOMElement* opNode = m_xmlUtil->AddChildNode(m_opsNode, "Operation");
    ACE_ASSERT(NULL != opNode);

    m_xmlUtil->AddTextNode(opNode, "Name", opInfo.GetName().c_str());
    m_xmlUtil->AddTextNode(opNode, "Version", opInfo.GetVersion().c_str());

    DOMElement* paramsNode = m_xmlUtil->AddChildNode(opNode, "Parameters");
    ACE_ASSERT(NULL != paramsNode);
    const MgOpParamMap& opParams = opInfo.GetParameters();

    for (MgOpParamMap::const_iterator i = opParams.begin(); i != opParams.end(); ++i)
    {
        DOMElement* paramNode = m_xmlUtil->AddChildNode(paramsNode, "Parameter");
        ACE_ASSERT(NULL != paramNode);
        const MgOperationParameter& opParam = (*i).second;

        m_xmlUtil->AddTextNode(paramNode, "Name", (*i).first.c_str());
        m_xmlUtil->AddTextNode(paramNode, "Value", opParam.GetValue().c_str());

        if (!opParam.GetContentType().empty())
        {
            m_xmlUtil->AddTextNode(paramNode, "ContentType", opParam.GetContentType().c_str());
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManifestSerializer.Serialize")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the resource package manifest's DOM document as a byte reader.
///
MgByteReader* MgResourcePackageManifestSerializer::ToByteReader()
{
    return m_xmlUtil->ToReader();
}
