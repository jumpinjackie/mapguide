AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerAdminService.vcproj ServerAdminService.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common\MdfModel \  
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Manager

lib_LTLIBRARIES = libMgServerAdminService.la

libMgServerAdminService_la_SOURCES = \
  ServerAdminServiceBuild.cpp

include_SOURCES = \
  OpBringOnline.cpp \
  OpClearLog.cpp \
  OpDeleteLog.cpp \
  OpDeletePackage.cpp \
  OpEnableMaximumLogSize.cpp \
  OpEnumerateLogs.cpp \
  OpEnumeratePackages.cpp \
  OpGetConfigurationProperties.cpp \
  OpGetDocument.cpp \
  OpGetInformationProperties.cpp \
  OpGetLog.cpp \
  OpGetLogFile.cpp \
  OpGetPackageLog.cpp \
  OpGetPackageStatus.cpp \
  OpIsMaximumLogSizeEnabled.cpp \
  OpIsOnline.cpp \
  OpLoadPackage.cpp \
  OpMakePackage.cpp \
  OpNotifyResourcesChanged.cpp \
  OpRegisterServicesOnServers.cpp \
  OpRenameLog.cpp \
  OpSetConfigurationProperties.cpp \
  OpRemoveConfigurationProperties.cpp \
  OpSetDocument.cpp \
  OpTakeOffline.cpp \
  OpUnregisterServicesOnServers.cpp \
  OpSetMaximumLogSize.cpp \
  OpSetLogDelimiter.cpp \
  ServerAdminOperation.cpp \
  ServerAdminOperationFactory.cpp \
  ServerAdminService.cpp \
  ServerAdminServiceHandler.cpp

noinst_HEADERS = $(include_SOURCES) \
  OpBringOnline.h \
  OpClearLog.h \
  OpDeleteLog.h \
  OpDeletePackage.h \
  OpEnableMaximumLogSize.h \
  OpEnumerateLogs.h \
  OpEnumeratePackages.h \
  OpGetConfigurationProperties.h \
  OpGetDocument.h \
  OpGetInformationProperties.h \
  OpGetLog.h \
  OpGetLogFile.h \
  OpGetPackageLog.h \
  OpGetPackageStatus.h \
  OpIsMaximumLogSizeEnabled.h \
  OpIsOnline.h \
  OpLoadPackage.h \
  OpMakePackage.h \
  OpNotifyResourcesChanged.h \
  OpRegisterServicesOnServers.h \
  OpRemoveConfigurationProperties.h \
  OpRenameLog.h \
  OpSetConfigurationProperties.h \
  OpSetDocument.h \
  OpTakeOffline.h \
  OpUnregisterServicesOnServers.h \
  OpSetMaximumLogSize.h \
  OpSetLogDelimiter.h \
  ServerAdminDllExport.h \
  ServerAdminOperation.h \
  ServerAdminOperationFactory.h \
  ServerAdminService.h \
  ServerAdminServiceHandler.h \
  ../../../../Common/PlatformBase/Services/Service.h

libMgServerAdminService_la_LDFLAGS = -version-info 7:0:0
