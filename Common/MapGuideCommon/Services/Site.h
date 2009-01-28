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

#ifndef MG_SITE_H_
#define MG_SITE_H_

///////////////////////////////////////////////////////////////////////////////
/// MgSite try/catch/throw macros.
///
#define MG_SITE_TRY()                                                         \
    MG_TRY()                                                                  \

#define MG_SITE_CATCH(methodName)                                             \
    MG_CATCH(methodName)                                                      \

#define MG_SITE_THROW()                                                       \
    MG_THROW()                                                                \

#define MG_SITE_CATCH_AND_THROW(methodName)                                   \
    MG_CATCH_AND_THROW(methodName)                                            \

#define CHECK_SITE_CONN(methodName)                                           \
    if (NULL == this->m_pSiteConn)                                            \
        throw new MgConnectionNotOpenException(methodName, __LINE__, __WFILE__, NULL, L"", NULL); \

#define CHECK_EMPTY_CREFSTRING(str, methodName)                               \
    if (((CREFSTRING)str).empty())                                            \
    {                                                                         \
        MgStringCollection arguments;                                         \
        arguments.Add(L"1");                                                  \
        arguments.Add(MgResources::BlankArgument);                            \
        throw new MgInvalidArgumentException(methodName, __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL); \
    }                                                                         \

/// \defgroup MgSite MgSite
/// \ingroup Site_Service_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// The Site Service contains one class, MgSite.
/// \remarks
/// The MgSite class
/// contains methods which allow access to basic site-related
/// functionality:
///   <ul>
///       <li>Enumerate users
///       </li>
///       <li>Enumerate groups
///       </li>
///       <li>Create and destroy user sessions
///       </li>
///   </ul>
///
class MG_MAPGUIDE_API MgSite : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgSite)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of users.
    ///
    /// \remarks
    /// If a group is specified, this returns a list of the users in
    /// that group. If a group is not specified, it returns a list of
    /// all users.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader EnumerateUsers(string group);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader EnumerateUsers(String group);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader EnumerateUsers(string group);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param group (String/string)
    /// The name of a group, for example "Engineering". Or,
    /// an empty string to return a list of all users.
    ///
    /// \return
    /// Returns an MgByteReader object listing the users in XML
    /// format using the MgUserList schema.
    ///
    /// <!-- Examples (PHP) -->
    /// \htmlinclude PHPExamplesTop.html
    /// This returns a list of all users in the group "Engineering":
    ///
    /// \code
    /// $byteReader = $site->EnumerateUsers("Engineering");
    /// \endcode
    ///
    ///
    /// This returns a list of all users:
    ///
    /// \code
    /// $byteReader = $site->EnumerateUsers("");
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgGroupNotFoundException
    ///
    ///
    MgByteReader* EnumerateUsers( CREFSTRING group );

    /////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of available groups.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader EnumerateGroups();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader EnumerateGroups();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader EnumerateGroups();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgByteReader object listing the groups in XML
    /// format using the \link GroupList_schema GroupList \endlink schema.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// $byteReader = $site->EnumerateGroups();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgConnectionNotOpenException
    ///
    ///
    MgByteReader* EnumerateGroups( );

    /////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a \link session_repository session repository \endlink
    /// for the current user.
    ///
    /// \remarks
    /// Resources placed into the session repository will exist until
    /// the session expires or the session is destroyed by using
    /// DestroySession. The session repository is tied exclusively to
    /// the user, and can be used, for example, to store temporary
    /// layers.
    /// \n
    /// You can use the session identifier to tie a web tier
    /// application session to the stored data. You
    /// can use standard web tier session mechanisms like client side
    /// cookies, hidden form fields, and additional URL parameters on
    /// dynamically generated web pages to propagate the identifier
    /// from one page to another.
    /// \n
    /// For the lifetime of the session, the session identifier can
    /// also be used to reauthenticate with the MapGuide site.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string CreateSession();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String CreateSession();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string CreateSession();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The session identifier, for example
    /// "71b5d990-0000-1000-8000-005056c00008_eng".
    ///
    /// <!-- Examples (PHP) -->
    /// \htmlinclude PHPExamplesTop.html
    /// This example shows how to create a session repository. Note how the session ID
    /// is stored back in the MgUserInformation object. This is necessary for GetCurrentSession to work.
    /// \code
    /// // Initialize the web tier
    /// MgInitializeWebTier('C:\Inetpub\wwwroot\PhpMapAgent\webconfig.ini');
    ///
    /// // Establish a connection to a site.
    /// $userInformation = new MgUserInformation('Administrator', 'admin');
    /// $siteConnection = new MgSiteConnection();
    /// $siteConnection->Open($userInformation);
    ///
    /// // Create a session repository
    /// $site = $siteConnection->GetSite();
    /// $sessionID = $site->CreateSession();
    /// $userInformation->SetMgSessionId($sessionID);
    /// \endcode
    ///
    /// This example shows how to reauthenticate using the session ID:
    /// \code
    /// // Assuming this page was called by
    /// // <A href="page_two.php?sessionId=<?= $site->GetCurrentSession(); ?>">Page Two</A>
    /// $userInformation = new MgUserInformation($_GET['sessionId']);
    /// \endcode
    ///
    /// This example shows how to enumerate the resources in the session repository:
    /// \code
    /// $sessionName = "Session:$sessionID//";
    /// $resourceID = new MgResourceIdentifier($sessionName);
    /// $byteReader = $resourceService->EnumerateResources($resourceID, -1, "");
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception [[To be documented]]
    ///
    /// \todo
    ///   [[What are the exceptions?]]
    ///
    /// \see
    /// DestroySession
    /// \see
    /// GetCurrentSession
    ///
    STRING CreateSession();

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys the specified session and discards any resources
    /// stored in the session repository.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void DestroySession(string session);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void DestroySession(String session);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void DestroySession(string session);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param session (String/string)
    /// Identifier returned from CreateSession.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// $site->DestroySession($site->GetCurrentSession());
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception [[To be documented]]
    ///
    /// \todo
    ///   [[What are the exceptions?]]
    ///
    /// \see
    /// CreateSession
    /// \see
    /// GetCurrentSession
    ///
    void DestroySession(CREFSTRING session);

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the current session identifier.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetCurrentSession();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetCurrentSession();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetCurrentSession();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// If a session repository has been created, it returns the
    /// unique session identifier, for example
    /// "71b5d990-0000-1000-8000-005056c00008_eng". If a session
    /// repository has not been created, it returns an empty string.
    ///
    /// \exception [[To be documented]]
    ///
    /// \todo
    ///   * [[What are the exceptions?]]
    ///
    /// \see
    /// CreateSession
    /// \see
    /// DestroySession
    ///
    STRING GetCurrentSession();

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the user for the current session identifier.  An exception will
    /// be thrown if the session is invalid or does not exist.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetUserForSession();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetUserForSession();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetUserForSession();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// If a session has been created, it returns the
    /// userid for the user.
    ///
    /// \exception MgInvalidArgumentException
    ///
    ///
    /// \see
    /// GetCurrentSession
    /// \see
    /// CreateSession
    ///
    STRING GetUserForSession();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the list of group memberships for a user.
    ///
    /// \return
    /// Result containing the appropriate list of groups.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgUserNotFoundException
    ///
    MgByteReader* EnumerateGroups( CREFSTRING user );

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of roles for a user.
    ///
    /// \return
    /// Result containing the list of roles.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgUserNotFoundException
    ///
    MgStringCollection* EnumerateRoles( CREFSTRING user );

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgSite object and open a connection to the Site Server.
    ///
    /// \return
    /// Nothing
    ///
    MgSite();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Opens a connection to the Site Server.
    ///
    /// \param userInformation
    /// User information to authenticate against
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgNullArgumentException
    /// \exception MgAuthenticationFailedException
    /// \exception MgConnectionFailedException
    ///
    void Open(MgUserInformation* userInformation);
    void Open(MgUserInformation* userInformation, MgSiteInfo* siteInfo);
    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Closes the current Site Server connection.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    ///
    void Close();

    /// \brief
    /// HasWarning() method reports if there is a current warning.
    ///
    /// \return
    /// Returns true or false
    ///
    /// \exception MgConnectionNotOpenException
    ///
    bool HasWarning();

    /// \brief
    /// GetWarning() method returns all the current warning.
    ///
    /// \return
    /// Returns a pointer to the MgWarnings object.
    ///
    /// \exception MgConnectionNotOpenException
    ///
    MgWarnings* GetWarning();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of available users.  Parameters specify the contents of the list:
    /// <ul>
    ///     <li>If both group and role are NOT specified return a list of all available users.
    ///     </li>
    ///     <li>If group is specified, return a list of users in that group.
    ///     </li>
    ///     <li>If role is specified and includeGroups is false, return a list of users
    ///            that role.
    ///     </li>
    ///     <li>If role is specified and includeGroups is true, return a list of users and
    ///            groups in that role (the list of users in each group is not expanded).
    ///     </li>
    ///     <li>If both group and role are specified, throw exception.
    ///     </li>
    /// </ul>
    ///
    /// \param group
    /// Name of group, if returning the list of users in a group; empty, otherwise.
    /// \param role
    /// Role, if returning a list of users and plus, optionally, groups with a
    /// particular role; empty, otherwise.
    /// \param includeGroups
    /// Flag used when role is specified; True, indicates that groups with the specified
    /// role should be returned in the list along with the users; False, indicates that
    /// groups should not be included.
    ///
    /// \return
    /// Result containing the appropriate list of users.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgGroupNotFoundException
    /// \exception MgRoleNotFoundException
    ///
    MgByteReader* EnumerateUsers(CREFSTRING group, CREFSTRING role, bool includeGroups);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Add a user to the Resource Repository.
    ///
    /// \param userID
    /// ID of the new user.
    /// \param userName
    /// Name of the new user.
    /// \param password
    /// New user's password.
    /// \param description
    /// Description of the new user.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgDuplicateUserException
    /// \exception MgInvalidPasswordException
    ///
    void AddUser(CREFSTRING userID, CREFSTRING userName, CREFSTRING password, CREFSTRING description);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Updates the information stored for a user.
    ///
    /// \param userID
    /// ID of the user to be updated.
    /// \param newUserID
    /// New user ID.
    /// \param newUserName
    /// New user name.
    /// \param newPassword
    /// New password.
    /// \param newDescription
    /// New description.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgUserNotFoundException
    /// \exception MgDuplicateUserException
    /// \exception MgInvalidPasswordException
    ///
    void UpdateUser(CREFSTRING userID, CREFSTRING newUserID, CREFSTRING newUserName, CREFSTRING newPassword, CREFSTRING newDescription);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deletes a set of users from the server.  Prior to deletion, all of the users'
    /// group memberships are revoked.
    ///
    /// \note
    /// If users is empty, then all users will be deleted.
    ///
    /// \param users
    /// IDs of the users to be deleted.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgUserNotFoundException
    ///
    void DeleteUsers(MgStringCollection* users);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Grants the specified roles to the specified users.
    ///
    /// \param roles
    /// List of roles to grant to the specified users.
    /// \param users
    /// IDs of users to receive the specified roles.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgUserNotFoundException
    /// \exception MgRoleNotFoundException
    ///
    void GrantRoleMembershipsToUsers(MgStringCollection* roles, MgStringCollection* users);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Revokes the specified roles from the specified users.
    ///
    /// \param roles
    /// List of roles to revoke from the specified users.
    /// Repository.
    /// \param users
    /// IDs of users to lose the specified roles.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgUserNotFoundException
    /// \exception MgRoleNotFoundException
    ///
    void RevokeRoleMembershipsFromUsers(MgStringCollection* roles, MgStringCollection* users);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Grants membership in the specified groups to the specified users.
    ///
    /// \param groups
    /// Groups to receive the specified users.
    /// \param users
    /// IDs of users to gain membership in the specified groups.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgUserNotFoundException
    /// \exception MgGroupNotFoundException
    ///
    void GrantGroupMembershipsToUsers(MgStringCollection* groups, MgStringCollection* users);
    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Revokes membership in the specified groups from the specified users.
    ///
    /// \param groups
    /// Groups to lose the specified users.
    /// \param users
    /// IDs of users to lose membership in the specified groups.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgUserNotFoundException
    /// \exception MgGroupNotFoundException
    ///
    void RevokeGroupMembershipsFromUsers(MgStringCollection* groups, MgStringCollection* users);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of available groups.  Parameters specify the contents of the list:
    /// <ul>
    ///     <li>If both user and role are NOT specified, return a list of all available groups.
    ///     </li>
    ///     <li>If user is specified, return a list of groups of which that user is a member.
    ///     </li>
    ///     <li>If role is specified, return a list of groups in that role.
    ///     </li>
    ///     <li>If both user and role are specified, throw exception.
    ///     </li>
    /// </ul>
    ///
    /// \param user
    /// ID of user, if returning a list of groups owning a particular user; empty,
    /// otherwise.
    /// \param role
    /// Role, if returning a list of groups with a particular role; empty, otherwise.
    ///
    /// \return
    /// Result containing the appropriate list of groups.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgUserNotFoundException
    /// \exception MgRoleNotFoundException
    ///
    MgByteReader* EnumerateGroups( CREFSTRING user, CREFSTRING role );

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Add a group to the Resource Repository.
    /// TODO:
    /// - Determine what roles are initially assigned to these new groups.
    ///
    /// \param group
    /// Name of the new group.
    /// \param description
    /// Description of the new group.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgDuplicateGroupException
    ///
    void AddGroup(CREFSTRING group, CREFSTRING description);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Update the information stored for a group.
    ///
    /// \param group
    /// Name of the group to be updated.
    /// \param newGroup
    /// New group name.
    /// \param newDescription
    /// New description.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgGroupNotFoundException
    /// \exception MgDuplicateGroupException
    ///
    void UpdateGroup(CREFSTRING group, CREFSTRING newGroup, CREFSTRING newDescription);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deletes a set of groups from the server.  All users owned by the groups will
    /// have their membership revoked prior to deletion of the groups.
    ///
    /// \param groups
    /// Names of the groups to be deleted.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgGroupNotFoundException
    ///
    void DeleteGroups(MgStringCollection* groups);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Grants the specified role memberships to the specified groups.
    ///
    /// \param roles
    /// List of roles to be granted to the specified groups.
    /// \param groups
    /// Groups to receive the specified roles.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgGroupNotFoundException
    /// \exception MgRoleNotFoundException
    ///
    void GrantRoleMembershipsToGroups(MgStringCollection* roles, MgStringCollection* groups);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Revokes the specified roles from the specified groups.
    ///
    /// \param roles
    /// Roles to be revoked from the groups.
    /// \param groups
    /// Groups to lose the specified roles.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgGroupNotFoundException
    /// \exception MgRoleNotFoundException
    ///
    void RevokeRoleMembershipsFromGroups(MgStringCollection* roles, MgStringCollection* groups);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of available roles.  Parameters specify contents of the list:
    /// <ul>
    ///     <li>If both user and group are NOT specified, return a list of all available roles.
    ///     </li>
    ///     <li>If group is specified, return a list of roles for that group.
    ///     </li>
    ///     <li>If user is specified, return a list of roles for that user.
    ///     </li>
    ///     <li>If both user and group are specified, throw an exception.
    ///     </li>
    /// </ul>
    ///
    /// \param user
    /// ID of user, if returning list of roles for a particular user; empty, otherwise.
    /// \param group
    /// Name of group, if returning list of roles for a particular group; empty, otherwise.
    ///
    /// \return
    /// Result containing the appropriate list of roles.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgGroupNotFoundException
    /// \exception MgUserNotFoundException
    ///
    MgStringCollection* EnumerateRoles( CREFSTRING user, CREFSTRING group );

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Return an XML document describing the server on the site.
    ///
    /// \return
    /// XML document describing the list of servers.
    ///
    /// \exception MgConnectionNotOpenException
    ///
    MgByteReader* EnumerateServers();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Add server to the site.
    ///
    /// \param name
    /// Server name.  May be the same as the server DNS name.
    /// \param description
    /// Server long description.  May indicate the server's intended use.
    /// \param address
    /// Server IP address or DNS name.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgServerAlreadyExistsException
    /// \exception MgInvalidServerAddressException
    ///
    void AddServer(CREFSTRING name, CREFSTRING description, CREFSTRING address);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Update server information for the site.
    ///
    /// \param oldName
    /// Current server name.
    /// \param newName
    /// New server name.
    /// \param newDescription
    /// New long description.
    /// \param newAddress
    /// New server IP address or DNS name.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgServerNotFoundException
    /// \exception MgServerAlreadyExistsException
    /// \exception MgInvalidServerAddressException
    ///
    void UpdateServer(CREFSTRING oldName, CREFSTRING newName, CREFSTRING newDescription, CREFSTRING newAddress);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Remove server from the site.
    ///
    /// \param name
    /// Name of server to remove from site.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgServerNotFoundException
    ///
    void RemoveServer(CREFSTRING name);

    MgSiteInfo* GetSiteServerInfo(INT32 index);
    INT32 GetSiteServerCount();
    STRING GetCurrentSiteAddress();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the IP address of the next server available for the specified
    /// service.
    ///
    /// \param serviceType
    /// Type of the service.
    ///
    /// \return
    /// IP address of the server.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgInvalidArgumentException
    ///
    STRING RequestServer(INT32 serviceType);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Authenticates a user and validates his/her roles.
    ///
    /// \param userInformation
    /// User information to authenticate against.
    /// \param siteInfo
    /// Used to specify which site server to connect to. If NULL, the next
    /// available site server is used.
    /// \param requiredRoles
    /// Required roles to validate against.
    /// \param returnAssignedRoles
    /// Flag to determine whether or not the assigned roles of the user should
    /// be returned.
    ///
    /// \return
    /// Assigned roles of the user (maybe NULL).
    ///
    /// \exception MgNullArgumentException
    /// \exception MgAuthenticationFailedException
    /// \exception MgUnauthorizedAccessException
    ///
    MgStringCollection* Authenticate(MgUserInformation* userInformation,
        MgSiteInfo* siteInfo,
        MgStringCollection* requiredRoles,
        bool returnAssignedRoles,
        bool skipAuthenticate = false);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Opens a connection to the Site Server.
    ///
    /// \param userInformation
    /// User information to authenticate against
    ///
    /// \param skipAuthenticate
    /// Skip authentication.  Used for internal performance speedups.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgNullArgumentException
    /// \exception MgAuthenticationFailedException
    /// \exception MgConnectionFailedException
    ///
    void Open(MgUserInformation* userInformation, bool skipAuthenticate);

 protected:

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();


CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Service_Site;

private:
    Ptr<MgConnectionProperties>  m_connProp;
    Ptr<MgWarnings>              m_warning;

    void SetWarning(MgWarnings* warning);
};
/// \}

#endif // MGSITE_H_
