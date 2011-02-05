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
#include "HttpSetResourceData.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpSetResourceData)

///----------------------------------------------------------------------------
/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
/// <param name="hRequest">Input
/// This contains all the parameters of the HTTP request.
/// </param>
///----------------------------------------------------------------------------

MgHttpSetResourceData::MgHttpSetResourceData(MgHttpRequest* hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> param = m_hRequest->GetRequestParam();

    // Get resource id.
    m_resourceId = param->GetParameterValue(MgHttpResourceStrings::reqResourceId);

    // Get data name.
    m_dataName = param->GetParameterValue(MgHttpResourceStrings::reqDataName);

    // Get data type.
    m_dataType = param->GetParameterValue(MgHttpResourceStrings::reqDataType);

    // Get data length.
    STRING dataLength = param->GetParameterValue(MgHttpResourceStrings::reqDataLength);
    m_dataLength = MgUtil::StringToInt32(dataLength);

    // Get data.
    STRING dataFileName = param->GetParameterValue(MgHttpResourceStrings::reqData);
    STRING isTemp = param->GetParameterValue(dataFileName);
    m_data = new MgByteSource(dataFileName, isTemp.length() > 0);

    // Get mime type.
    m_data->SetMimeType(param->GetParameterType(MgHttpResourceStrings::reqData));
}

///----------------------------------------------------------------------------
/// <summary>
/// Executes the specific request.
/// </summary>
/// <param name="hResponse">Output
/// This contains the HTTP response from the server.
/// </param>
///----------------------------------------------------------------------------

void MgHttpSetResourceData::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult;

    MG_HTTP_HANDLER_TRY()

    hResult = hResponse.GetResult();

    // Check common parameters.
    ValidateCommonParameters();

    // Create ProxyResourceService instance.
    Ptr<MgResourceService> service = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

    // Create MgResourceIdentifier.
    MgResourceIdentifier resource(m_resourceId);

    // Create byte reader to read in data.
    Ptr<MgByteReader> byteReader = m_data->GetReader();

    // Run API command.
    service->SetResourceData(&resource, m_dataName, m_dataType, byteReader);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpSetResourceData.Execute")
}

MgHttpSetResourceData::~MgHttpSetResourceData()
{
}
