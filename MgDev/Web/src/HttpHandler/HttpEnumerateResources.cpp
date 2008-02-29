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
#include "HttpEnumerateResources.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpEnumerateResources)

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
MgHttpEnumerateResources::MgHttpEnumerateResources(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Get resource id
    m_resourceId = hrParam->GetParameterValue(MgHttpResourceStrings::reqResourceId);

    // Get depth
    m_depth = MgUtil::StringToInt32(hrParam->GetParameterValue(MgHttpResourceStrings::reqDepth));

    // Get type
    m_type = hrParam->GetParameterValue(MgHttpResourceStrings::reqType);

    // In order to maintain backward compatibility, the Compute Children flag
    // will be set to true by default if it is not specified.
    STRING computeChildrenParam = hrParam->GetParameterValue(MgHttpResourceStrings::reqComputeChildren);

    m_computeChildren = (computeChildrenParam.empty() || 0 != MgUtil::StringToInt32(computeChildrenParam));
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpEnumerateResources::Execute(MgHttpResponse& hResponse)
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
    Ptr<MgByteReader> byteReader = mgprService->EnumerateResources(&mgrIdentifier, m_depth, m_type, m_computeChildren);

    // Convert to requested response format, if necessary
    ProcessFormatConversion(byteReader);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpEnumerateResources.Execute")
}
