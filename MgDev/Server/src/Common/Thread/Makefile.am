AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerThread.vcproj ServerThread.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common/Service \
  -I../../../../Common/Service/Services \
  -I../../../../Common/Service/System \
  -I../../Services/Resource \
  -I../../Services/Site

lib_LTLIBRARIES = libMgServerThread.la

libMgServerThread_la_SOURCES = \
  ServerThreadBuild.cpp

include_SOURCES = \
  ThreadBase.cpp

noinst_HEADERS = $(include_SOURCES) \
  ThreadBase.h \
  ServerThreadDllExport.h

libMgServerThread_la_LDFLAGS = -version-info 7:0:0
