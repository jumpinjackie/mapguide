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
#include "HttpRenameResourceData.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpRenameResourceData)

///----------------------------------------------------------------------------
/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
/// <param name="hRequest">Input
/// This contains all the parameters of the HTTP request.
/// </param>
///----------------------------------------------------------------------------

MgHttpRenameResourceData::MgHttpRenameResourceData(MgHttpRequest* hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> param = m_hRequest->GetRequestParam();

    // Get resource id.
    m_resourceId = param->GetParameterValue(MgHttpResourceStrings::reqResourceId);

    // Get old data name.
    m_oldDataName = param->GetParameterValue(MgHttpResourceStrings::reqOldDataName);

    // Get new data name.
    m_newDataName = param->GetParameterValue(MgHttpResourceStrings::reqNewDataName);

    // Get the flag to determine whether or not the destination resource data
    // should be overwritten if it exists.
    m_overwrite = ::atoi(MgUtil::WideCharToMultiByte(
        param->GetParameterValue(
        MgHttpResourceStrings::reqOverwrite)).c_str()) != 0;
}

///----------------------------------------------------------------------------
/// <summary>
/// Executes the specific request.
/// </summary>
/// <param name="hResponse">Output
/// This contains the HTTP response from the server.
/// </param>
///----------------------------------------------------------------------------

void MgHttpRenameResourceData::Execute(MgHttpResponse& hResponse)
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

    // Run API command.
    service->RenameResourceData(&resource, m_oldDataName, m_newDataName, m_overwrite);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpRenameResourceData.Execute")
}
