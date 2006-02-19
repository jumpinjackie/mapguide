AUTOMAKE_OPTIONS = nostdinc
INCLUDES = -I../../..

SUBDIRS = extensions

package_writer_includedir = $(pkgincludedir)/package/writer

package_writer_include_HEADERS =  DWFXMLSerializer.h PackageVersionExtension.h PackageWriter.h

noinst_LTLIBRARIES =  libdwfpackagewriter.la

libdwfpackagewriter_la_LDFLAGS = -avoid-version

libdwfpackagewriter_la_SOURCES =  DWFXMLSerializer.cpp PackageWriter.cpp

libdwfpackagewriter_la_LIBADD = $(top_builddir)/package/writer/extensions/libdwfpackagewriterextensions.la
