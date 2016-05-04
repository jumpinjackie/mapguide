AUTOMAKE_OPTIONS = foreign

EXTRA_DIST =WebSupport.vcproj

prefix=@prefix@/webserverextensions

lib_LTLIBRARIES = libMgWebSupport.la

INCLUDES    = -I../../../Common/MdfModel \
              -I../../../Common/Foundation \
              -I../../../Common/Geometry \
              -I../../../Common/PlatformBase \
              -I../../../Common/MapGuideCommon \
              -I../../../Oem/ACE/ACE_wrappers
              
libMgWebSupport_la_SOURCES = InitializeWebTier.cpp

noinst_HEADERS = WebSupport.h

libMgWebSupport_la_LIBADD = -lMgFoundation -lMgPlatformBase -lMgMapGuideCommon

libMgWebSupport_la_LDFLAGS = -release $(PACKAGE_VERSION) \
            -L../../../Common/Foundation \
            -L../../../Common/PlatformBase \
            -L../../../Common/MapGuideCommon
