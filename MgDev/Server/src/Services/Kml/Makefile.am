AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerKmlService.vcproj ServerKmlService.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Oem/dbxml/xerces-c-src/src \
  -I../../../../Oem/DWFTK7.1/develop/global/src \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Manager \
  -I$(map_fdo_include) \
  -I$(map_fdo_include)/ExpressionEngine \
  -I../../../../Common/MdfModel \
  -I../../../../Common/MdfParser \
  -I../../../../Common/CoordinateSystem \
  -I../../../../Common/Stylization \
  -I../../../../Common/Renderers \
  -I../Feature \
  -I../Mapping

AM_CXXFLAGS = @CXXFLAGS@ -DDWFTK_USE_DWFCORE_ZLIB

lib_LTLIBRARIES = libMgServerKmlService.la

libMgServerKmlService_la_SOURCES = \
  ServerKmlServiceBuild.cpp

include_SOURCES = \
  KmlOperation.cpp \
  KmlOperationFactory.cpp \
  KmlServiceHandler.cpp \
  ServerKmlService.cpp \
  OpGetMapKml.cpp \
  OpGetLayerKml.cpp \
  OpGetFeaturesKml.cpp

noinst_HEADERS = $(include_SOURCES) \
  KmlOperationFactory.h \
  KmlOperation.h \
  KmlServiceHandler.h \
  ServerKmlDllExport.h \
  ServerKmlService.h \
  OpGetMapKml.h \
  OpGetLayerKml.h \
  OpGetFeaturesKml.h

libMgServerKmlService_la_LDFLAGS = -release $(PACKAGE_VERSION)
