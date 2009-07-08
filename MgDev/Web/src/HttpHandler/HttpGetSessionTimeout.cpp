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
#include "HttpGetSessionTimeout.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetSessionTimeout)

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
MgHttpGetSessionTimeout::MgHttpGetSessionTimeout(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetSessionTimeout::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

        hResult = hResponse.GetResult();

    // Check common parameters
    ValidateCommonParameters();

    // Get the site
    Ptr<MgSite> mgSite = m_siteConn->GetSite();

    // Run API command
    INT32 ret = mgSite->GetSessionTimeout();

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(ret);
    if(!value)
        throw new MgOutOfMemoryException(L"MgHttpGetSessionTimeout.Execute", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, MgMimeType::Text);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetSessionTimeout.Execute")
}
