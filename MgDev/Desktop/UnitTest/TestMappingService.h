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

#ifndef TESTMAPPINGSERVICE_H_
#define TESTMAPPINGSERVICE_H_

#include <cppunit/extensions/HelperMacros.h>

class TestMappingService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestMappingService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    //CPPUNIT_TEST(TestCase_GetMap);
    //CPPUNIT_TEST(TestCase_GetMapUpdate);
    //CPPUNIT_TEST(TestCase_SaveMap);
    CPPUNIT_TEST(TestCase_GetMultiPlot);
    CPPUNIT_TEST(TestCase_GetPlotUsingCurrentCenterAndScale);
    CPPUNIT_TEST(TestCase_GetPlotUsingOverriddenCenterAndScale);
    CPPUNIT_TEST(TestCase_GetPlotUsingExtents);
    CPPUNIT_TEST(TestCase_GetPlotUsingExtentsAndExpandToFit);
    //CPPUNIT_TEST(TestCase_GetLegendPlot);
    //CPPUNIT_TEST(TestCase_QueryFeaturesImageMap);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:

    TestMappingService();
    ~TestMappingService();

    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    //void TestCase_GetMap();
    //void TestCase_GetMapUpdate();
    //void TestCase_SaveMap();
    void TestCase_GetPlot();
    void TestCase_GetMultiPlot();
    void TestCase_GetPlotUsingCurrentCenterAndScale();
    void TestCase_GetPlotUsingOverriddenCenterAndScale();
    void TestCase_GetPlotUsingExtents();
    void TestCase_GetPlotUsingExtentsAndExpandToFit();
    //void TestCase_GetLegendPlot();
    //void TestCase_QueryFeaturesImageMap();

private:

    void PublishTheResources();

    Ptr<MgdResourceService> m_svcResource;
    Ptr<MgdMappingService> m_svcMapping;
};

#endif // TESTMAPPINGSERVICE_H_
