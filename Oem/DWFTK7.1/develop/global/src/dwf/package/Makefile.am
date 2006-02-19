AUTOMAKE_OPTIONS = nostdinc
INCLUDES = -I../..


package_includedir = $(pkgincludedir)/package

package_include_HEADERS =  Bookmark.h Constants.h CustomSection.h DefinedObject.h Dependency.h DwfResult.h EModelSection.h EPlotSection.h FontResource.h GlobalSection.h GraphicResource.h Interface.h Manifest.h ObjectDefinition.h ObjectDefinitionResource.h Paper.h Property.h Resource.h Section.h SectionBuilder.h Source.h Units.h XML.h

SUBDIRS = reader writer utility

noinst_LTLIBRARIES =  libdwfpackage.la

libdwfpackage_la_LDFLAGS = -avoid-version

libdwfpackage_la_SOURCES =  Bookmark.cpp Constants.cpp CustomSection.cpp DefinedObject.cpp EModelSection.cpp EPlotSection.cpp FontResource.cpp GlobalSection.cpp GraphicResource.cpp Interface.cpp Manifest.cpp ObjectDefinition.cpp ObjectDefinitionResource.cpp Paper.cpp Property.cpp Resource.cpp Section.cpp SectionBuilder.cpp Source.cpp Units.cpp XML.cpp

libdwfpackage_la_LIBADD = $(top_builddir)/package/utility/libdwfpackageutility.la $(top_builddir)/package/writer/libdwfpackagewriter.la $(top_builddir)/package/reader/libdwfpackagereader.la
