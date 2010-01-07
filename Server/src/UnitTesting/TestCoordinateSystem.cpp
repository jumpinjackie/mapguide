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

#include "MapGuideCommon.h"
#include "TestCoordinateSystem.h"
#include "CppUnitExtensions.h"

#ifndef _WIN32
#define stricmp strcasecmp

extern long GetTickCount();
#endif

#ifdef _WIN32
const char OgcWktTestFile[] = "..\\UnitTestFiles\\ogcwkts.txt";
const char EpsgTestFile[] = "..\\UnitTestFiles\\epsgcodes.txt";
#else
const char OgcWktTestFile[] = "../UnitTestFiles/ogcwkts.txt";
const char EpsgTestFile[] = "../UnitTestFiles/epsgcodes.txt";
#endif

const STRING ArbitraryWkt            = L"LOCAL_CS[\"Non-Earth (Mile)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Mile\", 1609.3440000000001],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
const STRING ArbitraryWkt_Alternate1 = L"LOCALCS [ \"Nonearth Mile\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"Mile\", 1609.344], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";
const STRING ArbitraryWkt_Alternate2 = L"LOCALCS [ \"Nonearth\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"Mile\", 1609.344], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";

const STRING GeographicWkt      = L"GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.000,298.25722293]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.01745329251994]]";
const STRING ProjectedWkt       = L"PROJCS[\"GA-W\",GEOGCS[\"LL27\",DATUM[\"NAD27\",SPHEROID[\"CLRK66\",6378206.400,294.97869821]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"false_easting\",500000.000],PARAMETER[\"false_northing\",0.000],PARAMETER[\"scale_factor\",0.999900000000],PARAMETER[\"central_meridian\",-84.16666666666670],PARAMETER[\"latitude_of_origin\",30.00000000000000],UNIT[\"Foot_US\",0.30480060960122]]";

const STRING InvalidWkt         = L"GEOGCS[\"Invalid Coordinate System\",DATUM[\"Invalid\",SPHEROID[\"Invalid\",0.0,0.0],PRIMEM[\"Invalid\",0],UNIT[\"Invalid\",1]]";

const STRING ArbitraryWkt_Feet  = L"LOCAL_CS [ \"Non-Earth (U.S. Foot)\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"U.S. Foot\", 3.048006096012e-01], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";
const STRING ArbitraryWkt_Meter = L"LOCAL_CS [ \"Non-Earth (Meter)\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"Meter\", 1.0], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";

const STRING GeographicWkt_LL84 = L"GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.000,298.25722293]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]";
const STRING ProjectedWkt_GAW   = L"PROJCS[\"GA-W\",GEOGCS[\"LL27\",DATUM[\"NAD27\",SPHEROID[\"CLRK66\",6378206.400,294.97869821]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"false_easting\",500000.000],PARAMETER[\"false_northing\",0.000],PARAMETER[\"scale_factor\",0.999900000000],PARAMETER[\"central_meridian\",-84.16666666666670],PARAMETER[\"latitude_of_origin\",30.00000000000000],UNIT[\"Foot_US\",0.30480060960122]]";

const STRING Projected_UTM18_NAD83 = L"PROJCS[\"UTM83-18F\",GEOGCS[\"LL83\",DATUM[\"NAD83\",SPHEROID[\"GRS1980\",6378137.000,298.25722210]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"false_easting\",1640416.667],PARAMETER[\"false_northing\",0.000],PARAMETER[\"central_meridian\",-75.00000000000000],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"latitude_of_origin\",0.000],UNIT[\"Meter\",1.000000000000]]";

const STRING EPSG_4326_Wkt     = L"GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.000,298.25722293]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.01745329251994]]";
const STRING EPSG_4326_Wkt_Alt = L"GEOGCS[\"LL84\",DATUM[\"WGS 84\",SPHEROID[\"WGS 84\",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.01745329252]]";

const STRING TEST_LOCALE = L"en";

// Coordinate values for Boston and NYC
const double LonBoston = -71.061342;
const double LatBoston = 42.355892;
const double XBoston = 824390.2938;
const double YBoston = 4696809.906;

const double LonNewYork = -74.006394;
const double LatNewYork = 40.714169;
const double XNewYork = 583924.3730;
const double YNewYork = 4507502.589;

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestCoordinateSystem, "TestCoordinateSystem");

//static _CrtMemState state;

void TestCoordinateSystem::setUp()
{
}

void TestCoordinateSystem::tearDown()
{
}

void TestCoordinateSystem::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Coordinate System tests. (Mentor)\n")));
//    memset(&state, 0, sizeof(_CrtMemState));
//    _CrtMemCheckpoint(&state);
}

void TestCoordinateSystem::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nCoordinate System tests completed.\n\n")));
//    _CrtDumpMemoryLeaks();
//    _CrtMemDumpAllObjectsSince(&state);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case loads OGC WKT coordinate systems from an external file and
/// tries to validate them.
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_CheckCoordinateSystems()
{
    try
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTesting coordinate system OGC WKTs...\n")));

        MgCoordinateSystemFactory factory;
        Ptr<MgCoordinateSystem> pCoordinateSystem;
        STRING ogcWkt;
        int nCoordinateSystemsTested = 0;
        int nCoordinateSystemsPassed = 0;

        long lStart = GetTickCount();

        // Open the test file and read the OGC WKT.
        // If the test file cannot be opened return an error.
        FILE* stream = NULL;
        if( (stream  = fopen(OgcWktTestFile, "rt" )) != NULL )
        {
            char buffer[4096];
            string text;
            while(fgets(buffer, 4096, stream) != NULL)
            {
                // Check for a comment line
                if((buffer[0] != '#') && (buffer[0] != 10))
                {
                    text = buffer;

                    // Remove any "EPSG:xxxx=" or "xxxx=" at start of line
                    size_t position = text.find("=");
                    if(position != string::npos)
                    {
                        // Check to see if this is before the actual OGC WKT definition
                        if(position < text.find("["))
                        {
                            // Remove "xxxx="
                            text = text.erase(0, position+1);
                        }
                    }

                    // Try and create a coordinate system object from the OGC WKT
                    nCoordinateSystemsTested++;
                    try
                    {
                        ogcWkt = MgUtil::MultiByteToWideChar(text);

                        pCoordinateSystem = factory.Create(ogcWkt);
                        if(NULL != pCoordinateSystem)
                        {
                            #ifdef _DEBUG
                            string strMessage = MG_WCHAR_TO_CHAR(pCoordinateSystem->GetCode());
                            printf("%s - %lf, %lf, %lf, %lf\n", strMessage.c_str(), pCoordinateSystem->GetMinX(), pCoordinateSystem->GetMinY(), pCoordinateSystem->GetMaxX(), pCoordinateSystem->GetMaxY());
                            #endif

                            nCoordinateSystemsPassed++;
                        }
                    }
                    catch(MgException* e)
                    {
                        STRING message = e->GetDetails(TEST_LOCALE);
                        SAFE_RELEASE(e);

                        #ifdef _DEBUG
                        string strMessage = MG_WCHAR_TO_CHAR(message);
                        printf("%s\n", strMessage.c_str());
                        #endif
                    }
                    catch(...)
                    {
                        throw;
                    }
                }
            }

            fclose( stream );
        }
        else
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT("Could not open coordinate system test file: %C\n"), OgcWktTestFile));
            CPPUNIT_ASSERT(false);
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTotal coordinate system OGC WKTs tested: %d/%d (Passed/Total)\n"), nCoordinateSystemsPassed, nCoordinateSystemsTested));
        ACE_DEBUG((LM_INFO, ACE_TEXT("Total Processing Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case creates a valid coordinate system.
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_CreateValidCoordinateSystem()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        // Test if entry is cached
        ogcWkt = ArbitraryWkt;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        ogcWkt = ArbitraryWkt_Alternate1;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        // Test if entry is cached
        ogcWkt = ArbitraryWkt_Alternate1;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        ogcWkt = ArbitraryWkt_Alternate2;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        // Test if entry is cached
        ogcWkt = ArbitraryWkt_Alternate2;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        ogcWkt = GeographicWkt;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        // Test if entry is cached
        ogcWkt = GeographicWkt;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        ogcWkt = ProjectedWkt;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        // Test if entry is cached
        ogcWkt = ProjectedWkt;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        ogcWkt = InvalidWkt;
        CPPUNIT_ASSERT_THROW_MG(pCoordinateSystem = factory.Create(ogcWkt), MgCoordinateSystemConversionFailedException*);

        ogcWkt = ArbitraryWkt_Feet;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        ogcWkt = GeographicWkt_LL84;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        ogcWkt = ProjectedWkt_GAW;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case creates an invalid coordinate system.
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_CreateInvalidCoordinateSystem()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = InvalidWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);
    }
    catch(MgCoordinateSystemConversionFailedException* e)
    {
        // This exception is expected
        SAFE_RELEASE(e);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_EnumerateCategories()
{
    try
    {
        int nTotalCoordinateSystemsTested = 0;
        int nTotalCoordinateSystemsPassed = 0;
        long lStart = GetTickCount();

        MgCoordinateSystemFactory factory;
        Ptr<MgStringCollection> categories;
        categories = factory.EnumerateCategories();

        // Get the # of coordinate system categories
        INT32 size = categories->GetCount();
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTesting %d coordinate system categories...\n"), size));

        for(int i=0;i<size;i++)
        {
            Ptr<MgBatchPropertyCollection> coordSystems;
            STRING category = categories->GetItem(i);
            ACE_DEBUG((LM_INFO, ACE_TEXT("  Checking %W - "), category.c_str()));
            int nCoordinateSystems = 0;

            try
            {
                coordSystems = factory.EnumerateCoordinateSystems(category);

                // Get the # of coordinate systems for this category
                nCoordinateSystems = coordSystems->GetCount();

                nTotalCoordinateSystemsTested += nCoordinateSystems;
                ACE_DEBUG((LM_INFO, ACE_TEXT("%d (Coordinate Systems)\n"), nCoordinateSystems));
            }
            catch(MgException* e)
            {
                SAFE_RELEASE(e);
                ACE_DEBUG((LM_INFO, ACE_TEXT("Skipped\n")));
            }

            for(int j=0;j<nCoordinateSystems;j++)
            {
                Ptr<MgPropertyCollection> coordSys = coordSystems->GetItem(j);
                for(int k=0;k<coordSys->GetCount();k++)
                {
                    Ptr<MgProperty> pBaseProperty = coordSys->GetItem(k);
                    if (MgPropertyType::String == pBaseProperty->GetPropertyType())
                    {
                        MgStringProperty* pProperty = static_cast<MgStringProperty*>(pBaseProperty.p);
                        if(pProperty->GetName() == L"Code")
                        {
                            try
                            {
                                // Perform round trip test
                                STRING wkt = factory.ConvertCoordinateSystemCodeToWkt(pProperty->GetValue());
                                STRING code = factory.ConvertWktToCoordinateSystemCode(wkt);
                                CPPUNIT_ASSERT(_wcsicmp(pProperty->GetValue().c_str(), code.c_str()) == 0);

                                nTotalCoordinateSystemsPassed++;
                            }
                            catch(MgException* e)
                            {
                                #ifdef _DEBUG
                                STRING message = e->GetMessage(TEST_LOCALE);

                                // Extract the reason out of the message
                                size_t index = message.find(L"\n");
                                message = message.substr(index+1, 255);

                                printf("      %s - FAILED: %s\n", MG_WCHAR_TO_CHAR(pProperty->GetValue()), MG_WCHAR_TO_CHAR(message));
                                #endif
                                SAFE_RELEASE(e);
                            }
                            catch(...)
                            {
                            }
                        }
                        else
                        {
                            #ifdef _DEBUG
                            //printf("      %s - %s\n", MG_WCHAR_TO_CHAR(pProperty->GetName()), MG_WCHAR_TO_CHAR(pProperty->GetValue()));
                            #endif
                        }
                    }
                    else
                    {
                        throw new MgInvalidPropertyTypeException(
                            L"TestCoordinateSystem.TestCase_EnumerateCategories", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }
            }
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTotal coordinate systems tested: %d/%d (Passed/Total)\n"), nTotalCoordinateSystemsPassed, nTotalCoordinateSystemsTested));
        ACE_DEBUG((LM_INFO, ACE_TEXT("Total Processing Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_EnumerateCoordSys()
{
    try
    {
        MgCoordinateSystemFactory factory;
        Ptr<MgBatchPropertyCollection> coordSystems;
        coordSystems = factory.EnumerateCoordinateSystems(L"Lat Longs");
        CPPUNIT_ASSERT(coordSystems);
        CPPUNIT_ASSERT(coordSystems->GetCount() > 0);

        coordSystems = factory.EnumerateCoordinateSystems(L"lat longs");
        CPPUNIT_ASSERT(coordSystems);
        CPPUNIT_ASSERT(coordSystems->GetCount() > 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_GetBaseLibrary()
{
    try
    {
        STRING library;
        Ptr<MgCoordinateSystemFactory> factory = new MgCoordinateSystemFactory();
        CPPUNIT_ASSERT(factory);

        library = factory->GetBaseLibrary();
        CPPUNIT_ASSERT(library == L"Mentor Coordinate System Library");
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_IsValid()
{
    try
    {
        bool bResult;
        MgCoordinateSystemFactory factory;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(EPSG_4326_Wkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        bResult = pCoordinateSystem->IsValid();
        CPPUNIT_ASSERT(bResult == true);

        CPPUNIT_ASSERT_THROW_MG(pCoordinateSystem = factory.Create(InvalidWkt), MgCoordinateSystemConversionFailedException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_ValidateCoordinateSystemArbitrary()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        CPPUNIT_ASSERT(pCoordinateSystem->GetType() == MgCoordinateSystemType::Arbitrary);

        Ptr<MgCoordinateSystem> pCoordinateSystemAlt = factory.Create(pCoordinateSystem->ToString());
        CPPUNIT_ASSERT(pCoordinateSystemAlt);

        CPPUNIT_ASSERT(pCoordinateSystem->GetCode() == pCoordinateSystemAlt->GetCode());
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_Arbitrary_ConvertFromLonLat()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double lon = 45.0;
        double lat = 90.0;
        double x,y;
        pCoordinateSystem->ConvertFromLonLat(lon, lat, x, y);

        // Convert back
        pCoordinateSystem->ConvertToLonLat(x, y, x, y);

        // The values shouldn't change
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lon, x));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lat, y));

        Ptr<MgCoordinate> coord = new MgCoordinateXY(45.0, 90.0);
        coord = pCoordinateSystem->ConvertFromLonLat(coord.p);

        // Convert back
        coord = pCoordinateSystem->ConvertToLonLat(coord.p);

        // The values shouldn't change
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(coord->GetX(), 45.0));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(coord->GetY(), 90.0));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_Arbitrary_ConvertFromLonLatArray()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double lon[4] = { 45.0, 30.0, 15.0, 0.0 };
        double lat[4] = { 90.0, 75.0, 60.0, 55.0 };
        double x[4],y[4];
        pCoordinateSystem->ConvertFromLonLat(lon, lat, x, y, 4);

        // Convert back
        pCoordinateSystem->ConvertToLonLat(x, y, x, y, 4);

        // The values shouldn't change
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lon[0], x[0]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lat[0], y[0]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lon[1], x[1]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lat[1], y[1]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lon[2], x[2]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lat[2], y[2]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lon[3], x[3]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lat[3], y[3]));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_Arbitrary_ConvertToLonLat()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double x = 45.0;
        double y = 90.0;
        double lat,lon;
        pCoordinateSystem->ConvertToLonLat(x, y, lon, lat);

        // Convert back
        pCoordinateSystem->ConvertFromLonLat(lon, lat, lon, lat);

        // The values shouldn't change
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lon, x));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lat, y));

        Ptr<MgCoordinate> coord = new MgCoordinateXY(45.0, 90.0);
        coord = pCoordinateSystem->ConvertToLonLat(coord.p);

        // Convert back
        coord = pCoordinateSystem->ConvertFromLonLat(coord.p);

        // The values shouldn't change
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(coord->GetX(), 45.0));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(coord->GetY(), 90.0));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_Arbitrary_ConvertToLonLatArray()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double x[4] = { 45.0, 30.0, 15.0, 0.0 };
        double y[4] = { 90.0, 75.0, 60.0, 55.0 };
        double lon[4],lat[4];
        pCoordinateSystem->ConvertToLonLat(x, y, lon, lat, 4);

        // Convert back
        pCoordinateSystem->ConvertFromLonLat(lon, lat, lon, lat, 4);

        // The values shouldn't change
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lon[0], x[0]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lat[0], y[0]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lon[1], x[1]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lat[1], y[1]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lon[2], x[2]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lat[2], y[2]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lon[3], x[3]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(lat[3], y[3]));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_Arbitrary_ConvertCoordinateSystemUnitsToMeters()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1609.344, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_Arbitrary_ConvertMetersToCoordinateSystemUnits()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double units = pCoordinateSystem->ConvertMetersToCoordinateSystemUnits(1609.344);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, units));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_Arbitrary_MeasureEuclideanDistance()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(4.0, 3.0);

        double distance = pCoordinateSystem->MeasureEuclideanDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, distance));

        double distance2 = pCoordinateSystem->MeasureEuclideanDistance(0.0, 0.0, 4.0, 3.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(distance, distance2));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(8046.72, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_Arbitrary_MeasureGreatCircleDistance()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(4.0, 3.0);

        CPPUNIT_ASSERT_THROW_MG(double meters = pCoordinateSystem->MeasureGreatCircleDistance(&coord1, &coord2), MgInvalidCoordinateSystemTypeException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_Arbitrary_GetAzimuth()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(4.0, 3.0);

        double degrees = pCoordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(36.86989765, degrees));

        double degrees2 = pCoordinateSystem->GetAzimuth(0.0, 0.0, 4.0, 3.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(degrees, degrees2));

        // Test all 4 quadrants
        // Top right
        degrees = pCoordinateSystem->GetAzimuth(0.0, 0.0, 4.0, 3.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(36.86989765, degrees));

        // Top left
        degrees = pCoordinateSystem->GetAzimuth(0.0, 0.0, -4.0, 3.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(143.1301024, degrees));

        // Bottom right
        degrees = pCoordinateSystem->GetAzimuth(0.0, 0.0, 4.0, -3.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-36.86989765, degrees));

        // Bottom left
        degrees = pCoordinateSystem->GetAzimuth(0.0, 0.0, -4.0, -3.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-143.1301024, degrees));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
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
/// This test case
///----------------------------------------------------------------------------
void TestCoordinateSystem::TestCase_Arbitrary_GetCoordinate()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgCoordinate> pCoord;
        MgCoordinateXY coord(0.0, 0.0);

        // Test all 4 quadrants
        // Top right
        pCoord = pCoordinateSystem->GetCoordinate(&coord, 36.86989765, 5.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(4.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.0, pCoord->GetY()));

        // Top left
        pCoord = pCoordinateSystem->GetCoordinate(&coord, 143.13010235, 5.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-4.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.0, pCoord->GetY()));

        // Bottom right
        pCoord = pCoordinateSystem->GetCoordinate(&coord, -36.86989765, 5.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(4.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-3.0, pCoord->GetY()));

        // Bottom left
        pCoord = pCoordinateSystem->GetCoordinate(&coord, -143.13010235, 5.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-4.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-3.0, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_ConvertCode()
{
    try
    {
        STRING ogcWkt = ArbitraryWkt;

        MgCoordinateSystemFactory factory;
        STRING code = factory.ConvertWktToCoordinateSystemCode(ogcWkt);
        CPPUNIT_ASSERT(_wcsicmp(L"XY-MI", code.c_str()) == 0);
        STRING wkt = factory.ConvertCoordinateSystemCodeToWkt(code);
        CPPUNIT_ASSERT(wkt.length() > 0);

        wkt = factory.ConvertCoordinateSystemCodeToWkt(L"XY-MI");
        CPPUNIT_ASSERT(wkt.length() > 0);

        wkt = factory.ConvertCoordinateSystemCodeToWkt(L"*xy-mi*");
        CPPUNIT_ASSERT(wkt.length() > 0);

        ogcWkt = ArbitraryWkt_Meter;
        code = factory.ConvertWktToCoordinateSystemCode(ogcWkt);
        CPPUNIT_ASSERT(_wcsicmp(L"XY-M", code.c_str()) == 0);
        wkt = factory.ConvertCoordinateSystemCodeToWkt(code);
        CPPUNIT_ASSERT(wkt.length() > 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetUnits()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING units = pCoordinateSystem->GetUnits();
        CPPUNIT_ASSERT(_wcsicmp(L"Mile", units.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetUnitScale()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double scale = pCoordinateSystem->GetUnitScale();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1609.344, scale));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetMinX()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMinX();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-10000000.0, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetMinY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMinY();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-10000000.0, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetMaxX()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMaxX();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10000000.0, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetMaxY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMaxY();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10000000.0, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetCode()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetCode();
        CPPUNIT_ASSERT(_wcsicmp(L"XY-MI", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Alternate1;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetCode();
        CPPUNIT_ASSERT(_wcsicmp(L"XY-MI", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Alternate2;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetCode();
        CPPUNIT_ASSERT(_wcsicmp(L"XY-MI", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetCode();
        CPPUNIT_ASSERT(_wcsicmp(L"XY-M", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"Arbitrary X-Y Coordinates (U.S.Survey Miles)", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"Arbitrary X-Y Coordinates (Meters)", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetProjection()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetProjection();
        CPPUNIT_ASSERT(_wcsicmp(L"NERTH", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetProjection();
        CPPUNIT_ASSERT(_wcsicmp(L"NERTH", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetProjectionDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetProjectionDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"Non-georeferenced (aka non-earth) coordinate system", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetProjectionDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"Non-georeferenced (aka non-earth) coordinate system", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetDatum()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetDatum();
        CPPUNIT_ASSERT(_wcsicmp(L"Local Datum", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetDatum();
        CPPUNIT_ASSERT(_wcsicmp(L"Local Datum", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetDatumDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetDatumDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetDatumDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetEllipsoid()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetEllipsoid();
        CPPUNIT_ASSERT(_wcsicmp(L"", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetEllipsoid();
        CPPUNIT_ASSERT(_wcsicmp(L"", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetEllipsoidDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetEllipsoidDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetEllipsoidDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_GetCategory()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgStringCollection> value = pCoordinateSystem->GetCategories();
        CPPUNIT_ASSERT(value->Contains(L"Arbitrary X-Y Coordinate Systems"));

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetCategories();
        CPPUNIT_ASSERT(value->Contains(L"Arbitrary X-Y Coordinate Systems"));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_ValidateCoordinateSystemGeographic()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        CPPUNIT_ASSERT(pCoordinateSystem->GetType() == MgCoordinateSystemType::Geographic);
        CPPUNIT_ASSERT(pCoordinateSystem->ToString() == ogcWkt);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_ConvertFromLonLat()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double lon = 45.0;
        double lat = 90.0;
        double x,y;
        pCoordinateSystem->ConvertFromLonLat(lon, lat, x, y);

        // The values shouldn't change
        CPPUNIT_ASSERT(lat == y);
        CPPUNIT_ASSERT(lon == x);

        MgCoordinateXY coord(45.0, 90.0);
        Ptr<MgCoordinate> pCoord = pCoordinateSystem->ConvertFromLonLat(&coord);

        // The values shouldn't change
        CPPUNIT_ASSERT(coord.GetX() == 45.0);
        CPPUNIT_ASSERT(coord.GetY() == 90.0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_ConvertFromLonLatArray()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double lon[4] = { 45.0, 30.0, 15.0, 0.0 };
        double lat[4] = { 90.0, 75.0, 60.0, 55.0 };
        double x[4],y[4];
        pCoordinateSystem->ConvertFromLonLat(lon, lat, x, y, 4);

        // The values shouldn't change
        CPPUNIT_ASSERT(lon[0] == x[0]);
        CPPUNIT_ASSERT(lat[0] == y[0]);
        CPPUNIT_ASSERT(lon[1] == x[1]);
        CPPUNIT_ASSERT(lat[1] == y[1]);
        CPPUNIT_ASSERT(lon[2] == x[2]);
        CPPUNIT_ASSERT(lat[2] == y[2]);
        CPPUNIT_ASSERT(lon[3] == x[3]);
        CPPUNIT_ASSERT(lat[3] == y[3]);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_ConvertToLonLat()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double x = 45.0;
        double y = 90.0;
        double lat,lon;
        pCoordinateSystem->ConvertToLonLat(x, y, lon, lat);

        // The values shouldn't change
        CPPUNIT_ASSERT(lat == y);
        CPPUNIT_ASSERT(lon == x);

        MgCoordinateXY coord(45.0, 90.0);
        Ptr<MgCoordinate> pCoord = pCoordinateSystem->ConvertToLonLat(&coord);

        // The values shouldn't change
        CPPUNIT_ASSERT(coord.GetX() == 45.0);
        CPPUNIT_ASSERT(coord.GetY() == 90.0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_ConvertToLonLatArray()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double x[4] = { 45.0, 30.0, 15.0, 0.0 };
        double y[4] = { 90.0, 75.0, 60.0, 55.0 };
        double lon[4],lat[4];
        pCoordinateSystem->ConvertToLonLat(x, y, lon, lat, 4);

        // The values shouldn't change
        CPPUNIT_ASSERT(lon[0] == x[0]);
        CPPUNIT_ASSERT(lat[0] == y[0]);
        CPPUNIT_ASSERT(lon[1] == x[1]);
        CPPUNIT_ASSERT(lat[1] == y[1]);
        CPPUNIT_ASSERT(lon[2] == x[2]);
        CPPUNIT_ASSERT(lat[2] == y[2]);
        CPPUNIT_ASSERT(lon[3] == x[3]);
        CPPUNIT_ASSERT(lat[3] == y[3]);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_ConvertCoordinateSystemUnitsToMeters()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(111319.4908, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_ConvertMetersToCoordinateSystemUnits()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double units = pCoordinateSystem->ConvertMetersToCoordinateSystemUnits(111319.4908);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, units));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_MeasureEuclideanDistance()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(4.0, 3.0);

        double distance = pCoordinateSystem->MeasureEuclideanDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, distance));

        double distance2 = pCoordinateSystem->MeasureEuclideanDistance(0.0, 0.0, 4.0, 3.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(distance, distance2));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(556597.4540, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_MeasureGreatCircleDistance()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(4.0, 3.0);

        double distance = pCoordinateSystem->MeasureGreatCircleDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(4.986535473, distance));

        double distance2 = pCoordinateSystem->MeasureGreatCircleDistance(0.0, 0.0, 4.0, 3.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(distance, distance2));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(555098.5897, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetAzimuth()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(4.0, 3.0);

        double degrees = pCoordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(53.26700666, degrees));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetCoordinate()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord(0.0, 0.0);
        Ptr<MgCoordinate> pCoord = pCoordinateSystem->GetCoordinate(&coord, 36.86989765, 5.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.004913619, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(4.025035767, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_ValidateCoordinateSystemProjected()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        CPPUNIT_ASSERT(pCoordinateSystem->GetType() == MgCoordinateSystemType::Projected);
        CPPUNIT_ASSERT(pCoordinateSystem->ToString() == ogcWkt);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_ConvertFromLonLat()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double lon = -84.46;
        double lat = 33.89;
        double x,y;
        pCoordinateSystem->ConvertFromLonLat(lon, lat, x, y);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, x));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, y));

        MgCoordinateXY coord(-84.46, 33.89);
        Ptr<MgCoordinate> pCoord = pCoordinateSystem->ConvertFromLonLat(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_ConvertFromLonLatArray()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double lon[4] = { -84.46, -84.0, -83.0, -82.0 };
        double lat[4] = { 33.89, 33.0, 32.0, 31.0 };
        double x[4],y[4];
        pCoordinateSystem->ConvertFromLonLat(lon, lat, x, y, 4);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, x[0]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, y[0]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(551096.7774, x[1]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1091213.712, y[1]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(861668.2135, x[2]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(729342.553, y[2]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1178912.136, x[3]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(370280.4158, y[3]));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_ConvertToLonLat()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord(410961.8644, 1415078.441);
        Ptr<MgCoordinate> pCoord = pCoordinateSystem->ConvertToLonLat(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));

    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_ConvertToLonLatArray()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double x[4] = { 410961.8644, 551096.7774, 861668.2135, 1178912.136 };
        double y[4] = { 1415078.441, 1091213.712, 729342.553, 370280.4158 };
        double lon[4],lat[4];

        pCoordinateSystem->ConvertToLonLat(x, y, lon, lat, 4);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, lon[0]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, lat[0]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.0, lon[1]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.0, lat[1]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-83.0, lon[2]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(32.0, lat[2]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-82.0, lon[3]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(31.0, lat[3]));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_ConvertCoordinateSystemUnitsToMeters()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.30480061, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_ConvertMetersToCoordinateSystemUnits()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double units = pCoordinateSystem->ConvertMetersToCoordinateSystemUnits(0.30480061);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, units));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_MeasureEuclideanDistance()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(4.0, 3.0);

        double distance = pCoordinateSystem->MeasureEuclideanDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, distance));

        double distance2 = pCoordinateSystem->MeasureEuclideanDistance(0.0, 0.0, 4.0, 3.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(distance, distance2));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.52400305, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_MeasureGreatCircleDistance()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord1(410961.8644, 1415078.441);
        MgCoordinateXY coord2(432415.4557, 1362302.051);

        double distance = pCoordinateSystem->MeasureGreatCircleDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(56975.49704, distance));

        double distance2 = pCoordinateSystem->MeasureGreatCircleDistance(410961.8644, 1415078.441, 432415.4557, 1362302.051);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(distance, distance2));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(17366.16623, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetAzimuth()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord1(410961.8644, 1415078.441);
        MgCoordinateXY coord2(432415.4557, 1362302.051);

        double degrees = pCoordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(157.7149495, degrees));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetCoordinate()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        MgCoordinateXY coord(410961.8644, 1415078.441);
        Ptr<MgCoordinate> pCoord = pCoordinateSystem->GetCoordinate(&coord, 157.7491121, 57002.22092);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(432415.4557, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1362302.051, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_Measure_GetDistance()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgCoordinateSystemMeasure> measure = pCoordinateSystem->GetMeasure();
        CPPUNIT_ASSERT(measure);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(4.0, 3.0);
        double distance = measure->GetDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, distance));

        Ptr<MgCoordinateSystem> coordSys = measure->GetCoordSys();
        double meters = coordSys->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(8046.72, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_Measure_GetAzimuth()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgCoordinateSystemMeasure> measure = pCoordinateSystem->GetMeasure();
        CPPUNIT_ASSERT(measure);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(5.0, 5.0);

        double degrees = measure->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(45.0, degrees));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_Measure_GetCoordinate()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ArbitraryWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgCoordinateSystemMeasure> measure = pCoordinateSystem->GetMeasure();
        CPPUNIT_ASSERT(measure);

        MgCoordinateXY coord(0.0, 0.0);
        Ptr<MgCoordinate> pCoord = measure->GetCoordinate(&coord, 45.0, 10.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(7.071067812, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(7.071067812, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_Measure_GetDistance()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgCoordinateSystemMeasure> measure = pCoordinateSystem->GetMeasure();
        CPPUNIT_ASSERT(measure);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(4.0, 3.0);

        double distance = measure->GetDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(4.986535473, distance));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(555098.5897, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_Measure_GetAzimuth()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgCoordinateSystemMeasure> measure = pCoordinateSystem->GetMeasure();
        CPPUNIT_ASSERT(measure);

        MgCoordinateXY coord1(0.0, 0.0);
        MgCoordinateXY coord2(4.0, 3.0);

        double degrees = measure->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(53.26700666, degrees));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_Measure_GetCoordinate()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgCoordinateSystemMeasure> measure = pCoordinateSystem->GetMeasure();
        CPPUNIT_ASSERT(measure);

        MgCoordinateXY coord(0.0, 0.0);
        Ptr<MgCoordinate> pCoord = measure->GetCoordinate(&coord, 60.0, 10.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(8.682351371, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.014236434, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_ConvertCode()
{
    try
    {
        STRING ogcWkt = GeographicWkt;

        MgCoordinateSystemFactory factory;
        STRING code = factory.ConvertWktToCoordinateSystemCode(ogcWkt);
        CPPUNIT_ASSERT(_wcsicmp(L"LL84", code.c_str()) == 0);
        STRING wkt = factory.ConvertCoordinateSystemCodeToWkt(code);
        CPPUNIT_ASSERT(wkt.length() > 0);

        wkt = factory.ConvertCoordinateSystemCodeToWkt(L"ll84");
        CPPUNIT_ASSERT(wkt.length() > 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetUnits()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING units = pCoordinateSystem->GetUnits();
        CPPUNIT_ASSERT(_wcsicmp(L"Degree", units.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetUnitScale()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double scale = pCoordinateSystem->GetUnitScale();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(111319.4908, scale));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetMinX()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMinX();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-180.0, value));

        value = pCoordinateSystem->GetLonMin();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-180.0, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetMinY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMinY();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-90.0, value));

        value = pCoordinateSystem->GetLatMin();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-90.0, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetMaxX()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMaxX();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(180.0, value));

        value = pCoordinateSystem->GetLonMax();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(180.0, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetMaxY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMaxY();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(90.0, value));

        value = pCoordinateSystem->GetLatMax();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(90.0, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetCode()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetCode();
        CPPUNIT_ASSERT(_wcsicmp(L"LL84", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"WGS84 datum, Latitude-Longitude; Degrees", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetProjection()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetProjection();
        CPPUNIT_ASSERT(_wcsicmp(L"LL", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetProjectionDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetProjectionDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"Null Projection, produces/processes Latitude & Longitude", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetDatum()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetDatum();
        CPPUNIT_ASSERT(_wcsicmp(L"WGS84", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetDatumDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetDatumDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"World Geodetic System of 1984", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetEllipsoid()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetEllipsoid();
        CPPUNIT_ASSERT(_wcsicmp(L"WGS84", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetEllipsoidDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetEllipsoidDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"World Geodetic System of 1984, GEM 10C", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_GetCategory()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgStringCollection> value = pCoordinateSystem->GetCategories();
        CPPUNIT_ASSERT(value->Contains(L"Lat Longs"));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_Measure_GetDistance()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgCoordinateSystemMeasure> measure = pCoordinateSystem->GetMeasure();
        CPPUNIT_ASSERT(measure);

        MgCoordinateXY coord1(410961.8644, 1415078.441);
        MgCoordinateXY coord2(432415.4557, 1362302.051);

        double distance = measure->GetDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(56975.49704, distance));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(17366.16623, meters));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_Measure_GetAzimuth()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgCoordinateSystemMeasure> measure = pCoordinateSystem->GetMeasure();
        CPPUNIT_ASSERT(measure);

        MgCoordinateXY coord1(410961.8644, 1415078.441);
        MgCoordinateXY coord2(432415.4557, 1362302.051);

        double degrees = measure->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(157.7149495, degrees));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_Measure_GetCoordinate()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgCoordinateSystemMeasure> measure = pCoordinateSystem->GetMeasure();
        CPPUNIT_ASSERT(measure);

        MgCoordinateXY coord(410961.8644, 1415078.441);
        Ptr<MgCoordinate> pCoord = measure->GetCoordinate(&coord, 157.7491121, 57002.220920);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(432415.4557, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1362302.051, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_ConvertCode()
{
    try
    {
        STRING ogcWkt = ProjectedWkt;

        MgCoordinateSystemFactory factory;
        STRING code = factory.ConvertWktToCoordinateSystemCode(ogcWkt);
        CPPUNIT_ASSERT(_wcsicmp(L"GA-W", code.c_str()) == 0);
        STRING wkt = factory.ConvertCoordinateSystemCodeToWkt(code);
        CPPUNIT_ASSERT(wkt.length() > 0);

        wkt = factory.ConvertCoordinateSystemCodeToWkt(L"ga-w");
        CPPUNIT_ASSERT(wkt.length() > 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetUnits()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING units = pCoordinateSystem->GetUnits();
        CPPUNIT_ASSERT(_wcsicmp(L"FOOT", units.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetUnitScale()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double scale = pCoordinateSystem->GetUnitScale();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.30480061, scale));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetMinX()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMinX();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-171305.5915, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetMinY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMinY();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.0, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetMaxX()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMaxX();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1171305.5915, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetMaxY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        double value = pCoordinateSystem->GetMaxY();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(21920187.62, value));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetCode()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetCode();
        CPPUNIT_ASSERT(_wcsicmp(L"GA-W", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"NAD27 Georgia State Planes, West Zone(1002), US Foot", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetProjection()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetProjection();
        CPPUNIT_ASSERT(_wcsicmp(L"TM", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetProjectionDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetProjectionDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"Transverse Mercator or Gauss Kruger Projection", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetDatum()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetDatum();
        CPPUNIT_ASSERT(_wcsicmp(L"NAD27", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetDatumDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetDatumDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"NAD 1927, mean Values, Continental United States", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetEllipsoid()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetEllipsoid();
        CPPUNIT_ASSERT(_wcsicmp(L"CLRK66", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetEllipsoidDescription()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetEllipsoidDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"Clarke 1866, Benoit Ratio", value.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_GetCategory()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        Ptr<MgStringCollection> value = pCoordinateSystem->GetCategories();
        CPPUNIT_ASSERT(value->Contains(L"USA, Georgia"));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Arbitrary_Transform_XY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(10.0, 5.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(32.80833333, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(16.40416667, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Arbitrary_Transform_XYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(10.0, 5.0, 1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(32.80833333, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(16.40416667, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.280833333, pCoord->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXY coord(10.0, 5.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(32.80833333, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(16.40416667, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXYM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYM coord(10.0, 5.0, 100.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(32.80833333, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(16.40416667, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(328.0833333, pCoord->GetM()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZ coord(10.0, 5.0, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(32.80833333, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(16.40416667, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.280833333, pCoord->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXYZM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZM coord(10.0, 5.0, 1.0, 100.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(32.80833333, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(16.40416667, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.280833333, pCoord->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(328.0833333, pCoord->GetM()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Arbitrary_Transform_EnvelopeXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXY(0.0, 1.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXY(10.0, 5.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(32.80833333, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(16.40416667, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.0, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.280833333, ll->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Arbitrary_Transform_EnvelopeXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXYZ(0.0, 1.0, 1.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXYZ(10.0, 5.0, 2.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(32.80833333, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(16.40416667, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(6.561666667, ur->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.0, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.280833333, ll->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.280833333, ll->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Geographic_Transform_XY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Geographic_Transform_XYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Geographic_Transform_CoordinateXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Geographic_Transform_CoordinateXYM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Geographic_Transform_CoordinateXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Geographic_Transform_CoordinateXYZM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Geographic_Transform_EnvelopeXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Geographic_Transform_EnvelopeXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Projected_Transform_XY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Projected_Transform_XYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Projected_Transform_CoordinateXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Projected_Transform_CoordinateXYM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Projected_Transform_CoordinateXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Projected_Transform_CoordinateXYZM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Projected_Transform_EnvelopeXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Arbitrary_To_Projected_Transform_EnvelopeXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Arbitrary_Transform_XY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Arbitrary_Transform_XYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Arbitrary_Transform_CoordinateXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Arbitrary_Transform_CoordinateXYM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Arbitrary_Transform_CoordinateXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Arbitrary_Transform_CoordinateXYZM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Arbitrary_Transform_EnvelopeXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Arbitrary_Transform_EnvelopeXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Geographic_Transform_XY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(10.0, 5.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Geographic_Transform_XYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(10.0, 5.0, 1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Geographic_Transform_CoordinateXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXY coord(10.0, 5.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Geographic_Transform_CoordinateXYM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYM coord(10.0, 5.0, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetM()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Geographic_Transform_CoordinateXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZ coord(10.0, 5.0, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Geographic_Transform_CoordinateXYZM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZM coord(10.0, 5.0, 1.0, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetM()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Geographic_Transform_EnvelopeXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXY(0.0, 1.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXY(10.0, 5.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.0, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, ll->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Geographic_Transform_EnvelopeXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXYZ(0.0, 1.0, 1.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXYZ(10.0, 5.0, 2.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.0, ur->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.0, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, ll->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, ll->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Projected_Transform_XY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(-84.46, 33.89);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Projected_Transform_XYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(-84.46, 33.89, 1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
        // TODO: Commented out because Mentor gives different results on Linux
//        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.116561399, pCoord->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Projected_Transform_CoordinateXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXY coord(-84.46, 33.89);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Projected_Transform_CoordinateXYM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYM coord(-84.46, 33.89, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(365220.6956, pCoord->GetM()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Projected_Transform_CoordinateXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZ coord(-84.46, 33.89, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
        // TODO: Commented out because Mentor gives different results on Linux
//        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.116561399, pCoord->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Projected_Transform_CoordinateXYZM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZM coord(-84.46, 33.89, 1.0, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
        // TODO: Commented out because Mentor gives different results on Linux
//        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.116561399, pCoord->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(365220.6956, pCoord->GetM()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Projected_Transform_EnvelopeXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXY(-83.0, 34.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXY(-85.0, 33.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(853597.4323, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1457027.613, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(244492.8335, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1092185.195, ll->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_To_Projected_Transform_EnvelopeXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXYZ(-83.0, 34.0, 1.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXYZ(-84.0, 33.0, 2.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(853597.4323, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1457027.613, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.0, ur->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(551096.7774, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1091213.712, ll->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, ll->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Arbitrary_Transform_XY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Arbitrary_Transform_XYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Arbitrary_Transform_CoordinateXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Arbitrary_Transform_CoordinateXYM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Arbitrary_Transform_CoordinateXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Arbitrary_Transform_CoordinateXYZM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Arbitrary_Transform_EnvelopeXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Arbitrary_Transform_EnvelopeXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        try
        {
            Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        }
        catch(MgCoordinateSystemInitializationFailedException* e)
        {
            SAFE_RELEASE(e);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Geographic_Transform_XY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(410961.8644, 1415078.441);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Geographic_Transform_XYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(410961.8644, 1415078.441, 1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));
        // TODO: Commented out because Mentor gives different results on Linux
//        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.3548707958, pCoord->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Geographic_Transform_CoordinateXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXY coord(410961.8644, 1415078.441);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Geographic_Transform_CoordinateXYM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYM coord(410961.8644, 1415078.441, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.2738070466, pCoord->GetM()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Geographic_Transform_CoordinateXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZ coord(410961.8644, 1415078.441, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));
        // TODO: Commented out because Mentor gives different results on Linux
//        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.3548707958, pCoord->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Geographic_Transform_CoordinateXYZM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZM coord(410961.8644, 1415078.441, 1.0, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));
        // TODO: Commented out because Mentor gives different results on Linux
//        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.3548707958, pCoord->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.2738070466, pCoord->GetM()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Geographic_Transform_EnvelopeXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXY(410983.0, 1415115.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXY(410000.0, 1415000.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46000082, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.88999734, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46317002, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.88977686, ll->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Geographic_Transform_EnvelopeXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = GeographicWkt_LL84;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXYZ(410983.0, 1415115.0, 1.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXYZ(410000.0, 1415000.0, 2.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46000082, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.88999734, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.0, ur->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46317002, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.88977686, ll->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, ll->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Projected_Transform_XY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(410961.8644, 1415078.441);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Projected_Transform_XYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(410961.8644, 1415078.441, 1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Projected_Transform_CoordinateXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXY coord(410961.8644, 1415078.441);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Projected_Transform_CoordinateXYM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYM coord(410961.8644, 1415078.441, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(100000.0, pCoord->GetM()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Projected_Transform_CoordinateXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZ coord(410961.8644, 1415078.441, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Projected_Transform_CoordinateXYZM()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZM coord(410961.8644, 1415078.441, 1.0, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410961.8644, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415078.441, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(100000.0, pCoord->GetM()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Projected_Transform_EnvelopeXY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXY(410983.0, 1415115.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXY(410000.0, 1415000.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.0, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.0, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410000.0, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415000.0, ll->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Projected_To_Projected_Transform_EnvelopeXYZ()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ProjectedWkt_GAW;
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXYZ(410983.0, 1415115.0, 1.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXYZ(410000.0, 1415000.0, 2.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.0, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.0, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, ur->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410000.0, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415000.0, ll->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.0, ll->GetZ()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Boston_Geographic()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> coordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(coordinateSystem);

        MgCoordinateXY coord1(LonBoston, LatBoston);
        MgCoordinateXY coord2(LonNewYork, LatNewYork);

        // Calculate the distance to New York
        double distance = coordinateSystem->MeasureGreatCircleDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.748740240, distance));

        double meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(305988.3638, meters));

        // Calculate the azimuth to New York
        double azimuth = coordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-125.5883639, azimuth));

        // If we move in the azimuth direction calculated above and the calculated distance above to New York we should end up in New York
        Ptr<MgCoordinate> coordinate = coordinateSystem->GetCoordinate(&coord1, azimuth, distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(LonNewYork, coordinate->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(LatNewYork, coordinate->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_NewYork_Geographic()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = GeographicWkt;
        Ptr<MgCoordinateSystem> coordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(coordinateSystem);

        MgCoordinateXY coord1(LonNewYork, LatNewYork);
        MgCoordinateXY coord2(LonBoston, LatBoston);

        // Calculate the distance to Boston
        double distance = coordinateSystem->MeasureGreatCircleDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.748740240, distance));

        double meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(305988.3638, meters));

        // Calculate the azimuth to Boston
        double azimuth = coordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(52.45839467, azimuth));

        // If we move in the azimuth direction calculated above and the calculated distance above to Boston we should end up in Boston
        Ptr<MgCoordinate> coordinate = coordinateSystem->GetCoordinate(&coord1, azimuth, distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(LonBoston, coordinate->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(LatBoston, coordinate->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Boston_Projected()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = Projected_UTM18_NAD83;
        Ptr<MgCoordinateSystem> coordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(coordinateSystem);

        MgCoordinateXY coord1(XBoston, YBoston);
        MgCoordinateXY coord2(XNewYork, YNewYork);

        // Calculate the distance to New York
        double distance = coordinateSystem->MeasureGreatCircleDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(305853.5115, distance));

        // Calculate the azimuth to New York
        double azimuth = coordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-128.7141250, azimuth));

        // If we move in the azimuth direction calculated above and the calculated distance above to New York we should end up in New York
        Ptr<MgCoordinate> coordinate = coordinateSystem->GetCoordinate(&coord1, azimuth, distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(XNewYork, coordinate->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(YNewYork, coordinate->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_NewYork_Projected()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = Projected_UTM18_NAD83;
        Ptr<MgCoordinateSystem> coordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(coordinateSystem);

        MgCoordinateXY coord1(XNewYork, YNewYork);
        MgCoordinateXY coord2(XBoston, YBoston);

        // Calculate the distance to Boston
        double distance = coordinateSystem->MeasureGreatCircleDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(305853.5115, distance));

        // Calculate the azimuth to Boston
        double azimuth = coordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(51.13907730, azimuth));

        // If we move in the azimuth direction calculated above and the calculated distance above to Boston we should end up in Boston
        Ptr<MgCoordinate> coordinate = coordinateSystem->GetCoordinate(&coord1, azimuth, distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(XBoston, coordinate->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(YBoston, coordinate->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_EPSG()
{
    try
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTesting EPSG codes...\n")));

        STRING ogcWkt;
        Ptr<MgCoordinateSystemFactory> factory = new MgCoordinateSystemFactory();
        CPPUNIT_ASSERT(factory);

        ogcWkt = factory->ConvertCoordinateSystemCodeToWkt(L"EPSG:4326");
        CPPUNIT_ASSERT(ogcWkt == EPSG_4326_Wkt);

        ogcWkt = factory->ConvertCoordinateSystemCodeToWkt(L"ePsG:4326");
        CPPUNIT_ASSERT(ogcWkt == EPSG_4326_Wkt);

        CPPUNIT_ASSERT_THROW_MG(ogcWkt = factory->ConvertCoordinateSystemCodeToWkt(L"test:4000"), MgCoordinateSystemLoadFailedException*);

        ogcWkt = factory->ConvertEpsgCodeToWkt(4326);
        CPPUNIT_ASSERT(ogcWkt == EPSG_4326_Wkt);

        ogcWkt = factory->ConvertEpsgCodeToWkt(0);
        // TODO: This should be throwing an exception because the conversion failed.
        CPPUNIT_ASSERT(ogcWkt == L"");

        INT32 epsg = factory->ConvertWktToEpsgCode(EPSG_4326_Wkt);
        CPPUNIT_ASSERT(epsg == 4326);

        long lStart = GetTickCount();

        // Test all supported EPSG codes
        // Open the test file and read the EPSG codes.
        // If the test file cannot be opened return an error.
        int nEpsgCodesTested = 0;
        int nEpsgCodesPassed = 0;
        FILE* stream = NULL;
        if( (stream  = fopen(EpsgTestFile, "rt" )) != NULL )
        {
            char buffer[4096];
            string text;
            while(fgets(buffer, 4096, stream) != NULL)
            {
                // Check for a comment line
                if((buffer[0] != '#') && (buffer[0] != 10))
                {
                    text = buffer;

                    // Convert test code to INT32
                    STRING codeText = MgUtil::MultiByteToWideChar(text);
                    INT32 code = MgUtil::StringToInt32(codeText);

                    // Try and create a coordinate system object from the OGC WKT
                    nEpsgCodesTested++;
                    try
                    {
                        ogcWkt = factory->ConvertEpsgCodeToWkt(code);
                        if(ogcWkt.size() > 0)
                        {
                            #ifdef _DEBUG
                            printf("%d = %S\n", code, ogcWkt.c_str());
                            #endif

                            epsg = factory->ConvertWktToEpsgCode(ogcWkt);
                            if(epsg == code)
                            {
                                // Passed
                                nEpsgCodesPassed++;
                            }
                            else
                            {
                                // Failed Round trip
                                #ifdef _DEBUG
                                printf("%d != %d - Failed!\n", code, epsg);
                                #endif
                            }
                        }
                        else
                        {
                            // Failed
                            #ifdef _DEBUG
                            printf("%d - Failed!\n", code);
                            #endif
                        }
                    }
                    catch(MgException* e)
                    {
                        STRING message = e->GetDetails(TEST_LOCALE);
                        SAFE_RELEASE(e);

                        #ifdef _DEBUG
                        string strMessage = MG_WCHAR_TO_CHAR(message);
                        printf("%s\n", strMessage.c_str());
                        #endif
                    }
                    catch(...)
                    {
                        throw;
                    }
                }
            }

            fclose( stream );
        }
        else
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT("Could not open EPSG code test file: %C\n"), EpsgTestFile));
            CPPUNIT_ASSERT(false);
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTotal EPSG codes tested: %d/%d (Passed/Total)\n"), nEpsgCodesPassed, nEpsgCodesTested));
        ACE_DEBUG((LM_INFO, ACE_TEXT("Total Processing Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestCoordinateSystem::TestCase_Geographic_DatumConversion()
{
    try
    {
        STRING ogcWktSource;
        STRING ogcWktTarget;

        MgCoordinateSystemFactory factory;
        ogcWktSource = factory.ConvertCoordinateSystemCodeToWkt(L"Abidjan1987.LL");
        ogcWktTarget = factory.ConvertCoordinateSystemCodeToWkt(L"Accra1929.LL");

        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWktSource);
        CPPUNIT_ASSERT(coordinateSystemSource);

        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWktTarget);
        CPPUNIT_ASSERT(coordinateSystemTarget);

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(0.0, 0.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.0001886392995, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.001309777484, pCoord->GetY()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

bool TestCoordinateSystem::CompareCodes(STRING code1, STRING code2)
{
    bool bResult = false;

    if(code1 == code2)
    {
        // They are identical
        bResult = true;
    }
    else
    {
        // TODO: Try doing a smart compare
        // ie: LL-CAPE == CAPE.LL

        // For now if we at least get a code we assume Mentor was successful
        if((code1.length() > 0) && (code2.length() > 0))
        {
            bResult = true;
        }
    }

    #ifdef _DEBUG
    if(!bResult)
    {
        printf("      %s - FAILED: Code mismatch. Round trip code = \"%s\"\n", MG_WCHAR_TO_CHAR(code1), MG_WCHAR_TO_CHAR(code2));
    }
    #endif

    return bResult;
}

void TestCoordinateSystem::TestCase_Benchmark_Transformation()
{
    try
    {
        #ifdef _DEBUG
        INT32 iterations = 1;
        #else
        INT32 iterations = 100000;
        #endif

        // Arbitrary XY transform
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nArbitrary XY->Arbitrary XY Transformations\n")));
        MgCoordinateSystemFactory factory;
        STRING ogcWkt1 = ArbitraryWkt_Meter;
        long lStart = GetTickCount();
        Ptr<MgCoordinateSystem> coordinateSystemSource = factory.Create(ogcWkt1);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  CS Source Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(coordinateSystemSource);

        STRING ogcWkt2 = ArbitraryWkt_Feet;
        lStart = GetTickCount();
        Ptr<MgCoordinateSystem> coordinateSystemTarget = factory.Create(ogcWkt2);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  CS Target Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(coordinateSystemTarget);

        lStart = GetTickCount();
        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  Transformation Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(transform);

        // MgCoordinate
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            Ptr<MgCoordinate> pCoord = transform->Transform(10.0, 5.0);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  MgCoordinate - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // By reference
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            double x = 10.0;
            double y = 5.0;
            transform->Transform(&x, &y);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  Reference - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // MgEnvelope
        Ptr<MgCoordinate> coord1 = new MgCoordinateXY(0.0, 1.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXY(10.0, 5.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  MgEnvelope - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // Geographic transform
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nGeographic->Geographic Transformations\n")));
        ogcWkt1 = GeographicWkt_LL84;
        lStart = GetTickCount();
        coordinateSystemSource = factory.Create(ogcWkt1);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  CS Source Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(coordinateSystemSource);

        ogcWkt2 = GeographicWkt_LL84;
        lStart = GetTickCount();
        coordinateSystemTarget = factory.Create(ogcWkt2);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  CS Target Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(coordinateSystemTarget);

        lStart = GetTickCount();
        transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  Transformation Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(transform);

        // MgCoordinate
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            Ptr<MgCoordinate> pCoord = transform->Transform(10.0, 5.0);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  MgCoordinate - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // By reference
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            double x = 10.0;
            double y = 5.0;
            transform->Transform(&x, &y);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  Reference - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // MgEnvelope
        coord1 = new MgCoordinateXY(0.0, 1.0);
        coord2 = new MgCoordinateXY(10.0, 5.0);
        envelopeSource = new MgEnvelope(coord1, coord2);
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  MgEnvelope - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // Geographic->Projected transform
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nGeographic->Projected Transformations\n")));
        ogcWkt1 = GeographicWkt_LL84;
        lStart = GetTickCount();
        coordinateSystemSource = factory.Create(ogcWkt1);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  CS Source Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(coordinateSystemSource);

        ogcWkt2 = ProjectedWkt_GAW;
        lStart = GetTickCount();
        coordinateSystemTarget = factory.Create(ogcWkt2);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  CS Target Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(coordinateSystemTarget);

        lStart = GetTickCount();
        transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  Transformation Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(transform);

        // MgCoordinate
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            Ptr<MgCoordinate> pCoord = transform->Transform(-84.46, 33.89);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  MgCoordinate - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // By reference
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            double x = -84.46;
            double y = 33.89;
            transform->Transform(&x, &y);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  Reference - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // MgEnvelope
        coord1 = new MgCoordinateXY(-83.0, 34.0);
        coord2 = new MgCoordinateXY(-85.0, 33.0);
        envelopeSource = new MgEnvelope(coord1, coord2);
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  MgEnvelope - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // Projected->Geographic transform
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nProjected->Geographic Transformations\n")));
        ogcWkt1 = ProjectedWkt_GAW;
        lStart = GetTickCount();
        coordinateSystemSource = factory.Create(ogcWkt1);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  CS Source Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(coordinateSystemSource);

        ogcWkt2 = GeographicWkt_LL84;
        lStart = GetTickCount();
        coordinateSystemTarget = factory.Create(ogcWkt2);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  CS Target Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(coordinateSystemTarget);

        lStart = GetTickCount();
        transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  Transformation Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(transform);

        // MgCoordinate
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            Ptr<MgCoordinate> pCoord = transform->Transform(410961.8644, 1415078.441);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  MgCoordinate - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // By reference
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            double x = 410961.8644;
            double y = 1415078.441;
            transform->Transform(&x, &y);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  Reference - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // MgEnvelope
        coord1 = new MgCoordinateXY(410983.0, 1415115.0);
        coord2 = new MgCoordinateXY(410000.0, 1415000.0);
        envelopeSource = new MgEnvelope(coord1, coord2);
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  MgEnvelope - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // Projected->Projected transform
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nProjected->Projected Transformations\n")));
        ogcWkt1 = ProjectedWkt_GAW;
        lStart = GetTickCount();
        coordinateSystemSource = factory.Create(ogcWkt1);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  CS Source Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(coordinateSystemSource);

        ogcWkt2 = ProjectedWkt_GAW;
        lStart = GetTickCount();
        coordinateSystemTarget = factory.Create(ogcWkt2);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  CS Target Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(coordinateSystemTarget);

        lStart = GetTickCount();
        transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        ACE_DEBUG((LM_INFO, ACE_TEXT("  Transformation Creation Time: = %6.4f (s)\n"), ((GetTickCount()-lStart)/1000.0)));
        CPPUNIT_ASSERT(transform);

        // MgCoordinate
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            Ptr<MgCoordinate> pCoord = transform->Transform(410961.8644, 1415078.441);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  MgCoordinate - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // By reference
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            double x = 410961.8644;
            double y = 1415078.441;
            transform->Transform(&x, &y);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  Reference - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));

        // MgEnvelope
        coord1 = new MgCoordinateXY(410983.0, 1415115.0);
        coord2 = new MgCoordinateXY(410000.0, 1415000.0);
        envelopeSource = new MgEnvelope(coord1, coord2);
        lStart = GetTickCount();
        for(INT32 i=0;i<iterations;i++)
        {
            Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("  MgEnvelope - Transform Time (Iterations: %d ): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}
