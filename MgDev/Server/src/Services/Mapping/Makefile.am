AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerMappingService.vcproj ServerMappingService.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common/Stylization \
  -I../../../../Common/Renderers \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Manager \
  -I../../../../Common/MdfModel \
  -I../../../../Common/MdfParser \
  -I$(map_fdo_include) \
  -I$(map_fdo_include)/ExpressionEngine \
  -I../../../../Oem/dbxml/xerces-c-src/src \
  -I../../../../Common/CoordinateSystem \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../Feature \
  -I../../Gws/Include

lib_LTLIBRARIES = libMgServerMappingService.la

libMgServerMappingService_la_SOURCES = \
  ServerMappingServiceBuild.cpp

include_SOURCES = \
  LegendPlotUtil.cpp \
  MappingOperation.cpp \
  MappingOperationFactory.cpp \
  MappingServiceHandler.cpp \
  MappingUtil.cpp \
  OpGenerateLegendImage.cpp \
  OpGenerateLegendPlot.cpp \
  OpGenerateMap.cpp \
  OpGenerateMapUpdate.cpp \
  OpGenerateMultiPlot.cpp \
  OpGeneratePlot.cpp \
  OpQueryFeatures.cpp \
  OpQueryFeaturesWms.cpp \
  RSMgFeatureReader.cpp \
  RSMgRaster.cpp \
  RSMgSymbolManager.cpp \
  SEMgSymbolManager.cpp \
  ServerMappingService.cpp

noinst_HEADERS = $(include_SOURCES) \
  icons.h \
  LegendPlotUtil.h \
  MappingOperation.h \
  MappingOperationFactory.h \
  MappingServiceHandler.h \
  MappingUtil.h \
  OpGenerateLegendImage.h \
  OpGenerateLegendPlot.h \
  OpGenerateMap.h \
  OpGenerateMapUpdate.h \
  OpGenerateMultiPlot.h \
  OpGeneratePlot.h \
  OpQueryFeatures.h \
  OpQueryFeaturesWms.h \
  RSMgFeatureReader.h \
  RSMgInputStream.h \
  RSMgRaster.h \
  RSMgSymbolManager.h \
  SEMgSymbolManager.h \
  ServerMappingDllExport.h \
  ServerMappingService.h \
  ServerMappingServiceDefs.h

libMgServerMappingService_la_LIBADD =

libMgServerMappingService_la_LDFLAGS = -release $(PACKAGE_VERSION)
