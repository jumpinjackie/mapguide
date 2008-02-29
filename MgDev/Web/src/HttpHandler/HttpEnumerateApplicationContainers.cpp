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
#include "HttpEnumerateApplicationContainers.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpEnumerateApplicationContainers)

const STRING WIDGET_TYPE_CONTAINER = L"CONTAINER"; //NOXLATE
const STRING WIDGET_TYPE_COMMAND = L"COMMAND"; //NOXLATE
const STRING CONTAINERINFO_DEFAULT_LOCALE = L"en";

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

    // Get the response in XML format
    string responseString = GetXmlResponse();

    // Create a byte reader.
    Ptr<MgByteReader> byteReader = MgUtil::GetByteReader(responseString, (STRING*)&MgMimeType::Xml);

    //Convert to alternate response format, if necessary
    ProcessFormatConversion(byteReader);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpEnumerateApplicationContainers.Execute")
}

string MgHttpEnumerateApplicationContainers::GetXmlResponse()
{
    string response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"; //NOXLATE

    response += "<ApplicationDefinitionContainerInfoSet xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ApplicationDefinitionContainerInfoSet-1.0.0.xsd\">\n"; //NOXLATE

    for(ContainerInfoVector::iterator iter = m_containerInfoVector.begin();
        iter != m_containerInfoVector.end(); iter++)
    {
        ContainerInfo* containerInfo = *iter;
        response += "\t<ContainerInfo>\n"; //NOXLATE
        response += "\t\t<Type>" + containerInfo->type + "</Type>\n"; //NOXLATE
        response += "\t\t<LocalizedType>" + containerInfo->localizedType + "</LocalizedType>\n"; //NOXLATE
        response += "\t\t<Description>" + containerInfo->description + "</Description>\n"; //NOXLATE
        response += "\t\t<PreviewImageUrl>" + containerInfo->previewImageUrl + "</PreviewImageUrl>\n"; //NOXLATE
        response += "\t</ContainerInfo>\n"; //NOXLATE
    }
    response += "</ApplicationDefinitionContainerInfoSet>"; //NOXLATE

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

    // Get the path to the container info folder
    STRING containerInfoFolder = L"";
    MgConfiguration* config = MgConfiguration::GetInstance();
    if(config != NULL)
    {
        config->GetStringValue(MgConfigProperties::WebApplicationPropertiesSection,
            MgConfigProperties::ContainerInfoFolder, containerInfoFolder, L"");
    }
    if(containerInfoFolder.length() > 0)
    {
        // Find all container info files
        FindContainers(containers, containerInfoFolder);

        // Parse container info files
        for(int i = 0; i < containers->GetCount(); i++)
        {
            MgXmlUtil xmlUtil;
            STRING containerFile = containers->GetItem(i);
            MG_HTTP_HANDLER_TRY()
            {
                Ptr<MgByteReader> reader = new MgByteReader(containerFile, MgMimeType::Xml, false);
                STRING xmlTemplateInfo = reader->ToString();
                string xmlContent = MgUtil::WideCharToMultiByte(xmlTemplateInfo);
                xmlUtil.ParseString(xmlContent.c_str());
                DOMElement* root = xmlUtil.GetRootNode();
                STRING rootName = MgXmlUtil::GetTagName(root);
                if(rootName == L"ContainerInfo") //NOXLATE
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

                            if(strName == L"Type") //NOXLATE
                            {
                                containerInfo->type = GetStringFromElement(elt);
                            }
                            if(strName == L"LocalizedType") //NOXLATE
                            {
                                containerInfo->localizedType = GetStringFromElement(elt);
                            }
                            else if(strName == L"Description") //NOXLATE
                            {
                                containerInfo->description = GetStringFromElement(elt);
                            }
                            else if(strName == L"PreviewImageUrl") //NOXLATE
                            {
                                containerInfo->previewImageUrl = GetStringFromElement(elt);
                            }
                        }
                        child = MgXmlUtil::GetNextSibling(child);
                    }
                    m_containerInfoVector.push_back(containerInfo);
                }
            }
            MG_HTTP_HANDLER_CATCH(L"MgHttpEnumerateApplicationContainers::ReadContainerInfo");
        }
    }
}

void MgHttpEnumerateApplicationContainers::FindContainers(MgStringCollection* containers, STRING rootFolder)
{
    STRING locale = m_userInfo->GetLocale();
    if(locale.empty())
    {
        locale = CONTAINERINFO_DEFAULT_LOCALE;
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
			if(locale != CONTAINERINFO_DEFAULT_LOCALE)
			{
				localeRootFolder = rootFolder + L"/" + CONTAINERINFO_DEFAULT_LOCALE;
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


