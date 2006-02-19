AUTOMAKE_OPTIONS = 
INCLUDES = -I../../.. -I.. -I.
pkginclude_HEADERS =  		AddLayer.h \
		   		AddLayerGroup.h \
		 		Command.h \
				RemoveLayer.h \
				RemoveLayerGroup.h \
				TransactionSection.h \
				TransactionSectionDescriptorReader.h \
				UpdateLayer.h \
				UpdateLayerGroup.h \
				UpdateUIGraphic.h	

noinst_LTLIBRARIES =  libemaptransaction.la

	
libemaptransaction_la_SOURCES = AddLayer.cpp \
				AddLayerGroup.cpp \
				Command.cpp \
				RemoveLayer.cpp \
				RemoveLayerGroup.cpp \
				TransactionSection.cpp \
				TransactionSectionDescriptorReader.cpp \
				UpdateLayer.cpp \
				UpdateLayerGroup.cpp \
				UpdateUIGraphic.cpp

	
libemaptransaction_la_LDFLAGS = -version-info 1.0.0

libemaptransaction_la_CPPFLAGS = -DDWFTK_EXPORT_API \
				-DDWFTK_BUILD_EXPAT \
				-DBSTREAM_STATIC_LIB \
				-DDWFCORE_BUILD_ZLIB \
				-DUNICODE \
				-D__linux__

