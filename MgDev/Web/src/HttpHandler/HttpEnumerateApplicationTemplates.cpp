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

#include "HttpHandler.h"
#include "HttpEnumerateApplicationTemplates.h"


const STRING TEMPLATE_FILENAME = L"TemplateInfo";
const STRING TEMPLATE_FILENAME_EXTENSION = L".xml";
const STRING TEMPLATEINFO_DEFAULT_LOCALE = L"en";

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
    Ptr<MgByteReader> byteReader = MgUtil::GetByteReader(responseString, (STRING*)&MgMimeType::Xml);

    //Convert to alternate response format, if necessary
    ProcessFormatConversion(byteReader);

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
            string templateResponse = "";
            MG_HTTP_HANDLER_TRY()
            {
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
                    templateResponse += CreateOpenElement(TEMPLATEINFO_ELEMENT, depth, true);
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
                                    templateResponse += CreateOpenElement(elementName, depth, false);
                                    templateResponse += elementValue;
                                    templateResponse += CreateCloseElement(elementName, 0, true);
                                    break;
                                }
                            }

                            if(strName == PANEL_ELEMENT)
                            {
                                DOMNode* panelChild = MgXmlUtil::GetFirstChild(elt);

                                // Write a Panel element
                                templateResponse += CreateOpenElement(PANEL_ELEMENT, depth, true);
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
                                                templateResponse += CreateOpenElement(elementName, depth, false);
                                                templateResponse += elementValue;
                                                templateResponse += CreateCloseElement(elementName, 0, true);
                                                break;
                                            }
                                        }
                                    }
                                    panelChild = MgXmlUtil::GetNextSibling(panelChild);
                                }
                                depth--;
                                templateResponse += CreateCloseElement(PANEL_ELEMENT, depth, true);
                            }
                        }
                        child = MgXmlUtil::GetNextSibling(child);
                    }
                    depth--;
                    templateResponse += CreateCloseElement(TEMPLATEINFO_ELEMENT, depth, true);
                }
                response += templateResponse;
            }
            MG_HTTP_HANDLER_CATCH(L"MgHttpEnumerateApplicationTemplates::GetXmlResponse")
        }
    }
    response += "</ApplicationDefinitionTemplateInfoSet>";

    return response;
}

void MgHttpEnumerateApplicationTemplates::FindTemplates(MgStringCollection* templates, STRING rootFolder)
{
    STRING locale = m_userInfo->GetLocale();
    if(locale.empty())
    {
        locale = TEMPLATEINFO_DEFAULT_LOCALE;
    }
    STRING localeRootFolder = rootFolder + L"/" + locale;
    
    // Open the locale-specific directory
    ACE_DIR* directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(localeRootFolder.c_str()));
    if (directory == NULL)
    {
        // If no locale-specific directory exists, then if the locale is an extended local (5 characters), 
        // truncate the locale to the 2 character "parent" locale and try again
        if (MG_EXTENDED_LOCALE_LENGTH == locale.length())
        {
            STRING parentLocale = rootFolder + L"/" + locale.substr(0, 2);
            directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(parentLocale.c_str()));
        }

        if (directory == NULL)
        {
            // If no locale-specific directory exists, open the default directory
            if(locale != TEMPLATEINFO_DEFAULT_LOCALE)
            {
                localeRootFolder = rootFolder + L"/" + TEMPLATEINFO_DEFAULT_LOCALE;
                directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(localeRootFolder.c_str()));
            }
            // If no default locale folder exists, try the root folder
            if (directory == NULL)
            {
                localeRootFolder = rootFolder;
                directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(localeRootFolder.c_str()));
            }
        }
    }

    if (directory != NULL)
    {
        dirent* dirEntry = NULL;

        // Go through the files in the directory
        while ((dirEntry = ACE_OS::readdir(directory)) != NULL)
        {
            STRING entryName = MG_TCHAR_TO_WCHAR(dirEntry->d_name);
            STRING fullDataPathname = localeRootFolder + L"/" + entryName;

            if (MgFileUtil::IsFile(fullDataPathname) &&
                MgFileUtil::EndsWithExtension(fullDataPathname, L".xml"))
            {
                // Add the file to the list
                templates->Add(fullDataPathname);
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
