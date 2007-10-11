//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "HttpCsEnumerateCategories.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpCsEnumerateCategories)

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
MgHttpCsEnumerateCategories::MgHttpCsEnumerateCategories(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get format.
    m_format = params->GetParameterValue(MgHttpResourceStrings::format);
    if (m_format == L"")
    {
        // Default to XML response format
        m_format = MgMimeType::Xml;
    }
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpCsEnumerateCategories::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    Ptr<MgCoordinateSystemFactory> factory;
    Ptr<MgStringCollection> categories = factory->EnumerateCategories();
    Ptr<MgByteReader> byteReader;
    if (m_format == MgMimeType::Xml)
    {
        byteReader = categories->ToXml();
    }
    else if (m_format == MgMimeType::Json)
    {
        byteReader = categories->ToJson();
    }
    else
    {
        MgStringCollection arguments;
        arguments.Add(m_format);

        throw new MgInvalidFormatException(L"MgHttpCsEnumerateCategories.Execute",
            __LINE__,__WFILE__, &arguments, L"", NULL);
    }
    STRING xmlSchema = byteReader->ToString();

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(xmlSchema);
    if(!value)
        throw new MgOutOfMemoryException(L"", __LINE__, __WFILE__, NULL, L"", NULL);

    hResult->SetResultObject(value, m_format);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpCsEnumerateCategories.Execute")
}
