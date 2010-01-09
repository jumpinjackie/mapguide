AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = WebApp.rc WebApp.vcproj

prefix=@prefix@/webserverextensions

lib_LTLIBRARIES = libMgWebApp.la

include_SOURCES = WebBufferCommand.cpp \
                WebCommand.cpp \
                WebCommandCollection.cpp \
                WebCommandWidget.cpp \
                WebContextMenu.cpp \
                WebFlyoutWidget.cpp \
                WebGettingStarted.cpp \
                WebGetPrintablePageCommand.cpp \
                WebHelpCommand.cpp \
                WebInformationPane.cpp \
                WebInvokeScriptCommand.cpp \
                WebInvokeUrlCommand.cpp \
                WebLayout.cpp \
                WebMeasureCommand.cpp \
                WebPrintCommand.cpp \
                WebSearchCommand.cpp \
                WebSelectWithinCommand.cpp \
                WebSeparatorWidget.cpp \
                WebTaskBarWidget.cpp \
                WebTaskBar.cpp \
                WebTaskPane.cpp \
                WebToolbar.cpp \
                WebUiPane.cpp \
                WebUiSizablePane.cpp \
                WebUiTargetCommand.cpp \
                WebViewOptionsCommand.cpp \
                WebWidget.cpp \
                WebWidgetCollection.cpp

noinst_HEADERS = $(include_SOURCES) \
                WebActions.h \
                WebApp.h \
                WebAppClassId.h \
                WebAppExport.h \
                WebBufferCommand.h \
                WebCommand.h \
                WebCommandCollection.h \
                WebCommandWidget.h \
                WebContextMenu.h \
                WebFlyoutWidget.h \
                WebGetPrintablePageCommand.h \
                WebHelpCommand.h \
                WebInformationPane.h \
                WebInvokeScriptCommand.h \
                WebInvokeUrlCommand.h \
                WebLayout.h \
                WebMeasureCommand.h \
                WebPrintCommand.h \
                WebSearchCommand.h \
                WebSelectWithinCommand.h \
                WebSeparatorWidget.h \
                WebTargetType.h \
                WebTargetViewerType.h \
                WebTaskBar.h \
                WebTaskBarWidget.h \
                WebTaskButtonType.h \
                WebTaskPane.h \
                WebToolbar.h \
                WebUiPane.h \
                WebUiSizablePane.h \
                WebUiTargetCommand.h \
                WebViewOptionsCommand.h \
                WebWidgetType.h \
                WebWidget.h \
                WebWidgetCollection.h

libMgWebApp_la_SOURCES = WebAppBuild.cpp

INCLUDES = -I../../../Common/MdfModel \
           -I../../../Common/Foundation \
           -I../../../Common/Geometry \
           -I../../../Common/PlatformBase \
           -I../../../Common/MapGuideCommon \
           -I../../../Oem/dbxml-2.3.10/xerces-c-src/src \
           -I../../../Oem/ACE/ACE_wrappers

libMgWebApp_la_LIBADD = -lxerces-c -lMgFoundation -lMgGeometry -lMgPlatformBase -lMgMapGuideCommon

libMgWebApp_la_LDFLAGS = -version-info 7:0:0 \
            -L../../../Oem/dbxml-2.3.10/install/lib \
            -L../../../Common/Foundation \
            -L../../../Common/Geometry \
            -L../../../Common/PlatformBase \
            -L../../../Common/MapGuideCommon
