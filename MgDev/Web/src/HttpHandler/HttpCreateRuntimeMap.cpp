//
//  Copyright (C) 2004-2013 by Autodesk, Inc.
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
#include "HttpCreateRuntimeMap.h"
#include "VectorLayerDefinition.h"
#include "PointTypeStyle.h"
#include "AreaTypeStyle.h"
#include "LineTypeStyle.h"
#include "CompositeTypeStyle.h"

#define REQUEST_LAYER_STRUCTURE         1 /* Request layer and group structure */
#define REQUEST_LAYER_ICONS             2 /* Request layer scale and icon information */
#define REQUEST_LAYER_FEATURE_SOURCE    4 /* Request information about a layer's feature source */

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpCreateRuntimeMap)

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
MgHttpCreateRuntimeMap::MgHttpCreateRuntimeMap(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_mapDefinition = params->GetParameterValue(MgHttpResourceStrings::reqMappingMapDefinition);
    STRING mask = params->GetParameterValue(MgHttpResourceStrings::reqMappingRequestedFeatures);
    if (mask.empty())
        m_requestDataMask = 0;
    else
        m_requestDataMask = MgUtil::StringToInt32(mask);
    m_iconLimitPerScaleRange = 25;
    STRING sIconsPerScaleRange = params->GetParameterValue(MgHttpResourceStrings::reqMappingIconsPerScaleRange);
    if (!sIconsPerScaleRange.empty())
    {
        m_iconLimitPerScaleRange = MgUtil::StringToInt32(sIconsPerScaleRange);
    }
    STRING sTargetMapName = params->GetParameterValue(MgHttpResourceStrings::reqMappingTargetMapName);
    if (!sTargetMapName.empty())
    {
        m_targetMapName = sTargetMapName;
    }
    m_iconFormat = MgImageFormats::Png;
    m_iconWidth = 16;
    m_iconHeight = 16;
    STRING sIconFormat = params->GetParameterValue(MgHttpResourceStrings::reqMappingIconFormat);
    if (!sIconFormat.empty())
    {
        m_iconFormat = sIconFormat;
    }
    STRING sIconWidth = params->GetParameterValue(MgHttpResourceStrings::reqMappingIconWidth);
    if (!sIconWidth.empty())
    {
        m_iconWidth = MgUtil::StringToInt32(sIconWidth);
    }
    STRING sIconHeight = params->GetParameterValue(MgHttpResourceStrings::reqMappingIconHeight);
    if (!sIconHeight.empty())
    {
        m_iconHeight = MgUtil::StringToInt32(sIconHeight);
    }
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpCreateRuntimeMap::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    Ptr<MgByteReader> byteReader;

    Ptr<MgResourceIdentifier> mdfId = new MgResourceIdentifier(m_mapDefinition);
    STRING mapName = mdfId->GetName();

    // Do we have a target map name? Use that instead
    if (!m_targetMapName.empty())
        mapName = m_targetMapName;

    // Create a new session id if we don't have one
    STRING sessionId = m_userInfo->GetMgSessionId();
    if (sessionId.empty())
    {
        // Get the site
        Ptr<MgSite> mgSite = m_siteConn->GetSite();
        // Run API command
        sessionId = mgSite->CreateSession();
        m_userInfo->SetMgSessionId(sessionId);
    }

    Ptr<MgMappingService> mappingService = (MgMappingService*)CreateService(MgServiceType::MappingService);
    byteReader = mappingService->CreateRuntimeMap(mdfId, mapName, sessionId, m_iconFormat, m_iconWidth, m_iconHeight, m_requestDataMask, m_iconLimitPerScaleRange);
    // Convert to requested response format, if necessary
    ProcessFormatConversion(byteReader);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpCreateRuntimeMap.Execute")
}

/// <summary>
/// This method is responsible for checking if
/// a valid version was given
/// </summary>
/// <returns>Returns nothing</returns>
void MgHttpCreateRuntimeMap::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    // There are multiple supported versions
    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(2,6,0))
    {
        throw new MgInvalidOperationVersionException(
        L"MgHttpCreateRuntimeMap.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpCreateRuntimeMap.ValidateOperationVersion");
}
