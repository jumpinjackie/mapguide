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
#include "HttpWfsDescribeFeatureType.h"

#include "OgcFramework.h"
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

    // We have to wrap the request parameters, since the outside
    // world is case-sensitive (with respect to names,) but
    // we need our parameters NOT to be so.
    Ptr<MgHttpRequestParam> origReqParams = m_hRequest->GetRequestParam();
    MgHttpRequestParameters Parms(origReqParams);
    MgHttpResponseStream Out;

    MgOgcServer::SetLoader(GetDocument);

    MgUserInformation::SetCurrentUserInfo(m_userInfo);

    // Instance a server-lette
    MgOgcWfsServer Wfs(Parms,Out);

    // Determine required feature types
    CPSZ pszFeatureTypes = Wfs.RequestParameter(MgHttpResourceStrings::reqWfsTypeName.c_str());
    STRING sFeatureTypes = pszFeatureTypes? pszFeatureTypes : _("");
    Ptr<MgStringCollection> featureTypeList;
    if(sFeatureTypes.empty())
    {
        featureTypeList = NULL;
    }
    else
    {
        featureTypeList = MgStringCollection::ParseCollection(sFeatureTypes, L",");
    }

    Ptr<MgResourceService> pResourceService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));
    Ptr<MgFeatureService> pFeatureService = (MgFeatureService*)(CreateService(MgServiceType::FeatureService));

    // Retrieve feature definitions
    auto_ptr<MgWfsFeatureDefinitions> pFeatureTypes;
    if(NULL == featureTypeList)
    {
        pFeatureTypes.reset(new MgWfsFeatureDefinitions(pResourceService,pFeatureService));
    }
    else
    {
        pFeatureTypes.reset(new MgWfsFeatureDefinitions(pResourceService,pFeatureService,featureTypeList));
    }
    Wfs.SetFeatureDefinitions(pFeatureTypes.get());

    // In order to validate request we have to invoke the ProcessRequest
    if(!Wfs.ProcessRequest(this))
    {
        // Obtain the response byte reader
        Ptr<MgByteReader> errorResponse = Out.Stream().GetReader();

        // Set the result
        hResult->SetResultObject(errorResponse, errorResponse->GetMimeType());
        return;
    }

    // Determine required output format
    // This part must behind the Wfs.ProcessRequest, where parameters have been validated.
    CPSZ pszOutputFormat = Wfs.RequestParameter(MgHttpResourceStrings::reqWfsOutputFormat.c_str());
    STRING sOutputFormat = pszOutputFormat? pszOutputFormat : _("");
    if(sOutputFormat.empty())
    {
        sOutputFormat = Wfs.GetDefaultDescribeFeatureTypeOutputFormat(STRING(Wfs.RequestParameter(MgHttpResourceStrings::reqWfsVersion.c_str())));
    }

    if(pFeatureTypes->InSameNamespace()) 
    {
        STRING sPrefix = L"";
        STRING sUrl = L"";
        STRING sResource = L""; // TODO: look for this in arg, since POST may put it there to save us trouble.
        STRING sSchemaHash = L"";
        Ptr<MgResourceIdentifier> idResource;
        Ptr<MgStringCollection> pFeatureClasses = new MgStringCollection();

        while(pFeatureTypes->ReadNext())
        {
            STRING sClassFullName = pFeatureTypes->GetClassFullName();
            
            if(!sFeatureTypes.empty() && STRING::npos == sFeatureTypes.find(sClassFullName))
            {
                continue;
            }

            STRING::size_type iPos = sClassFullName.find(_(":")); //NOXLATE
            if(iPos != STRING::npos)
            {
                if(sPrefix.empty())
                {
                    sPrefix = sClassFullName.substr(0,iPos);
                }

                STRING sClass = sClassFullName.substr(iPos+1);

                sUrl = pFeatureTypes->GetNamespaceUrl();

                if(NULL == idResource)
                {
                    if(pFeatureTypes->PrefixToFeatureSource(sPrefix, sResource, sSchemaHash)) {
                        idResource = new MgResourceIdentifier(sResource);
                    }
                    else
                    {
                        // Badly formed feature type?  Throw an exception.
                        GenerateTypeNameException(hResult,sFeatureTypes);
                        return;
                    }
                }

                pFeatureClasses->Add(((sSchemaHash.size()==0) ? sClass : sSchemaHash + _(":") + sClass)); //NOXLATE
            }
            else {
                // Badly formed feature type?  Throw an exception.
                GenerateTypeNameException(hResult,sFeatureTypes);
                return;
            }
        }

        if(pFeatureClasses->GetCount() == 0)
        {
            // Badly formed feature type?  Throw an exception.
            GenerateTypeNameException(hResult,sFeatureTypes);
            return;
        }

        Ptr<MgByteReader> response  = pFeatureService->DescribeWfsFeatureType(idResource, pFeatureClasses, sPrefix, sUrl);

        // Set the result
        hResult->SetResultObject(response, sOutputFormat);
    }
    else {
        // There's more than one feature, so we need to enumerate
        // them and have each get imported.
        //
        if(!pFeatureTypes->SubsetFeatureList(sFeatureTypes.c_str()))
            GenerateTypeNameException(hResult,sFeatureTypes);
        else {

#ifdef _WFS_LOGGING
            MyLog.Write(_("WFS::DescribeFeatureType\r\n"));
#endif
            // Execute the request
            //Wfs.ProcessRequest(this);

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
    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    string xmlString = params->GetXmlPostData();
    STRING wxmlString = MgUtil::MultiByteToWideChar(xmlString);

    //TODO Parse params->GetXmlPostData();
    //TODO When parsing, extract prefix and namespace and put them into the param collection.
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
    MgWfsFeatureDefinitions oFeatureTypes(pResourceService,pFeatureService);

    // We have to wrap the request parameters, since the outside
    // world is case-sensitive (with respect to names,) but
    // we need our parameters NOT to be so.
    Ptr<MgHttpRequestParam> origReqParams = m_hRequest->GetRequestParam();
    MgHttpRequestParameters Parms(origReqParams);
    MgHttpResponseStream Out;

    MgOgcServer::SetLoader(GetDocument);

    // Instance a server-lette
    MgOgcWfsServer Wfs(Parms,Out,oFeatureTypes);

    // All that tedium, just to get to this call! ;-)
    Wfs.GenerateTypeNameException(sFeatureTypes);

    Ptr<MgByteReader> capabilities = Out.Stream().GetReader();

    // Set the result
    pResult->SetResultObject(capabilities, capabilities->GetMimeType());
}
