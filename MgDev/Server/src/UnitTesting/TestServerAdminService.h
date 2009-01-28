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

#ifndef TESTSERVERADMINSERVICE_H_
#define TESTSERVERADMINSERVICE_H_

#include <cppunit/extensions/HelperMacros.h>

class TestServerAdminService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestServerAdminService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_TakeOffline);
    CPPUNIT_TEST(TestCase_BringOnline);
    //CPPUNIT_TEST(TestCase_IsOnline);
    CPPUNIT_TEST(TestCase_GetConfigurationProperties);
    CPPUNIT_TEST(TestCase_SetConfigurationProperties);
    CPPUNIT_TEST(TestCase_RemoveConfigurationProperties);
    CPPUNIT_TEST(TestCase_GetInformationProperties);

    CPPUNIT_TEST(TestCase_ClearLog);
    CPPUNIT_TEST(TestCase_GetLog);
    CPPUNIT_TEST(TestCase_GetLogEntries);
    CPPUNIT_TEST(TestCase_GetLogByDate);

    CPPUNIT_TEST(TestCase_EnumeratePackages);
    CPPUNIT_TEST(TestCase_LoadPackage);
    CPPUNIT_TEST(TestCase_MakePackage);
    CPPUNIT_TEST(TestCase_GetPackageLog);
    CPPUNIT_TEST(TestCase_GetPackageStatus);
    CPPUNIT_TEST(TestCase_DeletePackage);

    // TODO: add these tests once methods implemented
    //CPPUNIT_TEST(TestCase_RegisterServicesOnServer);
    //CPPUNIT_TEST(TestCase_UnregisterServicesOnServer);

    CPPUNIT_TEST(TestCase_SetDocument);
    CPPUNIT_TEST(TestCase_GetDocument);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_TakeOffline();
    void TestCase_BringOnline();
    //void TestCase_IsOnline();
    void TestCase_GetConfigurationProperties();
    void TestCase_SetConfigurationProperties();
    void TestCase_RemoveConfigurationProperties();
    void TestCase_GetInformationProperties();

    void TestCase_ClearLog();
    void TestCase_GetLog();
    void TestCase_GetLogEntries();
    void TestCase_GetLogByDate();

    void TestCase_EnumeratePackages();
    void TestCase_LoadPackage();
    void TestCase_MakePackage();
    void TestCase_GetPackageLog();
    void TestCase_GetPackageStatus();
    void TestCase_DeletePackage();

    //void TestCase_RegisterServicesOnServer();
    //void TestCase_UnregisterServicesOnServer();

    void TestCase_GetDocument();
    void TestCase_SetDocument();

private:
    const static STRING PackageName;
    const static STRING PackageName2;
};

#endif
