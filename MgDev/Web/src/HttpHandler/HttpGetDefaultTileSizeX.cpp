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
#include "HttpGetDefaultTileSizeX.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetDefaultTileSizeX)

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
MgHttpGetDefaultTileSizeX::MgHttpGetDefaultTileSizeX(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    INT32 version = m_userInfo->GetApiVersion();
    if (version == MG_API_VERSION(3,0,0))
    {
        m_tilesetId = params->GetParameterValue(MgHttpResourceStrings::reqTileSet);
    }
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetDefaultTileSizeX::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    // Get Proxy Tile Service instance
    Ptr<MgTileService> service = (MgTileService*)(CreateService(MgServiceType::TileService));
    Ptr<MgHttpPrimitiveValue> value;
    INT32 version = m_userInfo->GetApiVersion();
    if (version == MG_API_VERSION(3, 0, 0) && !m_tilesetId.empty())
    {
        Ptr<MgResourceIdentifier> tileSetId = new MgResourceIdentifier(m_tilesetId);
        INT32 size = service->GetDefaultTileSizeX(tileSetId);
        value = new MgHttpPrimitiveValue(size);
    }
    else
    {
        INT32 size = service->GetDefaultTileSizeX();
        value = new MgHttpPrimitiveValue(size);
    }
    if(!value)
        throw new MgOutOfMemoryException(L"", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, MgMimeType::Text);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetDefaultTileSizeX.Execute")
}

/// <summary>
/// This method is responsible for checking if
/// a valid version was given
/// </summary>
/// <returns>Returns nothing</returns>
void MgHttpGetDefaultTileSizeX::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    // There are multiple supported versions
    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(1,0,0) &&
        version != MG_API_VERSION(3,0,0))
    {
        throw new MgInvalidOperationVersionException(
        L"MgHttpGetDefaultTileSizeX.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpGetDefaultTileSizeX.ValidateOperationVersion");
}