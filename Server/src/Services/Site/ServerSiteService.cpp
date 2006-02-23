//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "SiteServiceDefs.h"
#include "ServerSiteService.h"
#include "Util.h"
#include "SecurityManager.h"
#include "SessionManager.h"
#include "LoadBalanceManager.h"
#include "ServiceManager.h"
#include "SiteServiceUtil.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgServerSiteService::MgServerSiteService( MgConnectionProperties* connection )
    : MgService (connection)
{
    MG_SITE_SERVICE_TRY()

    m_loadBalanceManager = MgLoadBalanceManager::GetInstance();
    assert(NULL != m_loadBalanceManager);

    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    assert(NULL != serviceManager);

    m_resourceService = dynamic_cast<MgServerResourceService*>(
        serviceManager->RequestService(MgServiceType::ResourceService));
    assert(m_resourceService != NULL);

    MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());

    MG_SITE_SERVICE_CATCH_AND_THROW(L"MgServerSiteService.MgServerSiteService")
}

///----------------------------------------------------------------------------
/// <summary>
/// Default constructor. Need this to prevent a GCC 3 compile error.
/// </summary>
///----------------------------------------------------------------------------

MgServerSiteService::MgServerSiteService() : MgService(NULL)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgServerSiteService::~MgServerSiteService( void )
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///----------------------------------------------------------------------------

void MgServerSiteService::Dispose()
{
    delete this;
}

//-----------------------------------------------------------------------------
//  User Methods
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Add a user to the Mg Resource Repository.
/// </summary>
///
/// <param name="userID">
/// ID of the new user.
/// </param>
/// <param name="userName">
/// Name of the new user.
/// </param>
/// <param name="password">
/// New user's password.
/// </param>
/// <param name="description">
/// Description of the new user.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgDuplicateUserException
/// MgInvalidPasswordException
void MgServerSiteService::AddUser( CREFSTRING userID, CREFSTRING userName,
        CREFSTRING password, CREFSTRING description )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::AddUser()");

    this->m_resourceService->AddUser( userID, userName, password, description );
    MgSecurityManager::SetUser(userID, password);

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.AddUser" )
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes a set of users from the server.  Prior to deletion, all of the users'
/// group memberships are revoked.
/// NOTE:
/// - If users is empty, then all users will be deleted.
/// <param name="users">
/// IDs of the users to be deleted.
/// </summary>
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
void MgServerSiteService::DeleteUsers( MgStringCollection* users )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::DeleteUsers()");

    this->m_resourceService->DeleteUsers( users );
    MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.DeleteUsers" )
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a list of available users.  Parameters specify the contents of the list:
/// - If both group and role are NOT specified return a list of all available users.
/// - If group is specified, return a list of users in that group.
/// - If role is specified and includeGroups is false, return a list of users
///   that role.
/// - If role is specified and includeGroups is true, return a list of users and
///   groups in that role (the list of users in each group is not expanded).
/// - If both group and role are specified, throw exception.
/// </summary>
///
/// <param name="group">
/// Name of group, if returning the list of users in a group; NULL, otherwise.
/// </param>
/// <param name="role">
/// Role, if returning a list of users and plus, optionally, groups with a
/// particular role; empty, otherwise.
/// </param>
/// <param name="includeGroups">
/// Flag used when role is specified; True, indicates that groups with the specified
/// role should be returned in the list along with the users; False, indicates that
/// groups should not be included.
/// </param>
/// <returns>
/// Result containing the appropriate list of users.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
/// MgRoleNotFoundException
MgByteReader* MgServerSiteService::EnumerateUsers( CREFSTRING group,
        CREFSTRING role, bool includeGroups )
{
    MgByteReader* ret = 0;

    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::EnumerateUsers()");

    ret = this->m_resourceService->EnumerateUsers( group, role, false, includeGroups );

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.EnumerateUsers" )

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Grants membership in the specified groups to the specified users.
/// </summary>
///
/// <param name="groups">
/// Groups to receive the specified users.
/// </param>
/// <param name="users">
/// IDs of users to gain membership in the specified groups.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgGroupNotFoundException
void MgServerSiteService::GrantGroupMembershipsToUsers( MgStringCollection* groups,
        MgStringCollection* users )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::GrantGroupMembershipsToUsers()");

    this->m_resourceService->GrantGroupMembershipsToUsers( groups, users );
    MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.GrantGroupMembershipsToUsers" )
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Grants the specified roles to the specified users.
/// </summary>
/// <param name="roles">
/// List of roles to grant to the specified users.
/// </param>
/// <param name="users">
/// IDs of users to receive the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgRoleNotFoundException
void MgServerSiteService::GrantRoleMembershipsToUsers( MgStringCollection* roles,
        MgStringCollection* users )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::GrantRoleMembershipsToUsers()");

    this->m_resourceService->GrantRoleMembershipsToUsers( roles, users );
    MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.GrantRoleMembershipsToUsers" )
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Revokes membership in the specified groups from the specified users.
/// </summary>
///
/// <param name="groups">
/// Groups to lose the specified users.
/// </param>
/// <param name="users">
/// IDs of users to lose membership in the specified groups.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgGroupNotFoundException
void MgServerSiteService::RevokeGroupMembershipsFromUsers( MgStringCollection* groups,
        MgStringCollection* users )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::RevokeGroupMembershipsFromUsers()");

    this->m_resourceService->RevokeGroupMembershipsFromUsers( groups, users );
    MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.RevokeGroupMembershipsFromUsers" )
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Revokes the specified roles from the specified users.
/// </summary>
///
/// <param name="roles">
/// List of roles to revoke from the specified users.
/// Repository.
/// <param name="users">
/// IDs of users to lose the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgRoleNotFoundException
void MgServerSiteService::RevokeRoleMembershipsFromUsers( MgStringCollection* roles,
        MgStringCollection* users )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::RevokeRoleMembershipsFromUsers()");

    this->m_resourceService->RevokeRoleMembershipsFromUsers( roles, users );
    MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.RevokeRoleMembershipsFromUsers" )
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Updates the information stored for a user.
/// </summary>
///
/// <param name="userID">
/// ID of the user to be updated.
/// </param>
/// <param name="newUserID">
/// New user ID, if applicable.  Set newUserID value to empty if no change is to be made.
/// </param>
/// <param name="newUserName">
/// New user name, if applicable.  Set newUserName value to empty if no change is to be made.
/// </param>
/// <param name="newPassword">
/// New password, if applicable.  Set newPassword value to empty, if no change is to be made.
/// </param>
/// <param name="newDescription">
/// New description, if applicable.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgDuplicateUserException
/// MgInvalidPasswordException
void MgServerSiteService::UpdateUser( CREFSTRING userID, CREFSTRING newUserID,
        CREFSTRING newUserName, CREFSTRING newPassword,
        CREFSTRING newDescription )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::UpdateUser()");

    this->m_resourceService->UpdateUser( userID, newUserID, newUserName, newPassword,
        newDescription );

    if (newUserID.empty() && !newPassword.empty())
    {
        MgSecurityManager::SetUser(userID, newPassword);
    }
    else
    {
        MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());
    }

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.UpdateUser" )
}

//-----------------------------------------------------------------------------
//  Group Methods
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Adds a new group.
/// </summary>
///
/// <param name="group">
/// Name of the new group.
/// </param>
/// <param name="description">
/// Description of the new group.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgDuplicateGroupException
void MgServerSiteService::AddGroup( CREFSTRING group, CREFSTRING description )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::AddGroup()");

    this->m_resourceService->AddGroup(group, description);
    MgSecurityManager::SetGroup(group);

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.AddGroup" );
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes a set of groups from the server.  All users owned by the groups will
/// have their membership revoked prior to deletion of the groups.
/// </summary>
///
/// <param name="groups">
/// Names of the groups to be deleted.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
void MgServerSiteService::DeleteGroups( MgStringCollection* groups )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::DeleteGroups()");

    m_resourceService->DeleteGroups( groups );
    MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.DeleteGroups" );
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a list of available groups.  Parameters specify the contents of the list:
/// - If both user and role are NOT specified, return a list of all available groups.
/// - If user is specified, return a list of groups of which that user is a member.
/// - If role is specified, return a list of groups in that role.
/// - If both user and role are specified, throw exception.
/// </summary>
///
/// <param name="user">
/// ID of user, if returning a list of groups owning a particular user; empty,
/// otherwise.
/// </param>
/// <param name="role">
/// Role, if returning a list of groups with a particular role; empty, otherwise.
/// </param>
/// <returns>
/// Result containing the appropriate list of groups.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgRoleNotFoundException
MgByteReader* MgServerSiteService::EnumerateGroups( CREFSTRING user, CREFSTRING role )
{
    Ptr<MgByteReader> ret;

    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::EnumerateGroups()");

    ret = this->m_resourceService->EnumerateGroups( user, role );

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.EnumerateGroups" )

    return SAFE_ADDREF( (MgByteReader*) ret);
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Grants the specified role memberships to the specified groups.
/// </summary>
///
/// <param name="roles">
/// List of roles to be granted to the specified groups.
/// </param>
/// <param name="groups">
/// Groups to receive the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
/// MgRoleNotFoundException
void MgServerSiteService::GrantRoleMembershipsToGroups( MgStringCollection* roles,
        MgStringCollection* groups )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::GrantRoleMembershipsToGroups()");

    this->m_resourceService->GrantRoleMembershipsToGroups( roles, groups );
    MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.GrantRoleMembershipsToGroups" )
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Revokes the specified roles from the specified groups.
/// </summary>
///
/// <param name="roles">
/// Roles to be revoked from the groups.
/// </param>
/// <param name="groups">
/// Groups to lose the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
/// MgRoleNotFoundException
void MgServerSiteService::RevokeRoleMembershipsFromGroups( MgStringCollection* roles,
        MgStringCollection* groups )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::RevokeRoleMembershipsFromGroups()");

    this->m_resourceService->RevokeRoleMembershipsFromGroups( roles, groups );
    MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.RevokeRoleMembershipsFromGroups" )
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Update the information stored for a group.
/// </summary>
///
/// <param name="group">
/// Name of the group to be updated.
/// </param>
/// <param name="newGroup">
/// New group name, if applicable.  Set newGroup value to NULL if no change is to
/// be made.
/// </param>
/// <param name="newDescription">
/// New description, if applicable.  Set newDescription value to NULL if no change
/// is to be made.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
/// MgDuplicateGroupException
void MgServerSiteService::UpdateGroup( CREFSTRING group, CREFSTRING newGroup,
        CREFSTRING newDescription )
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::UpdateGroup()");

    m_resourceService->UpdateGroup( group, newGroup, newDescription );

    if (!newGroup.empty())
    {
        MgSecurityManager::RefreshSecurityCache(m_resourceService->CreateSecurityCache());
    }

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.UpdateGroup" );
}

//-----------------------------------------------------------------------------
//  Role Methods
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a list of available roles.  Parameters specify contents of the list:
/// - If both user and group are NOT specified, return a list of all available roles.
/// - If group is specified, return a list of roles for that group.
/// - If user is specified, return a list of roles for that user.
/// - If both user and group are specified, throw an exception.
/// </summary>
///
/// <param name="user">
/// ID of user, if returning list of roles for a particular user; empty, otherwise.
/// </param>
/// <param name="group">
/// Name of group, if returning list of roles for a particular group; empty, otherwise.
/// </param>
/// <returns>
/// Result containing the appropriate list of roles.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
/// MgUserNotFoundException
MgStringCollection* MgServerSiteService::EnumerateRoles( CREFSTRING user,
        CREFSTRING group )
{
    Ptr<MgStringCollection> ret;
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::EnumerateRoles()");

    ret = m_resourceService->EnumerateRoles( user, group );

    MG_SITE_SERVICE_CATCH_AND_THROW( L"MgServerSiteService.EnumerateRoles" );

    return SAFE_ADDREF( (MgStringCollection*) ret);
}

///----------------------------------------------------------------------------
/// <summary>
/// Authenticates a user and validates his/her roles.
/// </summary>
///
/// <param name="userInformation">
/// User information to authenticate against.
/// </param>
/// <param name="requiredRoles">
/// Required roles to validate against.
/// </param>
/// <param name="returnAssignedRoles">
/// Flag to determine whether or not the assigned roles of the user should
/// be returned.
/// </param>
///
/// <returns>
/// Assigned roles of the user (maybe NULL).
/// </returns>
///
/// <exceptions>
/// MgNullArgumentException
/// MgAuthenticationFailedException
/// MgUnauthorizedAccessException
/// </exceptions>
///----------------------------------------------------------------------------

MgStringCollection* MgServerSiteService::Authenticate(
    MgUserInformation* userInformation, MgStringCollection* requiredRoles,
    bool returnAssignedRoles)
{
    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::Authenticate()");

    Ptr<MgStringCollection> assignedRoles = MgSecurityManager::Authenticate(
        userInformation, requiredRoles, returnAssignedRoles);

    return assignedRoles.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a session for the user and return the unique session identifier. For the
/// lifetime of the session the session identifier can be used to reauthenticate
/// with the Mg Site. The session also defines a Resource Repository with the
/// same name as the session identifier. Resources placed into this repository will
/// exist until the session expires or the session is destroyed.
/// </summary>
///
/// <exceptions>
/// MgDuplicateSessionException
/// </exceptions>
///----------------------------------------------------------------------------

STRING MgServerSiteService::CreateSession()
{
    STRING session;
    MgUserInformation* currUserInfo = MgUserInformation::GetCurrentUserInfo();
    assert(NULL != currUserInfo);

    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::CreateSession()");

    session = currUserInfo->GetMgSessionId();

    if (session.empty())
    {
        session = currUserInfo->CreateMgSessionId();
        currUserInfo->SetMgSessionId(session);
    }
    else
    {
        throw new MgDuplicateSessionException(
            L"MgServerSiteService.CreateSession", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Note that if an error occurs, the abandoned session repositry will
    // be eventually deleted by the internal session cleanup event timer.

    MgResourceIdentifier resource(MgRepositoryType::Session, session,
        L"", L"", MgResourceType::Folder);

    MgSessionManager::AddSession(session, currUserInfo->GetUserName());
    m_resourceService->CreateRepository(&resource, NULL, NULL);

    MG_SITE_SERVICE_CATCH_AND_THROW(L"MgServerSiteService.CreateSession")

    return session;
}

///----------------------------------------------------------------------------
/// <summary>
/// Destroys the specified session and discards any resources stored in the
/// session's resource repository.
/// </summary>
///
/// <exceptions>
/// MgSessionExpiredException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerSiteService::DestroySession(CREFSTRING session)
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::DestroySession()");

    MgResourceIdentifier resource(MgRepositoryType::Session, session,
        L"", L"", MgResourceType::Folder);

    m_resourceService->DeleteRepository(&resource);
    MgSessionManager::RemoveSession(session);

    MG_SITE_SERVICE_CATCH_AND_THROW(L"MgServerSiteService.DestroySession")
}

///////////////////////////////////////////////////////////////////////////////
///
/// Server Management
///
///////////////////////////////////////////////////////////////////////////////

///----------------------------------------------------------------------------
/// <summary>
/// Return an XML document describing the server on the site.
/// </summary>
/// <returns>
/// XML document describing the list of servers.
/// </returns>
///
/// EXCEPTIONS:
///----------------------------------------------------------------------------

MgByteReader* MgServerSiteService::EnumerateServers()
{
    Ptr<MgByteReader> byteReader;

    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::EnumerateServers()");

    byteReader = m_loadBalanceManager->EnumerateServers();

    MG_SITE_SERVICE_CATCH_AND_THROW(L"MgServerSiteService.EnumerateServers")

    return SAFE_ADDREF(byteReader.p);
}

///----------------------------------------------------------------------------
/// <summary>
/// Add server to the site.
/// </summary>
/// <param name="name">
/// Mg specific server name.  May be the same as the server DNS name.
/// </param>
/// <param name="description">
/// Server long description.  May indicate the server's intended use.
/// </param>
/// <param name="address">
/// Server IP address or DNS name.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgServerAlreadyExistsException
/// MgInvalidServerAddressException
///----------------------------------------------------------------------------

void MgServerSiteService::AddServer(CREFSTRING name, CREFSTRING description,
    CREFSTRING address)
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::AddServer()");

    m_loadBalanceManager->AddServer(name, description, address);

    MG_SITE_SERVICE_CATCH_AND_THROW(L"MgServerSiteService.AddServer");
}


///----------------------------------------------------------------------------
/// <summary>
/// Update server information for the site.
/// </summary>
/// <param name="oldName">
/// Current server name.
/// </param>
/// <param name="newName">
/// New server name.
/// </param>
/// <param name="newDescription">
/// New long description.
/// </param>
/// <param name="newAddress">
/// New server IP address or DNS name.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgServerNotFoundException
/// MgServerAlreadyExistsException
/// MgInvalidServerAddressException
///----------------------------------------------------------------------------

void MgServerSiteService::UpdateServer(CREFSTRING oldName, CREFSTRING newName,
    CREFSTRING newDescription, CREFSTRING newAddress)
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::UpdateServer()");

    m_loadBalanceManager->UpdateServer(oldName, newName, newDescription, newAddress);

    MG_SITE_SERVICE_CATCH_AND_THROW(L"MgServerSiteService.UpdateServer");
}

///----------------------------------------------------------------------------
/// <summary>
/// Remove server from the site.
/// </summary>
/// <param name="name">
/// Name of server to remove from site.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgServerNotFoundException
///----------------------------------------------------------------------------

void MgServerSiteService::RemoveServer(CREFSTRING name)
{
    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::RemoveServer()");

    m_loadBalanceManager->RemoveServer(name);

    MG_SITE_SERVICE_CATCH_AND_THROW(L"MgServerSiteService.RemoveServer");
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the IP address of the next server available for the specified
/// service.
/// </summary>
///
/// <param name="serviceType">
/// Type of the service.
/// </param>
///
/// <returns>
/// IP address of the server.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidArgumentException
///----------------------------------------------------------------------------

STRING MgServerSiteService::RequestServer(INT32 serviceType)
{
    STRING serverAddress;

    MG_SITE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerSiteService::RequestServer()");

    MG_CHECK_RANGE(serviceType, 0, MgServerInformation::sm_knMaxNumberServices - 1,
        L"MgServerSiteService.RequestServer");

    serverAddress = m_loadBalanceManager->RequestServer(serviceType);

    MG_SITE_SERVICE_CATCH_AND_THROW(L"MgServerSiteService.RequestServer")

    return serverAddress;
}
