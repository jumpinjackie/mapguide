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

#ifndef _TESTFEATURESERVICE_H
#define _TESTFEATURESERVICE_H

#include <cppunit/extensions/HelperMacros.h>

class TestFeatureService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestFeatureService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_GetFeatureProviders);
    CPPUNIT_TEST(TestCase_TestConnectionSDFProvider);
    CPPUNIT_TEST(TestCase_TestFdoConnectionManager);
    CPPUNIT_TEST(TestCase_GetConnectionPropertyValues);
    CPPUNIT_TEST(TestCase_GetCapabilities);
    CPPUNIT_TEST(TestCase_TestConnectionResourceIdentifier);
    CPPUNIT_TEST(TestCase_GetSchemas);
    CPPUNIT_TEST(TestCase_GetClasses);
    CPPUNIT_TEST(TestCase_GetClassDefinition);
    CPPUNIT_TEST(TestCase_DescribeSchema);
    CPPUNIT_TEST(TestCase_ApplySchema);
    CPPUNIT_TEST(TestCase_SelectFeatures);
    CPPUNIT_TEST(TestCase_SelectAggregate);
//    CPPUNIT_TEST(TestCase_UpdateFeatures);
//  TODO write test case when know how to make command collection
    CPPUNIT_TEST(TestCase_ExecuteSqlQuery);
    CPPUNIT_TEST(TestCase_ExecuteSqlNonQuery);
    CPPUNIT_TEST(TestCase_GetSpatialContexts);
    CPPUNIT_TEST(TestCase_GetLongTransactions);
    CPPUNIT_TEST(TestCase_SetLongTransaction);
    CPPUNIT_TEST(TestCase_GetFeatures);
    CPPUNIT_TEST(TestCase_CloseFeatureReader);
    CPPUNIT_TEST(TestCase_GetSqlRows);
    CPPUNIT_TEST(TestCase_CloseSqlReader);
    CPPUNIT_TEST(TestCase_GetRaster);
    CPPUNIT_TEST(TestCase_GetDataRows);
    CPPUNIT_TEST(TestCase_CloseDataReader);
    CPPUNIT_TEST(TestCase_JoinFeatures);
    CPPUNIT_TEST(TestCase_JoinFeaturesChainedInner1ToMany);
    CPPUNIT_TEST(TestCase_CreateFeatureSource);
    CPPUNIT_TEST(TestCase_BenchmarkSelectFeatures);
    CPPUNIT_TEST(TestCase_ConcurrentAccess);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_GetFeatureProviders();
    void TestCase_TestConnectionSDFProvider();
    void TestCase_TestFdoConnectionManager();
    void TestCase_GetConnectionPropertyValues();
    void TestCase_GetCapabilities();
    void TestCase_TestConnectionProviderConnectionString();
    void TestCase_TestConnectionResourceIdentifier();
    void TestCase_GetSchemas();
    void TestCase_GetClasses();
    void TestCase_GetClassDefinition();
    void TestCase_DescribeSchema();
    void TestCase_ApplySchema();
    void TestCase_SelectFeatures();
    void TestCase_SelectAggregate();
//    void TestCase_UpdateFeatures();
    void TestCase_ExecuteSqlQuery();
    void TestCase_ExecuteSqlNonQuery();
    void TestCase_GetSpatialContexts();
    void TestCase_GetLongTransactions();
    void TestCase_SetLongTransaction();
    void TestCase_GetFeatures();
    void TestCase_CloseFeatureReader();
    void TestCase_GetSqlRows();
    void TestCase_CloseSqlReader();
    void TestCase_GetRaster();
    void TestCase_GetDataRows();
    void TestCase_CloseDataReader();
    void TestCase_JoinFeatures();
    void TestCase_JoinFeaturesChainedInner1ToMany();
    void TestCase_CreateFeatureSource();
    void TestCase_BenchmarkSelectFeatures();
    void TestCase_ConcurrentAccess();
};

#endif // _TESTFEATURESERVICE_H
