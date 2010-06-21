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
#include "HttpWfsGetCapabilities.h"

#include "OgcFramework.h"
#include "XmlParser.h"
#include "OgcWfsServer.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpWfsGetCapabilities)

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
MgHttpWfsGetCapabilities::MgHttpWfsGetCapabilities(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);
}


// This simple function is handed into the MgOwsServer system, as
// a means to resolve documents.
extern bool GetDocument(CPSZ pszDoc,REFSTRING sRet);

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpWfsGetCapabilities::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Create Proxy Feature Service instance
    // Note: these things must have a lifetime that exceeds
    //      MgWfsFeatureDefinitions Features, below.
    Ptr<MgResourceService> pResourceService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));
    Ptr<MgFeatureService> pFeatureService = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));
    //
    MgWfsFeatureDefinitions Features(pResourceService,pFeatureService);

    // We have to wrap the request parameters, since the outside
    // world is case-sensitive (with respect to names,) but
    // we need our parameters NOT to be so.
    Ptr<MgHttpRequestParam> origReqParams = m_hRequest->GetRequestParam();
    MgHttpRequestParameters Parms(origReqParams);
    MgHttpResponseStream Out;

    MgOgcServer::SetLoader(GetDocument);

    MgUserInformation::SetCurrentUserInfo(m_userInfo);

    // Instance a server-lette
    MgOgcWfsServer Wfs(Parms,Out,Features);

    // Execute the request
    Wfs.ProcessRequest(this);

    // Slurp the results.
    Ptr<MgByteReader> capabilities = Out.Stream().GetReader();

    // Set the result
    hResult->SetResultObject(capabilities, capabilities->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpWfsGetCapabilities.Execute")
}

/// <summary>
/// Attempts to process a request made by POSTing an XML document
/// </summary>
/// <param name="hRequest">Input
/// Contains the request params and data
/// </param>
/// <param name="hResponse">Input
/// The response object into which to write the response data
/// </param>
/// <returns>
/// True if this method can process this request type
/// </returns>
bool MgHttpWfsGetCapabilities::ProcessPostRequest(MgHttpRequest *hRequest, MgHttpResponse& hResponse)
{
    //Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    //TODO Parse params->GetXmlPostData();
    return false;
}
