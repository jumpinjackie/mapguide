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

static vector<STRING> TemplateInfoElements;
static vector<STRING> TemplatePanelElements;
static bool InitTemplateInfoParams();
static bool initTemplateInfoParams = InitTemplateInfoParams();
bool InitTemplateInfoParams()
{
    //Create the set of supported info elements to parse
    TemplateInfoElements.push_back(L"Name"); //NOXLATE
    TemplateInfoElements.push_back(L"LocationUrl");  //NOXLATE
    TemplateInfoElements.push_back(L"Description"); //NOXLATE
    TemplateInfoElements.push_back(L"PreviewImageUrl");  //NOXLATE

    //Create the set of supported panel elements to parse
    TemplatePanelElements.push_back(L"Name"); //NOXLATE
    TemplatePanelElements.push_back(L"Label"); //NOXLATE
    TemplatePanelElements.push_back(L"Description"); //NOXLATE

    return true;
}

const static STRING TEMPLATEINFO_ELEMENT = L"TemplateInfo"; //NOXLATE
const static STRING PANEL_ELEMENT = L"Panel"; //NOXLATE

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

    // Get the response as XML
    string responseString = GetXmlResponse();

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
    Ptr<MgStringCollection> templates = new MgStringCollection();

    string response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    response += "<ApplicationDefinitionTemplateInfoSet xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ApplicationDefinitionTemplateInfoSet-1.0.0.xsd\">\n";

    // Get the path to the template root folder
    STRING templateRootFolder = L"";
    MgConfiguration* config = MgConfiguration::GetInstance();
    if(config != NULL)
    {
        config->GetStringValue(MgConfigProperties::WebApplicationPropertiesSection,
            MgConfigProperties::TemplateRootFolder, templateRootFolder, L"");
    }
    if(templateRootFolder.length() > 0)
    {
        FindTemplates(templates, templateRootFolder);

        for(int i = 0; i < templates->GetCount(); i++)
        {
            MgXmlUtil xmlUtil;
            STRING templateFile = templates->GetItem(i);
            Ptr<MgByteReader> reader = new MgByteReader(templateFile, MgMimeType::Xml, false);
            STRING xmlTemplateInfo = reader->ToString();
            string xmlContent = MgUtil::WideCharToMultiByte(xmlTemplateInfo);
            xmlUtil.ParseString(xmlContent.c_str());
            DOMElement* root = xmlUtil.GetRootNode();
            STRING rootName = MgXmlUtil::GetTagName(root);
            if(rootName == TEMPLATEINFO_ELEMENT)
            {
                DOMNode* child = MgXmlUtil::GetFirstChild(root);

                // Write a TemplateInfo element
                int depth = 1;
                response += CreateOpenElement(TEMPLATEINFO_ELEMENT, depth, true);
                depth++;
                while(0 != child)
                {
                    if(MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
                    {
                        DOMElement* elt = (DOMElement*)child;
                        wstring strName = MgXmlUtil::GetTagName(elt);

                        // Copy all supported parameters into the response
                        for(vector<STRING>::iterator iter = TemplateInfoElements.begin(); iter != TemplateInfoElements.end(); iter++)
                        {
                            if(*iter == strName)
                            {
                                string elementName = MgUtil::WideCharToMultiByte(strName.c_str());
                                string elementValue = GetStringFromElement(elt);
                                response += CreateOpenElement(elementName, depth, false);
                                response += elementValue;
                                response += CreateCloseElement(elementName, 0, true);
                                break;
                            }
                        }

                        if(strName == PANEL_ELEMENT)
                        {
                            DOMNode* panelChild = MgXmlUtil::GetFirstChild(elt);

                            // Write a Panel element
                            response += CreateOpenElement(PANEL_ELEMENT, depth, true);
                            depth++;
                            while(panelChild != 0)
                            {
                                if(MgXmlUtil::GetNodeType(panelChild) == DOMNode::ELEMENT_NODE)
                                {
                                    DOMElement* paramElt = (DOMElement*)panelChild;
                                    wstring paramName = MgXmlUtil::GetTagName(paramElt);
                                    for(vector<STRING>::iterator iter = TemplatePanelElements.begin(); iter != TemplatePanelElements.end(); iter++)
                                    {
                                        if(*iter == paramName)
                                        {
                                            string elementName = MgUtil::WideCharToMultiByte(paramName.c_str());
                                            string elementValue = GetStringFromElement(paramElt);
                                            response += CreateOpenElement(elementName, depth, false);
                                            response += elementValue;
                                            response += CreateCloseElement(elementName, 0, true);
                                            break;
                                        }
                                    }
                                }
                                panelChild = MgXmlUtil::GetNextSibling(panelChild);
                            }
                            depth--;
                            response += CreateCloseElement(PANEL_ELEMENT, depth, true);
                        }
                    }
                    child = MgXmlUtil::GetNextSibling(child);
                }
                depth--;
                response += CreateCloseElement(TEMPLATEINFO_ELEMENT, depth, true);
            }
        }
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

string MgHttpEnumerateApplicationTemplates::CreateOpenElement(const wstring name, int insetDepth, bool linebreak)
{
    return CreateOpenElement(MgUtil::WideCharToMultiByte(name), insetDepth, linebreak);
}

string MgHttpEnumerateApplicationTemplates::CreateOpenElement(const string name, int insetDepth, bool linebreak)
{
    string element = "";
    for(int i = 0; i < insetDepth; i++)
    {
        element += "\t";
    }
    element += "<";
    element += name;
    element += ">";
    if(linebreak)
    {
        element += "\n";
    }
    return element;
}

string MgHttpEnumerateApplicationTemplates::CreateCloseElement(const wstring name, int insetDepth, bool linebreak)
{
    return CreateCloseElement(MgUtil::WideCharToMultiByte(name), insetDepth, linebreak);
}

string MgHttpEnumerateApplicationTemplates::CreateCloseElement(const string name, int insetDepth, bool linebreak)
{
    string element = "";
    for(int i = 0; i < insetDepth; i++)
    {
        element += "\t";
    }
    element += "</";
    element += name;
    element += ">";
    if(linebreak)
    {
        element += "\n";
    }
    return element;
}



