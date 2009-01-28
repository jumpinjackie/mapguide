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
#include "HttpCopyResource.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpCopyResource)

///----------------------------------------------------------------------------
/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
/// <param name="hRequest">Input
/// MgHttpRequest
/// This contains all the parameters of the request.
/// </param>
/// <returns>
/// nothing
/// </returns>
///----------------------------------------------------------------------------

MgHttpCopyResource::MgHttpCopyResource(MgHttpRequest* hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Get source resource ID
    m_sourceResourceId = hrParam->GetParameterValue(MgHttpResourceStrings::reqSourceResourceId);

    // Get destination resource ID
    m_destResourceId = hrParam->GetParameterValue(MgHttpResourceStrings::reqDestinationResourceId);

    // Get the flag to determine whether or not the destination resource
    // should be overwritten if it exists.
    m_overwrite = ::atoi(MgUtil::WideCharToMultiByte(
        hrParam->GetParameterValue(
        MgHttpResourceStrings::reqOverwrite)).c_str()) != 0;
}

///----------------------------------------------------------------------------
/// <summary>
/// Executes the specific request.
/// </summary>
/// <param name="hResponse">Output
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </param>
/// <returns>
/// nothing
/// </returns>
///----------------------------------------------------------------------------

void MgHttpCopyResource::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters
    ValidateCommonParameters();

    // Create ProxyResourceService instance
    Ptr<MgResourceService> mgprService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

    // Create MgResourceIdentifier
    MgResourceIdentifier mgrSourceIdentifier(m_sourceResourceId);
    MgResourceIdentifier mgrDestIdentifier(m_destResourceId);

    // Run API command
    mgprService->CopyResource(&mgrSourceIdentifier, &mgrDestIdentifier, m_overwrite);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpCopyResource.Execute")
}
