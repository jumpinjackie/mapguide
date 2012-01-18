//
//  Copyright (C) 2007-2010 by Autodesk, Inc.
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

#ifndef TestKmlService_H_
#define TestKmlService_H_

#include <cppunit/extensions/HelperMacros.h>

class TestKmlService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestKmlService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_GetMapKml);
    CPPUNIT_TEST(TestCase_GetMapKmz);
    CPPUNIT_TEST(TestCase_GetLayerKml);
    CPPUNIT_TEST(TestCase_GetFeaturesKml);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    TestKmlService();
    ~TestKmlService();

    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_GetMapKml();
    void TestCase_GetMapKmz();
    void TestCase_GetLayerKml();
    void TestCase_GetFeaturesKml();

private:
    void PublishTheResources();
    STRING RemoveSessionIds(CREFSTRING content);
    void SaveToFile(CREFSTRING content, CREFSTRING filename);
    STRING LoadFromFile(CREFSTRING filename);
    bool CompareContent(MgByteReader* reader, CREFSTRING referenceFile);

    Ptr<MgSiteConnection> m_siteConnection;
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgKmlService> m_svcKml;
};

#endif // TestKmlService_H_
