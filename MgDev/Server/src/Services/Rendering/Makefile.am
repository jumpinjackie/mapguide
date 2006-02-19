AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerRenderingService.vcproj ServerRenderingService.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common/Service \
  -I../../../../Common/Service/Services \
  -I../../../../Common/Service/System \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Manager \
  -I$(map_fdo_include) \
  -I../../../../Common/MdfModel \
  -I../../../../Common/Stylization \
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
  FeatureInfoRenderer.cpp \
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
  FeatureInfoRenderer.h \
  OpRenderMapLegend.h

libMgServerRenderingService_la_LDFLAGS = -version-info 7:0:0
