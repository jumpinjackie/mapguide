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
#include "HttpUpdateRepository.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpUpdateRepository)

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
MgHttpUpdateRepository::MgHttpUpdateRepository(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Get Resource Repository name
    m_resourceId = hrParam->GetParameterValue(MgHttpResourceStrings::reqResourceId);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpUpdateRepository::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters
    ValidateCommonParameters();

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Create ProxyResourceService instance
    Ptr<MgResourceService> mgprService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

    // Create MgResourceIdentifier
    MgResourceIdentifier mgrIdentifier(m_resourceId);

    Ptr<MgByteReader> resourceByteReader;
    Ptr<MgByteReader> headerByteReader;
    Ptr<MgByteSource> resourceByteSource;
    Ptr<MgByteSource> headerByteSource;

    // Get resource content
    if (hrParam->GetParameterValue(MgHttpResourceStrings::reqResourceContent) != L"")
    {
        STRING resourceFileName = hrParam->GetParameterValue(MgHttpResourceStrings::reqResourceContent);
        STRING isTemp = hrParam->GetParameterValue(resourceFileName);
        resourceByteSource = new MgByteSource(resourceFileName, isTemp.length() > 0);
        resourceByteSource->SetMimeType(L"text/xml");
        resourceByteReader = resourceByteSource->GetReader();
    }

    // Get resource header
    if (hrParam->GetParameterValue(MgHttpResourceStrings::reqResourceHeader) != L"")
    {
        STRING headerFileName = hrParam->GetParameterValue(MgHttpResourceStrings::reqResourceHeader);
        STRING isTemp = hrParam->GetParameterValue(headerFileName);
        headerByteSource = new MgByteSource(headerFileName, isTemp.length() > 0);
        headerByteSource->SetMimeType(L"text/xml");
        headerByteReader = headerByteSource->GetReader();
    }

    // Run API command
    mgprService->UpdateRepository(&mgrIdentifier, resourceByteReader, headerByteReader);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpUpdateRepository.Execute")
}
