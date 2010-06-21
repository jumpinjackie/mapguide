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
#include "ResourcePackageManifestParser.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgResourcePackageManifestParser::MgResourcePackageManifestParser()
{
    m_operations.reserve(256);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgResourcePackageManifestParser::~MgResourcePackageManifestParser()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Parses the specified resource package manifest.
///
void MgResourcePackageManifestParser::Parse(const string& manifest)
{
    assert(!manifest.empty());

    MG_RESOURCE_SERVICE_TRY()

    m_description.clear();
    m_operations.clear();

    m_xmlUtil.reset(new MgXmlUtil(manifest));

    DOMElement* rootNode = m_xmlUtil->GetRootNode();

    m_xmlUtil->GetElementValue(rootNode, "Description", m_description, false);

    DOMNode* opsNode = m_xmlUtil->GetElementNode(rootNode, "Operations");
    DOMNodeList* opNodeList = m_xmlUtil->GetNodeList(opsNode, "Operation");

    if (NULL != opNodeList)
    {
        for (XMLSize_t i = 0; i < opNodeList->getLength(); ++i)
        {
            MgOperationInfo opInfo;
            wstring name, version;
            DOMNode* opNode = opNodeList->item(i);

            m_xmlUtil->GetElementValue(opNode, "Name", name);
            m_xmlUtil->GetElementValue(opNode, "Version", version);

            opInfo.SetName(name);
            opInfo.SetVersion(version);

            DOMNodeList* paramNodeList = m_xmlUtil->GetNodeList(opNode, "Parameter");

            if (NULL != paramNodeList)
            {
                for (XMLSize_t j = 0; j < paramNodeList->getLength(); ++j)
                {
                    MgOperationParameter opParam;
                    wstring name, value, contentType;
                    DOMNode* paramNode = paramNodeList->item(j);

                    m_xmlUtil->GetElementValue(paramNode, "Name", name);
                    m_xmlUtil->GetElementValue(paramNode, "Value", value);
                    m_xmlUtil->GetElementValue(paramNode, "ContentType", contentType, false);

                    opParam.SetValue(value);
                    opParam.SetContentType(contentType);
                    opInfo.AddParameter(name, opParam);
                }
            }

            m_operations.push_back(opInfo);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManifestParser.Parse")
}
