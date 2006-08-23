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
#include "HttpGetDynamicMapOverlayImage.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetDynamicMapOverlayImage)

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
MgHttpGetDynamicMapOverlayImage::MgHttpGetDynamicMapOverlayImage(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get the map name
    m_mapName = params->GetParameterValue(MgHttpResourceStrings::reqRenderingMapName);

    // Get the requested image format
    m_mapFormat = params->GetParameterValue(MgHttpResourceStrings::reqRenderingFormat);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetDynamicMapOverlayImage::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    // Call the HTML controller to render the map image
    MgHtmlController controller(m_siteConn);
    Ptr<MgByteReader> map = controller.GetDynamicMapOverlayImage(m_mapName, m_mapFormat);

    // Set the result
    hResult->SetResultObject(map, map->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetDynamicMapOverlayImage.Execute")
}
