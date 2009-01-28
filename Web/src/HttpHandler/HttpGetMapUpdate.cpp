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
#include "HttpGetMapUpdate.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGetMapUpdate)

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
MgHttpGetMapUpdate::MgHttpGetMapUpdate(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_mapName = params->GetParameterValue(MgHttpResourceStrings::reqMappingMapName);
    m_seqNo = params->GetParameterValue(MgHttpResourceStrings::reqMappingSeqNo);
    m_dwfVersion = params->GetParameterValue(MgHttpResourceStrings::reqMappingDwfVersion);
    m_emapVersion = params->GetParameterValue(MgHttpResourceStrings::reqMappingEmapVersion);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpGetMapUpdate::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();
    Ptr<MgHttpRequestParam> params = m_hRequest->GetRequestParam();

    MG_HTTP_HANDLER_TRY()

    if (params->GetParameterValue(MgHttpResourceStrings::reqVersion) == L"")
    {
        MgStringCollection arguments;
        arguments.Add(L"0");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgHttpGetMapUpdate.Execute",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    Ptr<MgPropertyCollection> commands = params->GetParameters()->GetPropertyCollection();

    //execute the request through a dwf controller
    MgDwfController controller(m_siteConn,m_hRequest->GetAgentUri());
    Ptr<MgByteReader> reader = controller.GetMapUpdate(m_mapName,
                                                       MgUtil::StringToInt32(m_seqNo),
                                                       m_dwfVersion,
                                                       m_emapVersion,
                                                       commands);

    hResult->SetResultObject(reader, reader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGetMapUpdate.Execute")
}
