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

#ifndef _TESTSERVERMANAGER_H
#define _TESTSERVERMANAGER_H

#include <cppunit/extensions/HelperMacros.h>

class TestServerManager : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestServerManager);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_ValidServerManager);
    CPPUNIT_TEST(TestCase_GetDefaultMessageLocale);
    CPPUNIT_TEST(TestCase_GetAdminPort);
    CPPUNIT_TEST(TestCase_GetClientPort);
    CPPUNIT_TEST(TestCase_GetSitePort);
    CPPUNIT_TEST(TestCase_GetAdminThreads);
    CPPUNIT_TEST(TestCase_GetClientThreads);
    CPPUNIT_TEST(TestCase_GetSiteThreads);
    CPPUNIT_TEST(TestCase_ValidateWorkerThreads);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_ValidServerManager();
    void TestCase_GetDefaultMessageLocale();
    void TestCase_GetAdminPort();
    void TestCase_GetClientPort();
    void TestCase_GetSitePort();
    void TestCase_GetAdminThreads();
    void TestCase_GetClientThreads();
    void TestCase_GetSiteThreads();
    void TestCase_ValidateWorkerThreads();

private:
    static void TestWorkerThreads();
};

#endif // _TESTSERVERMANAGER_H
