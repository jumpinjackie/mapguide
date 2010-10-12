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
#include "OgcWmsServer.h"
#include "MapGuideCommon.h"

CPSZ kpszQueryValueWms              = _("WMS");
CPSZ kpszQueryStringInfoFormat      = _("info_format");
CPSZ kpszQueryStringLayers          = _("layers");
CPSZ kpszQueryStringStyles          = _("styles");
CPSZ kpszQueryStringQueryLayers     = _("query_layers");
CPSZ kpszQueryStringICoord          = _("i");
CPSZ kpszQueryStringJCoord          = _("j");
CPSZ kpszQueryStringXCoord          = _("x");
CPSZ kpszQueryStringYCoord          = _("y");
CPSZ kpszQueryStringWidth           = _("width");
CPSZ kpszQueryStringHeight          = _("height");

CPSZ kpszFilenameGlobalConfigWms    = _("Wms:OgcWmsService.config");
CPSZ kpszFilenameTemplatePrefixWms  = _("Wms:");

CPSZ kpszDefineReferenceSystems     = _("ReferenceSystems");
CPSZ kpszDefineItem                 = _("item");
// Supported image formats
// This is the name of the define used to
// identify supported formats.  It is to contain
// a value map, which will translate supported mime types
// to internal RenderingService values.
CPSZ kpszDefineSupportedFormats     = _("RenderingService.SupportedFormats");
// This is the default definition.  Can be overridden
// in the template.
CPSZ kpszDefaultSupportedFormats    =  _("<translate from=\"image/jpeg\">JPG</translate>")
                                       _("<translate from=\"image/png\">PNG</translate>")
                                       _("<translate from=\"image/gif\">GIF</translate>")
                                       _("<translate from=\"image/tiff\">TIF</translate>")
                                       // Below is REQUIRED.  Illegal strings should map to "".
                                       _("<translate></translate>");


// GetCapabilities XML format
CPSZ kpszMimeTypeApplicationWmsXml                = _("application/vnd.ogc.wms_xml");

// Plain ol' "GetCapabilities" is general to all Ogc Services, so it lives in the OgcServer source.
CPSZ kpszQueryValueCapabilities     = _("Capabilities"); // keyword for pre 1.0.8 versions
CPSZ kpszQueryValueGetMap           = _("GetMap");
CPSZ kpszQueryValueMap              = _("Map");          // keyword for pre 1.0.8 versions
CPSZ kpszQueryValueGetFeatureInfo   = _("GetFeatureInfo");
CPSZ kpszQueryValueFeatureInfo      = _("FeatureInfo");  // keyword for pre 1.0.8 versions

// BEGIN LOCALIZATION -- The following strings are all subject to localization.
CPSZ kpszExceptionMessageMissingServiceWms    = _("Expected required SERVICE=WMS argument."); // Localize
CPSZ kpszExceptionMessageMissingRequest       = _("Expected required REQUEST= argument.");    // Localize
CPSZ kpszExceptionMessageUnknownRequest       = _("Expected valid REQUEST= argument, such as GetCapabilities. (Instead, found request='&Request.request;'.)"); // Localize
CPSZ kpszInternalErrorMissingGetCapyResponse  = _("Unable to generate a GetCapabilities Response --  missing a text/xml Response in the template file."); // Localize
CPSZ kpszInternalErrorMissingDefinition       = _("Unable to generate a response; missing a required internal definition."); // Localize
CPSZ kpszExceptionMessageGetFeatureInfoUnsupported = _("REQUEST=GetFeatureInfo operation is not supported on this server."); // Localize
CPSZ kpszExceptionMessageQueryLayerNotDefined = _("One or more layers specified in the QUERY_LAYERS argument are not defined in the map. (Found QUERY_LAYERS=&Request.query_layers;)"); // Localize
CPSZ kpszExceptionMessageMapLayersMissing     = _("The map request must contain at least one valid layer."); // Localize
CPSZ kpszExceptionMessageMapLayerNotDefined   = _("The map request contains invalid layer names. (Found LAYERS=&Request.layers;)"); // Localize
CPSZ kpszExceptionMessageMapLayerNotQueryable = _("The GetFeatureInfo request contains the names of layers that are not queryable. (Found LAYERS=&Request.layers;)"); // Localize
CPSZ kpszExceptionMessageStyleNotDefined      = _("An unsupported layer style was requested. Only default layer styles are supported."); // Localize
CPSZ kpszExceptionMessageMissingQueryPoint    = _("The request must contain I and J arguments specifying the query point. (Found I=&Request.I; J=&Request.J;)"); // Localize
CPSZ kpszExceptionMessageInvalidQueryPoint    = _("The point specified by the I and J arguments must fall within the map extent. (Found I=&Request.I; J=&Request.J; WIDTH=&Request.width; HEIGHT=&Request.height;)"); // Localize
CPSZ kpszExceptionMessageMissingMapDimension  = _("The request must contain both WIDTH and HEIGHT arguments to specify the map size."); // Localize
CPSZ kpszExceptionMessageInvalidMapDimension  = _("The WIDTH and HEIGHT arguments must be positive integer values. (Found WIDTH=&Request.width; HEIGHT=&Request.height; )"); // Localize
CPSZ kpszExceptionMessageMissingCrs           = _("The spatial reference system must be specified using either the CRS or SRS argument. Found neither."); // Localize
CPSZ kpszExceptionMessageInvalidCrs           = _("The spatial reference system specified is not supported. (Found CRS=&Request.CRS;)"); // Localize
CPSZ kpszExceptionMessageInvalidSrs           = _("The spatial reference system specified is not supported. (Found SRS=&Request.SRS;)"); // Localize
CPSZ kpszExceptionMessageMissingBoundingBox   = _("The bounding box for the map must be specified using the BBOX argument."); // Localize
CPSZ kpszExceptionMessageInvalidBoundingBox   = _("The bounding box for the map must be specified using four numerical values in the order: minX,minY,maxX,maxY. (Found BBOX=&Request.bbox;)"); // Localize
CPSZ kpszExceptionMessageInvalidImageFormat   = _("The request uses an unsupported image format. (Found FORMAT=&Request.format;)"); // Localize
CPSZ kpszExceptionMessageMissingImageFormat   = _("The request must contain a FORMAT parameter to specify the required image format."); // Localize
CPSZ kpszExceptionMessageMissingInfoFormat    = _("The request must contain an INFO_FORMAT parameter to specify the format of feature information (MIME type)."); // Localize
CPSZ kpszExceptionMessageMissingQueryLayers   = _("The request must contain a QUERY_LAYERS parameter to specify one or more layers to be queried."); // Localize
CPSZ kpszExceptionMessageInvalidInfoFormat    = _("The request uses an unsupported info format. (Found INFO_FORMAT=&Request.info_format;)"); // Localize
CPSZ kpszExceptionMessageMissingVersion       = _("The request must contain a VERSION parameter to specify the WMS version."); // Localize
// END LOCALIZATION

CPSZ kpszPiEnumLayers                      = _("EnumLayers");
CPSZ kpszPiEnumFeatureProperties           = _("EnumFeatureProperties");
CPSZ kpszPiEnumFeatureInfo                 = _("EnumFeatureInfo");
extern CPSZ kpszPiAttributeUsing;  // for consistency, borrow the "using" attribute from base class.
CPSZ kpszPiEnumLayersDefaultFormat         = _("&Layer.Name;\n");
CPSZ kpszPiEnumFeatureInfoDefaultFormat    = _("&FeatureInfo.xml;");
CPSZ kpszPiEnumFeaturePropertiesDefaultFormat = _("&FeatureProperty.xml;");

extern CPSZ kpszPiAttributeSubset;      // = _("subset");
extern CPSZ kpszDefinitionInitServerError; // inherited from OgcServer.


MgUtilDictionary MgOgcWmsServer::ms_GlobalDefinitions(NULL);
STRING           MgOgcWmsServer::ms_sExceptionTemplate;
STRING           MgOgcWmsServer::ms_sExceptionMimeType;


// Standard constructor.
MgOgcWmsServer::MgOgcWmsServer(MgHttpRequestParameters& Request,CStream& Response,MgWmsLayerDefinitions* pLayers)
: MgOgcServer(Request,Response,ms_GlobalDefinitions)
, m_pLayers(pLayers)
, m_pFeatureInfo(NULL)
{
    InitWmsServer(kpszFilenameGlobalConfigWms);
}

MgOgcWmsServer::MgOgcWmsServer(MgHttpRequestParameters& Request, CStream& Response)
: MgOgcServer(Request,Response,ms_GlobalDefinitions)
, m_pLayers(NULL)
, m_pFeatureInfo(NULL)
{
    InitWmsServer(kpszFilenameGlobalConfigWms);
}

bool MgOgcWmsServer::InitWmsServer(CPSZ pszFilenameGlobalConfig)
{
    bool bRet = InitServer(kpszFilenameGlobalConfigWms,
                           ms_GlobalDefinitions,
                           ms_sExceptionTemplate,
                           ms_sExceptionMimeType);
    // First time initialization?  Time to augment it.
    if(bRet) {
        // Initialize the supported formats, unless the config template
        // already has.
        if(ms_GlobalDefinitions.FindName(kpszDefineSupportedFormats) < 0)
            ms_GlobalDefinitions.AddDefinition(kpszDefineSupportedFormats,kpszDefaultSupportedFormats);
    }

    return bRet;
}

void MgOgcWmsServer::RespondToRequest()
{
    WmsRequestType requestType = GetRequestType();
    switch(requestType)
    {
        case WmsGetCapabilitiesType:
        {
            GetCapabilitiesResponse();
            break;
        }
        case WmsGetMapType:
        {
            GetMapResponse();
            break;
        }
        case WmsGetFeatureInfoType:
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
        case WmsGetCapabilitiesType:
        {
            bValid = ValidateGetCapabilitiesParameters();
            break;
        }
        case WmsGetMapType:
        {
            bValid = ValidateMapParameters();
            break;
        }
        case WmsGetFeatureInfoType:
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
    WmsRequestType requestType = WmsUnknownType;

    // Get the value of the REQUEST parameter
    CPSZ pszRequest = RequestParameter(kpszQueryStringRequest);
    if(pszRequest != NULL)
    {
        if(SZ_EQI(pszRequest,kpszQueryValueGetCapabilities) ||
            SZ_EQI(pszRequest,kpszQueryValueCapabilities))
        {
            requestType = WmsGetCapabilitiesType;
        }
        else if(SZ_EQI(pszRequest,kpszQueryValueGetMap) ||
            SZ_EQI(pszRequest,kpszQueryValueMap))
        {
            requestType = WmsGetMapType;
        }
        else if(SZ_EQI(pszRequest,kpszQueryValueGetFeatureInfo) ||
            SZ_EQI(pszRequest,kpszQueryValueFeatureInfo))
        {
            requestType = WmsGetFeatureInfoType;
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
    // Looks for a specific FORMAT parameter
    CPSZ pszFormat = RequestParameter(kpszQueryStringFormat);
    if(pszFormat != NULL)
    {
        // Make a request for the user-specified format
        if(GenerateResponse(kpszQueryValueGetCapabilities,pszFormat))
            return;
    }

    // If we get to here, either the user did not specify a format,
    // or we were unable to honor the requested format. So we try the
    // default format instead.

    // Default mime type for version 1.0.0, 1.3.0 and higher
    CPSZ pszDefaultFormat = kpszMimeTypeXml;
    CPSZ pszVersion = NegotiatedVersion();
    if(pszVersion != NULL && (SZ_EQI(pszVersion, _("1.1.0")) || SZ_EQI(pszVersion, _("1.1.1"))))
    {
        // Default mime type for 1.1.0 and 1.1.1 requests
        pszDefaultFormat = kpszMimeTypeApplicationWmsXml;
    }

    // Make a request for the default format
    if(pszFormat == NULL || SZ_NE(pszFormat, pszDefaultFormat))
    {
        if(GenerateResponse(kpszQueryValueGetCapabilities,pszDefaultFormat))
            return;
    }

    // Hmmm... this is bad.  We need to die gracefully.
    InternalError(kpszInternalErrorMissingGetCapyResponse);
}

// Probably will remain hereafter a stub.
void MgOgcWmsServer::GetMapResponse()
{
}


// Create the response for a GetFeatureInfo request
void MgOgcWmsServer::GetFeatureInfoResponse()
{
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
    return ValidateMapParameters(NULL);
}

// If the queryableLayers collection object is not NULL, we populate it with the
// names of the queryable layers when we parse for published layers.
bool MgOgcWmsServer::ValidateMapParameters(MgStringCollection* queryableLayers)
{
    bool bValid = true;

    //  Search for a VERSION= parameter
    STRING version = GetRequestVersion();
    if(version.empty())
    {
        ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszMissingVersion,
                                       kpszExceptionMessageMissingVersion));
        bValid = false;
    }

    //
    //  Search for a FORMAT= parameter, and verify that its value is supported.
    //
    CPSZ imageFormat = RequestParameter(kpszQueryStringFormat);
    if(bValid)
    {
        if(imageFormat != NULL && szlen(imageFormat) > 0)
        {
            // We're expecting a supported formats definition.  There's a hard-coded
            // default, but the templates can also define one.
            CPSZ pszMap = this->Definition(kpszDefineSupportedFormats);
            if(pszMap) {
                MgXmlParser Xml(pszMap);
                STRING sTo;
                if(!MapValue(Xml,imageFormat,sTo) || sTo.length() == 0) {
                    ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszInvalidFormat,
                                                                     kpszExceptionMessageInvalidImageFormat));
                    bValid = false;
                }
            }
            else {
                this->m_pTopOfDefinitions->AddDefinition(kpszDefinitionInitServerError,kpszDefineSupportedFormats);
                InternalError(kpszInternalErrorMissingDefinition);
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
    }

    //
    // Look for the LAYERS= parameter and parse its value into the comma-separates list of values.
    //
    // Check that a list of layers was specified
    CPSZ mapLayerList = RequestParameter(kpszQueryStringLayers); // greater scope, we use this again later.
    if(bValid)
    {
        if(mapLayerList == NULL || szlen(mapLayerList) == 0)
        {
            ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszLayerNotDefined,
                                                             kpszExceptionMessageMapLayersMissing));
            bValid = false;
        }
        else
        {
            // Get a list of the requested layers
            Ptr<MgStringCollection> requestedLayers = MgStringCollection::ParseCollection(mapLayerList, L",");

            // Get a list of the available layers
            Ptr<MgStringCollection> availableLayers = new MgStringCollection();
            if(m_pLayers != NULL)
            {
                while(m_pLayers->Next())
                {
                    MgUtilDictionary currentDef(NULL);
                    m_pLayers->GenerateDefinitions(currentDef);
                    CPSZ layerName = currentDef[L"Layer.Name"];
                    CPSZ isPublished = currentDef[L"Layer.IsPublished"];
                    if(isPublished != NULL && SZ_EQ(isPublished, L"1"))
                    {
                        // Add this layer to the list of published layers
                        availableLayers->Add(layerName);
                        if(queryableLayers != NULL)
                        {
                            CPSZ isQueryable = currentDef[L"Layer.Queryable"];
                            if(isQueryable != NULL && SZ_EQ(isQueryable, L"1"))
                            {
                                // Add this layer to the list of queryable layers
                                queryableLayers->Add(layerName);
                            }
                        }
                    }
                }
            }

            // Verify that all of the requested layers are available
            for(int i = 0; i < requestedLayers->GetCount(); i++)
            {
                if(!availableLayers->Contains(requestedLayers->GetItem(i)))
                {
                    ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszLayerNotDefined,
                                                                     kpszExceptionMessageMapLayerNotDefined));
                    bValid = false;
                    break;
                }
            }
        }
    }

    //
    // Examine the STYLES= parameter; (currently only support default style)
    //
    if(bValid)
    {
        // Since we only support default styles, check that no specific
        // style was requested.
        CPSZ styleList = RequestParameter(kpszQueryStringStyles);
        if(styleList != NULL && szlen(styleList) > 0)
        {
            Ptr<MgStringCollection> styles = MgStringCollection::ParseCollection(styleList, _(","));
            if(styles != NULL && styles->GetCount() > 0)
            {
                for(int i = 0; i < styles->GetCount(); i++)
                {
                    STRING styleName = styles->GetItem(i);
                    if(styleName.length() > 0)
                    {
                        ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszStyleNotDefined,
                                                                         kpszExceptionMessageStyleNotDefined));
                        bValid = false;
                        break;
                    }
                }
            }
        }
    }

    //
    // Examine the SRS= (or CRS=) parameter value.  We also verify that it's advertised
    // in the capability document.
    //
    if(bValid)
    {
        bool bArgIsCRS = true;
        // Look for a CRS parameter a la 1.3.0 (and beyond?)
        CPSZ crs = RequestParameter(kpszQueryStringCrs);
        if(crs == NULL || szlen(crs) == 0)
        {
            bArgIsCRS = false;
            // Look for the older SRS parameter (versions prior to 1.3.0)
            crs = RequestParameter(kpszQueryStringSrs);
            if(crs == NULL || szlen(crs) == 0)
            {
                ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszMissingCRS,
                                                                 kpszExceptionMessageMissingCrs));
                bValid = false;
            }
        }

        if(bValid)
        {
            bool bGlobalMatch =  false;

            // Search the globally supported reference systems for a match for the
            // requested SRS.
            CPSZ globalSRSs = this->Definition(kpszDefineReferenceSystems);
            if(globalSRSs != NULL)
            {
                MgXmlParser parser(globalSRSs);
                while(!parser.AtEnd())
                {
                    if(parser.Current().Type() == keBeginElement)
                    {
                        MgXmlSynchronizeOnElement srsItemElement(parser, kpszDefineItem);
                        MgXmlBeginElement* pBegin;
                        // Are we at the beginning of an <item> element?
                        if(srsItemElement.AtBegin(&pBegin))
                        {
                            parser.Next();
                            STRING sText = parser.Current().Contents();
                            if(SZ_EQI(sText.c_str(), crs))
                            {
                                bGlobalMatch = true;
                                break;
                            }
                        }
                        else
                        {
                            parser.Next();
                        }
                    }
                    else
                    {
                        parser.Next();
                    }
                }
            }

            // Now, given our SRS/CRS, and the known list of layers, do we support the
            // reference system being asked for?
            if(!bGlobalMatch && !m_pLayers->LayerSupportsReferenceSystem(mapLayerList,crs)) {
                ServiceExceptionReportResponse(MgOgcWmsException(bArgIsCRS ? MgOgcWmsException::kpszInvalidCRS
                                                                           : MgOgcWmsException::kpszInvalidSRS,
                                                                 bArgIsCRS ? kpszExceptionMessageInvalidCrs
                                                                           : kpszExceptionMessageInvalidSrs) );
                bValid = false;
            }
        }
    }

    //
    // Examine the BBOX= parameter value.  We ensure that the arguments are in the right order.
    //
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
            STRING sBBox(bbox);
            if(version >= _("1.3.0"))
            {
                CPSZ crs = RequestParameter(kpszQueryStringCrs);
                if(crs == NULL || szlen(crs) == 0)
                {
                    crs = RequestParameter(kpszQueryStringSrs);
                }
                MgWmsMapUtil::ProcessBoundingBoxAxes(crs,sBBox);
            }
            Ptr<MgStringCollection> bboxParams = MgStringCollection::ParseCollection(sBBox, _(","));
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

    //
    // Examine the WIDTH= and HEIGHT= parameters, ensuring that both are present and positive.
    //
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
    Ptr<MgStringCollection> queryableLayers = new MgStringCollection();

    // The ValidateMapParameters method will check that the map parameters in the
    // request are valid, and populate our list of queryable layers.
    bool bValid = ValidateMapParameters(queryableLayers);
    if(bValid)
    {
        // INFO_FORMAT becomes a mandatory parameter since WMS1.3.0
        STRING version = GetRequestVersion();
        // Note: lexical comparison, not numerical one;
        if(!version.empty() && version >= _("1.3.0"))
        {
            CPSZ pszFormat = RequestParameter(kpszQueryStringInfoFormat);
            if(pszFormat == NULL)
            {
                ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszMissingInfoFormat,
                                                         kpszExceptionMessageMissingInfoFormat));
                bValid = false;
            }

            CPSZ pszSupportedFormats = this->Definition(_("Formats.GetFeatureInfo")); //NOXLATE
            bool bSupport = false;

            if(pszSupportedFormats != NULL)
            {
                MgXmlParser SupportedFormats(pszSupportedFormats);

                while(SupportedFormats.Next())
                {
                    MgXmlNode& node = SupportedFormats.Current();
                    if(node.Contents() == pszFormat)
                    {
                        bSupport = true;
                        break;
                    }
                }
            }

            if(!bSupport)
            {
                ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszInvalidFormat,
                                                         kpszExceptionMessageInvalidInfoFormat));
                bValid = false;
            }
        }
    }
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
        if(queryLayerList == NULL || szlen(queryLayerList) == 0)
        {
            ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszMissingQueryLayers,
                                                     kpszExceptionMessageMissingQueryLayers));
            bValid = false;
        }
        else
        {
            // The LAYERS param has already been validated in ValidateMapParameters()
            CPSZ mapLayerList = RequestParameter(kpszQueryStringLayers);
            Ptr<MgStringCollection> mapLayers = MgStringCollection::ParseCollection(mapLayerList, L",");

            // Retrieve the layer names to query
            Ptr<MgStringCollection> queryLayers = MgStringCollection::ParseCollection(queryLayerList, L",");
            for(int i=0; i<queryLayers->GetCount(); i++)
            {
                STRING layerName = queryLayers->GetItem(i);

                // Check that this layer is part of the map
                if(!mapLayers->Contains(layerName))
                {
                    ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszLayerNotDefined,
                                                             kpszExceptionMessageQueryLayerNotDefined));
                    bValid = false;
                    break;
                }

                // Check that this layer is queryable
                if(!queryableLayers->Contains(layerName))
                {
                    ServiceExceptionReportResponse(MgOgcWmsException(MgOgcWmsException::kpszLayerNotQueryable,
                                                                     kpszExceptionMessageMapLayerNotQueryable));
                    bValid = false;
                    break;
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
    else if(sProc == kpszPiEnumFeatureInfo)
    {
        ProcedureEnumFeatureInfo(PI);
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

// Interpret a <?EnumFeatureInfo using= ?> processing instruction
// This will enumerate all feature info property sets
void MgOgcWmsServer::ProcedureEnumFeatureInfo(MgXmlProcessingInstruction& PIEnum)
{
    STRING sFormat;
    if(!PIEnum.GetAttribute(kpszPiAttributeUsing,sFormat))
        sFormat = kpszPiEnumFeatureInfoDefaultFormat;

    CDictionaryStackFrame forFeatureInfo(this);

    while(m_pFeatureInfo->Next())
    {
        m_pFeatureInfo->GenerateDefinitions(*m_pTopOfDefinitions);
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

    if(m_pFeatureInfo != NULL)
    {
        Ptr<MgWmsFeatureProperties> pProps = m_pFeatureInfo->GetCurrentProperties();
        if(pProps != NULL)
        {
            while(pProps->Next())
            {
                pProps->GenerateDefinitions(*m_pTopOfDefinitions);
                ProcessExpandableText(sFormat);
            }
        }
    }
}

// Set the feature info (used for generating GetFeatureInfo response)
void MgOgcWmsServer::SetFeatureInfo(MgWmsFeatureInfo* pFeatureInfo)
{
    m_pFeatureInfo = SAFE_ADDREF(pFeatureInfo);
}

// Set the layer definitions
void MgOgcWmsServer::SetLayerDefs(MgWmsLayerDefinitions* pLayerDefs)
{
    m_pLayers = SAFE_ADDREF(pLayerDefs);
}

// Get WMS request version
STRING MgOgcWmsServer::GetRequestVersion()
{
    CPSZ version = RequestParameter(kpszQueryStringVersion);
    if(version == NULL)
    {
        // In WMS version 1.0.0, the name of this parameter was "WMTVER".
        // That name is now deprecated, but for backwards compatibility and
        // version negotiation a post-1.0.0 server should accept
        // either form without issuing a Service Exception.
        version = RequestParameter(kpszQueryStringWmtVersion);
    }

    return STRING(version? version : _(""));
}
