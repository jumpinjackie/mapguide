AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerRenderingService.vcproj ServerRenderingService.rc

prefix=@prefix@/server

INCLUDES = \
  -I$(map_fdo_include) \
  -I$(map_fdo_include)/ExpressionEngine \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Oem/dbxml-2.3.10/xerces-c-src/src \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Manager \
  -I../../../../Common/MdfModel \
  -I../../../../Common/Stylization \
  -I../../../../Common/Renderers \
  -I../Feature \
  -I../Mapping

lib_LTLIBRARIES = libMgServerRenderingService.la

libMgServerRenderingService_la_SOURCES = \
  ServerRenderingServiceBuild.cpp

include_SOURCES = \
  RenderingOperation.cpp \
  RenderingOperationFactory.cpp \
  RenderingServiceHandler.cpp \
  ServerRenderingService.cpp \
  OpRenderDynamicOverlay.cpp \
  OpRenderMap.cpp \
  OpRenderTile.cpp \
  OpQueryFeatures.cpp \
  OpQueryFeatureProperties.cpp \
  FeatureInfoRenderer.cpp \
  FeaturePropRenderer.cpp \
  OpRenderMapLegend.cpp

noinst_HEADERS = $(include_SOURCES) \
  RenderingOperationFactory.h \
  RenderingOperation.h \
  RenderingServiceHandler.h \
  ServerRenderingDllExport.h \
  ServerRenderingService.h \
  OpRenderDynamicOverlay.h \
  OpRenderMap.h \
  OpRenderTile.h \
  OpQueryFeatures.h \
  OpQueryFeatureProperties.h \
  FeatureInfoRenderer.h \
  FeaturePropRenderer.h \
  OpRenderMapLegend.h

libMgServerRenderingService_la_LDFLAGS = -version-info 7:0:0
