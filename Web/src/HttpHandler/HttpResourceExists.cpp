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
#include "HttpResourceExists.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpResourceExists)

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

MgHttpResourceExists::MgHttpResourceExists(MgHttpRequest* hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Get resource ID.
    m_resourceId = hrParam->GetParameterValue(MgHttpResourceStrings::reqResourceId);
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

void MgHttpResourceExists::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters.
    ValidateCommonParameters();

    // Create ProxyResourceService instance.
    Ptr<MgResourceService> service = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

    // Create MgResourceIdentifier object.
    MgResourceIdentifier resource(m_resourceId);

    // Run API command.
    bool existed = service->ResourceExists(&resource);

    // Set HTTP result.
    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(existed);

    if (NULL == value.p)
    {
        throw new MgOutOfMemoryException(L"MgHttpResourceExists.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    hResult->SetResultObject(value, MgMimeType::Text);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpResourceExists.Execute")
}
