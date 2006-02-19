AUTOMAKE_OPTIONS = nostdinc
INCLUDES = -I../..

publisher_includedir = $(pkgincludedir)/publisher
publisher_include_HEADERS =  EmbeddedFont.h Image.h PublishedObject.h Publisher.h
noinst_LTLIBRARIES =  libdwftkpublisher.la
libdwftkpublisher_la_LDFLAGS = -avoid-version 
libdwftkpublisher_la_SOURCES =  EmbeddedFont.cpp PublishedObject.cpp
SUBDIRS =  impl model plot
libdwftkpublisher_la_LIBADD = $(top_builddir)/publisher/model/libdwftkpublishermodel.la $(top_builddir)/publisher/impl/libdwftkpublisherimpl.la $(top_builddir)/publisher/plot/libdwftkpublisherplot.la
