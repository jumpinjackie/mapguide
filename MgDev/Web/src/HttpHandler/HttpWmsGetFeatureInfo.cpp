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

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();

    // Get the WMS request version
    m_version = params->GetParameterValue(MgHttpResourceStrings::reqWmsVersion);

    // Get the requested layers
    m_queryLayers = params->GetParameterValue(MgHttpResourceStrings::reqWmsQueryLayers);

    // Get the requested styles
    m_infoFormat = params->GetParameterValue(MgHttpResourceStrings::reqWmsInfoFormat);

    // Get i pixel coordinate and convert to integer
    string iParam = MgUtil::WideCharToMultiByte(params->GetParameterValue(MgHttpResourceStrings::reqWmsICoord));
    if(iParam.length() == 0)
    {
        // If no "I" parameter was specified, look for an "X" parameter
        iParam = MgUtil::WideCharToMultiByte(params->GetParameterValue(MgHttpResourceStrings::reqWmsXCoord));
    }
    m_iCoord = atoi(iParam.c_str());

    // Get j pixel coordinate and convert to integer
    string jParam = MgUtil::WideCharToMultiByte(params->GetParameterValue(MgHttpResourceStrings::reqWmsJCoord));
    if(jParam.length() == 0)
    {
        // If no "J" parameter was specified, look for a "Y" parameter
        jParam = MgUtil::WideCharToMultiByte(params->GetParameterValue(MgHttpResourceStrings::reqWmsYCoord));
    }
    m_jCoord = atoi(jParam.c_str());

    // Get the requested layers
    m_layers = params->GetParameterValue(MgHttpResourceStrings::reqWmsLayers);

    // Get the requested styles
    m_styles = params->GetParameterValue(MgHttpResourceStrings::reqWmsStyles);

    // Get the requested styles
    m_crs = params->GetParameterValue(MgHttpResourceStrings::reqWmsCrs);

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

    // Get the maximum number of features to return
    m_featureCount = 1;
    string featureCountParam = MgUtil::WideCharToMultiByte(params->GetParameterValue(MgHttpResourceStrings::reqWmsFeatureCount));
    if(featureCountParam.length() > 0)
    {
        m_featureCount = atoi(featureCountParam.c_str());
        if(m_featureCount < 1)
        {
            m_featureCount = 1;
        }
    }
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

    // We have to wrap the request parameters, since the outside
    // world is not case-insensitive (with respect to names,) but
    // WMS is.
    Ptr<MgHttpRequestParam> origReqParams = m_hRequest->GetRequestParam();
    MgHttpRequestParameters requestParams(origReqParams);

    // Create the WMS handler
    MgHttpResponseStream responseStream;
    MgOgcWmsServer wms(requestParams, responseStream);

    if(wms.ValidateRequest())
    {
        // Get an instance of the resource service
        Ptr<MgResourceService> resourceService = (MgResourceService*)CreateService(MgServiceType::ResourceService);

        // Get an MgMap object corresponding to the request params
        Ptr<MgMap> map = MgWmsMapUtil::GetMap(m_layers, m_bbox, m_crs, m_width, m_height, resourceService);

        // Create Rendering Service instance
        Ptr<MgRenderingService> service = (MgRenderingService*)(CreateService(MgServiceType::RenderingService));

        // Determine the query layers
        Ptr<MgStringCollection> queryLayers = MgStringCollection::ParseCollection(m_queryLayers, L",");

        // Get the selection geometry
        Ptr<MgGeometry> selectionGeometry = GetSelectionGeometry(map);

        // Call the C++ API
        Ptr<MgFeatureInformation> featureInfo = service->QueryFeatures(map, queryLayers, selectionGeometry,
            MgFeatureSpatialOperations::Intersects, m_featureCount);

        // Convert to XML format
        Ptr<MgByteReader> featureInfoXml = featureInfo->ToXml();

        // Convert to a string
        STRING xmlString = featureInfoXml->ToString();
        MgXmlParser xmlFeatureInfo(xmlString.c_str());
        MgWmsFeatureInfo wmsFeatureInfo(xmlFeatureInfo);

        // Execute the request
        wms.SetFeatureInfo(&wmsFeatureInfo);
        wms.RespondToRequest();
    }

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
