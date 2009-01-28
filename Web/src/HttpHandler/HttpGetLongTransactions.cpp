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
#include "HttpGetLongTransactions.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetLongTransactions)

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
MgHttpGetLongTransactions::MgHttpGetLongTransactions(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_resId = params->GetParameterValue(MgHttpResourceStrings::reqFeatResourceId);
    STRING activeOnly = params->GetParameterValue(MgHttpResourceStrings::reqFeatActiveOnly);

    MG_HTTP_HANDLER_TRY()

    if (activeOnly == L"1")
        m_activeOnly = true;
    else if (activeOnly == L"0")
        m_activeOnly = false;
    else if (activeOnly.empty())
        m_activeOnly = false;
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"0");
        arguments.Add(activeOnly);

        throw new MgInvalidArgumentException(L"MgHttpGetLongTransactions.MgHttpGetLongTransactions",
            __LINE__, __WFILE__, &arguments, L"MgInvalidValueOutsideRange", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpGetLongTransactions.MgHttpGetLongTransactions")
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetLongTransactions::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    MgResourceIdentifier resId(m_resId);

    // Create Proxy Feature Service instance
    Ptr<MgFeatureService> service = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));

    // call the C++ API
    Ptr<MgLongTransactionReader> ltReader = service->GetLongTransactions(&resId, m_activeOnly);
    Ptr<MgByteReader> byteReader = ltReader->ToXml();

    //Convert to alternate response format, if necessary
    ProcessFormatConversion(byteReader);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetLongTransactions.Execute")
}
