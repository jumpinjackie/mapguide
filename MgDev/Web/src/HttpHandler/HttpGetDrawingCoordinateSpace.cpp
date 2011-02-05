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
#include "HttpGetDrawingCoordinateSpace.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetDrawingCoordinateSpace)

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
MgHttpGetDrawingCoordinateSpace::MgHttpGetDrawingCoordinateSpace(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_dwfId = params->GetParameterValue(MgHttpResourceStrings::reqDrawingResourceId);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetDrawingCoordinateSpace::Execute(MgHttpResponse& hResponse)
{
    MgHttpResult* hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    MgResourceIdentifier dwfResId(m_dwfId);

    // Create Proxy Drawing Service instance
    Ptr<MgDrawingService> service = (MgDrawingService*)(CreateService(MgServiceType::DrawingService));

    // call the C++ API
    STRING result = service->GetCoordinateSpace(&dwfResId);

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(result);
    if (!value)
        throw new MgOutOfMemoryException(L"MgHttpGetDrawingCoordinateSpace.Exceute", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, MgMimeType::Text);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetDrawingCoordinateSpace.Execute")
}
