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
#include "HttpGetTileImage.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetTileImage)

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
MgHttpGetTileImage::MgHttpGetTileImage(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    INT32 version = m_userInfo->GetApiVersion();
    if (version == MG_API_VERSION(1,0,0))
    {
        // Get the map name
        m_mapName = params->GetParameterValue(MgHttpResourceStrings::reqRenderingMapName);

        // Get the baseMapLayerGroup name
        m_baseMapLayerGroupName = params->GetParameterValue(MgHttpResourceStrings::reqRenderingBaseMapLayerGroupName);

        // Get the tile column index and convert to integer
        m_tileCol = MgUtil::StringToInt32(params->GetParameterValue(MgHttpResourceStrings::reqRenderingTileColumn));

        // Get the tile row index and convert to integer
        m_tileRow = MgUtil::StringToInt32(params->GetParameterValue(MgHttpResourceStrings::reqRenderingTileRow));
    }
    else if (version == MG_API_VERSION(1,2,0))
    {
        // Get the map name
        m_mapName = params->GetParameterValue(MgHttpResourceStrings::reqTileMapDefinition);

        // Get the baseMapLayerGroup name
        m_baseMapLayerGroupName = params->GetParameterValue(MgHttpResourceStrings::reqRenderingBaseMapLayerGroupName);

        // Get the tile column index and convert to integer
        m_tileCol = MgUtil::StringToInt32(params->GetParameterValue(MgHttpResourceStrings::reqRenderingTileColumn));

        // Get the tile row index and convert to integer
        m_tileRow = MgUtil::StringToInt32(params->GetParameterValue(MgHttpResourceStrings::reqRenderingTileRow));

        // Get the scale index and convert to integer
        m_scaleIndex = MgUtil::StringToInt32(params->GetParameterValue(MgHttpResourceStrings::reqRenderingScaleIndex));
    }
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetTileImage::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    if (m_mapName.empty() || m_baseMapLayerGroupName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"0");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgHttpGetTileImage.Execute",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    INT32 version = m_userInfo->GetApiVersion();
    if (version == MG_API_VERSION(1,0,0))
    {
        // Get Proxy Resource Service instance
        Ptr<MgResourceService> resourceService = (MgResourceService*)CreateService(MgServiceType::ResourceService);

        // Create MgMap
        Ptr<MgMap> map = new MgMap();
        map->Open(resourceService, m_mapName);

        // Get Proxy Tile Service instance
        Ptr<MgTileService> service = (MgTileService*)(CreateService(MgServiceType::TileService));

        // Call the C++ API
        Ptr<MgByteReader> tileImage = service->GetTile(map, m_baseMapLayerGroupName, m_tileCol, m_tileRow);

        // Set the result
        hResult->SetResultObject(tileImage, tileImage->GetMimeType());
    }
    else if (version == MG_API_VERSION(1,2,0))
    {
        Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(m_mapName);

        // Get Proxy Tile Service instance
        Ptr<MgTileService> service = (MgTileService*)(CreateService(MgServiceType::TileService));

        // Call the C++ API
        Ptr<MgByteReader> tileImage = service->GetTile(resId, m_baseMapLayerGroupName, m_tileCol, m_tileRow, m_scaleIndex);

        // Set the result
        hResult->SetResultObject(tileImage, tileImage->GetMimeType());
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetTileImage.Execute")
}

/// <summary>
/// This method is responsible for checking if
/// a valid version was given
/// </summary>
/// <returns>Returns nothing</returns>
void MgHttpGetTileImage::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    // There are multiple supported versions
    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(1,0,0) &&
        version != MG_API_VERSION(1,2,0))
    {
        throw new MgInvalidOperationVersionException(
        L"MgHttpGetTileImage.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpGetTileImage.ValidateOperationVersion");
}
