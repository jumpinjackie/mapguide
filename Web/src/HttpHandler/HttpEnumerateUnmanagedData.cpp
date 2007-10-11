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
#include "HttpEnumerateUnmanagedData.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpEnumerateUnmanagedData)

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
MgHttpEnumerateUnmanagedData::MgHttpEnumerateUnmanagedData(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Get path
    m_path = hrParam->GetParameterValue(MgHttpResourceStrings::reqPath);

    // Get the recursive flag
    STRING recursive = hrParam->GetParameterValue(MgHttpResourceStrings::reqRecursive);

    MG_HTTP_HANDLER_TRY()

    if (recursive == L"1")
        m_recursive = true;
    else if (recursive == L"0" || recursive == L"")
        m_recursive = false;
    else
    {
        MgStringCollection arguments;
        arguments.Add(MgHttpResourceStrings::reqRecursive);
        arguments.Add(recursive);

        throw new MgInvalidArgumentException(L"MgHttpEnumerateUnmanagedData.MgHttpEnumerateUnmanagedData",
            __LINE__, __WFILE__, &arguments, L"MgInvalidValueOutsideRange", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpEnumerateUnmanagedData.MgHttpEnumerateUnmanagedData")

    // Get type
    m_type = hrParam->GetParameterValue(MgHttpResourceStrings::reqType);

    // Get filter
    m_filter = hrParam->GetParameterValue(MgHttpResourceStrings::reqFilter);

    // Get format.
    m_format = hrParam->GetParameterValue(MgHttpResourceStrings::format);
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
void MgHttpEnumerateUnmanagedData::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters
    ValidateCommonParameters();

    // Check response format
    if (m_format != MgMimeType::Xml && m_format != MgMimeType::Json)
    {
        MgStringCollection arguments;
        arguments.Add(m_format);

        throw new MgInvalidFormatException(L"MgHttpEnumerateUnmanagedData::Execute",
            __LINE__,__WFILE__, &arguments, L"", NULL);
    }

    // Create ProxyResourceService instance
    Ptr<MgResourceService> mgprService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

    // call the C++ API
    Ptr<MgByteReader> byteReaderResult = mgprService->EnumerateUnmanagedData(
        m_path, m_recursive, m_type, m_filter, m_format);
    hResult->SetResultObject(byteReaderResult, byteReaderResult->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpEnumerateUnmanagedData.Execute")
}
