AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = HttpHandler.rc HttpHandler.vcproj

prefix=@prefix@/webserverextensions

INCLUDES    = -I../../../Common/Service \
              -I../../../Common/Service/Services \
              -I../../../Common/Service/Geometry \
              -I../../../Common/Service/MapLayer \
              -I../../../Common/Service/System \
              -I../../../Oem/ACE/ACE_wrappers \
              -I../../../Common/Service/Controller

lib_LTLIBRARIES = libMgHttpHandler.la

libMgHttpHandler_la_SOURCES = HttpHandlerBuild.cpp

include_SOURCES = \
  Dictionary.cpp \
  HttpApplyResourcePackage.cpp \
  HttpChangeResourceOwner.cpp \
  HttpClearTileCache.cpp \
  HttpCopyResource.cpp \
  HttpCreateRepository.cpp \
  HttpCreateSession.cpp \
  HttpDeleteRepository.cpp \
  HttpDeleteResource.cpp \
  HttpDeleteResourceData.cpp \
  HttpDescribeDrawing.cpp \
  HttpDescribeSchema.cpp \
  HttpEnumerateDataStores.cpp \
  HttpEnumerateDrawingLayers.cpp \
  HttpEnumerateDrawingSectionResources.cpp \
  HttpEnumerateDrawingSections.cpp \
  HttpEnumerateGroups.cpp \
  HttpEnumerateRepositories.cpp \
  HttpEnumerateResourceData.cpp \
  HttpEnumerateResourceReferences.cpp \
  HttpEnumerateResources.cpp \
  HttpEnumerateUsers.cpp \
  HttpExecuteSqlQuery.cpp \
  HttpGetCapabilities.cpp \
  HttpGetConnectionPropertyValues.cpp \
  HttpGetDrawing.cpp \
  HttpGetDrawingCoordinateSpace.cpp \
  HttpGetDrawingLayer.cpp \
  HttpGetDrawingSection.cpp \
  HttpGetDrawingSectionResource.cpp \
  HttpGetDwfViewerPage.cpp \
  HttpGetDynamicMapOverlayImage.cpp \
  HttpGetFeatureProviders.cpp \
  HttpGetFeatureSetEnvelope.cpp \
  HttpGetHtmlViewerPage.cpp \
  HttpGetLegendImage.cpp \
  HttpGetLongTransactions.cpp \
  HttpGetMap.cpp \
  HttpGetMapLegendImage.cpp \
  HttpGetMapImage.cpp \
  HttpGetMapUpdate.cpp \
  HttpGetPlot.cpp \
  HttpGetRepositoryContent.cpp \
  HttpGetRepositoryHeader.cpp \
  HttpGetResourceContent.cpp \
  HttpGetResourceData.cpp \
  HttpGetResourceHeader.cpp \
  HttpGetSchemaMapping.cpp \
  HttpGetSiteInfo.cpp \
  HttpGetSiteVersion.cpp \
  HttpGetSpatialContexts.cpp \
  HttpGetTileImage.cpp \
  HttpGetVisibleMapExtent.cpp \
  HttpHeader.cpp \
  HttpInheritPermissionsFrom.cpp \
  HttpMoveResource.cpp \
  HttpPrimitiveValue.cpp \
  HttpQueryMapFeatures.cpp \
  HttpRenameResourceData.cpp \
  HttpRequest.cpp \
  HttpRequestMetadata.cpp \
  HttpRequestParam.cpp \
  HttpRequestParameters.cpp \
  HttpRequestResponseHandler.cpp \
  HttpResourceStrings.cpp \
  HttpResponse.cpp \
  HttpResult.cpp \
  HttpSelectFeatures.cpp \
  HttpSelectFeaturesSpatially.cpp \
  HttpSetResource.cpp \
  HttpSetResourceData.cpp \
  HttpTestConnection.cpp \
  HttpUpdateRepository.cpp \
  HttpUtil.cpp \
  HttpWfsDescribeFeatureType.cpp \
  HttpWfsGetCapabilities.cpp \
  HttpWfsGetFeature.cpp \
  HttpWmsGetCapabilities.cpp \
  HttpWmsGetFeatureInfo.cpp \
  HttpWmsGetMap.cpp \
  NameStringValueCollection.cpp \
  NameValueCollection.cpp \
  OgcServer.cpp \
  OgcWfsException.cpp \
  OgcWfsServer.cpp \
  OgcWmsException.cpp \
  OgcWmsServer.cpp \
  WfsFeatureDefinitions.cpp \
  WfsFeatures.cpp \
  WfsGetFeatureParams.cpp \
  WmsFeatureInfo.cpp \
  WmsLayerDefinitions.cpp \
  WmsMapUtil.cpp \
  XmlParser.cpp

noinst_HEADERS = $(include_SOURCES) \
  Dictionary.h \
  HttpApplyResourcePackage.h \
  HttpChangeResourceOwner.h \
  HttpClearTileCache.h \
  HttpCopyResource.h \
  HttpCreateRepository.h \
  HttpCreateSession.h \
  HttpDeleteRepository.h \
  HttpDeleteResource.h \
  HttpDeleteResourceData.h \
  HttpDescribeDrawing.h \
  HttpDescribeSchema.h \
  HttpEnumerateDataStores.h \
  HttpEnumerateDrawingLayers.h \
  HttpEnumerateDrawingSectionResources.h \
  HttpEnumerateDrawingSections.h \
  HttpEnumerateGroups.h \
  HttpEnumerateRepositories.h \
  HttpEnumerateResourceData.h \
  HttpEnumerateResourceReferences.h \
  HttpEnumerateResources.h \
  HttpEnumerateUsers.h \
  HttpExecuteSqlQuery.h \
  HttpGetCapabilities.h \
  HttpGetConnectionPropertyValues.h \
  HttpGetDrawing.h \
  HttpGetDrawingCoordinateSpace.h \
  HttpGetDrawingLayer.h \
  HttpGetDrawingSection.h \
  HttpGetDrawingSectionResource.h \
  HttpGetDwfViewerPage.h \
  HttpGetDynamicMapOverlayImage.h \
  HttpGetFeatureProviders.h \
  HttpGetFeatureSetEnvelope.h \
  HttpGetHtmlViewerPage.h \
  HttpGetLegendImage.h \
  HttpGetLongTransactions.h \
  HttpGetMap.h \
  HttpGetMapLegendImage.h \
  HttpGetMapImage.h \
  HttpGetMapUpdate.h \
  HttpGetPlot.h \
  HttpGetRepositoryContent.h \
  HttpGetRepositoryHeader.h \
  HttpGetResourceContent.h \
  HttpGetResourceData.h \
  HttpGetResourceHeader.h \
  HttpGetSchemaMapping.h \
  HttpGetSiteInfo.h \
  HttpGetSiteVersion.h \
  HttpGetSpatialContexts.h \
  HttpGetTileImage.h \
  HttpGetVisibleMapExtent.h \
  HttpHandler.h \
  HttpHandlerApiDllExport.h \
  HttpHeader.h \
  HttpInheritPermissionsFrom.h \
  HttpMoveResource.h \
  HttpPrimitiveValue.h \
  HttpPrimitiveValueTypes.h \
  HttpQueryMapFeatures.h \
  HttpRenameResourceData.h \
  HttpRequest.h \
  HttpRequestMetadata.h \
  HttpRequestParam.h \
  HttpRequestParameters.h \
  HttpRequestResponseHandler.h \
  HttpResourceStrings.h \
  HttpResponse.h \
  HttpResult.h \
  HttpSelectFeatures.h \
  HttpSelectFeaturesSpatially.h \
  HttpSetResource.h \
  HttpSetResourceData.h \
  HttpStatusCodes.h \
  HttpTestConnection.h \
  HttpUpdateRepository.h \
  HttpUtil.h \
  HttpWfsDescribeFeatureType.h \
  HttpWfsGetCapabilities.h \
  HttpWfsGetFeature.h \
  HttpWmsGetCapabilities.h \
  HttpWmsGetFeatureInfo.h \
  HttpWmsGetMap.h \
  NameStringValueCollection.h \
  NameValueCollection.h \
  OgcException.h \
  OgcFramework.h \
  OgcServer.h \
  OgcWfsException.h \
  OgcWfsServer.h \
  OgcWmsException.h \
  OgcWmsServer.h \
  ResponseStream.h \
  Stream.h \
  StringStream.h \
  WfsFeatureDefinitions.h \
  WfsFeatures.h \
  WfsGetFeatureParams.h \
  WmsFeatureInfo.h \
  WmsLayerDefinitions.h \
  WmsMapUtil.h \
  XmlElementEmitter.h \
  XmlParser.h

libMgHttpHandler_la_LIBADD = -lMgCommon

libMgHttpHandler_la_LDFLAGS = -version-info 7:0:0 \
    -L../../../Common/Service
