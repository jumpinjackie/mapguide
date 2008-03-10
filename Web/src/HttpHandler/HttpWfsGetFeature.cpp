//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
: m_getFeatureParams(NULL)
{
    InitializeCommonParameters(hRequest);

    // Deferred until AcquireValidationData call
    //Ptr<MgHttpRequestParam> origReqParams = m_hRequest->GetRequestParam();
    m_getFeatureParams = NULL; // new WfsGetFeatureParams(origReqParams);
}

MgHttpWfsGetFeature::MgHttpWfsGetFeature(MgHttpRequest *hRequest, CREFSTRING sPostRequestXml /*WfsGetFeatureParams *params*/)
: m_getFeatureParams(NULL)
, m_sPostRequestXml(sPostRequestXml)
{
    InitializeCommonParameters(hRequest);
    // Deferred until AcquireValidationData call
    //m_getFeatureParams = SAFE_ADDREF((WfsGetFeatureParams*)params);
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

    MgUserInformation::SetCurrentUserInfo(m_userInfo);

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
        // We've put this off long enough.  Now we have a WFS server,
        // so we can create the parameters collection, as pre-processed
        // by the server's templates.
        if(m_sPostRequestXml.length() > 0)
            m_getFeatureParams = new WfsGetFeatureParams(*wfsServer,m_sPostRequestXml);
        else
            m_getFeatureParams = new WfsGetFeatureParams(*wfsServer);

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
                // Create required services
                Ptr<MgFeatureService> featureService = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));
                Ptr<MgResourceService> resourceService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

                MgWfsFeatureDefinitions oFeatureTypes(resourceService,featureService,featureTypeList);

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

                    // Get a name of a feature type, like "ns12345:Foo"
                    STRING sFeatureType = featureTypeList->GetItem(i);
                    STRING::size_type delimPos = sFeatureType.find_last_of(L":");
                    // Is it well-formed?  must take QName form: prefix-colon-classname
                    if(delimPos != STRING::npos)
                    {
                        // If so, break it up into its constituent parts
                        STRING sPrefix = sFeatureType.substr(0, delimPos);
                        STRING sClass  = sFeatureType.substr(delimPos+1);
                        STRING sSchemaHash;

                        // Given the prefix, reconstitute the resource name
                        // If from HTTP POST, the namespace manager should have a record of it.
                        STRING sResource = m_getFeatureParams->NamespaceManager().NamespaceFrom(sPrefix);

                        // If so, we're good.  If not (as might be the case for HTTP GET) try to guess what
                        // it might be by decoding the prefix's hash... this is fallible if the caller decided
                        // to use a different prefix, but didn't communicate the namespace associated with that
                        // prefix... buuuut.... there are limits to our tolerance.
                        if(sResource.length() > 0 || oFeatureTypes.PrefixToFeatureSource(sPrefix,sResource, sSchemaHash)) {
                            // From these inquiries, we now have a resource ID, "Library://...FeatureSource"
                            // and the class name (the "Foo" part from the example above.)
                            Ptr<MgResourceIdentifier> featureSourceId = new MgResourceIdentifier(sResource);

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
                            Ptr<MgByteReader> resultReader = featureService->GetWfsFeature(featureSourceId, ((sSchemaHash.size()==0) ? sClass : sSchemaHash + _(":") + sClass),
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
    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    string xmlString = params->GetXmlPostData();
    if(xmlString.length() > 0)
    {
        STRING wxmlString = MgUtil::MultiByteToWideChar(xmlString);
        bValid = WfsGetFeatureParams::IsValidXmlRequest(wxmlString);
        if(bValid)
        {
            //Ptr<WfsGetFeatureParams> featureParams = new WfsGetFeatureParams(wxmlString);
            // We (that is, *this* MgHttpWfsGetFeature) doesn't handle the POST request.
            // We clone another one of ourselves, dummying up the parameters, and let it
            // do our work for us.
            Ptr<MgHttpWfsGetFeature> getFeatureHandler = new MgHttpWfsGetFeature(hRequest, wxmlString /* featureParams*/);
            getFeatureHandler->Execute(hResponse);
        }
    }
    return bValid;
}
