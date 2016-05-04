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

#ifndef _TESTFEATURESERVICE_H
#define _TESTFEATURESERVICE_H

#include <cppunit/extensions/HelperMacros.h>

class TestFeatureService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestFeatureService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_GetFeatureProviders);
    CPPUNIT_TEST(TestCase_TestConnectionSDFProvider);
    //CPPUNIT_TEST(TestCase_TestFdoConnectionManager);
    CPPUNIT_TEST(TestCase_GetConnectionPropertyValues);
    CPPUNIT_TEST(TestCase_GetCapabilities);
    CPPUNIT_TEST(TestCase_TestConnectionResourceIdentifier);
    CPPUNIT_TEST(TestCase_GetSchemas);
    CPPUNIT_TEST(TestCase_GetClasses);
    CPPUNIT_TEST(TestCase_GetClassDefinition);
    CPPUNIT_TEST(TestCase_GetClassDefinitionImmutability);
    CPPUNIT_TEST(TestCase_DescribeSchema);
    CPPUNIT_TEST(TestCase_DescribeSchemaImmutability);
    CPPUNIT_TEST(TestCase_ApplySchema);
    CPPUNIT_TEST(TestCase_CreateFeatureSource);
    CPPUNIT_TEST(TestCase_SelectFeatures);
    CPPUNIT_TEST(TestCase_SelectFeaturesTransformed);
    CPPUNIT_TEST(TestCase_SelectScrollable);
    CPPUNIT_TEST(TestCase_SelectAggregate);
    CPPUNIT_TEST(TestCase_ExtendedFeatureClass);
    CPPUNIT_TEST(TestCase_InsertFeatures);
    CPPUNIT_TEST(TestCase_InsertFeaturesBatch);
    CPPUNIT_TEST(TestCase_UpdateFeatures);
    CPPUNIT_TEST(TestCase_UpdateFeaturesInsert);
    CPPUNIT_TEST(TestCase_UpdateFeaturesPartialFailure);
    CPPUNIT_TEST(TestCase_DeleteFeatures);
//  TODO write test case when know how to make command collection
    CPPUNIT_TEST(TestCase_ExecuteSqlQuery);
    CPPUNIT_TEST(TestCase_ExecuteSqlNonQuery);
    CPPUNIT_TEST(TestCase_GetSpatialContexts);
    CPPUNIT_TEST(TestCase_GetLongTransactions);
    CPPUNIT_TEST(TestCase_SetLongTransaction);
    CPPUNIT_TEST(TestCase_JoinFeatures);
    //Our sortability check has broken this for some reason
    CPPUNIT_TEST(TestCase_JoinFeaturesChainedInner1ToMany);
    CPPUNIT_TEST(TestCase_JoinFdoFeatures);
    CPPUNIT_TEST(TestCase_BenchmarkSelectFeatures);
    //CPPUNIT_TEST(TestCase_ConcurrentAccess);

    //CPPUNIT_TEST(TestCase_SecuredCredentials);
    CPPUNIT_TEST(TestCase_LayerInsertFeatures);
    CPPUNIT_TEST(TestCase_LayerUpdateFeatures);
    CPPUNIT_TEST(TestCase_LayerDeleteFeatures);
    CPPUNIT_TEST(TestCase_LayerSelectScrollable);

    CPPUNIT_TEST(TestCase_BenchmarkSqliteJoin);
    CPPUNIT_TEST(TestCase_BenchmarkSqliteAggregateJoin);
    CPPUNIT_TEST(TestCase_BenchmarkSdfJoin);
    CPPUNIT_TEST(TestCase_BenchmarkMergeSortJoin);
    CPPUNIT_TEST(TestCase_BenchmarkBatchSortedBlockJoin);
    CPPUNIT_TEST(TestCase_BenchmarkNestedLoopsSortedBlockJoin);
    CPPUNIT_TEST(TestCase_BenchmarkNestedLoopsJoin);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_GetFeatureProviders();
    void TestCase_TestConnectionSDFProvider();
    //void TestCase_TestFdoConnectionManager();
    void TestCase_GetConnectionPropertyValues();
    void TestCase_GetCapabilities();
    void TestCase_TestConnectionProviderConnectionString();
    void TestCase_TestConnectionResourceIdentifier();
    void TestCase_GetSchemas();
    void TestCase_GetClasses();
    void TestCase_GetClassDefinition();
    void TestCase_GetClassDefinitionImmutability();
    void TestCase_DescribeSchema();
    void TestCase_DescribeSchemaImmutability();
    void TestCase_ApplySchema();
    void TestCase_SelectFeatures();
    void TestCase_SelectFeaturesTransformed();
    void TestCase_SelectScrollable();
    void TestCase_SelectAggregate();
    void TestCase_UpdateFeaturesInsert();
    void TestCase_UpdateFeaturesPartialFailure();
    void TestCase_InsertFeatures();
    void TestCase_InsertFeaturesBatch();
    void TestCase_UpdateFeatures();
    void TestCase_DeleteFeatures();
    void TestCase_ExecuteSqlQuery();
    void TestCase_ExecuteSqlNonQuery();
    void TestCase_ExtendedFeatureClass();
    void TestCase_GetSpatialContexts();
    void TestCase_GetLongTransactions();
    void TestCase_SetLongTransaction();
    void TestCase_JoinFeatures();
    void TestCase_JoinFeaturesChainedInner1ToMany();
    void TestCase_JoinFdoFeatures();
    void TestCase_CreateFeatureSource();
    void TestCase_BenchmarkSelectFeatures();
    //void TestCase_ConcurrentAccess();

    void TestCase_BenchmarkSdfJoin();
    void TestCase_BenchmarkSqliteJoin();
    void TestCase_BenchmarkSqliteAggregateJoin();
    void TestCase_BenchmarkMergeSortJoin();
    void TestCase_BenchmarkBatchSortedBlockJoin();
    void TestCase_BenchmarkNestedLoopsSortedBlockJoin();
    void TestCase_BenchmarkNestedLoopsJoin();

    void TestCase_SecuredCredentials();
    void TestCase_LayerInsertFeatures();
    void TestCase_LayerUpdateFeatures();
    void TestCase_LayerDeleteFeatures();
    void TestCase_LayerSelectScrollable();

private:
    MgdMap* CreateTestMap();
    static MgResourceIdentifier* CreateFeatureSource(MgFeatureService* featSvc, CREFSTRING name, MgClassDefinition* classDefinition);
    static MgPropertyCollection* Prepare(MgClassDefinition* clsDef);
    static void Insert(MgdFeatureService* featSvc, MgReader* reader, MgResourceIdentifier* fsId, CREFSTRING className, MgPropertyCollection* propVals);
    static bool m_bDumpJoinBenchmarkResults;
};

#endif // _TESTFEATURESERVICE_H
