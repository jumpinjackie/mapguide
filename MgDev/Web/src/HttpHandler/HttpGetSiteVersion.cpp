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

#include "HttpHandler.h"
#include "HttpGetSiteVersion.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetSiteVersion)

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
MgHttpGetSiteVersion::MgHttpGetSiteVersion(MgHttpRequest *hRequest)
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
void MgHttpGetSiteVersion::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    STRING xml;
    xml += BeginXml();

    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        MgSiteManager* siteManager = MgSiteManager::GetInstance();
        if(siteManager)
        {
            MgSiteVector* sites = siteManager->GetSites();
            if(sites)
            {
                for(size_t i=0;i<sites->size();i++)
                {
                    MgSiteInfo* siteInfo = sites->at(i);

                    // Check the server status - though this status could be out of date and an exception might be thrown
                    bool bHaveSiteVersion = false;
                    STRING message = MgResources::Unknown;

                    if (MgSiteInfo::Ok == siteInfo->GetStatus())
                    {
                        MG_HTTP_HANDLER_TRY()

                        // Create ServerAdmin object
                        Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
                        serverAdmin->Open(siteInfo->GetTarget(), m_userInfo);

                        // call the C++ APIs
                        STRING version = serverAdmin->GetSiteVersion();
                        xml += GetXml(version);
                        bHaveSiteVersion = true;

                        MG_HTTP_HANDLER_CATCH(L"MgHttpGetSiteInfo.Execute")
                        if (mgException != NULL)
                        {
                            message = mgException->GetMessage();
                        }
                    }

                    if(!bHaveSiteVersion)
                    {
                        // This server is not available
                        xml += L"\t<Server>\n";

                        xml += L"\t\t<Version>";
                        xml += message;
                        xml += L"</Version>\n";

                        xml += L"\t</Server>\n";
                    }
                }
            }
        }
    }
    else
    {
        // Create ServerAdmin object
        Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
        serverAdmin->Open(m_userInfo);

        // call the C++ APIs
        STRING version = serverAdmin->GetSiteVersion();
        xml += GetXml(version);
    }
    xml += EndXml();

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(xml);
    if(!value)
        throw new MgOutOfMemoryException(L"", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, MgMimeType::Xml);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetSiteVersion.Execute")
}

STRING MgHttpGetSiteVersion::BeginXml()
{
    STRING xml = L"";

    xml += L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        xml += L"<SiteVersion xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SiteVersion-2.2.0.xsd\">\n";
    }
    else
    {
        xml += L"<SiteVersion xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SiteVersion-1.0.0.xsd\">\n";
    }

    return xml;
}

STRING MgHttpGetSiteVersion::EndXml()
{
    STRING xml = L"";

    xml += L"</SiteVersion>\n";

    return xml;
}

STRING MgHttpGetSiteVersion::GetXml(CREFSTRING version)
{
    STRING xml = L"";

    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        xml += L"\t<Server>\n";
    }

    xml += L"\t<Version>";
    xml += version;
    xml += L"</Version>\n";

    if (m_userInfo->GetApiVersion() >= MG_API_VERSION(2,2,0))
    {
        xml += L"\t</Server>\n";
    }

    return xml;
}
