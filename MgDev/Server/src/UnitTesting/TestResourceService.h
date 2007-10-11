//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef TESTRESOURCESERVICE_H_
#define TESTRESOURCESERVICE_H_

#include <cppunit/extensions/HelperMacros.h>

class TestResourceService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestResourceService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_EnumerateRepositories);
    CPPUNIT_TEST(TestCase_CreateRepository);
    CPPUNIT_TEST(TestCase_UpdateRepository);
    CPPUNIT_TEST(TestCase_GetRepositoryContent);
    CPPUNIT_TEST(TestCase_GetRepositoryHeader);
    CPPUNIT_TEST(TestCase_ApplyResourcePackage);
    CPPUNIT_TEST(TestCase_DeleteRepository);

    CPPUNIT_TEST(TestCase_EnumerateResourcesInXml);
    CPPUNIT_TEST(TestCase_EnumerateResourcesInJson);
    CPPUNIT_TEST(TestCase_SetResource);
    CPPUNIT_TEST(TestCase_MoveResource);
    CPPUNIT_TEST(TestCase_CopyResource);
    CPPUNIT_TEST(TestCase_GetResourceContentInXml);
    CPPUNIT_TEST(TestCase_GetResourceContentInJson);
    CPPUNIT_TEST(TestCase_GetResourceHeaderInXml);
    CPPUNIT_TEST(TestCase_GetResourceHeaderInJson);
    CPPUNIT_TEST(TestCase_EnumerateReferencesInXml);
    CPPUNIT_TEST(TestCase_EnumerateReferencesInJson);
    CPPUNIT_TEST(TestCase_ChangeResourceOwner);
    CPPUNIT_TEST(TestCase_InheritPermissionsFrom);

    CPPUNIT_TEST(TestCase_EnumerateResourceDataInXml);
    CPPUNIT_TEST(TestCase_EnumerateResourceDataInJson);
    CPPUNIT_TEST(TestCase_SetResourceData);
    CPPUNIT_TEST(TestCase_RenameResourceData);
    CPPUNIT_TEST(TestCase_GetResourceDataInXml);
    CPPUNIT_TEST(TestCase_GetResourceDataInJson);
    CPPUNIT_TEST(TestCase_DeleteResourceData);

    CPPUNIT_TEST(TestCase_DeleteResource);

    CPPUNIT_TEST(TestCase_EnumerateUnmanagedDataInXml);
    CPPUNIT_TEST(TestCase_EnumerateUnmanagedDataInJson);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    // Test Repository Management APIs
    void TestCase_EnumerateRepositories();
    void TestCase_CreateRepository();
    void TestCase_UpdateRepository();
    void TestCase_GetRepositoryContent();
    void TestCase_GetRepositoryHeader();
    void TestCase_ApplyResourcePackage();
    void TestCase_DeleteRepository();

    // Test Resource Management APIs
    void TestCase_EnumerateResourcesInXml();
    void TestCase_EnumerateResourcesInJson();
    void TestCase_SetResource();
    void TestCase_MoveResource();
    void TestCase_CopyResource();
    void TestCase_GetResourceContentInXml();
    void TestCase_GetResourceContentInJson();
    void TestCase_GetResourceHeaderInXml();
    void TestCase_GetResourceHeaderInJson();
    void TestCase_EnumerateReferencesInXml();
    void TestCase_EnumerateReferencesInJson();
    void TestCase_ChangeResourceOwner();
    void TestCase_InheritPermissionsFrom();

    // Test Resource Data Management APIs
    void TestCase_EnumerateResourceDataInXml();
    void TestCase_EnumerateResourceDataInJson();
    void TestCase_SetResourceData();
    void TestCase_RenameResourceData();
    void TestCase_GetResourceDataInXml();
    void TestCase_GetResourceDataInJson();
    void TestCase_DeleteResourceData();

    void TestCase_DeleteResource();

    void TestCase_EnumerateUnmanagedDataInXml();
    void TestCase_EnumerateUnmanagedDataInJson();
};

#endif // TESTRESOURCESERVICE_H_
