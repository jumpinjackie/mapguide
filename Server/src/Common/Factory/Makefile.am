AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerFactory.vcproj ServerFactory.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common/Service \
  -I../../../../Common/Service/Services \
  -I../../../../Common/Service/System \
  -I../../../../Common/Service/Geometry \
  -I../../../../Common/Service/MapLayer \
  -I../../Common \
  -I../../Common/Manager \
  -I../../Services/Drawing \
  -I../../Services/Feature \
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
