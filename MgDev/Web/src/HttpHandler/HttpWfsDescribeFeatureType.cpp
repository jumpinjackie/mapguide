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
#include "HttpWfsDescribeFeatureType.h"

#include "OgcFramework.h"
#include "ServerAdmin.h"
#include "XmlParser.h"
#include "OgcWfsServer.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpWfsDescribeFeatureType)

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
MgHttpWfsDescribeFeatureType::MgHttpWfsDescribeFeatureType(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
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
void MgHttpWfsDescribeFeatureType::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // This is a comma-sep a list.  If empty, == all.
    // If it's just one feature (no comma sep found) let's do
    // a single response, else let's recursively enumerate the features.
    STRING sFeatureTypes = m_hRequest->GetRequestParam()->GetParameterValue(MgHttpResourceStrings::reqWfsTypeName);
    if(sFeatureTypes.length() > 0 && sFeatureTypes.find(_(",")) != STRING::npos) {
        // TODO: assumes that this is GML type.
        //STRING sOutputFormat = m_hRequest->GetRequestParam()->GetParameterValue(_("OUTPUTFORMAT"));

        // Create Proxy Feature Service instance
        Ptr<MgFeatureService> pFeatureService = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));

        STRING::size_type iPos = sFeatureTypes.find(_(":"));
        if(iPos != STRING::npos) {
            STRING sResource = sFeatureTypes.substr(0,iPos);
            STRING sClass = sFeatureTypes.substr(iPos+1);

            sResource = _("Library://") + sResource + _(".FeatureSource");
            MgResourceIdentifier idResource(sResource);
            Ptr<MgStringCollection> pFeatureClasses = new MgStringCollection();
            pFeatureClasses->Add(sClass);

            Ptr<MgByteReader> response  = pFeatureService->DescribeWfsFeatureType(&idResource,pFeatureClasses);

            // Set the result
            hResult->SetResultObject(response, response->GetMimeType());
        }
        else {
            // Badly formed feature type?  Throw an exception.
            GenerateTypeNameException(hResult,sFeatureTypes);
        }
    }
    else {
        // There's more than one feature, so we need to enumerate
        // them and have each get imported.
        //
        // Create Proxy Feature Service instance
        // Note: these things must have a lifetime that exceeds
        //      MgWfsFeatureDefinitions Features, below.
        Ptr<MgResourceService> pResourceService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));
        Ptr<MgFeatureService> pFeatureService = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));
        //
        MgWfsFeatureDefinitions Features(pResourceService,pFeatureService);
        if(!Features.SubsetFeatureList(sFeatureTypes.c_str()))
            GenerateTypeNameException(hResult,sFeatureTypes);
        else {
            // We have to wrap the request parameters, since the outside
            // world is case-sensitive (with respect to names,) but
            // we need our parameters NOT to be so.
            MgHttpRequestParameters Parms(m_hRequest->GetRequestParam());
            MgHttpResponseStream Out;

            MgOgcServer::SetLoader(GetDocument);

#ifdef _WFS_LOGGING
            MyLog.Write(_("WFS::DescribeFeatureType\r\n"));
#endif
            // Instance a server-lette
            MgOgcWfsServer Wfs(Parms,Out,Features);

            // Execute the request
            Wfs.ProcessRequest(this);

            // Slurp the results.
            Ptr<MgByteReader> capabilities = Out.Stream().GetReader();

            // Set the result
            hResult->SetResultObject(capabilities, capabilities->GetMimeType());
        }
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpWfsDescribeFeatureType.Execute")
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
bool MgHttpWfsDescribeFeatureType::ProcessPostRequest(MgHttpRequest *hRequest, MgHttpResponse& hResponse)
{
    string xmlString = hRequest->GetRequestParam()->GetXmlPostData();
    STRING wxmlString = MgUtil::MultiByteToWideChar(xmlString);

    //TODO Parse hRequest->GetRequestParam()->GetXmlPostData();
#ifdef _WFS_LOGGING
    MyLog.Write(_("WFS::DescribeFeatureType POST\r\n"));
    MyLog.Write(wxmlString);
    MyLog.Write();
#endif
    return false;
}



void MgHttpWfsDescribeFeatureType::GenerateTypeNameException(MgHttpResult* pResult,CREFSTRING sFeatureTypes)
{
    Ptr<MgResourceService> pResourceService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));
    Ptr<MgFeatureService> pFeatureService   = (MgFeatureService *)(CreateService(MgServiceType::FeatureService ));
    //
    MgWfsFeatureDefinitions Features(pResourceService,pFeatureService);
    // We have to wrap the request parameters, since the outside
    // world is case-sensitive (with respect to names,) but
    // we need our parameters NOT to be so.
    MgHttpRequestParameters Parms(m_hRequest->GetRequestParam());
    MgHttpResponseStream Out;

    MgOgcServer::SetLoader(GetDocument);

    // Instance a server-lette
    MgOgcWfsServer Wfs(Parms,Out,Features);

    // All that tedium, just to get to this call! ;-)
    Wfs.GenerateTypeNameException(sFeatureTypes);

    Ptr<MgByteReader> capabilities = Out.Stream().GetReader();

    // Set the result
    pResult->SetResultObject(capabilities, capabilities->GetMimeType());
}
