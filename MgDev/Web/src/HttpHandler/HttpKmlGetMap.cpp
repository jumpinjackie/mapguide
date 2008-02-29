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
#include "HttpKmlGetMap.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpKmlGetMap)

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
MgHttpKmlGetMap::MgHttpKmlGetMap(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get the map definition
    m_mapDefinition = params->GetParameterValue(MgHttpResourceStrings::reqKmlMapDefinition);

    // Get the map agent Uri
    m_agentUri = hRequest->GetAgentUri();

    // Get the requested format
    m_format = params->GetParameterValue(MgHttpResourceStrings::reqKmlFormat);

    // Get the map resolution
    STRING dpi = params->GetParameterValue(MgHttpResourceStrings::reqKmlDpi);
    if(!dpi.empty())
    {
        m_dpi = MgUtil::StringToDouble(dpi);
    }
    else
    {
        m_dpi = 96; // default
    }
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpKmlGetMap::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    STRING sessionId = m_userInfo->GetMgSessionId();

    // Get a resource service instance
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(CreateService(MgServiceType::ResourceService));

    // Create MgMap
    Ptr<MgMap> map = new MgMap();
    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(m_mapDefinition);
    map->Create(resourceService, resId, resId->GetName());

    // Get a KML service instance
    Ptr<MgKmlService> kmlService = dynamic_cast<MgKmlService*>(CreateService(MgServiceType::KmlService));

    // Get the KML representation of the map
    Ptr<MgByteReader> reader = kmlService->GetMapKml(map, m_dpi, m_agentUri, m_format);

    // Set the result
    hResult->SetResultObject(reader, reader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpKmlGetMap.Execute")
}



