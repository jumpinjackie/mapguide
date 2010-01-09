AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = GwsResource.vcproj GwsResource.rc

lib_LTLIBRARIES = libMgGwsResource.la

libMgGwsResource_la_SOURCES = \
  GwsResourceLinux.cpp

noinst_HEADERS = \
  GwsResource.h \
  GwsResourceLinux.h

INCLUDES = -I../INCLUDE

libMgGwsResource_la_LDFLAGS = -version-info 1:0:0
