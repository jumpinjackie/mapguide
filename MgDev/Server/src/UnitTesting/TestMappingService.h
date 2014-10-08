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

#ifndef TESTMAPPINGSERVICE_H_
#define TESTMAPPINGSERVICE_H_

#include <cppunit/extensions/HelperMacros.h>

class TestMappingService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestMappingService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_SaveMap);
    CPPUNIT_TEST(TestCase_GetMultiPlot);
    CPPUNIT_TEST(TestCase_GetPlotUsingCurrentCenterAndScale);
    CPPUNIT_TEST(TestCase_GetPlotUsingOverriddenCenterAndScale);
    CPPUNIT_TEST(TestCase_GetPlotUsingExtents);
    CPPUNIT_TEST(TestCase_GetPlotUsingExtentsAndExpandToFit);
    CPPUNIT_TEST(TestCase_GetLegendPlot);
    CPPUNIT_TEST(TestCase_GetLegendImage);
    CPPUNIT_TEST(TestCase_GetLegendImagePointStyleWithConstRotations);
    CPPUNIT_TEST(TestCase_GetLegendImageConvenience);
    CPPUNIT_TEST(TestCase_GetLegendImageCompositeConvenience);
    CPPUNIT_TEST(TestCase_GetLegendImageCompositeThemedConvenience);
    CPPUNIT_TEST(TestCase_GetLegendImagePointStyleWithConstRotationsConvenience);
    CPPUNIT_TEST(TestCase_CreateRuntimeMap);
    CPPUNIT_TEST(TestCase_DescribeRuntimeMap);
    CPPUNIT_TEST(TestCase_CreateAndDescribeLinkedRuntimeMap);
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

    void TestCase_CreateRuntimeMap();
    void TestCase_DescribeRuntimeMap();
    void TestCase_SaveMap();
    void TestCase_GetPlot();
    void TestCase_GetMultiPlot();
    void TestCase_CreateAndDescribeLinkedRuntimeMap();
    void TestCase_GetPlotUsingCurrentCenterAndScale();
    void TestCase_GetPlotUsingOverriddenCenterAndScale();
    void TestCase_GetPlotUsingExtents();
    void TestCase_GetPlotUsingExtentsAndExpandToFit();
    void TestCase_GetLegendPlot();
    void TestCase_GetLegendImage();
    void TestCase_GetLegendImagePointStyleWithConstRotations();
    void TestCase_GetLegendImageConvenience();
    void TestCase_GetLegendImageCompositeConvenience();
    void TestCase_GetLegendImageCompositeThemedConvenience();
    void TestCase_GetLegendImagePointStyleWithConstRotationsConvenience();
    void TestCase_QueryFeaturesImageMap();

private:

    void PublishTheResources();

    Ptr<MgSiteConnection> m_siteConnection;
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgMappingService> m_svcMapping;

    STRING m_session;
};

#endif // TESTMAPPINGSERVICE_H_
