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

#ifndef TESTGEOMETRY_H_
#define TESTGEOMETRY_H_

#include <cppunit/extensions/HelperMacros.h>

#ifndef _WIN32
//Linux: different naming for string functions
#define stricmp strcasecmp
#endif

class TestGeometry : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestGeometry);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_Point);
    CPPUNIT_TEST(TestCase_LineString);
    CPPUNIT_TEST(TestCase_Polygon);
    CPPUNIT_TEST(TestCase_CurveString);
    CPPUNIT_TEST(TestCase_CurveRing);
    CPPUNIT_TEST(TestCase_CurvePolygon);
    CPPUNIT_TEST(TestCase_MultiPoint);
    CPPUNIT_TEST(TestCase_MultiLineString);
    CPPUNIT_TEST(TestCase_MultiPolygon);
    CPPUNIT_TEST(TestCase_MultiCurveString);
    CPPUNIT_TEST(TestCase_MultiCurvePolygon);
    CPPUNIT_TEST(TestCase_MultiGeometry);

    CPPUNIT_TEST(TestCase_BufferNoMeasure);
    CPPUNIT_TEST(TestCase_BufferArbitrary);
    CPPUNIT_TEST(TestCase_BufferGeographic);
    CPPUNIT_TEST(TestCase_BufferProjected);

    CPPUNIT_TEST(TestCase_Intersection);
    CPPUNIT_TEST(TestCase_Boundary);
    CPPUNIT_TEST(TestCase_ConvexHull);
    CPPUNIT_TEST(TestCase_Difference);
    CPPUNIT_TEST(TestCase_SymetricDifference);
    CPPUNIT_TEST(TestCase_Union);

    CPPUNIT_TEST(TestCase_Distance);
    CPPUNIT_TEST(TestCase_Transform);
    CPPUNIT_TEST(TestCase_Touches);
    CPPUNIT_TEST(TestCase_Envelope);
    CPPUNIT_TEST(TestCase_GetArea);
    CPPUNIT_TEST(TestCase_GetCentroid);
    CPPUNIT_TEST(TestCase_GetInteriorPoint);
    CPPUNIT_TEST(TestCase_CoordinateIterator);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();
    TestGeometry();

    void TestCase_Point();
    void TestCase_LineString();
    void TestCase_Polygon();
    void TestCase_CurveString();
    void TestCase_CurveRing();
    void TestCase_CurvePolygon();
    void TestCase_MultiPoint();
    void TestCase_MultiLineString();
    void TestCase_MultiPolygon();
    void TestCase_MultiCurveString();
    void TestCase_MultiCurvePolygon();
    void TestCase_MultiGeometry();

    void TestCase_BufferNoMeasure();
    void TestCase_BufferArbitrary();
    void TestCase_BufferGeographic();
    void TestCase_BufferProjected();

    void TestCase_Intersection();
    void TestCase_Boundary();
    void TestCase_ConvexHull();
    void TestCase_Difference();
    void TestCase_SymetricDifference();
    void TestCase_Union();

    void TestCase_Distance();
    void TestCase_Transform();
    void TestCase_Touches();
    void TestCase_Envelope();
    void TestCase_GetArea();
    void TestCase_GetCentroid();
    void TestCase_GetInteriorPoint();
    void TestCase_CoordinateIterator();

    MgPoint*             CreatePoint();
    MgLineString*        CreateLineString();
    MgLinearRing*        CreateLinearRing();
    MgPolygon*           CreatePolygon();
    MgCurveString*       CreateCurveString(double offset);
    MgCurveRing*         CreateCurveRing(double offset);
    MgCurvePolygon*      CreateCurvePolygon(double offset, int increment=100);
    MgMultiPoint*        CreateMultiPoint();
    MgMultiLineString*   CreateMultiLineString();
    MgMultiPolygon*      CreateMultiPolygon();
    MgMultiCurveString*  CreateMultiCurveString();
    MgMultiCurvePolygon* CreateMultiCurvePolygon(INT32 numCurvePolys, double offset);
    MgMultiGeometry*     CreateMultiGeometry();

    MgCoordinateSystemMeasure* CreateMeasureArbitrary();
    MgCoordinateSystemMeasure* CreateMeasureGeographic();
    MgCoordinateSystemMeasure* CreateMeasureProjected();

    MgCoordinateSystem* CreateCoordinateSystemArbitrary();
    MgCoordinateSystem* CreateCoordinateSystemGeographic();
    MgCoordinateSystem* CreateCoordinateSystemProjected();

    bool CheckGeometry(MgGeometry* geom, CREFSTRING wkt);
    bool CheckGeometry(CREFSTRING calculated, CREFSTRING base);
};

#endif
