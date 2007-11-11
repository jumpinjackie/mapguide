AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ApacheAgent.vcproj

prefix=@prefix@/webserverextensions

INCLUDES    = -I../../../Common/Foundation \
              -I../../../Common/Geometry \
              -I../../../Common/PlatformBase \
              -I../../../Common/MapGuideCommon \
              -I../../../Common/MdfModel \
              -I../../../Oem/ACE/ACE_wrappers \
              -I../../../Oem/LinuxApt/httpd-2.2.4/include \
              -I../../../Oem/LinuxApt/httpd-2.2.4/srclib/apr/include \
              -I../../../Oem/LinuxApt/httpd-2.2.4/srclib/apr-util/include \
              -I../../../Oem/LinuxApt/httpd-2.2.4/os/unix \
              -I/usr/local/mapguideopensource/webserverextensions/apache2/include\
              -I../HttpHandler \
              -I../WebSupport \
              -I../MapAgentCommon

lib_LTLIBRARIES = mod_mgmapagent.la

mod_mgmapagent_la_SOURCES = ApacheAgent.cpp \
                              ApachePostParser.cpp \
                              ApacheResponseHandler.cpp \
                              ../MapAgentCommon/MapAgentCommon.cpp \
                              ../MapAgentCommon/MapAgentGetParser.cpp \
                              ../MapAgentCommon/MapAgentStrings.cpp \
                              ../WebSupport/InitializeWebTier.cpp

noinst_HEADERS = ApachePostParser.h \
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
                 -lMgHttpHandler 

mod_mgmapagent_la_LDFLAGS = -version-info 7:0:0 \
            -L../../../Oem/ACE/ACE_wrappers/ace \
            -L../../../Oem/dbxml-2.3.10/install/lib \
            -L$(top_builddir)/$(map_coordsys_dir_path) \
            -L../../../Common/MdfModel \
            -L../../../Common/MdfParser \
            -L../../../Common/Foundation \
            -L../../../Common/Geometry \
            -L../../../Common/PlatformBase \
            -L../../../Common/MapGuideCommon \
            -L../HttpHandler \
            -module
