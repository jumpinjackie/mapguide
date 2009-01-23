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
#include "HttpWmsGetMap.h"
#include "WmsMapUtil.h"

#include "OgcFramework.h"
#include "OgcWmsServer.h"

extern CPSZ kpszDefineSupportedFormats; // borrowed. TODO: publish as member variable.

// TODO: migrate these to a common Mg*.h header (not Ogc*.h) for use in other handlers.

///////////////////////////////////////////////////////////////////////////////////////////
//                                      MgException* or derivative                       //
//                                      |                 MgOgcWmsException::kpsz...     //
//                                      |                 |                  OgcServer&  //
//                                      |                 |                  |           //
#define CATCH_MGEXCEPTION_HANDLE_AS_OGC(mg_exception_type,ogc_exception_code,ogc_server)  \
        catch (mg_exception_type* e) {                                                    \
            STRING sReport = e->GetMessage();                                             \
            sReport += _("<details>");                                                    \
            sReport += e->GetDetails();                                                   \
            sReport += _("</details>");                                                   \
            ogc_server.ServiceExceptionReportResponse(                                    \
                MgOgcWmsException(MgOgcWmsException::ogc_exception_code,                  \
                                  sReport.c_str() ));                                     \
            Ptr<MgByteReader> capabilities = responseStream.Stream().GetReader();         \
            hResult->SetResultObject(capabilities, capabilities->GetMimeType());          \
            e->Release();                                                                 \
        }                                                                                 \
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
//                                   MgOgcWmsException::kpsz...                          //
//                                   |                  OgcServer&                       //
//                                   |                  |                                //
#define CATCH_ANYTHING_HANDLE_AS_OGC(ogc_exception_code,ogc_server)                       \
        catch (...) {                                                                     \
            ogc_server.ServiceExceptionReportResponse(                                    \
                MgOgcWmsException(MgOgcWmsException::kpszInternalError,                   \
                                  _("Unexpected exception was thrown.  No additional details available.")));\
            Ptr<MgByteReader> capabilities = responseStream.Stream().GetReader();         \
            hResult->SetResultObject(capabilities, capabilities->GetMimeType());          \
        }                                                                                 \
///////////////////////////////////////////////////////////////////////////////////////////

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpWmsGetMap)

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
MgHttpWmsGetMap::MgHttpWmsGetMap(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);
}


void MgHttpWmsGetMap::InitializeRequestParameters(MgOgcWmsServer& oServer)
{
    // Get the WMS request version
    m_version = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsVersion);

    // Get the requested layers
    m_layerDefIds = MgWmsMapUtil::GetLayerDefinitionIds(
        GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsLayers));

    // Get the requested styles
    m_styles = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsStyles);

    // Get the requested CRS (Ver 1.3) or SRS (Ver 1.1) value
    m_crs = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsCrs);
    if(m_crs.length() == 0)
    {
        m_crs = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsSrs);
    }

    // Get the requested styles
    m_bbox = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsBbox);

    // Get width and convert to integer
    STRING sParameter;
    sParameter = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsWidth);
    m_width = MgUtil::StringToInt32(sParameter);

    // Get height and convert to integer
    sParameter = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsHeight);
    m_height = MgUtil::StringToInt32(sParameter);

    // Get the requested format
    m_format = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsFormat);

    /////////////////////////////////////////////////
    // Optional parameters
    /////////////////////////////////////////////////

    // Background transparency
    STRING transparent = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsTransparent);
    m_transparent = SZ_EQI(transparent.c_str(),L"TRUE");

    // Background color
    m_bgColor = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsBackgroundColor);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpWmsGetMap::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Declare the method we'd like the system to use for resolving
    // loader documents.
    // (Must be done before instancing an MgOgc{Wms}Server)
    MgOgcServer::SetLoader(MgWmsMapUtil::GetDocument);

    MgUserInformation::SetCurrentUserInfo(m_userInfo);

    // We have to wrap the request parameters, since the outside
    // world is not case-insensitive (with respect to names,) but
    // WMS is.
    Ptr<MgHttpRequestParam> origReqParams = m_hRequest->GetRequestParam();
    MgHttpRequestParameters requestParams(origReqParams);

    // Create the WMS handler
    MgHttpResponseStream responseStream;
    MgOgcWmsServer wms(requestParams, responseStream);
    if(wms.ProcessRequest(this))
    {
        // Get an instance of the resource service
        Ptr<MgResourceService> resourceService = (MgResourceService*)CreateService(MgServiceType::ResourceService);

        // Get the background color
        Ptr<MgColor> bkColor = MgWmsMapUtil::GetBackgroundColor(m_bgColor, m_transparent);

        // Get the extents
        Ptr<MgEnvelope> extents = MgWmsMapUtil::GetExtents(m_bbox);

        try {
            // Get a map object corresponding to the request parameters
            Ptr<MgMap> map = MgWmsMapUtil::GetMap(wms, m_layerDefIds, m_bbox, m_crs,
                m_width, m_height, resourceService);

            // Get the image format
            // Note: should be valid, since this mapping has already happened
            // in the Validate phase.
            STRING format;
            wms.MapValue(kpszDefineSupportedFormats,m_format.c_str(),format);

            // Render the map
            Ptr<MgRenderingService> renderingService = (MgRenderingService*)CreateService(MgServiceType::RenderingService);
            Ptr<MgSelection> selection;
            Ptr<MgByteReader> mapImage = renderingService->RenderMap(map, selection, extents, m_width, m_height, bkColor, format);

            // Set the result
            STRING sMimeType = mapImage->GetMimeType();
            hResult->SetResultObject(mapImage, sMimeType.length() > 0 ? sMimeType : m_format);
        }
        //  Custom catch clauses.  In short, NO, we do NOT want to let MapGuide exceptions
        //  pass through.  The buck stops here, with an exception report that WE generate
        //  according to OGC specifications.
        CATCH_MGEXCEPTION_HANDLE_AS_OGC(MgInvalidCoordinateSystemException,kpszInvalidCRS,   wms)
        CATCH_MGEXCEPTION_HANDLE_AS_OGC(MgException,                       kpszInternalError,wms)
        CATCH_ANYTHING_HANDLE_AS_OGC(                                      kpszInternalError,wms)
    }
    else
    {
        // Obtain the response byte reader
        Ptr<MgByteReader> errorResponse = responseStream.Stream().GetReader();

        // Set the result
        hResult->SetResultObject(errorResponse, errorResponse->GetMimeType());
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpWmsGetMap.Execute")
}

void MgHttpWmsGetMap::AcquireValidationData(MgOgcServer* ogcServer)
{
    MgOgcWmsServer* wmsServer = (MgOgcWmsServer*)ogcServer;
    if(wmsServer != NULL)
    {
        // The initialization that used to happen in the ctor is deferred until now
        // (when we need it) since now we have access to a server object.
        InitializeRequestParameters(*wmsServer);
        // Create an instance of the Resource Service
        Ptr<MgResourceService> resourceService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

        // Retrieve the layer definitions of the requested layers.
        Ptr<MgWmsLayerDefinitions> layerDefs = MgHttpWmsGetCapabilities::GetLayerDefinitions(
            *resourceService, m_layerDefIds);

        // The WMS Server takes ownership of the layer definitions
        wmsServer->SetLayerDefs(layerDefs);
    }
}

STRING MgHttpWmsGetMap::GetRequestParameter(MgOgcWmsServer& oServer,CREFSTRING sParameterName)
{
    CPSZ psz = oServer.RequestParameter(sParameterName.c_str());
    return STRING(psz? psz : _(""));
}



