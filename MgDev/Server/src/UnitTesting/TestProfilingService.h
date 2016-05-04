//
//  Copyright (C) 2011 by Autodesk, Inc.
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

#ifndef _TESTPROFILINGSERVICE_H
#define _TESTPROFILINGSERVICE_H

#include <cppunit/extensions/HelperMacros.h>

class TestProfilingService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestProfilingService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_ProfileRenderDynamicOverlay);
    CPPUNIT_TEST(TestCase_ProfileRenderMap);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    TestProfilingService();
    ~TestProfilingService();

    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_ProfileRenderDynamicOverlay();
    void TestCase_ProfileRenderMap();

private:
    MgMap* CreateTestMap();
    MgMap* CreateTestMapWithWatermark();
    MgSelection* CreateSelection(MgMap* map);

private:
    Ptr<MgSiteConnection> m_siteConnection;
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgProfilingService> m_svcProfiling;
};

#endif // _TESTPROFILINGSERVICE_H
