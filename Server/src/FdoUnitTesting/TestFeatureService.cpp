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

#include "AceCommon.h"
#include "TestFeatureService.h"
#include "../UnitTesting/CppUnitExtensions.h"
#include "ServiceManager.h"
#include "FdoConnectionManager.h"
#include "FeatureService.h"
#include "ServerFeatureService.h"
#include "Fdo.h"

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_REGISTRATION(TestFeatureService);

#ifdef _WIN32
        string outputDirectory = "..\\UnitTestFiles\\FeatureService\\Results\\";
        string baselineDirectory = "..\\UnitTestFiles\\FeatureService\\ExpectedResults\\";
#else
        string outputDirectory = "../../bin/UnitTestFiles/FeatureService/Results/";
        string baselineDirectory = "../../bin/UnitTestFiles/FeatureService/ExpectedResultsLinux/";
#endif

string extension = ".xml";

void TestFeatureService::SetUp()
{
}

void TestFeatureService::TearDown()
{
}

void TestFeatureService::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Feature Service tests.\n")));
}

void TestFeatureService::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nFeature Service tests completed.\n\n")));
}


///----------------------------------------------------------------------------
/// Writes the string to a file.
///----------------------------------------------------------------------------
bool TestFeatureService::WriteToDisk(const char* filename, const char* buf)
{
     FILE* stream = NULL;
      if( (stream  = fopen(filename, "w" )) != NULL )
      {
          fputs(buf,stream);
          fclose( stream );
          return true;
      }
      return false;
}

///----------------------------------------------------------------------------
/// Checks the equality of two files.
/// Returns true if files are equal.
/// Returns false if files are not equal.
/// Returns false if one or both files do not exist.
///----------------------------------------------------------------------------
bool TestFeatureService::CheckFile(const char* filename, const char* baselineName)
{
    bool fileSame = true;

    FILE* testFile = NULL;
    FILE* baseline = NULL;
    int char1;
    int char2;

    if ((testFile = fopen(filename, "r")) != NULL
        && (baseline = fopen(baselineName, "r")) != NULL)
    {
        int nOffset = 0;
        do
        {
            char1 = fgetc(testFile);
            char2 = fgetc(baseline);
            if (char1 != char2)
            {
                printf("\nCheckFile failed.\nSource File  : %s\nBaseline File: %s\nOffset: %d  Actual: '%c' Expected: '%c'\n", filename, baselineName, nOffset, char1, char2);
                // files are not equal
                // either files differ, or one file has reached the end and the other has not.
                return false;
            }

            nOffset++;
        }while(char1 != -1);
    }
    else
    {
        // file cannot be opened
        printf("\nCheckFile failed.\nCan't open either source or baseline file.\n");
        return false;
    }

    return fileSame;
}

///----------------------------------------------------------------------------
/// Adds the feature source pointed to by the connection string.
///----------------------------------------------------------------------------
void TestFeatureService::AddFeatureSource(MgResourceIdentifier* featureResId,
                                          STRING connectionString,
                                          STRING resourceDataName,
                                          STRING resourceData)
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    if(serviceManager == 0)
    {
        throw new MgNullReferenceException(L"TestFeatureService.AddFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
    if (resourceService == 0)
    {
        throw new MgServiceNotAvailableException(L"TestFeatureService.AddFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgByteSource> ptrSource = new MgByteSource(connectionString);
    Ptr<MgByteReader> ptrReader = ptrSource->GetReader();

    // Set credentials
    Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(MgUser::Administrator, L"");
    MgUserInformation::SetCurrentUserInfo(adminUserInfo);

    resourceService->SetResource(featureResId, ptrReader, NULL);

    if (!resourceData.empty())
    {
        Ptr<MgByteSource> ptrSource1 = new MgByteSource(resourceData);
        Ptr<MgByteReader> ptrReader1 = ptrSource1->GetReader();
        resourceService->SetResourceData(featureResId, resourceDataName, L"File", ptrReader1);
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the connection.  It fails if the connection cannot be
/// opened.
///----------------------------------------------------------------------------
bool TestFeatureService::TestFeatureConnection(MgResourceIdentifier* featureResId)
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    if(serviceManager == 0)
    {
        throw new MgNullReferenceException(L"TestFeatureService.TestFeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
    if (pService == 0)
    {
        throw new MgServiceNotAvailableException(L"TestFeatureService.TestFeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    bool canConnect = false;

    // This can throw an exception that should be caught by the caller
    canConnect = pService->TestConnection(featureResId);

    return canConnect;
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case describes the schema.  It fails if the returned
/// description is not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestDescribeSchema(STRING id)
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    if(serviceManager == 0)
    {
        throw new MgNullReferenceException(L"TestFeatureService.TestDescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
    if (pService == 0)
    {
        throw new MgServiceNotAvailableException(L"TestFeatureService.TestDescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> featureResId = new MgResourceIdentifier(id);
    //TODO: Old code leaving it for now until the tests are actually run(KNN)
    //Ptr<MgByteReader> reader = pService->DescribeSchema(featureResId, L"");
    //STRING mimetype = reader->GetMimeType();
    //CPPUNIT_ASSERT(wcscmp(mimetype.c_str(), MgMimeType::Xml.c_str()) == 0);

    Ptr<MgFeatureSchemaCollection> schemaCol = pService->DescribeSchema(featureResId, L"");
    STRING schema = pService->SchemaToXml(schemaCol);

    //string schemaText = MgUtil::GetTextFromReader(reader);
    string schemaText = MgUtil::WideCharToMultiByte(schema);
    STRING name = featureResId->GetName();

    string filename = "DescribeSchema-" + MgUtil::WideCharToMultiByte(name) + extension;
    string outputFile = outputDirectory + filename;
    WriteToDisk(outputFile.c_str(), schemaText.c_str());

    string baseline = baselineDirectory + filename;
    bool describeSchemaFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
    CPPUNIT_ASSERT(describeSchemaFilesEqual);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the schema.  It fails if the returned schema is not
/// the same as the baseline.
///----------------------------------------------------------------------------
INT32 TestFeatureService::TestGetSchema(MgFeatureService* service,
                                        MgResourceIdentifier* featureResId,
                                        MgStringCollection* &schemas)
{
    STRING name = featureResId->GetName();
    INT32 schemaCnt = 0;

    // Write all schemas for the database
    schemas = service->GetSchemas(featureResId);
    Ptr<MgByteReader> reader = schemas->ToXml();
    string schemaText = MgUtil::GetTextFromReader(reader);

    string filename = "GetSchemas-" + MgUtil::WideCharToMultiByte(name) + extension;
    string outputFile = outputDirectory + filename;
    WriteToDisk(outputFile.c_str(), schemaText.c_str());

    string baseline = baselineDirectory + filename;
    bool getSchemaFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
    CPPUNIT_ASSERT(getSchemaFilesEqual);

    schemaCnt = schemas->GetCount();

    return schemaCnt;
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the classes for the schema.  It fails if the returned
/// classes are not the same as the baseline.
///----------------------------------------------------------------------------
INT32 TestFeatureService::TestGetClasses(MgFeatureService* service,
                                         MgResourceIdentifier* featureResId,
                                         MgStringCollection* &cls,
                                         CREFSTRING schemaName)
{
    STRING name = featureResId->GetName();
    INT32 classCnt = 0;

    Ptr<MgStringCollection> classes = service->GetClasses(featureResId, schemaName);

    Ptr<MgByteReader> reader = classes->ToXml();
    string classText = MgUtil::GetTextFromReader(reader);

    string filename = "GetClasses-" + MgUtil::WideCharToMultiByte(name) + "_"
        + MgUtil::WideCharToMultiByte(schemaName) + extension;
    string outputFile = outputDirectory + filename;
    WriteToDisk(outputFile.c_str(), classText.c_str());

    string baseline = baselineDirectory + filename;
    bool getClassesFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
    CPPUNIT_ASSERT(getClassesFilesEqual);

    cls = SAFE_ADDREF((MgStringCollection*)classes);
    classCnt = classes->GetCount();

    return classCnt;
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the class definition.  It fails if the returned
/// definition is not the same as the baseline.
///----------------------------------------------------------------------------
INT32 TestFeatureService::TestGetClassDefinition(MgFeatureService* service,
                                                 MgResourceIdentifier* featureResId,
                                                 CREFSTRING schemaName,
                                                 CREFSTRING className,
                                                 INT32 index)
{
    STRING name = featureResId->GetName();
    INT32 classCnt = 0;

    Ptr<MgClassDefinition> classDef = service->GetClassDefinition(featureResId, schemaName, className);

    string classText = "";
    classDef->ToXml(classText); //TODO Internal API member call.  Should be removed.
    //Ptr<MgByteReader> reader = classDef->ToXml();  //(Missing function)

    char buffer[32];
#ifdef WIN32
    _itoa(index, &buffer[0], 10);
#else
    snprintf(&buffer[0], 10, "%d", index);
#endif

    string filename = "GetClassDefinition-" + MgUtil::WideCharToMultiByte(name) + "_"
        + MgUtil::WideCharToMultiByte(schemaName) +
        "_" + string(&buffer[0]) + extension;
    string outputFile = outputDirectory + filename;
    WriteToDisk(outputFile.c_str(), classText.c_str());

    string baseline = baselineDirectory + filename;
    bool getClassDefinitionFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());

    CPPUNIT_ASSERT(getClassDefinitionFilesEqual);

    return classCnt;
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the raster property.  It fails if the returned
/// property is not the same as the baseline.
///----------------------------------------------------------------------------
bool TestFeatureService::TestGetRaster(MgResourceIdentifier* featureResId,
                                       MgReader* featureReader,
                                       CREFSTRING rasterPropName)
{
    bool isDone = true;

    STRING name = featureResId->GetName();

    if (rasterPropName.empty()) // No Raster property

    {
        return false; //not done
    }

    int cnt = 0;
    while (featureReader->ReadNext())
    {
        MgRaster* raster = featureReader->GetRaster(rasterPropName);

        raster->SetImageXSize(400);
        raster->SetImageYSize(600);

        Ptr<MgByteReader> reader = raster->GetStream();

        char buf[32];
        sprintf(buf, "%d", cnt);

        string filename ="Image-"
            + MgUtil::WideCharToMultiByte(name) + "_"
            + string(buf) + ".img";
        string outputFile = outputDirectory + filename;
        string baseline = baselineDirectory + filename;

        FILE* stream = NULL;
        if ( (stream = fopen(outputFile.c_str(), "a+")) != NULL )
        {
            int size = 4096;
            char buffer[4096];
            INT32 bytesRead = 0;
            do
            {
                bytesRead = reader->Read((BYTE_ARRAY_OUT)&buffer, size);
                if (bytesRead > 0)
                {
                    fwrite(buffer, bytesRead, 1, stream);
                }
            } while (bytesRead > 0);

            fclose(stream);
        }
        cnt++;

        bool getRasterFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
        //TODO image files are not the same
        //CPPUNIT_ASSERT(getRasterFilesEqual);
    }
    featureReader->Close();

    return isDone;

}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case selects the features for the class.  It fails if the
/// returned features are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSelectFeatures(MgFeatureService* service,
                                            MgResourceIdentifier* featureResId,
                                            CREFSTRING className,
                                            REFSTRING strProp,
                                            REFSTRING intProp,
                                            REFSTRING geomProp,
                                            REFSTRING rasterProp,
                                            INT32 index,
                                            bool onlyIfRaster)
{
    STRING name = featureResId->GetName();

    Ptr<MgFeatureReader> featureReader = service->SelectFeatures(featureResId, className, NULL);
    Ptr<MgClassDefinition> classDef = featureReader->GetClassDefinition();

    if (onlyIfRaster)
    {
        rasterProp = classDef->GetRasterPropertyName();
        TestGetRaster(featureResId, featureReader, rasterProp);
    }
    else
    {

        // Find a string and integer property on which we can execute select aggregate
        STRING propName = L"";

        Ptr<MgReader> awBaseReader = SAFE_ADDREF((MgReader*)featureReader);
        INT32 propCnt = awBaseReader->GetPropertyCount();
        for (INT32 kk = 0; kk < propCnt; kk++)
        {
            propName = awBaseReader->GetPropertyName(kk);
            INT16 type = awBaseReader->GetPropertyType(propName);
            if (type == MgPropertyType::String)
            {
                strProp = propName;
            }
            if (type == MgPropertyType::Int32 ||
                type == MgPropertyType::Double ||
                type == MgPropertyType::Single)
            {
                intProp = propName;
            }
            if (type == MgPropertyType::Geometry)
            {
                geomProp = propName;
            }
            if (type == MgPropertyType::Raster)
            {
                rasterProp = propName;
            }

        }

        //TODO this call does not work, and throws a MgInvalidOpearationException
        //Ptr<MgByteReader> reader = featureReader->ToXml();

        featureReader->Close();

        //string str = MgUtil::GetTextFromReader(reader);
        string str = "MgInvalidOperationException";

        char clsId[65];
#ifdef WIN32
        _itoa(index, clsId, 10);
#else
        snprintf(clsId, 10, "%d", index);
#endif

        string clsName = MgUtil::WideCharToMultiByte(className);

        int idx = (int)clsName.find(":");
        if (idx >= 0)
        {
            clsName = clsName.substr(idx+1);
        }

        string filename = "SelectFeatures-"
            + MgUtil::WideCharToMultiByte(name) + "_"
            + string(clsId) + "_" + clsName + extension;
        string outputFile = outputDirectory + filename;
        WriteToDisk(outputFile.c_str(), str.c_str());

        string baseline = baselineDirectory + filename;
        bool selectFeaturesFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());

        CPPUNIT_ASSERT(selectFeaturesFilesEqual);
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the spatial filter.  It fails if the returned
/// features are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSpatialFilter(MgFeatureService* service,
                                           MgResourceIdentifier* featureResId,
                                           CREFSTRING className,
                                           CREFSTRING strProp,
                                           CREFSTRING intProp,
                                           CREFSTRING geomProp)
{
    STRING name = featureResId->GetName();

    std::vector<STRING> strCol;
    double minIntCol = 0, maxIntCol = 0;

    // Execute Select Aggregate on STRING property and write the output
    if (!strProp.empty())

    {
        Ptr<MgFeatureAggregateOptions> strQryOptions = new MgFeatureAggregateOptions();
        STRING cmd1 = L"UNIQUE(" + strProp + L")";
        strQryOptions->AddComputedProperty(L"CUSTOM_PROP", cmd1);

        Ptr<MgDataReader> strDataReader = service->SelectAggregate(featureResId, className, strQryOptions);
        while (strDataReader->ReadNext())
        {
            STRING strVal = strDataReader->GetString(L"CUSTOM_PROP");
            strCol.push_back(strVal);
        }
        strDataReader->Close();
    }

    // Execute Select Aggregate MINIMUM on INT32 property and write the output
    if (!intProp.empty())
    {
        Ptr<MgFeatureAggregateOptions> intQryOptions = new MgFeatureAggregateOptions();
        STRING cmd2 = L"MINIMUM(" + intProp + L")";
        intQryOptions->AddComputedProperty(L"CUSTOM_PROP", cmd2);
        Ptr<MgDataReader> intDataReader = service->SelectAggregate(featureResId, className, intQryOptions);
        while (intDataReader->ReadNext())
        {
            double minIntCol = 0;
            INT32 type = intDataReader->GetPropertyType(L"CUSTOM_PROP");

            switch (type)
            {
                case MgPropertyType::Int32:
                {
                    minIntCol = (double)intDataReader->GetInt32(L"CUSTOM_PROP");
                    break;
                }
                case MgPropertyType::Double:
                {
                    minIntCol = (double)intDataReader->GetDouble(L"CUSTOM_PROP");
                    break;
                }
                case MgPropertyType::Single:
                {
                    minIntCol = (double)intDataReader->GetSingle(L"CUSTOM_PROP");
                    break;
                }
                case MgPropertyType::Int64:
                {
                    minIntCol = (double)intDataReader->GetInt64(L"CUSTOM_PROP");
                    break;
                }
            }
        }
        intDataReader->Close();
    }

    // Execute Select Aggregate MINIMUM on INT32 property and write the output
    if (!intProp.empty())
    {
        Ptr<MgFeatureAggregateOptions> intQryOptions = new MgFeatureAggregateOptions();
        STRING cmd2 = L"MAXIMUM(" + intProp + L")";
        intQryOptions->AddComputedProperty(L"CUSTOM_PROP", cmd2);
        Ptr<MgDataReader> intDataReader = service->SelectAggregate(featureResId, className, intQryOptions);
        while (intDataReader->ReadNext())
        {
            double maxIntCol = 0;
            INT32 type = intDataReader->GetPropertyType(L"CUSTOM_PROP");

            switch (type)
            {
                case MgPropertyType::Int32:
                {
                    maxIntCol = (double)intDataReader->GetInt32(L"CUSTOM_PROP");
                    break;
                }
                case MgPropertyType::Double:
                {
                    maxIntCol = (double)intDataReader->GetDouble(L"CUSTOM_PROP");
                    break;
                }
                case MgPropertyType::Single:
                {
                    maxIntCol = (double)intDataReader->GetSingle(L"CUSTOM_PROP");
                    break;
                }
                case MgPropertyType::Int64:
                {
                    maxIntCol = (double)intDataReader->GetInt64(L"CUSTOM_PROP");
                    break;
                }
            }
        }
        intDataReader->Close();
    }

    // Execute Select Aggregate on Geometry property and write the output
    if (!geomProp.empty())
    {
        Ptr<MgFeatureAggregateOptions> geomQryOptions = new MgFeatureAggregateOptions();
        STRING cmd2 = L"EXTENT(" + geomProp + L")";
        geomQryOptions->AddComputedProperty(L"EXTENT_PROP", cmd2);
        Ptr<MgDataReader> geomDataReader = service->SelectAggregate(featureResId, className, geomQryOptions);

        Ptr<MgGeometry> geomFilter;
        while (geomDataReader->ReadNext())
        {
            Ptr<MgByteReader> byteReader = geomDataReader->GetGeometry(L"EXTENT_PROP");
            MgAgfReaderWriter agfWriter;
            Ptr<MgGeometry> geom = agfWriter.Read(byteReader);
            Ptr<MgEnvelope> envl = geom->Envelope();

            double minX, minY, maxX, maxY;

            Ptr<MgCoordinate> llcoord = envl->GetLowerLeftCoordinate();
            Ptr<MgCoordinate> urcoord = envl->GetUpperRightCoordinate();

            minX = llcoord->GetX();
            minY = llcoord->GetY();

            maxX = urcoord->GetX();
            maxY = urcoord->GetY();

            double filterMinX, filterMinY, filterMaxX, filterMaxY;

            // Create area between 1/4 and 3/4 of extent
            filterMinX = minX + ((maxX - minX)/4);
            filterMinY = minY + ((maxY - minY)/4);

            filterMaxX = minX + 3*((maxX - minX)/4);
            filterMaxY = minY + 3*((maxY - minY)/4);

            MgGeometryFactory factory;

            Ptr<MgCoordinateCollection> coordCol = new MgCoordinateCollection();
            Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXY(filterMinX, filterMinY);
            Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXY(filterMaxX, filterMinY);
            Ptr<MgCoordinate> coord3 = factory.CreateCoordinateXY(filterMaxX, filterMaxY);
            Ptr<MgCoordinate> coord4 = factory.CreateCoordinateXY(filterMinX, filterMaxY);
            Ptr<MgCoordinate> coord5 = factory.CreateCoordinateXY(filterMinX, filterMinY);

            coordCol->Add(coord1);
            coordCol->Add(coord2);
            coordCol->Add(coord3);
            coordCol->Add(coord4);
            coordCol->Add(coord5);

            Ptr<MgLinearRing> outerRing = factory.CreateLinearRing(coordCol);

            Ptr<MgPolygon> polygon = factory.CreatePolygon(outerRing, NULL);
            geomFilter = SAFE_ADDREF((MgPolygon*)polygon);
        }
        geomDataReader->Close();

        Ptr<MgFeatureQueryOptions> qryOptions = new MgFeatureQueryOptions();

        if (minIntCol != 0 && maxIntCol != 0)
        {
            string doubleStr;
            MgUtil::DoubleToString(minIntCol + 20.0, doubleStr);

            STRING filter = intProp + L" > " + MgUtil::MultiByteToWideChar(doubleStr);
            qryOptions->SetFilter(filter);
        }

        if (geomFilter != NULL)

        {
            qryOptions->SetBinaryOperator(false);
            qryOptions->SetSpatialFilter(geomProp,geomFilter,MgFeatureSpatialOperations::EnvelopeIntersects);
        }

        Ptr<MgReader> featureReader = service->SelectFeatures(featureResId, className, qryOptions);

        //TODO this call does not work, and throws a MgInvalidOpearationException
        //Ptr<MgByteReader> byteReader2 = featureReader->ToXml();
        featureReader->Close();

        //string str2 = MgUtil::GetTextFromReader(byteReader2);
        string str2 = "MgInvalidOperationException";
        string clsName = MgUtil::WideCharToMultiByte(className);

        int idx = (int)clsName.find(":");
        if (idx >= 0)
        {
            clsName = clsName.substr(idx+1);
        }

        string filename = "SelectFeatures-"
            + MgUtil::WideCharToMultiByte(name) + "-"
            + "FilterApplied-" + clsName + extension;

        string outputFile = outputDirectory + filename;
        string baseline = baselineDirectory + filename;

        WriteToDisk(outputFile.c_str(), str2.c_str());

        bool testSpatialFilterFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());

        CPPUNIT_ASSERT(testSpatialFilterFilesEqual);
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case selects the aggregate features.  It fails if the returned
/// features are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSelectAggregateDistinct(MgFeatureService* service,
                                                     MgResourceIdentifier* featureResId,
                                                     CREFSTRING className)
{
    STRING name = featureResId->GetName();

    Ptr<MgFeatureAggregateOptions> qryOptions = new MgFeatureAggregateOptions();
    qryOptions->SelectDistinct(true);

    Ptr<MgDataReader> dataReader = service->SelectAggregate(featureResId, className, qryOptions);
    //TODO this call does not work, and throws a MgInvalidOpearationException
    //Ptr<MgByteReader> byteReader = dataReader->ToXml();
    dataReader->Close();
    //string strAggr = MgUtil::GetTextFromReader(byteReader);
    string strAggr = "MgInvalidOperationException";

    string strCls = MgUtil::WideCharToMultiByte(className);
    int index = (int)strCls.find(":");

    string filename = "SelectAggregateDistinct-" + strCls.substr(index+1) + extension;
    string outputFile = outputDirectory + filename;
    WriteToDisk(outputFile.c_str(), strAggr.c_str());

    string baseline = baselineDirectory + filename;

    bool selectAggregateDistinctFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
    CPPUNIT_ASSERT(selectAggregateDistinctFilesEqual);
}

void TestFeatureService::TestSelectAggregateString(MgFeatureService* service,
                                                   MgResourceIdentifier* featureResId,
                                                   CREFSTRING className,
                                                   CREFSTRING strProp)
{
    STRING name = featureResId->GetName();

    // Execute Select Aggregate on STRING property and write the output
    if (!strProp.empty())
    {
        Ptr<MgFeatureAggregateOptions> strQryOptions = new MgFeatureAggregateOptions();
        STRING cmd1 = L"UNIQUE(" + strProp + L")";
        strQryOptions->AddComputedProperty(L"CUSTOM_PROP", cmd1);

        Ptr<MgDataReader> strDataReader = service->SelectAggregate(featureResId, className, strQryOptions);
        Ptr<MgByteReader> strByteReader = strDataReader->ToXml();
        strDataReader->Close();
        string strAggr = MgUtil::GetTextFromReader(strByteReader);

        string filename = "SelectAggregate-" + MgUtil::WideCharToMultiByte(name) + "-"
            + MgUtil::WideCharToMultiByte(strProp) + extension;
        string outputFile = outputDirectory + filename;
        WriteToDisk(outputFile.c_str(), strAggr.c_str());

        string baseline = baselineDirectory + filename;
        bool selectAggregateStringFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
        CPPUNIT_ASSERT(selectAggregateStringFilesEqual);
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case selects the aggregate int features.  It fails if the
/// returned features are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSelectAggregateInt(MgFeatureService* service,
                                                MgResourceIdentifier* featureResId,
                                                CREFSTRING className,
                                                CREFSTRING intProp)
{
    STRING name = featureResId->GetName();

    // Execute Select Aggregate on INT32 property and write the output
    if (!intProp.empty())
    {
        Ptr<MgFeatureAggregateOptions> intQryOptions = new MgFeatureAggregateOptions();
        STRING cmd2 = L"EQUAL_DIST(" + intProp + L",5)";
        intQryOptions->AddComputedProperty(L"CUSTOM_PROP", cmd2);
        Ptr<MgDataReader> intDataReader = service->SelectAggregate(featureResId, className, intQryOptions);
        Ptr<MgByteReader> intByteReader = intDataReader->ToXml();
        intDataReader->Close();
        string strAggr = MgUtil::GetTextFromReader(intByteReader);

        string filename = "SelectAggregate-" + MgUtil::WideCharToMultiByte(name) + "-"
            + MgUtil::WideCharToMultiByte(intProp) + extension;
        string outputFile = outputDirectory + filename;
        string baseline = baselineDirectory + filename;

        WriteToDisk(outputFile.c_str(), strAggr.c_str());
        bool selectAggregateIntFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
        CPPUNIT_ASSERT(selectAggregateIntFilesEqual);
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case selects the aggregate geometry features.  It fails if the
/// returned features are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSelectAggregateGeometry(MgFeatureService* service,
                                                     MgResourceIdentifier* featureResId,
                                                     CREFSTRING className,
                                                     CREFSTRING geomProp)
{
    STRING name = featureResId->GetName();

    // Execute Select Aggregate on Geometry property and write the output
    if (!geomProp.empty())
    {
        Ptr<MgFeatureAggregateOptions> geomQryOptions = new MgFeatureAggregateOptions();
        STRING cmd2 = L"EXTENT(" + geomProp + L")";
        geomQryOptions->AddComputedProperty(L"EXTENT_PROP", cmd2);
        Ptr<MgDataReader> geomDataReader = service->SelectAggregate(featureResId, className, geomQryOptions);
        Ptr<MgByteReader> geomByteReader = geomDataReader->ToXml();
        geomDataReader->Close();

        string strAggr = MgUtil::GetTextFromReader(geomByteReader);

        string filename = "SelectAggregate-Extent" + MgUtil::WideCharToMultiByte(name) + "-"
            + MgUtil::WideCharToMultiByte(geomProp) + extension;
        string outputFile = outputDirectory + filename;

        WriteToDisk(outputFile.c_str(), strAggr.c_str());

        string baseline = baselineDirectory + filename;
        bool selectAggregateGeometryFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
        CPPUNIT_ASSERT(selectAggregateGeometryFilesEqual);
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case selects the aggregate raster features.  It fails if the
/// returned features are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSelectAggregateRaster(MgFeatureService* service,
                                                   MgResourceIdentifier* featureResId,
                                                   CREFSTRING className,
                                                   CREFSTRING rasterProp)
{
    STRING name = featureResId->GetName();

    if (!rasterProp.empty())
    {
        Ptr<MgFeatureAggregateOptions> rasterQryOptions = new MgFeatureAggregateOptions();
        STRING cmd2 = L"MOSAIC(" + rasterProp + L")";
        rasterQryOptions->AddComputedProperty(L"MOSAIC", cmd2);
        Ptr<MgDataReader> rasterDataReader = service->SelectAggregate(featureResId, className, rasterQryOptions);

        TestGetRaster(featureResId, rasterDataReader, L"MOSAIC");

        Ptr<MgByteReader> rasterByteReader = rasterDataReader->ToXml();
        rasterDataReader->Close();



        string strAggr = MgUtil::GetTextFromReader(rasterByteReader);

        string filename = "SelectAggregate-Mosaic" + MgUtil::WideCharToMultiByte(name) + "-"
            + MgUtil::WideCharToMultiByte(rasterProp) + extension;
        string outputFile = outputDirectory + filename;
        string baseline = baselineDirectory + filename;

        WriteToDisk(outputFile.c_str(), strAggr.c_str());


        bool selectAggregateRasterFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
        CPPUNIT_ASSERT(selectAggregateRasterFilesEqual);
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case selects the aggregate features.  It fails if the returne
/// features are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSelectAggregate(MgFeatureService* service,
                                             MgResourceIdentifier* featureResId,
                                             CREFSTRING className,
                                             CREFSTRING strProp,
                                             CREFSTRING intProp,
                                             CREFSTRING geomProp,
                                             CREFSTRING rasterProp)
{
    STRING name = featureResId->GetName();

    TestSelectAggregateString(service, featureResId, className, strProp);
    TestSelectAggregateInt(service, featureResId, className, intProp);
    TestSelectAggregateGeometry(service, featureResId, className, geomProp);
    TestSelectAggregateRaster(service, featureResId, className, rasterProp);
    TestSelectAggregateDistinct(service, featureResId, className);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case selects the features for the schema.  It fails if the
/// returned features are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSelectFeatures(MgFeatureService* service,
                                            MgResourceIdentifier* featureResId,
                                            CREFSTRING schemaName)
{
    // Write all classes for a schema
    MgStringCollection* classes = NULL;
    INT32 cnt = TestGetClasses(service, featureResId, classes, schemaName);

    // We just do it for 3 classes
    if (cnt > 3)
    {
        cnt = 3;
    }

    for (int i = 0; i < cnt; i++)
    {
        //Write class definition
        STRING strProp, intProp, geomProp, rasterProp;

        STRING clsName = classes->GetItem(i);
        TestGetClassDefinition(service,featureResId, schemaName, clsName, i);
        TestSelectFeatures(service,featureResId, clsName, strProp, intProp, geomProp, rasterProp, i, true);
        TestSelectFeatures(service,featureResId, clsName, strProp, intProp, geomProp, rasterProp, i);
        TestSelectAggregate(service,featureResId, clsName, strProp, intProp, geomProp, rasterProp);
        TestSpatialFilter(service,featureResId, clsName, strProp, intProp, geomProp);
    }

    SAFE_RELEASE(classes);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case selects features.  It fails if the returned features are
/// not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSelectFeatures(STRING id)
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    if(serviceManager == 0)
    {
        throw new MgNullReferenceException(L"TestFeatureService.TestSelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
    if (pService == 0)
    {
        throw new MgServiceNotAvailableException(L"TestFeatureService.TestSelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> featureResId = new MgResourceIdentifier(id);
    STRING name = featureResId->GetName();

    MgStringCollection* schemas = NULL;
    INT32 schemaCnt = TestGetSchema(pService, featureResId, schemas);

    if (schemaCnt > 0)
    {
        // Execute only two schema
        if (schemaCnt > 2)
        {
            schemaCnt = 2;
        }

        for (INT32 jj = 0; jj < schemaCnt; jj++)

        {
            STRING schemaName = schemas->GetItem(jj);
            TestSelectFeatures(pService, featureResId, schemaName);
        }
    }
    else
    {
        // TODO: remove this call.  Currently ODBC does not return default schema
        TestSelectFeatures(pService, featureResId, L"");
    }

    SAFE_RELEASE(schemas);

}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the provider's capabilities.  It fails if the returned
/// capabilities are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestGetCapabilities(STRING provider)
{
    STRING str;

    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    if(serviceManager == 0)
    {
        throw new MgNullReferenceException(L"TestFeatureService.TestGetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
    if (pService == 0)
    {
        throw new MgServiceNotAvailableException(L"TestFeatureService.TestGetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgByteReader> reader = pService->GetCapabilities(provider);
    STRING mimetype = reader->GetMimeType();
    CPPUNIT_ASSERT(wcscmp(mimetype.c_str(), MgMimeType::Xml.c_str()) == 0);
    string capabilities = MgUtil::GetTextFromReader(reader);

    string filename = "GetCapabilities-" + MgUtil::WideCharToMultiByte(provider) + extension;
    string outputFile = outputDirectory + filename;
    WriteToDisk(outputFile.c_str(), capabilities.c_str());

    string baseline = baselineDirectory + filename;
    bool getCapabilitiesFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
    CPPUNIT_ASSERT(getCapabilitiesFilesEqual);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the spatial context.  It fails if the returned context
/// is not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestGetSpatialContext(STRING id)
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    if(serviceManager == 0)
    {
        throw new MgNullReferenceException(L"TestFeatureService.TestGetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
    if (pService == 0)
    {
        throw new MgServiceNotAvailableException(L"TestFeatureService.TestGetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> featureResId = new MgResourceIdentifier(id);
    Ptr<MgSpatialContextReader> spatialReader = pService->GetSpatialContexts(featureResId);

    Ptr<MgByteReader> reader = spatialReader->ToXml();
    string str = MgUtil::GetTextFromReader(reader);

    STRING name = featureResId->GetName();
    string filename = "GetSpatialContext-"
        + MgUtil::WideCharToMultiByte(name) + extension;
    string outputFile = outputDirectory + filename;
    WriteToDisk(outputFile.c_str(), str.c_str());

    string baseline = baselineDirectory + filename;
    bool getSpatialContextFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
    CPPUNIT_ASSERT(getSpatialContextFilesEqual);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the long transactions.  It fails if the returned
/// transactions are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestGetLongTransactions(STRING id)
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    if(serviceManager == 0)
    {
        throw new MgNullReferenceException(L"TestFeatureService.TestGetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
    if (pService == 0)
    {
        throw new MgServiceNotAvailableException(L"TestFeatureService.TestGetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> featureResId = new MgResourceIdentifier(id);
    Ptr<MgLongTransactionReader> reader = pService->GetLongTransactions(featureResId);

    Ptr<MgByteReader> byteReader = reader->ToXml();
    string str = MgUtil::GetTextFromReader(byteReader);

    STRING name = featureResId->GetName();

    string filename = "GetLongTransactions-" + MgUtil::WideCharToMultiByte(name) + extension;
    string outputFile = outputDirectory + filename;
    WriteToDisk(outputFile.c_str(), str.c_str());

    string baseline = baselineDirectory + filename;
    bool getLongTransactionFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
    //CPPUNIT_ASSERT(getLongTransactionFilesEqual);
    //TODO creation date is different in baseline
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the sql non-select queries.  It fails if the returned
/// information is not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSqlNonQuery(CREFSTRING id, CREFSTRING sqlCommand)
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    if(serviceManager == 0)
    {
        throw new MgNullReferenceException(L"TestFeatureService.TestSqlNonQuery", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
    if (pService == 0)
    {
        throw new MgServiceNotAvailableException(L"TestFeatureService.TestSqlNonQuery", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> featureResId = new MgResourceIdentifier(id);
    INT32 rowsReturned = pService->ExecuteSqlNonQuery(featureResId, sqlCommand);

}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case changes sql data to xml.  It fails if the xml is not the
/// same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestSqlToXml(MgResourceIdentifier* featureResId, MgSqlDataReader* sqlReader)
{
    //TODO this call does not work, and throws a MgInvalidOpearationException
    //Ptr<MgByteReader> reader = sqlReader->ToXml();


    //string str = MgUtil::GetTextFromReader(reader);
    string str = "MgInvalidOperationException";
    STRING name = featureResId->GetName();

    string filename = "SelectSql-" + MgUtil::WideCharToMultiByte(name) + extension;
    string outputFile = outputDirectory + filename;
    WriteToDisk(outputFile.c_str(), str.c_str());

    string baseline = baselineDirectory + filename;
    bool sqlToXmlFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
    CPPUNIT_ASSERT(sqlToXmlFilesEqual);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the sql reader.
///----------------------------------------------------------------------------
void TestFeatureService::TestSqlReader(MgResourceIdentifier* featureResId, MgSqlDataReader* sqlReader)
{
    INT32 cnt = sqlReader->GetPropertyCount();

    INT32 recCnt = 0;
    while (sqlReader->ReadNext())
    {
        for (int i = 0; i < cnt; i++)
        {
            STRING name = sqlReader->GetPropertyName(i);
            INT16 type = sqlReader->GetPropertyType(name);

            switch (type)
            {
            case MgPropertyType::String:
                {
                    STRING strVal = sqlReader->GetString(name);
                    break;
                }
            case MgPropertyType::Int32:
                {
                    INT32 int32Val = sqlReader->GetInt32(name);
                    break;
                }
            case MgPropertyType::Double:
                {
                    double doubleVal = sqlReader->GetDouble(name);
                    break;
                }
            }
        }
        recCnt++;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the sql select query.
///----------------------------------------------------------------------------
void TestFeatureService::TestSqlQuery(CREFSTRING id, CREFSTRING sqlCommand)
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    if(serviceManager == 0)
    {
        throw new MgNullReferenceException(L"TestFeatureService.TestSqlQuery", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
    if (pService == 0)
    {
        throw new MgServiceNotAvailableException(L"TestFeatureService.TestSqlQuery", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgResourceIdentifier> featureResId = new MgResourceIdentifier(id);
    MgSqlDataReader* sqlReader1 = pService->ExecuteSqlQuery(featureResId, sqlCommand);
    TestSqlReader(featureResId, sqlReader1);

    MgSqlDataReader* sqlReader2 = pService->ExecuteSqlQuery(featureResId, sqlCommand);
    TestSqlToXml(featureResId, sqlReader2);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the sql commands.
///----------------------------------------------------------------------------
void TestFeatureService::TestSqlCommands(CREFSTRING id)
{
    TestSqlNonQuery(id, L"DROP TABLE TUX_TEST");

    TestSqlNonQuery(id, L"CREATE TABLE TUX_TEST ( ID NUMBER, NAME VARCHAR(32) )");
    TestSqlNonQuery(id, L"INSERT INTO TUX_TEST (ID, NAME) VALUES ( 1, 'DUMMY1' )");
    TestSqlNonQuery(id, L"INSERT INTO TUX_TEST (ID, NAME) VALUES ( 2, 'DUMMY2' )");
    TestSqlNonQuery(id, L"INSERT INTO TUX_TEST (ID, NAME) VALUES ( 3, 'DUMMY3' )");
    TestSqlNonQuery(id, L"INSERT INTO TUX_TEST (ID, NAME) VALUES ( 4, 'DUMMY4' )");
    TestSqlNonQuery(id, L"INSERT INTO TUX_TEST (ID, NAME) VALUES ( 5, 'DUMMY5' )");
    TestSqlNonQuery(id, L"INSERT INTO TUX_TEST (ID, NAME) VALUES ( 6, 'DUMMY6' )");
    TestSqlNonQuery(id, L"INSERT INTO TUX_TEST (ID, NAME) VALUES ( 7, 'DUMMY7' )");
    //TestSqlNonQuery(id, L"COMMIT");

    TestSqlQuery(id, L"SELECT * FROM TUX_TEST");
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the feature providers using FDO directly.
/// It fails if FDO is not installed or a connection to the FDO provider could
/// not be made.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetFeatureProviders()
{
    try
    {
        // Test direct FDO connection
        GisPtr<IConnectionManager> pConnectionManager =  FdoFeatureAccessManager::GetConnectionManager();
        if(pConnectionManager)
        {
            try
            {
                printf("\nTesting available providers.\n");

                // Get the list of providers
                GisPtr<IProviderRegistry> fdoProviderRegistry;
                fdoProviderRegistry =  FdoFeatureAccessManager::GetProviderRegistry();
                const FdoProviderCollection* fdoProviderCollection = fdoProviderRegistry->GetProviders();

                // Enumerate the list of providers
                INT32 cnt = fdoProviderCollection->GetCount();
                for (INT32 i = 0; i < cnt; i++)
                {
                    try
                    {
                        GisPtr<FdoProvider> fdoProvider = fdoProviderCollection->GetItem(i);

                        // Get ProviderName
                        GisString* providerName = fdoProvider->GetName();

                        printf("  \"%ls\" - ", providerName);

                        // Try and connect to the provider
                        GisPtr<FdoIConnection> fdoConnection = pConnectionManager->CreateConnection(providerName);
                        printf("Successfully created connection");
                    }
                    catch(GisException* e)
                    {
                        printf("Failed - GisException: %ls", e->GetExceptionMessage());
                        GIS_SAFE_RELEASE(e);
                    }

                    printf("\n");
                }
            }
            catch(GisException* e)
            {
                printf("GisException: %ls\n", e->GetExceptionMessage());
                GIS_SAFE_RELEASE(e);
            }

            printf("\n");
        }
        else
        {
            // FDO not installed.
            printf("Warning: FDO is not installed.\n");
        }
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the feature providers.  It fails if the returned
/// providers are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetServerFeatureProviders()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetServerFeatureProviders", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetServerFeatureProviders", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Get the list of providers from the server
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
        string featureProviders = MgUtil::GetTextFromReader(byteReader);

        // Check to see if the expected providers are present
        string provider = "OSGeo.SHP.3.0";
        size_t pos = featureProviders.find(provider);
        if(string::npos == pos)
        {
            string message = "Missing required provider: " + provider;
            CPPUNIT_FAIL(message);
        }

        provider = "OSGeo.SDF.3.0";
        pos = featureProviders.find(provider);
        if(string::npos == pos)
        {
            string message = "Missing required provider: " + provider;
            CPPUNIT_FAIL(message);
        }

        provider = "OSGeo.ArcSDE.3.0";
        pos = featureProviders.find(provider);
        if(string::npos == pos)
        {
            string message = "Missing required provider: " + provider;
            CPPUNIT_FAIL(message);
        }

        provider = "OSGeo.WFS.3.0";
        pos = featureProviders.find(provider);
        if(string::npos == pos)
        {
            string message = "Missing required provider: " + provider;
            CPPUNIT_FAIL(message);
        }

        provider = "OSGeo.WMS.3.0";
        pos = featureProviders.find(provider);
        if(string::npos == pos)
        {
            string message = "Missing required provider: " + provider;
            CPPUNIT_FAIL(message);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the odbc provider
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OdbcProvider_GetCapabilities()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OdbcProvider_GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OdbcProvider_GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

        const STRING provider = L"OSGeo.ODBC.3.0";

        // Check to see if ODBC provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            TestGetCapabilities(provider);
        }
        else
        {
            printf("\nSkipping TestCase_OdbcProvider_GetCapabilities(). FDO ODBC is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the odbc provider
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OdbcProvider_FeatureConnection()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OdbcProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OdbcProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

        const STRING provider = L"OSGeo.ODBC.3.0";

        // Check to see if ODBC provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
#ifdef _WIN32
            STRING BboxConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\bboxdb.FeatureSource";
            STRING PointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Points.FeatureSource";
            STRING EmptyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Empty.FeatureSource";
            STRING InvalidConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPoints.FeatureSource";
#else
            STRING BboxConnectionString = L"../UnitTestFiles/FeatureService/ODBC/bboxdb.FeatureSource";
            STRING PointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Points.FeatureSource";
            STRING EmptyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Empty.FeatureSource";
            STRING InvalidConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"../UnitTestFiles/FeatureService/ODBC/COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPoints.FeatureSource";
#endif
            const STRING BboxId = L"Library://bbox.FeatureSource";
            const STRING PointsId = L"Library://points.FeatureSource";
            const STRING EmptyId = L"Library://Empty.FeatureSource";
            const STRING InvalidId = L"Library://Invalid.FeatureSource";
            const STRING InvalidPropertyId = L"Library://InvalidProperty.FeatureSource";
            const STRING CountryId = L"Library://COUNTRY.FeatureSource";
            const STRING TuxPointGeomId = L"Library://TuxPointGeom.FeatureSource";
            const STRING TuxPointsId = L"Library://TuxPoints.FeatureSource";

            Ptr<MgResourceIdentifier> BboxResId = new MgResourceIdentifier(BboxId);
            Ptr<MgResourceIdentifier> PointsResId = new MgResourceIdentifier(PointsId);
            Ptr<MgResourceIdentifier> EmptyResId = new MgResourceIdentifier(EmptyId);
            Ptr<MgResourceIdentifier> InvalidResId = new MgResourceIdentifier(InvalidId);
            Ptr<MgResourceIdentifier> InvalidPropertyResId = new MgResourceIdentifier(InvalidPropertyId);
            Ptr<MgResourceIdentifier> CountryResId = new MgResourceIdentifier(CountryId);
            Ptr<MgResourceIdentifier> TuxPointGeomResId = new MgResourceIdentifier(TuxPointGeomId);
            Ptr<MgResourceIdentifier> TuxPointsResId = new MgResourceIdentifier(TuxPointsId);

            // Try and create invalid resources
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(EmptyResId, EmptyConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidResId, InvalidConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidPropertyResId, InvalidPropertyConnectionString, L"", L""), MgException*);

            // Create valid resources
            AddFeatureSource(BboxResId, BboxConnectionString, L"", L"");
            AddFeatureSource(PointsResId, PointsConnectionString, L"", L"");
            AddFeatureSource(CountryResId, CountryConnectionString, L"", L"");
            AddFeatureSource(TuxPointGeomResId, TuxPointGeomConnectionString, L"", L"");
            AddFeatureSource(TuxPointsResId, TuxPointsConnectionString, L"", L"");

            bool canConnect = TestFeatureConnection(BboxResId);
            CPPUNIT_ASSERT(canConnect);

            canConnect = TestFeatureConnection(PointsResId);
            CPPUNIT_ASSERT(canConnect);

            // Should not be able to connect, as the resource is empty
            canConnect = TestFeatureConnection(EmptyResId);
            CPPUNIT_ASSERT(!canConnect);

            // Should not be able to connect, as the resource is invalid
            canConnect = TestFeatureConnection(InvalidResId);
            CPPUNIT_ASSERT(!canConnect);

            // Should not be able to connect due to the invalid property
            canConnect = TestFeatureConnection(InvalidPropertyResId);
            CPPUNIT_ASSERT(!canConnect);

            canConnect = TestFeatureConnection(CountryResId);
            CPPUNIT_ASSERT(canConnect);

            canConnect = TestFeatureConnection(TuxPointGeomResId);
            CPPUNIT_ASSERT(canConnect);

            canConnect = TestFeatureConnection(TuxPointsResId);
            CPPUNIT_ASSERT(canConnect);

            // Delete Odbc resources
            resourceService->DeleteResource(BboxResId);
            resourceService->DeleteResource(PointsResId);
            resourceService->DeleteResource(EmptyResId);
            resourceService->DeleteResource(InvalidResId);
            resourceService->DeleteResource(InvalidPropertyResId);
            resourceService->DeleteResource(CountryResId);
            resourceService->DeleteResource(TuxPointGeomResId);
            resourceService->DeleteResource(TuxPointsResId);
        }
        else
        {
            printf("\nSkipping TestCase_OdbcProvider_FeatureConnection(). FDO ODBC is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the odbc provider
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OdbcProvider_DescribeSchema()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OdbcProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OdbcProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

        const STRING provider = L"OSGeo.ODBC.3.0";

        // Check to see if ODBC provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
#ifdef _WIN32
            STRING BboxConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\bboxdb.FeatureSource";
            STRING PointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Points.FeatureSource";
            STRING EmptyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Empty.FeatureSource";
            STRING InvalidConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPoints.FeatureSource";
#else
            STRING BboxConnectionString = L"../UnitTestFiles/FeatureService/ODBC/bboxdb.FeatureSource";
            STRING PointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Points.FeatureSource";
            STRING EmptyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Empty.FeatureSource";
            STRING InvalidConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"../UnitTestFiles/FeatureService/ODBC/COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPoints.FeatureSource";
#endif
            const STRING BboxId = L"Library://bbox.FeatureSource";
            const STRING PointsId = L"Library://points.FeatureSource";
            const STRING EmptyId = L"Library://Empty.FeatureSource";
            const STRING InvalidId = L"Library://Invalid.FeatureSource";
            const STRING InvalidPropertyId = L"Library://InvalidProperty.FeatureSource";
            const STRING CountryId = L"Library://COUNTRY.FeatureSource";
            const STRING TuxPointGeomId = L"Library://TuxPointGeom.FeatureSource";
            const STRING TuxPointsId = L"Library://TuxPoints.FeatureSource";

            Ptr<MgResourceIdentifier> BboxResId = new MgResourceIdentifier(BboxId);
            Ptr<MgResourceIdentifier> PointsResId = new MgResourceIdentifier(PointsId);
            Ptr<MgResourceIdentifier> EmptyResId = new MgResourceIdentifier(EmptyId);
            Ptr<MgResourceIdentifier> InvalidResId = new MgResourceIdentifier(InvalidId);
            Ptr<MgResourceIdentifier> InvalidPropertyResId = new MgResourceIdentifier(InvalidPropertyId);
            Ptr<MgResourceIdentifier> CountryResId = new MgResourceIdentifier(CountryId);
            Ptr<MgResourceIdentifier> TuxPointGeomResId = new MgResourceIdentifier(TuxPointGeomId);
            Ptr<MgResourceIdentifier> TuxPointsResId = new MgResourceIdentifier(TuxPointsId);

            // Try and create invalid resources
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(EmptyResId, EmptyConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidResId, InvalidConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidPropertyResId, InvalidPropertyConnectionString, L"", L""), MgException*);

            // Create valid resources
            AddFeatureSource(BboxResId, BboxConnectionString, L"", L"");
            AddFeatureSource(PointsResId, PointsConnectionString, L"", L"");
            AddFeatureSource(CountryResId, CountryConnectionString, L"", L"");
            AddFeatureSource(TuxPointGeomResId, TuxPointGeomConnectionString, L"", L"");
            AddFeatureSource(TuxPointsResId, TuxPointsConnectionString, L"", L"");

            TestDescribeSchema(BboxId);
            TestDescribeSchema(PointsId);
            CPPUNIT_ASSERT_THROW_MG(TestDescribeSchema(EmptyId), MgException*);
            CPPUNIT_ASSERT_THROW_MG(TestDescribeSchema(InvalidId), MgException*);
            CPPUNIT_ASSERT_THROW_MG(TestDescribeSchema(InvalidPropertyId), MgException*);
            TestDescribeSchema(CountryId);
            TestDescribeSchema(TuxPointGeomId);
            TestDescribeSchema(TuxPointsId);

            // Delete Odbc resources
            resourceService->DeleteResource(BboxResId);
            resourceService->DeleteResource(PointsResId);
            resourceService->DeleteResource(EmptyResId);
            resourceService->DeleteResource(InvalidResId);
            resourceService->DeleteResource(InvalidPropertyResId);
            resourceService->DeleteResource(CountryResId);
            resourceService->DeleteResource(TuxPointGeomResId);
            resourceService->DeleteResource(TuxPointsResId);
        }
        else
        {
            printf("\nSkipping TestCase_OdbcProvider_DescribeSchema(). FDO ODBC is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the odbc provider
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OdbcProvider_SelectFeatures()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OdbcProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OdbcProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

        const STRING provider = L"OSGeo.ODBC.3.0";

        // Check to see if ODBC provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
#ifdef _WIN32
            STRING BboxConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\bboxdb.FeatureSource";
            STRING PointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Points.FeatureSource";
            STRING EmptyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Empty.FeatureSource";
            STRING InvalidConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPoints.FeatureSource";
#else
            STRING BboxConnectionString = L"../UnitTestFiles/FeatureService/ODBC/bboxdb.FeatureSource";
            STRING PointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Points.FeatureSource";
            STRING EmptyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Empty.FeatureSource";
            STRING InvalidConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"../UnitTestFiles/FeatureService/ODBC/COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPoints.FeatureSource";
#endif
            const STRING BboxId = L"Library://bbox.FeatureSource";
            const STRING PointsId = L"Library://points.FeatureSource";
            const STRING EmptyId = L"Library://Empty.FeatureSource";
            const STRING InvalidId = L"Library://Invalid.FeatureSource";
            const STRING InvalidPropertyId = L"Library://InvalidProperty.FeatureSource";
            const STRING CountryId = L"Library://COUNTRY.FeatureSource";
            const STRING TuxPointGeomId = L"Library://TuxPointGeom.FeatureSource";
            const STRING TuxPointsId = L"Library://TuxPoints.FeatureSource";

            Ptr<MgResourceIdentifier> BboxResId = new MgResourceIdentifier(BboxId);
            Ptr<MgResourceIdentifier> PointsResId = new MgResourceIdentifier(PointsId);
            Ptr<MgResourceIdentifier> EmptyResId = new MgResourceIdentifier(EmptyId);
            Ptr<MgResourceIdentifier> InvalidResId = new MgResourceIdentifier(InvalidId);
            Ptr<MgResourceIdentifier> InvalidPropertyResId = new MgResourceIdentifier(InvalidPropertyId);
            Ptr<MgResourceIdentifier> CountryResId = new MgResourceIdentifier(CountryId);
            Ptr<MgResourceIdentifier> TuxPointGeomResId = new MgResourceIdentifier(TuxPointGeomId);
            Ptr<MgResourceIdentifier> TuxPointsResId = new MgResourceIdentifier(TuxPointsId);

            // Try and create invalid resources
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(EmptyResId, EmptyConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidResId, InvalidConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidPropertyResId, InvalidPropertyConnectionString, L"", L""), MgException*);

            // Create valid resources
            AddFeatureSource(BboxResId, BboxConnectionString, L"", L"");
            AddFeatureSource(PointsResId, PointsConnectionString, L"", L"");
            AddFeatureSource(CountryResId, CountryConnectionString, L"", L"");
            AddFeatureSource(TuxPointGeomResId, TuxPointGeomConnectionString, L"", L"");
            AddFeatureSource(TuxPointsResId, TuxPointsConnectionString, L"", L"");

            TestSelectFeatures(BboxId);
            TestSelectFeatures(PointsId);
            CPPUNIT_ASSERT_THROW_MG(TestSelectFeatures(EmptyId), MgException*);
            CPPUNIT_ASSERT_THROW_MG(TestSelectFeatures(InvalidId), MgException*);
            CPPUNIT_ASSERT_THROW_MG(TestSelectFeatures(InvalidPropertyId), MgException*);
            TestSelectFeatures(CountryId);
            TestSelectFeatures(TuxPointGeomId);
            TestSelectFeatures(TuxPointsId);

            // Delete Odbc resources
            resourceService->DeleteResource(BboxResId);
            resourceService->DeleteResource(PointsResId);
            resourceService->DeleteResource(EmptyResId);
            resourceService->DeleteResource(InvalidResId);
            resourceService->DeleteResource(InvalidPropertyResId);
            resourceService->DeleteResource(CountryResId);
            resourceService->DeleteResource(TuxPointGeomResId);
            resourceService->DeleteResource(TuxPointsResId);
        }
        else
        {
            printf("\nSkipping TestCase_OdbcProvider_SelectFeatures(). FDO ODBC is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the odbc provider
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OdbcProvider_GetSpatialContext()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OdbcProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OdbcProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

        const STRING provider = L"OSGeo.ODBC.3.0";

        // Check to see if ODBC provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
#ifdef _WIN32
            STRING BboxConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\bboxdb.FeatureSource";
            STRING PointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Points.FeatureSource";
            STRING EmptyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Empty.FeatureSource";
            STRING InvalidConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPoints.FeatureSource";
#else
            STRING BboxConnectionString = L"../UnitTestFiles/FeatureService/ODBC/bboxdb.FeatureSource";
            STRING PointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Points.FeatureSource";
            STRING EmptyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Empty.FeatureSource";
            STRING InvalidConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"../UnitTestFiles/FeatureService/ODBC/COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPoints.FeatureSource";
#endif
            const STRING BboxId = L"Library://bbox.FeatureSource";
            const STRING PointsId = L"Library://points.FeatureSource";
            const STRING EmptyId = L"Library://Empty.FeatureSource";
            const STRING InvalidId = L"Library://Invalid.FeatureSource";
            const STRING InvalidPropertyId = L"Library://InvalidProperty.FeatureSource";
            const STRING CountryId = L"Library://COUNTRY.FeatureSource";
            const STRING TuxPointGeomId = L"Library://TuxPointGeom.FeatureSource";
            const STRING TuxPointsId = L"Library://TuxPoints.FeatureSource";

            Ptr<MgResourceIdentifier> BboxResId = new MgResourceIdentifier(BboxId);
            Ptr<MgResourceIdentifier> PointsResId = new MgResourceIdentifier(PointsId);
            Ptr<MgResourceIdentifier> EmptyResId = new MgResourceIdentifier(EmptyId);
            Ptr<MgResourceIdentifier> InvalidResId = new MgResourceIdentifier(InvalidId);
            Ptr<MgResourceIdentifier> InvalidPropertyResId = new MgResourceIdentifier(InvalidPropertyId);
            Ptr<MgResourceIdentifier> CountryResId = new MgResourceIdentifier(CountryId);
            Ptr<MgResourceIdentifier> TuxPointGeomResId = new MgResourceIdentifier(TuxPointGeomId);
            Ptr<MgResourceIdentifier> TuxPointsResId = new MgResourceIdentifier(TuxPointsId);

            // Try and create invalid resources
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(EmptyResId, EmptyConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidResId, InvalidConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidPropertyResId, InvalidPropertyConnectionString, L"", L""), MgException*);

            // Create valid resources
            AddFeatureSource(BboxResId, BboxConnectionString, L"", L"");
            AddFeatureSource(PointsResId, PointsConnectionString, L"", L"");
            AddFeatureSource(CountryResId, CountryConnectionString, L"", L"");
            AddFeatureSource(TuxPointGeomResId, TuxPointGeomConnectionString, L"", L"");
            AddFeatureSource(TuxPointsResId, TuxPointsConnectionString, L"", L"");

            TestGetSpatialContext(BboxId);
            TestGetSpatialContext(PointsId);
            CPPUNIT_ASSERT_THROW_MG(TestGetSpatialContext(EmptyId), MgException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetSpatialContext(InvalidId), MgException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetSpatialContext(InvalidPropertyId), MgException*);
            TestGetSpatialContext(CountryId);
            TestGetSpatialContext(TuxPointGeomId);
            TestGetSpatialContext(TuxPointsId);

            // Delete Odbc resources
            resourceService->DeleteResource(BboxResId);
            resourceService->DeleteResource(PointsResId);
            resourceService->DeleteResource(EmptyResId);
            resourceService->DeleteResource(InvalidResId);
            resourceService->DeleteResource(InvalidPropertyResId);
            resourceService->DeleteResource(CountryResId);
            resourceService->DeleteResource(TuxPointGeomResId);
            resourceService->DeleteResource(TuxPointsResId);
        }
        else
        {
            printf("\nSkipping TestCase_OdbcProvider_GetSpatialContext(). FDO ODBC is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the odbc provider
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OdbcProvider_GetLongTransactions()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OdbcProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OdbcProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

        const STRING provider = L"OSGeo.ODBC.3.0";

        // Check to see if ODBC provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
#ifdef _WIN32
            STRING BboxConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\bboxdb.FeatureSource";
            STRING PointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Points.FeatureSource";
            STRING EmptyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Empty.FeatureSource";
            STRING InvalidConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPoints.FeatureSource";
#else
            STRING BboxConnectionString = L"../UnitTestFiles/FeatureService/ODBC/bboxdb.FeatureSource";
            STRING PointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Points.FeatureSource";
            STRING EmptyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Empty.FeatureSource";
            STRING InvalidConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"../UnitTestFiles/FeatureService/ODBC/COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPoints.FeatureSource";
#endif
            const STRING BboxId = L"Library://bbox.FeatureSource";
            const STRING PointsId = L"Library://points.FeatureSource";
            const STRING EmptyId = L"Library://Empty.FeatureSource";
            const STRING InvalidId = L"Library://Invalid.FeatureSource";
            const STRING InvalidPropertyId = L"Library://InvalidProperty.FeatureSource";
            const STRING CountryId = L"Library://COUNTRY.FeatureSource";
            const STRING TuxPointGeomId = L"Library://TuxPointGeom.FeatureSource";
            const STRING TuxPointsId = L"Library://TuxPoints.FeatureSource";

            Ptr<MgResourceIdentifier> BboxResId = new MgResourceIdentifier(BboxId);
            Ptr<MgResourceIdentifier> PointsResId = new MgResourceIdentifier(PointsId);
            Ptr<MgResourceIdentifier> EmptyResId = new MgResourceIdentifier(EmptyId);
            Ptr<MgResourceIdentifier> InvalidResId = new MgResourceIdentifier(InvalidId);
            Ptr<MgResourceIdentifier> InvalidPropertyResId = new MgResourceIdentifier(InvalidPropertyId);
            Ptr<MgResourceIdentifier> CountryResId = new MgResourceIdentifier(CountryId);
            Ptr<MgResourceIdentifier> TuxPointGeomResId = new MgResourceIdentifier(TuxPointGeomId);
            Ptr<MgResourceIdentifier> TuxPointsResId = new MgResourceIdentifier(TuxPointsId);

            // Try and create invalid resources
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(EmptyResId, EmptyConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidResId, InvalidConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidPropertyResId, InvalidPropertyConnectionString, L"", L""), MgException*);

            // Create valid resources
            AddFeatureSource(BboxResId, BboxConnectionString, L"", L"");
            AddFeatureSource(PointsResId, PointsConnectionString, L"", L"");
            AddFeatureSource(CountryResId, CountryConnectionString, L"", L"");
            AddFeatureSource(TuxPointGeomResId, TuxPointGeomConnectionString, L"", L"");
            AddFeatureSource(TuxPointsResId, TuxPointsConnectionString, L"", L"");

            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(BboxId), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(PointsId), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(EmptyId), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(InvalidId), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(InvalidPropertyId), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(CountryId), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(TuxPointGeomId), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(TuxPointsId), MgInvalidOperationException*);

            // Delete Odbc resources
            resourceService->DeleteResource(BboxResId);
            resourceService->DeleteResource(PointsResId);
            resourceService->DeleteResource(EmptyResId);
            resourceService->DeleteResource(InvalidResId);
            resourceService->DeleteResource(InvalidPropertyResId);
            resourceService->DeleteResource(CountryResId);
            resourceService->DeleteResource(TuxPointGeomResId);
            resourceService->DeleteResource(TuxPointsResId);
        }
        else
        {
            printf("\nSkipping TestCase_OdbcProvider_GetLongTransactions(). FDO ODBC is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the odbc provider
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OdbcProvider_TestSqlCommands()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OdbcProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OdbcProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

        const STRING provider = L"OSGeo.ODBC.3.0";

        // Check to see if ODBC provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
#ifdef _WIN32
            STRING BboxConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\bboxdb.FeatureSource";
            STRING PointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Points.FeatureSource";
            STRING EmptyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Empty.FeatureSource";
            STRING InvalidConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"..\\UnitTestFiles\\FeatureService\\ODBC\\TuxPoints.FeatureSource";
#else
            STRING BboxConnectionString = L"../UnitTestFiles/FeatureService/ODBC/bboxdb.FeatureSource";
            STRING PointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Points.FeatureSource";
            STRING EmptyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Empty.FeatureSource";
            STRING InvalidConnectionString = L"../UnitTestFiles/FeatureService/ODBC/Invalid.FeatureSource";
            STRING InvalidPropertyConnectionString = L"../UnitTestFiles/FeatureService/ODBC/InvalidProperty.FeatureSource";
            STRING CountryConnectionString = L"../UnitTestFiles/FeatureService/ODBC/COUNTRY.FeatureSource";
            STRING TuxPointGeomConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPointGeom.FeatureSource";
            STRING TuxPointsConnectionString = L"../UnitTestFiles/FeatureService/ODBC/TuxPoints.FeatureSource";
#endif
            const STRING BboxId = L"Library://bbox.FeatureSource";
            const STRING PointsId = L"Library://points.FeatureSource";
            const STRING EmptyId = L"Library://Empty.FeatureSource";
            const STRING InvalidId = L"Library://Invalid.FeatureSource";
            const STRING InvalidPropertyId = L"Library://InvalidProperty.FeatureSource";
            const STRING CountryId = L"Library://COUNTRY.FeatureSource";
            const STRING TuxPointGeomId = L"Library://TuxPointGeom.FeatureSource";
            const STRING TuxPointsId = L"Library://TuxPoints.FeatureSource";

            Ptr<MgResourceIdentifier> BboxResId = new MgResourceIdentifier(BboxId);
            Ptr<MgResourceIdentifier> PointsResId = new MgResourceIdentifier(PointsId);
            Ptr<MgResourceIdentifier> EmptyResId = new MgResourceIdentifier(EmptyId);
            Ptr<MgResourceIdentifier> InvalidResId = new MgResourceIdentifier(InvalidId);
            Ptr<MgResourceIdentifier> InvalidPropertyResId = new MgResourceIdentifier(InvalidPropertyId);
            Ptr<MgResourceIdentifier> CountryResId = new MgResourceIdentifier(CountryId);
            Ptr<MgResourceIdentifier> TuxPointGeomResId = new MgResourceIdentifier(TuxPointGeomId);
            Ptr<MgResourceIdentifier> TuxPointsResId = new MgResourceIdentifier(TuxPointsId);

            // Try and create invalid resources
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(EmptyResId, EmptyConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidResId, InvalidConnectionString, L"", L""), MgException*);
            CPPUNIT_ASSERT_THROW_MG(AddFeatureSource(InvalidPropertyResId, InvalidPropertyConnectionString, L"", L""), MgException*);

            // Create valid resources
            AddFeatureSource(BboxResId, BboxConnectionString, L"", L"");
            AddFeatureSource(PointsResId, PointsConnectionString, L"", L"");
            AddFeatureSource(CountryResId, CountryConnectionString, L"", L"");
            AddFeatureSource(TuxPointGeomResId, TuxPointGeomConnectionString, L"", L"");
            AddFeatureSource(TuxPointsResId, TuxPointsConnectionString, L"", L"");

            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(BboxId), MgFdoException*);
            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(PointsId), MgFdoException*);
            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(EmptyId), MgException*);
            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(InvalidId), MgException*);
            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(InvalidPropertyId), MgException*);
            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(CountryId), MgFdoException*);
            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(TuxPointGeomId), MgFdoException*);
            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(TuxPointsId), MgFdoException*);

            // Delete Odbc resources
            resourceService->DeleteResource(BboxResId);
            resourceService->DeleteResource(PointsResId);
            resourceService->DeleteResource(EmptyResId);
            resourceService->DeleteResource(InvalidResId);
            resourceService->DeleteResource(InvalidPropertyResId);
            resourceService->DeleteResource(CountryResId);
            resourceService->DeleteResource(TuxPointGeomResId);
            resourceService->DeleteResource(TuxPointsResId);
        }
        else
        {
            printf("\nSkipping TestCase_OdbcProvider_TestSqlCommands(). FDO ODBC is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the Oracle provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OracleProvider_GetCapabilities()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OracleProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OracleProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Oracle.3.0";

        // Check to see if Oracle provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            TestGetCapabilities(provider);
        }
        else
        {
            printf("\nSkipping TestCase_OracleProvider_GetCapabilities(). FDO Oracle is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the Oracle provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OracleProvider_FeatureConnection()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OracleProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OracleProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Oracle.3.0";

        // Check to see if Oracle provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING AdskTux2ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Oracle\\OracleAdskTux2.FeatureSource";
#else
            STRING AdskTux2ConnectionString = L"../UnitTestFiles/FeatureService/Oracle/OracleAdskTux2.FeatureSource";
#endif
            const STRING AdskTux2Id = L"Library://OracleAdskTux2.FeatureSource";

            Ptr<MgResourceIdentifier> AdskTux2ResId = new MgResourceIdentifier(AdskTux2Id);

            AddFeatureSource(AdskTux2ResId, AdskTux2ConnectionString, L"", L"");

            bool canConnect = TestFeatureConnection(AdskTux2ResId);
            CPPUNIT_ASSERT(canConnect);

            // Delete Oracle resources
            resourceService->DeleteResource(AdskTux2ResId);
        }
        else
        {
            printf("\nSkipping TestCase_OracleProvider_FeatureConnection(). FDO Oracle is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the Oracle provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OracleProvider_DescribeSchema()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OracleProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OracleProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Oracle.3.0";

        // Check to see if Oracle provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING AdskTux2ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Oracle\\OracleAdskTux2.FeatureSource";
#else
            STRING AdskTux2ConnectionString = L"../UnitTestFiles/FeatureService/Oracle/OracleAdskTux2.FeatureSource";
#endif
            const STRING AdskTux2Id = L"Library://OracleAdskTux2.FeatureSource";

            Ptr<MgResourceIdentifier> AdskTux2ResId = new MgResourceIdentifier(AdskTux2Id);

            AddFeatureSource(AdskTux2ResId, AdskTux2ConnectionString, L"", L"");

            TestDescribeSchema(AdskTux2Id);

            // Delete Oracle resources
            resourceService->DeleteResource(AdskTux2ResId);
        }
        else
        {
            printf("\nSkipping TestCase_OracleProvider_DescribeSchema(). FDO Oracle is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the Oracle provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OracleProvider_SelectFeatures()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OracleProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OracleProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Oracle.3.0";

        // Check to see if Oracle provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING AdskTux2ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Oracle\\OracleAdskTux2.FeatureSource";
#else
            STRING AdskTux2ConnectionString = L"../UnitTestFiles/FeatureService/Oracle/OracleAdskTux2.FeatureSource";
#endif
            const STRING AdskTux2Id = L"Library://OracleAdskTux2.FeatureSource";

            Ptr<MgResourceIdentifier> AdskTux2ResId = new MgResourceIdentifier(AdskTux2Id);

            AddFeatureSource(AdskTux2ResId, AdskTux2ConnectionString, L"", L"");

            TestSelectFeatures(AdskTux2Id);

            // Delete Oracle resources
            resourceService->DeleteResource(AdskTux2ResId);
        }
        else
        {
            printf("\nSkipping TestCase_OracleProvider_SelectFeatures(). FDO Oracle is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the Oracle provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OracleProvider_GetSpatialContext()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OracleProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OracleProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Oracle.3.0";

        // Check to see if Oracle provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING AdskTux2ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Oracle\\OracleAdskTux2.FeatureSource";
#else
            STRING AdskTux2ConnectionString = L"../UnitTestFiles/FeatureService/Oracle/OracleAdskTux2.FeatureSource";
#endif
            const STRING AdskTux2Id = L"Library://OracleAdskTux2.FeatureSource";

            Ptr<MgResourceIdentifier> AdskTux2ResId = new MgResourceIdentifier(AdskTux2Id);

            AddFeatureSource(AdskTux2ResId, AdskTux2ConnectionString, L"", L"");

            TestGetSpatialContext(AdskTux2Id);

            // Delete Oracle resources
            resourceService->DeleteResource(AdskTux2ResId);
        }
        else
        {
            printf("\nSkipping TestCase_OracleProvider_GetSpatialContext(). FDO Oracle is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the Oracle provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OracleProvider_GetLongTransactions()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OracleProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OracleProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Oracle.3.0";

        // Check to see if Oracle provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING AdskTux2ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Oracle\\OracleAdskTux2.FeatureSource";
#else
            STRING AdskTux2ConnectionString = L"../UnitTestFiles/FeatureService/Oracle/OracleAdskTux2.FeatureSource";
#endif
            const STRING AdskTux2Id = L"Library://OracleAdskTux2.FeatureSource";

            Ptr<MgResourceIdentifier> AdskTux2ResId = new MgResourceIdentifier(AdskTux2Id);

            AddFeatureSource(AdskTux2ResId, AdskTux2ConnectionString, L"", L"");

            TestGetLongTransactions(AdskTux2Id);

            // Delete Oracle resources
            resourceService->DeleteResource(AdskTux2ResId);
        }
        else
        {
            printf("\nSkipping TestCase_OracleProvider_GetLongTransactions(). FDO Oracle is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the Oracle provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_OracleProvider_TestSqlCommands()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_OracleProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_OracleProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Oracle.3.0";

        // Check to see if Oracle provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING AdskTux2ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Oracle\\OracleAdskTux2.FeatureSource";
#else
            STRING AdskTux2ConnectionString = L"../UnitTestFiles/FeatureService/Oracle/OracleAdskTux2.FeatureSource";
#endif
            const STRING AdskTux2Id = L"Library://OracleAdskTux2.FeatureSource";

            Ptr<MgResourceIdentifier> AdskTux2ResId = new MgResourceIdentifier(AdskTux2Id);

            AddFeatureSource(AdskTux2ResId, AdskTux2ConnectionString, L"", L"");

            TestSqlCommands(AdskTux2Id);

            // Delete Oracle resources
            resourceService->DeleteResource(AdskTux2ResId);
        }
        else
        {
            printf("\nSkipping TestCase_OracleProvider_TestSqlCommands(). FDO Oracle is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the ArcSde provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ArcSdeProvider_GetCapabilities()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ArcSdeProvider_GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ArcSdeProvider_GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.ArcSDE.3.0";

        // Check to see if ArcSDE provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            TestGetCapabilities(provider);
        }
        else
        {
            printf("\nSkipping TestCase_ArcSdeProvider_GetCapabilities(). FDO ArcSDE is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the ArcSde provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ArcSdeProvider_FeatureConnection()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ArcSdeProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ArcSdeProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.ArcSDE.3.0";

        // Check to see if ArcSDE provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\ArcSde\\ArcSde.FeatureSource";
#else
            STRING ConnectionString = L"../UnitTestFiles/FeatureService/ArcSde/ArcSde.FeatureSource";
#endif
            const STRING Id = L"Library://ArcSde.FeatureSource";

            Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

            AddFeatureSource(ResId, ConnectionString, L"", L"");

            bool canConnect = TestFeatureConnection(ResId);
            CPPUNIT_ASSERT(canConnect);

            // Delete ArcSDE resources
            resourceService->DeleteResource(ResId);
        }
        else
        {
            printf("\nSkipping TestCase_ArcSdeProvider_FeatureConnection(). FDO ArcSDE is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the ArcSde provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ArcSdeProvider_DescribeSchema()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ArcSdeProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ArcSdeProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.ArcSDE.3.0";

        // Check to see if ArcSDE provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\ArcSde\\ArcSde.FeatureSource";
#else
            STRING ConnectionString = L"../UnitTestFiles/FeatureService/ArcSde/ArcSde.FeatureSource";
#endif
            const STRING Id = L"Library://ArcSde.FeatureSource";

            Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

            AddFeatureSource(ResId, ConnectionString, L"", L"");

            TestDescribeSchema(Id);

            // Delete ArcSDE resources
            resourceService->DeleteResource(ResId);
        }
        else
        {
            printf("\nSkipping TestCase_ArcSdeProvider_DescribeSchema(). FDO ArcSDE is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the ArcSde provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ArcSdeProvider_SelectFeatures()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ArcSdeProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ArcSdeProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.ArcSDE.3.0";

        // Check to see if ArcSDE provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\ArcSde\\ArcSde.FeatureSource";
#else
            STRING ConnectionString = L"../UnitTestFiles/FeatureService/ArcSde/ArcSde.FeatureSource";
#endif
            const STRING Id = L"Library://ArcSde.FeatureSource";

            Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

            AddFeatureSource(ResId, ConnectionString, L"", L"");

            TestSelectFeatures(Id);

            // Delete ArcSDE resources
            resourceService->DeleteResource(ResId);
        }
        else
        {
            printf("\nSkipping TestCase_ArcSdeProvider_SelectFeatures(). FDO ArcSDE is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the ArcSde provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ArcSdeProvider_GetSpatialContext()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ArcSdeProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ArcSdeProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.ArcSDE.3.0";

        // Check to see if ArcSDE provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\ArcSde\\ArcSde.FeatureSource";
#else
            STRING ConnectionString = L"../UnitTestFiles/FeatureService/ArcSde/ArcSde.FeatureSource";
#endif
            const STRING Id = L"Library://ArcSde.FeatureSource";

            Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

            AddFeatureSource(ResId, ConnectionString, L"", L"");

            TestGetSpatialContext(Id);

            // Delete ArcSDE resources
            resourceService->DeleteResource(ResId);
        }
        else
        {
            printf("\nSkipping TestCase_ArcSdeProvider_GetSpatialContext(). FDO ArcSDE is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the ArcSde provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ArcSdeProvider_GetLongTransactions()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ArcSdeProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ArcSdeProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.ArcSDE.3.0";

        // Check to see if ArcSDE provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\ArcSde\\ArcSde.FeatureSource";
#else
            STRING ConnectionString = L"../UnitTestFiles/FeatureService/ArcSde/ArcSde.FeatureSource";
#endif
            const STRING Id = L"Library://ArcSde.FeatureSource";

            Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

            AddFeatureSource(ResId, ConnectionString, L"", L"");

            TestGetLongTransactions(Id);

            // Delete ArcSDE resources
            resourceService->DeleteResource(ResId);
        }
        else
        {
            printf("\nSkipping TestCase_ArcSdeProvider_GetLongTransactions(). FDO ArcSDE is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the ArcSde provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ArcSdeProvider_TestSqlCommands()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ArcSdeProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ArcSdeProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.ArcSDE.3.0";

        // Check to see if ArcSDE provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\ArcSde\\ArcSde.FeatureSource";
#else
            STRING ConnectionString = L"../UnitTestFiles/FeatureService/ArcSde/ArcSde.FeatureSource";
#endif
            const STRING Id = L"Library://ArcSde.FeatureSource";

            Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

            AddFeatureSource(ResId, ConnectionString, L"", L"");

            TestSqlCommands(Id);

            // Delete ArcSDE resources
            resourceService->DeleteResource(ResId);
        }
        else
        {
            printf("\nSkipping TestCase_ArcSdeProvider_TestSqlCommands(). FDO ArcSDE is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SDF provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SdfProvider_GetCapabilities()
{
    try
    {
        const STRING provider = L"OSGeo.SDF.3.0";
        TestGetCapabilities(provider);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SDF provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SdfProvider_FeatureConnection()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_SdfProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SdfProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SDF\\World_Countries.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SDF/World_Countries.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SDF.3.0";
        const STRING Id = L"Library://World_Countries.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        bool canConnect = TestFeatureConnection(ResId);
        CPPUNIT_ASSERT(canConnect);

        // Delete Sdf resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SDF provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SdfProvider_DescribeSchema()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_SdfProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SdfProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SDF\\World_Countries.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SDF/World_Countries.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SDF.3.0";
        const STRING Id = L"Library://World_Countries.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        TestDescribeSchema(Id);

        // Delete Sdf resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SDF provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SdfProvider_SelectFeatures()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_SdfProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SdfProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SDF\\World_Countries.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SDF/World_Countries.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SDF.3.0";
        const STRING Id = L"Library://World_Countries.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        TestSelectFeatures(Id);

        // Delete Sdf resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SDF provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SdfProvider_GetSpatialContext()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_SdfProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SdfProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SDF\\World_Countries.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SDF/World_Countries.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SDF.3.0";
        const STRING Id = L"Library://World_Countries.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        TestGetSpatialContext(Id);

        // Delete Sdf resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SDF provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SdfProvider_GetLongTransactions()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_SdfProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SdfProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SDF\\World_Countries.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SDF/World_Countries.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SDF.3.0";
        const STRING Id = L"Library://World_Countries.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(Id), MgInvalidOperationException*);

        // Delete Sdf resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SDF provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SdfProvider_TestSqlCommands()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_SdfProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SdfProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SDF\\World_Countries.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SDF/World_Countries.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SDF.3.0";
        const STRING Id = L"Library://World_Countries.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(Id), MgInvalidOperationException*);

        // Delete Sdf resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the raster provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_RasterProvider_GetCapabilities()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_RasterProvider_GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_RasterProvider_GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Raster.3.0";

        // Check to see if Raster File provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            TestGetCapabilities(provider);
        }
        else
        {
            printf("\nSkipping TestCase_RasterProvider_GetCapabilities(). FDO RFP is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the raster provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_RasterProvider_FeatureConnection()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_RasterProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_RasterProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Raster.3.0";

        // Check to see if Raster File provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING Area1ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\area1.FeatureSource";
            STRING PciConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg.FeatureSource";
            STRING RgbConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB.FeatureSource";
            STRING PciData = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg\\config.xml";
            STRING RgbData = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB\\RGB.xml";
#else
            STRING Area1ConnectionString = L"../UnitTestFiles/FeatureService/Raster/area1.FeatureSource";
            STRING PciConnectionString = L"../UnitTestFiles/FeatureService/Raster/pci_eg.FeatureSource";
            STRING RgbConnectionString = L"../UnitTestFiles/FeatureService/Raster/RGB.FeatureSource";
            STRING PciData = L"../UnitTestFiles/FeatureService/Raster/pci_eg/config.xml";
            STRING RgbData = L"../UnitTestFiles/FeatureService/Raster/RGB/RGB.xml";
#endif

            const STRING Area1Id = L"Library://area1.FeatureSource";
            const STRING PciId = L"Library://pci_eg.FeatureSource";
            const STRING RgbId = L"Library://RGB.FeatureSource";

            const STRING DataName = L"CONFIG.XML";

            Ptr<MgResourceIdentifier> Area1ResId = new MgResourceIdentifier(Area1Id);
            Ptr<MgResourceIdentifier> PciResId = new MgResourceIdentifier(PciId);
            Ptr<MgResourceIdentifier> RgbResId = new MgResourceIdentifier(RgbId);

            AddFeatureSource(Area1ResId, Area1ConnectionString, L"", L"");
            AddFeatureSource(PciResId, PciConnectionString, DataName, PciData);
            AddFeatureSource(RgbResId, RgbConnectionString, DataName, RgbData);

            bool canConnect = TestFeatureConnection(Area1ResId);
            CPPUNIT_ASSERT(canConnect);

            canConnect = TestFeatureConnection(PciResId);
            CPPUNIT_ASSERT(canConnect);

            canConnect = TestFeatureConnection(RgbResId);
            CPPUNIT_ASSERT(canConnect);

            // Delete Raster resources
            resourceService->DeleteResourceData(PciResId, DataName);
            resourceService->DeleteResourceData(RgbResId, DataName);

            resourceService->DeleteResource(Area1ResId);
            resourceService->DeleteResource(PciResId);
            resourceService->DeleteResource(RgbResId);
        }
        else
        {
            printf("\nSkipping TestCase_RasterProvider_FeatureConnection(). FDO RFP is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the raster provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_RasterProvider_DescribeSchema()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_RasterProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_RasterProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Raster.3.0";

        // Check to see if Raster File provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING Area1ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\area1.FeatureSource";
            STRING PciConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg.FeatureSource";
            STRING RgbConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB.FeatureSource";
            STRING PciData = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg\\config.xml";
            STRING RgbData = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB\\RGB.xml";
#else
            STRING Area1ConnectionString = L"../UnitTestFiles/FeatureService/Raster/area1.FeatureSource";
            STRING PciConnectionString = L"../UnitTestFiles/FeatureService/Raster/pci_eg.FeatureSource";
            STRING RgbConnectionString = L"../UnitTestFiles/FeatureService/Raster/RGB.FeatureSource";
            STRING PciData = L"../UnitTestFiles/FeatureService/Raster/pci_eg/config.xml";
            STRING RgbData = L"../UnitTestFiles/FeatureService/Raster/RGB/RGB.xml";
#endif

            const STRING Area1Id = L"Library://area1.FeatureSource";
            const STRING PciId = L"Library://pci_eg.FeatureSource";
            const STRING RgbId = L"Library://RGB.FeatureSource";

            const STRING DataName = L"CONFIG.XML";

            Ptr<MgResourceIdentifier> Area1ResId = new MgResourceIdentifier(Area1Id);
            Ptr<MgResourceIdentifier> PciResId = new MgResourceIdentifier(PciId);
            Ptr<MgResourceIdentifier> RgbResId = new MgResourceIdentifier(RgbId);

            AddFeatureSource(Area1ResId, Area1ConnectionString, L"", L"");
            AddFeatureSource(PciResId, PciConnectionString, DataName, PciData);
            AddFeatureSource(RgbResId, RgbConnectionString, DataName, RgbData);

            TestDescribeSchema(Area1Id);
            TestDescribeSchema(PciId);
            TestDescribeSchema(RgbId);

            // Delete Raster resources
            resourceService->DeleteResourceData(PciResId, DataName);
            resourceService->DeleteResourceData(RgbResId, DataName);

            resourceService->DeleteResource(Area1ResId);
            resourceService->DeleteResource(PciResId);
            resourceService->DeleteResource(RgbResId);
        }
        else
        {
            printf("\nSkipping TestCase_RasterProvider_DescribeSchema(). FDO RFP is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the raster provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_RasterProvider_SelectFeatures()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_RasterProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_RasterProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Raster.3.0";

        // Check to see if Raster File provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING Area1ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\area1.FeatureSource";
            STRING PciConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg.FeatureSource";
            STRING RgbConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB.FeatureSource";
            STRING PciData = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg\\config.xml";
            STRING RgbData = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB\\RGB.xml";
#else
            STRING Area1ConnectionString = L"../UnitTestFiles/FeatureService/Raster/area1.FeatureSource";
            STRING PciConnectionString = L"../UnitTestFiles/FeatureService/Raster/pci_eg.FeatureSource";
            STRING RgbConnectionString = L"../UnitTestFiles/FeatureService/Raster/RGB.FeatureSource";
            STRING PciData = L"../UnitTestFiles/FeatureService/Raster/pci_eg/config.xml";
            STRING RgbData = L"../UnitTestFiles/FeatureService/Raster/RGB/RGB.xml";
#endif

            const STRING Area1Id = L"Library://area1.FeatureSource";
            const STRING PciId = L"Library://pci_eg.FeatureSource";
            const STRING RgbId = L"Library://RGB.FeatureSource";

            const STRING DataName = L"CONFIG.XML";

            Ptr<MgResourceIdentifier> Area1ResId = new MgResourceIdentifier(Area1Id);
            Ptr<MgResourceIdentifier> PciResId = new MgResourceIdentifier(PciId);
            Ptr<MgResourceIdentifier> RgbResId = new MgResourceIdentifier(RgbId);

            AddFeatureSource(Area1ResId, Area1ConnectionString, L"", L"");
            AddFeatureSource(PciResId, PciConnectionString, DataName, PciData);
            AddFeatureSource(RgbResId, RgbConnectionString, DataName, RgbData);

            CPPUNIT_ASSERT_THROW_MG(TestSelectFeatures(Area1Id), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestSelectFeatures(PciId), MgFeatureServiceException*);
            CPPUNIT_ASSERT_THROW_MG(TestSelectFeatures(RgbId), MgFeatureServiceException*);

            // Delete Raster resources
            resourceService->DeleteResourceData(PciResId, DataName);
            resourceService->DeleteResourceData(RgbResId, DataName);

            resourceService->DeleteResource(Area1ResId);
            resourceService->DeleteResource(PciResId);
            resourceService->DeleteResource(RgbResId);
        }
        else
        {
            printf("\nSkipping TestCase_RasterProvider_SelectFeatures(). FDO RFP is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the raster provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_RasterProvider_GetSpatialContext()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_RasterProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_RasterProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Raster.3.0";

        // Check to see if Raster File provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING Area1ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\area1.FeatureSource";
            STRING PciConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg.FeatureSource";
            STRING RgbConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB.FeatureSource";
            STRING PciData = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg\\config.xml";
            STRING RgbData = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB\\RGB.xml";
#else
            STRING Area1ConnectionString = L"../UnitTestFiles/FeatureService/Raster/area1.FeatureSource";
            STRING PciConnectionString = L"../UnitTestFiles/FeatureService/Raster/pci_eg.FeatureSource";
            STRING RgbConnectionString = L"../UnitTestFiles/FeatureService/Raster/RGB.FeatureSource";
            STRING PciData = L"../UnitTestFiles/FeatureService/Raster/pci_eg/config.xml";
            STRING RgbData = L"../UnitTestFiles/FeatureService/Raster/RGB/RGB.xml";
#endif

            const STRING Area1Id = L"Library://area1.FeatureSource";
            const STRING PciId = L"Library://pci_eg.FeatureSource";
            const STRING RgbId = L"Library://RGB.FeatureSource";

            const STRING DataName = L"CONFIG.XML";

            Ptr<MgResourceIdentifier> Area1ResId = new MgResourceIdentifier(Area1Id);
            Ptr<MgResourceIdentifier> PciResId = new MgResourceIdentifier(PciId);
            Ptr<MgResourceIdentifier> RgbResId = new MgResourceIdentifier(RgbId);

            AddFeatureSource(Area1ResId, Area1ConnectionString, L"", L"");
            AddFeatureSource(PciResId, PciConnectionString, DataName, PciData);
            AddFeatureSource(RgbResId, RgbConnectionString, DataName, RgbData);

            TestGetSpatialContext(Area1Id);
            TestGetSpatialContext(PciId);
            TestGetSpatialContext(RgbId);

            // Delete Raster resources
            resourceService->DeleteResourceData(PciResId, DataName);
            resourceService->DeleteResourceData(RgbResId, DataName);

            resourceService->DeleteResource(Area1ResId);
            resourceService->DeleteResource(PciResId);
            resourceService->DeleteResource(RgbResId);
        }
        else
        {
            printf("\nSkipping TestCase_RasterProvider_GetSpatialContext(). FDO RFP is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the raster provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_RasterProvider_GetLongTransactions()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_RasterProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_RasterProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Raster.3.0";

        // Check to see if Raster File provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING Area1ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\area1.FeatureSource";
            STRING PciConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg.FeatureSource";
            STRING RgbConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB.FeatureSource";
            STRING PciData = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg\\config.xml";
            STRING RgbData = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB\\RGB.xml";
#else
            STRING Area1ConnectionString = L"../UnitTestFiles/FeatureService/Raster/area1.FeatureSource";
            STRING PciConnectionString = L"../UnitTestFiles/FeatureService/Raster/pci_eg.FeatureSource";
            STRING RgbConnectionString = L"../UnitTestFiles/FeatureService/Raster/RGB.FeatureSource";
            STRING PciData = L"../UnitTestFiles/FeatureService/Raster/pci_eg/config.xml";
            STRING RgbData = L"../UnitTestFiles/FeatureService/Raster/RGB/RGB.xml";
#endif

            const STRING Area1Id = L"Library://area1.FeatureSource";
            const STRING PciId = L"Library://pci_eg.FeatureSource";
            const STRING RgbId = L"Library://RGB.FeatureSource";

            const STRING DataName = L"CONFIG.XML";

            Ptr<MgResourceIdentifier> Area1ResId = new MgResourceIdentifier(Area1Id);
            Ptr<MgResourceIdentifier> PciResId = new MgResourceIdentifier(PciId);
            Ptr<MgResourceIdentifier> RgbResId = new MgResourceIdentifier(RgbId);

            AddFeatureSource(Area1ResId, Area1ConnectionString, L"", L"");
            AddFeatureSource(PciResId, PciConnectionString, DataName, PciData);
            AddFeatureSource(RgbResId, RgbConnectionString, DataName, RgbData);

            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(Area1Id), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(PciId), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(RgbId), MgInvalidOperationException*);

            // Delete Raster resources
            resourceService->DeleteResourceData(PciResId, DataName);
            resourceService->DeleteResourceData(RgbResId, DataName);

            resourceService->DeleteResource(Area1ResId);
            resourceService->DeleteResource(PciResId);
            resourceService->DeleteResource(RgbResId);
        }
        else
        {
            printf("\nSkipping TestCase_RasterProvider_GetLongTransactions(). FDO RFP is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the raster provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_RasterProvider_TestSqlCommands()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_RasterProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_RasterProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"Autodesk.Raster.3.0";

        // Check to see if Raster File provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
            STRING Area1ConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\area1.FeatureSource";
            STRING PciConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg.FeatureSource";
            STRING RgbConnectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB.FeatureSource";
            STRING PciData = L"..\\UnitTestFiles\\FeatureService\\Raster\\pci_eg\\config.xml";
            STRING RgbData = L"..\\UnitTestFiles\\FeatureService\\Raster\\RGB\\RGB.xml";
#else
            STRING Area1ConnectionString = L"../UnitTestFiles/FeatureService/Raster/area1.FeatureSource";
            STRING PciConnectionString = L"../UnitTestFiles/FeatureService/Raster/pci_eg.FeatureSource";
            STRING RgbConnectionString = L"../UnitTestFiles/FeatureService/Raster/RGB.FeatureSource";
            STRING PciData = L"../UnitTestFiles/FeatureService/Raster/pci_eg/config.xml";
            STRING RgbData = L"../UnitTestFiles/FeatureService/Raster/RGB/RGB.xml";
#endif

            const STRING Area1Id = L"Library://area1.FeatureSource";
            const STRING PciId = L"Library://pci_eg.FeatureSource";
            const STRING RgbId = L"Library://RGB.FeatureSource";

            const STRING DataName = L"CONFIG.XML";

            Ptr<MgResourceIdentifier> Area1ResId = new MgResourceIdentifier(Area1Id);
            Ptr<MgResourceIdentifier> PciResId = new MgResourceIdentifier(PciId);
            Ptr<MgResourceIdentifier> RgbResId = new MgResourceIdentifier(RgbId);

            AddFeatureSource(Area1ResId, Area1ConnectionString, L"", L"");
            AddFeatureSource(PciResId, PciConnectionString, DataName, PciData);
            AddFeatureSource(RgbResId, RgbConnectionString, DataName, RgbData);

            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(Area1Id), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(PciId), MgInvalidOperationException*);
            CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(RgbId), MgInvalidOperationException*);

            // Delete Raster resources
            resourceService->DeleteResourceData(PciResId, DataName);
            resourceService->DeleteResourceData(RgbResId, DataName);

            resourceService->DeleteResource(Area1ResId);
            resourceService->DeleteResource(PciResId);
            resourceService->DeleteResource(RgbResId);
        }
        else
        {
            printf("\nSkipping TestCase_RasterProvider_TestSqlCommands(). FDO RFP is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the connection to the SDF provider.  It fails if the
/// connection cannot be opened.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestConnectionSDFProvider()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_TestConnectionSDFProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_TestConnectionSDFProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.SDF.3.0";
#ifdef _WIN32
        const STRING connectionString = L"File=..\\UnitTestFiles\\FeatureService\\SDF\\World_Countries.sdf;ReadOnly=true";
#else
        const STRING connectionString = L"File=../UnitTestFiles/FeatureService/SDF/World_Countries.sdf;ReadOnly=true";
#endif
        bool bSuccess = pService->TestConnection(provider, connectionString);
        CPPUNIT_ASSERT(bSuccess);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the connection to the ODBC provider.  It fails if the
/// connection cannot be opened.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestConnectionODBCProvider()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_TestConnectionODBCProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_TestConnectionODBCProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING provider = L"OSGeo.ODBC.3.0";

        // Check to see if ODBC provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
#ifdef _WIN32
            STRING connectionString = L"ConnectionString=\"Driver={Microsoft Access Driver (*.mdb)};DBQ=..\\UnitTestFiles\\FeatureService\\ODBC\\bboxdb.mdb\"";
#else
            STRING connectionString = L"../UnitTestFiles/FeatureService/ODBC/bboxdb.FeatureSource";
#endif

            bool bSuccess = pService->TestConnection(provider, connectionString);
            CPPUNIT_ASSERT(bSuccess);
        }
        else
        {
            printf("\nSkipping TestCase_TestConnectionODBCProvider(). FDO ODBC is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the connection to the Oracle provider.  It fails if the
/// connection cannot be opened.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestConnectionOracleProvider()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_TestConnectionOracleProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_TestConnectionOracleProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING provider = L"Autodesk.Oracle.3.0";

        // Check to see if Oracle provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
#ifdef _WIN32
            STRING connectionString = L"UserName=qa;Password=mgqa;Service=ADSKTUX2;DataStore=FDO_67";
#else
            STRING connectionString = L"UserName=qa;Password=mgqa;Service=ADSKTUX2;DataStore=FDO_67";
#endif

            bool bSuccess = pService->TestConnection(provider, connectionString);
            CPPUNIT_ASSERT(bSuccess);
        }
        else
        {
            printf("\nSkipping TestCase_TestConnectionOracleProvider(). FDO Oracle is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the connection to the ArcSde provider.  It fails if the
/// connection cannot be opened.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestConnectionArcSdeProvider()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_TestConnectionArcSdeProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_TestConnectionArcSdeProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING provider = L"OSGeo.ArcSDE.3.0";

        // Check to see if Oracle provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
#ifdef _WIN32
            STRING connectionString = L"Instance=esri_sde;Server=otwe1;Username=metadcov;Password=test;Datastore=Default Datastore";
#else
            STRING connectionString = L"../UnitTestFiles/FeatureService/ArcSde/ArcSde.FeatureSource";
#endif

            bool bSuccess = pService->TestConnection(provider, connectionString);
            CPPUNIT_ASSERT(bSuccess);
        }
        else
        {
            printf("\nSkipping TestCase_TestConnectionArcSdeProvider(). FDO ArcSDE is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case test the connection to the raster provider.  It fails if the
/// connection cannot be opened.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestConnectionRasterProvider()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_TestConnectionRasterProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_TestConnectionRasterProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING provider = L"Autodesk.Raster.3.0";

        // Check to see if Raster File provider is available before testing
        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
#ifdef _WIN32
            STRING connectionString = L"..\\UnitTestFiles\\FeatureService\\Raster\\area1.FeatureSource";
#else
            STRING connectionString = L"../UnitTestFiles/FeatureService/Raster/area1.FeatureSource";
#endif

//
//  Skip the FDO units test for raster provider on Linux
//
//  TODO:  Remove this #ifdef when FDO raster provider 3.0 issues have been resolved on Linux
//
#ifdef _WIN32
            bool bSuccess = pService->TestConnection(provider, connectionString);
            CPPUNIT_ASSERT(bSuccess);
#else
            printf("\n   ***   Skipping TestCase_TestConnectionRasterProvider()   ***\n");

#endif // End Skip Linux FDO raster provider unit test
        }
        else
        {
            printf("\nSkipping TestCase_TestConnectionRasterProvider(). FDO RFP is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the FDO coonection manager.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestFdoConnectionManager()
{
    try
    {
        MgFdoConnectionManager* pManager = MgFdoConnectionManager::GetInstance();
        if(pManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_TestFdoConnectionManager", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.SDF.3.0";
#ifdef _WIN32
        const STRING connectionString = L"File=..\\UnitTestFiles\\FeatureService\\SDF\\World_Countries.sdf;ReadOnly=true";
#else
        const STRING connectionString = L"File=../UnitTestFiles/FeatureService/SDF/World_Countries.sdf;ReadOnly=true";
#endif

        FdoIConnection* pFdoConnection1 = pManager->Open(provider, connectionString);
        CPPUNIT_ASSERT(pFdoConnection1);

        FdoIConnection* pFdoConnection2 = pManager->Open(provider, connectionString);
        CPPUNIT_ASSERT(pFdoConnection2);

        // These connections should be different
        CPPUNIT_ASSERT(pFdoConnection1 != pFdoConnection2);

        pManager->Close(pFdoConnection1);
        pManager->Close(pFdoConnection2);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the connection property values.  It fails if the
/// returned property values are not the same as the baseline.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetConnectionPropertyValue()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetConnectionPropertyValue", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetConnectionPropertyValue", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        string featureProviders = MgUtil::GetTextFromReader(byteReader);

        STRING provider = L"";
        STRING property = L"";
        STRING connectionString = L"";

        // Check to see if Oracle provider is available before testing
        provider = L"Autodesk.Oracle.3.0";
        size_t pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            CPPUNIT_ASSERT_THROW_MG(pService->GetConnectionPropertyValues(provider, property, connectionString),
                MgInvalidArgumentException*);

            Ptr<MgSiteConnection> ptrSite = new MgSiteConnection();
            Ptr<MgUserInformation> ptrUserCred = new MgUserInformation(L"Administrator", L"admin");
            ptrSite->Open(ptrUserCred);
            Ptr<MgFeatureService> service = (MgFeatureService*)ptrSite->CreateService(MgServiceType::FeatureService);

            provider = L"Autodesk.Oracle.3.0";
            property = L"DataStore";
            connectionString = L"Username=qa;Password=mgqa;Service=ADSKTUX2";

            Ptr<MgStringCollection> values = service->GetConnectionPropertyValues(provider, property, connectionString);
            Ptr<MgByteReader> reader = values->ToXml();
            string valueText = MgUtil::GetTextFromReader(reader);

            string filename = "GetConnectionPropertyValue-" + MgUtil::WideCharToMultiByte(provider) + extension;
            string outputFile = outputDirectory + filename;
            WriteToDisk(outputFile.c_str(), valueText.c_str());

            string baseline = baselineDirectory + filename;
            bool getConnectionPropertyValueFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
            CPPUNIT_ASSERT(getConnectionPropertyValueFilesEqual);
        }
        else
        {
            printf("\nSkipping TestCase_GetConnectionPropertyValue(). FDO Oracle is not registered.\n");
        }

        // Check to see if Oracle provider is available before testing
        provider = L"OSGeo.ArcSDE.3.0";
        pos = featureProviders.find(MgUtil::WideCharToMultiByte(provider));
        if(string::npos != pos)
        {
            //property = L"Datastore";
            //connectionString = L"Username=metadcov;Password=test;Instance=esri_sde;Server=otwe1";

            //values = service->GetConnectionPropertyValues(provider, property, connectionString);
            //reader = values->ToXml();
            //valueText = MgUtil::GetTextFromReader(reader);

            //filename = "GetConnectionPropertyValue-" + MgUtil::WideCharToMultiByte(provider) + extension;
            //outputFile = outputDirectory + filename;
            //WriteToDisk(outputFile.c_str(), valueText.c_str());

            //baseline = baselineDirectory + filename;
            //getConnectionPropertyValueFilesEqual = CheckFile(outputFile.c_str(), baseline.c_str());
            //CPPUNIT_ASSERT(getConnectionPropertyValueFilesEqual);
        }
        else
        {
            printf("\nSkipping TestCase_GetConnectionPropertyValue(). FDO ArcSDE is not registered.\n");
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SHP provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ShpProvider_GetCapabilities()
{
    try
    {
        const STRING provider = L"OSGeo.SHP.3.0";
        TestGetCapabilities(provider);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SHP provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ShpProvider_FeatureConnection()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if (serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ShpProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ShpProvider_FeatureConnection", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SHP\\CN_Provinces.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SHP/CN_Provinces.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SHP.3.0";
        const STRING Id = L"Library://CN_Provinces.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        bool canConnect = TestFeatureConnection(ResId);
        CPPUNIT_ASSERT(canConnect);

        // Delete Shp resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SHP provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ShpProvider_DescribeSchema()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if (serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ShpProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ShpProvider_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SHP\\CN_Provinces.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SHP/CN_Provinces.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SHP.3.0";
        const STRING Id = L"Library://CN_Provinces.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        TestDescribeSchema(Id);

        // Delete Shp resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SHP provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ShpProvider_SelectFeatures()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if (serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ShpProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ShpProvider_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SHP\\CN_Provinces.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SHP/CN_Provinces.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SHP.3.0";
        const STRING Id = L"Library://CN_Provinces.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        TestSelectFeatures(Id);

        // Delete Shp resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SHP provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ShpProvider_GetSpatialContext()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if (serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ShpProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ShpProvider_GetSpatialContext", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SHP\\CN_Provinces.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SHP/CN_Provinces.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SHP.3.0";
        const STRING Id = L"Library://CN_Provinces.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        TestGetSpatialContext(Id);

        // Delete Shp resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SHP provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ShpProvider_GetLongTransactions()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if (serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ShpProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ShpProvider_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SHP\\CN_Provinces.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SHP/CN_Provinces.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SHP.3.0";
        const STRING Id = L"Library://CN_Provinces.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        CPPUNIT_ASSERT_THROW_MG(TestGetLongTransactions(Id), MgInvalidOperationException*);

        // Delete Shp resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the SHP provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ShpProvider_TestSqlCommands()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if (serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ShpProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ShpProvider_TestSqlCommands", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));

#ifdef _WIN32
        STRING ConnectionString = L"..\\UnitTestFiles\\FeatureService\\SHP\\CN_Provinces.FeatureSource";
#else
        STRING ConnectionString = L"../UnitTestFiles/FeatureService/SHP/CN_Provinces.FeatureSource";
#endif
        const STRING provider = L"OSGeo.SHP.3.0";
        const STRING Id = L"Library://CN_Provinces.FeatureSource";

        Ptr<MgResourceIdentifier> ResId = new MgResourceIdentifier(Id);

        AddFeatureSource(ResId, ConnectionString, L"", L"");

        CPPUNIT_ASSERT_THROW_MG(TestSqlCommands(Id), MgInvalidOperationException*);

        // Delete Shp resources
        resourceService->DeleteResource(ResId);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestFeatureService::TestCase_TestConnectionShpProvider()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_TestConnectionShpProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerFeatureService> pService = dynamic_cast<MgServerFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_TestConnectionShpProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.SHP.3.0";
#ifdef _WIN32
        STRING connectionString = L"..\\UnitTestFiles\\FeatureService\\SHP\\CN_Provinces.FeatureSource";
#else
        STRING connectionString = L"../UnitTestFiles/FeatureService/SHP/CN_Provinces.FeatureSource";
#endif
        bool bSuccess = pService->TestConnection(provider, connectionString);
        CPPUNIT_ASSERT(bSuccess);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        STRING message = L"GisException: ";
        message += e->GetExceptionMessage();
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}
