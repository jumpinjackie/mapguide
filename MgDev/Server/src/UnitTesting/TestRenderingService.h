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

#ifndef _TESTRENDERINGSERVICE_H
#define _TESTRENDERINGSERVICE_H

#include <cppunit/extensions/HelperMacros.h>

class TestRenderingService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestRenderingService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_SymbologyPoints);
    CPPUNIT_TEST(TestCase_SymbologyPointsParam);
    CPPUNIT_TEST(TestCase_SymbologyLines);
    CPPUNIT_TEST(TestCase_SymbologyLinesCrossTick);
    CPPUNIT_TEST(TestCase_SymbologyPolygons);

    CPPUNIT_TEST(TestCase_Annotation1);
    CPPUNIT_TEST(TestCase_Annotation2);
    CPPUNIT_TEST(TestCase_Annotation3);

    CPPUNIT_TEST(TestCase_RenderDynamicOverlay);
    CPPUNIT_TEST(TestCase_RenderMap);
    CPPUNIT_TEST(TestCase_RenderMapWithWatermark);
    CPPUNIT_TEST(TestCase_RenderLegend);
    CPPUNIT_TEST(TestCase_QueryFeatures);

    //CPPUNIT_TEST(TestCase_RendererPerformance);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    TestRenderingService();
    ~TestRenderingService();

    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_RenderDynamicOverlay();
    void TestCase_RenderMap();
    void TestCase_RenderMapWithWatermark();
    void TestCase_RenderLegend();
    void TestCase_QueryFeatures();

    void TestCase_SymbologyPoints();
    void TestCase_SymbologyPointsParam();
    void TestCase_SymbologyLines();
    void TestCase_SymbologyLinesCrossTick();
    void TestCase_SymbologyPolygons();

    void TestCase_Annotation1();
    void TestCase_Annotation2();
    void TestCase_Annotation3();

    //void TestCase_RendererPerformance();

private:
    MgMap* CreateTestMap();
    MgMap* CreateTestMapWithWatermark();
    MgPolygon* CreateSelectionPolygon(MgMap* map, double width, double height);

private:
    Ptr<MgSiteConnection> m_siteConnection;
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgRenderingService> m_svcRendering;
};

#endif // _TESTRENDERINGSERVICE_H
