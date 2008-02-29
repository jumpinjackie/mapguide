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
#include "HttpGetFeatureSetEnvelope.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetFeatureSetEnvelope)

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
MgHttpGetFeatureSetEnvelope::MgHttpGetFeatureSetEnvelope(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_mapName = params->GetParameterValue(MgHttpResourceStrings::reqMappingMapName);
    m_featureSetXml = params->GetParameterValue(MgHttpResourceStrings::reqMappingFeatureSetXml);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetFeatureSetEnvelope::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();
    Ptr<MgResourceService> resourceService = (MgResourceService*)CreateService(MgServiceType::ResourceService);
    Ptr<MgFeatureService> featureService = (MgFeatureService*)CreateService(MgServiceType::FeatureService);

    // Create MgMap
    Ptr<MgMap> map = new MgMap();
    map->Open(resourceService, m_mapName);

    // Calculate selection extent
    Ptr<MgSelection> selection = new MgSelection(map, m_featureSetXml);
    Ptr<MgEnvelope> envelope = selection->GetExtents(featureService);
    Ptr<MgByteReader> byteReader = envelope->ToXml();

    hResult->SetResultObject(byteReader, MgMimeType::Xml);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetFeatureSetEnvelope.Execute")
}
