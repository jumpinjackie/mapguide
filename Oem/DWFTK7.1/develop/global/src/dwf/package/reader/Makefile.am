AUTOMAKE_OPTIONS = nostdinc
INCLUDES = -I../../..

package_reader_includedir = $(pkgincludedir)/package/reader

package_reader_include_HEADERS =  EModelSectionDescriptorReader.h EPlotSectionDescriptorReader.h GlobalSectionDescriptorReader.h ObjectDefinitionReader.h PackageManifestReader.h PackageReader.h SectionDescriptorReader.h

noinst_LTLIBRARIES =  libdwfpackagereader.la

libdwfpackagereader_la_LDFLAGS = -avoid-version

libdwfpackagereader_la_SOURCES =  EModelSectionDescriptorReader.cpp EPlotSectionDescriptorReader.cpp GlobalSectionDescriptorReader.cpp ObjectDefinitionReader.cpp PackageManifestReader.cpp PackageReader.cpp SectionDescriptorReader.cpp
