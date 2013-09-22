AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ApacheAgent.vcproj

prefix=@prefix@/webserverextensions

INCLUDES    = -I../../../Common/Foundation \
              -I../../../Common/Geometry \
              -I../../../Common/PlatformBase \
              -I../../../Common/MapGuideCommon \
              -I../../../Common/MdfModel \
              -I../../../Oem/ACE/ACE_wrappers \
              -I$(httpd_src)/include \
              -I$(httpd_src)/srclib/apr/include \
              -I$(httpd_src)/srclib/apr-util/include \
              -I$(httpd_src)/os/unix \
              -I/usr/local/mapguideopensource/webserverextensions/apache2/include\
              -I../HttpHandler \
              -I../WebSupport \
              -I../MapAgentCommon

lib_LTLIBRARIES = mod_mgmapagent.la

mod_mgmapagent_la_SOURCES = ApacheAgent.cpp \
                            ApachePostParser.cpp \
                            ApacheReaderStreamer.cpp \
                            ApacheResponseHandler.cpp \
                            ../MapAgentCommon/MapAgentCommon.cpp \
                            ../MapAgentCommon/MapAgentGetParser.cpp \
                            ../MapAgentCommon/MapAgentStrings.cpp \
                            ../WebSupport/InitializeWebTier.cpp

noinst_HEADERS = ApachePostParser.h \
                 ApacheReaderStreamer.h \
                 ApacheResponseHandler.h \
                 ../MapAgentCommon/MapAgentCommon.h \
                 ../MapAgentCommon/MapAgentGetParser.h \
                 ../MapAgentCommon/MapAgentStrings.h \
                 ../WebSupport/WebSupport.h

mod_mgmapagent_la_LIBADD = -lMgFoundation \
                 -lMgGeometry \
                 -lMgPlatformBase \
                 -lMgMdfModel \
                 -lMgMdfParser \
                 -lMgHttpHandler \
                 -lMgWebSupport

mod_mgmapagent_la_LDFLAGS = -release $(PACKAGE_VERSION) \
            -L../../../Oem/ACE/ACE_wrappers/ace \
            -L../../../Oem/dbxml/install/lib \
            -L../../../Common/CoordinateSystem \
            -L../../../Common/MdfModel \
            -L../../../Common/MdfParser \
            -L../../../Common/Foundation \
            -L../../../Common/Geometry \
            -L../../../Common/PlatformBase \
            -L../../../Common/MapGuideCommon \
            -L../HttpHandler \
            -L../WebSupport \
            -module
