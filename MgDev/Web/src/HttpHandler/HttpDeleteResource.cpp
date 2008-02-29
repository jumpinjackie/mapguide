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
#include "HttpDeleteResource.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpDeleteResource)

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
MgHttpDeleteResource::MgHttpDeleteResource(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Get Resource Id name
    m_resourceId = hrParam->GetParameterValue(MgHttpResourceStrings::reqResourceId);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpDeleteResource::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters
    ValidateCommonParameters();

    // Create ProxyResourceService instance
    Ptr<MgResourceService> mgprService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

    // Create MgResourceIdentifier
    MgResourceIdentifier mgrIdentifier(m_resourceId);

    // Run API command
    mgprService->DeleteResource(&mgrIdentifier);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpDeleteResource.Execute")
}
