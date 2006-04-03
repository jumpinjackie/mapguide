AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerBase.vcproj ServerBase.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common/Service \
  -I../../../../Common/Service/Services \
  -I../../../../Common/Service/System \
  -I../../Common/Manager \
  -I../../Services/Resource \
  -I../../Services/Site

lib_LTLIBRARIES = libMgServerBase.la

libMgServerBase_la_SOURCES = \
  ServerBaseBuild.cpp

include_SOURCES = \
  IServiceHandler.cpp \
  ServiceOperation.cpp

noinst_HEADERS = $(include_SOURCES) \
  IServiceHandler.h \
  ServerBaseDllExport.h \
  ServiceOperation.h

libMgServerBase_la_LDFLAGS = -version-info 7:0:0
