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

#include "OgcFramework.h"
#include "OgcWmsServer.h"
#include "Common.h"

CPSZ kpszQueryValueWms              = _("WMS");
CPSZ kpszQueryStringInfoFormat      = _("info_format");
CPSZ kpszQueryStringLayers          = _("layers");
CPSZ kpszQueryStringQueryLayers     = _("query_layers");
CPSZ kpszQueryStringICoord          = _("i");
CPSZ kpszQueryStringJCoord          = _("j");
CPSZ kpszQueryStringXCoord          = _("x");
CPSZ kpszQueryStringYCoord          = _("y");
CPSZ kpszQueryStringWidth           = _("width");
CPSZ kpszQueryStringHeight          = _("height");

CPSZ kpszFilenameGlobalConfigWms    = _("Wms:OgcWmsService.config");
CPSZ kpszFilenameTemplatePrefixWms  = _("Wms:");

// Supported image formats
CPSZ kpszImageFormatPNG             = _("image/png");
CPSZ kpszImageFormatGIF             = _("image/gif");
CPSZ kpszImageFormatJPEG            = _("image/jpeg");

// Plain ol' "GetCapabilities" is general to all Ogc Services, so it lives in the OgcServer source.
CPSZ kpszQueryValueCapabilities     = _("Capabilities"); // keyword for pre 1.0.8 versions
CPSZ kpszQueryValueGetMap           = _("GetMap");
CPSZ kpszQueryValueMap              = _("Map");          // keyword for pre 1.0.8 versions
CPSZ kpszQueryValueGetFeatureInfo   = _("GetFeatureInfo");
CPSZ kpszQueryValueFeatureInfo      = _("FeatureInfo");  // keyword for pre 1.0.8 versions


// BEGIN LOCALIZATION -- The following strings are all subject to localization.
CPSZ kpszExceptionMessageMissingServiceWms = _("Expected required SERVICE=WMS argument."); // Localize
CPSZ kpszExceptionMessageMissingRequest    = _("Expected required REQUEST= argument.");    // Localize
CPSZ kpszExceptionMessageUnknownRequest    = _("Expected valid REQUEST= argument (such as GetCapabilities.)"); // Localize
CPSZ kpszInternalErrorMissingGetCapyResponse = _("Unable to generate a GetCapabilities Response --  missing a text/xml Response in the template file."); // Localize
CPSZ kpszExceptionMessageGetFeatureInfoUnsupported = _("REQUEST=GetFeatureInfo operation is not supported on this server."); // Localize
CPSZ kpszExceptionMessageQueryLayerNotDefined = _("One or more layers specified in the QUERY_LAYERS argument are not defined in the map."); // Localize
CPSZ kpszExceptionMessageMapLayersMissing = _("The map request must contain at least one valid layer."); // Localize
CPSZ kpszExceptionMessageMissingQueryPoint = _("The request must contain I and J arguments specifying the query point."); // Localize
CPSZ kpszExceptionMessageInvalidQueryPoint = _("The point specified by the I and J arguments must fall within the map extent."); // Localize
CPSZ kpszExceptionMessageMissingMapDimension = _("The request must contain WIDTH and HEIGHT arguments to specify the map size."); // Localize
CPSZ kpszExceptionMessageInvalidMapDimension = _("The WIDTH and HEIGHT arguments must be positive integer values."); // Localize
CPSZ kpszExceptionMessageMissingCrs = _("The spatial reference system must be specified using either the CRS or SRS argument."); // Localize
CPSZ kpszExceptionMessageMissingBoundingBox = _("The bounding box for the map must be specified using the BBOX argument."); // Localize
CPSZ kpszExceptionMessageInvalidBoundingBox = _("The bounding box for the map must be specified using four numerical values in the order: minX,minY,maxX,maxY."); // Localize
CPSZ kpszExceptionMessageInvalidImageFormat = _("The request uses an unsupported image format."); // Localize
CPSZ kpszExceptionMessageMissingImageFormat = _("The request must contain a FORMAT parameter to specify the required image format."); // Localize
// END LOCALIZATION

CPSZ kpszPiEnumLayers                      = _("EnumLayers");
CPSZ kpszPiEnumFeatureProperties           = _("EnumFeatureProperties");
extern CPSZ kpszPiAttributeUsing;  // for consistency, borrow the "using" attribute from base class.
CPSZ kpszPiEnumLayersDefaultFormat         = _("&Layer.Name;\n");
CPSZ kpszPiEnumFeaturePropertiesDefaultFormat = _("&FeatureProperty.xml;");

extern CPSZ kpszPiAttributeSubset;      // = _("subset");


MgUtilDictionary MgOgcWmsServer::ms_GlobalDefinitions(NULL);
STRING           MgOgcWmsServer::ms_sExceptionTemplate;
STRING           MgOgcWmsServer::ms_sExceptionMimeType;


// Standard constructor.
MgOgcWmsServer::MgOgcWmsServer(MgHttpRequestParameters& Request,CStream& Response,MgWmsLayerDefinitions* pLayers)
: MgOgcServer(Request,Response,ms_GlobalDefinitions)
, m_pLayers(pLayers)
, m_pFeatureInfo(NULL)
{
    InitServer(kpszFilenameGlobalConfigWms,
               ms_GlobalDefinitions,
               ms_sExceptionTemplate,
               ms_sExceptionMimeType);
}

MgOgcWmsServer::MgOgcWmsServer(MgHttpRequestParameters& Request, CStream& Response)
: MgOgcServer(Request,Response,ms_GlobalDefinitions)
, m_pLayers(NULL)
, m_pFeatureInfo(NULL)
{
    InitServer(kpszFilenameGlobalConfigWms,
               ms_GlobalDefinitions,
               ms_sExceptionTemplate,
               ms_sExceptionMimeType);
}

void MgOgcWmsServer::RespondToRequest()
{
    WmsRequestType requestType = GetRequestType();
    switch(requestType)
    {
        case GetCapabilitiesType:
        {
            GetCapabilitiesResponse();
            break;
        }
        case GetMapType:
        {
            GetMapResponse();
            break;
        }
        case GetFeatureInfoType:
        {
            GetFeatureInfoResponse();
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

// Validate the supplied request parameters
bool MgOgcWmsServer::ValidateRequest()
{
    bool bValid = true;
    WmsRequestType requestType = GetRequestType();
    switch(requestType)
    {
        case GetCapabilitiesType:
        {
            bValid = ValidateGetCapabilitiesParameters();
            break;
        }
        case GetMapType:
        {
            bValid = ValidateMapParameters();
            break;
        }
        case GetFeatureInfoType:
        {
            bValid = ValidateGetFeatureInfoParameters();
            break;
        }
        default:
        {
            ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszOperationNotSupported,
                                                         kpszExceptionMessageUnknownRequest));
            break;
        }
    }
    return bValid;
}

// Determine the request type for this WMS request
enum MgOgcWmsServer::WmsRequestType MgOgcWmsServer::GetRequestType()
{
    WmsRequestType requestType = UnknownType;

    // Get the value of the REQUEST parameter
    CPSZ pszRequest = RequestParameter(kpszQueryStringRequest);
    if(pszRequest != NULL)
    {
        if(SZ_EQI(pszRequest,kpszQueryValueGetCapabilities) ||
            SZ_EQI(pszRequest,kpszQueryValueCapabilities))
        {
            requestType = GetCapabilitiesType;
        }
        else if(SZ_EQI(pszRequest,kpszQueryValueGetMap) ||
            SZ_EQI(pszRequest,kpszQueryValueMap))
        {
            requestType = GetMapType;
        }
        else if(SZ_EQI(pszRequest,kpszQueryValueGetFeatureInfo) ||
            SZ_EQI(pszRequest,kpszQueryValueFeatureInfo))
        {
            requestType = GetFeatureInfoType;
        }
    }
    return requestType;
}

CPSZ MgOgcWmsServer::GetServiceType()
{
    return kpszQueryValueWms;
}

CPSZ MgOgcWmsServer::GetTemplatePrefix()
{
    return kpszFilenameTemplatePrefixWms;
}


// Gets the template info for default exception, when no other exception type is at hand.
void MgOgcWmsServer::GetDefaultExceptionInfo(REFSTRING sTemplate,REFSTRING sMimeType)
{
    sTemplate = ms_sExceptionTemplate;
    sMimeType = ms_sExceptionMimeType;
}



// Implements the WMS GetCapabilities response.
void MgOgcWmsServer::GetCapabilitiesResponse()
{
    if(ValidateGetCapabilitiesParameters())
    {
        // Looks for a specific FORMAT= parameter, and if missing,
        // uses the default of text/xml.
        //
        // 04-024 ISO DIS 19128:
        // 7.2.3.1 FORMAT
        //     The optional FORMAT parameter states the desired format of the service metadata. ...
        //     Every server shall support the default text/xml format defined in Annex A. Support for
        //     other formats is optional. ... If the request specifies a format not supported by the
        //     server, the server shall respond with the default text/xml format.
        CPSZ pszFormat = RequestParameter(kpszQueryStringFormat);
        if(pszFormat == NULL)
          pszFormat = kpszMimeTypeXml;

        // The WMS spec says if we can't honor the requested format...
        if(GenerateResponse(kpszQueryValueGetCapabilities,pszFormat))
            return;

        // ... we must respond with xml (if we didn't just try that already)
        if(SZ_NE(pszFormat,kpszMimeTypeXml) &&
            GenerateResponse(kpszQueryValueGetCapabilities,kpszMimeTypeXml))
            return;

        // Hmmm... this is bad.  We need to die gracefully.
        InternalError(kpszInternalErrorMissingGetCapyResponse);
    }
}

// Probably will remain hereafter a stub.
void MgOgcWmsServer::GetMapResponse()
{
    ValidateMapParameters();
}


// Create the response for a GetFeatureInfo request
void MgOgcWmsServer::GetFeatureInfoResponse()
{
    if(ValidateGetFeatureInfoParameters())
    {
        //
        // 04-024 ISO DIS 19128:
        // 7.4.3.9
        // The server shall return a response according to the requested
        // INFO_FORMAT if the request is valid, or issue a service exception
        // otherwise. The nature of the response is at the discretion of the
        // service provider, but it shall pertain to the feature(s) nearest
        // to (I,J).
        CPSZ pszFormat = RequestParameter(kpszQueryStringInfoFormat);

        // Be more lenient than the spec, and default to text/xml if not specified
        if(pszFormat == NULL)
          pszFormat = kpszMimeTypeXml;

        if(GenerateResponse(kpszQueryValueGetFeatureInfo,pszFormat))
            return;

        if(GenerateResponse(kpszQueryValueGetFeatureInfo,kpszMimeTypeXml))
            return;

        ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszOperationNotSupported,
                                                         kpszExceptionMessageGetFeatureInfoUnsupported));
    }
}

bool MgOgcWmsServer::ValidateGetCapabilitiesParameters()
{
    bool bValid = true;

    // Check for SERVICE=WMS -- otherwise, we're not prepared to service the request
    // 04-024 ISO DIS 19128:
    //  7.2.2 When making the GetCapabilities request of a WMS server, which may offer
    //        other service types as well, it is necessary to indicate that the client
    //        seeks information about the Web Map Service in particular. Thus, the SERVICE
    //        parameter of the request shall have the value "WMS" as shown in Table 3.
    //
    CPSZ pService = RequestParameter(kpszQueryStringService);
    if(pService == NULL || SZ_NEI(pService,kpszQueryValueWms))
    {
        // TODO: verify that kpszOperationNotSupported is the right one to send back.
        ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszOperationNotSupported,
                                                         kpszExceptionMessageMissingServiceWms));
        bValid = false;
    }

    return bValid;
}

bool MgOgcWmsServer::ValidateMapParameters()
{
    bool bValid = true;

    // Check the image format
    CPSZ imageFormat = RequestParameter(kpszQueryStringFormat);
    if(imageFormat != NULL && szlen(imageFormat) > 0)
    {
        if(SZ_NEI(imageFormat, kpszImageFormatPNG) &&
            SZ_NEI(imageFormat, kpszImageFormatJPEG) &&
            SZ_NEI(imageFormat, kpszImageFormatGIF))
        {
            ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszInvalidFormat,
                                                             kpszExceptionMessageInvalidImageFormat));
            bValid = false;
        }
    }
    else
    {
        // The required FORMAT parameter is missing
        ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszInvalidFormat,
                                                         kpszExceptionMessageMissingImageFormat));
        bValid = false;
    }

    if(bValid)
    {
        // Check that a list of layers was specified
        CPSZ mapLayerList = RequestParameter(kpszQueryStringLayers);
        if(mapLayerList == NULL || szlen(mapLayerList) == 0)
        {
            ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszLayerNotDefined,
                                                             kpszExceptionMessageMapLayersMissing));
            bValid = false;
        }
    }

    //Check that a spatial reference system was specified
    if(bValid)
    {
        // Look for a CRS parameter
        CPSZ crs = RequestParameter(kpszQueryStringCrs);
        if(crs == NULL || szlen(crs) == 0)
        {
            // Look for the older SRS parameter
            crs = RequestParameter(kpszQueryStringSrs);
            if(crs == NULL || szlen(crs) == 0)
            {
                ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszMissingCRS,
                                                     kpszExceptionMessageMissingCrs));
                bValid = false;
            }
        }
    }

    //Check that a bounding box was specified
    if(bValid)
    {
        // Look for a CRS parameter
        CPSZ bbox = RequestParameter(kpszQueryStringBBox);
        if(bbox == NULL || szlen(bbox) == 0)
        {
            ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszMissingBoundingBox,
                                                 kpszExceptionMessageMissingBoundingBox));
            bValid = false;
        }
        else
        {
            Ptr<MgStringCollection> bboxParams = MgStringCollection::ParseCollection(bbox, _(","));
            if(bboxParams != NULL && bboxParams->GetCount() == 4)
            {
                double minX = MgUtil::StringToDouble(bboxParams->GetItem(0));
                double minY = MgUtil::StringToDouble(bboxParams->GetItem(1));
                double maxX = MgUtil::StringToDouble(bboxParams->GetItem(2));
                double maxY = MgUtil::StringToDouble(bboxParams->GetItem(3));
                if(minX >= maxX || minY >= maxY)
                {
                    ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszInvalidBoundingBox,
                                                 kpszExceptionMessageInvalidBoundingBox));
                    bValid = false;
                }
            }
            else
            {
                    ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszInvalidBoundingBox,
                                                 kpszExceptionMessageInvalidBoundingBox));
                    bValid = false;
            }
        }
    }

    // Check that a width and height were specified
    if(bValid)
    {
        CPSZ widthString = RequestParameter(kpszQueryStringWidth);
        CPSZ heightString = RequestParameter(kpszQueryStringHeight);
        if(widthString == NULL || heightString == NULL || szlen(widthString) == 0 || szlen(heightString) == 0)
        {
            ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszMissingDimensionValue,
                                                     kpszExceptionMessageMissingMapDimension));
            bValid = false;
        }
        else
        {
            // Check that the width and height are greater than zero
            INT32 mapWidth = MgUtil::StringToInt32(widthString);
            INT32 mapHeight = MgUtil::StringToInt32(heightString);
            if(mapWidth <= 0 || mapHeight <= 0)
            {
                ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszMissingDimensionValue,
                                                         kpszExceptionMessageInvalidMapDimension));
                bValid = false;
            }
        }
    }

    return bValid;
}

bool MgOgcWmsServer::ValidateGetFeatureInfoParameters()
{
    bool bValid = ValidateMapParameters();
    if(bValid)
    {
        // Make sure we have a query point specified by I and J parameters
        CPSZ iCoordString = RequestParameter(kpszQueryStringICoord);
        CPSZ jCoordString = RequestParameter(kpszQueryStringJCoord);
        if(iCoordString == NULL || jCoordString == NULL || szlen(iCoordString) == 0 || szlen(jCoordString) == 0)
        {
            // If we don't have I and J, look for X and Y
            iCoordString = RequestParameter(kpszQueryStringXCoord);
            jCoordString = RequestParameter(kpszQueryStringYCoord);
            if(iCoordString == NULL || jCoordString == NULL || szlen(iCoordString) == 0 || szlen(jCoordString) == 0)
            {
                ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszInvalidPoint,
                                                         kpszExceptionMessageMissingQueryPoint));
                bValid = false;
            }
        }
        if(bValid)
        {
            // Make sure the query point falls within the bounds of the map
            INT32 iValue = MgUtil::StringToInt32(iCoordString);
            INT32 jValue = MgUtil::StringToInt32(jCoordString);
            INT32 mapWidth = MgUtil::StringToInt32(RequestParameter(kpszQueryStringWidth));
            INT32 mapHeight = MgUtil::StringToInt32(RequestParameter(kpszQueryStringHeight));
            if(iValue < 0 || jValue < 0 || iValue > mapWidth || jValue > mapHeight)
            {
                ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszInvalidPoint,
                                                         kpszExceptionMessageInvalidQueryPoint));
                bValid = false;
            }
        }
    }
    if(bValid)
    {
        // Check that all query layers are present in the map
        CPSZ queryLayerList = RequestParameter(kpszQueryStringQueryLayers);
        if(queryLayerList != NULL && szlen(queryLayerList) > 0)
        {
            // The LAYERS param has already been validated in ValidateMapParameters()
            CPSZ mapLayerList = RequestParameter(kpszQueryStringLayers);
            Ptr<MgStringCollection> mapLayers = MgStringCollection::ParseCollection(mapLayerList, L",");

            Ptr<MgStringCollection> queryLayers = MgStringCollection::ParseCollection(queryLayerList, L",");
            for(int i=0; i<queryLayers->GetCount(); i++)
            {
                if(!mapLayers->Contains(queryLayers->GetItem(i)))
                {
                    ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszLayerNotDefined,
                                                             kpszExceptionMessageQueryLayerNotDefined));
                    bValid = false;
                }
            }
        }
    }

    return bValid;
}


bool MgOgcWmsServer::ProcessOtherInstruction(CREFSTRING sProc,MgXmlProcessingInstruction& PI)
{
    if(sProc == kpszPiEnumLayers)
    {
        ProcedureEnumLayers(PI);
    }
    else if(sProc == kpszPiEnumFeatureProperties)
    {
        ProcedureEnumFeatureProperties(PI);
    }
    else
    {
        return false;
    }

    return true;

}

// Interpret a <?EnumLayers using= ?> processing instruction
// This will enumerate all known layers that the server reports
// are elligble for WMS publication.
void MgOgcWmsServer::ProcedureEnumLayers(MgXmlProcessingInstruction& PIEnum)
{
    STRING sFormat;
    if(!PIEnum.GetAttribute(kpszPiAttributeUsing,sFormat))
        sFormat = kpszPiEnumLayersDefaultFormat;

    STRING sSubset;
    if(!PIEnum.GetAttribute(kpszPiAttributeSubset,sSubset))
        sSubset = kpszEmpty;
    ProcessExpandableTextIntoString(sSubset,sSubset);

    int iNum = 0;
    while(m_pLayers->Next()) {
        // We ensure that each layer gets its own stack frame
        // so definitions don't get carried over to the next layer.
        CDictionaryStackFrame ForLayers(this);

        m_pLayers->GenerateDefinitions(*m_pTopOfDefinitions);

        CPSZ pszIsPublished = this->Definition(_("Layer.IsPublished"));
        if(pszIsPublished != NULL && SZ_EQ(pszIsPublished,_("1")))
            if(this->IsIterationInSubset(++iNum,sSubset,_("Layer.iteration")))
                ProcessExpandableText(sFormat);
    }
}

// Interpret a <?EnumFeatureProperties using= ?> processing instruction
// This will enumerate all feature properties
void MgOgcWmsServer::ProcedureEnumFeatureProperties(MgXmlProcessingInstruction& PIEnum)
{
    STRING sFormat;
    if(!PIEnum.GetAttribute(kpszPiAttributeUsing,sFormat))
        sFormat = kpszPiEnumFeaturePropertiesDefaultFormat;

    CDictionaryStackFrame forFeatureProperties(this);

    while(m_pFeatureInfo->Next())
    {
        m_pFeatureInfo->GenerateDefinitions(*m_pTopOfDefinitions);
        ProcessExpandableText(sFormat);
    }
}

// Set the feature info (used for generating GetFeatureInfo response)
void MgOgcWmsServer::SetFeatureInfo(MgWmsFeatureInfo* pFeatureInfo)
{
    m_pFeatureInfo = pFeatureInfo;
}

