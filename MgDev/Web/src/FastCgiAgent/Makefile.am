AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = FastCgiAgent.rc FastCgiAgent.vcproj

prefix=@prefix@/webserverextensions

INCLUDES    = -I../../../Common/Foundation \
              -I../../../Common/Geometry \
              -I../../../Common/PlatformBase \
              -I../../../Common/MapGuideCommon \
              -I../../../Common/MdfModel \
              -I../../../Oem/ACE/ACE_wrappers \
              -I../../../Oem/fcgi/fcgi-2.4.0/include \
              -I../HttpHandler \
              -I../WebSupport

bin_PROGRAMS = mapagent

mapagent_SOURCES = CgiGetParser.cpp \
                   CgiPostParser.cpp \
                   CgiResponseHandler.cpp \
                   CgiStrings.cpp \
                   EchoTest.cpp \
                   FastCgiAgent.cpp \
                   ../WebSupport/InitializeWebTier.cpp

noinst_HEADERS = CgiGetParser.h \
                 CgiPostParser.h \
                 CgiResponseHandler.h \
                 CgiStrings.h \
                 Product.h \
                 ../WebSupport/WebSupport.h

mapagent_LDADD = -lACE \
                 -lxerces-c \
                 ../../../Oem/fcgi/fcgi-2.4.0/libfcgi/.libs/libfcgi.so \
                 -lMgFoundation \
                 -lMgGeometry \
                 -lMgPlatformBase \
                 -lMgCoordinateSystem \
                 -lMgMdfModel \
                 -lMgMdfParser \
                 -lMgHttpHandler 

mapagent_LDFLAGS = -version-info 7:0:0 \
            -L../../../Oem/ACE/ACE_wrappers/ace \
            -L../../../Oem/dbxml-2.2.13/install/lib \
            -L$(top_builddir)/$(map_coordsys_dir_path) \
            -L../../../Common/MdfModel \
            -L../../../Common/MdfParser \
            -L../../../Common/Foundation \
            -L../../../Common/Geometry \
            -L../../../Common/PlatformBase \
            -L../../../Common/MapGuideCommon \
            -L../HttpHandler
