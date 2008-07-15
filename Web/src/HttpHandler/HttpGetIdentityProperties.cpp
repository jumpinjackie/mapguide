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
#include "HttpGetIdentityProperties.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetIdentityProperties)

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
MgHttpGetIdentityProperties::MgHttpGetIdentityProperties(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_resId = params->GetParameterValue(MgHttpResourceStrings::reqFeatResourceId);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetIdentityProperties::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    MgResourceIdentifier resId(m_resId);

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();
    STRING schema = hrParam->GetParameterValue(MgHttpResourceStrings::reqFeatSchema);
    STRING className = hrParam->GetParameterValue(MgHttpResourceStrings::reqFeatClass);

    // Create Proxy Feature Service instance
    Ptr<MgFeatureService> service = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));

    // call the C++ APIs
    Ptr<MgPropertyDefinitionCollection> identityProps = service->GetIdentityProperties(&resId, schema, className);
    Ptr<MgByteReader> byteReader = identityProps->ToXml();

    //Convert to alternate response format, if necessary
    ProcessFormatConversion(byteReader);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetIdentityProperties.Execute")
}
