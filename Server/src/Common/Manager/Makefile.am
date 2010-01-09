AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerManager.vcproj ServerManager.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I$(map_fdo_include) \
  -I../../../../Oem/dbxml-2.3.10/xerces-c-src/src \
  -I../../../../Common \
  -I../../../../Common/Security \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/MapGuideCommon \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MdfModel \
  -I../../../../Common/MdfParser \
  -I../../Services/Drawing \
  -I../../Services/Feature \
  -I../../Services/Kml \
  -I../../Services/Mapping \
  -I../../Services/Rendering \
  -I../../Services/Resource \
  -I../../Services/ServerAdmin \
  -I../../Services/Site \
  -I../../Services/Tile \
  -I../Cache \
  -I../Thread

lib_LTLIBRARIES = libMgServerManager.la

libMgServerManager_la_SOURCES = \
  ServerManagerBuild.cpp

include_SOURCES = \
  CacheManager.cpp \
  Connection.cpp \
  FdoConnectionManager.cpp \
  GroupInfo.cpp \
  LicenseManager.cpp \
  LoadBalanceManager.cpp \
  LogDetail.cpp \
  LogEntryData.cpp \
  LogManager.cpp \
  LogThread.cpp \
  LongTransactionManager.cpp \
  PackageLogHandler.cpp \
  PackageLogReader.cpp \
  PackageLogWriter.cpp \
  PackageManager.cpp \
  PermissionCache.cpp \
  PermissionInfo.cpp \
  PermissionManager.cpp \
  RoleInfo.cpp \
  SecurityCache.cpp \
  SecurityManager.cpp \
  ServerManager.cpp \
  ServiceManager.cpp \
  SessionCache.cpp \
  SessionInfo.cpp \
  SessionManager.cpp \
  UnmanagedDataManager.cpp \
  UserInfo.cpp \
  WorkerThread.cpp \
  WorkerThreadData.cpp

noinst_HEADERS = $(include_SOURCES) \
  CacheManager.h \
  Connection.h \
  FdoConnectionManager.h \
  GroupInfo.h \
  LicenseManager.h \
  LoadBalanceManager.h \
  LogDetail.h \
  LogEntryData.h \
  LogManager.h \
  LogThread.h \
  LogType.h \
  LongTransactionManager.h \
  PackageLogHandler.h \
  PackageLogReader.h \
  PackageLogWriter.h \
  PackageManager.h \
  PermissionCache.h \
  PermissionInfo.h \
  PermissionManager.h \
  RoleInfo.h \
  SecurityCache.h \
  SecurityManager.h \
  ServerManager.h \
  ServerManagerDllExport.h \
  ServiceManager.h \
  SessionCache.h \
  SessionInfo.h \
  SessionManager.h \
  UnmanagedDataManager.h \
  UserInfo.h \
  WorkerThread.h \
  WorkerThreadData.h

libMgServerManager_la_LDFLAGS = -version-info 7:0:0
