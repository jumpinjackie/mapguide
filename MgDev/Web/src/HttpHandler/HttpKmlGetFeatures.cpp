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
#include "HttpKmlGetFeatures.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpKmlGetFeatures)

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
MgHttpKmlGetFeatures::MgHttpKmlGetFeatures(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get the layer definition
    m_layerDefinition = params->GetParameterValue(MgHttpResourceStrings::reqKmlLayerDefinition);

    // Get the bounding box
    m_boundingBox = params->GetParameterValue(MgHttpResourceStrings::reqKmlBoundingBox);

    // Get the requested format
    m_format = params->GetParameterValue(MgHttpResourceStrings::reqKmlFormat);

    // Get the scale range
    /*STRING scaleRange = params->GetParameterValue(MgHttpResourceStrings::reqKmlScaleRange);
    Ptr<MgStringCollection> rangeValues = MgStringCollection::ParseCollection(scaleRange, L",");
    if(rangeValues->GetCount() >= 2)
    {
        double minScale = MgUtil::StringToDouble(rangeValues->GetItem(0));
        double maxScale = MgUtil::StringToDouble(rangeValues->GetItem(1));
        m_scale = (minScale + maxScale) / 2;
    }*/

    // Get the map image width
    STRING width = params->GetParameterValue(MgHttpResourceStrings::reqKmlWidth);
    if(!width.empty())
    {
        m_width = MgUtil::StringToInt32(width);
    }

    // Get the map image height
    STRING height = params->GetParameterValue(MgHttpResourceStrings::reqKmlHeight);
    if(!height.empty())
    {
        m_height = MgUtil::StringToInt32(height);
    }

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
void MgHttpKmlGetFeatures::Execute(MgHttpResponse& hResponse)
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

    // Create Extents
    Ptr<MgEnvelope> extents = GetExtents(m_boundingBox);

    // Get a KML service instance
    Ptr<MgKmlService> kmlService = dynamic_cast<MgKmlService*>(CreateService(MgServiceType::KmlService));

    // Get the KML geometries
    Ptr<MgByteReader> reader = kmlService->GetFeaturesKml(layer, extents, m_width, m_height, m_dpi, m_drawOrder, m_format);

    // Set the result
    hResult->SetResultObject(reader, reader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpKmlGetFeatures.Execute")
}

MgEnvelope* MgHttpKmlGetFeatures::GetExtents(CREFSTRING bbox)
{
    MgEnvelope* extents;
    extents = NULL;
    Ptr<MgStringCollection> bounds = MgStringCollection::ParseCollection(bbox, L",");

    if(bounds->GetCount() >= 4)
    {
        double coords[4];
        for(INT32 i = 0; i < 4; i++)
        {
            coords[i] = MgUtil::StringToDouble(bounds->GetItem(i));
        }
        Ptr<MgCoordinate> coord1 = new MgCoordinateXY(coords[0], coords[1]);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXY(coords[2], coords[3]);
        extents = new MgEnvelope(coord1, coord2);
    }
    return extents;
}

