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
#include "HttpWmsGetMap.h"
#include "WmsMapUtil.h"

#include "OgcFramework.h"
#include "OgcWmsServer.h"

#include "ImageFormats.h"

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

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get the WMS request version
    m_version = params->GetParameterValue(MgHttpResourceStrings::reqWmsVersion);

    // Get the requested layers
    m_layers = params->GetParameterValue(MgHttpResourceStrings::reqWmsLayers);

    // Get the requested styles
    m_styles = params->GetParameterValue(MgHttpResourceStrings::reqWmsStyles);

    // Get the requested CRS (Ver 1.3) or SRS (Ver 1.1) value
    m_crs = params->GetParameterValue(MgHttpResourceStrings::reqWmsCrs);
    if(m_crs.length() == 0)
    {
        m_crs = params->GetParameterValue(MgHttpResourceStrings::reqWmsSrs);
    }

    // Get the requested styles
    m_bbox = params->GetParameterValue(MgHttpResourceStrings::reqWmsBbox);

    // Get width and convert to integer
    string widthParam = MgUtil::WideCharToMultiByte(params->GetParameterValue(MgHttpResourceStrings::reqWmsWidth));
    m_width = atoi(widthParam.c_str());

    // Get height and convert to integer
    string heightParam = MgUtil::WideCharToMultiByte(params->GetParameterValue(MgHttpResourceStrings::reqWmsHeight));
    m_height = atoi(heightParam.c_str());

    // Get the requested format
    m_format = params->GetParameterValue(MgHttpResourceStrings::reqWmsFormat);

    /////////////////////////////////////////////////
    // Optional parameters
    /////////////////////////////////////////////////

    // Background transparency
    STRING transparent = params->GetParameterValue(MgHttpResourceStrings::reqWmsTransparent);
    if(transparent == L"TRUE")
    {
        m_transparent = true;
    }
    else
    {
        m_transparent = false;
    }

    // Background color
    m_bgColor = params->GetParameterValue(MgHttpResourceStrings::reqWmsBackgroundColor);
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

        // Get a map object corresponding to the request parameters
        Ptr<MgMap> map = MgWmsMapUtil::GetMap(m_layers, m_bbox, m_crs,
            m_width, m_height, resourceService);

        // Get the image format
        STRING format = MgImageFormats::Png;
        if(_wcsicmp(m_format.c_str(), L"image/JPEG") == 0)
        {
            format = MgImageFormats::Jpeg;
        }
        else if(_wcsicmp(m_format.c_str(), L"image/GIF") == 0)
        {
            format = MgImageFormats::Gif;
        }
        else if(_wcsicmp(m_format.c_str(), L"image/TIFF") == 0)
        {
            format = MgImageFormats::Tiff;
        }

        // Render the map
        Ptr<MgRenderingService> renderingService = (MgRenderingService*)CreateService(MgServiceType::RenderingService);
        Ptr<MgSelection> selection;
        Ptr<MgByteReader> mapImage = renderingService->RenderMap(map, selection, extents, m_width, m_height, bkColor, format);

        // Set the result
        hResult->SetResultObject(mapImage, mapImage->GetMimeType());
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
        // Create an instance of the Resource Service
        Ptr<MgResourceService> resourceService = (MgResourceService*)(CreateService(MgServiceType::ResourceService));

        // Retrieve layer defs from the server
        Ptr<MgWmsLayerDefinitions> layerDefs = MgHttpWmsGetCapabilities::GetLayerDefinitions(*resourceService);

        // The WMS Server takes ownership of the layer definitions
        wmsServer->SetLayerDefs(layerDefs);
    }
}

