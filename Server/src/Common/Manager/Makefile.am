AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerManager.vcproj ServerManager.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I$(map_fdo_include) \
  -I../../../../Oem/dbxml-2.2.13/xerces-c-src/src \
  -I../../../../Common \
  -I../../../../Common/Security \
  -I../../../../Common/Service \
  -I../../../../Common/Service/Util \
  -I../../../../Common/Service/Services \
  -I../../../../Common/Service/System \
  -I../../../../Common/Service/Geometry \
  -I../../Services/Drawing \
  -I../../Services/Feature \
  -I../../Services/Mapping \
  -I../../Services/Rendering \
  -I../../Services/Resource \
  -I../../Services/ServerAdmin \
  -I../../Services/Site \
  -I../../Services/Tile \
  -I../ServerUtil/

lib_LTLIBRARIES = libMgServerManager.la

libMgServerManager_la_SOURCES = \
  ServerManagerBuild.cpp

include_SOURCES = \
  LoadBalanceManager.cpp \
  Connection.cpp \
  FdoConnectionManager.cpp \
  GroupInfo.cpp \
  LogManager.cpp \
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
  UserInfo.cpp

noinst_HEADERS = $(include_SOURCES) \
  LoadBalanceManager.h \
  Connection.h \
  FdoConnectionManager.h \
  GroupInfo.h \
  LogManager.h \
  LogType.h \
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
  UserInfo.h

libMgServerManager_la_LDFLAGS = -version-info 7:0:0
