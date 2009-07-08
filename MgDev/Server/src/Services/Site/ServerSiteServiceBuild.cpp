//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifdef _WIN32
#include "Dll.cpp"
#endif
#include "OpAddGroup.cpp"
#include "OpAddServer.cpp"
#include "OpAddUser.cpp"
#include "OpAuthenticate.cpp"
#include "OpCreateSession.cpp"
#include "OpDeleteGroups.cpp"
#include "OpDeleteUsers.cpp"
#include "OpDestroySession.cpp"
#include "OpGetSessionTimeout.cpp"
#include "OpEnumerateGroups.cpp"
#include "OpEnumerateRoles.cpp"
#include "OpEnumerateServers.cpp"
#include "OpEnumerateUsers.cpp"
#include "OpGetUserForSession.cpp"
#include "OpGrantGroupMembershipsToUsers.cpp"
#include "OpGrantRoleMembershipsToGroups.cpp"
#include "OpGrantRoleMembershipsToUsers.cpp"
#include "OpRemoveServer.cpp"
#include "OpRevokeGroupMembershipsFromUsers.cpp"
#include "OpRevokeRoleMembershipsFromGroups.cpp"
#include "OpRevokeRoleMembershipsFromUsers.cpp"
#include "OpUpdateGroup.cpp"
#include "OpUpdateServer.cpp"
#include "OpUpdateUser.cpp"
#include "OpRequestServer.cpp"
#include "ServerSiteService.cpp"
#include "SiteOperation.cpp"
#include "SiteOperationFactory.cpp"
#include "SiteServiceHandler.cpp"
#include "SiteServiceUtil.cpp"
