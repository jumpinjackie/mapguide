AUTOMAKE_OPTIONS = 
INCLUDES = -I..
SUBDIRS = package 
pkginclude_HEADERS = EMapToolkit.h EMapVersion.h

lib_LTLIBRARIES =  libdwfemap.la

libdwfemap_la_LDFLAGS = -release 1.0.0
libdwfemap_la_SOURCES = EMapToolkit.cpp

libdwfemap_la_LIBADD =  $(top_builddir)/package/libemappackage.la 
