//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#include "HttpHandler.h"
#include "HttpEnumerateApplicationContainers.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpEnumerateApplicationContainers)

const STRING WIDGET_TYPE_CONTAINER = L"CONTAINER";
const STRING WIDGET_TYPE_COMMAND = L"COMMAND";

/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
/// <param name="name">Input
/// MgHttpRequest
/// This contains all the parameters of the request.
/// </param>
/// <returns>
/// nothing
/// </returns>
MgHttpEnumerateApplicationContainers::MgHttpEnumerateApplicationContainers(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Get response format
    m_format = hrParam->GetParameterValue(MgHttpResourceStrings::reqFormat);
    if(m_format.empty())
    {
        m_format = MgMimeType::Xml; //default format is XML
    }

    STRING refreshValue = hrParam->GetParameterValue(MgHttpResourceStrings::reqRefresh);
    if(refreshValue == L"1")
    {
        m_refresh = true;
    }
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpEnumerateApplicationContainers::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters
    ValidateCommonParameters();

    // Obtain info about the available containers
    ReadContainerInfo();

    string responseString;
    //if(m_format != MgMimeType::Json)
    {
        responseString = GetXmlResponse();
    }
    
    // Create a byte reader.
    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)responseString.c_str(), (INT32)responseString.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpEnumerateApplicationContainers.Execute")
}

string MgHttpEnumerateApplicationContainers::GetXmlResponse()
{
    string response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    response += "<ApplicationDefinitionContainerInfoSet xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ApplicationDefinitionContainerInfoSet-1.0.0.xsd\">\n";

    for(ContainerInfoVector::iterator iter = m_containerInfoVector.begin();
        iter != m_containerInfoVector.end(); iter++)
    {
        ContainerInfo* containerInfo = *iter;
        response += "\t<ContainerInfo>\n";
        response += "\t\t<Type>" + containerInfo->type + "</Type>\n";
        response += "\t\t<Description>" + containerInfo->description + "</Description>\n";
        response += "\t\t<PreviewImageUrl>" + containerInfo->previewImageUrl + "</PreviewImageUrl>\n";
        response += "\t</ContainerInfo>\n";
    }
    response += "</ApplicationDefinitionContainerInfoSet>";
    
    return response;
}

void MgHttpEnumerateApplicationContainers::ReadContainerInfo()
{
    // Clear any old containers
    for(ContainerInfoVector::iterator iter = m_containerInfoVector.begin();
        iter != m_containerInfoVector.end(); iter++)
    {
        delete *iter;
    }
    m_containerInfoVector.clear();

    Ptr<MgStringCollection> containers = new MgStringCollection();
    FindContainers(containers, L"C:/Program Files/Autodesk/MapGuideEnterprise2008/WebServerExtensions/www/fusion/containerinfo");

    for(int i = 0; i < containers->GetCount(); i++)
    {
        MgXmlUtil xmlUtil;
        STRING containerFile = containers->GetItem(i);
        Ptr<MgByteReader> reader = new MgByteReader(containerFile, MgMimeType::Xml, false);
        STRING xmlTemplateInfo = reader->ToString();
        string xmlContent = MgUtil::WideCharToMultiByte(xmlTemplateInfo);
        xmlUtil.ParseString(xmlContent.c_str());
        DOMElement* root = xmlUtil.GetRootNode();
        STRING rootName = MgXmlUtil::GetTagName(root);
        if(rootName == L"ContainerInfo")
        {
            DOMNode* child = MgXmlUtil::GetFirstChild(root);
            
            // Read templates
            ContainerInfo* containerInfo = new ContainerInfo();
            while(0 != child)
            {
                if(MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
                {
                    DOMElement* elt = (DOMElement*)child;
                    wstring strName = MgXmlUtil::GetTagName(elt);

                    if(strName == L"Type")
                    {
                        containerInfo->type = GetStringFromElement(elt);
                    }
                    else if(strName == L"Description")
                    {
                        containerInfo->description = GetStringFromElement(elt);
                    }
                    else if(strName == L"PreviewImageUrl")
                    {
                        containerInfo->previewImageUrl = GetStringFromElement(elt);
                    }
                }
                child = MgXmlUtil::GetNextSibling(child);
            }
            m_containerInfoVector.push_back(containerInfo);
        }
    }
}

void MgHttpEnumerateApplicationContainers::FindContainers(MgStringCollection* containers, STRING rootFolder)
{
    // Open the directory
    ACE_DIR* directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(rootFolder.c_str()));

    if (directory != NULL)
    {
        dirent* dirEntry = NULL;

        // Go through the files in the directory
        while ((dirEntry = ACE_OS::readdir(directory)) != NULL)
        {
            STRING entryName = MG_TCHAR_TO_WCHAR(dirEntry->d_name);
            STRING fullDataPathname = rootFolder + L"/" + entryName;

            if (MgFileUtil::IsFile(fullDataPathname) &&
                MgFileUtil::EndsWithExtension(fullDataPathname, L".xml"))
            {
                // Add the file to the list
                containers->Add(fullDataPathname);
            }
        }

        ACE_OS::closedir(directory);
    }
}

///////////////////////////////////////////////////////////////////////////
// get the string value from the specified element
//
string MgHttpEnumerateApplicationContainers::GetStringFromElement(DOMElement* elt)
{
    string value = "";
    
    MG_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(elt);
    while(0 != child)
    {
        if(MgXmlUtil::GetNodeType(child) == DOMNode::TEXT_NODE)
        {
            wstring strval = MgXmlUtil::GetNodeValue(child);
            value = MgUtil::WideCharToMultiByte(MgUtil::Trim(strval));
            break;
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_CATCH_AND_THROW(L"MgHttpEnumerateApplicationContainers.GetStringFromElement")

    return value;
}


