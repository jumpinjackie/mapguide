//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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

#ifndef _TESTDRAWINGSERVICE_H
#define _TESTDRAWINGSERVICE_H

#include <cppunit/extensions/HelperMacros.h>

class TestDrawingService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestDrawingService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_DescribeDrawing);
    CPPUNIT_TEST(TestCase_EnumerateSections);
    CPPUNIT_TEST(TestCase_EnumerateSectionResources);
    CPPUNIT_TEST(TestCase_GetCoordinateSpace);
    CPPUNIT_TEST(TestCase_EnumerateLayers);
    CPPUNIT_TEST(TestCase_GetDrawing);
    CPPUNIT_TEST(TestCase_GetLayer);
    CPPUNIT_TEST(TestCase_GetSection);
    CPPUNIT_TEST(TestCase_GetSectionResource);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    TestDrawingService();
    ~TestDrawingService();

    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_DescribeDrawing();
    void TestCase_EnumerateSections();
    void TestCase_EnumerateSectionResources();
    void TestCase_GetCoordinateSpace();
    void TestCase_EnumerateLayers();
    void TestCase_GetDrawing();
    void TestCase_GetLayer();
    void TestCase_GetSection();
    void TestCase_GetSectionResource();

private:
    Ptr<MgDrawingService> m_svcDrawing;
};

#endif // _TESTDRAWINGSERVICE_H
