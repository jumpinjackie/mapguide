//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "ResourceServiceDefs.h"
#include "SiteRepositoryManager.h"
#include "SecurityManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgSiteRepositoryManager::MgSiteRepositoryManager(
    MgSiteRepository& repository) :
    MgSystemRepositoryManager(repository),
    m_resourceContentMan(NULL)
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan = new MgSiteResourceContentManager(*this);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.MgSiteRepositoryManager")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSiteRepositoryManager::~MgSiteRepositoryManager()
{
    delete m_resourceContentMan;
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a repository with the given content and header.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgSiteRepositoryManager::CreateRepository(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Let the base class do its things.

    MgRepositoryManager::CreateRepository(resource, content, header);

    //
    // Add users.
    //

    MgResourceIdentifier userRes(MgRepositoryType::Site, L"",
        MgResourceFolder::Users, MgUser::Administrator, MgResourceType::User);
    STRING fullName;
    STRING description;

    // Add Administrator user.

    fullName = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgUserFullNameAdministrator");
    description = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgUserDescriptionAdministrator");

    AddUser(userRes, fullName, L"admin", description);

    // Add Anonymous user.

    userRes.SetName(MgUser::Anonymous);
    fullName = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgUserFullNameAnonymous");
    description = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgUserDescriptionAnonymous");

    AddUser(userRes, fullName, L"", description);

    // Add Author user.

    userRes.SetName(MgUser::Author);
    fullName = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgUserFullNameAuthor");
    description = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgUserDescriptionAuthor");

    AddUser(userRes, fullName, L"author", description);

    // Add WFS user.

    userRes.SetName(MgUser::WfsUser);
    fullName = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgUserFullNameWfsUser");
    description = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgUserDescriptionWfsUser");

    AddUser(userRes, fullName, L"wfs", description);

    // Add WMS user.

    userRes.SetName(MgUser::WmsUser);
    fullName = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgUserFullNameWmsUser");
    description = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgUserDescriptionWmsUser");

    AddUser(userRes, fullName, L"wms", description);

    //
    // Add roles.
    //

    MgResourceIdentifier roleRes(MgRepositoryType::Site, L"",
        MgResourceFolder::Roles, MgRole::Administrator, MgResourceType::Role);
    MgStringCollection users, groups;

    // Add Administrator role.

    description = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgRoleDescriptionAdministrator");
    users.Add(MgUser::Administrator);

    AddRole(roleRes, description, users, groups);

    // Add Author role.

    roleRes.SetName(MgRole::Author);
    description = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgRoleDescriptionAuthor");
    users.Clear();
    users.Add(MgUser::Author);

    AddRole(roleRes, description, users, groups);

    // Add Viewer role.

    roleRes.SetName(MgRole::Viewer);
    description = MgUtil::GetResourceMessage(MgResources::SiteService,
        L"MgRoleDescriptionViewer");
    users.Clear();
    users.Add(MgUser::Anonymous);
    groups.Clear();
    groups.Add(MgGroup::Everyone);

    AddRole(roleRes, description, users, groups);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.CreateRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates users in the specified group or role. If no group and role are
/// specified, all available users will be returned.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgSiteRepositoryManager::EnumerateUsers(CREFSTRING group,
    CREFSTRING role, bool includePassword, bool includeGroups)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (!group.empty() && !role.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(group);
        arguments.Add(L"2");
        arguments.Add(role);

        throw new MgInvalidArgumentException(L"MgSiteRepositoryManager.EnumerateUsers",
            __LINE__, __WFILE__, &arguments, L"MgGroupAndRoleNotEmpty", NULL);
    }

    // Only allow the administrator to retrieve the user passwords.

    if (includePassword)
    {
        // TODO: Remove this option if it is not in use.

        MgSecurityManager securityMan;
        MgUserInformation* currUserInfo = MgUserInformation::GetCurrentUserInfo();
        assert(NULL != currUserInfo);
        STRING userName = currUserInfo->GetUserName();

        if (userName.empty())
        {
            STRING sessionId = currUserInfo->GetMgSessionId();

            if (!sessionId.empty())
            {
                userName = securityMan.GetUserName(sessionId);
            }
        }

        if (!securityMan.IsUserAnAdministrator(userName))
        {
            MG_LOG_AUTHENTICATION_ENTRY(MgResources::UnauthorizedAccess.c_str());

            throw new MgUnauthorizedAccessException(
                L"MgSiteRepositoryManager.EnumerateUsers",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    // Perform the operation.

    if (role.empty())
    {
        if (group.empty())
        {
            byteReader = m_resourceContentMan->EnumerateAllUsers(includePassword);
        }
        else
        {
            byteReader = m_resourceContentMan->EnumerateUsersInGroup(group,
                includePassword);
        }
    }
    else
    {
        byteReader = m_resourceContentMan->EnumerateUsersInRole(role,
            includePassword, includeGroups);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.EnumerateUsers")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a new user.
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::AddUser(CREFSTRING userId, CREFSTRING username,
    CREFSTRING password, CREFSTRING description)
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->AddUser(userId, username, password, description);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.AddUser")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes the given users in the MgStringCollection.  If the collection is
/// empty, all users are deleted.
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::DeleteUsers(MgStringCollection* users)
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->DeleteUsers(users);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.DeleteUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the user using the given parameters.  If a parameter is empty, that
/// user attribute is not changed (except for the newDescription parameter).
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::UpdateUser(CREFSTRING userId, CREFSTRING newUserId,
       CREFSTRING newUsername, CREFSTRING newPassword, CREFSTRING newDescription)
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->UpdateUser(userId, newUserId, newUsername,
        newPassword, newDescription);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.DeleteUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates groups that contain the given user and/or role.  If the user
/// and role parameters are empty, all groups are returned.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgSiteRepositoryManager::EnumerateGroups(CREFSTRING user,
    CREFSTRING role)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    if (!user.empty() && !role.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(user);
        arguments.Add(L"2");
        arguments.Add(role);

        throw new MgInvalidArgumentException(L"MgSiteRepositoryManager.EnumerateGroups",
            __LINE__, __WFILE__, &arguments, L"MgUserAndRoleNotEmpty", NULL);
    }

    if (role.empty())
    {
        if (user.empty())
        {
            byteReader = m_resourceContentMan->EnumerateAllGroups();
        }
        else
        {
            byteReader = m_resourceContentMan->EnumerateGroupsByUser(user);
        }
    }
    else
    {
        byteReader = m_resourceContentMan->EnumerateGroupsByRole(role);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.EnumerateGroups")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates groups that contain the given user and/or role.  If the user
/// and role parameters are empty, all groups are returned.
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::AddGroup(CREFSTRING group, CREFSTRING description)
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->AddGroup(group, description);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.AddGroup")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes the given groups in the MgStringCollection.  If the collection is
/// empty, all groups are deleted.
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::DeleteGroups(MgStringCollection* groups)
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->DeleteGroups(groups);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.DeleteGroups")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the group using the given parameters.  If a parameter is empty, that
/// group attribute is not changed (except for the newDescription parameter).
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::UpdateGroup(CREFSTRING group, CREFSTRING newGroup,
    CREFSTRING newDescription )
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->UpdateGroup(group, newGroup, newDescription);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.UpdateGroup")
}

///----------------------------------------------------------------------------
/// <summary>
/// Grants membership in the specified groups to the specified users.
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::GrantGroupMembershipsToUsers( MgStringCollection* groups,
    MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->GrantGroupMembershipsToUsers(groups, users);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.GrantGroupMembershipsToUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Revokes membership in the specified groups of the specified users.
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::RevokeGroupMembershipsFromUsers( MgStringCollection* groups,
    MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->RevokeGroupMembershipsFromUsers(groups, users);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.RevokeGroupMembershipsFromUsers")
}

///----------------------------------------------------------------------------
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
///
/// <returns>
/// Result containing the appropriate list of roles.
/// </returns>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgStringCollection* MgSiteRepositoryManager::EnumerateRoles(CREFSTRING user,
    CREFSTRING group)
{
    Ptr<MgStringCollection> roles;

    MG_RESOURCE_SERVICE_TRY()

    if (!user.empty() && !group.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(user);
        arguments.Add(L"2");
        arguments.Add(group);

        throw new MgInvalidArgumentException(L"MgSiteRepositoryManager.EnumerateRoles",
            __LINE__, __WFILE__, &arguments, L"MgUserAndGroupNotEmpty", NULL);
    }

    if (group.empty())
    {
        if (user.empty())
        {
            roles = m_resourceContentMan->EnumerateAllRoles();
        }
        else
        {
            roles = m_resourceContentMan->EnumerateRolesOfUser(user);
        }
    }
    else
    {
        roles = m_resourceContentMan->EnumerateRolesOfGroup(group);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.EnumerateRoles")

    return SAFE_ADDREF((MgStringCollection*)roles);
}

///----------------------------------------------------------------------------
/// <summary>
/// Grants the specified roles to the specified users.
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::GrantRoleMembershipsToUsers(
    MgStringCollection* roles, MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->GrantRoleMembershipsToUsers(roles, users);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.GrantRoleMembershipsToUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Revokes the specified roles from the specified users.
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::RevokeRoleMembershipsFromUsers(
    MgStringCollection* roles, MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->RevokeRoleMembershipsFromUsers(roles, users);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.RevokeRoleMembershipsFromUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Grants the specified role memberships to the specified groups.
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::GrantRoleMembershipsToGroups(
    MgStringCollection* roles, MgStringCollection* groups )
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->GrantRoleMembershipsToGroups(roles, groups);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.GrantRoleMembershipsToGroups")
}

///----------------------------------------------------------------------------
/// <summary>
/// Revokes the specified role memberships from the specified groups.
/// </summary>
///----------------------------------------------------------------------------
void MgSiteRepositoryManager::RevokeRoleMembershipsFromGroups(
    MgStringCollection* roles, MgStringCollection* groups )
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan->RevokeRoleMembershipsFromGroups(roles, groups);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepositoryManager.RevokeRoleMembershipsFromGroups")
}

///////////////////////////////////////////////////////////////////////////////
/// Add a user with the specified information.
///
void MgSiteRepositoryManager::AddUser(MgResourceIdentifier& resource,
    CREFSTRING fullName, CREFSTRING password, CREFSTRING description)
{
    MgCryptographyManager cryptoManager;
    string encryptedPassword;

    MgUtil::WideCharToMultiByte(cryptoManager.EncryptPassword(password),
        encryptedPassword);

    // TODO: Need an XML writer.
    // this XML follows the User-1.0.0.xsd schema
    string xmlContent = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xmlContent += "<User xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"User-1.0.0.xsd\">\n";
    xmlContent += "\t<FullName>";
    xmlContent += MgUtil::WideCharToMultiByte(fullName);
    xmlContent += "</FullName>\n";
    xmlContent += "\t<Password>";
    xmlContent += encryptedPassword;
    xmlContent += "</Password>\n";
    xmlContent += "\t<Description>";
    xmlContent += MgUtil::WideCharToMultiByte(description);
    xmlContent += "</Description>\n";
    xmlContent += "</User>";

    STRING mimeType = MgMimeType::Xml;
    Ptr<MgByteReader> byteReader = MgUtil::GetByteReader(xmlContent, &mimeType);

    AddResource(&resource, byteReader, NULL);
}

///////////////////////////////////////////////////////////////////////////////
/// Add a role with the specified information.
///
void MgSiteRepositoryManager::AddRole(MgResourceIdentifier& resource,
    CREFSTRING description, const MgStringCollection& users,
    const MgStringCollection& groups)
{
    // TODO: Need an XML writer.
    // this XML follows the Role-1.0.0.xsd schema
    string xmlContent = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xmlContent += "<Role xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"Role-1.0.0.xsd\">\n";
    xmlContent += "\t<Description>";
    xmlContent += MgUtil::WideCharToMultiByte(description);
    xmlContent += "</Description>\n";
    xmlContent += "\t<Users>\n";

    for (INT32 i = 0; i < users.GetCount(); ++i)
    {
        xmlContent += "\t\t<User>\n";
        xmlContent += "\t\t\t<Name>";
        xmlContent += MgUtil::WideCharToMultiByte(users.GetItem(i));
        xmlContent += "</Name>\n";
        xmlContent += "\t\t</User>\n";
    }

    xmlContent += "\t</Users>\n";
    xmlContent += "\t<Groups>\n";

    for (INT32 i = 0; i < groups.GetCount(); ++i)
    {
        xmlContent += "\t\t<Group>\n";
        xmlContent += "\t\t\t<Name>";
        xmlContent += MgUtil::WideCharToMultiByte(groups.GetItem(i));
        xmlContent += "</Name>\n";
        xmlContent += "\t\t</Group>\n";
    }

    xmlContent += "\t</Groups>\n";
    xmlContent += "</Role>";

    STRING mimeType = MgMimeType::Xml;
    Ptr<MgByteReader> byteReader = MgUtil::GetByteReader(xmlContent, &mimeType);

    AddResource(&resource, byteReader, NULL);
}
