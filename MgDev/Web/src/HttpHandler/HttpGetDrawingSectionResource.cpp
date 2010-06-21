//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "HttpGetDrawingSectionResource.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetDrawingSectionResource)

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
MgHttpGetDrawingSectionResource::MgHttpGetDrawingSectionResource(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    m_resId = params->GetParameterValue(MgHttpResourceStrings::reqDrawingResourceId);
    m_resName = params->GetParameterValue(MgHttpResourceStrings::reqDrawingResourceName);

    // fix potential double backslashes. This is is due to a bug in PHP for Windows which prevents us
    // to override the variable magic_quotes_gpc. This variable, when set to 'On', causes backslashes
    // to be doubled. On Linux, we can override this variable for our script to ensure that backslashes
    // are never doubled by PHP.
#ifdef _WIN32
    m_resName = MgUtil::ReplaceString(m_resName, L"\\\\", L"\\");
#endif
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetDrawingSectionResource::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    MgResourceIdentifier resId(m_resId);

    // Create Proxy Drawing Service instance
    Ptr<MgDrawingService> service = (MgDrawingService*)(CreateService(MgServiceType::DrawingService));

    // call the C++ API
    Ptr<MgByteReader> byteReader = service->GetSectionResource(&resId, m_resName);
    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetDrawingSectionResource.Execute")
}
