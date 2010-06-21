//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _TestSiteManager_H
#define _TestSiteManager_H

#include <cppunit/extensions/HelperMacros.h>

class TestSiteManager : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestSiteManager);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_ValidSiteManager);
    CPPUNIT_TEST(TestCase_GetSiteConnectionProperties);
    CPPUNIT_TEST(TestCase_GetSiteConnectionPropertiesWithSession);
    CPPUNIT_TEST(TestCase_GetSiteConnectionPropertiesIgnoreSession);
    CPPUNIT_TEST(TestCase_GetSpecificSiteConnectionProperties);
    CPPUNIT_TEST(TestCase_GetSupportServerConnectionProperties);
    CPPUNIT_TEST(TestCase_GetSiteInfo);
    CPPUNIT_TEST(TestCase_GetSiteCount);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_ValidSiteManager();
    void TestCase_GetSiteConnectionProperties();
    void TestCase_GetSiteConnectionPropertiesWithSession();
    void TestCase_GetSiteConnectionPropertiesIgnoreSession();
    void TestCase_GetSpecificSiteConnectionProperties();
    void TestCase_GetSupportServerConnectionProperties();
    void TestCase_GetSiteInfo();
    void TestCase_GetSiteCount();

private:
    static void TestWorkerThreads();
};

#endif // _TestSiteManager_H
