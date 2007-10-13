AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = CgiAgent.rc CgiAgent.vcproj

prefix=@prefix@/webserverextensions

INCLUDES    = -I../../../Common/Foundation \
              -I../../../Common/Geometry \
              -I../../../Common/PlatformBase \
              -I../../../Common/MapGuideCommon \
              -I../../../Common/MdfModel \
              -I../../../Oem/ACE/ACE_wrappers \
              -I../HttpHandler \
              -I../WebSupport \
              -I../MapAgentCommon

bin_PROGRAMS = mapagent

mapagent_SOURCES = ../MapAgentCommon/MapAgentGetParser.cpp \
                   CgiPostParser.cpp \
                   CgiResponseHandler.cpp \
                   ../MapAgentCommon/MapAgentStrings.cpp \
                   EchoTest.cpp \
                   CgiAgent.cpp \
                   ../WebSupport/InitializeWebTier.cpp

noinst_HEADERS = ../MapAgentCommon/MapAgentGetParser.h \
                 CgiPostParser.h \
                 CgiResponseHandler.h \
                 ../MapAgentCommon/MapAgentStrings.h \
                 ../WebSupport/WebSupport.h

mapagent_LDADD = -lACE \
                 -lxerces-c \
                 -lMgFoundation \
                 -lMgGeometry \
                 -lMgPlatformBase \
                 -lMgMdfModel \
                 -lMgMdfParser \
                 -lMgHttpHandler 

mapagent_LDFLAGS = -version-info 7:0:0 \
            -L../../../Oem/ACE/ACE_wrappers/ace \
            -L../../../Oem/dbxml-2.3.10/install/lib \
            -L$(top_builddir)/$(map_coordsys_dir_path) \
            -L../../../Common/MdfModel \
            -L../../../Common/MdfParser \
            -L../../../Common/Foundation \
            -L../../../Common/Geometry \
            -L../../../Common/PlatformBase \
            -L../../../Common/MapGuideCommon \
            -L../HttpHandler
