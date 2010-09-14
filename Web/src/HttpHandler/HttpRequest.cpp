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

// Http Resource Service operations
#include "HttpUpdateRepository.h"
#include "HttpApplyResourcePackage.h"
#include "HttpResourceExists.h"
#include "HttpEnumerateResources.h"
#include "HttpEnumerateApplicationTemplates.h"
#include "HttpEnumerateApplicationWidgets.h"
#include "HttpEnumerateApplicationContainers.h"
#include "HttpSetResource.h"
#include "HttpDeleteResource.h"
#include "HttpMoveResource.h"
#include "HttpCopyResource.h"
#include "HttpGetResourceContent.h"
#include "HttpGetResourceHeader.h"
#include "HttpEnumerateResourceReferences.h"
#include "HttpChangeResourceOwner.h"
#include "HttpInheritPermissionsFrom.h"
#include "HttpEnumerateResourceData.h"
#include "HttpSetResourceData.h"
#include "HttpDeleteResourceData.h"
#include "HttpRenameResourceData.h"
#include "HttpGetResourceData.h"
#include "HttpEnumerateUsers.h"
#include "HttpEnumerateGroups.h"
#include "HttpEnumerateUnmanagedData.h"

// Http Drawing Service operations
#include "HttpGetDrawing.h"
#include "HttpGetDrawingLayer.h"
#include "HttpDescribeDrawing.h"
#include "HttpGetDrawingSection.h"
#include "HttpGetDrawingSectionResource.h"
#include "HttpEnumerateDrawingLayers.h"
#include "HttpEnumerateDrawingSections.h"
#include "HttpEnumerateDrawingSectionResources.h"
#include "HttpGetDrawingCoordinateSpace.h"

// Http Feature Service operations
#include "HttpGetFeatureProviders.h"
#include "HttpGetConnectionPropertyValues.h"
#include "HttpTestConnection.h"
#include "HttpGetCapabilities.h"
#include "HttpDescribeSchema.h"
#include "HttpSelectFeatures.h"
#include "HttpSelectFeaturesSpatially.h"
#include "HttpExecuteSqlQuery.h"
#include "HttpGetSpatialContexts.h"
#include "HttpGetLongTransactions.h"
#include "HttpEnumerateDataStores.h"
#include "HttpGetSchemas.h"
#include "HttpGetClasses.h"
#include "HttpGetClassDefinition.h"
#include "HttpGetIdentityProperties.h"
#include "HttpGetSchemaMapping.h"
#include "HttpGetFdoCacheInfo.h"

// Http Mapping Service operations
#include "HttpGetMap.h"
#include "HttpGetMapUpdate.h"
#include "HttpGetPlot.h"
#include "HttpGetLegendImage.h"
#include "HttpGetFeatureSetEnvelope.h"

// Http Rendering Service operations
#include "HttpGetDynamicMapOverlayImage.h"
#include "HttpGetMapImage.h"
#include "HttpGetMapLegendImage.h"
#include "HttpGetVisibleMapExtent.h"
#include "HttpQueryMapFeatures.h"

// Http Tile Service operations
#include "HttpGetTileImage.h"
#include "HttpClearTileCache.h"

// Http Kml Service operations
#include "HttpKmlGetMap.h"
#include "HttpKmlGetLayer.h"
#include "HttpKmlGetFeatures.h"

// Http Wms operations
#include "OgcDataAccessor.h"
#include "HttpWmsGetCapabilities.h"
#include "HttpWmsGetMap.h"
#include "HttpWmsGetFeatureInfo.h"

// Http Wfs operations
#include "WfsGetFeatureParams.h"
#include "HttpWfsGetCapabilities.h"
#include "HttpWfsGetFeature.h"
#include "HttpWfsDescribeFeatureType.h"

// Other operations
#include "HttpCreateSession.h"
#include "HttpGetSessionTimeout.h"
#include "HttpGetDwfViewerPage.h"
#include "HttpGetHtmlViewerPage.h"

// Coordinate System
#include "HttpCsConvertWktToCoordinateSystemCode.h"
#include "HttpCsConvertCoordinateSystemCodeToWkt.h"
#include "HttpCsConvertEpsgCodeToWkt.h"
#include "HttpCsConvertWktToEpsgCode.h"
#include "HttpCsEnumerateCategories.h"
#include "HttpCsEnumerateCoordinateSystems.h"
#include "HttpCsGetBaseLibrary.h"
#include "HttpCsIsValid.h"

#include <algorithm>
using namespace std;

/// static data
static map<STRING,  MgHttpRequestResponseHandler* (*)(MgHttpRequest*)> httpClassCreators;
typedef vector<bool (*)(MgHttpRequest*, MgHttpResponse&)> postHandlerVector;
static postHandlerVector httpPostHandlerCreators;

static bool InitializeStaticData();
static bool initStatic = InitializeStaticData();


/// <summary>
/// Constructor. Initialize member variables.
/// </summary>
MgHttpRequest::MgHttpRequest(CREFSTRING agentUri)
{
    m_header = new MgHttpHeader();
    m_requestMetadata = new MgHttpRequestMetadata();
    m_requestParam = new MgHttpRequestParam();
    m_agentUri = agentUri;
}

/// <summary>
/// Default constructor to shut Ptr<> up.
/// </summary>
MgHttpRequest::MgHttpRequest()
{
}

/// <summary>
/// Makes available the pointer to header class.
/// User will use header class instance to add
/// header information for a request.
/// </summary>
/// <returns>Pointer to MgHttpHeader class</returns>
MgHttpHeader* MgHttpRequest::GetHeader()
{
    return SAFE_ADDREF((MgHttpHeader*)m_header);
}

/// <summary>
/// Makes available the pointer to RequestParam class.
/// User will use this class instance to add
/// all parameters specified for a request.
/// </summary>
/// <returns>Pointer to MgHttpRequestParam class</returns>
MgHttpRequestParam* MgHttpRequest::GetRequestParam()
{
    return SAFE_ADDREF((MgHttpRequestParam*)m_requestParam);
}

/// <summary>
/// Makes available the pointer to RequestMetadata class.
/// User will use this class instance to add
/// all required request metadata.
/// </summary>
/// <returns>Pointer to MgHttpRequestMetadata class</returns>
MgHttpRequestMetadata* MgHttpRequest::GetRequestMetadata()
{
    //comment
    return SAFE_ADDREF((MgHttpRequestMetadata*)m_requestMetadata);
}

/// <summary>
/// This method executes the request and returns the response
/// object to the client.
/// </summary>
/// <returns>Pointer to MgHttpResponse object</returns>
MgHttpResponse* MgHttpRequest::Execute()
{
    Ptr<MgHttpResponse> hResponse;
    Ptr<MgHttpResult> result;

    MG_HTTP_HANDLER_TRY()

    hResponse = new MgHttpResponse();
    result = hResponse->GetResult();

    // Get operation request value
    STRING sParamValue = m_requestParam->GetParameterValue(MgHttpResourceStrings::reqOperation);
    if(sParamValue.length() == 0)
    {
        // Get the REQUEST parameter of a WMS or WFS request
        STRING sRequestValue = m_requestParam->GetParameterValue(MgHttpResourceStrings::reqWmsRequest);
        if(sRequestValue.length() != 0)
        {
            STRING sServiceValue = m_requestParam->GetParameterValue(MgHttpResourceStrings::reqWmsService);
            if(sServiceValue.length() != 0)
            {
                sParamValue = sServiceValue;
            }
            else
            {
                sParamValue = L"WFS";
            }
            sParamValue.append(L".");
            sParamValue.append(sRequestValue);
        }
        else
        {
            // Error handling for OGC certification.
            // MapGuide should generate an WFS exception while receiveing following request:
            // http://locahost/mapguide/mapagent/mapagent.fcgi??request~GetCapabilities!service~WFS!version~1.1.0
            Ptr<MgStringCollection> parameterNames = m_requestParam->GetParameterNames();
            for(int i = 0; i < parameterNames->GetCount(); i++)
            {
                STRING parameterName = parameterNames->GetItem(i);
                if(parameterName.find(L"WFS") != STRING::npos)
                {
                    sParamValue = L"WFS";
                    sParamValue.append(L".");
                    sParamValue.append(L"GETCAPABILITIES");
                }
            }
        }
    }
    if(sParamValue.length() > 0)
    {
        transform(sParamValue.begin(), sParamValue.end(), sParamValue.begin(), ::toupper);

        // Get handler to requested function
        Ptr<MgHttpRequestResponseHandler> rrHandler = CreateRequestResponseHandler(sParamValue, result);

        // Execute request
        if(rrHandler != NULL)
            rrHandler->Execute(*hResponse);
    }
    else
    {
        // Loop through the available POST request handlers to see if any of them can
        // process this request
        for(postHandlerVector::iterator iter = httpPostHandlerCreators.begin(); iter != httpPostHandlerCreators.end(); iter++)
        {
            bool (*processFunc)(MgHttpRequest*, MgHttpResponse&) = *iter;
            if(processFunc != NULL)
            {
                if((*processFunc)(this, *hResponse))
                {
                    break;
                }
            }
        }
    }

    MG_HTTP_HANDLER_CATCH(L"MgHttpRequest.Execute")

    return SAFE_ADDREF((MgHttpResponse*)hResponse);
}

/// <summary>
/// Get the Uri of the agent which served this request
/// </summary>
STRING MgHttpRequest::GetAgentUri()
{
    return m_agentUri;
}

/// <summary>
/// This method returns a RequestResponseHandler based on the Operation
/// name
/// <param name="OperationName">Input
/// Name of the operation to construct handler for
/// </param>
/// </summary>
/// <returns>Pointer to MgHttpRequestResponseHandler object</returns>
MgHttpRequestResponseHandler* MgHttpRequest::CreateRequestResponseHandler(CREFSTRING OperationName, MgHttpResult* result)
{
    Ptr<MgHttpRequestResponseHandler> returnHandler;

    MG_HTTP_HANDLER_TRY()

    //find in the map the creator for the http mapping class
    MgHttpRequestResponseHandler* (*createFunc)(MgHttpRequest*) = httpClassCreators[OperationName];
    if(createFunc == NULL)
        throw new MgInvalidOperationException(L"MgHttpRequest::CreateRequestResponseHandler", __LINE__, __WFILE__, NULL, L"", NULL);

    //instantiate the http class
    returnHandler = (*createFunc)(this);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX2(L"MgHttpRequest::CreateRequestResponseHandler", result, this);

    return SAFE_ADDREF((MgHttpRequestResponseHandler*)returnHandler);
}

void MgHttpRequest::Dispose()
{
    delete this;
}

INT32 MgHttpRequest::GetClassId()
{
    return m_cls_id;
}

MgHttpRequest::~MgHttpRequest()
{
}

bool InitializeStaticData()
{
    //declare each Http class creator, associate the operation name with it
    //
    httpClassCreators[MgHttpResourceStrings::opGetMap] = MgHttpGetMap::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetMapUpdate] = MgHttpGetMapUpdate::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetDrawing] = MgHttpGetDrawing::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetDrawingLayer] = MgHttpGetDrawingLayer::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opDescribeDrawing] = MgHttpDescribeDrawing::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateDrawingLayers] = MgHttpEnumerateDrawingLayers::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetDrawingSection] = MgHttpGetDrawingSection::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetDrawingSectionResource] = MgHttpGetDrawingSectionResource::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateDrawingSections] = MgHttpEnumerateDrawingSections::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateDrawingSectionResources] = MgHttpEnumerateDrawingSectionResources::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetPlot] = MgHttpGetPlot::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opSetResource] = MgHttpSetResource::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opDeleteResource] = MgHttpDeleteResource::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opResourceExists] = MgHttpResourceExists::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateResources] = MgHttpEnumerateResources::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetResourceContent] = MgHttpGetResourceContent::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetResourceHeader] = MgHttpGetResourceHeader::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opMoveResource] = MgHttpMoveResource::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opCopyResource] = MgHttpCopyResource::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opSetResourceData] = MgHttpSetResourceData::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opDeleteResourceData] = MgHttpDeleteResourceData::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateResourceData] = MgHttpEnumerateResourceData::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetResourceData] = MgHttpGetResourceData::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opRenameResourceData] = MgHttpRenameResourceData::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opUpdateRepository] = MgHttpUpdateRepository::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateResourceReferences] = MgHttpEnumerateResourceReferences::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opApplyResourcePackage] = MgHttpApplyResourcePackage::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opChangeResourceOwner] = MgHttpChangeResourceOwner::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opInheritPermissionsFrom] = MgHttpInheritPermissionsFrom::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetFeatureProviders] = MgHttpGetFeatureProviders::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetConnectionPropertyValues] = MgHttpGetConnectionPropertyValues::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opTestConnection] = MgHttpTestConnection::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetCapabilities] = MgHttpGetCapabilities::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opDescribeSchema] = MgHttpDescribeSchema::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opSelectFeatures] = MgHttpSelectFeatures::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opSelectFeaturesSpatially] = MgHttpSelectFeaturesSpatially::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opExecuteSqlQuery] = MgHttpExecuteSqlQuery::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetSpatialContexts] = MgHttpGetSpatialContexts::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetLongTransactions] = MgHttpGetLongTransactions::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateUsers] = MgHttpEnumerateUsers::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateGroups] = MgHttpEnumerateGroups::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetSiteVersion] = MgHttpGetSiteVersion::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetSiteInfo] = MgHttpGetSiteInfo::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetSiteStatus] = MgHttpGetSiteStatus::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetDrawingCoordinateSpace] = MgHttpGetDrawingCoordinateSpace::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opCreateSession] = MgHttpCreateSession::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetSessionTimeout] = MgHttpGetSessionTimeout::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetDwfViewerPage] = MgHttpGetDwfViewerPage::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetHtmlViewerPage] = MgHttpGetHtmlViewerPage::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetLegendImage] = MgHttpGetLegendImage::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetFeatureSetEnvelope] = MgHttpGetFeatureSetEnvelope::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetDynamicMapOverlayImage] = MgHttpGetDynamicMapOverlayImage::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetMapImage] = MgHttpGetMapImage::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetMapLegendImage] = MgHttpGetMapLegendImage::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetVisibleMapExtent] = MgHttpGetVisibleMapExtent::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opQueryMapFeatures] = MgHttpQueryMapFeatures::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetTileImage] = MgHttpGetTileImage::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opClearTileCache] = MgHttpClearTileCache::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opWmsGetCapabilities] = MgHttpWmsGetCapabilities::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opWmsGetMap] = MgHttpWmsGetMap::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opWmsCapabilities] = MgHttpWmsGetCapabilities::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opWmsMap] = MgHttpWmsGetMap::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opWmsGetFeatureInfo] = MgHttpWmsGetFeatureInfo::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opWfsGetCapabilities] = MgHttpWfsGetCapabilities::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opWfsGetFeature] = MgHttpWfsGetFeature::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opWfsDescribeFeatureType] = MgHttpWfsDescribeFeatureType::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateDataStores] = MgHttpEnumerateDataStores::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetSchemaMapping] = MgHttpGetSchemaMapping::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetSchemas] = MgHttpGetSchemas::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetClasses] = MgHttpGetClasses::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetClassDefinition] = MgHttpGetClassDefinition::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetIdentityProperties] = MgHttpGetIdentityProperties::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetMapKml] = MgHttpKmlGetMap::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetLayerKml] = MgHttpKmlGetLayer::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetFeaturesKml] = MgHttpKmlGetFeatures::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opCsConvertWktToCoordinateSystemCode] = MgHttpCsConvertWktToCoordinateSystemCode::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opCsConvertCoordinateSystemCodeToWkt] = MgHttpCsConvertCoordinateSystemCodeToWkt::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opCsConvertEpsgCodeToWkt] = MgHttpCsConvertEpsgCodeToWkt::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opCsConvertWktToEpsgCode] = MgHttpCsConvertWktToEpsgCode::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opCsEnumerateCategories] = MgHttpCsEnumerateCategories::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opCsEnumerateCoordinateSystems] = MgHttpCsEnumerateCoordinateSystems::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opCsGetBaseLibrary] = MgHttpCsGetBaseLibrary::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opCsIsValid] = MgHttpCsIsValid::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateUnmanagedData] = MgHttpEnumerateUnmanagedData::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetFdoCacheInfo] = MgHttpGetFdoCacheInfo::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateApplicationTemplates] = MgHttpEnumerateApplicationTemplates::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateApplicationWidgets] = MgHttpEnumerateApplicationWidgets::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opEnumerateApplicationContainers] = MgHttpEnumerateApplicationContainers::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetDefaultTileSizeX] = MgHttpGetDefaultTileSizeX::CreateObject;
    httpClassCreators[MgHttpResourceStrings::opGetDefaultTileSizeY] = MgHttpGetDefaultTileSizeY::CreateObject;

    httpPostHandlerCreators.push_back(MgHttpWfsGetCapabilities::ProcessPostRequest);
    httpPostHandlerCreators.push_back(MgHttpWfsDescribeFeatureType::ProcessPostRequest);
    httpPostHandlerCreators.push_back(MgHttpWfsGetFeature::ProcessPostRequest);

    return true;
}
