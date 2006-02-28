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

// It is important to include the resource strings before the definition
// of the static data below, because the static data initialization uses
// them.  The resource strings must then be initialized first
#include "HttpResourceStrings.cpp"

#include "HttpApplyResourcePackage.cpp"
#include "HttpChangeResourceOwner.cpp"
#include "HttpClearTileCache.cpp"
#include "HttpCopyResource.cpp"
#include "HttpCreateRepository.cpp"
#include "HttpCreateSession.cpp"
#include "HttpDeleteRepository.cpp"
#include "HttpDeleteResource.cpp"
#include "HttpDeleteResourceData.cpp"
#include "HttpDescribeDrawing.cpp"
#include "HttpDescribeSchema.cpp"
#include "HttpEnumerateDataStores.cpp"
#include "HttpEnumerateDrawingLayers.cpp"
#include "HttpEnumerateDrawingSectionResources.cpp"
#include "HttpEnumerateDrawingSections.cpp"
#include "HttpEnumerateGroups.cpp"
#include "HttpEnumerateRepositories.cpp"
#include "HttpEnumerateResourceData.cpp"
#include "HttpEnumerateResourceReferences.cpp"
#include "HttpEnumerateResources.cpp"
#include "HttpEnumerateUsers.cpp"
#include "HttpExecuteSqlQuery.cpp"
#include "HttpGetCapabilities.cpp"
#include "HttpGetConnectionPropertyValues.cpp"
#include "HttpGetDrawing.cpp"
#include "HttpGetDrawingCoordinateSpace.cpp"
#include "HttpGetDrawingLayer.cpp"
#include "HttpGetDrawingSection.cpp"
#include "HttpGetDrawingSectionResource.cpp"
#include "HttpGetDwfViewerPage.cpp"
#include "HttpGetDynamicMapOverlayImage.cpp"
#include "HttpGetFeatureProviders.cpp"
#include "HttpGetFeatureSetEnvelope.cpp"
#include "HttpGetHtmlViewerPage.cpp"
#include "HttpGetLegendImage.cpp"
#include "HttpGetLongTransactions.cpp"
#include "HttpGetMap.cpp"
#include "HttpGetMapImage.cpp"
#include "HttpGetMapLegendImage.cpp"
#include "HttpGetMapUpdate.cpp"
#include "HttpGetPlot.cpp"
#include "HttpGetRepositoryContent.cpp"
#include "HttpGetRepositoryHeader.cpp"
#include "HttpGetResourceContent.cpp"
#include "HttpGetResourceData.cpp"
#include "HttpGetResourceHeader.cpp"
#include "HttpGetSchemaMapping.cpp"
#include "HttpGetSiteInfo.cpp"
#include "HttpGetSiteVersion.cpp"
#include "HttpGetSpatialContexts.cpp"
#include "HttpGetTileImage.cpp"
#include "HttpGetVisibleMapExtent.cpp"
#include "HttpHeader.cpp"
#include "HttpInheritPermissionsFrom.cpp"
#include "HttpMoveResource.cpp"
#include "HttpPrimitiveValue.cpp"
#include "HttpQueryMapFeatures.cpp"
#include "HttpRenameResourceData.cpp"
#include "HttpRequest.cpp"
#include "HttpRequestMetadata.cpp"
#include "HttpRequestParam.cpp"
#include "HttpRequestResponseHandler.cpp"
#include "HttpResponse.cpp"
#include "HttpResult.cpp"
#include "HttpSelectFeatures.cpp"
#include "HttpSelectFeaturesSpatially.cpp"
#include "HttpSetResource.cpp"
#include "HttpSetResourceData.cpp"
#include "HttpTestConnection.cpp"
#include "HttpUpdateRepository.cpp"
#include "HttpUtil.cpp"
#include "HttpWmsGetCapabilities.cpp"
#include "HttpWmsGetMap.cpp"
#include "HttpWmsGetFeatureInfo.cpp"
#include "HttpWfsGetCapabilities.cpp"
#include "HttpWfsGetFeature.cpp"
#include "HttpWfsDescribeFeatureType.cpp"
#include "WmsMapUtil.cpp"


// This should be the root of all evil, OGC-speaking.
#include "OgcWmsServer.cpp"
#include "OgcWfsServer.cpp"

// And that means trucking along these files.
// (Damn Linux and its inability to efficiently compile files ... :-\)
#include "OgcServer.cpp"
#include "HttpRequestParameters.cpp"
#include "WmsLayerDefinitions.cpp"
#include "WmsFeatureInfo.cpp"
#include "WfsFeatureDefinitions.cpp"
#include "WfsFeatures.cpp"
#include "XmlParser.cpp"
#include "Dictionary.cpp"
#include "NameStringValueCollection.cpp"
#include "NameValueCollection.cpp"
#include "OgcWmsException.cpp"
#include "OgcWfsException.cpp"
#include "WfsGetFeatureParams.cpp"
