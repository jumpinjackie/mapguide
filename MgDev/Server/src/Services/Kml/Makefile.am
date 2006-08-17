AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerKmlService.vcproj ServerKmlService.rc

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

libMgServerKmlService_la_LDFLAGS = -version-info 7:0:0
