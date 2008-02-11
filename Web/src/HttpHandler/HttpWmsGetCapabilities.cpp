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
#include "HttpWmsGetCapabilities.h"

#include "OgcFramework.h"
#include "XmlParser.h"
#include "OgcWmsServer.h"
#include "WmsLayerDefinitions.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpWmsGetCapabilities)

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
MgHttpWmsGetCapabilities::MgHttpWmsGetCapabilities(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);
}


// This simple function is handed into the MgOwsServer system, as
// a means to resolve documents.
bool GetDocument(CPSZ pszDoc,REFSTRING sRet)
{
    //TODO: Remove dependency on thread local storage
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
    Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
    serverAdmin->Open(userInfo);

    Ptr<MgByteReader> brDoc;
    brDoc = serverAdmin->GetDocument(pszDoc);

    sRet = brDoc->ToString();
    return sRet.length() > 0;
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpWmsGetCapabilities::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // We have to wrap the request parameters, since the outside
    // world is case-sensitive (with respect to names,) but
    // we need our parameters NOT to be so.
    Ptr<MgHttpRequestParam> origReqParams = m_hRequest->GetRequestParam();
    MgHttpRequestParameters requestParams(origReqParams);
    MgHttpResponseStream responseStream;

    // Declare the method we'd like the system to use for resolving
    // loader documents.
    // (Must be done before instancing an MgOgc{Wms}Server)
    MgOgcServer::SetLoader(GetDocument);

    MgUserInformation::SetCurrentUserInfo(m_userInfo);

    // Instance a server-lette
    MgOgcWmsServer wms(requestParams, responseStream);

    // Execute the request
    wms.ProcessRequest(this);

    // Slurp the results.
    Ptr<MgByteReader> capabilities = responseStream.Stream().GetReader();

    // Set the result
    hResult->SetResultObject(capabilities, capabilities->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpWmsGetCapabilities.Execute")
}

void MgHttpWmsGetCapabilities::AcquireResponseData(MgOgcServer* ogcServer)
{
    MgOgcWmsServer* wmsServer = (MgOgcWmsServer*)ogcServer;
    if(wmsServer != NULL)
    {
        // Create an instance of the Resource Service
        Ptr<MgResourceService> resourceService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

        // Retrieve the layer definitions
        Ptr<MgWmsLayerDefinitions> layerDefs = MgHttpWmsGetCapabilities::GetLayerDefinitions(*resourceService);

        // WMS Server takes ownership of layer defs
        wmsServer->SetLayerDefs(layerDefs);
    }
}

// Static method to retrieve layer definitions
MgWmsLayerDefinitions* MgHttpWmsGetCapabilities::GetLayerDefinitions(MgResourceService& resourceService)
{
    MgWmsLayerDefinitions* layerDefs = NULL;

    // Create MgResourceIdentifier
    MgResourceIdentifier mgrIdentifier(_("Library://"));

      // Run API command
    STRING sType = _("LayerDefinition");
    INT32 keProperties = MgResourceHeaderProperties::Metadata;
    STRING sDontCare(_(""));
    Ptr<MgByteReader> Result =
          resourceService.EnumerateResources(&mgrIdentifier, // "Library://"
                                          -1,             // Infinite depth
                                          sType,          // "LayerDefinition"
                                          keProperties,   // want metadata, not security
                                          sDontCare,      // start date; don't care
                                          sDontCare,      // end date; don't care
                                          false);         // Not to compute children

    STRING sLayers = Result->ToString();
    return new MgWmsLayerDefinitions(sLayers.c_str());
}
