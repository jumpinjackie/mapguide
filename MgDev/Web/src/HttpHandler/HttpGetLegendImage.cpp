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
#include "HttpGetLegendImage.h"

const INT32 DefaultWidth = 16;
const INT32 DefaultHeight = 16;
const STRING DefaultFormat = L"PNG8";

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetLegendImage)

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
MgHttpGetLegendImage::MgHttpGetLegendImage(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> hrParam = hRequest->GetRequestParam();

    m_layerDefinition = hrParam->GetParameterValue(MgHttpResourceStrings::reqMappingLayerDefinition);

    // Get scale and convert to integer
    string scale_str = MgUtil::WideCharToMultiByte(hrParam->GetParameterValue(MgHttpResourceStrings::reqMappingScale));
    m_scale = atoi(scale_str.c_str());

    // The parameters below are optional

    // Get width and convert to integer
    string width_str = MgUtil::WideCharToMultiByte(hrParam->GetParameterValue(MgHttpResourceStrings::reqMappingWidth));
    if(width_str.empty())
    {
        // Use default
        m_width = DefaultWidth;
    }
    else
    {
        m_width = atoi(width_str.c_str());
    }

    // Get height and convert to integer
    string height_str = MgUtil::WideCharToMultiByte(hrParam->GetParameterValue(MgHttpResourceStrings::reqMappingHeight));
    if(height_str.empty())
    {
        // Use default
        m_height = DefaultHeight;
    }
    else
    {
        m_height = atoi(height_str.c_str());
    }

    m_format = hrParam->GetParameterValue(MgHttpResourceStrings::reqMappingFormat);
    if(m_format.empty())
    {
        // Use default
        m_format = DefaultFormat;
    }

    // Get type and convert to integer
    string type_str = MgUtil::WideCharToMultiByte(hrParam->GetParameterValue(MgHttpResourceStrings::reqMappingType));
    if(type_str.empty())
    {
        // Use default
        m_type = -1;
    }
    else
    {
        m_type = atoi(type_str.c_str());
    }

    // Get theme category and convert to integer
    string themeCategory_str = MgUtil::WideCharToMultiByte(hrParam->GetParameterValue(MgHttpResourceStrings::reqMappingThemeCategory));
    if(themeCategory_str.empty())
    {
        // Use default
        m_themeCategory = -1;
    }
    else
    {
        m_themeCategory = atoi(themeCategory_str.c_str());
    }
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetLegendImage::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters
    ValidateCommonParameters();

    // Create ProxyMappingService instance
    Ptr<MgMappingService> mgprService = (MgMappingService*)(CreateService(MgServiceType::MappingService));

    // Create MgResourceIdentifier
    MgResourceIdentifier layerDefinition(m_layerDefinition);

    // Run API command
    Ptr<MgByteReader> byteReaderResult = mgprService->GenerateLegendImage(&layerDefinition, m_scale, m_width, m_height, m_format, m_type, m_themeCategory);

    if (byteReaderResult)
    {
        hResult->SetResultObject(byteReaderResult, byteReaderResult->GetMimeType());
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetLegendImage.Execute")
}
