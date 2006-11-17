AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerFactory.vcproj ServerFactory.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/MapGuideCommon \
  -I../../../../Common/PlatformBase \
  -I../../Common \
  -I../../Common/Manager \
  -I../../Services/Drawing \
  -I../../Services/Feature \
  -I../../Services/Kml \
  -I../../Services/Mapping \
  -I../../Services/Rendering \
  -I../../Services/Resource \
  -I../../Services/ServerAdmin \
  -I../../Services/Site \
  -I../../Services/Tile

lib_LTLIBRARIES = libMgServerFactory.la

libMgServerFactory_la_SOURCES = \
  ServerFactoryBuild.cpp

include_SOURCES = \
  ServerFactory.cpp

noinst_HEADERS = $(include_SOURCES) \
  ServerFactory.h \
  ServerFactoryDllExport.h

libMgServerFactory_la_LDFLAGS = -version-info 7:0:0
