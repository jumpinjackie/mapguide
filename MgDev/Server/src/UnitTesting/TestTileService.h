//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
    CPPUNIT_TEST(TestCase_GetTileProviders);
    CPPUNIT_TEST(TestCase_MgMap_Inline);
    CPPUNIT_TEST(TestCase_MgMap_Linked);
    CPPUNIT_TEST(TestCase_MgMap_LinkedSameCS);
    CPPUNIT_TEST(TestCase_MgMapFromXYZTileSetStrict);
    CPPUNIT_TEST(TestCase_MgMapFromXYZTileSetLoose);
    CPPUNIT_TEST(TestCase_MgMapFromTileSet);
    CPPUNIT_TEST(TestCase_GetTile);
    CPPUNIT_TEST(TestCase_SetTile);
    CPPUNIT_TEST(TestCase_GetSetTile);
    CPPUNIT_TEST(TestCase_ClearCache);
    CPPUNIT_TEST(TestCase_GetTileLinked);
    CPPUNIT_TEST(TestCase_GetTileXYZ);
    CPPUNIT_TEST(TestCase_ClearCacheLinked);
    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    TestTileService();
    ~TestTileService();

    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_GetTileProviders();
    void TestCase_MgMap_Inline();
    void TestCase_MgMap_Linked();
    void TestCase_MgMap_LinkedSameCS();
    void TestCase_MgMapFromXYZTileSetStrict();
    void TestCase_MgMapFromXYZTileSetLoose();
    void TestCase_MgMapFromTileSet();
    void TestCase_GetTile();
    void TestCase_SetTile();
    void TestCase_GetSetTile();
    void TestCase_GetTileXYZ();
    void TestCase_ClearCache();
    void TestCase_GetTileLinked();
    void TestCase_ClearCacheLinked();

private:
    MgMap* CreateMap(CREFSTRING mapName = L"");
    MgMap* CreateMapLinked(CREFSTRING mapName = L"");
    MgMap* CreateMapLinked2(CREFSTRING mapName = L"");
    INT32 Rand(INT32 n);

private:
    Ptr<MgSiteConnection> m_siteConnection;
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgTileService> m_svcTile;
};

#endif
