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
#include "HttpKmlGetLayer.h"
#include "HttpKmlGetFeatures.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpKmlGetLayer)

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
MgHttpKmlGetLayer::MgHttpKmlGetLayer(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get the layer definition
    m_layerDefinition = params->GetParameterValue(MgHttpResourceStrings::reqKmlLayerDefinition);

    // Get the map agent Uri
    m_agentUri = hRequest->GetAgentUri();

    // Get the bounding box
    m_boundingBox = params->GetParameterValue(MgHttpResourceStrings::reqKmlBoundingBox);

    // Get the requested format
    m_format = params->GetParameterValue(MgHttpResourceStrings::reqKmlFormat);

    // Get the map image width
    STRING width = params->GetParameterValue(MgHttpResourceStrings::reqKmlWidth);
    m_width = MgUtil::StringToInt32(width);

    // Get the map image height
    STRING height = params->GetParameterValue(MgHttpResourceStrings::reqKmlHeight);
    m_height = MgUtil::StringToInt32(height);

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

    // Get the draw order
    STRING drawOrder = params->GetParameterValue(MgHttpResourceStrings::reqKmlDrawOrder);
    m_drawOrder = drawOrder.empty() ? 0 : MgUtil::StringToInt32(drawOrder);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpKmlGetLayer::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    STRING sessionId = m_userInfo->GetMgSessionId();

    // Get a resource service instance
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(CreateService(MgServiceType::ResourceService));

    // Create MgLayer
    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(m_layerDefinition);
    Ptr<MgLayer> layer = new MgLayer(resId, resourceService);

    // Get a KML service instance
    Ptr<MgKmlService> kmlService = dynamic_cast<MgKmlService*>(CreateService(MgServiceType::KmlService));

    // Create Extents
    Ptr<MgEnvelope> extents = MgHttpKmlGetFeatures::GetExtents(m_boundingBox);

    // Get the KML representation of the map
    Ptr<MgByteReader> reader = kmlService->GetLayerKml(layer, extents, m_width, m_height, m_dpi, m_drawOrder, m_agentUri, m_format);

    // Set the result
    hResult->SetResultObject(reader, reader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpKmlGetLayer.Execute")
}



