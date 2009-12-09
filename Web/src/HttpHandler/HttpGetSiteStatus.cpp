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

#include "HttpHandler.h"
#include "HttpGetSiteStatus.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetSiteStatus)

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
MgHttpGetSiteStatus::MgHttpGetSiteStatus(MgHttpRequest *hRequest)
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
void MgHttpGetSiteStatus::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    STRING xml = BeginXml();

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
                bool bHaveSiteStatus = false;
                STRING message = MgResources::Unknown;

                if (MgSiteInfo::Ok == siteInfo->GetStatus())
                {
                    MG_HTTP_HANDLER_TRY()
                    // Create ServerAdmin object
                    Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
                    serverAdmin->Open(siteInfo->GetTarget(), m_userInfo);

                    // call the C++ APIs
                    Ptr<MgPropertyCollection> properties = serverAdmin->GetSiteStatus();
                    xml += GetXml(properties);
                    bHaveSiteStatus = true;

                    MG_HTTP_HANDLER_CATCH(L"MgHttpGetSiteStatus.Execute")
                    if (mgException != NULL)
                    {
                        message = mgException->GetMessage();
                    }
                }

                if(!bHaveSiteStatus)
                {
                    // This server is not available
                    xml += L"\t<Server>\n";

                    xml += L"\t\t<DisplayName></DisplayName>\n";
                    xml += L"\t\t<Status>";
                    xml += message;
                    xml += L"</Status>\n";
                    xml += L"\t\t<ApiVersion></ApiVersion>\n";

                    xml += L"\t</Server>\n";
                }
            }
        }
    }

    xml += EndXml();

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(xml);
    if(!value)
        throw new MgOutOfMemoryException(L"", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, MgMimeType::Xml);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetSiteStatus.Execute")
}

STRING MgHttpGetSiteStatus::BeginXml()
{
    STRING xml = L"";

    // this XML follows the SiteStatus-2.2.0.xsd schema
    xml += L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml += L"<SiteStatus xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SiteStatus-2.2.0.xsd\">\n";

    return xml;
}

STRING MgHttpGetSiteStatus::EndXml()
{
    STRING xml = L"";

    xml = L"</SiteStatus>\n";

    return xml;
}

STRING MgHttpGetSiteStatus::GetXml(MgPropertyCollection* properties)
{
    STRING xml = L"";
    Ptr<MgStringProperty> strProp;
    Ptr<MgBooleanProperty> boolProp;

    xml += L"\t<Server>\n";

    xml += L"\t\t<DisplayName>";
    strProp = (MgStringProperty*)properties->GetItem(MgServerInformationProperties::DisplayName);
    xml += strProp->GetValue();
    xml += L"</DisplayName>\n";

    xml += L"\t\t<Status>";
    boolProp = (MgBooleanProperty*)properties->GetItem(MgServerInformationProperties::Status);
    xml += boolProp->GetValue() ? L"Online" : L"Offline";
    xml += L"</Status>\n";

    xml += L"\t\t<ApiVersion>";
    strProp = (MgStringProperty*)properties->GetItem(MgServerInformationProperties::ApiVersion);
    xml += strProp->GetValue();
    xml += L"</ApiVersion>\n";

    xml += L"\t</Server>\n";

    return xml;
}
