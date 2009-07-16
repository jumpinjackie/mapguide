AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerSiteService.vcproj ServerSiteService.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Common/Security \
  -I../../../../Common\MdfModel \  
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Oem/dbxml-2.3.10/xerces-c-src/src \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Manager \
  -I../../Common/ServerUtil \
  -I../Resource

lib_LTLIBRARIES = libMgServerSiteService.la

libMgServerSiteService_la_SOURCES = \
  ServerSiteServiceBuild.cpp

include_SOURCES = \
  OpAddGroup.cpp \
  OpAddServer.cpp \
  OpAddUser.cpp \
  OpAuthenticate.cpp \
  OpCreateSession.cpp \
  OpDeleteGroups.cpp \
  OpDeleteUsers.cpp \
  OpDestroySession.cpp \
  OpEnumerateGroups.cpp \
  OpEnumerateRoles.cpp \
  OpEnumerateServers.cpp \
  OpEnumerateUsers.cpp \
  OpGetSessionTimeout.cpp \
  OpGetUserForSession.cpp \
  OpGrantGroupMembershipsToUsers.cpp \
  OpGrantRoleMembershipsToGroups.cpp \
  OpGrantRoleMembershipsToUsers.cpp \
  OpRemoveServer.cpp \
  OpRevokeGroupMembershipsFromUsers.cpp \
  OpRevokeRoleMembershipsFromGroups.cpp \
  OpRevokeRoleMembershipsFromUsers.cpp \
  OpUpdateGroup.cpp \
  OpUpdateServer.cpp \
  OpUpdateUser.cpp \
  OpRequestServer.cpp \
  ServerSiteService.cpp \
  SiteOperation.cpp \
  SiteOperationFactory.cpp \
  SiteServiceHandler.cpp \
  SiteServiceUtil.cpp

noinst_HEADERS = $(include_SOURCES) \
  OpAddGroup.h \
  OpAddServer.h \
  OpAddUser.h \
  OpAuthenticate.h \
  OpCreateSession.h \
  OpDeleteGroups.h \
  OpDeleteUsers.h \
  OpDestroySession.h \
  OpEnumerateGroups.h \
  OpEnumerateRoles.h \
  OpEnumerateServers.h \
  OpEnumerateUsers.h \
  OpGetSessionTimeout.h \
  OpGetUserForSession.h \
  OpGrantGroupMembershipsToUsers.h \
  OpGrantRoleMembershipsToGroups.h \
  OpGrantRoleMembershipsToUsers.h \
  OpRemoveServer.h \
  OpRevokeGroupMembershipsFromUsers.h \
  OpRevokeRoleMembershipsFromGroups.h \
  OpRevokeRoleMembershipsFromUsers.h \
  OpUpdateGroup.h \
  OpUpdateServer.h \
  OpUpdateUser.h \
  OpRequestServer.h \
  ServerSiteDllExport.h \
  ServerSiteService.h \
  SiteOperation.h \
  SiteOperationFactory.h \
  SiteServiceDefs.h \
  SiteServiceHandler.h \
  SiteServiceUtil.h

libMgServerSiteService_la_LDFLAGS = -version-info 7:0:0
