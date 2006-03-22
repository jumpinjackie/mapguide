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
    MgOgcWfsServer wfsServer(requestParams, responseStream);
    wfsServer.ProcessRequest(this);

    // Obtain the response byte reader
    Ptr<MgByteReader> responseReader = responseStream.Stream().GetReader();

    // Set the result
    hResult->SetResultObject(responseReader, responseReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpWfsGetFeature.Execute")
}

// Acquire data required to validate the request
void MgHttpWfsGetFeature::AcquireValidationData(MgOgcServer* ogcServer)
{
    MgOgcWfsServer* wfsServer = (MgOgcWfsServer*)ogcServer;
    if(wfsServer != NULL)
    {
        wfsServer->SetGetFeatureRequestParams(m_getFeatureParams);
    }
}

// Acquire data required to generate the response
void MgHttpWfsGetFeature::AcquireResponseData(MgOgcServer* ogcServer)
{
    MgOgcWfsServer* wfsServer = (MgOgcWfsServer*)ogcServer;
    if(wfsServer != NULL)
    {
        string responseString = "";

        // Loop through the requested feature types
        if(m_getFeatureParams != NULL)
        {
            Ptr<MgStringCollection> featureTypeList = m_getFeatureParams->GetFeatureTypes();
            if(featureTypeList != NULL && featureTypeList->GetCount() > 0)
            {
                // Create an instance of the feature service
                Ptr<MgFeatureService> featureService = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));
                
                int numFeaturesRetrieved = 0;
                int maxFeatures = m_getFeatureParams->GetMaxFeatures();
                for(int i = 0; i < featureTypeList->GetCount(); i++)
                {
                    // Check to see if we've already retrieved the maximum
                    // number of features
                    if(maxFeatures > 0 && numFeaturesRetrieved >= maxFeatures)
                    {
                        break;
                    }

                    STRING featureType = featureTypeList->GetItem(i);
                    int delimPos = (int)featureType.find_last_of(L":");
                    if(delimPos != wstring::npos)
                    {
                        STRING featureSource = featureType.substr(0, delimPos);
                        STRING fullFeatureSource = L"Library://" + featureSource + L".FeatureSource";
                        STRING featureClass = featureType.substr(delimPos + 1);

                        Ptr<MgResourceIdentifier> featureSourceId = new MgResourceIdentifier(fullFeatureSource);

                        Ptr<MgStringCollection> requiredProperties;
                        requiredProperties = NULL;
                        Ptr<MgStringCollection> requiredPropertiesList = m_getFeatureParams->GetRequiredProperties();
                        if(requiredPropertiesList != NULL && requiredPropertiesList->GetCount() > i)
                        {
                            STRING requiredPropertiesString = requiredPropertiesList->GetItem(i);
                            if(requiredPropertiesString.length() > 0)
                            {
                                requiredProperties = MgStringCollection::ParseCollection(requiredPropertiesString, L",");
                            }
                        }

                        STRING filter = L"";
                        Ptr<MgStringCollection> filterList = m_getFeatureParams->GetFilterStrings();
                        if(filterList->GetCount() > i)
                        {
                            filter = filterList->GetItem(i);
                        }
                        else if(filterList->GetCount() == 1)
                        {
                            // If there's only one filter in the list, use it for all typenames
                            filter = filterList->GetItem(0);
                        }

                        // Calculate the max remaining features to retrieve
                        int numFeaturesToRetrieve = maxFeatures;
                        if(maxFeatures > 0)
                        {
                            numFeaturesToRetrieve = maxFeatures - numFeaturesRetrieved;
                            if(numFeaturesToRetrieve <= 0)
                            {
                                // We have all the features we need, so break out of the loop
                                break;
                            }
                        }

                        // Call the C++ API
                        Ptr<MgByteReader> resultReader = featureService->GetWfsFeature(featureSourceId, featureClass,
                            requiredProperties, m_getFeatureParams->GetSrs(), filter, numFeaturesToRetrieve);

                        // TODO How to determine number of features retrieved...?
                        // Note: for now, maxFeatures is managed by the MgWfsFeatures object. - TMT 2006-3-20
                        // numFeaturesRetrieved += ?

                        // Write the byte reader's data into our response data object
                        string thisResponseString;
                        resultReader->ToStringUtf8(thisResponseString);

                        // just append the entire thing; there's important stuff, like namespace declarations
                        // that we would lose if we just extracted the <featureMember> elements.
                        // The MgWfsFeatures object will parse the pseudo-XML that results.
                        responseString += thisResponseString;
                    }
                }
            }
        }
        if(responseString.length() > 0)
        {
            STRING wResponseString = MgUtil::MultiByteToWideChar(responseString);
            Ptr<MgWfsFeatures> features = new MgWfsFeatures(wResponseString.c_str(),m_getFeatureParams->GetMaxFeatures());
            wfsServer->SetFeatures(features);
        }
    }
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


