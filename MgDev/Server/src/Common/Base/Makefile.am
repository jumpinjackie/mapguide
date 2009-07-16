AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerBase.vcproj ServerBase.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common\MdfModel \  
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/MapGuideCommon \
  -I../../../../Common/PlatformBase \
  -I../../Common/Manager \
  -I../../Services/Resource \
  -I../../Services/Site

lib_LTLIBRARIES = libMgServerBase.la

libMgServerBase_la_SOURCES = \
  ServerBaseBuild.cpp

include_SOURCES = \
  ClientHandler.cpp \
  IServiceHandler.cpp \
  ServerStreamData.cpp \
  ServiceOperation.cpp

noinst_HEADERS = $(include_SOURCES) \
  ClientHandler.h \
  IServiceHandler.h \
  ServerBaseDllExport.h \
  ServerStreamData.h \
  ServiceOperation.h

libMgServerBase_la_LDFLAGS = -version-info 7:0:0
