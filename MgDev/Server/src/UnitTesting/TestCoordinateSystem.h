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

#ifndef TESTCOORDINATESYSTEM_H_
#define TESTCOORDINATESYSTEM_H_

#include <cppunit/extensions/HelperMacros.h>

// Comment out the line below to disable the slow test cases
#define ENABLE_SLOW_TESTS 1

class TestCoordinateSystem : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCoordinateSystem);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_ReadAllCoordinateSystems);
    CPPUNIT_TEST(TestCase_ReadAllDatums);
    CPPUNIT_TEST(TestCase_ReadAllEllipsoids);
    CPPUNIT_TEST(TestCase_ReadAllCategories);
    CPPUNIT_TEST(TestCase_ReadAllGeodeticTransformations);
    CPPUNIT_TEST(TestCase_ReadAllGeodeticPaths);

    CPPUNIT_TEST(TestCase_UpdateCoordinateSystems);
    CPPUNIT_TEST(TestCase_UpdateDatums);
    CPPUNIT_TEST(TestCase_UpdateEllipsoids);
    CPPUNIT_TEST(TestCase_UpdateCategories);
    CPPUNIT_TEST(TestCase_UpdateGeodeticTransformations);
    CPPUNIT_TEST(TestCase_UpdateGeodeticPaths);

    CPPUNIT_TEST(TestCase_InitializeInvalidUserDictionaryDir);
    CPPUNIT_TEST(TestCase_InitializeValidUserDictionaryDir);

    CPPUNIT_TEST(TestCase_UpdateUserCoordinateSystems);
    CPPUNIT_TEST(TestCase_UpdateUserDatums);
    CPPUNIT_TEST(TestCase_UpdateUserEllipsoids);
    CPPUNIT_TEST(TestCase_UpdateUserCategories);
    CPPUNIT_TEST(TestCase_UpdateUserGeodeticTransformations);
    CPPUNIT_TEST(TestCase_UpdateUserGeodeticPaths);

#ifdef ENABLE_SLOW_TESTS
    CPPUNIT_TEST(TestCase_CheckCoordinateSystems);
#endif
    CPPUNIT_TEST(TestCase_CreateValidCoordinateSystem);
    CPPUNIT_TEST(TestCase_CreateInvalidCoordinateSystem);
#ifdef ENABLE_SLOW_TESTS
    CPPUNIT_TEST(TestCase_EnumerateCategories);
#endif
    CPPUNIT_TEST(TestCase_EnumerateCoordSys);
    CPPUNIT_TEST(TestCase_GetBaseLibrary);
    CPPUNIT_TEST(TestCase_IsValid);

    CPPUNIT_TEST(TestCase_ValidateCoordinateSystemArbitrary);
    CPPUNIT_TEST(TestCase_Arbitrary_ConvertFromLonLat);
    CPPUNIT_TEST(TestCase_Arbitrary_ConvertFromLonLatArray);
    CPPUNIT_TEST(TestCase_Arbitrary_ConvertToLonLat);
    CPPUNIT_TEST(TestCase_Arbitrary_ConvertToLonLatArray);
    CPPUNIT_TEST(TestCase_Arbitrary_ConvertCoordinateSystemUnitsToMeters);
    CPPUNIT_TEST(TestCase_Arbitrary_ConvertMetersToCoordinateSystemUnits);
    CPPUNIT_TEST(TestCase_Arbitrary_MeasureEuclideanDistance);
    CPPUNIT_TEST(TestCase_Arbitrary_MeasureGreatCircleDistance);
    CPPUNIT_TEST(TestCase_Arbitrary_GetAzimuth);
    CPPUNIT_TEST(TestCase_Arbitrary_GetCoordinate);
    CPPUNIT_TEST(TestCase_Arbitrary_ConvertCode);
    CPPUNIT_TEST(TestCase_Arbitrary_GetUnits);
    CPPUNIT_TEST(TestCase_Arbitrary_GetUnitScale);
    CPPUNIT_TEST(TestCase_Arbitrary_GetMinX);
    CPPUNIT_TEST(TestCase_Arbitrary_GetMinY);
    CPPUNIT_TEST(TestCase_Arbitrary_GetMaxX);
    CPPUNIT_TEST(TestCase_Arbitrary_GetMaxY);
    CPPUNIT_TEST(TestCase_Arbitrary_GetCsCode);
    CPPUNIT_TEST(TestCase_Arbitrary_GetDescription);
    CPPUNIT_TEST(TestCase_Arbitrary_GetProjection);
    CPPUNIT_TEST(TestCase_Arbitrary_GetProjectionDescription);
    CPPUNIT_TEST(TestCase_Arbitrary_GetDatum);
    CPPUNIT_TEST(TestCase_Arbitrary_GetDatumDescription);
    CPPUNIT_TEST(TestCase_Arbitrary_GetEllipsoid);
    CPPUNIT_TEST(TestCase_Arbitrary_GetEllipsoidDescription);
    CPPUNIT_TEST(TestCase_Arbitrary_GetCategory);

    CPPUNIT_TEST(TestCase_ValidateCoordinateSystemGeographic);
    CPPUNIT_TEST(TestCase_Geographic_ConvertFromLonLat);
    CPPUNIT_TEST(TestCase_Geographic_ConvertFromLonLatArray);
    CPPUNIT_TEST(TestCase_Geographic_ConvertToLonLat);
    CPPUNIT_TEST(TestCase_Geographic_ConvertToLonLatArray);
    CPPUNIT_TEST(TestCase_Geographic_ConvertCoordinateSystemUnitsToMeters);
    CPPUNIT_TEST(TestCase_Geographic_ConvertMetersToCoordinateSystemUnits);
    CPPUNIT_TEST(TestCase_Geographic_MeasureEuclideanDistance);
    CPPUNIT_TEST(TestCase_Geographic_MeasureGreatCircleDistance);
    CPPUNIT_TEST(TestCase_Geographic_GetAzimuth);
    CPPUNIT_TEST(TestCase_Geographic_GetCoordinate);
    CPPUNIT_TEST(TestCase_Geographic_ConvertCode);
    CPPUNIT_TEST(TestCase_Geographic_GetUnits);
    CPPUNIT_TEST(TestCase_Geographic_GetUnitScale);
    //CPPUNIT_TEST(TestCase_Geographic_GetMinX);
    CPPUNIT_TEST(TestCase_Geographic_GetMinY);
    //CPPUNIT_TEST(TestCase_Geographic_GetMaxX);
    CPPUNIT_TEST(TestCase_Geographic_GetMaxY);
    CPPUNIT_TEST(TestCase_Geographic_GetCsCode);
    CPPUNIT_TEST(TestCase_Geographic_GetDescription);
    CPPUNIT_TEST(TestCase_Geographic_GetProjection);
    CPPUNIT_TEST(TestCase_Geographic_GetProjectionDescription);
    CPPUNIT_TEST(TestCase_Geographic_GetDatum);
    CPPUNIT_TEST(TestCase_Geographic_GetDatumDescription);
    CPPUNIT_TEST(TestCase_Geographic_GetEllipsoid);
    CPPUNIT_TEST(TestCase_Geographic_GetEllipsoidDescription);
    CPPUNIT_TEST(TestCase_Geographic_GetCategory);

    CPPUNIT_TEST(TestCase_ValidateCoordinateSystemProjected);
    CPPUNIT_TEST(TestCase_Projected_ConvertFromLonLat);
    CPPUNIT_TEST(TestCase_Projected_ConvertFromLonLatArray);
    CPPUNIT_TEST(TestCase_Projected_ConvertToLonLat);
    CPPUNIT_TEST(TestCase_Projected_ConvertToLonLatArray);
    CPPUNIT_TEST(TestCase_Projected_ConvertCoordinateSystemUnitsToMeters);
    CPPUNIT_TEST(TestCase_Projected_ConvertMetersToCoordinateSystemUnits);
    CPPUNIT_TEST(TestCase_Projected_MeasureEuclideanDistance);
    CPPUNIT_TEST(TestCase_Projected_MeasureGreatCircleDistance);
    CPPUNIT_TEST(TestCase_Projected_GetAzimuth);
    CPPUNIT_TEST(TestCase_Projected_GetCoordinate);
    CPPUNIT_TEST(TestCase_Projected_ConvertCode);
    CPPUNIT_TEST(TestCase_Projected_GetUnits);
    CPPUNIT_TEST(TestCase_Projected_GetUnitScale);
    //CPPUNIT_TEST(TestCase_Projected_GetMinX);
    //CPPUNIT_TEST(TestCase_Projected_GetMinY);
    //CPPUNIT_TEST(TestCase_Projected_GetMaxX);
    //CPPUNIT_TEST(TestCase_Projected_GetMaxY);
    CPPUNIT_TEST(TestCase_Projected_GetCsCode);
    CPPUNIT_TEST(TestCase_Projected_GetDescription);
    CPPUNIT_TEST(TestCase_Projected_GetProjection);
    CPPUNIT_TEST(TestCase_Projected_GetProjectionDescription);
    CPPUNIT_TEST(TestCase_Projected_GetDatum);
    CPPUNIT_TEST(TestCase_Projected_GetDatumDescription);
    CPPUNIT_TEST(TestCase_Projected_GetEllipsoid);
    CPPUNIT_TEST(TestCase_Projected_GetEllipsoidDescription);
    CPPUNIT_TEST(TestCase_Projected_GetCategory);

    CPPUNIT_TEST(TestCase_Arbitrary_Measure_GetDistance);
    CPPUNIT_TEST(TestCase_Arbitrary_Measure_GetAzimuth);
    CPPUNIT_TEST(TestCase_Arbitrary_Measure_GetCoordinate);
    CPPUNIT_TEST(TestCase_Geographic_Measure_GetDistance);
    CPPUNIT_TEST(TestCase_Geographic_Measure_GetAzimuth);
    CPPUNIT_TEST(TestCase_Geographic_Measure_GetCoordinate);
    CPPUNIT_TEST(TestCase_Projected_Measure_GetDistance);
    CPPUNIT_TEST(TestCase_Projected_Measure_GetAzimuth);
    CPPUNIT_TEST(TestCase_Projected_Measure_GetCoordinate);

    // Arbitrary
    CPPUNIT_TEST(TestCase_Arbitrary_To_Arbitrary_Transform_XY);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Arbitrary_Transform_XYZ);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXY);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXYM);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXYZ);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXYZM);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Arbitrary_Transform_EnvelopeXY);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Arbitrary_Transform_EnvelopeXYZ);

    CPPUNIT_TEST(TestCase_Arbitrary_To_Geographic_Transform_XY);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Geographic_Transform_XYZ);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Geographic_Transform_CoordinateXY);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Geographic_Transform_CoordinateXYM);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Geographic_Transform_CoordinateXYZ);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Geographic_Transform_CoordinateXYZM);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Geographic_Transform_EnvelopeXY);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Geographic_Transform_EnvelopeXYZ);

    CPPUNIT_TEST(TestCase_Arbitrary_To_Projected_Transform_XY);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Projected_Transform_XYZ);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Projected_Transform_CoordinateXY);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Projected_Transform_CoordinateXYM);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Projected_Transform_CoordinateXYZ);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Projected_Transform_CoordinateXYZM);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Projected_Transform_EnvelopeXY);
    CPPUNIT_TEST(TestCase_Arbitrary_To_Projected_Transform_EnvelopeXYZ);

    // Geographic
    CPPUNIT_TEST(TestCase_Geographic_To_Arbitrary_Transform_XY);
    CPPUNIT_TEST(TestCase_Geographic_To_Arbitrary_Transform_XYZ);
    CPPUNIT_TEST(TestCase_Geographic_To_Arbitrary_Transform_CoordinateXY);
    CPPUNIT_TEST(TestCase_Geographic_To_Arbitrary_Transform_CoordinateXYM);
    CPPUNIT_TEST(TestCase_Geographic_To_Arbitrary_Transform_CoordinateXYZ);
    CPPUNIT_TEST(TestCase_Geographic_To_Arbitrary_Transform_CoordinateXYZM);
    CPPUNIT_TEST(TestCase_Geographic_To_Arbitrary_Transform_EnvelopeXY);
    CPPUNIT_TEST(TestCase_Geographic_To_Arbitrary_Transform_EnvelopeXYZ);

    CPPUNIT_TEST(TestCase_Geographic_To_Geographic_Transform_XY);
    CPPUNIT_TEST(TestCase_Geographic_To_Geographic_Transform_XYZ);
    CPPUNIT_TEST(TestCase_Geographic_To_Geographic_Transform_CoordinateXY);
    CPPUNIT_TEST(TestCase_Geographic_To_Geographic_Transform_CoordinateXYM);
    CPPUNIT_TEST(TestCase_Geographic_To_Geographic_Transform_CoordinateXYZ);
    CPPUNIT_TEST(TestCase_Geographic_To_Geographic_Transform_CoordinateXYZM);
    CPPUNIT_TEST(TestCase_Geographic_To_Geographic_Transform_EnvelopeXY);
    CPPUNIT_TEST(TestCase_Geographic_To_Geographic_Transform_EnvelopeXYZ);

    CPPUNIT_TEST(TestCase_Geographic_To_Projected_Transform_XY);
    CPPUNIT_TEST(TestCase_Geographic_To_Projected_Transform_XYZ);
    CPPUNIT_TEST(TestCase_Geographic_To_Projected_Transform_CoordinateXY);
    CPPUNIT_TEST(TestCase_Geographic_To_Projected_Transform_CoordinateXYM);
    CPPUNIT_TEST(TestCase_Geographic_To_Projected_Transform_CoordinateXYZ);
    CPPUNIT_TEST(TestCase_Geographic_To_Projected_Transform_CoordinateXYZM);
    CPPUNIT_TEST(TestCase_Geographic_To_Projected_Transform_EnvelopeXY);
    CPPUNIT_TEST(TestCase_Geographic_To_Projected_Transform_EnvelopeXYZ);

    // Projected
    CPPUNIT_TEST(TestCase_Projected_To_Arbitrary_Transform_XY);
    CPPUNIT_TEST(TestCase_Projected_To_Arbitrary_Transform_XYZ);
    CPPUNIT_TEST(TestCase_Projected_To_Arbitrary_Transform_CoordinateXY);
    CPPUNIT_TEST(TestCase_Projected_To_Arbitrary_Transform_CoordinateXYM);
    CPPUNIT_TEST(TestCase_Projected_To_Arbitrary_Transform_CoordinateXYZ);
    CPPUNIT_TEST(TestCase_Projected_To_Arbitrary_Transform_CoordinateXYZM);
    CPPUNIT_TEST(TestCase_Projected_To_Arbitrary_Transform_EnvelopeXY);
    CPPUNIT_TEST(TestCase_Projected_To_Arbitrary_Transform_EnvelopeXYZ);

    CPPUNIT_TEST(TestCase_Projected_To_Geographic_Transform_XY);
    CPPUNIT_TEST(TestCase_Projected_To_Geographic_Transform_XYZ);
    CPPUNIT_TEST(TestCase_Projected_To_Geographic_Transform_CoordinateXY);
    CPPUNIT_TEST(TestCase_Projected_To_Geographic_Transform_CoordinateXYM);
    CPPUNIT_TEST(TestCase_Projected_To_Geographic_Transform_CoordinateXYZ);
    CPPUNIT_TEST(TestCase_Projected_To_Geographic_Transform_CoordinateXYZM);
    CPPUNIT_TEST(TestCase_Projected_To_Geographic_Transform_EnvelopeXY);
    CPPUNIT_TEST(TestCase_Projected_To_Geographic_Transform_EnvelopeXYZ);

    CPPUNIT_TEST(TestCase_Projected_To_Projected_Transform_XY);
    CPPUNIT_TEST(TestCase_Projected_To_Projected_Transform_XYZ);
    CPPUNIT_TEST(TestCase_Projected_To_Projected_Transform_CoordinateXY);
    CPPUNIT_TEST(TestCase_Projected_To_Projected_Transform_CoordinateXYM);
    CPPUNIT_TEST(TestCase_Projected_To_Projected_Transform_CoordinateXYZ);
    CPPUNIT_TEST(TestCase_Projected_To_Projected_Transform_CoordinateXYZM);
    CPPUNIT_TEST(TestCase_Projected_To_Projected_Transform_EnvelopeXY);
    CPPUNIT_TEST(TestCase_Projected_To_Projected_Transform_EnvelopeXYZ);

    // Datum conversion
    CPPUNIT_TEST(TestCase_Geographic_DatumConversion);

    // Real world locations
    CPPUNIT_TEST(TestCase_Boston_Geographic);
    CPPUNIT_TEST(TestCase_NewYork_Geographic);
    CPPUNIT_TEST(TestCase_Boston_Projected);
    CPPUNIT_TEST(TestCase_NewYork_Projected);

    // EPSG
#ifdef ENABLE_SLOW_TESTS
    CPPUNIT_TEST(TestCase_EPSG);
#endif

    // Performance
#ifdef ENABLE_SLOW_TESTS
    CPPUNIT_TEST(TestCase_Benchmark_Transformation);
#endif

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    //check sanity of the dictionaries
    void TestCase_ReadAllCoordinateSystems();
    void TestCase_ReadAllDatums();
    void TestCase_ReadAllEllipsoids();
    void TestCase_ReadAllCategories();
    void TestCase_ReadAllGeodeticTransformations();
    void TestCase_ReadAllGeodeticPaths();

    void TestCase_UpdateCoordinateSystems();
    void TestCase_UpdateDatums();
    void TestCase_UpdateEllipsoids();
    void TestCase_UpdateCategories();
    void TestCase_UpdateGeodeticTransformations();
    void TestCase_UpdateGeodeticPaths();

    void TestCase_InitializeInvalidUserDictionaryDir();
    void TestCase_InitializeValidUserDictionaryDir();

    void TestCase_UpdateUserCoordinateSystems();
    void TestCase_UpdateUserDatums();
    void TestCase_UpdateUserEllipsoids();
    void TestCase_UpdateUserCategories();
    void TestCase_UpdateUserGeodeticTransformations();
    void TestCase_UpdateUserGeodeticPaths();

    void TestCase_CheckCoordinateSystems();
    void TestCase_CreateValidCoordinateSystem();
    void TestCase_CreateInvalidCoordinateSystem();
    void TestCase_EnumerateCategories();
    void TestCase_EnumerateCoordSys();
    void TestCase_GetBaseLibrary();
    void TestCase_IsValid();

    void TestCase_ValidateCoordinateSystemArbitrary();
    void TestCase_Arbitrary_ConvertFromLonLat();
    void TestCase_Arbitrary_ConvertFromLonLatArray();
    void TestCase_Arbitrary_ConvertToLonLat();
    void TestCase_Arbitrary_ConvertToLonLatArray();
    void TestCase_Arbitrary_ConvertCoordinateSystemUnitsToMeters();
    void TestCase_Arbitrary_ConvertMetersToCoordinateSystemUnits();
    void TestCase_Arbitrary_MeasureEuclideanDistance();
    void TestCase_Arbitrary_MeasureGreatCircleDistance();
    void TestCase_Arbitrary_GetAzimuth();
    void TestCase_Arbitrary_GetCoordinate();
    void TestCase_Arbitrary_ConvertCode();
    void TestCase_Arbitrary_GetUnits();
    void TestCase_Arbitrary_GetUnitScale();
    void TestCase_Arbitrary_GetMinX();
    void TestCase_Arbitrary_GetMinY();
    void TestCase_Arbitrary_GetMaxX();
    void TestCase_Arbitrary_GetMaxY();
    void TestCase_Arbitrary_GetCsCode();
    void TestCase_Arbitrary_GetDescription();
    void TestCase_Arbitrary_GetProjection();
    void TestCase_Arbitrary_GetProjectionDescription();
    void TestCase_Arbitrary_GetDatum();
    void TestCase_Arbitrary_GetDatumDescription();
    void TestCase_Arbitrary_GetEllipsoid();
    void TestCase_Arbitrary_GetEllipsoidDescription();
    void TestCase_Arbitrary_GetCategory();

    void TestCase_ValidateCoordinateSystemGeographic();
    void TestCase_Geographic_ConvertFromLonLat();
    void TestCase_Geographic_ConvertFromLonLatArray();
    void TestCase_Geographic_ConvertToLonLat();
    void TestCase_Geographic_ConvertToLonLatArray();
    void TestCase_Geographic_ConvertCoordinateSystemUnitsToMeters();
    void TestCase_Geographic_ConvertMetersToCoordinateSystemUnits();
    void TestCase_Geographic_MeasureEuclideanDistance();
    void TestCase_Geographic_MeasureGreatCircleDistance();
    void TestCase_Geographic_GetAzimuth();
    void TestCase_Geographic_GetCoordinate();
    void TestCase_Geographic_ConvertCode();
    void TestCase_Geographic_GetUnits();
    void TestCase_Geographic_GetUnitScale();
    void TestCase_Geographic_GetMinX();
    void TestCase_Geographic_GetMinY();
    void TestCase_Geographic_GetMaxX();
    void TestCase_Geographic_GetMaxY();
    void TestCase_Geographic_GetCsCode();
    void TestCase_Geographic_GetDescription();
    void TestCase_Geographic_GetProjection();
    void TestCase_Geographic_GetProjectionDescription();
    void TestCase_Geographic_GetDatum();
    void TestCase_Geographic_GetDatumDescription();
    void TestCase_Geographic_GetEllipsoid();
    void TestCase_Geographic_GetEllipsoidDescription();
    void TestCase_Geographic_GetCategory();

    void TestCase_ValidateCoordinateSystemProjected();
    void TestCase_Projected_ConvertFromLonLat();
    void TestCase_Projected_ConvertFromLonLatArray();
    void TestCase_Projected_ConvertToLonLat();
    void TestCase_Projected_ConvertToLonLatArray();
    void TestCase_Projected_ConvertCoordinateSystemUnitsToMeters();
    void TestCase_Projected_ConvertMetersToCoordinateSystemUnits();
    void TestCase_Projected_MeasureEuclideanDistance();
    void TestCase_Projected_MeasureGreatCircleDistance();
    void TestCase_Projected_GetAzimuth();
    void TestCase_Projected_GetCoordinate();
    void TestCase_Projected_ConvertCode();
    void TestCase_Projected_GetUnits();
    void TestCase_Projected_GetUnitScale();
    void TestCase_Projected_GetMinX();
    void TestCase_Projected_GetMinY();
    void TestCase_Projected_GetMaxX();
    void TestCase_Projected_GetMaxY();
    void TestCase_Projected_GetCsCode();
    void TestCase_Projected_GetDescription();
    void TestCase_Projected_GetProjection();
    void TestCase_Projected_GetProjectionDescription();
    void TestCase_Projected_GetDatum();
    void TestCase_Projected_GetDatumDescription();
    void TestCase_Projected_GetEllipsoid();
    void TestCase_Projected_GetEllipsoidDescription();
    void TestCase_Projected_GetCategory();

    // Test Measure
    void TestCase_Arbitrary_Measure_GetDistance();
    void TestCase_Arbitrary_Measure_GetAzimuth();
    void TestCase_Arbitrary_Measure_GetCoordinate();
    void TestCase_Geographic_Measure_GetDistance();
    void TestCase_Geographic_Measure_GetAzimuth();
    void TestCase_Geographic_Measure_GetCoordinate();
    void TestCase_Projected_Measure_GetDistance();
    void TestCase_Projected_Measure_GetAzimuth();
    void TestCase_Projected_Measure_GetCoordinate();

    // Test Transform
    // Arbitrary
    void TestCase_Arbitrary_To_Arbitrary_Transform_XY();
    void TestCase_Arbitrary_To_Arbitrary_Transform_XYZ();
    void TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXY();
    void TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXYM();
    void TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXYZ();
    void TestCase_Arbitrary_To_Arbitrary_Transform_CoordinateXYZM();
    void TestCase_Arbitrary_To_Arbitrary_Transform_EnvelopeXY();
    void TestCase_Arbitrary_To_Arbitrary_Transform_EnvelopeXYZ();

    void TestCase_Arbitrary_To_Geographic_Transform_XY();
    void TestCase_Arbitrary_To_Geographic_Transform_XYZ();
    void TestCase_Arbitrary_To_Geographic_Transform_CoordinateXY();
    void TestCase_Arbitrary_To_Geographic_Transform_CoordinateXYM();
    void TestCase_Arbitrary_To_Geographic_Transform_CoordinateXYZ();
    void TestCase_Arbitrary_To_Geographic_Transform_CoordinateXYZM();
    void TestCase_Arbitrary_To_Geographic_Transform_EnvelopeXY();
    void TestCase_Arbitrary_To_Geographic_Transform_EnvelopeXYZ();

    void TestCase_Arbitrary_To_Projected_Transform_XY();
    void TestCase_Arbitrary_To_Projected_Transform_XYZ();
    void TestCase_Arbitrary_To_Projected_Transform_CoordinateXY();
    void TestCase_Arbitrary_To_Projected_Transform_CoordinateXYM();
    void TestCase_Arbitrary_To_Projected_Transform_CoordinateXYZ();
    void TestCase_Arbitrary_To_Projected_Transform_CoordinateXYZM();
    void TestCase_Arbitrary_To_Projected_Transform_EnvelopeXY();
    void TestCase_Arbitrary_To_Projected_Transform_EnvelopeXYZ();

    // Geographic
    void TestCase_Geographic_To_Arbitrary_Transform_XY();
    void TestCase_Geographic_To_Arbitrary_Transform_XYZ();
    void TestCase_Geographic_To_Arbitrary_Transform_CoordinateXY();
    void TestCase_Geographic_To_Arbitrary_Transform_CoordinateXYM();
    void TestCase_Geographic_To_Arbitrary_Transform_CoordinateXYZ();
    void TestCase_Geographic_To_Arbitrary_Transform_CoordinateXYZM();
    void TestCase_Geographic_To_Arbitrary_Transform_EnvelopeXY();
    void TestCase_Geographic_To_Arbitrary_Transform_EnvelopeXYZ();

    void TestCase_Geographic_To_Geographic_Transform_XY();
    void TestCase_Geographic_To_Geographic_Transform_XYZ();
    void TestCase_Geographic_To_Geographic_Transform_CoordinateXY();
    void TestCase_Geographic_To_Geographic_Transform_CoordinateXYM();
    void TestCase_Geographic_To_Geographic_Transform_CoordinateXYZ();
    void TestCase_Geographic_To_Geographic_Transform_CoordinateXYZM();
    void TestCase_Geographic_To_Geographic_Transform_EnvelopeXY();
    void TestCase_Geographic_To_Geographic_Transform_EnvelopeXYZ();

    void TestCase_Geographic_To_Projected_Transform_XY();
    void TestCase_Geographic_To_Projected_Transform_XYZ();
    void TestCase_Geographic_To_Projected_Transform_CoordinateXY();
    void TestCase_Geographic_To_Projected_Transform_CoordinateXYM();
    void TestCase_Geographic_To_Projected_Transform_CoordinateXYZ();
    void TestCase_Geographic_To_Projected_Transform_CoordinateXYZM();
    void TestCase_Geographic_To_Projected_Transform_EnvelopeXY();
    void TestCase_Geographic_To_Projected_Transform_EnvelopeXYZ();

    // Projected
    void TestCase_Projected_To_Arbitrary_Transform_XY();
    void TestCase_Projected_To_Arbitrary_Transform_XYZ();
    void TestCase_Projected_To_Arbitrary_Transform_CoordinateXY();
    void TestCase_Projected_To_Arbitrary_Transform_CoordinateXYM();
    void TestCase_Projected_To_Arbitrary_Transform_CoordinateXYZ();
    void TestCase_Projected_To_Arbitrary_Transform_CoordinateXYZM();
    void TestCase_Projected_To_Arbitrary_Transform_EnvelopeXY();
    void TestCase_Projected_To_Arbitrary_Transform_EnvelopeXYZ();

    void TestCase_Projected_To_Geographic_Transform_XY();
    void TestCase_Projected_To_Geographic_Transform_XYZ();
    void TestCase_Projected_To_Geographic_Transform_CoordinateXY();
    void TestCase_Projected_To_Geographic_Transform_CoordinateXYM();
    void TestCase_Projected_To_Geographic_Transform_CoordinateXYZ();
    void TestCase_Projected_To_Geographic_Transform_CoordinateXYZM();
    void TestCase_Projected_To_Geographic_Transform_EnvelopeXY();
    void TestCase_Projected_To_Geographic_Transform_EnvelopeXYZ();

    void TestCase_Projected_To_Projected_Transform_XY();
    void TestCase_Projected_To_Projected_Transform_XYZ();
    void TestCase_Projected_To_Projected_Transform_CoordinateXY();
    void TestCase_Projected_To_Projected_Transform_CoordinateXYM();
    void TestCase_Projected_To_Projected_Transform_CoordinateXYZ();
    void TestCase_Projected_To_Projected_Transform_CoordinateXYZM();
    void TestCase_Projected_To_Projected_Transform_EnvelopeXY();
    void TestCase_Projected_To_Projected_Transform_EnvelopeXYZ();

    // Datum conversion
    void TestCase_Geographic_DatumConversion();

    // Real world locations
    void TestCase_Boston_Geographic();
    void TestCase_NewYork_Geographic();
    void TestCase_Boston_Projected();
    void TestCase_NewYork_Projected();

    // EPSG
    void TestCase_EPSG();

    // Performance
    void TestCase_Benchmark_Transformation();

    static bool SetDefaultUserDictionaryDir();

    class FileAutoBackup
    {
    private:

        STRING m_sFilename;
        STRING m_sRotateSuffix;
        STRING m_sBackupFilename;
        bool m_bRotated;
        bool m_bKeepFile;

    public:
        FileAutoBackup(const CREFSTRING filename, const CREFSTRING rotateSuffix, bool keepFile = false)
            : m_sFilename(filename), m_sRotateSuffix(rotateSuffix), m_bRotated(false), m_bKeepFile(keepFile)
        {
            struct _stat64 fileStatus;
            bool fileExists = MgFileUtil::GetFileStatus(filename, fileStatus);

            this->m_sBackupFilename = (this->m_sFilename + this->m_sRotateSuffix);

            ACE_DEBUG((LM_INFO, ACE_TEXT("\nBacking up file\n%W\n-->\n%W\n"), this->m_sFilename.c_str(), this->m_sBackupFilename.c_str()));

            MgFileUtil::DeleteFile(this->m_sBackupFilename);
            if (fileExists)
            {
                if (!this->m_bKeepFile)
                {
                    MgFileUtil::RenameFile(this->m_sFilename, this->m_sBackupFilename);
                }
                else
                {
                    MgFileUtil::CopyFile(this->m_sFilename, this->m_sBackupFilename);
                }

                this->m_bRotated = true;
            }
        }

        ~FileAutoBackup()
        {
            MgFileUtil::DeleteFile(this->m_sFilename);
            if (this->m_bRotated)
            {
                ACE_DEBUG((LM_INFO, ACE_TEXT("\nRestoring file\n%W\n-->\n%W\n"), this->m_sBackupFilename.c_str(), this->m_sFilename.c_str()));
                MgFileUtil::RenameFile(this->m_sBackupFilename, this->m_sFilename, true);
            }
            else
            {
                ACE_DEBUG((LM_INFO, ACE_TEXT("\nDid not restore file\n%W\nas it did not exist before\n"), this->m_sFilename.c_str()));
            }
        }
    };

private:

    bool CompareCodes(STRING code1, STRING code2);
};

#endif
