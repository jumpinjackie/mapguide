//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "HttpQueryMapFeatures.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpQueryMapFeatures)

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
MgHttpQueryMapFeatures::MgHttpQueryMapFeatures(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get the requested map name
    m_mapName = params->GetParameterValue(MgHttpResourceStrings::reqRenderingMapName);

    // Get the requested layer names
    m_layerNames = params->GetParameterValue(MgHttpResourceStrings::reqRenderingLayerNames);

    // Get maximum number of features to select
    string maxFeatures = MgUtil::WideCharToMultiByte(params->GetParameterValue(MgHttpResourceStrings::reqRenderingMaxFeatures));
    if(maxFeatures.length() == 0)
        m_maxFeatures = -1;
    else
        m_maxFeatures = atoi(maxFeatures.c_str());

    // Get the geometry WKT string
    m_geometry = params->GetParameterValue(MgHttpResourceStrings::reqRenderingGeometry);

    // Get the selection variant
    m_selectionVariant = params->GetParameterValue(MgHttpResourceStrings::reqRenderingSelectionVariant);

    // Flag indicated if the selection should be stored in the session repository
    m_persist = params->GetParameterValue(MgHttpResourceStrings::reqRenderingPersist) == L"1";
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpQueryMapFeatures::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    // Create the list of layers to include in the selection
    Ptr<MgStringCollection> layerNames = MgStringCollection::ParseCollection(m_layerNames, L",");

    // Create the selection geometry
    MgWktReaderWriter wktReader;
    Ptr<MgGeometry> geometry = wktReader.Read(m_geometry);

    // Create the selection variant
    INT32 selectionVariant = 0;
    if(m_selectionVariant.length() > 0)
    {
        if (m_selectionVariant == L"TOUCHES")
            selectionVariant = MgFeatureSpatialOperations::Touches;
        else if (m_selectionVariant == L"INTERSECTS")
            selectionVariant = MgFeatureSpatialOperations::Intersects;
        else if (m_selectionVariant == L"WITHIN")
            selectionVariant = MgFeatureSpatialOperations::Within;
        else    // add more values if necessary
        {
            MgStringCollection arguments;
            arguments.Add(L"0");
            arguments.Add(m_selectionVariant);

            throw new MgInvalidArgumentException(L"MgHttpQueryMapFeatures.Execute",
                __LINE__, __WFILE__, &arguments, L"MgInvalidFeatureSpatialOperation", NULL);
        }
    }

    // Call the HTML controller to process the request
    MgHtmlController controller;
    Ptr<MgByteReader> featureDescriptionInfo = controller.QueryMapFeatures(
        m_mapName, layerNames, geometry, selectionVariant, m_maxFeatures, m_persist);

    // Set the result
    hResult->SetResultObject(featureDescriptionInfo, featureDescriptionInfo->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpQueryMapFeatures.Execute")
}
