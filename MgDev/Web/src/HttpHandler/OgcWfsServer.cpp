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

#include "OgcFramework.h"
#include "OgcWfsServer.h"

#include "OgcWfsException.h"

CPSZ kpszQueryValueWfs                     = _("WFS");
CPSZ kpszFilenameGlobalConfigWfs           = _("Wfs:OgcWfsService.config");
CPSZ kpszFilenameTemplatePrefixWfs         = _("Wfs:");

CPSZ kpszQueryValueDescribeFeatureType     = _("DescribeFeatureType");
CPSZ kpszQueryStringOutputFormat           = _("OutputFormat");
CPSZ kpszQueryStringOutputFormatDefault    = _("Formats.DescribeFeatureType.default");

CPSZ kpszPiEnumFeatureTypes                = _("EnumFeatureTypes");
CPSZ kpszPiEnumFeatures                    = _("EnumFeatures");
CPSZ kpszPiEnumFeaturesDefaultFormat       = _("&Feature.Name;\n");

CPSZ kpszPiDefinitionFeatureIteration      = _("Feature.iteration");

CPSZ kpszQueryValueGetFeature                = _("GetFeature");
CPSZ kpszQueryStringGetFeatureOutputFormatDefault_1_0_0    = _("Formats.GetFeature.default.1.0.0");
CPSZ kpszQueryStringGetFeatureOutputFormatDefault_1_1_0    = _("Formats.GetFeature.default.1.1.0");
CPSZ kpszPiGetFeatureCollection              = _("GetFeatureCollection");
CPSZ kpszPiGetFeatureCollectionDefaultFormat = _("&GetFeatureCollection.xml;");

extern CPSZ kpszPiAttributeSubset;      // = _("subset");

// Localize
CPSZ kpszExceptionMessageMissingServiceWfs = _("Expected required SERVICE=WFS argument."); // Localize
CPSZ kpszExceptionMessageWfsMissingRequest = _("Expected required REQUEST= argument.");    // Localize
CPSZ kpszExceptionMessageWfsUnknownRequest = _("Expected valid REQUEST= argument, such as GetCapabilities.  (Instead, found request='&Request.request;'.)"); // Localize
CPSZ kpszExceptionMessageUnknownOutputFormat = _("Expected valid outputFormat argument, as enumerated by GetCapabilities. (Instead, found outputFormat='&Request.OutputFormat;'.)"); // Localize
CPSZ kpszExceptionMessageUnknownTypeName   = _("Expected valid typeName argument, as enumerated by GetCapabilities. (Instead, found typeName='&Request.TypeName;'.)"); // Localize
CPSZ kpszExceptionMessageWfsGetFeatureMissingFeatureType = _("A WFS GetFeature request requires at least one feature type to be specified."); // Localize

CPSZ kpszInternalErrorMissingGetFeatureRequestParams   = _("Internal Error: Missing WFS GetFeature request parameters."); // Localize
//

MgUtilDictionary MgOgcWfsServer::ms_GlobalDefinitions(NULL);
STRING           MgOgcWfsServer::ms_sExceptionTemplate;
STRING           MgOgcWfsServer::ms_sExceptionMimeType;

MgOgcWfsServer::MgOgcWfsServer(MgHttpRequestParameters& Request,CStream& Response,MgWfsFeatureDefinitions& Features)
: MgOgcServer(Request,Response,ms_GlobalDefinitions)
, m_pFeatures(&Features)
, m_pFeatureSet(NULL)
{
  InitServer(kpszFilenameGlobalConfigWfs,
             ms_GlobalDefinitions,
             ms_sExceptionTemplate,
             ms_sExceptionMimeType);
}

MgOgcWfsServer::MgOgcWfsServer(MgHttpRequestParameters& Request,CStream& Response)
: MgOgcServer(Request,Response,ms_GlobalDefinitions)
, m_pFeatures(NULL)
, m_pFeatureSet(NULL)
{
  InitServer(kpszFilenameGlobalConfigWfs,
             ms_GlobalDefinitions,
             ms_sExceptionTemplate,
             ms_sExceptionMimeType);
}

void MgOgcWfsServer::RespondToRequest()
{
    WfsRequestType requestType = GetRequestType();
    switch(requestType)
    {
        case WfsGetCapabilitiesType:
        {
            GetCapabilitiesResponse();
            break;
        }
        case WfsDescribeFeatureTypeType:
        {
            DescribeFeatureTypeResponse();
            break;
        }
        case WfsGetFeatureType:
        {
            GetFeatureResponse();
            break;
        }
        default:
        {
            ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWfsException::kpszOperationNotSupported,
                                                         kpszExceptionMessageWfsUnknownRequest));
            break;
        }
    }
}

// Validate the supplied request parameters
bool MgOgcWfsServer::ValidateRequest()
{
    bool bValid = true;

    // Check for SERVICE=WFS -- otherwise, we're not prepared to service the request
    CPSZ pService = RequestParameter(kpszQueryStringService);
    if(pService == NULL || SZ_NEI(pService,kpszQueryValueWfs))
    {
        // TODO: verify that kpszOperationNotSupported is the right one to send back.
        ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszOperationNotSupported,
                                                         kpszExceptionMessageMissingServiceWfs));
        bValid = false;
    }

    if(bValid)
    {
        // Check that we have a request parameter
        CPSZ pszRequest = RequestParameter(kpszQueryStringRequest);
        if(pszRequest == NULL)
        {
            // TODO: verify that kpszOperationNotSupported is the right one to send back.
            ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWfsException::kpszOperationNotSupported,
                                                             kpszExceptionMessageWfsMissingRequest));
            bValid = false;
        }
    }

    if(bValid)
    {
        WfsRequestType requestType = GetRequestType();
        switch(requestType)
        {
            case WfsGetCapabilitiesType:
            {
                // Currently no validation is carried out
                //bValid = ValidateGetCapabilitiesParameters();
                break;
            }
            case WfsDescribeFeatureTypeType:
            {
                // Currently no validation is carried out
                //bValid = ValidateDescribeFeatureTypeParameters();
                break;
            }
            case WfsGetFeatureType:
            {
                bValid = ValidateGetFeatureRequest();
                break;
            }
            default:
            {
                ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszOperationNotSupported,
                                                             kpszExceptionMessageUnknownRequest));
                break;
            }
        }
    }

    return bValid;
}

// Determine the request type for this WMS request
enum MgOgcWfsServer::WfsRequestType MgOgcWfsServer::GetRequestType()
{
    WfsRequestType requestType = WfsUnknownType;

    // Get the value of the REQUEST parameter
    CPSZ pszRequest = RequestParameter(kpszQueryStringRequest);
    if(pszRequest != NULL)
    {
        if(SZ_EQI(pszRequest,kpszQueryValueGetCapabilities))
        {
            requestType = WfsGetCapabilitiesType;
        }
        else if(SZ_EQI(pszRequest,kpszQueryValueDescribeFeatureType))
        {
            requestType = WfsDescribeFeatureTypeType;
        }
        else if(SZ_EQI(pszRequest,kpszQueryValueGetFeature))
        {
            requestType = WfsGetFeatureType;
        }
    }
    return requestType;
}

CPSZ MgOgcWfsServer::GetServiceType()
{
    return kpszQueryValueWfs;
}

CPSZ MgOgcWfsServer::GetTemplatePrefix()
{
    return kpszFilenameTemplatePrefixWfs;
}


// Gets the template info for default exception, when no other exception type is at hand.
void MgOgcWfsServer::GetDefaultExceptionInfo(REFSTRING sTemplate,REFSTRING sMimeType)
{
    sTemplate = ms_sExceptionTemplate;
    sMimeType = ms_sExceptionMimeType;
}

bool MgOgcWfsServer::ValidateGetFeatureRequest()
{
    if(m_pGetFeatureParams == NULL)
    {
        // The request params are missing
        InternalError(kpszInternalErrorMissingGetFeatureRequestParams);
        return false;
    }

    // Check that the request contains at least one feature type
    Ptr<MgStringCollection> pFeatureTypes = m_pGetFeatureParams->GetFeatureTypes();
    if(pFeatureTypes == NULL || pFeatureTypes->GetCount() == 0)
    {
        ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszMissingRequestParameter,
                                                 kpszExceptionMessageWfsGetFeatureMissingFeatureType));
        return false;
    }

    return true;
}

// Implements the WFS GetFeature response.
void MgOgcWfsServer::GetFeatureResponse()
{
    try
    {
        // See what format they requested the GetFeature response in...
        CPSZ pszFormat = RequestParameter(kpszQueryStringOutputFormat);
        STRING sOutputFormat;

        if(pszFormat == NULL)
        {
            CPSZ pszVersion = RequestParameter(MgHttpResourceStrings::reqWfsVersion.c_str());
            sOutputFormat = this->GetDefaultOutputFormat(pszVersion);
            pszFormat = sOutputFormat.c_str();
        }
        // Generate a response to the GetFeature request
        if(GenerateResponse(kpszQueryValueGetFeature,pszFormat))
        {
            // The response was generated successfully, so we return
            return;
        }
    }
    catch(MgException* pEx)
    {
        ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszProcessingError,
            pEx->GetDetails().c_str()));
        return;
    }

    // If we get to here, we were unable to generate a response for the requested format
    ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszUnknownParameterValue,
                                                     kpszExceptionMessageUnknownOutputFormat));
}

// Implements the WMS GetCapabilities response.
void MgOgcWfsServer::GetCapabilitiesResponse()
{
    // See what format they requested the GetCapy response in...
    CPSZ pszFormat = RequestParameter(kpszQueryStringFormat);
    if(pszFormat == NULL)
      pszFormat = kpszMimeTypeXml;

    // TODO: see if this holds true for WFS as well as WMS...
    // The WMS spec says if we can't honor the requested format...
    if(GenerateResponse(kpszQueryValueGetCapabilities,pszFormat))
        return;

    // ... we must respond with xml (if we didn't just try that already)
    if(SZ_NE(pszFormat,kpszMimeTypeXml) &&
        GenerateResponse(kpszQueryValueGetCapabilities,kpszMimeTypeXml))
        return;

    // Hmmm... this is bad.  We need to die gracefully.
    // Note: unlike the bottom of DescribeFeatureTypeResponse, this is an internal
    // error, since the XML response type is something we really truly need.
    InternalError(kpszInternalErrorMissingGetCapyResponse);
}


void MgOgcWfsServer::DescribeFeatureTypeResponse()
{
    // See what format they requested the GetCapy response in...
    CPSZ pszFormat = RequestParameter(kpszQueryStringOutputFormat);
    if(pszFormat == NULL)
        pszFormat = this->Definition(kpszQueryStringOutputFormatDefault);

    if(pszFormat != NULL)
        if(GenerateResponse(kpszQueryValueDescribeFeatureType,pszFormat))
            return;

    // Conformance test wfs/1.0.0/basic/describefeaturetype/get/3
    // requires that we provide a specific exception report.
    ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWfsException::kpszUnknownParameterValue,
                                                     kpszExceptionMessageUnknownOutputFormat));
}


bool MgOgcWfsServer::ProcessOtherInstruction(CREFSTRING sProc,MgXmlProcessingInstruction& PI)
{
    if(sProc == kpszPiEnumFeatureTypes)
    {
        ProcedureEnumFeatureTypes(PI);
    }
    else if(sProc == kpszPiEnumFeatures)
    {
        ProcedureEnumFeatures(PI);
    }
    else if(sProc == kpszPiGetFeatureCollection)
    {
        ProcedureGetFeatureCollection(PI);
    }
    else
    {
        return false; // Unknown
    }

    return true; // processed... we're happy.
}

// This Enum is used by GetCapabilities to list the type names of features
//
void MgOgcWfsServer::ProcedureEnumFeatureTypes(MgXmlProcessingInstruction& PIEnum)
{
    STRING sFormat;
    if(!PIEnum.GetAttribute(kpszPiAttributeUsing,sFormat))
        sFormat = kpszPiEnumFeaturesDefaultFormat;

    STRING sSubset;
    if(!PIEnum.GetAttribute(kpszPiAttributeSubset,sSubset))
        sSubset = kpszEmpty;
    ProcessExpandableTextIntoString(sSubset,sSubset);

    int iNum = 0;

    while(m_pFeatures->Next()) {
        // We ensure that each feature gets its own stack frame
        // so definitions don't get carried over to the next feature.
        CDictionaryStackFrame ForEachFeature(this);
        m_pFeatures->GenerateDefinitions(*m_pTopOfDefinitions);

        CPSZ pszIsPublished = this->Definition(_("Feature.IsPublished"));
        if(pszIsPublished != NULL && SZ_EQ(pszIsPublished,_("1")))
            if(IsIterationInSubset(++iNum,sSubset,kpszPiDefinitionFeatureIteration)) {
                ProcessExpandableText(sFormat);
        }
    }
}


void MgOgcWfsServer::ProcedureEnumFeatures(MgXmlProcessingInstruction& PIEnum)
{
    STRING sFormat;
    if(!PIEnum.GetAttribute(kpszPiAttributeUsing,sFormat))
        sFormat = kpszPiGetFeatureCollectionDefaultFormat;

    STRING sSubset;
    if(!PIEnum.GetAttribute(kpszPiAttributeSubset,sSubset))
        sSubset = kpszEmpty;
    ProcessExpandableTextIntoString(sSubset,sSubset);

    int iNum = 0;

    if(m_pFeatureSet != NULL) {
        while(m_pFeatureSet->Next()) {
            // We ensure that each feature gets its own stack frame
            // so definitions don't get carried over to the next feature.
            CDictionaryStackFrame ForEachFeature(this);

            m_pFeatureSet->GenerateDefinitions(*m_pTopOfDefinitions);

            if(IsIterationInSubset(++iNum,sSubset,kpszPiDefinitionFeatureIteration))
                ProcessExpandableText(sFormat);
        }
    }
}





void MgOgcWfsServer::GenerateTypeNameException(CREFSTRING sTypeName)
{
    sTypeName; // RESERVED FOR FUTURE USE; unused for now... to provide this info in exception body.
    ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWfsException::kpszUnknownParameterValue,
                                                     kpszExceptionMessageUnknownTypeName));
}

void MgOgcWfsServer::ProcedureGetFeatureCollection(MgXmlProcessingInstruction& PI)
{
    STRING sFormat;
    if(!PI.GetAttribute(kpszPiAttributeUsing,sFormat))
        sFormat = kpszPiGetFeatureCollectionDefaultFormat;

    STRING sSubset;
    if(!PI.GetAttribute(kpszPiAttributeSubset,sSubset))
        sSubset = kpszEmpty;
    ProcessExpandableTextIntoString(sSubset,sSubset);

    bool bHasNamespace = false;

    if(m_pFeatureSet != NULL) {
        while(!bHasNamespace && m_pFeatureSet->Next()) {
            bHasNamespace = m_pFeatureSet->GenerateNamespacesDefinition(*m_pTopOfDefinitions);
        }
    }

    ProcessExpandableText(sFormat);
}

void MgOgcWfsServer::SetFeatures(MgWfsFeatures* pFeatures)
{
    m_pFeatureSet = SAFE_ADDREF(pFeatures);
}

void MgOgcWfsServer::SetGetFeatureRequestParams(WfsGetFeatureParams* pGetFeatureParams)
{
    m_pGetFeatureParams = SAFE_ADDREF(pGetFeatureParams);
}

void MgOgcWfsServer::SetFeatureDefinitions(MgWfsFeatureDefinitions* pFeatureDefs)
{
    m_pFeatures = pFeatureDefs;
}

// Help method to get the default output format
STRING MgOgcWfsServer::GetDefaultOutputFormat(CREFSTRING sVersion)
{
    if(_("1.0.0") == sVersion)
    {
        CPSZ psz = this->Definition(kpszQueryStringGetFeatureOutputFormatDefault_1_0_0);
        return STRING(psz? psz : _(""));
    }
    else
    {
        CPSZ psz = this->Definition(kpszQueryStringGetFeatureOutputFormatDefault_1_1_0);
        return STRING(psz? psz : _(""));
    }
}



