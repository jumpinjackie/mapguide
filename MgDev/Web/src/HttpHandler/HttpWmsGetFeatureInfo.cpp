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

#include "HttpHandler.h"
#include "HttpWmsGetFeatureInfo.h"

#include "OgcFramework.h"
#include "XmlParser.h"
#include "OgcWmsServer.h"
#include "WmsFeatureInfo.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpWmsGetFeatureInfo)

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
MgHttpWmsGetFeatureInfo::MgHttpWmsGetFeatureInfo(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);
}

void MgHttpWmsGetFeatureInfo::InitializeRequestParameters(MgOgcWmsServer& oServer)
{
    // Get the WMS request version
    m_version = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsVersion);

    // Get the requested layers
    m_queryLayers = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsQueryLayers);

    // Get the requested styles
    m_infoFormat = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsInfoFormat);

    // Get i pixel coordinate and convert to integer
    m_iCoord = GetRequestParameterInt32(oServer,MgHttpResourceStrings::reqWmsICoord);
    if(m_iCoord == 0)
        m_iCoord = GetRequestParameterInt32(oServer,MgHttpResourceStrings::reqWmsXCoord);

    // Get j pixel coordinate and convert to integer
    m_jCoord = GetRequestParameterInt32(oServer,MgHttpResourceStrings::reqWmsJCoord);
    if(m_jCoord == 0)
        m_jCoord = GetRequestParameterInt32(oServer,MgHttpResourceStrings::reqWmsYCoord);

    // Get the requested layers
    m_layerDefIds = MgWmsMapUtil::GetLayerDefinitionIds(
        GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsLayers));

    // Get the requested styles
    m_styles = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsStyles);

    // Get the requested reference system
    m_crs = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsCrs);
    if(m_crs.length() == 0)
    {
        m_crs = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsSrs);
    }

    // Get the requested bounds
    m_bbox = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsBbox);

    // Get width and convert to integer
    m_width = GetRequestParameterInt32(oServer,MgHttpResourceStrings::reqWmsWidth);

    // Get height and convert to integer
    m_height = GetRequestParameterInt32(oServer,MgHttpResourceStrings::reqWmsHeight);

    // Get the requested format
    m_format = GetRequestParameter(oServer,MgHttpResourceStrings::reqWmsFormat);

    // Get the maximum number of features to return
    m_featureCount = GetRequestParameterInt32(oServer,MgHttpResourceStrings::reqWmsFeatureCount);
    if(m_featureCount < 1)
        m_featureCount = 1;
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpWmsGetFeatureInfo::Execute(MgHttpResponse& hResponse)
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

    // Process the request
    wms.ProcessRequest(this);

    // Obtain the response byte reader
    Ptr<MgByteReader> featInfoResponse = responseStream.Stream().GetReader();

    // Set the result
    hResult->SetResultObject(featInfoResponse, featInfoResponse->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpWmsGetFeatureInfo.Execute")
}

MgGeometry* MgHttpWmsGetFeatureInfo::GetSelectionGeometry(MgMap* map)
{
    MgGeometry* selectionGeometry = NULL;

    // Get the lower left and upper right map coords in MCS
    Ptr<MgEnvelope> mcsExtent = map->GetMapExtent();
    Ptr<MgCoordinate> mcsLowerLeft = mcsExtent->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> mcsUpperRight = mcsExtent->GetUpperRightCoordinate();

    // Convert the pixel coords to MCS coords
    double mcsX = mcsLowerLeft->GetX() + (double)m_iCoord * mcsExtent->GetWidth() / map->GetDisplayWidth();
    double mcsY = mcsUpperRight->GetY() - (double)m_jCoord * mcsExtent->GetHeight() / map->GetDisplayHeight();

    // Create a new MgPoint representing these coordinates
    Ptr<MgCoordinateXY> selectionCoord = new MgCoordinateXY(mcsX, mcsY);
    selectionGeometry = new MgPoint(selectionCoord);

    return selectionGeometry;
}

void MgHttpWmsGetFeatureInfo::AcquireValidationData(MgOgcServer* ogcServer)
{
    MgOgcWmsServer* wmsServer = (MgOgcWmsServer*)ogcServer;
    if(wmsServer != NULL)
    {
        // The initialization that used to happen in the ctor is deferred until now
        // (when we need it) since now we have access to a server object.
        InitializeRequestParameters(*wmsServer);

        // Get an instance of the resource service
        Ptr<MgResourceService> resourceService = (MgResourceService*)CreateService(MgServiceType::ResourceService);

        // Retrieve the layer definitions of the requested layers.
        Ptr<MgWmsLayerDefinitions> layerDefs = MgHttpWmsGetCapabilities::GetLayerDefinitions(
            *resourceService, m_layerDefIds);

        // WMS Server takes ownership of layer definitions
        wmsServer->SetLayerDefs(layerDefs);
    }
}

void MgHttpWmsGetFeatureInfo::AcquireResponseData(MgOgcServer* ogcServer)
{
    MgOgcWmsServer* wmsServer = (MgOgcWmsServer*)ogcServer;
    if(wmsServer != NULL)
    {
        // Get an instance of the resource service
        Ptr<MgResourceService> resourceService = (MgResourceService*)CreateService(MgServiceType::ResourceService);

        // Get an MgMap object corresponding to the request params
        Ptr<MgMap> map = MgWmsMapUtil::GetMap(*wmsServer, m_layerDefIds, m_bbox, m_crs, m_width, m_height, resourceService);

        // Create Rendering Service instance
        Ptr<MgRenderingService> service = (MgRenderingService*)(CreateService(MgServiceType::RenderingService));

        // Determine the query layers
        Ptr<MgStringCollection> queryLayers = MgStringCollection::ParseCollection(m_queryLayers, L",");

        // Get the selection geometry
        Ptr<MgGeometry> selectionGeometry = GetSelectionGeometry(map);

        // Call the C++ API
        Ptr<MgBatchPropertyCollection> propertyCollection = service->QueryFeatureProperties(map, queryLayers, selectionGeometry,
            MgFeatureSpatialOperations::Intersects, L"", m_featureCount, 1 /*Any visible features*/);

        // Create the object to store the feature info
        Ptr<MgWmsFeatureInfo> wmsFeatureInfo = new MgWmsFeatureInfo(propertyCollection);

        // The WMS Server takes ownership of the feature info
        wmsServer->SetFeatureInfo(wmsFeatureInfo);
    }
}

STRING MgHttpWmsGetFeatureInfo::GetRequestParameter(MgOgcWmsServer& oServer,CREFSTRING sParameterName)
{
    CPSZ psz = oServer.RequestParameter(sParameterName.c_str());
    return STRING(psz? psz : _(""));
}

INT32 MgHttpWmsGetFeatureInfo::GetRequestParameterInt32(MgOgcWmsServer& oServer,CREFSTRING sParameterName)
{
    STRING sNum = GetRequestParameter(oServer,sParameterName);
    return MgUtil::StringToInt32(sNum);
}


