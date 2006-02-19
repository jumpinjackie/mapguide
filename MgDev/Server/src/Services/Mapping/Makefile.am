AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerMappingService.vcproj ServerMappingService.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common/Stylization \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Manager \
  -I../../../../Common/MdfModel \
  -I../../../../Common/MdfParser \
  -I$(map_fdo_include) \
  -I../../../../Oem/dbxml-2.2.13/xerces-c-src/src \
  -I../../../../Common/Service \
  -I$(top_builddir)/$(map_coordsys_include_path) \
  -I../../../../Common/Service/Services \
  -I../../../../Common/Service/System \
  -I../../../../Common/Service/MapLayer \
  -I../../../../Common/Service/Geometry \
  -I../../../../Common/Service/Util

lib_LTLIBRARIES = libMgServerMappingService.la

libMgServerMappingService_la_SOURCES = \
  ServerMappingServiceBuild.cpp

include_SOURCES = \
  MappingOperation.cpp \
  MappingOperationFactory.cpp \
  MappingServiceHandler.cpp \
  MgCSTrans.cpp \
  ServerMappingService.cpp \
  OpGeneratePlot.cpp \
  OpGenerateMap.cpp \
  OpGenerateMapUpdate.cpp \
  OpGenerateMultiPlot.cpp \
  OpGenerateLegendPlot.cpp \
  OpGenerateLegendImage.cpp \
  OpQueryFeatures.cpp \
  OpQueryFeaturesWms.cpp \
  RSMgFeatureReader.cpp \
  RSMgRaster.cpp \
  RSMgSymbolManager.cpp \
  QueryRenderer.cpp \
  LegendPlotUtil.cpp \
  StylizationUtil.cpp

noinst_HEADERS = $(include_SOURCES) \
  icons.h \
  MappingOperationFactory.h \
  MappingOperation.h \
  MappingServiceHandler.h \
  MgCSTrans.h \
  ServerMappingDllExport.h \
  ServerMappingService.h \
  ServerMappingServiceDefs.h \
  OpGeneratePlot.h \
  OpGenerateMap.h \
  OpGenerateMapUpdate.h \
  OpGenerateMultiPlot.h \
  OpGenerateLegendPlot.h \
  OpGenerateLegendImage.h \
  OpQueryFeatures.h \
  OpQueryFeaturesWms.h \
  RSMgFeatureReader.h \
  RSMgRaster.h \
  RSMgInputStream.h \
  RSMgSymbolManager.h \
  QueryRenderer.h \
  LegendPlotUtil.h \
  StylizationUtil.h

libMgServerMappingService_la_LIBADD =

libMgServerMappingService_la_LDFLAGS = -version-info 7:0:0
