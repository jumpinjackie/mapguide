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
#include "HttpApplyResourcePackage.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpApplyResourcePackage)

///----------------------------------------------------------------------------
/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
///
/// <param name="hRequest">Input
/// HTTP request sent to the server.
/// </param>
///----------------------------------------------------------------------------

MgHttpApplyResourcePackage::MgHttpApplyResourcePackage(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> hrParam = m_hRequest->GetRequestParam();

    // Get package.
    STRING packageFileName = hrParam->GetParameterValue(MgHttpResourceStrings::reqPackage);
    STRING isTemp = hrParam->GetParameterValue(packageFileName);
    m_package = new MgByteSource(packageFileName, isTemp.length() > 0);
    m_package->SetMimeType(hrParam->GetParameterType(MgHttpResourceStrings::reqPackage));
}

///----------------------------------------------------------------------------
/// <summary>
/// Executes the specific request.
/// </summary>
///
/// <param name="hResponse">Output
/// HTTP response received from the server.
/// </param>
///----------------------------------------------------------------------------

void MgHttpApplyResourcePackage::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters
    ValidateCommonParameters();

    // Create ProxyResourceService instance
    Ptr<MgResourceService> service = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

    // Create byte reader to read in package data
    Ptr<MgByteReader> byteReader = m_package->GetReader();

    // Run API command
    service->ApplyResourcePackage(byteReader);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpApplyResourcePackage.Execute")
}
