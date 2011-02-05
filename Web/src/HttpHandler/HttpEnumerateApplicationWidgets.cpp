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

#include "HttpHandler.h"
#include "HttpEnumerateApplicationWidgets.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpEnumerateApplicationWidgets)

const STRING WIDGETINFO_DEFAULT_LOCALE = L"en";

static vector<STRING> WidgetInfoElements;
static vector<STRING> WidgetParameterElements;
static vector<STRING> WidgetAllowedValueElements;

static bool InitWidgetInfoParams();
static bool initWidgetParams = InitWidgetInfoParams();
bool InitWidgetInfoParams()
{
    //Create the set of supported info elements to parse
    WidgetInfoElements.push_back(L"Type"); //NOXLATE
    WidgetInfoElements.push_back(L"LocalizedType"); //NOXLATE
    WidgetInfoElements.push_back(L"Location");  //NOXLATE
    WidgetInfoElements.push_back(L"Description"); //NOXLATE
    WidgetInfoElements.push_back(L"Label");  //NOXLATE
    WidgetInfoElements.push_back(L"Tooltip");  //NOXLATE
    WidgetInfoElements.push_back(L"StatusText");  //NOXLATE
    WidgetInfoElements.push_back(L"ImageUrl");  //NOXLATE
    WidgetInfoElements.push_back(L"ImageClass"); //NOXLATE
    WidgetInfoElements.push_back(L"ContainableBy"); //NOXLATE
    WidgetInfoElements.push_back(L"StandardUi"); //NOXLATE

    //Create the set of supported parameter elements to parse
    WidgetParameterElements.push_back(L"Name"); //NOXLATE
    WidgetParameterElements.push_back(L"Description"); //NOXLATE
    WidgetParameterElements.push_back(L"DefaultValue"); //NOXLATE
    WidgetParameterElements.push_back(L"IsMandatory"); //NOXLATE
    WidgetParameterElements.push_back(L"Type"); //NOXLATE
    WidgetParameterElements.push_back(L"Label"); //NOXLATE
    WidgetParameterElements.push_back(L"Min"); //NOXLATE
    WidgetParameterElements.push_back(L"Max"); //NOXLATE

    //Create the set of supported allowed value elements to parse
    WidgetAllowedValueElements.push_back(L"Name"); //NOXLATE
    WidgetAllowedValueElements.push_back(L"Label"); //NOXLATE

    return true;
}
const static STRING PARAMETER_ELEMENT = L"Parameter"; //NOXLATE
const static STRING ALLOWEDVALUE_ELEMENT = L"AllowedValue";
const static STRING ROOT_ELEMENT = L"WidgetInfo"; //NOXLATE

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
MgHttpEnumerateApplicationWidgets::MgHttpEnumerateApplicationWidgets(MgHttpRequest *hRequest)
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
void MgHttpEnumerateApplicationWidgets::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters
    ValidateCommonParameters();

    // Obtain info about the available widgets
    string responseString = GetXmlResponse();

    // Create a byte reader.
    Ptr<MgByteReader> byteReader = MgUtil::GetByteReader(responseString, (STRING*)&MgMimeType::Xml);

    //Convert to alternate response format, if necessary
    ProcessFormatConversion(byteReader);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpEnumerateApplicationWidgets.Execute")
}

string MgHttpEnumerateApplicationWidgets::GetXmlResponse()
{
    Ptr<MgStringCollection> widgets = new MgStringCollection();

    // Get the path to the widget info folder
    STRING widgetInfoFolder = L"";
    MgConfiguration* config = MgConfiguration::GetInstance();
    if(config != NULL)
    {
        config->GetStringValue(MgConfigProperties::WebApplicationPropertiesSection,
            MgConfigProperties::WidgetInfoFolder, widgetInfoFolder, L"");
    }
    string response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    response += "<ApplicationDefinitionWidgetInfoSet xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ApplicationDefinitionInfo-1.0.0.xsd\">\n";
    if(widgetInfoFolder.length() > 0)
    {
        FindWidgets(widgets, widgetInfoFolder);
        for(int i = 0; i < widgets->GetCount(); i++)
        {
            MgXmlUtil xmlUtil;
            STRING widgetFile = widgets->GetItem(i);
            string widgetResponse = "";
            MG_HTTP_HANDLER_TRY()
            {
                Ptr<MgByteReader> reader = new MgByteReader(widgetFile, MgMimeType::Xml, false);
                STRING xmlTemplateInfo = reader->ToString();
                string xmlContent = MgUtil::WideCharToMultiByte(xmlTemplateInfo);
                xmlUtil.ParseString(xmlContent.c_str());
                DOMElement* root = xmlUtil.GetRootNode();
                STRING rootName = MgXmlUtil::GetTagName(root);
                if(rootName == ROOT_ELEMENT)
                {
                    DOMNode* child = MgXmlUtil::GetFirstChild(root);

                    // Write a WidgetInfo element
                    widgetResponse += "\t<WidgetInfo>\n";
                    while(0 != child)
                    {
                        if(MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
                        {
                            DOMElement* elt = (DOMElement*)child;
                            wstring strName = MgXmlUtil::GetTagName(elt);

                            // Copy all supported parameters into the response
                            for(vector<STRING>::iterator iter = WidgetInfoElements.begin(); iter != WidgetInfoElements.end(); iter++)
                            {
                                if(*iter == strName)
                                {
                                    string elementName = MgUtil::WideCharToMultiByte(strName.c_str());
                                    string elementValue = GetStringFromElement(elt);
                                    widgetResponse += "\t\t<" + elementName + ">";
                                    widgetResponse += elementValue;
                                    widgetResponse += "</" + elementName + ">\n";
                                    break;
                                }
                            }

                            if(strName == PARAMETER_ELEMENT)
                            {
                                DOMNode* paramChild = MgXmlUtil::GetFirstChild(elt);

                                // Write a Parameter element
                                widgetResponse += "\t\t\t<Parameter>\n";
                                while(paramChild != 0)
                                {
                                    if(MgXmlUtil::GetNodeType(paramChild) == DOMNode::ELEMENT_NODE)
                                    {
                                        DOMElement* paramElt = (DOMElement*)paramChild;
                                        wstring paramName = MgXmlUtil::GetTagName(paramElt);
                                        for(vector<STRING>::iterator iter = WidgetParameterElements.begin(); iter != WidgetParameterElements.end(); iter++)
                                        {
                                            if(*iter == paramName)
                                            {
                                                string elementName = MgUtil::WideCharToMultiByte(paramName.c_str());
                                                string elementValue = GetStringFromElement(paramElt);
                                                widgetResponse += "\t\t\t\t<" + elementName + ">";
                                                widgetResponse += elementValue;
                                                widgetResponse += "</" + elementName + ">\n";
                                                break;
                                            }
                                        }
                                        if(paramName == ALLOWEDVALUE_ELEMENT)
                                        {
                                            DOMNode* allowedValueChild = MgXmlUtil::GetFirstChild(paramElt);

                                            // Write an AllowedValue element
                                            widgetResponse += "\t\t\t\t<AllowedValue>\n";
                                            while(allowedValueChild != 0)
                                            {
                                                if(MgXmlUtil::GetNodeType(allowedValueChild) == DOMNode::ELEMENT_NODE)
                                                {
                                                    DOMElement* allowedValueElt = (DOMElement*)allowedValueChild;
                                                    wstring avParamName = MgXmlUtil::GetTagName(allowedValueElt);
                                                    for(vector<STRING>::iterator iter = WidgetAllowedValueElements.begin(); iter != WidgetAllowedValueElements.end(); iter++)
                                                    {
                                                        if(*iter == avParamName)
                                                        {
                                                            string elementName = MgUtil::WideCharToMultiByte(avParamName.c_str());
                                                            string elementValue = GetStringFromElement(allowedValueElt);
                                                            widgetResponse += "\t\t\t\t\t<" + elementName + ">";
                                                            widgetResponse += elementValue;
                                                            widgetResponse += "</" + elementName + ">\n";
                                                            break;
                                                        }
                                                    }
                                                }
                                                allowedValueChild = MgXmlUtil::GetNextSibling(allowedValueChild);
                                            }
                                            widgetResponse += "\t\t\t\t</AllowedValue>\n";
                                        }
                                    }
                                    paramChild = MgXmlUtil::GetNextSibling(paramChild);
                                }
                                widgetResponse += "\t\t\t</Parameter>\n";
                            }
                        }
                        child = MgXmlUtil::GetNextSibling(child);
                    }
                    widgetResponse += "\t</WidgetInfo>\n";
                }
                response += widgetResponse;
            }
            MG_HTTP_HANDLER_CATCH(L"MgHttpEnumerateApplicationWidgets::GetXmlResponse");
        }
    }
    response += "</ApplicationDefinitionWidgetInfoSet>";
    return response;
}

void MgHttpEnumerateApplicationWidgets::FindWidgets(MgStringCollection* widgets, STRING rootFolder)
{
    STRING locale = m_userInfo->GetLocale();
    if(locale.empty())
    {
        locale = WIDGETINFO_DEFAULT_LOCALE;
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
            if(locale != WIDGETINFO_DEFAULT_LOCALE)
            {
                localeRootFolder = rootFolder + L"/" + WIDGETINFO_DEFAULT_LOCALE;
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
                widgets->Add(fullDataPathname);
            }
        }

        ACE_OS::closedir(directory);
    }
}

///////////////////////////////////////////////////////////////////////////
// get the string value from the specified element
//
string MgHttpEnumerateApplicationWidgets::GetStringFromElement(DOMElement* elt)
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

    MG_CATCH_AND_THROW(L"MgHttpEnumerateApplicationWidgets.GetStringFromElement")

    return value;
}
