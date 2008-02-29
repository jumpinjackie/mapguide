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

#ifndef _TESTSITESERVICE_H
#define _TESTSITESERVICE_H

#include <cppunit/extensions/HelperMacros.h>

class TestSiteService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestSiteService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_EnumerateUsers);
    CPPUNIT_TEST(TestCase_AddUser);
    CPPUNIT_TEST(TestCase_UpdateUser);

    CPPUNIT_TEST(TestCase_AddGroup);

    CPPUNIT_TEST(TestCase_GrantGroupMembershipsToUsers);
    CPPUNIT_TEST(TestCase_GrantRoleMembershipsToUsers);
    CPPUNIT_TEST(TestCase_GrantRoleMembershipsToGroups);

    CPPUNIT_TEST(TestCase_UpdateGroup);
    CPPUNIT_TEST(TestCase_EnumerateGroups);
    CPPUNIT_TEST(TestCase_EnumerateRoles);

    CPPUNIT_TEST(TestCase_RevokeGroupMembershipsFromUsers);
    CPPUNIT_TEST(TestCase_RevokeRoleMembershipsFromUsers);
    CPPUNIT_TEST(TestCase_RevokeRoleMembershipsFromGroups);

    CPPUNIT_TEST(TestCase_DeleteUsers);
    CPPUNIT_TEST(TestCase_DeleteGroups);

    CPPUNIT_TEST(TestCase_AddServer);
    CPPUNIT_TEST(TestCase_EnumerateServers);
    CPPUNIT_TEST(TestCase_UpdateServer);
    //CPPUNIT_TEST(TestCase_RequestServer);

    CPPUNIT_TEST(TestCase_RemoveServer);

    CPPUNIT_TEST(TestCase_Authenticate);
    CPPUNIT_TEST(TestCase_CreateSession);
    CPPUNIT_TEST(TestCase_DestroySession);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_AddUser();
    void TestCase_EnumerateUsers();
    void TestCase_UpdateUser();

    void TestCase_AddGroup();

    void TestCase_GrantGroupMembershipsToUsers();
    void TestCase_GrantRoleMembershipsToUsers();
    void TestCase_GrantRoleMembershipsToGroups();

    void TestCase_UpdateGroup();
    void TestCase_EnumerateGroups();
    void TestCase_EnumerateRoles();

    void TestCase_RevokeGroupMembershipsFromUsers();
    void TestCase_RevokeRoleMembershipsFromUsers();
    void TestCase_RevokeRoleMembershipsFromGroups();

    void TestCase_DeleteUsers();
    void TestCase_DeleteGroups();

    void TestCase_AddServer();
    void TestCase_EnumerateServers();
    void TestCase_UpdateServer();

    //TODO: add these tests once these functions are implemented
    //void TestCase_RequestServer();

    void TestCase_RemoveServicesFromServer();
    void TestCase_RemoveServer();

    void TestCase_Authenticate();
    void TestCase_CreateSession();
    void TestCase_DestroySession();
};

#endif // _TESTSITESERVICE_H
