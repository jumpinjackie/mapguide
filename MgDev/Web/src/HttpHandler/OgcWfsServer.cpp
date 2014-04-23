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

#include "OgcFramework.h"
#include "OgcWfsServer.h"

#include "OgcWfsException.h"

CPSZ kpszQueryValueWfs                     = _("WFS");
CPSZ kpszFilenameGlobalConfigWfs           = _("Wfs:OgcWfsService.config");
CPSZ kpszFilenameTemplatePrefixWfs         = _("Wfs:");

CPSZ kpszQueryValueDescribeFeatureType     = _("DescribeFeatureType");
CPSZ kpszQueryStringOutputFormat           = _("OutputFormat");
CPSZ kpszQueryStringDescribeFeatureTypeOutputFormatDefault_1_0_0   = _("Formats.DescribeFeatureType.default.1.0.0");
CPSZ kpszQueryStringDescribeFeatureTypeOutputFormatDefault_1_1_0   = _("Formats.DescribeFeatureType.default.1.1.0");

CPSZ kpszPiEnumFeatureTypes                = _("EnumFeatureTypes");
CPSZ kpszPiEnumFeatures                    = _("EnumFeatures");
CPSZ kpszPiEnumFeaturesDefaultFormat       = _("&Feature.Name;\n");

CPSZ kpszPiDefinitionFeatureIteration      = _("Feature.iteration");

CPSZ kpszQueryValueGetFeature                = _("GetFeature");
CPSZ kpszQueryStringGetFeatureOutputFormatDefault_1_0_0    = _("Formats.GetFeature.default.1.0.0");
CPSZ kpszQueryStringGetFeatureOutputFormatDefault_1_1_0    = _("Formats.GetFeature.default.1.1.0");
CPSZ kpszPiGetFeatureCollection              = _("GetFeatureCollection");
CPSZ kpszPiGetFeatureCollectionDefaultFormat = _("&GetFeatureCollection.xml;");
CPSZ kpszQueryStringTypeName                 = _("typename");

CPSZ kpszQueryStringAcceptVersions           = _("acceptversions");
CPSZ kpszQueryStringSections                 = _("sections");

CPSZ kpszDefinitionSectionServiceIdentification     = _("Section.ServiceIdentification");
CPSZ kpszDefinitionSectionServiceProvider           = _("Section.ServiceProvider");
CPSZ kpszDefinitionSectionOperationsMetadata        = _("Section.OperationsMetadata");
CPSZ kpszDefinitionSectionFeatureTypeList           = _("Section.FeatureTypeList");

extern CPSZ kpszPiAttributeSubset;            // = _("subset");
extern CPSZ kpszElementVersion;               // = _("Version");
extern CPSZ kpszAttributeNumber;              //= _("number");
extern CPSZ kpszDictionarySupportedVersions;  //= _("SupportedVersions");

// Localize
CPSZ kpszExceptionMessageMissingServiceWfs = _("Expected required SERVICE=WFS argument."); // Localize
CPSZ kpszExceptionMessageWfsMissingRequest = _("Expected required REQUEST= argument.");    // Localize
CPSZ kpszExceptionMessageWfsMissingVersion = _("Expected required VERSION= argument.");    // Localize
CPSZ kpszExceptionMessageWfsUnknownRequest = _("Expected valid REQUEST= argument, such as GetCapabilities.  (Instead, found request='&Request.request;'.)"); // Localize
CPSZ kpszExceptionMessageUnknownOutputFormat = _("Expected valid outputFormat argument, as enumerated by GetCapabilities. (Instead, found outputFormat='&Request.OutputFormat;'.)"); // Localize
CPSZ kpszExceptionMessageUnknownTypeName   = _("Expected valid typeName argument, as enumerated by GetCapabilities. (Instead, found typeName='&Request.TypeName;'.)"); // Localize
CPSZ kpszExceptionMessageWfsGetFeatureMissingFeatureType = _("A WFS GetFeature request requires at least one feature type to be specified."); // Localize
CPSZ kpszExceptionMessageWfsVersionNegotiationFailed = _("Requested version is unsupported in AcceptVersions"); //Localize
CPSZ kpszExceptionMessageWfsInvalidVersion = _("Version number specified must correspond to a version supported by the service."); // Localize
CPSZ kpszExceptionMessageWfsInvalidService = _("Invalid parameter value for SERVICE"); //Localize
CPSZ kpszExceptionMessageMalformedRequestedType = _("Encountered malformed typeName in request"); //Localize

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
            ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszOperationNotSupported,
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
    if(pService == NULL)
    {
        ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszMissingRequestParameter,
                                                         kpszExceptionMessageMissingServiceWfs,
                                                         kpszQueryStringService));
        bValid = false;
    }
    else if(SZ_NEI(pService,kpszQueryValueWfs))
    {
        ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszInvalidParameterValue,
                                                         kpszExceptionMessageWfsInvalidService,
                                                         kpszQueryStringService));
        bValid = false;
    }

    if(bValid)
    {
        // Check that we have a request parameter
        CPSZ pszRequest = RequestParameter(kpszQueryStringRequest);
        if(pszRequest == NULL)
        {
            ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszMissingRequestParameter,
                                                             kpszExceptionMessageWfsMissingRequest,
                                                             kpszQueryStringRequest));
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
                bValid = ValidateGetCapabilitiesRequest();
                break;
            }
            case WfsDescribeFeatureTypeType:
            {
                bValid = ValidateDescribeFeatureTypeRequest();
                break;
            }
            case WfsGetFeatureType:
            {
                bValid = ValidateGetFeatureRequest();
                break;
            }
            default:
            {
                ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWmsException::kpszOperationNotSupported,
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

bool MgOgcWfsServer::ValidateGetCapabilitiesRequest()
{
    // Check that we have a acceptVersions parameter
    CPSZ pszAcceptVersions = RequestParameter(kpszQueryStringAcceptVersions);
    if(pszAcceptVersions != NULL)
    {
        STRING sAcceptVersions(pszAcceptVersions);
        CPSZ pszSupportedVersions = Definition(kpszDictionarySupportedVersions);
        // Pass out a NULL, which caller must use to tickle an exception.
        if(pszSupportedVersions == NULL)
            return false;

        MgXmlParser SupportedVersions(pszSupportedVersions);

        STRING sVersion;
        bool bSupported = false;
        while(SupportedVersions.Next())
        {
            if(SupportedVersions.Current().Type() == keBeginElement)
            {
                MgXmlBeginElement& Begin = (MgXmlBeginElement&)SupportedVersions.Current();
                if(Begin.Name() == kpszElementVersion && Begin.GetAttribute(kpszAttributeNumber,sVersion))
                {
                    if(sAcceptVersions.find(sVersion) != STRING::npos)
                    {
                        bSupported = true;
                        break;
                    }
                }
            }
        }
        if(!bSupported)
        {
            ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszVersionNegotiationFailed,
                                                             kpszExceptionMessageWfsVersionNegotiationFailed,
                                                             kpszQueryStringVersion));
            return false;
        }
    }

    return true;
}
bool MgOgcWfsServer::ValidateDescribeFeatureTypeRequest()
{
    CPSZ pVersion = RequestParameter(kpszQueryStringVersion);
    if(pVersion == NULL)
    {
        ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszMissingRequestParameter,
                                                         kpszExceptionMessageWfsMissingVersion,
                                                         kpszQueryStringVersion));
        return false;
    }

    // If all the validation passed, we add the FeatureTypeList into definition to
    // make ProcedureEnumFeatureTypes happy.
    AddDefinition(kpszDefinitionSectionFeatureTypeList,kpszOmittedValue);

    return true;
}

bool MgOgcWfsServer::ValidateGetFeatureRequest()
{
    CPSZ pVersion = RequestParameter(kpszQueryStringVersion);
    if(pVersion == NULL || STRING(pVersion).empty())
    {
        ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszMissingRequestParameter,
                                                         kpszExceptionMessageWfsMissingVersion,
                                                         kpszQueryStringVersion));
        return false;
    }
    else
    {
        // OGC CITE: Test wfs:wfs-1.1.0-Basic-GetFeature-tc44.2 (s0001/d1e35137_1/d1e740_1/d1e25217_1/d1e6238_1)
        // Assertion: Version number specified in a given request must correspond to a version supported by the service.
        CPSZ pszSupportedVersions = Definition(kpszDictionarySupportedVersions);
        // Pass out a NULL, which caller must use to tickle an exception.
        if(pszSupportedVersions == NULL)
            return false;

        MgXmlParser SupportedVersions(pszSupportedVersions);

        STRING sVersion;
        bool bSupported = false;
        while(SupportedVersions.Next())
        {
            if(SupportedVersions.Current().Type() == keBeginElement)
            {
                MgXmlBeginElement& Begin = (MgXmlBeginElement&)SupportedVersions.Current();
                if(Begin.Name() == kpszElementVersion && Begin.GetAttribute(kpszAttributeNumber,sVersion))
                {
                    if(SZ_EQI(pVersion,sVersion.c_str()))
                    {
                        bSupported = true;
                        break;
                    }
                }
            }
        }
        if(!bSupported)
        {
            ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszInvalidParameterValue,
                                                             kpszExceptionMessageWfsInvalidVersion,
                                                             kpszQueryStringVersion));
            return false;
        }
    }

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
                                                 kpszExceptionMessageWfsGetFeatureMissingFeatureType,
                                                 MgHttpResourceStrings::reqWfsTypeName.c_str()));
        return false;
    }

    return true;
}

// Implements the WFS GetFeature response.
void MgOgcWfsServer::GetFeatureResponse()
{
    try
    {
        //MgHttpWfsGetFeature will pass a MgGetWfsFeaturesResponseStream to this instance when executing
        MgGetWfsFeaturesResponseStream* wfsResponse = static_cast<MgGetWfsFeaturesResponseStream*>(m_pResponse);
        wfsResponse->SetReader(m_pFeatureSet);
        return;
    }
    catch(MgException* pEx)
    {
        ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszProcessingError,
            pEx->GetDetails().c_str()));
        return;
    }

    // If we get to here, we were unable to generate a response for the requested format
    ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszUnknownParameterValue,
                                                     kpszExceptionMessageUnknownOutputFormat,
                                                     MgHttpResourceStrings::reqWfsOutputFormat.c_str()));
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
    STRING sOutputFormat;

    if(pszFormat == NULL)
    {
        CPSZ pszVersion = RequestParameter(MgHttpResourceStrings::reqWfsVersion.c_str());
        sOutputFormat = this->GetDefaultDescribeFeatureTypeOutputFormat(pszVersion);
        pszFormat = sOutputFormat.c_str();
    }

    if(pszFormat != NULL)
        if(GenerateResponse(kpszQueryValueDescribeFeatureType,pszFormat))
            return;

    // Conformance test wfs/1.0.0/basic/describefeaturetype/get/3
    // requires that we provide a specific exception report.
    ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszUnknownParameterValue,
                                                     kpszExceptionMessageUnknownOutputFormat,
                                                     MgHttpResourceStrings::reqWfsOutputFormat.c_str()));
}


bool MgOgcWfsServer::ProcessOtherInstruction(CREFSTRING sProc,MgXmlProcessingInstruction& PI)
{
    if(sProc == kpszPiEnumFeatureTypes)
    {
        ProcedureEnumFeatureTypes(PI);
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
    // OGC CITE: Test wfs:wfs-1.1.0-Basic-GetCapabilities-tc19.2 (s0012/d1e34887_1/d1e732_1/d1e25171_1/d1e949_1)
    // Assertion:
    // The response to a GetCapabilities request that includes a sections parameter
    // listing optional elements shall include only the requested elements in the
    // response entity.
    CPSZ psz = this->Definition(kpszDefinitionSectionFeatureTypeList);
    if(!psz)
        return;

    STRING sFormat;
    if(!PIEnum.GetAttribute(kpszPiAttributeUsing,sFormat))
        sFormat = kpszPiEnumFeaturesDefaultFormat;

    STRING sSubset;
    if(!PIEnum.GetAttribute(kpszPiAttributeSubset,sSubset))
        sSubset = kpszEmpty;
    ProcessExpandableTextIntoString(sSubset,sSubset);

    int iNum = 0;

    if(NULL != m_pFeatures)
    {
        while(m_pFeatures->Next())
        {
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
}

void MgOgcWfsServer::GenerateTypeNameException(CREFSTRING sTypeName)
{
    sTypeName; // RESERVED FOR FUTURE USE; unused for now... to provide this info in exception body.
    ServiceExceptionReportResponse(MgOgcWfsException(MgOgcWfsException::kpszUnknownParameterValue,
                                                     kpszExceptionMessageUnknownTypeName,
                                                     MgHttpResourceStrings::reqWfsTypeName.c_str()));
}

void MgOgcWfsServer::SetFeatures(MgByteReader* pFeatures)
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


void MgOgcWfsServer::SetGetCapabilitiesSection(CREFSTRING sSection)
{
    if(!sSection.empty())
    {
        AddDefinition(sSection.c_str(),kpszOmittedValue);
    }
    else
    {
        AddDefinition(kpszDefinitionSectionServiceIdentification,kpszOmittedValue);
        AddDefinition(kpszDefinitionSectionServiceProvider,kpszOmittedValue);
        AddDefinition(kpszDefinitionSectionOperationsMetadata,kpszOmittedValue);
        AddDefinition(kpszDefinitionSectionFeatureTypeList,kpszOmittedValue);
    }
}

// Help method to get the default DescribeFeatureType output format
STRING MgOgcWfsServer::GetDefaultDescribeFeatureTypeOutputFormat(CREFSTRING sVersion)
{
    if(_("1.0.0") == sVersion)
    {
        CPSZ psz = this->Definition(kpszQueryStringDescribeFeatureTypeOutputFormatDefault_1_0_0);
        return STRING(psz? psz : _(""));
    }
    else
    {
        CPSZ psz = this->Definition(kpszQueryStringDescribeFeatureTypeOutputFormatDefault_1_1_0);
        return STRING(psz? psz : _(""));
    }
}

// Help method to get the default GetFeature output format
STRING MgOgcWfsServer::GetDefaultGetFeatureOutputFormat(CREFSTRING sVersion)
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



