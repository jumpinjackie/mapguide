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

    // Create ServerAdmin object
    Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
    serverAdmin->Open(m_userInfo);

    // call the C++ APIs
    Ptr<MgPropertyCollection> properties = serverAdmin->GetInformationProperties();
    STRING xml = GetXml(properties);

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(xml);
    if(!value)
        throw new MgOutOfMemoryException(L"", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, MgMimeType::Xml);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetSiteVersion.Execute")
}

STRING MgHttpGetSiteVersion::GetXml(MgPropertyCollection* properties)
{
    Ptr<MgStringProperty> property = (MgStringProperty*)properties->GetItem(L"ServerVersion");
    STRING xml = L"";

    // this XML follows the SiteVersion-1.0.0.xsd schema
    xml += L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml += L"<SiteVersion xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SiteVersion-1.0.0.xsd\">\n";
    xml += L"\t<Version>";
    xml += property->GetValue();
    xml += L"</Version>\n";
    xml += L"</SiteVersion>\n";

    return xml;
}
