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
#include "HttpEnumerateApplicationTemplates.h"


const STRING TEMPLATE_FILENAME = L"templateInfo.xml";

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpEnumerateApplicationTemplates)

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
MgHttpEnumerateApplicationTemplates::MgHttpEnumerateApplicationTemplates(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Get response format
    m_format = hrParam->GetParameterValue(MgHttpResourceStrings::reqFormat);
    if(m_format.empty())
    {
        m_format = MgMimeType::Xml; //default format is XML
    }
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpEnumerateApplicationTemplates::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters
    ValidateCommonParameters();

    // Obtain info about the available templates
    ReadTemplateInfo();

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

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpEnumerateApplicationTemplates.Execute")
}

string MgHttpEnumerateApplicationTemplates::GetXmlResponse()
{
    string response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    response += "<ApplicationDefinitionTemplateInfoSet xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ApplicationDefinitionTemplateInfoSet-1.0.0.xsd\">\n";

    for(TemplateInfoVector::iterator iter = m_templateInfoVector.begin();
        iter != m_templateInfoVector.end(); iter++)
    {
        TemplateInfo* templateInfo = *iter;
        response += "\t<TemplateInfo>\n";
        response += "\t\t<Name>" + templateInfo->name + "</Name>\n";
        response += "\t\t<LocationUrl>" + templateInfo->locationUrl + "</LocationUrl>\n";
        response += "\t\t<Description>" + templateInfo->description + "</Description>\n";
        response += "\t\t<PreviewImageUrl>" + templateInfo->previewImageUrl + "</PreviewImageUrl>\n";
        response += "\t</TemplateInfo>\n";
    }
    response += "</ApplicationDefinitionTemplateInfoSet>";
    
    return response;
}

void MgHttpEnumerateApplicationTemplates::FindTemplates(MgStringCollection* templates, STRING rootFolder)
{
    // Open the directory
    ACE_DIR* directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(rootFolder.c_str()));

    if (directory != NULL)
    {
        dirent* dirEntry = NULL;

        // Go through the sub directories
        while ((dirEntry = ACE_OS::readdir(directory)) != NULL)
        {
            STRING entryName = MG_TCHAR_TO_WCHAR(dirEntry->d_name);
            STRING fullDataPathname = rootFolder + L"/" + entryName;

            if (MgFileUtil::IsDirectory(fullDataPathname)
                && entryName.compare(L"..") != 0 // skip ..
                && entryName.compare(L".") != 0) // skip .
            {
                // Look for the template file
                STRING templateFile = fullDataPathname + L"/" + TEMPLATE_FILENAME;
                if(MgFileUtil::IsFile(templateFile))
                {
                    templates->Add(templateFile);
                }
            }
        }

        ACE_OS::closedir(directory);
    }
}

void MgHttpEnumerateApplicationTemplates::ReadTemplateInfo()
{
    // Clear any old templates
    for(TemplateInfoVector::iterator iter = m_templateInfoVector.begin();
        iter != m_templateInfoVector.end(); iter++)
    {
        delete *iter;
    }
    m_templateInfoVector.clear();

    Ptr<MgStringCollection> templates = new MgStringCollection();
    FindTemplates(templates, L"C:/Program Files/Autodesk/MapGuideEnterprise2008/WebServerExtensions/www/fusion/templates");

    for(int i = 0; i < templates->GetCount(); i++)
    {
        MgXmlUtil xmlUtil;
        STRING templateFile = templates->GetItem(i);
        Ptr<MgByteReader> reader = new MgByteReader(templateFile, MgMimeType::Xml, false);
        STRING xmlTemplateInfo = reader->ToString();
        string xmlContent = MgUtil::WideCharToMultiByte(xmlTemplateInfo);
        xmlUtil.ParseString(xmlContent.c_str());
        DOMElement* root = xmlUtil.GetRootNode();
        DOMNode* child = MgXmlUtil::GetFirstChild(root);
        
        // Read templates
        TemplateInfo* templateInfo = new TemplateInfo();
        while(0 != child)
        {
            if(MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
            {
                DOMElement* elt = (DOMElement*)child;
                wstring strName = MgXmlUtil::GetTagName(elt);

                if(strName == L"Name")
                {
                    templateInfo->name = GetStringFromElement(elt);
                }
                else if(strName == L"LocationUrl")
                {
                    templateInfo->locationUrl = GetStringFromElement(elt);
                }
                else if(strName == L"Description")
                {
                    templateInfo->description = GetStringFromElement(elt);
                }
                else if(strName == L"PreviewImageUrl")
                {
                    templateInfo->previewImageUrl = GetStringFromElement(elt);
                }
            }
            child = MgXmlUtil::GetNextSibling(child);
        }
        m_templateInfoVector.push_back(templateInfo);
    }
}

///////////////////////////////////////////////////////////////////////////
// get the string value from the specified element
//
string MgHttpEnumerateApplicationTemplates::GetStringFromElement(DOMElement* elt)
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

    MG_CATCH_AND_THROW(L"MgHttpEnumerateApplicationTemplates.GetStringFromElement")

    return value;
}


