AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = GwsCommon.rc GwsCommon.vcproj

lib_LTLIBRARIES = libMgGwsCommon.la

libMgGwsCommon_la_SOURCES = \
  GwsCommonFactories.cpp \
  GwsCommonFdoUtils.cpp \
  GwsCsName.cpp \
  GwsDebugUtils.cpp \
  GwsException.cpp \
  GwsExtendedFeatureId.cpp \
  GwsFeatureId.cpp \
  GwsObjectImpl.cpp \
  GwsQualifiedName.cpp \
  GwsQualifiedNames.cpp \
  GwsResourceUtil.cpp \
  GwsXmlUtils.cpp \
  IGwsException.cpp

include_SOURCES = \
  GwsCommon.cpp

noinst_HEADERS = $(include_SOURCES) \
  stdafx.h \
  inc/GwsCommonImp.h \
  inc/GwsCommonUtils.h \
  inc/GwsException.h \
  inc/GwsFeatureIdSet.h \
  inc/GwsObject.h \
  inc/GwsObjectImpl.h \
  inc/GwsOperationHandler.h \
  inc/GwsResourceUtil.h \
  inc/GwsThreads.h \
  inc/GwsStatus.h

INCLUDES = \
  -I$(map_fdo_include) \
  -I../Include \
  -I../GwsResource \
  -I./inc \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry

libMgGwsCommon_la_LDFLAGS = -release $(PACKAGE_VERSION)

