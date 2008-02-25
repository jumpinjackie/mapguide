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

    INT32 version = m_userInfo->GetApiVersion();
    if (version == MG_API_VERSION(1,0,0))
    {
        // Get the keep selection flag
        m_bKeepSelection = true; // default
        STRING keepSelection = params->GetParameterValue(MgHttpResourceStrings::reqRenderingKeepSelection);
        if(!keepSelection.empty())
        {
            m_bKeepSelection = (keepSelection == L"1");
        }
    }
    else if (version == MG_API_VERSION(2,0,0))
    {
        m_behavior = MgUtil::StringToInt32(params->GetParameterValue(MgHttpResourceStrings::reqRenderingBehavior));
        m_selectionColor = params->GetParameterValue(MgHttpResourceStrings::reqRenderingSelectionColor);
    }
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
    Ptr<MgRenderingOptions> options;

    INT32 version = m_userInfo->GetApiVersion();
    if (version == MG_API_VERSION(1,0,0))
    {
        options = new MgRenderingOptions(m_mapFormat, MgRenderingOptions::RenderSelection |
            MgRenderingOptions::RenderLayers | (m_bKeepSelection ? MgRenderingOptions::KeepSelection : 0), NULL);
    }
    else if (version == MG_API_VERSION(2,0,0))
    {
        Ptr<MgColor> selectionColor = new MgColor(m_selectionColor);
        options = new MgRenderingOptions(m_mapFormat, m_behavior, selectionColor);
    }
    Ptr<MgByteReader> map = controller.GetDynamicMapOverlayImage(m_mapName, options);

    // Set the result
    hResult->SetResultObject(map, map->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetDynamicMapOverlayImage.Execute")
}

/// <summary>
/// This method is responsible for checking if
/// a valid version was given
/// </summary>
/// <returns>Returns nothing</returns>
void MgHttpGetDynamicMapOverlayImage::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    // There are multiple supported versions
    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(1,0,0) &&
        version != MG_API_VERSION(2,0,0))
    {
        throw new MgInvalidOperationVersionException(
        L"MgHttpGetDynamicMapOverlayImage.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpGetDynamicMapOverlayImage.ValidateOperationVersion");
}
