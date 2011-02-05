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
#include "HttpGetDrawingLayer.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetDrawingLayer)

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
MgHttpGetDrawingLayer::MgHttpGetDrawingLayer(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    m_resId = params->GetParameterValue(MgHttpResourceStrings::reqDrawingResourceId);
    m_sectionName = params->GetParameterValue(MgHttpResourceStrings::reqDrawingSectionName);
    m_layerName = params->GetParameterValue(MgHttpResourceStrings::reqDrawingLayerName);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetDrawingLayer::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    MgResourceIdentifier resId(m_resId);

    // Create Proxy Drawing Service instance
    Ptr<MgDrawingService> service = (MgDrawingService*)(CreateService(MgServiceType::DrawingService));

    // call the C++ API
    Ptr<MgByteReader> byteReaderResult = service->GetLayer(&resId, m_sectionName, m_layerName);
    hResult->SetResultObject(byteReaderResult, byteReaderResult->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetDrawingLayer.Execute")
}
