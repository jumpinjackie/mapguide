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
#include "HttpTestConnection.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpTestConnection)

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
MgHttpTestConnection::MgHttpTestConnection(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // either resource is specified or  provider and connection string are specified
    m_resId = params->GetParameterValue(MgHttpResourceStrings::reqFeatResourceId);
    if(m_resId == L"")
    {
        m_providerName = params->GetParameterValue(MgHttpResourceStrings::reqFeatProvider);
        m_connectionString = params->GetParameterValue(MgHttpResourceStrings::reqFeatConnectionString);
    }
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpTestConnection::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    // Create Proxy Feature Service instance
    Ptr<MgFeatureService> service = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));

    // call the C++ API
    bool ret;
    if(m_resId.length() != 0)
    {
        MgResourceIdentifier resId(m_resId);
        ret = service->TestConnection(&resId);
    }
    else
        ret = service->TestConnection(m_providerName, m_connectionString);

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(ret);
    if(!value)
        throw new MgOutOfMemoryException(L"", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, MgMimeType::Text);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpTestConnection.Execute")
}
