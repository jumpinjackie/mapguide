AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerProfilingService.vcproj ServerProfilingService.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Oem/dbxml/xerces-c-src/src \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Manager \
  -I../../../../Common/MdfModel \
  -I../../../../Common/MdfParser \
  -I../../../../Common/CoordinateSystem

AM_CXXFLAGS = @CXXFLAGS@ -DDWFTK_USE_DWFCORE_ZLIB

lib_LTLIBRARIES = libMgServerProfilingService.la

libMgServerProfilingService_la_SOURCES = \
  ServerProfilingServiceBuild.cpp

include_SOURCES = \
  ProfilingOperation.cpp \
  ProfilingOperationFactory.cpp \
  ProfilingServiceHandler.cpp \
  ServerProfilingService.cpp \
  OpProfileRenderMap.cpp \
  OpProfileRenderDynamicOverlay.cpp

noinst_HEADERS = $(include_SOURCES) \
  ProfilingOperationFactory.h \
  ProfilingOperation.h \
  ProfilingServiceHandler.h \
  ServerProfilingDllExport.h \
  ServerProfilingService.h \
  OpProfileRenderMap.h \
  OpProfileRenderDynamicOverlay.h

libMgServerProfilingService_la_LDFLAGS = -release $(PACKAGE_VERSION)
