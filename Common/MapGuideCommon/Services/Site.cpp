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

#include "MapGuideCommon.h"
#include "Site.h"
#include "Command.h"

static const int Site_Admin = (int)MgPacketParser::msiSite;

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgSite object.
/// </summary>
MgSite::MgSite()
{
    // One warning object per Site instance
    m_warning = new MgWarnings();
}

//  Inherited from MgDisposable
void MgSite::Dispose()
{
    delete this;
}

//-----------------------------------------------------------------------------
//  Methods/Functions
//-----------------------------------------------------------------------------

/// <summary>
/// GetWarning() method returns the latest warning.
/// </summary>
///
/// <returns>
/// Returns a pointer to the MgWarnings.
/// </returns>
MgWarnings* MgSite::GetWarning()
{
    return SAFE_ADDREF((MgWarnings*)(this->m_warning));
}

/// <summary>
/// HasWarning() method reports if there is a current warning.
/// </summary>
///
/// <returns>
/// Returns true or false
/// </returns>
bool MgSite::HasWarning()
{
    return this->m_warning != NULL ? true : false;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Get the unique identifier for the class
///</summary>
///<returns>Class Identifider.</returns>
///
INT32 MgSite::GetClassId()
{
    return this->m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Opens a connection to a Site Server.
/// </summary>

void MgSite::Open(MgUserInformation* userInformation)
{
    MG_SITE_TRY()

    Open(userInformation, false);

    MG_SITE_CATCH_AND_THROW(L"MgSite.Open")
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Opens a connection to a Site Server.
/// </summary>

void MgSite::Open(MgUserInformation* userInformation, bool skipAuthenticate)
{
    MG_SITE_TRY()

    // Authenticate the user.
    Authenticate(userInformation, NULL, NULL, false, skipAuthenticate);

    MG_SITE_CATCH_AND_THROW(L"MgSite.Open")
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Opens a connection to the specified Site Server.
/// </summary>

void MgSite::Open(MgUserInformation* userInformation, MgSiteInfo* siteInfo)
{
    MG_SITE_TRY()

    // Authenticate the user.

    Authenticate(userInformation, siteInfo, NULL, false, false);

    MG_SITE_CATCH_AND_THROW(L"MgSite.Open")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Closes the current Site Server connection.
/// </summary>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
void MgSite::Close()
{
    m_connProp = NULL;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the addresses of the Site Servers.
/// </summary>
/// <returns>
/// Returns the site server address.
/// </returns>
///
MgSiteInfo* MgSite::GetSiteServerInfo(INT32 index)
{
    MgSiteManager* siteManager = MgSiteManager::GetInstance();
    return siteManager->GetSiteInfo(index);
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the addresses of the Site Servers.
/// </summary>
/// <returns>
/// Returns the site server address.
/// </returns>
///
INT32 MgSite::GetSiteServerCount()
{
    MgSiteManager* siteManager = MgSiteManager::GetInstance();
    return siteManager->GetSiteCount();
}

STRING MgSite::GetCurrentSiteAddress()
{
    STRING target = L"";
    if(m_connProp != NULL)
    {
        target = m_connProp->GetTarget();
    }
    return target;
}


///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Create a session for the user and return the unique session identifier. For the
/// lifetime of the session the session identifier can be used to reauthenticate
/// with the Site. The session also defines a Resource Repository with the
/// same name as the session identifier. Resources placed into this repository will
/// exist until the session expires or the session is closed.
/// <returns>
/// The newly created session identifier
/// </returns>
/// EXCEPTIONS:
/// To be documented
STRING MgSite::CreateSession()
{
    STRING session;

    MG_SITE_TRY()

    // Return the current one if it already exists.
    session = GetCurrentSession();
    if (session.length() > 0)
    {
        return session;
    }

    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                  // Connection
                        MgCommand::knString,                // Return type expected
                        MgSiteOpId::CreateSession,          // Command Code
                        0,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    session = *(cmd.GetReturnValue().val.m_str);
    delete cmd.GetReturnValue().val.m_str;

    MG_SITE_CATCH_AND_THROW( L"MgSite.CreateSession" );

    return session;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Destroys the specified session and discards any resources stored in the session's
/// resource repository.
/// <param name="session">
/// Identifier returned from create session
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
/// To be documented
void MgSite::DestroySession(CREFSTRING session)
{
    MG_SITE_TRY()

    CHECK_EMPTY_CREFSTRING(session, L"MgSite::DestroySession")

    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                  // Connection
                        MgCommand::knVoid,                  // Return type expected
                        MgSiteOpId::DestroySession,         // Command Code
                        1,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knString, &session,      // Argument#1
                        MgCommand::knNone );

    SetWarning(cmd.GetWarningObject());

    MG_SITE_CATCH_AND_THROW(L"MgSite.DestroySession")
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the current session identifier if a session has already been created.
/// <returns>
/// Previously created unique session identifier
/// </returns>
/// EXCEPTIONS:
/// To be documented

STRING MgSite::GetCurrentSession()
{
    STRING sessionId;
    Ptr<MgUserInformation> userInformation = m_connProp->GetUserInfo();

    if (userInformation.p != NULL)
    {
        sessionId = userInformation->GetMgSessionId();
    }

    return sessionId;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the session time out value.
/// Resources placed into the session repository will exist until
/// the session expires or the session is destroyed by using
/// DestroySession. The session timeout value is the lifttime of session
/// repository.
/// <returns>
/// The timeout value of session measured in second.
/// </returns>
/// EXCEPTIONS:
/// To be documented
INT32 MgSite::GetSessionTimeout()
{
    INT32 sessionTimeout;

    MG_SITE_TRY()

        MgConfiguration *m_config = MgConfiguration::GetInstance();
        m_config->GetIntValue(
        MgConfigProperties::SiteServicePropertiesSection,
        MgConfigProperties::SiteServicePropertySessionTimeout,
        sessionTimeout,
        MgConfigProperties::DefaultSiteServicePropertySessionTimeout);

    MG_SITE_CATCH_AND_THROW( L"MgSiteConnection.GetSessionTimeout" );

    return sessionTimeout;
}

STRING MgSite::GetUserForSession()
{
    STRING userId;

    MG_SITE_TRY()

    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                          // Connection
                        MgCommand::knString,                // Return type expected
                        MgSiteOpId::GetUserForSession,      // Command Code
                        0,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(2,0,0),               // Operation version
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    userId = *(cmd.GetReturnValue().val.m_str);
    delete cmd.GetReturnValue().val.m_str;

    MG_SITE_CATCH_AND_THROW( L"MgSite.GetUserForSession" );

    return userId;
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
/// Name of group, if returning the list of users in a group; empty, otherwise.
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
MgByteReader* MgSite::EnumerateUsers( CREFSTRING group,
    CREFSTRING role, bool includeGroups)
{
    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                  // Connection
                        MgCommand::knObject,                // Return type expected
                        MgSiteOpId::EnumerateUsers,         // Command Code
                        3,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knString, &group,        // Argument#1
                        MgCommand::knString, &role,         // Argument#2
                        MgCommand::knInt8, includeGroups,   // Argument#3
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    MG_SITE_CATCH_AND_THROW( L"MgSite::EnumerateUsers" )

    return (MgByteReader*) cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a list of available users.  Parameters specify the contents of the list:
/// - If group is NOT specified, return a list of all available users.
/// - If group is specified, return a list of users in that group.
/// </summary>
///
/// <param name="group">
/// Name of group, if returning the list of users in a group; empty, otherwise.
/// </param>
/// <returns>
/// Result containing the appropriate list of users.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
///
/// NOTE:
/// 1.  This method is part of the public API.
MgByteReader* MgSite::EnumerateUsers( CREFSTRING group )
{
    return this->EnumerateUsers( group, L"", false );
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Add a user to the Resource Repository.
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
void MgSite::AddUser( CREFSTRING userID, CREFSTRING userName,
    CREFSTRING password, CREFSTRING description )
{
    CHECK_EMPTY_CREFSTRING(userID,   L"MgSite.AddUser")
    CHECK_EMPTY_CREFSTRING(userName, L"MgSite.AddUser")
    CHECK_EMPTY_CREFSTRING(password, L"MgSite.AddUser")

    MG_SITE_TRY()

    MgCommand cmd;
    STRING encryptedPassword;

    if (!password.empty())
    {
        MgCryptographyManager cryptoManager;

        encryptedPassword = cryptoManager.EncryptPassword(password);
    }

    cmd.ExecuteCommand(m_connProp,                          // Connection
                        MgCommand::knVoid,                  // Return type expected
                        MgSiteOpId::AddUser,                // Command Code
                        4,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knString, &userID,       // Argument#1
                        MgCommand::knString, &userName,     // Argument#2
                        MgCommand::knString, &encryptedPassword, // Argument#3
                        MgCommand::knString, &description,  // Argument#4
                        MgCommand::knNone );

    SetWarning(cmd.GetWarningObject());

    MG_SITE_CATCH_AND_THROW(L"MgSite.AddUser")
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
/// New user ID.
/// </param>
/// <param name="newUserName">
/// New user name.
/// </param>
/// <param name="newPassword">
/// New password.
/// </param>
/// <param name="newDescription">
/// New description.
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
void MgSite::UpdateUser( CREFSTRING userID, CREFSTRING newUserID,
    CREFSTRING newUserName, CREFSTRING newPassword, CREFSTRING newDescription )
{
    CHECK_EMPTY_CREFSTRING(userID, L"MgSite.UpdateUser")

    MG_SITE_TRY()

    MgCommand cmd;
    STRING encryptedPassword;

    if (!newPassword.empty())
    {
        MgCryptographyManager cryptoManager;

        encryptedPassword = cryptoManager.EncryptPassword(newPassword);
    }

    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgSiteOpId::UpdateUser,                 // Command Code
                        5,                                      // No of arguments
                        Site_Admin,                             // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &userID,           // Argument#1
                        MgCommand::knString, &newUserID,        // Argument#2
                        MgCommand::knString, &newUserName,      // Argument#3
                        MgCommand::knString, &encryptedPassword, // Argument#4
                        MgCommand::knString, &newDescription,   // Argument#5
                        MgCommand::knNone );

    SetWarning(cmd.GetWarningObject());

    MG_SITE_CATCH_AND_THROW(L"MgSite.UpdateUser")
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
void MgSite::DeleteUsers(MgStringCollection* users)
{
    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                      // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgSiteOpId::DeleteUsers,                // Command Code
                        1,                                      // No of arguments
                        Site_Admin,                             // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knObject, users,             // Argument#1
                        MgCommand::knNone );

    SetWarning(cmd.GetWarningObject());

    MG_SITE_CATCH_AND_THROW( L"MgSite::DeleteUsers" )
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
void MgSite::GrantRoleMembershipsToUsers(MgStringCollection* roles, MgStringCollection* users)
{
    CHECKNULL      ( (MgStringCollection*) roles, L"MgSite::GrantRoleMembershipsToUsers" )
    CHECKNULL      ( (MgStringCollection*) users, L"MgSite::GrantRoleMembershipsToUsers" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                      // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgSiteOpId::GrantRoleMembershipsToUsers,// Command Code
                        2,                                      // No of arguments
                        Site_Admin,                             // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knObject, roles,             // Argument#1
                        MgCommand::knObject, users,             // Argument#2
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    MG_SITE_CATCH_AND_THROW( L"MgSite::GrantRoleMembershipsToUsers" )
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
void MgSite::RevokeRoleMembershipsFromUsers(MgStringCollection* roles, MgStringCollection* users)
{
    CHECKNULL      ( (MgStringCollection*) roles, L"MgSite::RevokeRoleMembershipsFromUsers" )
    CHECKNULL      ( (MgStringCollection*) users, L"MgSite::RevokeRoleMembershipsFromUsers" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                         // Connection
                        MgCommand::knVoid,                         // Return type expected
                        MgSiteOpId::RevokeRoleMembershipsFromUsers,// Command Code
                        2,                                         // No of arguments
                        Site_Admin,                                // Service Id
                        BUILD_VERSION(1,0,0),                      // Operation version
                        MgCommand::knObject, roles,                // Argument#1
                        MgCommand::knObject, users,                // Argument#2
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    MG_SITE_CATCH_AND_THROW( L"MgSite::RevokeRoleMembershipsFromUsers" )
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
void MgSite::GrantGroupMembershipsToUsers(MgStringCollection* groups, MgStringCollection* users)
{
    CHECKNULL      ( (MgStringCollection*) groups, L"MgSite::GrantGroupMembershipsToUsers" )
    CHECKNULL      ( (MgStringCollection*) users,  L"MgSite::GrantGroupMembershipsToUsers" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                       // Connection
                        MgCommand::knVoid,                       // Return type expected
                        MgSiteOpId::GrantGroupMembershipsToUsers,// Command Code
                        2,                                       // No of arguments
                        Site_Admin,                              // Service Id
                        BUILD_VERSION(1,0,0),                    // Operation version
                        MgCommand::knObject,  groups,            // Argument#1
                        MgCommand::knObject,  users,             // Argument#2
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    MG_SITE_CATCH_AND_THROW( L"MgSite::GrantGroupMembershipsToUsers" )
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
void MgSite::RevokeGroupMembershipsFromUsers(MgStringCollection* groups, MgStringCollection* users)
{
    CHECKNULL      ( (MgStringCollection*) groups, L"MgSite::RevokeGroupMembershipsFromUsers" )
    CHECKNULL      ( (MgStringCollection*) users,  L"MgSite::RevokeGroupMembershipsFromUsers" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                          // Connection
                        MgCommand::knVoid,                          // Return type expected
                        MgSiteOpId::RevokeGroupMembershipsFromUsers,// Command Code
                        2,                                          // No of arguments
                        Site_Admin,                                 // Service Id
                        BUILD_VERSION(1,0,0),                       // Operation version
                        MgCommand::knObject, groups,                // Argument#1
                        MgCommand::knObject, users,                 // Argument#2
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    MG_SITE_CATCH_AND_THROW( L"MgSite::RevokeGroupMembershipsFromUsers" )
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
MgByteReader* MgSite::EnumerateGroups( CREFSTRING user, CREFSTRING role )
{
    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                  // Connection
                        MgCommand::knObject,                // Return type expected
                        MgSiteOpId::EnumerateGroups,        // Command Code
                        2,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knString, &user,         // Argument#1
                        MgCommand::knString, &role,         // Argument#2
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    MG_SITE_CATCH_AND_THROW( L"MgSite::EnumerateGroups" )

    return (MgByteReader*) cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a list of groups for a user.
/// </summary>
///
/// <returns>
/// Result containing the list of groups.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
///
/// NOTE:
/// 1.  This method is part of the public API.
MgByteReader* MgSite::EnumerateGroups( CREFSTRING user )
{
    return this->EnumerateGroups( user, L"" );
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a list of available groups.
/// </summary>
///
/// <returns>
/// Result containing the list of groups.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
///
/// NOTE:
/// 1.  This method is part of the public API.
MgByteReader* MgSite::EnumerateGroups( )
{
    return this->EnumerateGroups( L"", L"" );
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Add a group to the Resource Repository.
/// TODO:
/// - Determine what roles are initially assigned to these new groups.
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
void MgSite::AddGroup( CREFSTRING group, CREFSTRING description )
{
    CHECK_EMPTY_CREFSTRING( group, L"MgSite::AddGroup" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                  // Connection
                        MgCommand::knVoid,                  // Return type expected
                        MgSiteOpId::AddGroup,               // Command Code
                        2,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knString, &group,        // Argument#1
                        MgCommand::knString, &description,  // Argument#2
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    MG_SITE_CATCH_AND_THROW( L"MgSite::AddGroup" )
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
/// New group name.
/// </param>
/// <param name="newDescription">
/// New description.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
/// MgDuplicateGroupException
void MgSite::UpdateGroup( CREFSTRING group, CREFSTRING newGroup,
    CREFSTRING newDescription )
{
    CHECK_EMPTY_CREFSTRING( group,    L"MgSite::UpdateGroup" )
    //CHECK_EMPTY_CREFSTRING( newGroup, L"MgSite::UpdateGroup" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                      // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgSiteOpId::UpdateGroup,                // Command Code
                        3,                                      // No of arguments
                        Site_Admin,                             // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knString, &group,            // Argument#1
                        MgCommand::knString, &newGroup,         // Argument#2
                        MgCommand::knString, &newDescription,   // Argument#3
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    MG_SITE_CATCH_AND_THROW( L"MgSite::UpdateGroup" )
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
void MgSite::DeleteGroups( MgStringCollection* groups )
{
    CHECKNULL      ( (MgStringCollection*) groups,  L"MgSite::DeleteGroups" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                      // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgSiteOpId::DeleteGroups,               // Command Code
                        1,                                      // No of arguments
                        Site_Admin,                             // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knObject, groups,            // Argument#1
                        MgCommand::knNone );

    SetWarning(cmd.GetWarningObject());

    MG_SITE_CATCH_AND_THROW( L"MgSite::DeleteGroups" )
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
void MgSite::GrantRoleMembershipsToGroups(MgStringCollection* roles, MgStringCollection* groups)
{
    CHECKNULL      ( (MgStringCollection*) roles,  L"MgSite::GrantRoleMembershipsToGroups" )
    CHECKNULL      ( (MgStringCollection*) groups, L"MgSite::GrantRoleMembershipsToGroups" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                      // Connection
                        MgCommand::knVoid,                      // Return type expected
                        MgSiteOpId::GrantRoleMembershipsToGroups,// Command Code
                        2,                                      // No of arguments
                        Site_Admin,                             // Service Id
                        BUILD_VERSION(1,0,0),                   // Operation version
                        MgCommand::knObject, roles,             // Argument#1
                        MgCommand::knObject, groups,            // Argument#2
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    MG_SITE_CATCH_AND_THROW( L"MgSite::GrantRoleMembershipsToGroups" )
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
void MgSite::RevokeRoleMembershipsFromGroups(MgStringCollection* roles, MgStringCollection* groups)
{
    CHECKNULL      ( (MgStringCollection*) roles,  L"MgSite::RevokeRoleMembershipsFromGroups" )
    CHECKNULL      ( (MgStringCollection*) groups, L"MgSite::RevokeRoleMembershipsFromGroups" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                          // Connection
                        MgCommand::knVoid,                          // Return type expected
                        MgSiteOpId::RevokeRoleMembershipsFromGroups,// Command Code
                        2,                                          // No of arguments
                        Site_Admin,                                 // Service Id
                        BUILD_VERSION(1,0,0),                       // Operation version
                        MgCommand::knObject, roles,                 // Argument#1
                        MgCommand::knObject, groups,                // Argument#2
                        MgCommand::knNone );

    SetWarning( cmd.GetWarningObject() );

    MG_SITE_CATCH_AND_THROW( L"MgSite::RevokeRoleMembershipsFromGroups" )
}

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
MgStringCollection* MgSite::EnumerateRoles( CREFSTRING user, CREFSTRING group )
{
    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                  // Connection
                        MgCommand::knObject,                // Return type expected
                        MgSiteOpId::EnumerateRoles,         // Command Code
                        2,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knString, &user,         // Argument#1
                        MgCommand::knString, &group,        // Argument#2
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    MG_SITE_CATCH_AND_THROW( L"MgSite::EnumerateRoles" )

    return (MgStringCollection*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a list of roles for a user.
/// </summary>
///
/// <returns>
/// Result containing the list of roles.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
///
/// NOTE:
/// 1.  This method is part of the public API.
MgStringCollection* MgSite::EnumerateRoles( CREFSTRING user )
{
    return this->EnumerateRoles( user, L"" );
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Return an XML document describing the server on the site.
/// </summary>
/// <returns>
/// XML document describing the list of servers.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
MgByteReader* MgSite::EnumerateServers()
{
    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                  // Connection
                        MgCommand::knObject,                // Return type expected
                        MgSiteOpId::EnumerateServers,       // Command Code
                        0,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    MG_SITE_CATCH_AND_THROW( L"MgSite::EnumerateServers" )

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Add server to the site.
/// </summary>
/// <param name="name">
/// Server name.  May be the same as the server DNS name.
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
void MgSite::AddServer(CREFSTRING name, CREFSTRING description, CREFSTRING address)
{
    CHECK_EMPTY_CREFSTRING( name,    L"MgSite::AddServer" )
    CHECK_EMPTY_CREFSTRING( address, L"MgSite::AddServer" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                  // Connection
                        MgCommand::knVoid,                  // Return type expected
                        MgSiteOpId::AddServer,              // Command Code
                        3,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knString, &name,         // Argument#1
                        MgCommand::knString, &description,  // Argument#2
                        MgCommand::knString, &address,      // Argument#3
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    MG_SITE_CATCH_AND_THROW( L"MgSite::AddServer" )
}

///////////////////////////////////////////////////////////////////////////////////
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
void MgSite::UpdateServer(CREFSTRING oldName, CREFSTRING newName, CREFSTRING newDescription, CREFSTRING newAddress)
{
    CHECK_EMPTY_CREFSTRING( oldName,    L"MgSite::UpdateServer" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                     // Connection
                        MgCommand::knVoid,                     // Return type expected
                        MgSiteOpId::UpdateServer,              // Command Code
                        4,                                     // No of arguments
                        Site_Admin,                            // Service Id
                        BUILD_VERSION(1,0,0),                  // Operation version
                        MgCommand::knString, &oldName,         // Argument#1
                        MgCommand::knString, &newName,         // Argument#1
                        MgCommand::knString, &newDescription,  // Argument#2
                        MgCommand::knString, &newAddress,      // Argument#3
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    MG_SITE_CATCH_AND_THROW( L"MgSite::UpdateServer" )
}

///////////////////////////////////////////////////////////////////////////////////
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
void MgSite::RemoveServer(CREFSTRING name)
{
    CHECK_EMPTY_CREFSTRING( name, L"MgSite::RemoveServer" )

    MgCommand cmd;

    MG_SITE_TRY()

    cmd.ExecuteCommand(m_connProp,                  // Connection
                        MgCommand::knVoid,                  // Return type expected
                        MgSiteOpId::RemoveServer,         // Command Code
                        1,                                  // No of arguments
                        Site_Admin,                         // Service Id
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knString, &name,         // Argument#1
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    MG_SITE_CATCH_AND_THROW( L"MgSite::RemoveServer" )
}

///////////////////////////////////////////////////////////////////////////////
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

STRING MgSite::RequestServer(INT32 serviceType)
{
    STRING serverAddress;

    MG_SITE_TRY()

    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                          // Connection
                        MgCommand::knString,                // Return type
                        MgSiteOpId::RequestServer,          // Command code
                        1,                                  // Number of arguments
                        Site_Admin,                         // Service ID
                        BUILD_VERSION(1,0,0),               // Operation version
                        MgCommand::knInt16, serviceType,    // Argument #1
                        MgCommand::knNone );

    SetWarning(cmd.GetWarningObject());

    serverAddress = *(cmd.GetReturnValue().val.m_str);
    delete cmd.GetReturnValue().val.m_str;

    MG_SITE_CATCH_AND_THROW(L"MgSite.RequestServer")

    return serverAddress;
}

MgStringCollection* MgSite::Authenticate(MgUserInformation* userInformation,
    MgSiteInfo* siteInfo, MgStringCollection* requiredRoles,
    bool returnAssignedRoles, bool skipAuthenticate)
{
    MgCommand cmd;
    Ptr<MgStringCollection> retval;

    MG_SITE_TRY()

    CHECKARGUMENTNULL((MgUserInformation*)userInformation, L"MgSite.Authenticate")

    assert(m_connProp == NULL);
    MgSiteManager* siteManager = MgSiteManager::GetInstance();
    if(siteInfo != NULL)
    {
        m_connProp = siteManager->GetConnectionProperties(userInformation,
            siteInfo, MgSiteInfo::Site);
    }
    else
    {
        m_connProp = siteManager->GetConnectionProperties(userInformation,
            MgSiteInfo::Site, true);
    }

    if (!skipAuthenticate)
    {
        cmd.ExecuteCommand(m_connProp,                          // Connection
                            MgCommand::knObject,                // Return type
                            MgSiteOpId::Authenticate,           // Command code
                            3,                                  // Number of arguments
                            Site_Admin,                         // Service ID
                            BUILD_VERSION(1,0,0),               // Operation version
                            MgCommand::knObject, userInformation, // Argument #1
                            MgCommand::knObject, requiredRoles, // Argument #2
                            MgCommand::knInt8, (int)returnAssignedRoles, // Argument #3
                            MgCommand::knNone);

        SetWarning(cmd.GetWarningObject());
        retval = (MgStringCollection*) cmd.GetReturnValue().val.m_obj;
    }

    MG_SITE_CATCH_AND_THROW(L"MgSite.Authenticate")

    return retval.Detach();
}

void MgSite::SetWarning(MgWarnings* warning)
{
    if (warning)
    {
        Ptr<MgWarnings> ptrWarning = warning;
        Ptr<MgStringCollection> ptrCol = ptrWarning->GetMessages();
        this->m_warning->AddMessages(ptrCol);
    }
}
