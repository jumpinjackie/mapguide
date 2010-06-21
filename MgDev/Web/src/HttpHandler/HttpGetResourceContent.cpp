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
#include "HttpGetResourceContent.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetResourceContent)

///----------------------------------------------------------------------------
/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
/// <param name="hRequest">Input
/// This contains all the parameters of the HTTP request.
/// </param>
///----------------------------------------------------------------------------

MgHttpGetResourceContent::MgHttpGetResourceContent(MgHttpRequest* hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> param = m_hRequest->GetRequestParam();

    // Get resource id.
    m_resourceId = param->GetParameterValue(MgHttpResourceStrings::reqResourceId);
}

///----------------------------------------------------------------------------
/// <summary>
/// Executes the specific request.
/// </summary>
/// <param name="hResponse">Output
/// This contains the HTTP response from the server.
/// </param>
///----------------------------------------------------------------------------

void MgHttpGetResourceContent::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters.
    ValidateCommonParameters();

    // Create ProxyResourceService instance.
    Ptr<MgResourceService> service = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

    // Create MgResourceIdentifier.
    MgResourceIdentifier resource(m_resourceId);

    // Run API command.
    Ptr<MgByteReader> byteReader = service->GetResourceContent(&resource);

    // Convert to requested response format, if necessary
    ProcessFormatConversion(byteReader);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetResourceContent.Execute")
}
