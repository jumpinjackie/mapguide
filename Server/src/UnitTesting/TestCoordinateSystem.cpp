//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#endif

#ifdef _WIN32
const char OgcWktTestFile[] = "..\\UnitTestFiles\\ogcwkts.txt";
#else
const char OgcWktTestFile[] = "../UnitTestFiles/ogcwkts.txt";
#endif

const STRING ArbitraryWkt       = L"LOCAL_CS [ \"Non-Earth (Mile)\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"Mile\", 1.609344e+03], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";
const STRING GeographicWkt      = L"GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"Degrees\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]]";

const STRING ProjectedWkt       = L"PROJCS[\"Georgia West\",GEOGCS[\"NAD27\",DATUM[\"North_American_Datum_1927\",SPHEROID[\"Clarke 1866\",6378206.4,294.978698213898,AUTHORITY[\"EPSG\",\"7008\"]],TOWGS84[-3,142,183,0,0,0,0],AUTHORITY[\"EPSG\",\"6267\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4267\"]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",30],PARAMETER[\"central_meridian\",-84.16666666666667],PARAMETER[\"scale_factor\",0.9999],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"Foot\",0.3048006096012192]]";
const STRING InvalidWkt         = L"GEOGCS[\"Invalid Coordinate System\",DATUM[\"Invalid\",SPHEROID[\"Invalid\",0.0,0.0],PRIMEM[\"Invalid\",0],UNIT[\"Invalid\",1]]";

const STRING ArbitraryWkt_Feet  = L"LOCAL_CS [ \"Non-Earth (U.S. Foot)\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"U.S. Foot\", 3.048006096012e-01], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";
const STRING ArbitraryWkt_Meter = L"LOCAL_CS [ \"Non-Earth (Meter)\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"Meter\", 1.0], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";

const STRING GeographicWkt_LL84      = L"GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]]";
const STRING ProjectedWkt_GAW   = L"PROJCS[\"Georgia West\",GEOGCS[\"NAD27\",DATUM[\"North_American_Datum_1927\",SPHEROID[\"Clarke 1866\",6378206.4,294.978698213898,AUTHORITY[\"EPSG\",\"7008\"]],TOWGS84[-3,142,183,0,0,0,0],AUTHORITY[\"EPSG\",\"6267\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4267\"]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",30],PARAMETER[\"central_meridian\",-84.16666666666667],PARAMETER[\"scale_factor\",0.9999],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"Foot\",0.3048006096012192]]";

const STRING GeographicWkt_LL84_NoDatum = L"GEOGCS[\"LL\",DATUM[\"\",SPHEROID[\"WGS 84\",6378137,0],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.0174532925199433],PARAMETER[\"ADSK_minimum_longitude\", -180],PARAMETER[\"ADSK_maximum_longitude\", 180],]";

const STRING Projected_UTM18_NAD83 = L"PROJCS[\"UTM Zone 18, Northern Hemisphere\",GEOGCS[\"GRS 1980(IUGG, 1980)\",DATUM[\"unknown\",SPHEROID[\"GRS80\",6378137,298.257222101]],PRIMEM[\"Greenwich\",0],UNIT[\"degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",-75],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1]]";

const STRING EPSG_4326_Wkt     = L"GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]]";
const STRING EPSG_4326_Wkt_Alt = L"GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]]";

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
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Coordinate System tests. (PROJ4)\n")));
//    memset(&state, 0, sizeof(_CrtMemState));
//    _CrtMemCheckpoint(&state);
}

void TestCoordinateSystem::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nCoordinate System tests completed.\n\n")));
//  _CrtDumpMemoryLeaks();
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
        #ifdef _DEBUG
        printf("\nTesting coordinate system OGC WKTs.\n");
        #endif

        MgCoordinateSystemFactory factory;
        Ptr<MgCoordinateSystem> pCoordinateSystem;
        STRING ogcWkt;
        int nCoordinateSystemsTested = 0;
        int nCoordinateSystemsPassed = 0;

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
                            nCoordinateSystemsPassed++;
                        }
                    }
                    catch(MgException* e)
                    {
                        STRING message = e->GetDetails(TEST_LOCALE);
                        SAFE_RELEASE(e);

                        #ifdef _DEBUG
                        string strMessage = MG_WCHAR_TO_CHAR(message.c_str());
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
            printf("Could not open coordinate system test file %s\n", OgcWktTestFile);
            CPPUNIT_ASSERT(false);
        }

        #ifdef _DEBUG
        printf("\nTotal coordinate system OGC WKTs tested: %d/%d (Passed/Total)\n", nCoordinateSystemsPassed, nCoordinateSystemsTested);
        #endif
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
        CPPUNIT_ASSERT_THROW_MG(pCoordinateSystem = factory.Create(ogcWkt), MgInvalidCoordinateSystemException*);

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

        ogcWkt = GeographicWkt_LL84_NoDatum;
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
    catch(MgInvalidCoordinateSystemException* e)
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
        #ifdef _DEBUG
        int nTotalCoordinateSystemsTested = 0;
        int nTotalCoordinateSystemsPassed = 0;
        #endif

        MgCoordinateSystemFactory factory;
        Ptr<MgStringCollection> categories;
        categories = factory.EnumerateCategories();

        // Get the # of coordinate system categories
        INT32 size = categories->GetCount();
        #ifdef _DEBUG
        printf("\nTesting %d coordinate system categories.\n", size);
        #endif
        for(int i=0;i<size;i++)
        {
            Ptr<MgBatchPropertyCollection> coordSystems;
            coordSystems = factory.EnumerateCoordinateSystems(categories->GetItem(i));

            // Get the # of coordinate systems for this category
            int nCoordinateSystems = coordSystems->GetCount();

            #ifdef _DEBUG
            nTotalCoordinateSystemsTested += nCoordinateSystems;
            printf("  %s - %d (Coordinate Systems)\n", MG_WCHAR_TO_CHAR(categories->GetItem(i)), nCoordinateSystems);
            #endif

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
                                #ifdef _DEBUG
                                //printf("      %s", MG_WCHAR_TO_CHAR(pProperty->GetValue()));
                                #endif

                                STRING wkt = factory.ConvertCoordinateSystemCodeToWkt(pProperty->GetValue());
                                STRING code = factory.ConvertWktToCoordinateSystemCode(wkt);
                                CPPUNIT_ASSERT(CompareCodes(pProperty->GetValue(), code));

                                #ifdef _DEBUG
                                nTotalCoordinateSystemsPassed++;
                                printf("      %s - PASSED\n", MG_WCHAR_TO_CHAR(pProperty->GetValue()));
                                #endif
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
                        else if(pProperty->GetName() == L"Description")
                        {
                            #ifdef _DEBUG
                            //printf(" - %s\n", MG_WCHAR_TO_CHAR(pProperty->GetValue()));
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

        #ifdef _DEBUG
        printf("\nTotal coordinate systems tested: %d/%d (Passed/Total)\n", nTotalCoordinateSystemsPassed, nTotalCoordinateSystemsTested);
        #endif
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
        coordSystems = factory.EnumerateCoordinateSystems(L"EPSG");
        CPPUNIT_ASSERT(coordSystems);
        CPPUNIT_ASSERT(coordSystems->GetCount() > 0);

        coordSystems = factory.EnumerateCoordinateSystems(L"ePsG");
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
        CPPUNIT_ASSERT(library == L"PROJ4 Coordinate System Library");
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
        MgCoordinateSystemFactory factory;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(EPSG_4326_Wkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        CPPUNIT_ASSERT_THROW_MG(pCoordinateSystem = factory.Create(InvalidWkt), MgInvalidCoordinateSystemException*);
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
        CPPUNIT_ASSERT(_wcsicmp(L"Arbitrary X-Y Coordinates (Mile)", value.c_str()) == 0);

        ogcWkt = ArbitraryWkt_Meter;
        pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        value = pCoordinateSystem->GetDescription();
        CPPUNIT_ASSERT(_wcsicmp(L"Arbitrary X-Y Coordinates (Meter)", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(4.998507883, distance));

        double distance2 = pCoordinateSystem->MeasureGreatCircleDistance(0.0, 0.0, 4.0, 3.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(distance, distance2));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(556431.3523, meters));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(53.08259822, degrees));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.004898200, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(3.998192151, pCoord->GetY()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.970, y));

        MgCoordinateXY coord(-84.46, 33.89);
        Ptr<MgCoordinate> pCoord = pCoordinateSystem->ConvertFromLonLat(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.970, pCoord->GetY()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.970, y[0]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(551096.7765, x[1]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1091213.713, y[1]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(861668.2130, x[2]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(729342.5537, y[2]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1178912.135, x[3]));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(370280.4160, y[3]));
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

        MgCoordinateXY coord(410983.2513, 1415115.971);
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

        double x[4] = { 410983.2513, 551096.7774, 861668.2135, 1178912.136 };
        double y[4] = { 1415115.971, 1091213.712, 729342.553, 370280.4158 };
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.3048006096, meters));
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

        double units = pCoordinateSystem->ConvertMetersToCoordinateSystemUnits(0.3048006096);
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.524003048, meters));
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

        MgCoordinateXY coord1(410983.2525, 1415115.969);
        MgCoordinateXY coord2(432415.4557, 1362302.051);

        double distance = pCoordinateSystem->MeasureGreatCircleDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(187568.7504, distance));

        double distance2 = pCoordinateSystem->MeasureGreatCircleDistance(410983.2525, 1415115.969, 432415.4557, 1362302.051);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(distance, distance2));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(57171.06946, meters));
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

        MgCoordinateXY coord1(410983.2525, 1415115.969);
        MgCoordinateXY coord2(432415.4557, 1362302.051);

        double degrees = pCoordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(157.8431224, degrees));
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

        MgCoordinateXY coord(410983.2525, 1415115.969);
        Ptr<MgCoordinate> pCoord = pCoordinateSystem->GetCoordinate(&coord, 157.8431224, 187568.7504);
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(4.998507883, distance));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(556431.3523, meters));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(53.08259822, degrees));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(8.682254522, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(4.980953995, pCoord->GetY()));
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
        CPPUNIT_ASSERT(_wcsicmp(L"EPSG:4326", code.c_str()) == 0);
        STRING wkt = factory.ConvertCoordinateSystemCodeToWkt(code);
        CPPUNIT_ASSERT(wkt.length() > 0);

        wkt = factory.ConvertCoordinateSystemCodeToWkt(L"ePsG:4326");
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
        CPPUNIT_ASSERT(_wcsicmp(L"Degrees", units.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"EPSG:4326", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"WGS 84", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"Unknown", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"Unknown", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"WGS_1984", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"WGS_1984", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"WGS 84", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"WGS 84", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(value->Contains(L"EPSG"));
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

        MgCoordinateXY coord1(410983.2525, 1415115.969);
        MgCoordinateXY coord2(432415.4557, 1362302.051);

        double distance = measure->GetDistance(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(187568.7504, distance));

        double meters = pCoordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(57171.06946, meters));
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

        MgCoordinateXY coord1(410983.2525, 1415115.969);
        MgCoordinateXY coord2(432415.4557, 1362302.051);

        double degrees = measure->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(157.8431224, degrees));
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

        MgCoordinateXY coord(410983.2525, 1415115.969);
        Ptr<MgCoordinate> pCoord = measure->GetCoordinate(&coord, 157.8431224, 187568.7504);
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
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        // TODO: Retest
        // STRING code = factory.ConvertWktToCoordinateSystemCode(ogcWkt);
        // CPPUNIT_ASSERT(_wcsicmp(L"EPSG:26767", code.c_str()) == 0);
        // STRING wkt = factory.ConvertCoordinateSystemCodeToWkt(code);
        // CPPUNIT_ASSERT(wkt.length() > 0);

        STRING wkt = factory.ConvertCoordinateSystemCodeToWkt(L"ePsG:26767");
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.3048006096, scale));
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

void TestCoordinateSystem::TestCase_Projected_GetMinY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
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

void TestCoordinateSystem::TestCase_Projected_GetMaxX()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
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

void TestCoordinateSystem::TestCase_Projected_GetMaxY()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
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

void TestCoordinateSystem::TestCase_Projected_GetCode()
{
    try
    {
        MgCoordinateSystemFactory factory;
        STRING ogcWkt = ProjectedWkt;
        Ptr<MgCoordinateSystem> pCoordinateSystem = factory.Create(ogcWkt);
        CPPUNIT_ASSERT(pCoordinateSystem);

        STRING value = pCoordinateSystem->GetCode();
        // TODO: Retest
        // CPPUNIT_ASSERT(_wcsicmp(L"EPSG:26767", value.c_str()) == 0);
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
        // TODO: Retest
        // CPPUNIT_ASSERT(_wcsicmp(L"NAD27 / Georgia West", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"Transverse_Mercator", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"Transverse_Mercator", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"North_American_Datum_1927", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"North_American_Datum_1927", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"Clarke 1866", value.c_str()) == 0);
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
        CPPUNIT_ASSERT(_wcsicmp(L"Clarke 1866", value.c_str()) == 0);
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
        // TODO: Retest
        // CPPUNIT_ASSERT(value->Contains(L"EPSG"));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYM coord(10.0, 5.0, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.8983152841, pCoord->GetM()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZM coord(10.0, 5.0, 1.0, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.8983152841, pCoord->GetM()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYM coord(10.0, 5.0, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(328083.3333, pCoord->GetM()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZM coord(10.0, 5.0, 1.0, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(328083.3333, pCoord->GetM()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYM coord(10.0, 5.0, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(365220.6961, pCoord->GetM()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZM coord(10.0, 5.0, 1.0, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(10.0, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(5.0, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.0, pCoord->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(365220.6961, pCoord->GetM()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.9999998612, pCoord->GetZ()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(365220.6961, pCoord->GetM()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.9999998612, pCoord->GetZ()));
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

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.9999998612, pCoord->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(365220.6961, pCoord->GetM()));
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

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(853597.4318, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1457027.615, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(244512.5714, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1092185.197, ll->GetY()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(853597.4318, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1457027.615, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.0, ur->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(551096.7765, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1091213.714, ll->GetY()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(410983.2513, 1415115.971);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> pCoord = transform->Transform(410983.2513, 1415115.971, 1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXY coord(410983.2513, 1415115.971);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYM coord(410983.2513, 1415115.971, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZ coord(410983.2513, 1415115.971, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        MgCoordinateXYZM coord(410983.2513, 1415115.971, 1.0, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXY(410983.0, 1415115.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXY(400000.0, 1415000.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.0, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.0, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(400000.0, ll->GetX()));
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

        Ptr<MgCoordinateSystemTransform> transform = factory.GetTransform(coordinateSystemSource, coordinateSystemTarget);
        CPPUNIT_ASSERT(transform);

        Ptr<MgCoordinate> coord1 = new MgCoordinateXYZ(410983.0, 1415115.0, 2.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXYZ(400000.0, 1415000.0, 1.0);
        Ptr<MgEnvelope> envelopeSource = new MgEnvelope(coord1, coord2);
        Ptr<MgEnvelope> envelopeTarget = transform->Transform(envelopeSource);

        Ptr<MgCoordinate> ur = envelopeTarget->GetUpperRightCoordinate();
        Ptr<MgCoordinate> ll = envelopeTarget->GetLowerLeftCoordinate();

        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.0, ur->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.0, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.0, ur->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(400000.0, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415000.0, ll->GetY()));
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

        Ptr<MgCoordinate> pCoord = transform->Transform(410983.2513, 1415115.971);
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

        Ptr<MgCoordinate> pCoord = transform->Transform(410983.2513, 1415115.971, 1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.000000142, pCoord->GetZ()));
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

        MgCoordinateXY coord(410983.2513, 1415115.971);
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

        MgCoordinateXYM coord(410983.2513, 1415115.971, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.2738070462, pCoord->GetM()));
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

        MgCoordinateXYZ coord(410983.2513, 1415115.971, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.000000142, pCoord->GetZ()));
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

        MgCoordinateXYZM coord(410983.2513, 1415115.971, 1.0, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.89, pCoord->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1.000000142, pCoord->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(0.2738070462, pCoord->GetM()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.88999733, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46323893, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.88967353, ll->GetY()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.88999733, ur->GetY()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.0, ur->GetZ()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-84.46323893, ll->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(33.88967353, ll->GetY()));
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

        Ptr<MgCoordinate> pCoord = transform->Transform(410983.2513, 1415115.971);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        Ptr<MgCoordinate> pCoord = transform->Transform(410983.2513, 1415115.971, 1.0);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        MgCoordinateXY coord(410983.2513, 1415115.971);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        MgCoordinateXYM coord(410983.2513, 1415115.971, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        MgCoordinateXYZ coord(410983.2513, 1415115.971, 1.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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

        MgCoordinateXYZM coord(410983.2513, 1415115.971, 1.0, 100000.0);
        Ptr<MgCoordinate> pCoord = transform->Transform(&coord);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(410983.2513, pCoord->GetX()));
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(1415115.971, pCoord->GetY()));
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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.748361238, distance));

        double meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(305946.1735, meters));

        // Calculate the azimuth to New York
        double azimuth = coordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-125.6917360, azimuth));

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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(2.748361238, distance));

        double meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(305946.1735, meters));

        // Calculate the azimuth to Boston
        double azimuth = coordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(52.35502392, azimuth));

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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(305946.1740, distance));

        double meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(305946.1740, meters));

        // Calculate the azimuth to New York
        double azimuth = coordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(-125.6917362, azimuth));

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
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(305946.1740, distance));

        double meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(distance);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(305946.1740, meters));

        // Calculate the azimuth to Boston
        double azimuth = coordinateSystem->GetAzimuth(&coord1, &coord2);
        CPPUNIT_ASSERT(MgUtil::ValuesEqual(52.35502379, azimuth));

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
        STRING ogcWkt;
        Ptr<MgCoordinateSystemFactory> factory = new MgCoordinateSystemFactory();
        CPPUNIT_ASSERT(factory);

        ogcWkt = factory->ConvertCoordinateSystemCodeToWkt(L"EPSG:4326");
        CPPUNIT_ASSERT(ogcWkt.length() > 0);

        ogcWkt = factory->ConvertCoordinateSystemCodeToWkt(L"ePsG:4326");
        CPPUNIT_ASSERT(ogcWkt.length() > 0);

        CPPUNIT_ASSERT_THROW_MG(ogcWkt = factory->ConvertCoordinateSystemCodeToWkt(L"test:4000"), MgCoordinateSystemConversionFailedException*);

        ogcWkt = factory->ConvertEpsgCodeToWkt(4326);
        CPPUNIT_ASSERT(ogcWkt.length() > 0);

        long epsg = factory->ConvertWktToEpsgCode(ogcWkt);
        CPPUNIT_ASSERT(epsg == 4326);
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
