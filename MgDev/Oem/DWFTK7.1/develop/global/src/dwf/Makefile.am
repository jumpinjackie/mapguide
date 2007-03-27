AUTOMAKE_OPTIONS = nostdinc
INCLUDES = -I..

pkginclude_HEADERS =  Toolkit.h Version.h

lib_LTLIBRARIES =  libdwftk.la

libdwftk_la_LDFLAGS = -release 7.1.1

libdwftk_la_SOURCES =  Toolkit.cpp

SUBDIRS = package expat whiptk w3dtk publisher

libdwftk_la_LIBADD = $(top_builddir)/whiptk/libdwfwhiptk.la $(top_builddir)/w3dtk/libdwfw3dtk.la $(top_builddir)/expat/libdwfexpat.la $(top_builddir)/package/libdwfpackage.la $(top_builddir)/publisher/libdwftkpublisher.la
