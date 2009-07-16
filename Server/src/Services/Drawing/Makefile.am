AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerDrawingService.vcproj ServerDrawingService.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common\MdfModel \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../../../../Oem/DWFTK7.1/develop/global/src \
  -I../../../../Oem/DWFTK7.1/develop/global/src/dwf \
  -I../../../../Oem/dbxml-2.3.10/xerces-c-src/src \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Manager \
  -I../../Common/ServerUtil \
  -I../Resource

AM_CXXFLAGS = @CXXFLAGS@ -DDWFTK_BUILD_EXPAT -DDWFTK_USE_DWFCORE_ZLIB

lib_LTLIBRARIES = libMgServerDrawingService.la

MgServerDrawingServiceBuild.lo : Makefile

libMgServerDrawingService_la_SOURCES = \
  ServerDrawingServiceBuild.cpp

include_SOURCES = \
  DrawingOperation.cpp \
  DrawingOperationFactory.cpp \
  DrawingServiceHandler.cpp \
  OpDescribeDrawing.cpp \
  OpEnumerateLayers.cpp \
  OpEnumerateSections.cpp \
  OpEnumerateSectionResources.cpp \
  OpGetCoordinateSpace.cpp \
  OpGetDrawing.cpp \
  OpGetLayer.cpp \
  OpGetSection.cpp \
  OpGetSectionResource.cpp \
  ServerDrawingService.cpp \
  ServerDrawingServiceUtil.cpp

noinst_HEADERS = $(include_SOURCES) \
  DrawingOperation.h \
  DrawingOperationFactory.h \
  DrawingServiceHandler.h \
  OpDescribeDrawing.h \
  OpEnumerateLayers.h \
  OpEnumerateSections.h \
  OpEnumerateSectionResources.h \
  OpGetCoordinateSpace.h \
  OpGetDrawing.h \
  OpGetLayer.h \
  OpGetSection.h \
  OpGetSectionResource.h \
  ServerDrawingDllExport.h \
  ServerDrawingService.h \
  ServerDrawingServiceUtil.h \
  ServerDrawingServiceDefs.h \
  ../../../../Common/PlatformBase/Services/Service.h

libMgServerDrawingService_la_LIBADD = -lMgFoundation -lMgGeometry -lMgPlatformBase -lMgMapGuideCommon -lxerces-c

libMgServerDrawingService_la_LDFLAGS = -version-info 7:0:0 \
  -L../../../../Oem/dbxml-2.3.10/install/lib \
  -L../../../../Common/Foundation \
  -L../../../../Common/Geometry \
  -L../../../../Common/PlatformBase \
  -L../../../../Common/MapGuideCommon
