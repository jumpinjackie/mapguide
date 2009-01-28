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
#include "ServiceManager.h"
#include "ServerSiteService.h"
#include "TestSiteService.h"
#include "CppUnitExtensions.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestSiteService, "TestSiteService");

const STRING adminName = L"Administrator";
const STRING adminPass = L"admin";
const STRING userLocale = L"en";
const STRING anonymousName = L"Anonymous";
const STRING TEST_LOCALE = L"en";

STRING userId1 = L"TestUser1";
STRING userId2 = L"TestUser2";

STRING groupName1 = L"TestGroup1";
STRING groupName2 = L"TestGroup2";
STRING groupName3 = L"TestGroup3";

STRING serverName1 = L"TestServer1";
STRING serverName2 = L"TestServer2";

STRING serverAddress1 = L"127.0.0.2";
STRING serverAddress2 = L"127.0.0.3";

STRING session;


void TestSiteService::setUp()
{
}

void TestSiteService::tearDown()
{
}

void TestSiteService::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Site Service tests.\n")));
}

void TestSiteService::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nSite Service tests completed.\n\n")));
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case adds two new users
///----------------------------------------------------------------------------
void TestSiteService::TestCase_AddUser()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_AddUser", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_AddUser", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to add a user with empty strings
        CPPUNIT_ASSERT_THROW_MG(pService->AddUser(L"", L"username", L"password", L"description"), MgInvalidArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->AddUser(userId1, L"", L"password", L"description"), MgInvalidArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->AddUser(userId1, L"username", L"", L"description"), MgInvalidArgumentException*);

        //Add 2 new users
        pService->AddUser(userId1, L"TestUser1", L"TestPassword1", L"This is a test user");
        pService->AddUser(userId2, L"TestUser2", L"TestPassword2", L"This is another test user");

        //Try to add an already existing user id
        CPPUNIT_ASSERT_THROW_MG(pService->AddUser(userId1, L"TestUser1", L"TestPassword1", L"This is a test user"), MgDuplicateUserException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the users
///-------------------------------------------- --------------------------------
void TestSiteService::TestCase_EnumerateUsers()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_EnumerateUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_EnumerateUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader;

        //Enumerate users not including groups
        byteReader = pService->EnumerateUsers(L"", L"", false);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);

        //Enumerate users including groups
        byteReader = pService->EnumerateUsers(L"", L"", true);
        mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case updates one of the users added earlier
///----------------------------------------------------------------------------
void TestSiteService::TestCase_UpdateUser()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_UpdateUser", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_UpdateUser", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to update user with empty strings
        CPPUNIT_ASSERT_THROW_MG(pService->UpdateUser(L"", L"", L"username", L"password", L"description"), MgInvalidArgumentException*);

        //Try to update a user that does not exist
        CPPUNIT_ASSERT_THROW_MG(pService->UpdateUser(L"DoesNotExist", L"", L"username", L"password", L"description"), MgUserNotFoundException*);

        //Try to change the user id to one that already exists
        CPPUNIT_ASSERT_THROW_MG(pService->UpdateUser(userId1, userId1, L"newUsername", L"newPassword", L"newDescription"), MgDuplicateUserException*);

        //Update the user that was created earlier
        pService->UpdateUser(userId1, L"", L"newUsername", L"newPassword", L"newDescription");
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case adds a group
///----------------------------------------------------------------------------
void TestSiteService::TestCase_AddGroup()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_AddGroup", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_AddGroup", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to add an empty group name
        CPPUNIT_ASSERT_THROW_MG(pService->AddGroup(L"", L"This will throw an exception"), MgInvalidArgumentException*);

        //Add a new group
        pService->AddGroup(groupName1, L"This is a group used for testing");
        pService->AddGroup(groupName2, L"This is a second group used for testing");

        //Try to add the group again
        CPPUNIT_ASSERT_THROW_MG(pService->AddGroup(groupName1, L"This is a duplicate group"), MgDuplicateGroupException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case adds they newly created users to the newly created groups
///----------------------------------------------------------------------------
void TestSiteService::TestCase_GrantGroupMembershipsToUsers()
{
    //TODO: does not throw exceptions described in MgServerSiteService.cpp
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_GrantGroupMembershipsToUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_GrantGroupMembershipsToUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgStringCollection> groupCollection = new MgStringCollection();
        Ptr<MgStringCollection> userCollection = new MgStringCollection();

        //Try to use NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->GrantGroupMembershipsToUsers(NULL, userCollection), MgNullReferenceException*);
        CPPUNIT_ASSERT_THROW_MG(pService->GrantGroupMembershipsToUsers(groupCollection, NULL), MgNullReferenceException*);

        //Make both new users members of first group
        groupCollection->Add(groupName1);
        userCollection->Add(userId1);
        userCollection->Add(userId2);
        pService->GrantGroupMembershipsToUsers(groupCollection, userCollection);

        //Make 2nd new user member of 2nd group
        groupCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        groupCollection->Add(groupName2);
        userCollection->Add(userId2);
        pService->GrantGroupMembershipsToUsers(groupCollection, userCollection);

        //Try with a user that doesn't exist
        groupCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        userCollection->Add(L"DoesNotExist");
        CPPUNIT_ASSERT_THROW_MG(pService->GrantGroupMembershipsToUsers(groupCollection, userCollection), MgUserNotFoundException*);

        //Try with a group that doesn't exist
        groupCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        groupCollection->Add(L"DoesNotExist");
        CPPUNIT_ASSERT_THROW_MG(pService->GrantGroupMembershipsToUsers(groupCollection, userCollection), MgGroupNotFoundException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gives the role of author to one of the newly created users
///----------------------------------------------------------------------------
void TestSiteService::TestCase_GrantRoleMembershipsToUsers()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_GrantRoleMembershipsToUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_GrantRoleMembershipsToUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgStringCollection> roleCollection;
        Ptr<MgStringCollection> userCollection;

        //Try using NULL arguments
        roleCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        CPPUNIT_ASSERT_THROW_MG(pService->GrantRoleMembershipsToUsers(NULL,  userCollection), MgNullReferenceException*);
        CPPUNIT_ASSERT_THROW_MG(pService->GrantRoleMembershipsToUsers(roleCollection, NULL), MgNullReferenceException*);

        //Try using with a user that does not exist
        roleCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        userCollection->Add(L"DoesNotExist");
        CPPUNIT_ASSERT_THROW_MG(pService->GrantRoleMembershipsToUsers(roleCollection, userCollection), MgUserNotFoundException*);

        //Try to give the role viewer to one of the newly created users
        roleCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Viewer);
        userCollection->Add(userId1);
        CPPUNIT_ASSERT_THROW_MG(pService->GrantRoleMembershipsToUsers(roleCollection, userCollection), MgInvalidOperationException*);

        //Give the role of author to one of the newly created users
        roleCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Author);
        userCollection->Add(userId1);
        pService->GrantRoleMembershipsToUsers(roleCollection, userCollection);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case
///----------------------------------------------------------------------------
void TestSiteService::TestCase_GrantRoleMembershipsToGroups()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_GrantRoleMembershipsToGroups", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_GrantRoleMembershipsToGroups", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgStringCollection> roleCollection;
        Ptr<MgStringCollection> groupCollection;

        //Try using NULL arguments
        roleCollection = new MgStringCollection();
        groupCollection = new MgStringCollection();
        CPPUNIT_ASSERT_THROW_MG(pService->GrantRoleMembershipsToGroups(NULL,  groupCollection), MgNullReferenceException*);
        CPPUNIT_ASSERT_THROW_MG(pService->GrantRoleMembershipsToGroups(roleCollection, NULL), MgNullReferenceException*);

        //Try using with a group that does not exist
        roleCollection = new MgStringCollection();
        groupCollection = new MgStringCollection();
        groupCollection->Add(L"DoesNotExist");
        CPPUNIT_ASSERT_THROW_MG(pService->GrantRoleMembershipsToGroups(roleCollection, groupCollection), MgGroupNotFoundException*);

        //Try to give the role viewer to one of the newly created groups
        roleCollection = new MgStringCollection();
        groupCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Viewer);
        groupCollection->Add(groupName1);
        CPPUNIT_ASSERT_THROW_MG(pService->GrantRoleMembershipsToGroups(roleCollection, groupCollection), MgInvalidOperationException*);

        //Give the role of administrator to one of the newly created groups
        roleCollection = new MgStringCollection();
        groupCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Administrator);
        groupCollection->Add(groupName2);
        pService->GrantRoleMembershipsToGroups(roleCollection, groupCollection);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case updates the groups created earlier
///----------------------------------------------------------------------------
void TestSiteService::TestCase_UpdateGroup()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_UpdateGroup", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_UpdateGroup", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to update using an empty string for group name
        CPPUNIT_ASSERT_THROW_MG(pService->UpdateGroup(L"", L"", L""), MgInvalidArgumentException*);

        //Try to update a group that does not exist
        CPPUNIT_ASSERT_THROW_MG(pService->UpdateGroup(L"DoesNotExist", L"", L"description for non-existing group"), MgGroupNotFoundException*);

        //Try to change the group name to something that already exists
        CPPUNIT_ASSERT_THROW_MG(pService->UpdateGroup(groupName1, groupName1, L"This shouldn't work since we'll have a duplicate group name"), MgDuplicateGroupException*);

        //Update group description for first group
        pService->UpdateGroup(groupName1, L"", L"new description for this group used for testing");

        //Update group name of second group added
        pService->UpdateGroup(groupName2, groupName3, L"");
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the current groups
///----------------------------------------------------------------------------
void TestSiteService::TestCase_EnumerateGroups()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_EnumerateGroups", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_EnumerateGroups", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader;
        STRING mimeType;

        //Enumerate groups that a particular user is part of
        byteReader = pService->EnumerateGroups(userId1, L"");
        mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);

        //Enumerate groups that a particular user is part of
        byteReader = pService->EnumerateGroups(userId2, L"");
        mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);

        //Enumerate all groups
        byteReader = pService->EnumerateGroups(L"", L"");
        mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the current roles
///----------------------------------------------------------------------------
void TestSiteService::TestCase_EnumerateRoles()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_EnumerateRoles", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_EnumerateRoles", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgStringCollection> roleCollection;
        INT32 collSize;

        //Try to enumerate roles by specifying both a user and group
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateRoles(userId1, groupName1), MgInvalidArgumentException*);

        //Enumerate the roles of a user
        roleCollection = pService->EnumerateRoles(userId1, L"");
        collSize = roleCollection->GetCount();
        CPPUNIT_ASSERT(collSize == 1);

        //Enumerate the roles of a user
        roleCollection = pService->EnumerateRoles(userId2, L"");
        collSize = roleCollection->GetCount();
        CPPUNIT_ASSERT(collSize == 0);

        //Enumerate the roles of a group
        roleCollection = pService->EnumerateRoles(L"", groupName1);
        collSize = roleCollection->GetCount();
        CPPUNIT_ASSERT(collSize == 0);

        //Enumerate all roles
        roleCollection = pService->EnumerateRoles(L"", L"");
        collSize = roleCollection->GetCount();
        CPPUNIT_ASSERT(collSize == 3);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case removes users from groups
///----------------------------------------------------------------------------
void TestSiteService::TestCase_RevokeGroupMembershipsFromUsers()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_RevokeGroupMembershipsFromUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_RevokeGroupMembershipsFromUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgStringCollection> groupCollection = new MgStringCollection();
        Ptr<MgStringCollection> userCollection = new MgStringCollection();

        //Try to use NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeGroupMembershipsFromUsers(NULL, userCollection), MgNullReferenceException*);
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeGroupMembershipsFromUsers(groupCollection, NULL), MgNullReferenceException*);

        //Remove both new users from first group
        groupCollection->Add(groupName1);
        userCollection->Add(userId1);
        userCollection->Add(userId2);
        pService->RevokeGroupMembershipsFromUsers(groupCollection, userCollection);

        //Try with a user that doesn't exist
        groupCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        userCollection->Add(L"DoesNotExist");
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeGroupMembershipsFromUsers(groupCollection, userCollection), MgUserNotFoundException*);

        //Try with a group that doesn't exist
        groupCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        groupCollection->Add(L"DoesNotExist");
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeGroupMembershipsFromUsers(groupCollection, userCollection), MgGroupNotFoundException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case removes users from roles
///----------------------------------------------------------------------------
void TestSiteService::TestCase_RevokeRoleMembershipsFromUsers()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_RevokeRoleMembershipsFromUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_RevokeRoleMembershipsFromUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgStringCollection> roleCollection;
        Ptr<MgStringCollection> userCollection;

        //Try using NULL arguments
        roleCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeRoleMembershipsFromUsers(NULL,  userCollection), MgNullReferenceException*);
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeRoleMembershipsFromUsers(roleCollection, NULL), MgNullReferenceException*);

        //Try using with a user that does not exist
        roleCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        userCollection->Add(L"DoesNotExist");
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeRoleMembershipsFromUsers(roleCollection, userCollection), MgUserNotFoundException*);

        //Try to take away the role viewer from one of the newly created users
        roleCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Viewer);
        userCollection->Add(userId1);
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeRoleMembershipsFromUsers(roleCollection, userCollection), MgInvalidOperationException*);

        //Take away role of author from one of the newly created users
        roleCollection = new MgStringCollection();
        userCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Author);
        userCollection->Add(userId1);
        pService->RevokeRoleMembershipsFromUsers(roleCollection, userCollection);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case removes groups from roles
///----------------------------------------------------------------------------
void TestSiteService::TestCase_RevokeRoleMembershipsFromGroups()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_RevokeRoleMembershipsFromGroups", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_RevokeRoleMembershipsFromGroups", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgStringCollection> roleCollection;
        Ptr<MgStringCollection> groupCollection;

        //Try using NULL arguments
        roleCollection = new MgStringCollection();
        groupCollection = new MgStringCollection();
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeRoleMembershipsFromGroups(NULL,  groupCollection), MgNullReferenceException*);
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeRoleMembershipsFromGroups(roleCollection, NULL), MgNullReferenceException*);

        //Try using with a group that does not exist
        roleCollection = new MgStringCollection();
        groupCollection = new MgStringCollection();
        groupCollection->Add(L"DoesNotExist");
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeRoleMembershipsFromGroups(roleCollection, groupCollection), MgGroupNotFoundException*);

        //Try to give the role viewer to one of the newly created groups
        roleCollection = new MgStringCollection();
        groupCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Viewer);
        groupCollection->Add(groupName1);
        CPPUNIT_ASSERT_THROW_MG(pService->RevokeRoleMembershipsFromGroups(roleCollection, groupCollection), MgInvalidOperationException*);

        //Give the role of administrator to one of the newly created groups
        roleCollection = new MgStringCollection();
        groupCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Administrator);
        groupCollection->Add(groupName3);
        pService->RevokeRoleMembershipsFromGroups(roleCollection, groupCollection);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case deletes the 2 users created earlier
///----------------------------------------------------------------------------
void TestSiteService::TestCase_DeleteUsers()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_DeleteUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_DeleteUsers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try using a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->DeleteUsers(NULL), MgNullReferenceException*);

        //Delete the two users added earlier
        MgStringCollection userCollection;
        userCollection.Add(userId1);
        userCollection.Add(userId2);
        pService->DeleteUsers(&userCollection);

        //Try to delete again now that users shouldn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->DeleteUsers(&userCollection), MgUserNotFoundException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case deletes the 2 groups created earlier
///----------------------------------------------------------------------------
void TestSiteService::TestCase_DeleteGroups()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_DeleteGroups", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_DeleteGroups", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to use a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->DeleteGroups(NULL), MgNullReferenceException*);

        MgStringCollection groupCollection;
        groupCollection.Add(groupName1);
        groupCollection.Add(groupName3);

        //Delete the group added earlier
        pService->DeleteGroups(&groupCollection);

        //Try to delete again, the groups should no longer exist
        CPPUNIT_ASSERT_THROW_MG(pService->DeleteGroups(&groupCollection), MgGroupNotFoundException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case adds a new server
///----------------------------------------------------------------------------
void TestSiteService::TestCase_AddServer()
{
    //TODO: does not throw exceptions described in MgServerSiteService.cpp
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_AddServer", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_AddServer", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try using empty string arguments
        CPPUNIT_ASSERT_THROW_MG(pService->AddServer(L"", L"", serverAddress1), MgNullArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->AddServer(serverName1, L"", L""), MgNullArgumentException*);

        // Disable ACE's ERROR message for the server name
        #ifdef _DEBUG
        ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_WARNING | LM_STARTUP | LM_CRITICAL | LM_ALERT | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
        #else
        ACE_LOG_MSG->priority_mask(LM_INFO | LM_NOTICE | LM_WARNING | LM_STARTUP | LM_CRITICAL | LM_ALERT | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
        #endif

        //Try adding some servers
        pService->AddServer(serverName1, L"This is a test server", serverAddress1);
        pService->AddServer(serverName2, L"This is a dummy server", serverAddress2);

        // Enable ACE's ERROR message
        #ifdef _DEBUG
        ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_WARNING | LM_STARTUP | LM_ERROR | LM_CRITICAL | LM_ALERT | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
        #else
        ACE_LOG_MSG->priority_mask(LM_INFO | LM_NOTICE | LM_WARNING | LM_STARTUP | LM_ERROR | LM_CRITICAL | LM_ALERT | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
        #endif

        //Try adding the server again
        CPPUNIT_ASSERT_THROW_MG(pService->AddServer(serverName1, L"This is a test server", serverAddress1), MgDuplicateServerException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the list of servers
///----------------------------------------------------------------------------
void TestSiteService::TestCase_EnumerateServers()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_EnumerateServers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_EnumerateServers", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader;

        //Enumerate the servers
        byteReader = pService->EnumerateServers();
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case updates the description of the server that was added
///----------------------------------------------------------------------------
void TestSiteService::TestCase_UpdateServer()
{
    //TODO: does not throw exceptions described in MgServerSiteService.cpp
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_UpdateServer", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_UpdateServer", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Disable ACE's ERROR message for the server name
        #ifdef _DEBUG
        ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_WARNING | LM_STARTUP | LM_CRITICAL | LM_ALERT | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
        #else
        ACE_LOG_MSG->priority_mask(LM_INFO | LM_NOTICE | LM_WARNING | LM_STARTUP | LM_CRITICAL | LM_ALERT | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
        #endif

        //Try using a blank string for the name of the server
        CPPUNIT_ASSERT_THROW_MG(pService->UpdateServer(L"", L"", L"", L""), MgNullArgumentException*);

        //Try to update a server that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->UpdateServer(L"DoesNotExist", L"", L"This server doesn't exist", L""), MgServerNotFoundException*);

        //Try to update a server giving it aname that already exists
        CPPUNIT_ASSERT_THROW_MG(pService->UpdateServer(serverName1, serverName2, L"The new name for the server already exists", L""), MgDuplicateServerException*);
        CPPUNIT_ASSERT_THROW_MG(pService->UpdateServer(serverName1, L"", L"The new name for the server already exists", serverAddress2), MgDuplicateServerException*);

        //Update the server description
        pService->UpdateServer(serverName1, L"", L"This is the updated description", L"");

        // Enable ACE's ERROR message
        #ifdef _DEBUG
        ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_WARNING | LM_STARTUP | LM_ERROR | LM_CRITICAL | LM_ALERT | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
        #else
        ACE_LOG_MSG->priority_mask(LM_INFO | LM_NOTICE | LM_WARNING | LM_STARTUP | LM_ERROR | LM_CRITICAL | LM_ALERT | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
        #endif
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case removes the server that was added before
///----------------------------------------------------------------------------
void TestSiteService::TestCase_RemoveServer()
{
    //TODO: does not throw exceptions described in MgServerSiteService.cpp
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_RemoveServer", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_RemoveServer", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Delete the servers that were added earlier
        pService->RemoveServer(serverName1);
        pService->RemoveServer(serverName2);

        //Try to delete a server that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->RemoveServer(L"DoesNotExist"), MgServerNotFoundException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case
///----------------------------------------------------------------------------
void TestSiteService::TestCase_Authenticate()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_Authenticate", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_Authenticate", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgStringCollection> roleCollection;
        Ptr<MgUserInformation> userInfo;

        //Try to use a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->Authenticate(NULL, NULL, false), MgAuthenticationFailedException*);

        //Try to authenticate an anonymous user when admin role is needed
        userInfo = new MgUserInformation(anonymousName, L"");
        roleCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Administrator);
        CPPUNIT_ASSERT_THROW_MG(pService->Authenticate(userInfo, roleCollection, false), MgUnauthorizedAccessException*);

        //Try to authenticate when an incorrect password is given
        userInfo = new MgUserInformation(adminName, L"");
        CPPUNIT_ASSERT_THROW_MG(pService->Authenticate(userInfo, NULL, false), MgAuthenticationFailedException*);

        //Authenticate an anonymous user that has appropriate access
        userInfo = new MgUserInformation(anonymousName, L"");
        pService->Authenticate(userInfo, NULL, false);

        //Authenticate an anonymous user that has appropriate access
        userInfo = new MgUserInformation(anonymousName, L"");
        roleCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Viewer);
        pService->Authenticate(userInfo, roleCollection, false);

        //Authenticate a user that has appropriate access
        userInfo = new MgUserInformation(adminName, adminPass);
        roleCollection = new MgStringCollection();
        roleCollection->Add(MgRole::Administrator);
        pService->Authenticate(userInfo, roleCollection, false);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case creates a session
///----------------------------------------------------------------------------
void TestSiteService::TestCase_CreateSession()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_CreateSession", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_CreateSession", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be anonymous
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(anonymousName, L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            session = pService->CreateSession();
            CPPUNIT_ASSERT(!session.empty());
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetStackTrace(userLocale);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case destroys a session
///----------------------------------------------------------------------------
void TestSiteService::TestCase_DestroySession()
{
   try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestSiteService.TestCase_CreateSession", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerSiteService> pService = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestSiteService.TestCase_CreateSession", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            // Destroy the session that should have been created earlier in the create session test case
            pService->DestroySession(session);

            CPPUNIT_ASSERT_THROW_MG(pService->DestroySession(session), MgRepositoryNotFoundException*);
        }

        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            CPPUNIT_ASSERT_THROW_MG(pService->DestroySession(session), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}
