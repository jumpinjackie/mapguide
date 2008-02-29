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
#include "SiteResourceContentManager.h"
#include "SiteRepositoryManager.h"
#include "SiteServiceDefs.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgSiteResourceContentManager::MgSiteResourceContentManager(
    MgSiteRepositoryManager& repositoryMan) :
    MgSystemResourceContentManager(repositoryMan)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSiteResourceContentManager::~MgSiteResourceContentManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Appends a given user document to the specified user list.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgSiteResourceContentManager::WriteUserList(const XmlDocument& userDoc,
    bool includePassword, string& list)
{
    MG_RESOURCE_SERVICE_TRY()

    MgResourceIdentifier userRes(MgUtil::MultiByteToWideChar(
        userDoc.getName()));
    string xmlContent;
    MgXmlUtil xmlUtil(userDoc.getContent(xmlContent));
    DOMElement* node = xmlUtil.GetRootNode();
    wstring elementValue;

    // TODO: Need an XML writer.
    list += "\t<User>\n";

    list += "\t\t<Name>";
    list += MgUtil::WideCharToMultiByte(userRes.GetName());
    list += "</Name>\n";

    xmlUtil.GetElementValue(node, ELEMENT_USER_FULLNAME, elementValue);
    list += "\t\t<FullName>";
    list += MgUtil::WideCharToMultiByte(elementValue).c_str();
    list += "</FullName>\n";

    if (includePassword)
    {
        xmlUtil.GetElementValue(node, ELEMENT_USER_PASSWORD, elementValue);
        list += "\t\t<Password>";
        list += MgUtil::WideCharToMultiByte(elementValue).c_str();
        list += "</Password>\n";
    }

    xmlUtil.GetElementValue(node, ELEMENT_USER_DESCRIPTION, elementValue, false);
    list += "\t\t<Description>";
    list += MgUtil::WideCharToMultiByte(elementValue).c_str();
    list += "</Description>\n";

    list += "\t</User>\n";

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.WriteUserList")
}

///----------------------------------------------------------------------------
/// <summary>
/// Appends a given group document to the specified user or group list.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgSiteResourceContentManager::WriteGroupList(const XmlDocument* groupDoc,
    string& list)
{
    MG_RESOURCE_SERVICE_TRY()

    wstring name, description;

    if (NULL == groupDoc)
    {
        // Note that we do not maintain the Everyone group document in the
        // repository at all. So, just write out the following information.

        name = MgGroup::Everyone;
        description = MgUtil::GetResourceMessage(MgResources::SiteService,
            L"MgGroupDescriptionEveryone");
    }
    else
    {
        MgResourceIdentifier groupRes(MgUtil::MultiByteToWideChar(
            groupDoc->getName()));
        string xmlContent;
        MgXmlUtil xmlUtil(groupDoc->getContent(xmlContent));
        DOMElement* node = xmlUtil.GetRootNode();

        name = groupRes.GetName();
        xmlUtil.GetElementValue(node, ELEMENT_GROUP_DESCRIPTION, description, false);
    }

    // TODO: Need an XML writer.
    list += "\t<Group>\n";

    list += "\t\t<Name>";
    list += MgUtil::WideCharToMultiByte(name).c_str();
    list += "</Name>\n";

    list += "\t\t<Description>";
    list += MgUtil::WideCharToMultiByte(description).c_str();
    list += "</Description>\n";

    list += "\t</Group>\n";

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.WriteGroupList")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates all available users.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgSiteResourceContentManager::EnumerateAllUsers(
    bool includePassword)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MgResourceIdentifier userFolder(MgRepositoryType::Site, L"",
        L"", MgResourceFolder::Users, MgResourceType::Folder);

    string userFolderPathname;
    MgUtil::WideCharToMultiByte(userFolder.ToString(), userFolderPathname);

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
    query += userFolderPathname;
    query += "')]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Get the user documents and populate the user list.

    // this XML follows the UserList-1.0.0.xsd schema
    XmlValue xmlValue;
    string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    list += "<UserList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"UserList-1.0.0.xsd\">\n";

    while (results.next(xmlValue)) // TODO: Need an XML writer
    {
        const XmlDocument& userDoc = xmlValue.asDocument();

        WriteUserList(userDoc, includePassword, list);
    }

    list += "</UserList>";

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.EnumerateAllUsers")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates users in the specified group.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgSiteResourceContentManager::EnumerateUsersInGroup(
    CREFSTRING group, bool includePassword)
{
    assert(!group.empty());

    // For the Everyone group, return all the users.

    if (MgGroup::Everyone == group)
    {
        return EnumerateAllUsers(includePassword);
    }

    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Create a Group resource identifier.

    MgResourceIdentifier groupRes(MgRepositoryType::Site, L"",
        MgResourceFolder::Groups, group, MgResourceType::Group);

    string groupDocPathname;
    MgUtil::WideCharToMultiByte(groupRes.ToString(), groupDocPathname);

    // Set up an XQuery.

    string colQuery = "collection('";
    colQuery += m_container.getName();
    colQuery += "')";
    colQuery += "/*[dbxml:metadata('dbxml:name')='";
    colQuery += groupDocPathname;
    colQuery += "']";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext colContext = xmlMan.createQueryContext();

    XmlResults docResults = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), colQuery, colContext).
            execute(GetXmlTxn(), colContext, 0) :
        xmlMan.prepare(colQuery, colContext).
            execute(colContext, 0);

    // Get the group document and populate the user list.

    // this XML follows the UserList-1.0.0.xsd schema
    XmlValue docValue;
    string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    list += "<UserList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"UserList-1.0.0.xsd\">\n";

    if (docResults.next(docValue)) // TODO: Need an XML writer
    {
        assert(1 == docResults.size());

        // Set up an XQuery.

        string docQuery = "/Group/Users/User/Name/text()";
        XmlQueryContext docContext = xmlMan.createQueryContext();
        docContext.setReturnType(XmlQueryContext::DeadValues);

        // Execute the XQuery.

        XmlResults nodeResults = IsTransacted() ?
            xmlMan.prepare(GetXmlTxn(), docQuery, docContext).
                execute(GetXmlTxn(), docValue, docContext, 0) :
            xmlMan.prepare(docQuery, docContext).
                execute(docValue, docContext, 0);

        // Get the user document and populate the user list.

        XmlValue nodeValue;

        while (nodeResults.next(nodeValue))
        {
            MgResourceIdentifier userRes(MgRepositoryType::Site, L"",
                MgResourceFolder::Users,
                MgUtil::MultiByteToWideChar(nodeValue.asString()),
                MgResourceType::User);
            XmlDocument userDoc = GetDocument(userRes);

            WriteUserList(userDoc, includePassword, list);
        }
    }

    list += "</UserList>";

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.EnumerateUsersInGroup")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates users in the specified role.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgSiteResourceContentManager::EnumerateUsersInRole(
    CREFSTRING role, bool includePassword, bool includeGroups)
{
    assert(!role.empty());
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Create a Role resource identifier.

    MgResourceIdentifier roleRes(MgRepositoryType::Site, L"",
        MgResourceFolder::Roles, role, MgResourceType::Role);

    string roleDocPathname;
    MgUtil::WideCharToMultiByte(roleRes.ToString(), roleDocPathname);

    // Set up an XQuery.

    string colQuery = "collection('";
    colQuery += m_container.getName();
    colQuery += "')";
    colQuery += "/*[dbxml:metadata('dbxml:name')='";
    colQuery += roleDocPathname;
    colQuery += "']";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext colContext = xmlMan.createQueryContext();

    XmlResults docResults = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), colQuery, colContext).
            execute(GetXmlTxn(), colContext, 0) :
        xmlMan.prepare(colQuery, colContext).
            execute(colContext, 0);

    // Get the role document and populate the user list.

    // this XML follows the UserList-1.0.0.xsd schema
    XmlValue docValue;
    string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    list += "<UserList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"UserList-1.0.0.xsd\">\n";

    if (docResults.next(docValue)) // TODO: Need an XML writer
    {
        assert(1 == docResults.size());

        // Set up an XQuery.

        string docQuery = "/Role/Users/User/Name/text()";
        XmlQueryContext docContext = xmlMan.createQueryContext();
        docContext.setReturnType(XmlQueryContext::DeadValues);

        // Execute the XQuery.

        XmlResults nodeResults = IsTransacted() ?
            xmlMan.prepare(GetXmlTxn(), docQuery, docContext).
                execute(GetXmlTxn(), docValue, docContext, 0) :
            xmlMan.prepare(docQuery, docContext).
                execute(docValue, docContext, 0);

        // Get the user document and populate the user list.

        XmlValue nodeValue;

        while (nodeResults.next(nodeValue))
        {
            MgResourceIdentifier userRes(MgRepositoryType::Site, L"",
                MgResourceFolder::Users,
                MgUtil::MultiByteToWideChar(nodeValue.asString()),
                MgResourceType::User);
            XmlDocument userDoc = GetDocument(userRes);

            WriteUserList(userDoc, includePassword, list);
        }

        if (includeGroups)
        {
            // Set up an XQuery.

            docQuery = "/Role/Groups/Group/Name/text()";

            // Execute the XQuery.

            nodeResults = IsTransacted() ?
                xmlMan.prepare(GetXmlTxn(), docQuery, docContext).
                    execute(GetXmlTxn(), docValue, docContext, 0) :
                xmlMan.prepare(docQuery, docContext).
                    execute(docValue, docContext, 0);

            // Get the group document and populate the user list.

            while (nodeResults.next(nodeValue))
            {
                MgResourceIdentifier groupRes(MgRepositoryType::Site, L"",
                    MgResourceFolder::Groups,
                    MgUtil::MultiByteToWideChar(nodeValue.asString()),
                    MgResourceType::Group);
                XmlDocument groupDoc = GetDocument(groupRes);

                WriteGroupList(&groupDoc, list);
            }
        }
    }

    list += "</UserList>";

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.EnumerateUsersInRole")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a new user,
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgDuplicateUserException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::AddUser(CREFSTRING userId,
    CREFSTRING username, CREFSTRING password, CREFSTRING description)
{
    MG_RESOURCE_SERVICE_TRY()

    CHECK_EMPTY_CREFSTRING( userId,   L"MgSiteResourceContentManager.AddUser" );
    CHECK_EMPTY_CREFSTRING( username, L"MgSiteResourceContentManager.AddUser" );
    CHECK_EMPTY_CREFSTRING( password, L"MgSiteResourceContentManager.AddUser" );

    //  get user resource id
    MgResourceIdentifier userRes( MgRepositoryType::Site, L"", MgResourceFolder::Users,
        userId, MgResourceType::User );
    MgResourceInfo resInfo( userRes );

    //  create user xml
    MgXmlUtil xmlUtil(ELEMENT_USER);

    DOMElement* root = xmlUtil.GetRootNode();

    xmlUtil.SetAttribute( root, "xmlns:xsi",
        L"http://www.w3.org/2001/XMLSchema-instance" );
    xmlUtil.SetAttribute( root, "xsi:noNamespaceSchemaLocation",
        L"User-1.0.0.xsd" );

    MgCryptographyManager cryptoManager;
    STRING encryptedPassword = cryptoManager.EncryptPassword(password);

    xmlUtil.AddTextNode( root, ELEMENT_USER_FULLNAME, username.c_str() );
    xmlUtil.AddTextNode( root, ELEMENT_USER_PASSWORD, encryptedPassword.c_str() );
    xmlUtil.AddTextNode( root, ELEMENT_USER_DESCRIPTION, description.c_str() );

    string xmlContent;
    xmlUtil.ToStringUtf8(xmlContent);

    //  add user
    AddResource(resInfo, xmlContent);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.AddUser" )
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes the given users in the MgStringCollection.  If the collection is
/// empty, all users are deleted.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException, MgUserNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::DeleteUsers(MgStringCollection* users)
{
    MG_RESOURCE_SERVICE_TRY()

    CHECKNULL(users, L"MgSiteResourceContentManager.DeleteUsers");

    //  iterate through user collection
    INT32 numUsers = users->GetCount();

    if (0 == numUsers)
    {
        return;
    }

    for (int i = 0; i < numUsers; ++i)
    {
        STRING id = users->GetItem(i);

        // Do not allow deleting system user accounts.

        if (MgSecurityManager::IsSystemUser(id))
        {
            throw new MgInvalidOperationException(
                L"MgSiteResourceContentManager.DeleteUsers",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //  get user resource id
        MgResourceIdentifier resUser( MgRepositoryType::Site, L"", MgResourceFolder::Users,
            id, MgResourceType::User );

        //  check for userId in database
        if (!FindResource(resUser.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(id);

            throw new MgUserNotFoundException(
                L"MgSiteResourceContentManager.DeleteUsers",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        //  delete group memberships
        Ptr<MgStringCollection> groupCollection = this->RemoveUserFromAllGroups( id );

        //  delete role memberships
        Ptr<MgStringCollection> roleCollection = this->RemoveUserFromAllRoles( id );

        //  delete the user
        this->DeleteResource( &resUser);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.DeleteUsers" )
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the user using the given parameters.  If a parameter is empty, that
/// user attribute is not changed (except for the newDescription parameter).
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgDuplicateUserException, MgUserNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::UpdateUser( CREFSTRING userId,
    CREFSTRING newUserId, CREFSTRING newUsername, CREFSTRING newPassword,
    CREFSTRING newDescription )
{
    MG_RESOURCE_SERVICE_TRY()

    CHECK_EMPTY_CREFSTRING( userId,   L"MgSiteResourceContentManager.UpdateUser" );

    // get user resource id
    MgResourceIdentifier userRes(MgRepositoryType::Site, L"",
        MgResourceFolder::Users, userId, MgResourceType::User);

    //  check for newUserId and update if necessary
    if (!newUserId.empty())
    {
        // Disallow changing the system user IDs.
        if (MgSecurityManager::IsSystemUser(userId))
        {
            throw new MgInvalidOperationException(
                L"MgSiteResourceContentManager.UpdateUser",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        MgResourceIdentifier newUserRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Users, newUserId, MgResourceType::User);

        //  rename resource
        this->MoveResource(&userRes, &newUserRes, false);

        userRes = newUserRes;
    }

    // Disallow changing the Anonymous user's blank password.
    if (!newPassword.empty() && MgUser::Anonymous == userId)
    {
        throw new MgInvalidOperationException(
            L"MgSiteResourceContentManager.UpdateUser",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //  get user xml
    Ptr<MgByteReader> byteReader = this->GetResource(&userRes);

    //  parse user xml
    string xmlContent;
    byteReader->ToStringUtf8(xmlContent);
    MgXmlUtil xmlUtil(xmlContent);
    DOMDocument* doc = xmlUtil.GetDocument();
    DOMElement* root = doc->getDocumentElement();

    //  update user id
    if (!newUserId.empty())
    {
        //  update groups
        Ptr<MgStringCollection> userCollection = new MgStringCollection();
        userCollection->Add( newUserId );

        Ptr<MgStringCollection> groupCollection = this->RemoveUserFromAllGroups( userId );
        this->GrantGroupMembershipsToUsers( groupCollection, userCollection );

        //  update roles
        Ptr<MgStringCollection> roleCollection = this->RemoveUserFromAllRoles( userId );
        this->GrantRoleMembershipsToUsers( roleCollection, userCollection );
    }

    //  update user full name
    if (!newUsername.empty())
    {
        DOMElement* node = (DOMElement*) root->getElementsByTagName( X(ELEMENT_USER_FULLNAME) )->item(0);
        xmlUtil.UpdateTextNode( node, newUsername.c_str() );
    }

    //  update password
    if (!newPassword.empty())
    {
        DOMElement* node = (DOMElement*) root->getElementsByTagName( X(ELEMENT_USER_PASSWORD) )->item(0);
        MgCryptographyManager cryptoManager;
        STRING encryptedPassword = cryptoManager.EncryptPassword(newPassword);

        xmlUtil.UpdateTextNode( node, encryptedPassword.c_str() );
    }

    //  update description
    {
        DOMElement* node = (DOMElement*) root->getElementsByTagName( X(ELEMENT_USER_DESCRIPTION) )->item(0);
        xmlUtil.UpdateTextNode( node, newDescription.c_str() );
    }

    MgResourceInfo resInfo(userRes);
    xmlUtil.ToStringUtf8(xmlContent);

    //  update the user resource
    UpdateResource(resInfo, xmlContent);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.UpdateUser" )
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates all available groups.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgSiteResourceContentManager::EnumerateAllGroups()
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MgResourceIdentifier groupFolder(MgRepositoryType::Site, L"",
        L"", MgResourceFolder::Groups, MgResourceType::Folder);

    string groupFolderPathname;
    MgUtil::WideCharToMultiByte(groupFolder.ToString(), groupFolderPathname);

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
    query += groupFolderPathname;
    query += "')]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Get the group documents and populate the group list.

    // this XML follows the GroupList-1.0.0.xsd schema
    XmlValue xmlValue;
    string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    list += "<GroupList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"GroupList-1.0.0.xsd\">\n";

    while (results.next(xmlValue)) // TODO: Need an XML writer
    {
        const XmlDocument& groupDoc = xmlValue.asDocument();

        WriteGroupList(&groupDoc, list);
    }

    // Write out the Everyone group.
    WriteGroupList(NULL, list);

    list += "</GroupList>";

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.EnumerateAllGroups")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates groups that contain the given user.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgSiteResourceContentManager::EnumerateGroupsByUser(
    CREFSTRING user)
{
    assert(!user.empty());
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/Group/Users/User/Name[text()=\"";
    query += MgUtil::WideCharToMultiByte(user);
    query += "\"]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Get the group documents and populate the group list.

    // this XML follows the GroupList-1.0.0.xsd schema
    XmlValue xmlValue;
    string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    list += "<GroupList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"GroupList-1.0.0.xsd\">\n";

    while (results.next(xmlValue)) // TODO: Need an XML writer
    {
        const XmlDocument& groupDoc = xmlValue.asDocument();

        WriteGroupList(&groupDoc, list);
    }

    // Write out the Everyone group.
    WriteGroupList(NULL, list);

    list += "</GroupList>";

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.EnumerateGroupsByUser")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates groups in the specified role.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgSiteResourceContentManager::EnumerateGroupsByRole(
    CREFSTRING role)
{
    assert(!role.empty());
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Create a Role resource identifier.

    MgResourceIdentifier roleRes(MgRepositoryType::Site, L"",
        MgResourceFolder::Roles, role, MgResourceType::Role);

    string roleDocPathname;
    MgUtil::WideCharToMultiByte(roleRes.ToString(), roleDocPathname);

    // Set up an XQuery.

    string colQuery = "collection('";
    colQuery += m_container.getName();
    colQuery += "')";
    colQuery += "/*[dbxml:metadata('dbxml:name')='";
    colQuery += roleDocPathname;
    colQuery += "']";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext colContext = xmlMan.createQueryContext();

    XmlResults docResults = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), colQuery, colContext).
            execute(GetXmlTxn(), colContext, 0) :
        xmlMan.prepare(colQuery, colContext).
            execute(colContext, 0);

    // Get the role document and populate the group list.

    // this XML follows the GroupList-1.0.0.xsd schema
    XmlValue docValue;
    string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    list += "<GroupList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"GroupList-1.0.0.xsd\">\n";

    if (docResults.next(docValue)) // TODO: Need an XML writer
    {
        assert(1 == docResults.size());

        // Set up an XQuery.

        string docQuery = "/Role/Groups/Group/Name/text()";
        XmlQueryContext docContext = xmlMan.createQueryContext();
        docContext.setReturnType(XmlQueryContext::DeadValues);

        // Execute the XQuery.

        XmlResults nodeResults = IsTransacted() ?
            xmlMan.prepare(GetXmlTxn(), docQuery, docContext).
                execute(GetXmlTxn(), docValue, docContext, 0) :
            xmlMan.prepare(docQuery, docContext).
                execute(docValue, docContext, 0);

        // Get the group document and populate the user list.

        XmlValue nodeValue;

        string strEveryone = MgUtil::WideCharToMultiByte(MgGroup::Everyone);

        while (nodeResults.next(nodeValue))
        {
            string value = nodeValue.asString();

            if ( value != strEveryone )
            {
                MgResourceIdentifier groupRes(MgRepositoryType::Site, L"",
                    MgResourceFolder::Groups,
                    MgUtil::MultiByteToWideChar(value),
                    MgResourceType::Group);
                XmlDocument groupDoc = GetDocument(groupRes);
                WriteGroupList(&groupDoc, list);
            }
            else
            {
                WriteGroupList(NULL, list);
            }
        }
    }

    list += "</GroupList>";

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.EnumerateGroupsByRole")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a group.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgDuplicateGroupException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::AddGroup(CREFSTRING group,
    CREFSTRING description)
{
    MG_RESOURCE_SERVICE_TRY()

    CHECK_EMPTY_CREFSTRING( group, L"MgSiteResourceContentManager.AddGroup" );

    // Do not allow adding the Everyone group
    // because we do not maintain its document in the repository at all.

    if (MgGroup::Everyone == group)
    {
        throw new MgInvalidOperationException(
            L"MgSiteResourceContentManager.AddGroup",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //  get user resource id
    MgResourceIdentifier groupRes( MgRepositoryType::Site, L"", MgResourceFolder::Groups,
        group, MgResourceType::Group );
    MgResourceInfo resInfo( groupRes );

    //  create group xml
    //  this XML follows the Group-1.0.0.xsd schema
    MgXmlUtil xmlUtil(ELEMENT_GROUP);

    DOMElement* root = xmlUtil.GetRootNode();

    xmlUtil.SetAttribute( root, "xmlns:xsi",
        L"http://www.w3.org/2001/XMLSchema-instance" );
    xmlUtil.SetAttribute( root, "xsi:noNamespaceSchemaLocation",
        L"Group-1.0.0.xsd" );

    xmlUtil.AddTextNode( root, ELEMENT_GROUP_DESCRIPTION, description.c_str() );
    xmlUtil.AddChildNode( root, MgUtil::WideCharToMultiByte( MgResourceFolder::Users ).c_str() );

    string xmlContent;
    xmlUtil.ToStringUtf8(xmlContent);

    //  add group
    AddResource(resInfo, xmlContent);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.AddGroup" )
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes the given groups in the MgStringCollection.  If the collection is
/// empty, all groups are deleted.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException, MgGroupNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::DeleteGroups(MgStringCollection* groups)
{
    MG_RESOURCE_SERVICE_TRY()

    CHECKNULL(groups, L"MgSiteResourceContentManager.DeleteGroups");

    //  iterate through group collection
    INT32 numGroups = groups->GetCount();

    if (0 == numGroups)
    {
        return;
    }

    for (int i = 0; i < numGroups; ++i)
    {
        STRING id = groups->GetItem(i);

        // Do not allow deleting the Everyone group
        // because we do not maintain its document in the repository at all.

        if (MgGroup::Everyone == id)
        {
            throw new MgInvalidOperationException(
                L"MgSiteResourceContentManager.DeleteGroups",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //  get user resource id
        MgResourceIdentifier groupRes( MgRepositoryType::Site, L"", MgResourceFolder::Groups,
            id, MgResourceType::Group );

        //  check for userId in database
        if (!FindResource(groupRes.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(id);

            throw new MgGroupNotFoundException(
                L"MgSiteResourceContentManager.DeleteGroups",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        //  delete role memberships
        Ptr<MgStringCollection> roleCollection = this->RemoveGroupFromAllRoles( id );

        //  delete the group
        this->DeleteResource( &groupRes );
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.DeleteGroups" )
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the group using the given parameters.  If a parameter is empty, that
/// group attribute is not changed (except for the newDescription parameter).
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// MgDuplicateGroupException, MgGroupNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::UpdateGroup( CREFSTRING group,
    CREFSTRING newGroup, CREFSTRING newDescription )
{
    MG_RESOURCE_SERVICE_TRY()

    CHECK_EMPTY_CREFSTRING( group,   L"MgSiteResourceContentManager.UpdateGroup" );

    // Do not allow updating the Everyone group
    // because we do not maintain its document in the repository at all.

    if (MgGroup::Everyone == group || MgGroup::Everyone == newGroup)
    {
        throw new MgInvalidOperationException(
            L"MgSiteResourceContentManager.UpdateGroup",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //  get group resource id
    MgResourceIdentifier groupRes(MgRepositoryType::Site, L"",
        MgResourceFolder::Groups, group, MgResourceType::Group);

    //  check for resource id and update if necessary
    if (!newGroup.empty())
    {
        MgResourceIdentifier newGroupRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Groups, newGroup, MgResourceType::Group);

        //  rename resource
        this->MoveResource(&groupRes, &newGroupRes, false);

        groupRes = newGroupRes;
    }

    //  get group xml
    Ptr<MgByteReader> byteReader = this->GetResource(&groupRes);

    //  parse user xml
    string xmlContent;
    byteReader->ToStringUtf8(xmlContent);
    MgXmlUtil xmlUtil(xmlContent);
    DOMDocument* doc = xmlUtil.GetDocument();
    DOMElement* root = doc->getDocumentElement();

    //  update group id
    if (!newGroup.empty())
    {
        //  update roles
        Ptr<MgStringCollection> groupCollection = new MgStringCollection();
        groupCollection->Add( newGroup );

        Ptr<MgStringCollection> roleCollection = this->RemoveGroupFromAllRoles( group );
        this->GrantRoleMembershipsToGroups( roleCollection, groupCollection );
    }

    //  update description
    {
        DOMElement* node = (DOMElement*) root->getElementsByTagName( X(ELEMENT_GROUP_DESCRIPTION) )->item(0);
        xmlUtil.UpdateTextNode( node, newDescription.c_str() );
    }

    MgResourceInfo resInfo(groupRes);
    xmlUtil.ToStringUtf8(xmlContent);

    //  update the user resource
    UpdateResource(resInfo, xmlContent);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.UpdateGroup" )
}

///----------------------------------------------------------------------------
/// <summary>
/// Grants membership in the specified groups to the specified users.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException, MgGroupNotFoundException, MgUserNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::GrantGroupMembershipsToUsers(
    MgStringCollection* groups, MgStringCollection* users)
{
    MG_RESOURCE_SERVICE_TRY()

    CHECKNULL( groups,      L"MgSiteResourceContentManager.GrantGroupMembershipsToUsers" );
    CHECKNULL( users,       L"MgSiteResourceContentManager.GrantGroupMembershipsToUsers" );

    //  check to make sure users exist
    INT32 numUsers = users->GetCount();

    for (INT32 i = 0; i < numUsers; ++i)
    {
        MgResourceIdentifier userRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Users, users->GetItem(i), MgResourceType::User);

        if (!FindResource(userRes.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(userRes.GetName());

            throw new MgUserNotFoundException(
                L"MgSiteResourceContentManager.GrantGroupMembershipsToUsers",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    //  iterate through groups, then add every user to group

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();
    INT32 numGroups = groups->GetCount();

    for (INT32 i = 0; i < numGroups; ++i)
    {
        bool update = false;    //  should we update the group?

        //  get the group document
        MgResourceIdentifier groupRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Groups, groups->GetItem(i), MgResourceType::Group);

        // Do not allow granting the Everyone group membership to users
        // because we do not maintain its document in the repository at all.

        if (MgGroup::Everyone == groupRes.GetName())
        {
            throw new MgInvalidOperationException(
                L"MgSiteResourceContentManager.GrantGroupMembershipsToUsers",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        XmlDocument groupDoc = this->GetDocument(groupRes, MgResourcePermission::ReadWrite);
        string xmlContent;
        MgXmlUtil xmlUtil(groupDoc.getContent(xmlContent));
        DOMDocument* groupDOM = xmlUtil.GetDocument();
        assert(NULL != groupDOM);

        //  get user folder
        DOMNodeList* list = groupDOM->getElementsByTagName( W2X( MgResourceFolder::Users.c_str() ) );
        DOMElement* elUsers = NULL;

        if (0 == list->getLength())
        {
            // Users node must exist according to the schema.
            // Should not reach here as XML validation is performed when adding
            // a resource.
            throw new MgXmlParserException(
                L"MgSiteResourceContentManager.GrantGroupMembershipsToUsers",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            elUsers = (DOMElement*) list->item(0);
            assert(NULL != elUsers);
        }

        //  add users to group list
        for (INT32 j = 0; j < numUsers; ++j)
        {
            STRING userId = users->GetItem(j);

            // Set up an XQuery.

            string query = "/Group/Users/User/Name/text()=\"";
            query += MgUtil::WideCharToMultiByte(userId);
            query += "\"";

            // Execute the XQuery.

            XmlResults results = IsTransacted() ?
                xmlMan.prepare(GetXmlTxn(), query, queryContext).
                    execute(GetXmlTxn(), groupDoc, queryContext, 0) :
                xmlMan.prepare(query, queryContext).
                    execute(groupDoc, queryContext, 0);

            XmlValue xmlValue;
            results.next(xmlValue);

            //  is there a user already?
            if ( "false" == xmlValue.asString() )    //  NOXLATE
            {
                //  add User Node
                DOMElement* elUser = groupDOM->createElement( X( ELEMENT_USER ) );
                elUsers->appendChild( elUser );
                //  add userId node
                DOMElement* elUserId = groupDOM->createElement( X( ELEMENT_USER_NAME ));
                elUser->appendChild( elUserId );

                //  add userId text
                DOMText* elText = groupDOM->createTextNode( W2X( userId.c_str() ) );
                elUserId->appendChild( elText);

                //  set update flag
                update = true;
            }

        }   //  next user

        if ( update )
        {
            //  update the group document
            MgXmlUtil xmlUtil(groupDOM, false);
            string xmlContent;

            xmlUtil.ToStringUtf8(xmlContent);
            groupDoc.setContent(xmlContent);

            UpdateDocument(groupRes, groupDoc, updateContext);
        }

    }   //  next group

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.GrantGroupMembershipsToUsers" )
}

///----------------------------------------------------------------------------
/// <summary>
/// Revokes membership in the specified groups from the specified users.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException, MgGroupNotFoundException, MgUserNotDFoundException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::RevokeGroupMembershipsFromUsers(
    MgStringCollection* groups, MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    CHECKNULL( groups,      L"MgSiteResourceContentManager.RevokeGroupMembershipsFromUsers" );
    CHECKNULL( users,       L"MgSiteResourceContentManager.RevokeGroupMembershipsFromUsers" );

    //  check to make sure users exist
    INT32 numUsers = users->GetCount();

    for (INT32 i = 0; i < numUsers; ++i)
    {
        MgResourceIdentifier userRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Users, users->GetItem(i), MgResourceType::User);

        if (!FindResource(userRes.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(userRes.GetName());

            throw new MgUserNotFoundException(
                L"MgSiteResourceContentManager.RevokeGroupMembershipsFromUsers",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    //  iterate through groups, then add every user to group
    INT32 numGroups = groups->GetCount();

    for (INT32 i = 0; i < numGroups; ++i)
    {
        //  check for the group
        MgResourceIdentifier groupRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Groups, groups->GetItem(i), MgResourceType::Group);

        // Do not allow revoking the Everyone group membership from users
        // because we do not maintain its document in the repository at all.

        if (MgGroup::Everyone == groupRes.GetName())
        {
            throw new MgInvalidOperationException(
                L"MgSiteResourceContentManager.RevokeGroupMembershipsFromUsers",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (!FindResource(groupRes.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(groupRes.GetName());

            throw new MgGroupNotFoundException(
                L"MgSiteResourceContentManager.RevokeGroupMembershipsFromUsers",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        //  remove each user from group
        for (INT32 j = 0; j < numUsers; ++j)
        {
            STRING userId = users->GetItem(j);

            this->RemoveUserFromGroup(&groupRes, userId);

        }   //  next user

    }   //  next group

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.RevokeGroupMembershipsFromUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates all available roles.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgStringCollection* MgSiteResourceContentManager::EnumerateAllRoles()
{
    Ptr<MgStringCollection> roles;

    MG_RESOURCE_SERVICE_TRY()

    MgResourceIdentifier roleFolder(MgRepositoryType::Site, L"",
        L"", MgResourceFolder::Roles, MgResourceType::Folder);

    string roleFolderPathname;
    MgUtil::WideCharToMultiByte(roleFolder.ToString(), roleFolderPathname);

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
    query += roleFolderPathname;
    query += "')]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Get the role documents and populate the role collection.

    roles = new MgStringCollection();
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        const XmlDocument& roleDoc = xmlValue.asDocument();
        MgResourceIdentifier roleRes(MgUtil::MultiByteToWideChar(
            roleDoc.getName()));

        roles->Add(roleRes.GetName());
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.EnumerateAllRoles")

    return SAFE_ADDREF((MgStringCollection*)roles);
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates roles for the given user.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgStringCollection* MgSiteResourceContentManager::EnumerateRolesOfUser(
    CREFSTRING user)
{
    assert(!user.empty());
    Ptr<MgStringCollection> roles;

    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/Role/Users/User/Name[text()=\"";
    query += MgUtil::WideCharToMultiByte(user);
    query += "\"]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Get the role documents and populate the role collection.

    roles = new MgStringCollection();
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        const XmlDocument& roleDoc = xmlValue.asDocument();
        MgResourceIdentifier roleRes(MgUtil::MultiByteToWideChar(
            roleDoc.getName()));

        roles->Add(roleRes.GetName());
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.EnumerateRolesOfUser")

    return SAFE_ADDREF((MgStringCollection*)roles);
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates roles for the given group.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgStringCollection* MgSiteResourceContentManager::EnumerateRolesOfGroup(
    CREFSTRING group)
{
    assert(!group.empty());
    Ptr<MgStringCollection> roles;

    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/Role/Groups/Group/Name[text()=\"";
    query += MgUtil::WideCharToMultiByte(group);
    query += "\"]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Get the role documents and populate the role collection.

    roles = new MgStringCollection();
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        const XmlDocument& roleDoc = xmlValue.asDocument();
        MgResourceIdentifier roleRes(MgUtil::MultiByteToWideChar(
            roleDoc.getName()));

        roles->Add(roleRes.GetName());
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.EnumerateRolesOfGroup")

    return SAFE_ADDREF((MgStringCollection*)roles);
}

///----------------------------------------------------------------------------
/// <summary>
/// Grants the given users membership to the specified roles
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException, MgRoleNotFoundException, MgUserNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::GrantRoleMembershipsToUsers(
    MgStringCollection* roles, MgStringCollection* users)
{
    MG_RESOURCE_SERVICE_TRY()

    CHECKNULL( roles,      L"MgSiteResourceContentManager.GrantRoleMembershipsToUsers" );
    CHECKNULL( users,       L"MgSiteResourceContentManager.GrantRoleMembershipsToUsers" );

    //  check to make sure users exist
    INT32 numUsers = users->GetCount();

    for (INT32 i = 0; i < numUsers; ++i)
    {
        MgResourceIdentifier userRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Users, users->GetItem(i), MgResourceType::User);

        if (!FindResource(userRes.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(userRes.GetName());

            throw new MgUserNotFoundException(
                L"MgSiteResourceContentManager.GrantRoleMembershipsToUsers",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    //  iterate through roles, then add every user to role

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();
    INT32 numRoles= roles->GetCount();

    for (INT32 i = 0; i < numRoles; ++i)
    {
        bool update = false;    //  should we update the role?

        //  get the role document
        MgResourceIdentifier roleRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Roles, roles->GetItem(i), MgResourceType::Role);

        // Do not allow granting the Viewer role membership to any user
        // because all users are in this role by default.

        if (MgRole::Viewer == roleRes.GetName())
        {
            throw new MgInvalidOperationException(
                L"MgSiteResourceContentManager.GrantRoleMembershipsToUsers",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        XmlDocument roleDoc = this->GetDocument(roleRes, MgResourcePermission::ReadWrite);
        string xmlContent;
        MgXmlUtil xmlUtil(roleDoc.getContent(xmlContent));
        DOMDocument* roleDOM = xmlUtil.GetDocument();
        assert(NULL != roleDOM);

        //  get user folder
        DOMNodeList* list = roleDOM->getElementsByTagName( W2X( MgResourceFolder::Users.c_str() ));
        DOMElement* elUsers = NULL;

        if ( 0 == list->getLength() )
        {
            // Users node must exist according to the schema.
            // Should not reach here as XML validation is performed when adding
            // a resource.
            throw new MgXmlParserException(
                L"MgSiteResourceContentManager.GrantRoleMembershipsToUsers",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            elUsers = (DOMElement*) list->item(0);
            assert(NULL != elUsers);
        }

        //  add users to role list
        for (INT32 j = 0; j < numUsers; ++j)
        {
            STRING userId = users->GetItem(j);

            // Set up an XQuery.

            string query = "/Role/Users/User/Name/text()=\"";
            query += MgUtil::WideCharToMultiByte(userId);
            query += "\"";

            // Execute the XQuery.

            XmlResults results = IsTransacted() ?
                xmlMan.prepare(GetXmlTxn(), query, queryContext).
                    execute(GetXmlTxn(), roleDoc, queryContext, 0) :
                xmlMan.prepare(query, queryContext).
                    execute(roleDoc, queryContext, 0);

            XmlValue xmlValue;
            results.next(xmlValue);

            //  is there a user already?
            if ( "false" == xmlValue.asString() )    //  NOXLATE
            {
                //  add User Node
                DOMElement* elUser = roleDOM->createElement( X( ELEMENT_USER ) );
                elUsers->appendChild( elUser );
                //  add userId node
                DOMElement* elUserId = roleDOM->createElement( X( ELEMENT_USER_NAME ));
                elUser->appendChild( elUserId );

                //  add userId text
                DOMText* elText = roleDOM->createTextNode( W2X( userId.c_str() ) );
                elUserId->appendChild( elText );

                //  set update flag
                update = true;
            }

        }   //  next user

        if ( update )
        {
            //  update the role document
            MgXmlUtil xmlUtil(roleDOM, false);
            string xmlContent;

            xmlUtil.ToStringUtf8(xmlContent);
            roleDoc.setContent(xmlContent);

            UpdateDocument(roleRes, roleDoc, updateContext);
        }

    }   //  next role

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.GrantRoleMembershipsToUsers" )
}

///----------------------------------------------------------------------------
/// <summary>
/// Revokes membership in the specified roles from the specified users.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException, MgRoleNotFoundException, MgUserNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::RevokeRoleMembershipsFromUsers(
    MgStringCollection* roles, MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    CHECKNULL( roles,       L"MgSiteResourceContentManager.RevokeGroupMembershipsFromUsers" );
    CHECKNULL( users,       L"MgSiteResourceContentManager.RevokeGroupMembershipsFromUsers" );

    //  check to make sure users exist
    INT32 numUsers = users->GetCount();

    for (INT32 i = 0; i < numUsers; ++i)
    {
        MgResourceIdentifier userRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Users, users->GetItem(i), MgResourceType::User);

        if (!FindResource(userRes.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(userRes.GetName());

            throw new MgUserNotFoundException(
                L"MgSiteResourceContentManager.RevokeRoleMembershipsFromUsers",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    //  iterate through roless, then remove every user from role
    INT32 numRoles = roles->GetCount();

    for (INT32 i = 0; i < numRoles; ++i)
    {
        //  check for the role
        MgResourceIdentifier roleRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Roles, roles->GetItem(i), MgResourceType::Role);

        // Do not allow revoking the Viewer role membership from any user
        // because all users are in this role by default.

        if (MgRole::Viewer == roleRes.GetName())
        {
            throw new MgInvalidOperationException(
                L"MgSiteResourceContentManager.RevokeRoleMembershipsFromUsers",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (!FindResource(roleRes.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(roleRes.GetName());

            throw new MgRoleNotFoundException(
                L"MgSiteResourceContentManager.RevokeRoleMembershipsFromUsers",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        //  remove each user from role
        for (INT32 j = 0; j < numUsers; ++j)
        {
            STRING userId = users->GetItem(j);

            this->RemoveUserFromRole(&roleRes, userId);

        }   //  next user

    }   //  next role

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.RevokeRoleMembershipsFromUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Grants to the given groups membership in the specified roles
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException, MgGroupNotFoundException, MgRoleNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::GrantRoleMembershipsToGroups(
    MgStringCollection* roles, MgStringCollection* groups)
{
    MG_RESOURCE_SERVICE_TRY()

    CHECKNULL( roles,       L"MgSiteResourceContentManager.GrantRoleMembershipsToGroups" );
    CHECKNULL( groups,      L"MgSiteResourceContentManager.GrantRoleMembershipsToGroups" );

    //  check to make sure groups exist
    INT32 numGroups = groups->GetCount();

    for (INT32 i = 0; i < numGroups; ++i)
    {
        MgResourceIdentifier groupRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Groups, groups->GetItem(i), MgResourceType::Group);

        // Note that we do not maintain the Everyone group document in the
        // repository at all. So, just skip it.

        if (MgGroup::Everyone == groupRes.GetName())
        {
            continue;
        }

        if (!FindResource(groupRes.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(groupRes.GetName());

            throw new MgGroupNotFoundException(
                L"MgSiteResourceContentManager.GrantRoleMembershipsToGroups",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    //  iterate through roles, then add every group to role

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();
    INT32 numRoles= roles->GetCount();

    for (INT32 i = 0; i < numRoles; ++i)
    {
        bool update = false;    //  should we update the role?

        //  get the role document
        MgResourceIdentifier roleRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Roles, roles->GetItem(i), MgResourceType::Role);

        // Do not allow granting the Viewer role membership to any group
        // because all groups are in this role by default.

        if (MgRole::Viewer == roleRes.GetName())
        {
            throw new MgInvalidOperationException(
                L"MgSiteResourceContentManager.GrantRoleMembershipsToGroups",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        XmlDocument roleDoc = this->GetDocument(roleRes, MgResourcePermission::ReadWrite);
        string xmlContent;
        MgXmlUtil xmlUtil(roleDoc.getContent(xmlContent));
        DOMDocument* roleDOM = xmlUtil.GetDocument();
        assert(NULL != roleDOM);

        //  get group folder
        DOMNodeList* list = roleDOM->getElementsByTagName( W2X( MgResourceFolder::Groups.c_str() ));
        DOMElement* elGroups = NULL;

        if (0 == list->getLength())
        {
            // Groups node must exist according to the schema.
            // Should not reach here as XML validation is performed when adding
            // a resource.
            throw new MgXmlParserException(
                L"MgSiteResourceContentManager.GrantRoleMembershipsToGroups",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            elGroups = (DOMElement*) list->item(0);
            assert(NULL != elGroups);
        }

        //  add groups to role list
        for (INT32 j = 0; j < numGroups; ++j)
        {
            STRING groupId = groups->GetItem(j);

            // Set up an XQuery.

            string query = "/Role/Groups/Group/Name/text()=\"";
            query += MgUtil::WideCharToMultiByte(groupId);
            query += "\"";

            // Execute the XQuery.

            XmlResults results = IsTransacted() ?
                xmlMan.prepare(GetXmlTxn(), query, queryContext).
                    execute(GetXmlTxn(), roleDoc, queryContext, 0) :
                xmlMan.prepare(query, queryContext).
                    execute(roleDoc, queryContext, 0);

            XmlValue xmlValue;
            results.next(xmlValue);

            //  is there a group already?
            if ( "false" == xmlValue.asString() )    //  NOXLATE
            {
                //  add group Node
                DOMElement* elGroup = roleDOM->createElement( X( ELEMENT_GROUP ) );
                elGroups->appendChild( elGroup );

                //  add groupId node
                DOMElement* elGroupId = roleDOM->createElement( X( ELEMENT_GROUP_NAME ));
                elGroup->appendChild( elGroupId );

                //  add groupId text
                DOMText* elText = roleDOM->createTextNode( W2X( groupId.c_str() ) );
                elGroupId->appendChild( elText );

                //  set update flag
                update = true;
            }

        }   //  next group

        if ( update )
        {
            //  update the role document
            MgXmlUtil xmlUtil(roleDOM, false);
            string xmlContent;

            xmlUtil.ToStringUtf8(xmlContent);
            roleDoc.setContent(xmlContent);

            UpdateDocument(roleRes, roleDoc, updateContext);
        }

    }   //  next role

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.GrantRoleMembershipsToGroups" )
}

///----------------------------------------------------------------------------
/// <summary>
/// Revokes the specified role memberships from the specified groups.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException, MgGroupNotFoundException, MgRoleNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------
void MgSiteResourceContentManager::RevokeRoleMembershipsFromGroups(
    MgStringCollection* roles, MgStringCollection* groups )
{
    MG_RESOURCE_SERVICE_TRY()

    CHECKNULL( roles,       L"MgSiteResourceContentManager.RevokeRoleMembershipsFromGroups" );
    CHECKNULL( groups,      L"MgSiteResourceContentManager.RevokeRoleMembershipsFromGroups" );

    //  check to make sure groups exist
    INT32 numGroups = groups->GetCount();

    for (INT32 i = 0; i < numGroups; ++i)
    {
        MgResourceIdentifier groupRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Groups, groups->GetItem(i), MgResourceType::Group);

        // Note that we do not maintain the Everyone group document in the
        // repository at all. So, just skip it.

        if (MgGroup::Everyone == groupRes.GetName())
        {
            continue;
        }

        if (!FindResource(groupRes.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(groupRes.GetName());

            throw new MgGroupNotFoundException(
                L"MgSiteResourceContentManager.RevokeRoleMembershipsFromGroups",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    //  iterate through roles, then remove every group from role
    INT32 numRoles = roles->GetCount();

    for (INT32 i = 0; i < numRoles; ++i)
    {
        //  check for the role
        MgResourceIdentifier roleRes(MgRepositoryType::Site, L"",
            MgResourceFolder::Roles, roles->GetItem(i), MgResourceType::Role);

        // Do not allow revoking the Viewer role membership from any group
        // because all groups are in this role by default.

        if (MgRole::Viewer == roleRes.GetName())
        {
            throw new MgInvalidOperationException(
                L"MgSiteResourceContentManager.RevokeRoleMembershipsFromGroups",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (!FindResource(roleRes.ToString()))
        {
            MgStringCollection arguments;
            arguments.Add(roleRes.GetName());

            throw new MgRoleNotFoundException(
                L"MgSiteResourceContentManager.RevokeRoleMembershipsFromGroups",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        //  remove each group from role
        for (INT32 j = 0; j < numGroups; ++j)
        {
            STRING groupId = groups->GetItem(j);

            this->RemoveGroupFromRole(&roleRes, groupId);

        }   //  next group

    }   //  next role

    MG_RESOURCE_SERVICE_CATCH_AND_THROW( L"MgSiteResourceContentManager.RevokeRoleMembershipsFromGroups")
}

///----------------------------------------------------------------------------
/// <summary>
/// Removes a user from the given group.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgSiteResourceContentManager::RemoveUserFromGroup(
    MgResourceIdentifier * const groupRes, STRING userId)
{
    assert(NULL != groupRes);

    MG_RESOURCE_SERVICE_TRY()

    // Do not allow removing the Everyone group membership from any user
    // because we do not maintain its document in the repository at all.

    if (MgGroup::Everyone == groupRes->GetName())
    {
        throw new MgInvalidOperationException(
            L"MgSiteResourceContentManager.RemoveUserFromGroup",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get the group document.

    XmlDocument xmlDoc = GetDocument(*groupRes, MgResourcePermission::ReadWrite);
    XmlValue docValue(xmlDoc);

    // Set up an XQuery.

    string query = "/Group/Users/User[Name=\"";
    query += MgUtil::WideCharToMultiByte(userId);
    query += "\"]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();
    XmlModify xmlMod = xmlMan.createModify();

    if (IsTransacted())
    {
        XmlQueryExpression xmlSelect = xmlMan.prepare(GetXmlTxn(), query, queryContext);

        xmlMod.addRemoveStep(xmlSelect);
        xmlMod.execute(GetXmlTxn(), docValue, queryContext, updateContext);
    }
    else
    {
        XmlQueryExpression xmlSelect = xmlMan.prepare(query, queryContext);

        xmlMod.addRemoveStep(xmlSelect);
        xmlMod.execute(docValue, queryContext, updateContext);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.RemoveUserFromGroup")
}

///----------------------------------------------------------------------------
/// <summary>
/// Removes user from all groups; returns a collection of the groups affected.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------
MgStringCollection* MgSiteResourceContentManager::RemoveUserFromAllGroups(
    STRING userId )
{
    Ptr<MgStringCollection> groups;

    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/Group/Users/User/Name[text()=\"";
    query += MgUtil::WideCharToMultiByte(userId);
    query += "\"]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Remove user from all groups.

    groups = new MgStringCollection();
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        const XmlDocument& xmlDoc = xmlValue.asDocument();
        MgResourceIdentifier groupRes(MgUtil::MultiByteToWideChar(
            xmlDoc.getName()));

        this->RemoveUserFromGroup(&groupRes, userId);
        groups->Add(groupRes.GetName());
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.RemoveUserFromAllGroups")

    return SAFE_ADDREF((MgStringCollection*)groups);
}

///----------------------------------------------------------------------------
/// <summary>
/// Removes a user from the given role.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgSiteResourceContentManager::RemoveUserFromRole(
    MgResourceIdentifier * const roleRes, STRING userId)
{
    assert(NULL != roleRes);

    MG_RESOURCE_SERVICE_TRY()

    // Do not allow removing the role of the Administrator or the Author.

    if ((MgRole::Administrator == roleRes->GetName() && MgUser::Administrator == userId)
     || (MgRole::Author        == roleRes->GetName() && MgUser::Author        == userId))
    {
        throw new MgInvalidOperationException(
            L"MgSiteResourceContentManager.RemoveUserFromRole",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get the role document.

    XmlDocument xmlDoc = GetDocument(*roleRes, MgResourcePermission::ReadWrite);
    XmlValue docValue(xmlDoc);

    // Set up an XQuery.

    string query = "/Role/Users/User[Name=\"";
    query += MgUtil::WideCharToMultiByte(userId);
    query += "\"]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();
    XmlModify xmlMod = xmlMan.createModify();

    if (IsTransacted())
    {
        XmlQueryExpression xmlSelect = xmlMan.prepare(GetXmlTxn(), query, queryContext);

        xmlMod.addRemoveStep(xmlSelect);
        xmlMod.execute(GetXmlTxn(), docValue, queryContext, updateContext);
    }
    else
    {
        XmlQueryExpression xmlSelect = xmlMan.prepare(query, queryContext);

        xmlMod.addRemoveStep(xmlSelect);
        xmlMod.execute(docValue, queryContext, updateContext);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.RemoveUserFromRole")

}

///----------------------------------------------------------------------------
/// <summary>
/// Removes user from all roles; returns a collection of the roles affected.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------
MgStringCollection* MgSiteResourceContentManager::RemoveUserFromAllRoles(
    STRING userId)
{
    Ptr<MgStringCollection> roles;

    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/Role/Users/User/Name[text()=\"";
    query += MgUtil::WideCharToMultiByte(userId);
    query += "\"]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Remove user from all roles.

    roles = new MgStringCollection();
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        const XmlDocument& xmlDoc = xmlValue.asDocument();
        MgResourceIdentifier roleRes(MgUtil::MultiByteToWideChar(
            xmlDoc.getName()));

        this->RemoveUserFromRole(&roleRes, userId);
        roles->Add(roleRes.GetName());
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.RemoveUserFromAllRoles")

    return SAFE_ADDREF((MgStringCollection*)roles);
}

///----------------------------------------------------------------------------
/// <summary>
/// Removes a group from the given role.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgSiteResourceContentManager::RemoveGroupFromRole(
    MgResourceIdentifier * const roleRes, STRING groupId)
{
    assert(NULL != roleRes);

    MG_RESOURCE_SERVICE_TRY()

    // Do not allow removing the Viewer role from the Everyone group.

    if (MgRole::Viewer == roleRes->GetName() && MgGroup::Everyone == groupId)
    {
        throw new MgInvalidOperationException(
            L"MgSiteResourceContentManager.RemoveGroupFromRole",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get the role document.

    XmlDocument xmlDoc = GetDocument(*roleRes, MgResourcePermission::ReadWrite);
    XmlValue docValue(xmlDoc);

    // Set up an XQuery.

    string query = "/Role/Groups/Group[Name=\"";
    query += MgUtil::WideCharToMultiByte(groupId);
    query += "\"]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();
    XmlModify xmlMod = xmlMan.createModify();

    if (IsTransacted())
    {
        XmlQueryExpression xmlSelect = xmlMan.prepare(GetXmlTxn(), query, queryContext);

        xmlMod.addRemoveStep(xmlSelect);
        xmlMod.execute(GetXmlTxn(), docValue, queryContext, updateContext);
    }
    else
    {
        XmlQueryExpression xmlSelect = xmlMan.prepare(query, queryContext);

        xmlMod.addRemoveStep(xmlSelect);
        xmlMod.execute(docValue, queryContext, updateContext);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.RemoveGroupFromRole")
}

///----------------------------------------------------------------------------
/// <summary>
/// Removes group from all roles; returns a collection of the roles affected.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------
MgStringCollection* MgSiteResourceContentManager::RemoveGroupFromAllRoles(
    STRING groupId)
{
    Ptr<MgStringCollection> roles;

    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "/Role/Groups/Group/Name[text()=\"";
    query += MgUtil::WideCharToMultiByte(groupId);
    query += "\"]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Remove group from all roles.

    roles = new MgStringCollection();
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        const XmlDocument& xmlDoc = xmlValue.asDocument();
        MgResourceIdentifier roleRes(MgUtil::MultiByteToWideChar(
            xmlDoc.getName()));

        this->RemoveGroupFromRole(&roleRes, groupId);
        roles->Add(roleRes.GetName());
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.RemoveGroupFromAllRoles")

    return SAFE_ADDREF((MgStringCollection*)roles);
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a new security cache.
/// </summary>
///----------------------------------------------------------------------------

MgSecurityCache* MgSiteResourceContentManager::CreateSecurityCache()
{
    Ptr<MgSecurityCache> securityCache;

    MG_RESOURCE_SERVICE_TRY()

    // Set up an XQuery.

    XmlManager& xmlMan = m_container.getManager();

    XmlQueryContext docContext = xmlMan.createQueryContext();
    docContext.setReturnType(XmlQueryContext::DeadValues);

    string xpathUserPassword = "/User/Password/text()";
    XmlQueryExpression selectUserPassword = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), xpathUserPassword, docContext) :
        xmlMan.prepare(xpathUserPassword, docContext);

    string xpathUsersInGroup = "/Group/Users/User/Name/text()";
    XmlQueryExpression selectUsersInGroup = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), xpathUsersInGroup, docContext) :
        xmlMan.prepare(xpathUsersInGroup, docContext);

    string xpathUsersInRole = "/Role/Users/User/Name/text()";
    XmlQueryExpression selectUsersInRole = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), xpathUsersInRole, docContext) :
        xmlMan.prepare(xpathUsersInRole, docContext);

    string xpathGroupsInRole = "/Role/Groups/Group/Name/text()";
    XmlQueryExpression selectGroupsInRole = IsTransacted() ?
        xmlMan.prepare(GetXmlTxn(), xpathGroupsInRole, docContext) :
        xmlMan.prepare(xpathGroupsInRole, docContext);

    // Execute the XQuery.

    XmlResults docResults = IsTransacted() ?
        m_container.getAllDocuments(GetXmlTxn(), DBXML_LAZY_DOCS) :
        m_container.getAllDocuments(DBXML_LAZY_DOCS);

    // Get the results.

    securityCache = Ptr<MgSecurityCache>(new MgSecurityCache);

    MgUserInfoMap& userInfoMap = securityCache->GetUserInfoMap();
    MgGroupInfoMap& groupInfoMap = securityCache->GetGroupInfoMap();
    MgRoleInfoMap& roleInfoMap = securityCache->GetRoleInfoMap();

    MgResourceIdentifier resource;
    STRING resourcePathname;
    STRING name, password;

    auto_ptr<MgUserInfo> userInfo;
    auto_ptr<MgGroupInfo> groupInfo;
    auto_ptr<MgRoleInfo> roleInfo;

    XmlDocument xmlDoc;

    while (docResults.next(xmlDoc))
    {
        MgUtil::MultiByteToWideChar(xmlDoc.getName(), resourcePathname);

        resource.SetResource(resourcePathname);

        if (resource.IsResourceTypeOf(MgResourceType::User))
        {
            // Get user.

            XmlResults nodeResults = IsTransacted() ?
                selectUserPassword.execute(GetXmlTxn(), xmlDoc, docContext, 0) :
                selectUserPassword.execute(xmlDoc, docContext, 0);

            XmlValue nodeValue;
            userInfo.reset(new MgUserInfo());

            if (nodeResults.next(nodeValue))
            {
                assert(1 == nodeResults.size());

                // Decrypt passwords in advance (instead of on demand) to
                // resolve the problem on Linux where WFS servers fail
                // requests with an authentication error.

                MG_CRYPTOGRAPHY_TRY()

                MgCryptographyUtil cryptoUtil;
                string decryptedPassword;

                cryptoUtil.DecryptPassword(nodeValue.asString(), decryptedPassword);
                MgUtil::MultiByteToWideChar(decryptedPassword, password);

                MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgSiteResourceContentManager.CreateSecurityCache")

                userInfo->SetPassword(password, false);
            }

            userInfoMap.insert(MgUserInfoMap::value_type(
                resource.GetName(), userInfo.release()));
        }
        else if (resource.IsResourceTypeOf(MgResourceType::Group))
        {
            // Get users in group.

            XmlResults nodeResults = IsTransacted() ?
                selectUsersInGroup.execute(GetXmlTxn(), xmlDoc, docContext, 0) :
                selectUsersInGroup.execute(xmlDoc, docContext, 0);

            XmlValue nodeValue;
            groupInfo.reset(new MgGroupInfo());
            set<STRING>& users = groupInfo->GetUsers();

            while (nodeResults.next(nodeValue))
            {
                MgUtil::MultiByteToWideChar(nodeValue.asString(), name);
                users.insert(name);
            }

            groupInfoMap.insert(MgGroupInfoMap::value_type(
                resource.GetName(), groupInfo.release()));
        }
        else if (resource.IsResourceTypeOf(MgResourceType::Role))
        {
            // Get users in role.

            XmlResults nodeResults = IsTransacted() ?
                selectUsersInRole.execute(GetXmlTxn(), xmlDoc, docContext, 0) :
                selectUsersInRole.execute(xmlDoc, docContext, 0);

            XmlValue nodeValue;
            roleInfo.reset(new MgRoleInfo());
            set<STRING>& users = roleInfo->GetUsers();

            while (nodeResults.next(nodeValue))
            {
                MgUtil::MultiByteToWideChar(nodeValue.asString(), name);
                users.insert(name);
            }

            // Get groups in role.

            nodeResults = IsTransacted() ?
                selectGroupsInRole.execute(GetXmlTxn(), xmlDoc, docContext, 0) :
                selectGroupsInRole.execute(xmlDoc, docContext, 0);

            set<STRING>& groups = roleInfo->GetGroups();

            while (nodeResults.next(nodeValue))
            {
                MgUtil::MultiByteToWideChar(nodeValue.asString(), name);
                groups.insert(name);
            }

            roleInfoMap.insert(MgRoleInfoMap::value_type(
                resource.GetName(), roleInfo.release()));
        }
        else
        {
            assert(resource.IsRoot());
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteResourceContentManager.CreateSecurityCache")

    return securityCache.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// specified resource.
/// </summary>
///
///----------------------------------------------------------------------------

bool MgSiteResourceContentManager::CheckPermission(
    MgResourceIdentifier& resource, CREFSTRING permission, bool strict)
{
    // This method does nothing because it relies on the role-based security
    // checking where the user must be an administrator.

    return true;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// parent of the specified resource.
/// </summary>
///----------------------------------------------------------------------------

bool MgSiteResourceContentManager::CheckParentPermission(
    MgResourceIdentifier& resource, CREFSTRING permission, bool strict)
{
    // This method does nothing because it relies on the role-based security
    // checking where the user must be an administrator.

    return true;
}
