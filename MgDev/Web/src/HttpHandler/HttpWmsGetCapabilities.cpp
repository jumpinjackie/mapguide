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
#include "HttpWmsGetCapabilities.h"

#include "OgcFramework.h"
#include "ServerAdmin.h"
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

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
}


// This simple function is handed into the MgOwsServer system, as
// a means to resolve documents.
bool GetDocument(CPSZ pszDoc,REFSTRING sRet)
{
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
    Ptr<MgSite> mgSite;
    mgSite = new MgSite();
    mgSite->Open(userInfo);
    STRING serverAddress = mgSite->GetSiteServerAddress();

    Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
    serverAdmin->Open(serverAddress,userInfo);

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

    // We need a list of the layers.
    // This comes to us from the ResourceService in the form
    // of an XML description.  We parse on the fly, in the
    // MgWmsLayerDefinitions object.
    STRING sLayers;
    {
        // Create ProxyResourceService instance
        Ptr<MgResourceService> mgprService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

          // Create MgResourceIdentifier
        MgResourceIdentifier mgrIdentifier(_("Library://"));

          // Run API command
        STRING sType = _("LayerDefinition");
        INT32 keProperties = MgResourceHeaderProperties::General |
                             MgResourceHeaderProperties::Metadata;
        STRING sDontCare(_(""));
        Ptr<MgByteReader> Result =
              mgprService->EnumerateResources(&mgrIdentifier, // "Library://"
                                              -1,             // Infinite depth
                                              sType,          // "LayerDefinition"
                                              keProperties,   // want metadata, not security
                                              sDontCare,      // start date; don't care
                                              sDontCare);     // end date; don't care

        sLayers = Result->ToString();
    }
    MgXmlParser LayerDefs(sLayers.c_str());
    MgWmsLayerDefinitions Layers(LayerDefs);

    // We have to wrap the request parameters, since the outside
    // world is case-sensitive (with respect to names,) but
    // we need our parameters NOT to be so.
    MgHttpRequestParameters Parms(m_hRequest->GetRequestParam());
    MgHttpResponseStream Out;

    // Declare the method we'd like the system to use for resolving
    // loader documents.
    // (Must be done before instancing an MgOgc{Wms}Server)
    MgOgcServer::SetLoader(GetDocument);

    // Instance a server-lette
    MgOgcWmsServer Wms(Parms,Out,&Layers);

    // Execute the request
    Wms.RespondToRequest();

    // Slurp the results.
    Ptr<MgByteReader> capabilities = Out.Stream().GetReader();

    // Set the result
    hResult->SetResultObject(capabilities, capabilities->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpWmsGetCapabilities.Execute")
}
