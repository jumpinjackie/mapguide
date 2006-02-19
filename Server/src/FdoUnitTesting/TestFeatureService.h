//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
    CPPUNIT_TEST(TestStart);

    CPPUNIT_TEST(TestCase_GetFeatureProviders);
    CPPUNIT_TEST(TestCase_GetServerFeatureProviders);
    CPPUNIT_TEST(TestCase_TestFdoConnectionManager);
    CPPUNIT_TEST(TestCase_TestConnectionSDFProvider);
    CPPUNIT_TEST(TestCase_TestConnectionShpProvider);
    CPPUNIT_TEST(TestCase_TestConnectionODBCProvider);
    CPPUNIT_TEST(TestCase_TestConnectionOracleProvider);
    CPPUNIT_TEST(TestCase_TestConnectionArcSdeProvider);
    CPPUNIT_TEST(TestCase_TestConnectionRasterProvider);

    // Test SDF provider
    CPPUNIT_TEST(TestCase_SdfProvider_GetCapabilities);
    CPPUNIT_TEST(TestCase_SdfProvider_FeatureConnection);
    CPPUNIT_TEST(TestCase_SdfProvider_DescribeSchema);
    CPPUNIT_TEST(TestCase_SdfProvider_SelectFeatures);
    CPPUNIT_TEST(TestCase_SdfProvider_GetSpatialContext);
    CPPUNIT_TEST(TestCase_SdfProvider_GetLongTransactions);
    CPPUNIT_TEST(TestCase_SdfProvider_TestSqlCommands);

    // Test SHP provider
    CPPUNIT_TEST(TestCase_ShpProvider_GetCapabilities);
    CPPUNIT_TEST(TestCase_ShpProvider_FeatureConnection);
    CPPUNIT_TEST(TestCase_ShpProvider_DescribeSchema);
    CPPUNIT_TEST(TestCase_ShpProvider_SelectFeatures);
    CPPUNIT_TEST(TestCase_ShpProvider_GetSpatialContext);
    CPPUNIT_TEST(TestCase_ShpProvider_GetLongTransactions);
    CPPUNIT_TEST(TestCase_ShpProvider_TestSqlCommands);

    // Test ODBC provider
    CPPUNIT_TEST(TestCase_OdbcProvider_GetCapabilities);
    CPPUNIT_TEST(TestCase_OdbcProvider_FeatureConnection);
    CPPUNIT_TEST(TestCase_OdbcProvider_DescribeSchema);
    CPPUNIT_TEST(TestCase_OdbcProvider_SelectFeatures);
    CPPUNIT_TEST(TestCase_OdbcProvider_GetSpatialContext);
    CPPUNIT_TEST(TestCase_OdbcProvider_GetLongTransactions);
    CPPUNIT_TEST(TestCase_OdbcProvider_TestSqlCommands);

    // Test Oracle provider
    CPPUNIT_TEST(TestCase_OracleProvider_GetCapabilities);
    CPPUNIT_TEST(TestCase_OracleProvider_FeatureConnection);
    CPPUNIT_TEST(TestCase_OracleProvider_DescribeSchema);
    CPPUNIT_TEST(TestCase_OracleProvider_SelectFeatures);
    CPPUNIT_TEST(TestCase_OracleProvider_GetSpatialContext);
    CPPUNIT_TEST(TestCase_OracleProvider_GetLongTransactions);
    CPPUNIT_TEST(TestCase_OracleProvider_TestSqlCommands);

    // Test ArcSDE provider
    CPPUNIT_TEST(TestCase_ArcSdeProvider_GetCapabilities);
    CPPUNIT_TEST(TestCase_ArcSdeProvider_FeatureConnection);
    CPPUNIT_TEST(TestCase_ArcSdeProvider_DescribeSchema);
    CPPUNIT_TEST(TestCase_ArcSdeProvider_SelectFeatures);
    CPPUNIT_TEST(TestCase_ArcSdeProvider_GetSpatialContext);
    CPPUNIT_TEST(TestCase_ArcSdeProvider_GetLongTransactions);
    CPPUNIT_TEST(TestCase_ArcSdeProvider_TestSqlCommands);

    // Test Raster provider
    CPPUNIT_TEST(TestCase_RasterProvider_GetCapabilities);
    CPPUNIT_TEST(TestCase_RasterProvider_FeatureConnection);
    CPPUNIT_TEST(TestCase_RasterProvider_DescribeSchema);
    CPPUNIT_TEST(TestCase_RasterProvider_SelectFeatures);
    CPPUNIT_TEST(TestCase_RasterProvider_GetSpatialContext);
    CPPUNIT_TEST(TestCase_RasterProvider_GetLongTransactions);
    CPPUNIT_TEST(TestCase_RasterProvider_TestSqlCommands);


    CPPUNIT_TEST(TestCase_GetConnectionPropertyValue);

    CPPUNIT_TEST(TestEnd);
    CPPUNIT_TEST_SUITE_END();

public:
    void SetUp();
    void TearDown();
    void TestStart();
    void TestEnd();

    // Unit test helper functions
    bool WriteToDisk(const char* filename, const char* buf);
    bool CheckFile(const char* filename, const char* baselineName);
    void AddFeatureSource(MgResourceIdentifier* featureResId, STRING connectionString, STRING resourceDataName, STRING resourceData);
    bool TestFeatureConnection(MgResourceIdentifier* featureResId);
    void TestDescribeSchema(STRING id);
    INT32 TestGetSchema(MgFeatureService* service, MgResourceIdentifier* featureResId, MgStringCollection* &schemas);
    INT32 TestGetClasses(MgFeatureService* service, MgResourceIdentifier* featureResId, MgStringCollection* &cls, CREFSTRING schemaName);
    INT32 TestGetClassDefinition(MgFeatureService* service, MgResourceIdentifier* featureResId, CREFSTRING schemaName, CREFSTRING className, INT32 index);
    bool TestGetRaster(MgResourceIdentifier* featureResId, MgReader* featureReader, CREFSTRING rasterPropName);
    void TestSelectFeatures(MgFeatureService* service, MgResourceIdentifier* featureResId, CREFSTRING className,REFSTRING strProp,REFSTRING intProp,REFSTRING geomProp,REFSTRING rasterProp,INT32 index,bool onlyIfRaster=false);
    void TestSpatialFilter(MgFeatureService* service, MgResourceIdentifier* featureResId, CREFSTRING className, CREFSTRING strProp, CREFSTRING intProp, CREFSTRING geomProp);
    void TestSelectAggregateDistinct(MgFeatureService* service, MgResourceIdentifier* featureResId, CREFSTRING className);
    void TestSelectAggregateString(MgFeatureService* service, MgResourceIdentifier* featureResId, CREFSTRING className, CREFSTRING strProp);
    void TestSelectAggregateInt(MgFeatureService* service, MgResourceIdentifier* featureResId, CREFSTRING className, CREFSTRING intProp);
    void TestSelectAggregateGeometry(MgFeatureService* service, MgResourceIdentifier* featureResId, CREFSTRING className, CREFSTRING geomProp);
    void TestSelectAggregateRaster(MgFeatureService* service, MgResourceIdentifier* featureResId, CREFSTRING className, CREFSTRING rasterProp);
    void TestSelectAggregate(MgFeatureService* service, MgResourceIdentifier* featureResId, CREFSTRING className,CREFSTRING strProp,CREFSTRING intProp,CREFSTRING geomProp,CREFSTRING rasterProp);
    void TestSelectFeatures(MgFeatureService* service, MgResourceIdentifier* featureResId, CREFSTRING schemaName);
    void TestSelectFeatures(STRING id);
    void TestGetCapabilities(STRING provider);
    void TestGetSpatialContext(STRING id);
    void TestGetLongTransactions(STRING id);
    void TestSqlNonQuery(CREFSTRING id, CREFSTRING sqlCommand);
    void TestSqlToXml(MgResourceIdentifier* featureResId, MgSqlDataReader* sqlReader);
    void TestSqlReader(MgResourceIdentifier* featureResId, MgSqlDataReader* sqlReader);
    void TestSqlQuery(CREFSTRING id, CREFSTRING sqlCommand);
    void TestSqlCommands(CREFSTRING id);
    void TestGeometry(CREFSTRING dataDir);

    // Unit tests
    void TestCase_GetFeatureProviders();
    void TestCase_GetServerFeatureProviders();
    void TestCase_TestConnectionSDFProvider();
    void TestCase_TestConnectionODBCProvider();
    void TestCase_TestConnectionOracleProvider();
    void TestCase_TestConnectionArcSdeProvider();
    void TestCase_TestConnectionRasterProvider();
    void TestCase_TestConnectionShpProvider();
    void TestCase_TestFdoConnectionManager();

    void TestCase_SdfProvider_GetCapabilities();
    void TestCase_SdfProvider_FeatureConnection();
    void TestCase_SdfProvider_DescribeSchema();
    void TestCase_SdfProvider_SelectFeatures();
    void TestCase_SdfProvider_GetSpatialContext();
    void TestCase_SdfProvider_GetLongTransactions();
    void TestCase_SdfProvider_TestSqlCommands();

    void TestCase_ShpProvider_GetCapabilities();
    void TestCase_ShpProvider_FeatureConnection();
    void TestCase_ShpProvider_DescribeSchema();
    void TestCase_ShpProvider_SelectFeatures();
    void TestCase_ShpProvider_GetSpatialContext();
    void TestCase_ShpProvider_GetLongTransactions();
    void TestCase_ShpProvider_TestSqlCommands();

    void TestCase_OdbcProvider_GetCapabilities();
    void TestCase_OdbcProvider_FeatureConnection();
    void TestCase_OdbcProvider_DescribeSchema();
    void TestCase_OdbcProvider_SelectFeatures();
    void TestCase_OdbcProvider_GetSpatialContext();
    void TestCase_OdbcProvider_GetLongTransactions();
    void TestCase_OdbcProvider_TestSqlCommands();

    void TestCase_OracleProvider_GetCapabilities();
    void TestCase_OracleProvider_FeatureConnection();
    void TestCase_OracleProvider_DescribeSchema();
    void TestCase_OracleProvider_SelectFeatures();
    void TestCase_OracleProvider_GetSpatialContext();
    void TestCase_OracleProvider_GetLongTransactions();
    void TestCase_OracleProvider_TestSqlCommands();

    void TestCase_ArcSdeProvider_GetCapabilities();
    void TestCase_ArcSdeProvider_FeatureConnection();
    void TestCase_ArcSdeProvider_DescribeSchema();
    void TestCase_ArcSdeProvider_SelectFeatures();
    void TestCase_ArcSdeProvider_GetSpatialContext();
    void TestCase_ArcSdeProvider_GetLongTransactions();
    void TestCase_ArcSdeProvider_TestSqlCommands();

    void TestCase_RasterProvider_GetCapabilities();
    void TestCase_RasterProvider_FeatureConnection();
    void TestCase_RasterProvider_DescribeSchema();
    void TestCase_RasterProvider_SelectFeatures();
    void TestCase_RasterProvider_GetSpatialContext();
    void TestCase_RasterProvider_GetLongTransactions();
    void TestCase_RasterProvider_TestSqlCommands();



    void TestCase_GetConnectionPropertyValue();
};

#endif // _TESTFEATURESERVICE_H
