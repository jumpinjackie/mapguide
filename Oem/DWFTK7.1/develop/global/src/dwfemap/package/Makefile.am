AUTOMAKE_OPTIONS = 
INCLUDES = -I../.. -I.


pkginclude_HEADERS =  Background.h \
				Bounds.h \
				CoordinateSpace.h \
				EMapConstants.h \
				EMapInterface.h \
				EMapSection.h \
				EMapGlobalSection.h \
				EMapSectionDescriptorReader.h \
				EMapUtility.h \
				Layer.h \
				LayerGroup.h \
				LinearUnit.h \
				ScaleRange.h \
				UIGraphic.h \
				View.h 

SUBDIRS = transaction

noinst_LTLIBRARIES =  libemappackage.la

libemappackage_la_SOURCES =  	Background.cpp \
				Bounds.cpp \
				CoordinateSpace.cpp \
				EMapConstants.cpp \
				EMapInterface.cpp \
				EMapSection.cpp \
				EMapGlobalSection.cpp \
				EMapSectionDescriptorReader.cpp \
				EMapUtility.cpp \
				Layer.cpp \
				LayerGroup.cpp \
				LinearUnit.cpp \
				ScaleRange.cpp \
				UIGraphic.cpp \
				View.cpp 
 
libemappackage_la_LDFLAGS = -version-info 1.0.0

libemappackage_la_CPPFLAGS = 	-DDWFTK_EXPORT_API \
				-DDWFTK_BUILD_EXPAT \
				-DBSTREAM_STATIC_LIB \
				-DDWFCORE_BUILD_ZLIB \
				-DUNICODE \
				-D__linux__
				

libemappackage_la_LIBADD = $(top_builddir)/package/transaction/libemaptransaction.la 
