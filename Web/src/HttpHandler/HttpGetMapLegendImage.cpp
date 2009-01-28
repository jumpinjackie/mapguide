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
#include "HttpGetMapLegendImage.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetMapLegendImage)

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
MgHttpGetMapLegendImage::MgHttpGetMapLegendImage(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get the map resource
    m_mapName = params->GetParameterValue(MgHttpResourceStrings::reqRenderingMapName);

    // Get the requested image format
    m_mapFormat = params->GetParameterValue(MgHttpResourceStrings::reqRenderingFormat);

    // Get width and convert to integer
    string widthParam = MgUtil::WideCharToMultiByte(params->GetParameterValue(MgHttpResourceStrings::reqRenderingWidth));
    m_width = atoi(widthParam.c_str());

    // Get height and convert to integer
    string heightParam = MgUtil::WideCharToMultiByte(params->GetParameterValue(MgHttpResourceStrings::reqRenderingHeight));
    m_height = atoi(heightParam.c_str());
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetMapLegendImage::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    // Determine the background color
    // TODO - Get background color from request
    Ptr<MgColor> backgroundColor = new MgColor(255, 255, 255);

    // Use the HTML controller to generate the map legend image
    MgHtmlController controller(m_siteConn);
    Ptr<MgByteReader> mapLegendImage = controller.GetMapLegendImage(m_mapName,
        m_mapFormat, backgroundColor, m_width, m_height);

    // Set the result
    hResult->SetResultObject(mapLegendImage, mapLegendImage->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetMapLegendImage.Execute")
}
