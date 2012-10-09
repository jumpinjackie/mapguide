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

#ifndef _TESTTILESERVICE_H
#define _TESTTILESERVICE_H

#include <cppunit/extensions/HelperMacros.h>

class TestTileService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestTileService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_GetTile);
    CPPUNIT_TEST(TestCase_SetTile);
    CPPUNIT_TEST(TestCase_GetSetTile);
    CPPUNIT_TEST(TestCase_ClearCache);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    TestTileService();
    ~TestTileService();

    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_GetTile();
    void TestCase_SetTile();
    void TestCase_GetSetTile();
    void TestCase_ClearCache();

private:
    MgdMap* CreateMap(CREFSTRING mapName = L"");
    INT32 Rand(INT32 n);

private:
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgdTileService> m_svcTile;
};

#endif
