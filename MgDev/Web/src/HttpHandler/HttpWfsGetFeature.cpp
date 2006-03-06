//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "WfsGetFeatureParams.h"
#include "HttpWfsGetFeature.h"
#include "WmsMapUtil.h"

#include "OgcFramework.h"
#include "OgcWfsServer.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpWfsGetFeature)

const string startMemberElement = MgUtil::WideCharToMultiByte(L"<gml:featureMember>");
const string endMemberElement = MgUtil::WideCharToMultiByte(L"</gml:featureMember>");

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
MgHttpWfsGetFeature::MgHttpWfsGetFeature(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    m_getFeatureParams = new WfsGetFeatureParams(params);
}

MgHttpWfsGetFeature::MgHttpWfsGetFeature(MgHttpRequest *hRequest, WfsGetFeatureParams *params)
{
    InitializeCommonParameters(hRequest);
    m_getFeatureParams = SAFE_ADDREF((WfsGetFeatureParams*)params);
}

// Default constructor to keep Ptr<> happy
MgHttpWfsGetFeature::MgHttpWfsGetFeature()
{
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
void MgHttpWfsGetFeature::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Create Proxy Feature Service instance
    Ptr<MgFeatureService> featureService = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));

    MgWfsFeatures retrievedFeatures(m_getFeatureParams, featureService);

    // Make sure the REQUEST and SERVICE parameters are set, in case this came from
    // a POST request.
    Ptr<MgHttpRequestParam> origReqParams = m_hRequest->GetRequestParam();
    if(!origReqParams->ContainsParameter(MgHttpResourceStrings::reqWmsRequest))
    {
        origReqParams->AddParameter(MgHttpResourceStrings::reqWmsRequest, L"GetFeature"); //NOXLATE
    }
    if(!origReqParams->ContainsParameter(MgHttpResourceStrings::reqWmsService))
    {
        origReqParams->AddParameter(MgHttpResourceStrings::reqWmsService, L"WFS"); //NOXLATE
    }
    MgHttpRequestParameters requestParams(origReqParams);

    MgHttpResponseStream responseStream;
    MgOgcServer::SetLoader(GetDocument);

    // Create the WFS Server object and respond to the request
    MgOgcWfsServer wfsServer(requestParams, responseStream, retrievedFeatures);
    wfsServer.RespondToRequest();

    // Obtain the response byte reader
    Ptr<MgByteReader> responseReader = responseStream.Stream().GetReader();

    // Set the result
    hResult->SetResultObject(responseReader, responseReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpWfsGetFeature.Execute")
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
bool MgHttpWfsGetFeature::ProcessPostRequest(MgHttpRequest *hRequest, MgHttpResponse& hResponse)
{
    bool bValid = false;
    string xmlString = hRequest->GetRequestParam()->GetXmlPostData();
    if(xmlString.length() > 0)
    {
        STRING wxmlString = MgUtil::MultiByteToWideChar(xmlString);
        bValid = WfsGetFeatureParams::IsValidXmlRequest(wxmlString);
        if(bValid)
        {
            Ptr<WfsGetFeatureParams> featureParams = new WfsGetFeatureParams(wxmlString);
            Ptr<MgHttpWfsGetFeature> getFeatureHandler = new MgHttpWfsGetFeature(hRequest, featureParams);
            getFeatureHandler->Execute(hResponse);
        }
    }
    return bValid;
}


