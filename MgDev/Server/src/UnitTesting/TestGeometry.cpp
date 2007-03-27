//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "TestGeometry.h"
#include "CppUnitExtensions.h"

#ifndef _WIN32
// TODO: results under Linux are sometimes significantly different than in Windows
#define DIFFERENT_LINUX_RESULT
#endif

const STRING ArbitraryWkt  = L"LOCAL_CS [ \"Non-Earth (Mile)\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"Mile\", 1.609344e+03], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";
const STRING GeographicWkt = L"GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"Degrees\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]]";
const STRING ProjectedWkt  = L"PROJCS[\"Georgia West\",GEOGCS[\"NAD27\",DATUM[\"North_American_Datum_1927\",SPHEROID[\"Clarke 1866\",6378206.4,294.978698213898,AUTHORITY[\"EPSG\",\"7008\"]],TOWGS84[-3,142,183,0,0,0,0],AUTHORITY[\"EPSG\",\"6267\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4267\"]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",30],PARAMETER[\"central_meridian\",-84.16666666666667],PARAMETER[\"scale_factor\",0.9999],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"Foot\",0.3048006096012192]]";

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestGeometry, "TestGeometry");

void TestGeometry::setUp()
{
}


void TestGeometry::tearDown()
{
}


void TestGeometry::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Geometry tests. (PROJ4)\n")));
}


void TestGeometry::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nGeometry tests completed.\n\n")));
}


TestGeometry::TestGeometry()
{
}


MgPoint* TestGeometry::CreatePoint()
{
    MgGeometryFactory factory;
    Ptr<MgCoordinate> coord = factory.CreateCoordinateXYZ(5.0, 3.0, 2.0);

    return factory.CreatePoint(coord);
}


MgLineString* TestGeometry::CreateLineString()
{
    MgGeometryFactory factory;

    Ptr<MgCoordinateCollection> coordCol = new MgCoordinateCollection();

    Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXY(0.0, 1.0);
    Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXY(2.0, 3.0);
    Ptr<MgCoordinate> coord3 = factory.CreateCoordinateXY(4.0, 5.0);

    coordCol->Add(coord1);
    coordCol->Add(coord2);
    coordCol->Add(coord3);

    return factory.CreateLineString(coordCol);
}


MgLinearRing* TestGeometry::CreateLinearRing()
{
    MgGeometryFactory factory;

    Ptr<MgCoordinate> oCoord1 = factory.CreateCoordinateXY(0.0, 0.0);
    Ptr<MgCoordinate> oCoord2 = factory.CreateCoordinateXY(5.0, 0.0);
    Ptr<MgCoordinate> oCoord3 = factory.CreateCoordinateXY(5.0, 5.0);
    Ptr<MgCoordinate> oCoord4 = factory.CreateCoordinateXY(0.0, 5.0);
    Ptr<MgCoordinate> oCoord5 = factory.CreateCoordinateXY(0.0, 0.0);

    Ptr<MgCoordinateCollection> outerRingCoord = new MgCoordinateCollection();
    outerRingCoord->Add(oCoord1);
    outerRingCoord->Add(oCoord2);
    outerRingCoord->Add(oCoord3);
    outerRingCoord->Add(oCoord4);
    outerRingCoord->Add(oCoord5);

    return factory.CreateLinearRing(outerRingCoord);
}


MgPolygon* TestGeometry::CreatePolygon()
{
    MgGeometryFactory factory;

    // OuterRing
    Ptr<MgCoordinate> oCoord1 = factory.CreateCoordinateXY(0.0, 0.0);
    Ptr<MgCoordinate> oCoord2 = factory.CreateCoordinateXY(5.0, 0.0);
    Ptr<MgCoordinate> oCoord3 = factory.CreateCoordinateXY(5.0, 5.0);
    Ptr<MgCoordinate> oCoord4 = factory.CreateCoordinateXY(0.0, 5.0);
    Ptr<MgCoordinate> oCoord5 = factory.CreateCoordinateXY(0.0, 0.0);

    Ptr<MgCoordinateCollection> outerRingCoord = new MgCoordinateCollection();
    outerRingCoord->Add(oCoord1);
    outerRingCoord->Add(oCoord2);
    outerRingCoord->Add(oCoord3);
    outerRingCoord->Add(oCoord4);
    outerRingCoord->Add(oCoord5);

    // Inner Ring1
    Ptr<MgCoordinate> i1Coord1 = factory.CreateCoordinateXY(1.0, 1.0);
    Ptr<MgCoordinate> i1Coord2 = factory.CreateCoordinateXY(2.0, 1.0);
    Ptr<MgCoordinate> i1Coord3 = factory.CreateCoordinateXY(2.0, 2.0);
    Ptr<MgCoordinate> i1Coord4 = factory.CreateCoordinateXY(1.0, 1.0);

    Ptr<MgCoordinateCollection> inner1RingCoord = new MgCoordinateCollection();
    inner1RingCoord->Add(i1Coord1);
    inner1RingCoord->Add(i1Coord2);
    inner1RingCoord->Add(i1Coord3);
    inner1RingCoord->Add(i1Coord4);

    // Inner Ring2
    Ptr<MgCoordinate> i2Coord1 = factory.CreateCoordinateXY(3.0, 3.0);
    Ptr<MgCoordinate> i2Coord2 = factory.CreateCoordinateXY(4.0, 3.0);
    Ptr<MgCoordinate> i2Coord3 = factory.CreateCoordinateXY(4.0, 4.0);
    Ptr<MgCoordinate> i2Coord4 = factory.CreateCoordinateXY(3.0, 3.0);

    Ptr<MgCoordinateCollection> inner2RingCoord = new MgCoordinateCollection();
    inner2RingCoord->Add(i2Coord1);
    inner2RingCoord->Add(i2Coord2);
    inner2RingCoord->Add(i2Coord3);
    inner2RingCoord->Add(i2Coord4);

    Ptr<MgLinearRing> extRing  = factory.CreateLinearRing(outerRingCoord);
    Ptr<MgLinearRing> intRing1 = factory.CreateLinearRing(inner1RingCoord);
    Ptr<MgLinearRing> intRing2 = factory.CreateLinearRing(inner2RingCoord);

    Ptr<MgLinearRingCollection> intRings = new MgLinearRingCollection();
    intRings->Add(intRing1);
    intRings->Add(intRing2);

    return factory.CreatePolygon(extRing, intRings);
}


MgCurveString* TestGeometry::CreateCurveString(double offset)
{
    MgGeometryFactory factory;

    // Create and return a curvestring consisting of
    // one circulararc segment and one linearstring segment

    // arcseg  = (0,0), (0,1), (1,2)
    // lineseg = (1,2), (3,0), (3,2)

    // ArcSegment
    Ptr<MgCoordinate> startPos = factory.CreateCoordinateXY(offset+0.0, offset+0.0);
    Ptr<MgCoordinate> midPos   = factory.CreateCoordinateXY(offset+0.0, offset+1.0);
    Ptr<MgCoordinate> endPos   = factory.CreateCoordinateXY(offset+1.0, offset+2.0);
    Ptr<MgArcSegment> arcSeg   = factory.CreateArcSegment(startPos, endPos, midPos);

    // Linear Segment
    Ptr<MgCoordinateCollection> points = new MgCoordinateCollection();
    Ptr<MgCoordinate> pt1 = factory.CreateCoordinateXY(offset+1.0, offset+2.0);
    Ptr<MgCoordinate> pt2 = factory.CreateCoordinateXY(offset+3.0, offset+0.0);
    Ptr<MgCoordinate> pt3 = factory.CreateCoordinateXY(offset+3.0, offset+2.0);
    points->Add(pt1);
    points->Add(pt2);
    points->Add(pt3);
    Ptr<MgLinearSegment> lineSeg = factory.CreateLinearSegment(points);

    // CurveSegment
    Ptr<MgCurveSegmentCollection> curveSegs = new MgCurveSegmentCollection();
    curveSegs->Add(arcSeg);
    curveSegs->Add(lineSeg);

    return factory.CreateCurveString(curveSegs);
}


MgCurveRing* TestGeometry::CreateCurveRing(double offset)
{
    // Ring is a closed entity.
    // Create and return a ring consisting of
    // one circulararc segment and one linearstring segment

    // arcseg  = (0,0), (0,1), (1,2)
    // lineseg = (1,2), (0,0)

    MgGeometryFactory factory;

    // ArcSegment
    Ptr<MgCoordinate> startPos = factory.CreateCoordinateXY(offset+0.0, offset+0.0);
    Ptr<MgCoordinate> midPos   = factory.CreateCoordinateXY(offset+0.0, offset+1.0);
    Ptr<MgCoordinate> endPos   = factory.CreateCoordinateXY(offset+1.0, offset+2.0);
    Ptr<MgArcSegment> arcSeg   = factory.CreateArcSegment(startPos, endPos, midPos);

    // Linear Segment
    Ptr<MgCoordinateCollection> points = new MgCoordinateCollection;
    Ptr<MgCoordinate> fromPt = factory.CreateCoordinateXY(offset+1.0, offset+2.0);
    Ptr<MgCoordinate>   toPt = factory.CreateCoordinateXY(offset+0.0, offset+0.0);
    points->Add(fromPt);
    points->Add(toPt);
    Ptr<MgLinearSegment> lineSeg = factory.CreateLinearSegment(points);

    // Curve Segment
    Ptr<MgCurveSegmentCollection> curveSegs = new MgCurveSegmentCollection();
    curveSegs->Add(arcSeg);
    curveSegs->Add(lineSeg);

    return factory.CreateCurveRing(curveSegs);
}


MgCurvePolygon* TestGeometry::CreateCurvePolygon(double offset, int increment)
{
    MgGeometryFactory factory;

    Ptr<MgCurveRing> extRing = CreateCurveRing(offset + increment);
    INT32 numIntRings = 2;

    Ptr<MgCurveRingCollection> intRings = new MgCurveRingCollection();

    Ptr<MgCurveRing> ring1 = CreateCurveRing(offset + 2 * increment);
    Ptr<MgCurveRing> ring2 = CreateCurveRing(offset + 3 * increment);

    intRings->Add(ring1);
    intRings->Add(ring2);

    return factory.CreateCurvePolygon(extRing, intRings);
}


MgMultiPoint* TestGeometry::CreateMultiPoint()
{
    MgGeometryFactory factory;

    Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXYZ(1.0, 2.0, 3.0);
    Ptr<MgPoint> point1 = factory.CreatePoint(coord1);

    Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXYZ(4.0, 5.0, 6.0);
    Ptr<MgPoint> point2 = factory.CreatePoint(coord2);

    Ptr<MgCoordinate> coord3 = factory.CreateCoordinateXYZ(7.0, 8.0, 9.0);
    Ptr<MgPoint> point3 = factory.CreatePoint(coord3);

    Ptr<MgPointCollection> pnts = new MgPointCollection();
    pnts->Add(point1);
    pnts->Add(point2);
    pnts->Add(point3);

    return factory.CreateMultiPoint(pnts);
}


MgMultiLineString* TestGeometry::CreateMultiLineString()
{
    MgGeometryFactory factory;

    Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXYZ(0.0, 1.0, 2.0);
    Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXYZ(3.0, 4.0, 5.0);
    Ptr<MgCoordinate> coord3 = factory.CreateCoordinateXYZ(6.0, 7.0, 8.0);

    Ptr<MgCoordinateCollection> coordColA = new MgCoordinateCollection();
    coordColA->Add(coord1);
    coordColA->Add(coord2);
    coordColA->Add(coord3);

    Ptr<MgCoordinate> coord4 = factory.CreateCoordinateXYZ( 9.0, 10.0, 11.0);
    Ptr<MgCoordinate> coord5 = factory.CreateCoordinateXYZ(12.0, 13.0, 14.0);
    Ptr<MgCoordinate> coord6 = factory.CreateCoordinateXYZ(15.0, 16.0, 17.0);

    Ptr<MgCoordinateCollection> coordColB = new MgCoordinateCollection();
    coordColB->Add(coord4);
    coordColB->Add(coord5);
    coordColB->Add(coord6);

    Ptr<MgLineString> lineString1 = factory.CreateLineString(coordColA);
    Ptr<MgLineString> lineString2 = factory.CreateLineString(coordColB);

    Ptr<MgLineStringCollection> lineStrings = new MgLineStringCollection();
    lineStrings->Add(lineString1);
    lineStrings->Add(lineString2);

    return factory.CreateMultiLineString(lineStrings);
}


MgMultiPolygon* TestGeometry::CreateMultiPolygon()
{
    MgGeometryFactory factory;

    Ptr<MgPolygon> polygon1 = CreatePolygon();
    Ptr<MgPolygon> polygon2 = CreatePolygon();

    Ptr<MgPolygonCollection> polygons = new MgPolygonCollection();
    polygons->Add(polygon1);
    polygons->Add(polygon2);

    return factory.CreateMultiPolygon(polygons);
}


MgMultiCurveString* TestGeometry::CreateMultiCurveString()
{
    MgGeometryFactory factory;

    Ptr<MgCurveString> curveString1 = CreateCurveString(100);
    Ptr<MgCurveString> curveString2 = CreateCurveString(200);
    Ptr<MgCurveString> curveString3 = CreateCurveString(300);

    Ptr<MgCurveStringCollection> curveStrings = new MgCurveStringCollection();
    curveStrings->Add(curveString1);
    curveStrings->Add(curveString2);
    curveStrings->Add(curveString3);

    return factory.CreateMultiCurveString(curveStrings);
}


MgMultiCurvePolygon* TestGeometry::CreateMultiCurvePolygon(INT32 numCurvePolys, double offset)
{
    MgGeometryFactory factory;

    Ptr<MgCurvePolygonCollection> curvePolys = new MgCurvePolygonCollection();
    for (INT32 i=0; i < numCurvePolys; i++)
    {
        Ptr<MgCurvePolygon> curvePoly = CreateCurvePolygon(i+offset);
        curvePolys->Add(curvePoly);
    }

    return factory.CreateMultiCurvePolygon(curvePolys);
}


MgMultiGeometry* TestGeometry::CreateMultiGeometry()
{
    MgGeometryFactory factory;

    Ptr<MgGeometryCollection> geometries = new MgGeometryCollection();
    Ptr<MgGeometry> geometry;

    // CurvePolygon
    geometry = (MgGeometry*)CreateCurvePolygon(0);
    geometries->Add(geometry);

    // CurveString
    // Not doing CurveString because of some unfixed defect.
    // It may be the same one that sometimes affects MultiPolygon.
    geometry = (MgGeometry*)CreateCurveString(100);
    geometries->Add(geometry);

    // LineString
    geometry = (MgGeometry*)CreateLineString();
    geometries->Add(geometry);

    // Point
    geometry = (MgGeometry*)CreatePoint();
    geometries->Add(geometry);

    // Polygon
    geometry = CreatePolygon();
    geometries->Add(geometry);

    // Make MultiGeometry from the many geometries collected above.
    return factory.CreateMultiGeometry(geometries);
}


MgCoordinateSystemMeasure* TestGeometry::CreateMeasureArbitrary()
{
    MgCoordinateSystemFactory factory;
    Ptr<MgCoordinateSystem> pCoordSys = factory.Create(ArbitraryWkt);
    return new MgCoordinateSystemMeasure(pCoordSys);
}


MgCoordinateSystemMeasure* TestGeometry::CreateMeasureGeographic()
{
    MgCoordinateSystemFactory factory;
    Ptr<MgCoordinateSystem> pCoordSys = factory.Create(GeographicWkt);
    return new MgCoordinateSystemMeasure(pCoordSys);
}


MgCoordinateSystemMeasure* TestGeometry::CreateMeasureProjected()
{
    MgCoordinateSystemFactory factory;
    Ptr<MgCoordinateSystem> pCoordSys = factory.Create(ProjectedWkt);
    return new MgCoordinateSystemMeasure(pCoordSys);
}


MgCoordinateSystem* TestGeometry::CreateCoordinateSystemArbitrary()
{
    MgCoordinateSystemFactory factory;
    return factory.Create(ArbitraryWkt);
}

MgCoordinateSystem* TestGeometry::CreateCoordinateSystemGeographic()
{
    MgCoordinateSystemFactory factory;
    return factory.Create(GeographicWkt);
}


MgCoordinateSystem* TestGeometry::CreateCoordinateSystemProjected()
{
    MgCoordinateSystemFactory factory;
    return factory.Create(ProjectedWkt);
}

bool TestGeometry::CheckGeometry(MgGeometry* geom, CREFSTRING wkt)
{
    bool success = true;
    MgWktReaderWriter readerWriter;
    Ptr<MgCoordinateIterator> iterator = geom->GetCoordinates();
    Ptr<MgGeometry> lineString = readerWriter.Read(wkt);
    Ptr<MgCoordinateIterator> baselineIterator = lineString->GetCoordinates();

    while(iterator->MoveNext() && baselineIterator->MoveNext())
    {
        Ptr<MgCoordinate> coord = iterator->GetCurrent();
        Ptr<MgCoordinate> baselineCoord = baselineIterator->GetCurrent();

        if (!MgUtil::ValuesEqual(baselineCoord->GetX(), coord->GetX()) ||
            !MgUtil::ValuesEqual(baselineCoord->GetY(), coord->GetY()))
        {
            success = false;
            break;
        }
    }

    return success;
}

void TestGeometry::TestCase_Point()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"POINT XYZ (5 3 2)";

        Ptr<MgGeometry> geom = CreatePoint();
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> point = readerWriter.Read(base);
        found = readerWriter.Write(point);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_LineString()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"LINESTRING (0 1, 2 3, 4 5)";

        Ptr<MgGeometry> geom = CreateLineString();
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> lineString = readerWriter.Read(base);
        found = readerWriter.Write(lineString);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_Polygon()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"POLYGON ((0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3))";

        Ptr<MgGeometry> geom = CreatePolygon();
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> geom1 = readerWriter.Read(base);
        found = readerWriter.Write(geom1);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_CurveString()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"CURVESTRING (0 0 (CIRCULARARCSEGMENT (0 1, 1 2), LINESTRINGSEGMENT (3 0, 3 2)))";

        Ptr<MgGeometry> geom = CreateCurveString(0.0);
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> geom1 = readerWriter.Read(base);
        found = readerWriter.Write(geom1);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_CurveRing()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"(3.14 3.14 (CIRCULARARCSEGMENT (3.14 4.140000000000001, 4.140000000000001 5.140000000000001),"
                      L" LINESTRINGSEGMENT (3.14 3.14)))";

        Ptr<MgGeometricEntity> geom = CreateCurveRing(3.14);
        STRING found = geom->ToAwkt(true);
        CPPUNIT_ASSERT(wstring::npos != found.find(L"CIRCULARARCSEGMENT"));
        CPPUNIT_ASSERT(wstring::npos != found.find(L"LINESTRINGSEGMENT"));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_CurvePolygon()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"CURVEPOLYGON ((200 200 (CIRCULARARCSEGMENT (200 201, 201 202), LINESTRINGSEGMENT (200 200))),"
                      L" (300 300 (CIRCULARARCSEGMENT (300 301, 301 302), LINESTRINGSEGMENT (300 300))),"
                      L" (400 400 (CIRCULARARCSEGMENT (400 401, 401 402), LINESTRINGSEGMENT (400 400))))";

        Ptr<MgGeometry> geom = CreateCurvePolygon(100.0);
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> geom1 = readerWriter.Read(base);
        found = readerWriter.Write(geom1);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_MultiPoint()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"MULTIPOINT XYZ (1 2 3, 4 5 6, 7 8 9)";

        Ptr<MgGeometry> geom = CreateMultiPoint();
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> geom1 = readerWriter.Read(base);
        found = readerWriter.Write(geom1);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_MultiLineString()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"MULTILINESTRING XYZ ((0 1 2, 3 4 5, 6 7 8), (9 10 11, 12 13 14, 15 16 17))";

        Ptr<MgGeometry> geom = CreateMultiLineString();
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> geom1 = readerWriter.Read(base);
        found = readerWriter.Write(geom1);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_MultiPolygon()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"MULTIPOLYGON (((0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3)),"
                      L" ((0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3)))";

        Ptr<MgGeometry> geom = CreateMultiPolygon();
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> geom1 = readerWriter.Read(base);
        found = readerWriter.Write(geom1);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_MultiCurveString()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"MULTICURVESTRING ((100 100 (CIRCULARARCSEGMENT (100 101, 101 102), LINESTRINGSEGMENT (103 100, 103 102))),"
                      L" (200 200 (CIRCULARARCSEGMENT (200 201, 201 202), LINESTRINGSEGMENT (203 200, 203 202))),"
                      L" (300 300 (CIRCULARARCSEGMENT (300 301, 301 302), LINESTRINGSEGMENT (303 300, 303 302))))";

        Ptr<MgGeometry> geom = CreateMultiCurveString();
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> geom1 = readerWriter.Read(base);
        found = readerWriter.Write(geom1);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_MultiCurvePolygon()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"MULTICURVEPOLYGON (((200 200 (CIRCULARARCSEGMENT (200 201, 201 202), LINESTRINGSEGMENT (200 200))),"
                      L" (300 300 (CIRCULARARCSEGMENT (300 301, 301 302), LINESTRINGSEGMENT (300 300))),"
                      L" (400 400 (CIRCULARARCSEGMENT (400 401, 401 402), LINESTRINGSEGMENT (400 400)))),"
                      L" ((201 201 (CIRCULARARCSEGMENT (201 202, 202 203), LINESTRINGSEGMENT (201 201))),"
                      L" (301 301 (CIRCULARARCSEGMENT (301 302, 302 303), LINESTRINGSEGMENT (301 301))),"
                      L" (401 401 (CIRCULARARCSEGMENT (401 402, 402 403), LINESTRINGSEGMENT (401 401)))),"
                      L" ((202 202 (CIRCULARARCSEGMENT (202 203, 203 204), LINESTRINGSEGMENT (202 202))),"
                      L" (302 302 (CIRCULARARCSEGMENT (302 303, 303 304), LINESTRINGSEGMENT (302 302))),"
                      L" (402 402 (CIRCULARARCSEGMENT (402 403, 403 404), LINESTRINGSEGMENT (402 402)))))";

        Ptr<MgGeometry> geom = CreateMultiCurvePolygon(3, 100.0);
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> geom1 = readerWriter.Read(base);
        found = readerWriter.Write(geom1);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_MultiGeometry()
{
    try
    {
        MgWktReaderWriter readerWriter;

        STRING base = L"GEOMETRYCOLLECTION (CURVEPOLYGON ((100 100 (CIRCULARARCSEGMENT (100 101, 101 102),"
                      L" LINESTRINGSEGMENT (100 100))), (200 200 (CIRCULARARCSEGMENT (200 201, 201 202),"
                      L" LINESTRINGSEGMENT (200 200))), (300 300 (CIRCULARARCSEGMENT (300 301, 301 302),"
                      L" LINESTRINGSEGMENT (300 300)))), CURVESTRING (100 100 (CIRCULARARCSEGMENT (100 101, 101 102),"
                      L" LINESTRINGSEGMENT (103 100, 103 102))), LINESTRING (0 1, 2 3, 4 5), POINT XYZ (5 3 2),"
                      L" POLYGON ((0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3)))";

        Ptr<MgGeometry> geom = CreateMultiGeometry();
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);

        Ptr<MgGeometry> geom1 = readerWriter.Read(base);
        found = readerWriter.Write(geom1);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), base.c_str()) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_BufferNoMeasure()
{
    try
    {
        MgWktReaderWriter readerWriter;

        Ptr<MgPoint> point = CreatePoint();
        Ptr<MgGeometry> geom = point->Buffer(5.0, NULL);
        STRING base = L"POLYGON ((10 3, 9.975923596377587 3.490085727284638, 9.903926545615587 3.975451660210092,"
                      L" 9.784701751750813 4.451423490217149, 9.619397796356546 4.913417209794189, 9.409606421566393"
                      L" 5.356983853219015, 9.157347926935429 5.777851257068069, 8.865052393975997 6.171966752048345,"
                      L" 8.535533758135662 6.535533968869922, 8.17196656298438 6.865052624351099, 7.777851091495221"
                      L" 7.157348174732707, 7.356983712731821 7.409606684399449, 6.913417095745629 7.619398071694144,"
                      L" 6.451423403705562 7.784702036941296, 5.975451602068639 7.903926837912422, 5.49008569807325"
                      L" 7.975923892965781, 5 8.000000298023259, 4.509914301926751 7.975923892965781, 4.024548397931361"
                      L" 7.903926837912422, 3.548576596294438 7.784702036941296, 3.086582904254371 7.619398071694144,"
                      L" 2.643016287268179 7.409606684399449, 2.222148908504779 7.157348174732707, 1.828033437015619"
                      L" 6.865052624351099, 1.464466241864338 6.535533968869922, 1.134947606024003 6.171966752048345,"
                      L" 0.84265207306457 5.777851257068069, 0.5903935784336077 5.356983853219015, 0.3806022036434533"
                      L" 4.913417209794189, 0.215298248249188 4.451423490217149, 0.09607345438441363 3.975451660210092,"
                      L" 0.0240764036224129 3.490085727284638, 0 3, 0.0240764036224129 2.509914272715363,"
                      L" 0.09607345438441363 2.024548339789908, 0.215298248249188 1.548576509782852, 0.3806022036434533"
                      L" 1.086582790205811, 0.5903935784336077 0.6430161467809854, 0.84265207306457 0.2221487429319313,"
                      L" 1.134947606024003 -0.1719667520483437, 1.464466241864338 -0.5355339688699208, 1.828033437015619"
                      L" -0.865052624351099, 2.222148908504779 -1.157348174732706, 2.643016287268179 -1.409606684399448,"
                      L" 3.086582904254371 -1.619398071694144, 3.548576596294438 -1.784702036941294, 4.024548397931361"
                      L" -1.903926837912421, 4.509914301926751 -1.975923892965781, 5 -2.000000298023259, 5.49008569807325"
                      L" -1.975923892965781, 5.975451602068639 -1.903926837912421, 6.451423403705562 -1.784702036941294,"
                      L" 6.913417095745629 -1.619398071694144, 7.356983712731821 -1.409606684399448, 7.777851091495221"
                      L" -1.157348174732706, 8.17196656298438 -0.865052624351099, 8.535533758135662 -0.5355339688699208,"
                      L" 8.865052393975997 -0.1719667520483437, 9.157347926935429 0.2221487429319313, 9.409606421566393"
                      L" 0.6430161467809854, 9.619397796356546 1.086582790205811, 9.784701751750813 1.548576509782852,"
                      L" 9.903926545615587 2.024548339789908, 9.975923596377587 2.509914272715363, 10 3))";
        CPPUNIT_ASSERT(CheckGeometry(geom, base));
        STRING found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        Ptr<MgMultiGeometry> multiGeometry = CreateMultiGeometry();
        geom = multiGeometry->Buffer(5.0, NULL);
        base = L"POLYGON ((95.66986904560739 97.49999879726333, 96.46446230795755 96.46446273993995, 97.49999836087217"
               L" 95.66986947420682, 98.70590446626571 95.17036764773094, 99.99999955296514 94.99999759452668,"
               L" 101.2940946396646 95.17036764773094, 102.5000007450581 95.66986947420682, 103.5355367979727"
               L" 96.46446273993995, 104.0528232486739 97.1386048158538, 104.2940960701761 97.17036860992026,"
               L" 105.5000021755697 97.66987043639615, 106.5355382284843 98.46446370212927, 107.3301314908345"
               L" 99.49999975945266, 107.8296333151837 100.7059058699803, 108.0000033676626 102.0000009621893,"
               L" 107.8296333151837 103.2940960543983, 107.3301314908345 104.500002164926, 106.5355382284843"
               L" 105.5355382222494, 105.5000021755697 106.3301314879825, 104.2940960701761 106.8296333144584,"
               L" 103.0000009834767 107.0000033676626, 101.0000000298023 107.0000033676626, 99.70590494310289"
               L" 106.8296333144584, 98.49999883770936 106.3301314879825, 97.46446278479473 105.5355382222494,"
               L" 96.46446230795755 104.5355377411547, 95.66986904560739 103.5000016838313, 95.17036722125812"
               L" 102.2940955733037, 94.9999971687792 101.0000004810947, 94.9999971687792 100, 95.17036722125812"
               L" 98.70590490779098, 95.66986904560739 97.49999879726333), (195.1703643900373 198.705899134655,"
               L" 195.6698695820492 197.4999930241274, 196.4644628443994 196.4644637021293, 197.4999921619887"
               L" 195.6698704363961, 198.7058982673823 195.1703652422576, 199.9999967217443 194.9999884537281,"
               L" 201.2940951761064 195.1703652422576, 202.5000012815 195.6698704363961, 203.5355305990893"
               L" 196.4644637021293, 204.3301238614395 197.4999930241274, 204.472131459244 197.763910010281,"
               L" 205.4721386714064 199.7639109724703, 205.8296362656138 200.7059000968444, 206.000013053418"
               L" 201.999998556716, 205.8296362656138 203.2940970165876, 205.3301310736019 204.5000031271153,"
               L" 204.5355378112517 205.5355324491134, 203.5000084936624 206.3301257148465, 202.2941023882688"
               L" 206.8296309089851, 201.0000039339068 206.999994226864, 199.7059054795447 206.8296309089851,"
               L" 198.4999993741512 206.3301257148465, 197.4644969978629 205.5355593904146, 196.4644897857004"
               L" 204.5355656446452, 196.4644628443994 204.535538703344, 195.6698695820492 203.5000093813459,"
               L" 195.1703643900373 202.2941032708183, 194.9999876022332 201.0000048109466, 194.9999876022332"
               L" 199.9999975945267, 195.1703643900373 198.705899134655), (294.9999814033497 299.9999918213907,"
               L" 295.1703581911539 298.705893361519, 295.6698499125153 297.500000721642, 296.464456645516"
               L" 296.4644579289933, 297.4999994337558 295.6698511926096, 298.7058920684988 295.1703594691217,"
               L" 299.9999905228609 294.9999826805921, 301.294088977223 295.1703594691217, 302.499981611966"
               L" 295.6698511926096, 303.5355244002059 296.4644579289933, 304.3301311332066 297.500000721642,"
               L" 304.4721387310111 297.7639177077956, 305.4721459431735 299.7639321406355, 305.8296300667304"
               L" 300.705907794359, 306.0000068545346 302.0000062542306, 305.8296300667304 303.2941047141023,"
               L" 305.330138345369 304.4999973539793, 304.5355316123684 305.535540146628, 303.4999888241284"
               L" 306.3301468830117, 302.2940961893854 306.8296386064997, 300.9999977350234 307.0000153950292,"
               L" 299.7058992806613 306.8296386064997, 298.5000066459183 306.3301468830117, 297.4644638576784"
               L" 305.535540146628, 296.464456645516 304.535532930208, 295.6698499125153 303.4999901375594,"
               L" 295.1703581911539 302.2940974976823, 294.9999814033497 300.9999990378107, 294.9999814033497"
               L" 299.9999918213907), (-9.83476697413721E-007 -5.000003367662645, 5.000001400709236 -5.000003367662645,"
               L" 6.294096487408668 -4.829633314458386, 7.500002592802202 -4.330131487982496, 8.535538645716825"
               L" -3.535538222249377, 9.330131908066983 -2.500002164925986, 9.829633732416257 -1.294096054398336,"
               L" 10.00000378489517 -9.621893273248361E-007, 10.00000378489517 3.000000481094663, 10.00000378489517"
               L" 5.00000144328399, 9.829633732416257 6.294096535492999, 9.330131908066983 7.500002646020649,"
               L" 8.535538645716825 8.53553870334404, 7.500002592802202 9.330131969077158, 6.294096487408668"
               L" 9.829633795553049, 5.000001400709236 10.00000384875731, 4.000000923872049 10.00000384875731,"
               L" -9.83476697413721E-007 10.00000384875731, -1.294096070176129 9.829633795553049, -2.500002175569664"
               L" 9.330131969077158, -3.535538228484286 8.53553870334404, -4.330131490834444 7.500002646020649,"
               L" -4.829633315183718 6.294096535492999, -5 5.00000144328399, -5 0.9999995189053363, -5"
               L" -9.621893273248361E-007, -4.829633315183718 -1.294096054398336, -4.330131490834444 -2.500002164925986,"
               L" -3.535538228484286 -3.535538222249377, -2.500002175569664 -4.330131487982496, -1.294096070176129"
               L" -4.829633314458386, -9.83476697413721E-007 -5.000003367662645))";
#ifndef DIFFERENT_LINUX_RESULT
        CPPUNIT_ASSERT(CheckGeometry(geom, base));
#endif
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_BufferArbitrary()
{
    try
    {
        MgWktReaderWriter readerWriter;
        Ptr<MgMeasure> measure = CreateMeasureArbitrary();

        Ptr<MgPoint> testPoint = CreatePoint();
        Ptr<MgGeometry> geom = testPoint->Buffer(5.0, measure);
        STRING base = L"POLYGON ((10 3, 9.975923596377587 3.490085727284638, 9.903926545615587 3.975451660210092,"
                      L" 9.784701751750813 4.451423490217149, 9.619397796356546 4.913417209794189, 9.409606421566393"
                      L" 5.356983853219015, 9.157347926935429 5.777851257068069, 8.865052393975997 6.171966752048345,"
                      L" 8.535533758135662 6.535533968869922, 8.17196656298438 6.865052624351099, 7.777851091495221"
                      L" 7.157348174732707, 7.356983712731821 7.409606684399449, 6.913417095745629 7.619398071694144,"
                      L" 6.451423403705562 7.784702036941296, 5.975451602068639 7.903926837912422, 5.49008569807325"
                      L" 7.975923892965781, 5 8.000000298023259, 4.509914301926751 7.975923892965781, 4.024548397931361"
                      L" 7.903926837912422, 3.548576596294438 7.784702036941296, 3.086582904254371 7.619398071694144,"
                      L" 2.643016287268179 7.409606684399449, 2.222148908504779 7.157348174732707, 1.828033437015619"
                      L" 6.865052624351099, 1.464466241864338 6.535533968869922, 1.134947606024003 6.171966752048345,"
                      L" 0.84265207306457 5.777851257068069, 0.5903935784336077 5.356983853219015, 0.3806022036434533"
                      L" 4.913417209794189, 0.215298248249188 4.451423490217149, 0.09607345438441363 3.975451660210092,"
                      L" 0.0240764036224129 3.490085727284638, 0 3, 0.0240764036224129 2.509914272715363, 0.09607345438441363"
                      L" 2.024548339789908, 0.215298248249188 1.548576509782852, 0.3806022036434533 1.086582790205811,"
                      L" 0.5903935784336077 0.6430161467809854, 0.84265207306457 0.2221487429319313, 1.134947606024003"
                      L" -0.1719667520483437, 1.464466241864338 -0.5355339688699208, 1.828033437015619 -0.865052624351099,"
                      L" 2.222148908504779 -1.157348174732706, 2.643016287268179 -1.409606684399448, 3.086582904254371"
                      L" -1.619398071694144, 3.548576596294438 -1.784702036941294, 4.024548397931361 -1.903926837912421,"
                      L" 4.509914301926751 -1.975923892965781, 5 -2.000000298023259, 5.49008569807325 -1.975923892965781,"
                      L" 5.975451602068639 -1.903926837912421, 6.451423403705562 -1.784702036941294, 6.913417095745629"
                      L" -1.619398071694144, 7.356983712731821 -1.409606684399448, 7.777851091495221 -1.157348174732706,"
                      L" 8.17196656298438 -0.865052624351099, 8.535533758135662 -0.5355339688699208, 8.865052393975997"
                      L" -0.1719667520483437, 9.157347926935429 0.2221487429319313, 9.409606421566393 0.6430161467809854,"
                      L" 9.619397796356546 1.086582790205811, 9.784701751750813 1.548576509782852, 9.903926545615587"
                      L" 2.024548339789908, 9.975923596377587 2.509914272715363, 10 3))";
        CPPUNIT_ASSERT(CheckGeometry(geom, base));
        STRING found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_BufferGeographic()
{
    try
    {
        MgWktReaderWriter readerWriter;
        Ptr<MgMeasure> measure = CreateMeasureGeographic();

        Ptr<MgPoint> testPoint = CreatePoint();
        Ptr<MgGeometry> geom = testPoint->Buffer(5.0, measure);
        STRING base = L"POLYGON ((4.999999999999929 78.3565782733653, -20.1417553509328 77.10228658941824, -37.94641244092062"
                      L" 73.91618906452526, -49.00707536977979 69.68840249638588, -55.94794130015023 64.94147371548101,"
                      L" -60.50526025922657 59.92107879174695, -63.62768075631146 54.74487253962428, -65.83573733781202"
                      L" 49.47364216728713, -67.4281875746362 44.14140047328478, -68.58493736892565 38.76857993228197,"
                      L" -69.41885885112636 33.36825780490134, -70.00298917311366 27.9493199526453, -70.38541736515865"
                      L" 22.51816433884672, -70.59781018482508 17.07968212123896, -70.66037599208212 11.63787157987018,"
                      L" -70.58477345614276 6.196235382108139, -70.37579002236069 0.7580835232834251, -70.03209054661338"
                      L" -4.673221549179739, -69.54632696785491 -10.09402264285358, -68.90456133512028 -15.50009900332677,"
                      L" -68.08502126246818 -20.88632105425847, -67.05588740443513 -26.24619069650062, -65.77175055573883"
                      L" -31.57113916529884, -64.1679945092198 -36.84948871725663, -62.15191257905439 -42.06476462659414,"
                      L" -59.58850738933727 -47.19291045581228, -56.27765037284198 -52.19752683610044, -51.91806148994336"
                      L" -57.02146482723532, -46.05493343203863 -61.57178885600435, -38.02553403529728 -65.69360788984393,"
                      L" -26.9956843254378 -69.13080789218043, -12.38732411785866 -71.49731284347926, 5.000000000000018"
                      L" -72.3565782733653, 22.38732471390514 -71.49731284347926, 36.99568253729835 -69.13080789218043,"
                      L" 48.02553463134376 -65.69360788984393, 56.05493402808511 -61.57178885600435, 61.91805731761797"
                      L" -57.02146482723532, 66.27765096888847 -52.19752683610044, 69.58850798538374 -47.19291045581228,"
                      L" 72.15191317510087 -42.06476462659414, 74.16799033689442 -36.84948871725663, 75.77175115178532"
                      L" -31.57113916529884, 77.0558880004816 -26.24619069650062, 78.08502185851467 -20.88632105425847,"
                      L" 78.90456193116677 -15.50009900332677, 79.5463275639014 -10.09402264285358, 80.03209114265985"
                      L" -4.673221549179739, 80.3757858500353 0.7580835232834251, 80.58476928381737 6.196235382108139,"
                      L" 80.66037181975673 11.63787157987018, 80.59781078087156 17.07968212123896, 80.38541796120512"
                      L" 22.51816433884672, 80.00298976916014 27.9493199526453, 79.41885944717285 33.36825780490134,"
                      L" 78.58493796497214 38.76857993228197, 77.42818817068267 44.14140047328478, 75.83573793385851"
                      L" 49.47364216728713, 73.62768135235794 54.74487253962428, 70.50526085527307 59.92107879174695,"
                      L" 65.94794189619671 64.94147371548101, 59.00707596582627 69.68840249638588, 47.94640826859524"
                      L" 73.91618906452526, 30.14175356279335 77.10228658941824, 4.999999999999929 78.3565782733653))";
        CPPUNIT_ASSERT(CheckGeometry(geom, base));
        STRING found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        Ptr<MgLineString> testLine = CreateLineString();
        geom = testLine->Buffer(5.0, measure);
        base = L"POLYGON ((43.59033063592497 -34.05103934419624, 44.65470753042147 -32.63883061871893, 44.66610966122804"
               L" -32.62406726170388, 44.7366286359208 -32.5324496665537, 45.82727115316815 -31.14630629376248,"
               L" 45.82955758747801 -31.14347245019346, 52.28583057438318 -20.52623886182771, 56.13651973822831"
               L" -9.01475238975911, 57.93014901460105 2.948745170086046, 57.84257023588241 15.04478395519065,"
               L" 55.66402558469925 26.96984552977628, 50.68134782799172 38.32110301507747, 41.44422331119915"
               L" 48.40566079684028, 25.88804691362661 55.91421472003635, 4 58.82612750841707, -17.88804774809168"
               L" 55.91421472003635, -33.44422414566422 48.40566079684028, -42.6794127034791 38.32419721176591,"
               L" -43.59759465441672 36.89445804291463, -44.55622145868668 35.4880605325771, -50.16851646712521"
               L" 24.44719236459641, -53.04983759223447 12.6617927148095, -53.83028607548988 0.5902180481216961,"
               L" -52.70913551504228 -11.45472341236155, -49.49431875314228 -23.1570153423566, -43.56598177945505"
               L" -34.08412087966453, -33.82480030207636 -43.52521926465264, -19.08789837884297 -50.29143634932474,"
               L" 0 -52.82612750841707, 19.08789921330805 -50.29143634932474, 33.82480280547159 -43.52521926465264,"
               L" 43.56598094498997 -34.08412087966453, 43.59033063592497 -34.05103934419624))";
#ifndef DIFFERENT_LINUX_RESULT
        CPPUNIT_ASSERT(CheckGeometry(geom, base));
#endif
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

/* TODO: This test fails because of a bug in borderwalker
        Ptr<MgPolygon> testPolygon = CreatePolygon();
        geom = testPolygon->Buffer(5.0, measure);
        base = L"POLYGON ((0 -50.46350991290926, 5 -50.46350991290926, 22.31251939013716 -48.16484220800903,"
               L" 36.04466325310846 -41.93738871066436, 45.39020242632642 -33.09474892553674, 51.16175151835391"
               L" -22.72714617575958, 54.26214571369563 -11.54157239694266, 55.23742915615017 7.450581485102248E-008,"
               L" 55.34360530040296 3.18439851291758, 55.1470816223074 14.76409462262328, 52.81053053203407"
               L" 26.12451179319761, 47.69203500104159 36.81988767622563, 38.61214540077123 46.13994284152303,"
               L" 24.2009269714908 52.88466219719198, 5.294587927734133 55.43114101065886, -0.2945879277341321"
               L" 55.43114101065886, -19.20092607742107 52.88466219719198, -33.6121445067015 46.13994284152303,"
               L" -42.69203768325077 36.81988767622563, -47.81052963796434 26.12451179319761, -50.14708072823768"
               L" 14.76409462262328, -50.34360440633324 3.18439851291758, -50.23742826208045 7.450581485102248E-008,"
               L" -49.2621448196259 -11.54157239694266, -46.16175420056309 -22.72714617575958, -40.3902015322567"
               L" -33.09474892553674, -31.04466593531763 -41.93738871066436, -17.31251849606743 -48.16484220800903,"
               L" 0 -50.46350991290926))";
        CPPUNIT_ASSERT(CheckGeometry(geom, base));
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));
*/

        Ptr<MgCurveString> testCurveString = CreateCurveString(0.0);
        geom = testCurveString->Buffer(5.0, measure);
        base = L"POLYGON ((39.95566636059679 -45.83775584922766, 40.33457311001856 -45.66001526475043, 50.41059466663567"
               L" -35.49034097733146, 56.4178489099651 -23.89740305527467, 59.70675329606254 -11.58176675926249,"
               L" 60.98229753865584 1.060672225321664, 60.46188267838255 13.75605379081093, 57.93560212466729"
               L" 26.23938150640616, 52.65519795746791 38.14021331907591, 43.03798365819357 48.78369926117058,"
               L" 26.629294373351 56.81057452026454, 3.059117588944291 59.96658104458935, 0.9408823626269323"
               L" 59.96658104458935, -22.629294373351 56.81057452026454, -39.03798210847271 48.78369926117058,"
               L" -48.65313527900787 38.14353592059231, -49.07684445839193 37.47242000534654, -54.53822192777526"
               L" 25.66203765046821, -57.25992228149904 13.21669746736869, -57.97052508416921 0.5303941908713696,"
               L" -57.96660119096048 0, -57.06677747170791 -12.67444084134345, -54.15289313512404 -25.07913029665532,"
               L" -48.49613925791617 -36.82994758069203, -38.62941060241524 -47.23658348539969, -22.47293683725219"
               L" -54.97013997853637, 0 -57.96659964123962, 22.47293683725219 -54.97013997853637, 38.6294121521361"
               L" -47.23658348539969, 39.95566636059679 -45.83775584922766))";
#ifndef DIFFERENT_LINUX_RESULT
        CPPUNIT_ASSERT(CheckGeometry(geom, base));
#endif
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        Ptr<MgCurvePolygon> testCurvePolygon = CreateCurvePolygon(0.0, 10);
        geom = testCurvePolygon->Buffer(1.0, measure);
        base = L"POLYGON ((-22.4498915592657 -9.412192488443402, -16.83059173116552 -16.75034485759407, -9.327597908591891"
               L" -22.57309148151228, -0.1690891057791877 -26.3591138934561, 10.00000000993411 -27.67829350699743,"
               L" 20.16908990050791 -26.3591138934561, 29.32759774964615 -22.57309148151228, 36.83059157221978"
               L" -16.75034485759407, 41.17602454440189 -11.07571721528275, 45.86312283883458 -7.784312294978637,"
               L" 51.79035080120576 -1.141241678073506, 55.61374999900659 1.227877153627703, 62.27230894920173"
               L" 7.530758531734856, 64.97702129316667 10.91670624415316, 66.02167630540153 12.30182524334343,"
               L" 69.02406859780447 16.5737004025996, 73.32284781501258 24.79717539532038, 76.14788335925474"
               L" 33.6120162374983, 77.10854635374489 42.7329636653044, 75.34759026330157 51.80734889551096,"
               L" 69.03428435870558 60.26303406137431, 54.82086886101863 66.93998300671476, 30.99999944368989"
               L" 69.6782911228115, 7.179130264779769 66.93998300671476, -7.034285471325791 60.26303406137431,"
               L" -13.34759328327072 51.80734889551096, -14.20956432176659 49.24628306903142, -14.49272553010393"
               L" 48.2231163515518, -14.93014549098738 45.4088804965544, -16.0786079690628 44.20405788445818,"
               L" -16.69676259307267 43.14552510652096, -17.25134718439534 42.14441115524836, -18.65962873452961"
               L" 38.40821632195808, -21.27645234687365 35.96642273464339, -21.46314366072713 35.71497694939238,"
               L" -22.16240448354157 34.76539002450645, -26.37626471142208 26.80884944253263, -28.33162543481596"
               L" 17.83684258680598, -28.19345326319587 8.685854177823911, -28.103418764022 7.899433308806021,"
               L" -26.19247638264916 -1.051959636924245, -22.4498915592657 -9.412192488443402))";
#ifndef DIFFERENT_LINUX_RESULT
        CPPUNIT_ASSERT(CheckGeometry(geom, base));
#endif
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        // TODO: write more tests
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_BufferProjected()
{
    try
    {
        MgWktReaderWriter readerWriter;
        Ptr<MgMeasure> measure = CreateMeasureProjected();

        // TODO: Commented out because these tests fail.  Need to investigate why
        //Ptr<MgPoint> testPoint = CreatePoint();
        //Ptr<MgGeometry> geom = testPoint->Buffer(5.0, measure);
        //STRING found = readerWriter.Write(geom);
        //ACE_DEBUG((LM_INFO, ACE_TEXT(found.c_str())));

        //Ptr<MgLineString> testLine = CreateLineString();
        //CPPUNIT_ASSERT_THROW_MG(testLine->Buffer(5.0, measure), BorderWalkerException*);

        //Ptr<MgPolygon> testPolygon = CreatePolygon();
        //CPPUNIT_ASSERT_THROW_MG(testPolygon->Buffer(5.0, measure), BorderWalkerException*);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_Intersection()
{
    try
    {
        MgWktReaderWriter readerWriter;
        MgGeometryFactory factory;

        //TEST 1
        Ptr<MgCoordinate> coord = factory.CreateCoordinateXY(100.0, 100.0);
        Ptr<MgPoint> point = new MgPoint(coord);
        Ptr<MgPolygon> polygon = CreatePolygon();
        Ptr<MgGeometry> geom = point->Intersection(polygon);
        CPPUNIT_ASSERT(geom == NULL);

        //TEST 2
        coord = factory.CreateCoordinateXY(100.0, 100.0);
        point = factory.CreatePoint(coord);
        Ptr<MgCoordinateCollection> outerCoords = new MgCoordinateCollection();
        Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXY( 45.0,  45.0);
        Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXY( 45.0, -45.0);
        Ptr<MgCoordinate> coord3 = factory.CreateCoordinateXY(-45.0, -45.0);
        Ptr<MgCoordinate> coord4 = factory.CreateCoordinateXY(-45.0,  45.0);
        Ptr<MgCoordinate> coord5 = factory.CreateCoordinateXY( 45.0,  45.0);
        outerCoords->Add(coord1);
        outerCoords->Add(coord2);
        outerCoords->Add(coord3);
        outerCoords->Add(coord4);
        outerCoords->Add(coord5);
        Ptr<MgLinearRing> outerRing = factory.CreateLinearRing(outerCoords);
        Ptr<MgLinearRingCollection> innerRings;
        polygon = factory.CreatePolygon(outerRing, innerRings);
        geom = point->Intersection(polygon);
        CPPUNIT_ASSERT(geom == NULL);

        //TEST 3
        point = CreatePoint();
        polygon = CreatePolygon();
        geom = point->Intersection(polygon);
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POINT (5 3)") == 0);

        //TEST 4
        Ptr<MgLineString> lineString = CreateLineString();
        geom = lineString->Intersection(point);
        CPPUNIT_ASSERT(geom == NULL);

        //TEST 5
        polygon = CreatePolygon();
        geom = polygon->Intersection(lineString);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"LINESTRING (0 1, 2 3, 4 5)") == 0);

        //TEST 6
        Ptr<MgCurveString> curveString = CreateCurveString(0.0);
        geom = curveString->Intersection(polygon);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"GEOMETRYCOLLECTION", ::wcslen(L"GEOMETRYCOLLECTION")));
        CPPUNIT_ASSERT(wstring::npos != found.find(L"POINT"));
        CPPUNIT_ASSERT(wstring::npos != found.find(L"LINESTRING"));

        //TEST 7
        Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        geom = curvePolygon->Intersection(curveString);
        CPPUNIT_ASSERT(geom == NULL);

        //TEST 8
        Ptr<MgMultiPoint> multiPoint = CreateMultiPoint();
        geom = multiPoint->Intersection(point);
        CPPUNIT_ASSERT(geom == NULL);

        //TEST 9
        Ptr<MgMultiLineString> multiLineString = CreateMultiLineString();
        geom = multiLineString->Intersection(multiPoint);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT (1 2, 4 5)") == 0);

        //TEST 10
        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        geom = multiPolygon->Intersection(multiLineString);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"LINESTRING (0 1, 3 4, 4 5)") == 0);

        //TEST 11
        Ptr<MgMultiCurveString> multiCurveString = CreateMultiCurveString();
        geom = multiCurveString->Intersection(multiPolygon);
        CPPUNIT_ASSERT(geom == NULL);

        //TEST 12 TODO: commented out due to parse exception
        //Ptr<MgMultiCurvePolygon> multiCurvePolygon = CreateMultiCurvePolygon(3, 0.0);
        //geom = multiCurvePolygon->Intersection(curvePolygon);
        //CPPUNIT_ASSERT(geom == NULL);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_Boundary()
{
    try
    {
        MgWktReaderWriter readerWriter;

        //TEST 1
        Ptr<MgPoint> point = CreatePoint();
        Ptr<MgGeometry> geom = point->Boundary();
        CPPUNIT_ASSERT(geom == NULL);

        //TEST 2
        Ptr<MgLineString> lineString = CreateLineString();
        geom = lineString->Boundary();
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT (0 1, 4 5)") == 0);

        //TEST 3
        Ptr<MgPolygon> polygon = CreatePolygon();
        geom = polygon->Boundary();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTILINESTRING ((0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3))") == 0);

        //TEST 4
        Ptr<MgCurveString> curveString = CreateCurveString(0.0);
        geom = curveString->Boundary();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT (0 0, 3 2)") == 0);

        //TEST 5
        Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        geom = curvePolygon->Boundary();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"MULTILINESTRING", ::wcslen(L"MULTILINESTRING")));

        //TEST 6
        Ptr<MgMultiPoint> multiPoint = CreateMultiPoint();
        geom = multiPoint->Boundary();
        CPPUNIT_ASSERT(geom == NULL);

        //TEST 7
        Ptr<MgMultiLineString> multiLineString = CreateMultiLineString();
        geom = multiLineString->Boundary();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT (0 1, 6 7, 9 10, 15 16)") == 0);

        //TEST 8
        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        geom = multiPolygon->Boundary();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTILINESTRING ((0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3), (0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3))") == 0);

        //TEST 9
        Ptr<MgMultiCurveString> multiCurveString = CreateMultiCurveString();
        geom = multiCurveString->Boundary();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT (100 100, 103 102, 200 200, 203 202, 300 300, 303 302)") == 0);

        //TEST 10
        Ptr<MgMultiCurvePolygon> multiCurvePolygon = CreateMultiCurvePolygon(3, 0.0);
        geom = multiCurvePolygon->Boundary();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"MULTILINESTRING", ::wcslen(L"MULTILINESTRING")));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_ConvexHull()
{
    try
    {
        MgWktReaderWriter readerWriter;

        //TEST 1
        Ptr<MgPoint> point = CreatePoint();
        Ptr<MgGeometry> geom = point->ConvexHull();
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POINT (5 3)") == 0);

        //TEST 2
        Ptr<MgLineString> lineString = CreateLineString();
        geom = lineString->ConvexHull();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"LINESTRING (0 1, 4 5)") == 0);

        //TEST 3
        Ptr<MgPolygon> polygon = CreatePolygon();
        geom = polygon->ConvexHull();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON ((0 0, 0 5, 5 5, 5 0, 0 0))") == 0);

        //TEST 4
        Ptr<MgCurveString> curveString = CreateCurveString(0.0);
        geom = curveString->ConvexHull();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        //TEST 5
        Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        geom = curvePolygon->ConvexHull();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        //TEST 6
        Ptr<MgMultiPoint> multiPoint = CreateMultiPoint();
        geom = multiPoint->ConvexHull();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"LINESTRING (1 2, 7 8)") == 0);

        //TEST 7
        Ptr<MgMultiLineString> multiLineString = CreateMultiLineString();
        geom = multiLineString->ConvexHull();
        found = readerWriter.Write(geom);

        // The reason for the different check below is because we get slightly
        // different results with rounding under Debug and Release builds.
#ifdef _WIN32
    #ifdef _DEBUG
            CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"LINESTRING (0 1, 15 16)") == 0);
    #else
            CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON ((0 1, 15 16, 3 4, 12 13, 0 1))") == 0);
    #endif
#else
            CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"LINESTRING (0 1, 15 16)") == 0);
#endif

        //TEST 8
        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        geom = multiPolygon->ConvexHull();
        found = readerWriter.Write(geom);

        // The reason for the different check below is because we get slightly
        // different results with rounding under Debug and Release builds.
#ifdef _WIN32
    #ifdef _DEBUG
            CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON ((0 0, 0 5, 5 5, 5 0, 0 0))") == 0);
    #else
            CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON ((0 0, 0 5, 5 5, 1 1, 2 2, 4 3, 5 0, 0 0))") == 0);
    #endif
#else
            CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON ((0 0, 0 5, 5 5, 5 0, 0 0))") == 0);
#endif

        //TEST 9
        Ptr<MgMultiCurveString> multiCurveString = CreateMultiCurveString();
        geom = multiCurveString->ConvexHull();
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON ((100 100, 100 101, 301 302, 303 302, 303 300, 103 100, 100 100))") == 0);

        //TEST 10
        Ptr<MgMultiCurvePolygon> multiCurvePolygon = CreateMultiCurvePolygon(3, 0.0);
        geom = multiCurvePolygon->ConvexHull();
        found = readerWriter.Write(geom);
        // Note that results from release and debug builds are slightly different.
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        //TEST 11
        Ptr<MgMultiGeometry> multiGeometry = CreateMultiGeometry();
        geom = multiGeometry->ConvexHull();
        found = readerWriter.Write(geom);
        // Note that results from release and debug builds are slightly different.
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_Difference()
{
    try
    {
        MgWktReaderWriter readerWriter;
        MgGeometryFactory factory;

        //TEST 1
        Ptr<MgPoint> point = CreatePoint();
        Ptr<MgCoordinate> coord = factory.CreateCoordinateXY(4.0, 8.0);
        Ptr<MgPoint> pointCoord = factory.CreatePoint(coord);
        Ptr<MgGeometry> geom = point->Difference(pointCoord);
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POINT (5 3)") == 0);

        //TEST 2
        Ptr<MgLineString> lineString = CreateLineString();
        geom = lineString->Difference(point);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"LINESTRING (0 1, 2 3, 4 5)") == 0);

        //TEST 3
        Ptr<MgPolygon> polygon = CreatePolygon();
        geom = polygon->Difference(lineString);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON ((4 5, 5 5, 5 0, 0 0, 0 1, 0 5, 4 5), (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3))") == 0);

        //TEST 4
        Ptr<MgCurveString> curveString = CreateCurveString(0.0);
        geom = curveString->Difference(polygon);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"MULTILINESTRING", ::wcslen(L"MULTILINESTRING")));

        //TEST 5 TODO: commented out due to parse exception
        //Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        //CPPUNIT_ASSERT_THROW_MG(curvePolygon->Difference(curveString), MgGeometryException*);

        //TEST 6
        Ptr<MgMultiPoint> multiPoint = CreateMultiPoint();
        geom = multiPoint->Difference(point);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT (1 2, 4 5, 7 8)") == 0);

        //TEST 7
        Ptr<MgMultiLineString> multiLineString = CreateMultiLineString();
        geom = multiLineString->Difference(multiPoint);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTILINESTRING ((0 1, 3 4, 6 7), (9 10, 12 13, 15 16))") == 0);

        //TEST 8
        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        geom = multiPolygon->Difference(multiLineString);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON ((5 0, 0 0, 0 1, 0 5, 4 5, 5 5, 5 0), (1 1, 2 1, 2 2, 1 1),"
                                               L" (3 3, 4 3, 4 4, 3 3))") == 0);

        //TEST 9
        Ptr<MgMultiCurveString> multiCurveString = CreateMultiCurveString();
        geom = multiCurveString->Difference(multiPolygon);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTILINESTRING ((100 100, 100 101, 101 102, 103 100, 103 102),"
                                               L" (200 200, 200 201, 201 202, 203 200, 203 202), (300 300, 300 301,"
                                               L" 301 302, 303 300, 303 302))") == 0);

        //TEST 10 TODO: commented out due to parse exception
        //Ptr<MgMultiCurvePolygon> multiCurvePolygon = CreateMultiCurvePolygon(3, 0.0);
        //CPPUNIT_ASSERT_THROW_MG(multiCurvePolygon->Difference(curvePolygon), MgGeometryException*);

        //TEST 11 TODO: commented out due to parse exception
        //Ptr<MgMultiGeometry> multiGeometry = CreateMultiGeometry();
        //geom = multiGeometry->Difference(polygon);
        //CPPUNIT_ASSERT_THROW_MG(multiGeometry->Difference(multiCurvePolygon), MgGeometryException*);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_SymetricDifference()
{
    try
    {
        MgWktReaderWriter readerWriter;
        MgGeometryFactory factory;

        //TEST 1
        Ptr<MgPoint> point = CreatePoint();
        Ptr<MgCoordinate> coord = factory.CreateCoordinateXY(4.0, 8.0);
        Ptr<MgPoint> pointCoord = factory.CreatePoint(coord);
        Ptr<MgGeometry> geom = point->SymetricDifference(pointCoord);
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT (4 8, 5 3)") == 0);

        //TEST 2
        Ptr<MgLineString> lineString = CreateLineString();
        geom = lineString->SymetricDifference(point);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"GEOMETRYCOLLECTION (POINT (5 3), LINESTRING (0 1, 2 3, 4 5))") == 0);

        //TEST 3
        Ptr<MgPolygon> polygon = CreatePolygon();
        geom = polygon->SymetricDifference(lineString);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON ((4 5, 5 5, 5 0, 0 0, 0 1, 0 5, 4 5), (1 1, 2 1, 2 2, 1 1),"
                                               L" (3 3, 4 3, 4 4, 3 3))") == 0);

        //TEST 4
        Ptr<MgCurveString> curveString = CreateCurveString(0.0);
        geom = curveString->SymetricDifference(polygon);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"GEOMETRYCOLLECTION", ::wcslen(L"GEOMETRYCOLLECTION")));
        CPPUNIT_ASSERT(wstring::npos != found.find(L"LINESTRING"));

        //TEST 5 TODO: commented out due to parse exception
        //Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        //CPPUNIT_ASSERT_THROW_MG(curvePolygon->SymetricDifference(pointCoord), MgGeometryException*);

        //TEST 6
        Ptr<MgMultiPoint> multiPoint = CreateMultiPoint();
        geom = multiPoint->SymetricDifference(point);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT (1 2, 4 5, 5 3, 7 8)") == 0);

        //TEST 7
        Ptr<MgMultiLineString> multiLineString = CreateMultiLineString();
        geom = multiLineString->SymetricDifference(multiPoint);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"GEOMETRYCOLLECTION (POINT (7 8), LINESTRING (0 1, 3 4, 6 7, 9 10, 12 13, 15 16))") == 0);

        //TEST 8
        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        geom = multiPolygon->SymetricDifference(multiLineString);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"GEOMETRYCOLLECTION (LINESTRING (4 5, 6 7, 9 10, 12 13, 15 16),"
                                               L" POLYGON ((5 0, 0 0, 0 1, 0 5, 4 5, 5 5, 5 0), (1 1, 2 1, 2 2, 1 1),"
                                               L" (3 3, 4 3, 4 4, 3 3)))") == 0);

        //TEST 9
        Ptr<MgMultiCurveString> multiCurveString = CreateMultiCurveString();
        geom = multiCurveString->SymetricDifference(multiPoint);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"GEOMETRYCOLLECTION (POINT (1 2), POINT (4 5), POINT (7 8),"
                                               L" LINESTRING (100 100, 100 101, 101 102, 103 100, 103 102, 200 200, 200 201,"
                                               L" 201 202, 203 200, 203 202, 300 300, 300 301, 301 302, 303 300, 303 302))") == 0);

        //TEST 10 TODO: commented out due to parse exception
        //Ptr<MgMultiCurvePolygon> multiCurvePolygon = CreateMultiCurvePolygon(3, 0.0);
        //CPPUNIT_ASSERT_THROW_MG(multiCurvePolygon->SymetricDifference(curvePolygon), MgGeometryException*);

        //TEST 11 TODO: commented out due to parse exception
        //Ptr<MgMultiGeometry> multiGeometry = CreateMultiGeometry();
        //CPPUNIT_ASSERT_THROW_MG(multiGeometry->SymetricDifference(multiCurvePolygon), MgGeometryException*);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_Union()
{
    try
    {
        MgWktReaderWriter readerWriter;
        MgGeometryFactory factory;

        //TEST 1
        Ptr<MgPoint> point = CreatePoint();
        Ptr<MgCoordinate> coord = factory.CreateCoordinateXY(4.0, 8.0);
        Ptr<MgPoint> pointCoord = factory.CreatePoint(coord);
        Ptr<MgGeometry> geom = point->Union(pointCoord);
        STRING found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT (5 3, 4 8)") == 0);

        //TEST 2
        Ptr<MgLineString> lineString = CreateLineString();
        geom = lineString->Union(point);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"GEOMETRYCOLLECTION (LINESTRING (0 1, 2 3, 4 5), POINT (5 3))") == 0);

        //TEST 3
        Ptr<MgPolygon> polygon = CreatePolygon();
        geom = polygon->Union(lineString);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON ((4 5, 5 5, 5 0, 0 0, 0 1, 0 5, 4 5), (1 1, 2 1, 2 2, 1 1),"
                                               L" (3 3, 4 3, 4 4, 3 3))") == 0);

        //TEST 4
        Ptr<MgCurveString> curveString = CreateCurveString(0.0);
        geom = curveString->Union(polygon);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"GEOMETRYCOLLECTION", ::wcslen(L"GEOMETRYCOLLECTION")));
        CPPUNIT_ASSERT(wstring::npos != found.find(L"LINESTRING"));

        //TEST 5
        //Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        //geom = curvePolygon->Union(curveString);
        //CPPUNIT_ASSERT_THROW_MG(curvePolygon->Union(curveString), MgGeometryException*);

        //TEST 6
        Ptr<MgMultiPoint> multiPoint = CreateMultiPoint();
        geom = multiPoint->Union(point);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT (1 2, 4 5, 5 3, 7 8)") == 0);

        //TEST 7
        Ptr<MgMultiLineString> multiLineString = CreateMultiLineString();
        geom = multiLineString->Union(multiPoint);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"GEOMETRYCOLLECTION (POINT (7 8), LINESTRING (0 1, 3 4, 6 7, 9 10, 12 13, 15 16))") == 0);

        //TEST 8
        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        geom = multiPolygon->Union(multiLineString);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"GEOMETRYCOLLECTION (LINESTRING (4 5, 6 7, 9 10, 12 13, 15 16),"
                                               L" POLYGON ((5 0, 0 0, 0 1, 0 5, 4 5, 5 5, 5 0), (1 1, 2 1, 2 2, 1 1),"
                                               L" (3 3, 4 3, 4 4, 3 3)))") == 0);

        //TEST 9
        Ptr<MgMultiCurveString> multiCurveString = CreateMultiCurveString();
        geom = multiCurveString->Union(multiPolygon);
        found = readerWriter.Write(geom);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"GEOMETRYCOLLECTION (LINESTRING (100 100, 100 101, 101 102, 103 100,"
                                               L" 103 102, 200 200, 200 201, 201 202, 203 200, 203 202, 300 300, 300 301,"
                                               L" 301 302, 303 300, 303 302), POLYGON ((0 0, 5 0, 5 5, 0 5, 0 0),"
                                               L" (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3, 0 0, 5 0, 5 5, 0 5, 0 0),"
                                               L" (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3)))") == 0);

        //TEST 10
        //Ptr<MgMultiCurvePolygon> multiCurvePolygon = CreateMultiCurvePolygon(3, 0.0);
        //CPPUNIT_ASSERT_THROW_MG(multiCurvePolygon->Union(curvePolygon), MgGeometryException*);

        //TEST 11
        //Ptr<MgMultiGeometry> multiGeometry = CreateMultiGeometry();
        //CPPUNIT_ASSERT_THROW_MG(multiGeometry->Union(multiCurvePolygon), MgGeometryException*);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_Distance()
{
    try
    {
        Ptr<MgPoint> point1 = CreatePoint();
        Ptr<MgPoint> point2 = CreatePoint();
        Ptr<MgCoordinateSystemMeasure> measure = CreateMeasureArbitrary();
        Ptr<MgCoordinateSystem> coordinateSystem = measure->GetCoordSys();
        double dist = point1->Distance(point2, measure);
        CPPUNIT_ASSERT(dist == 0.0);

        Ptr<MgLineString> lineString = CreateLineString();
        dist = lineString->Distance(point1, measure);
        CPPUNIT_ASSERT(dist == 2.2360679774997898);

        double meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(dist);
        CPPUNIT_ASSERT(meters == 3598.6025831814218);

        Ptr<MgPolygon> polygon = CreatePolygon();
        dist = polygon->Distance(lineString, measure);
        CPPUNIT_ASSERT(dist == 1.0);

        meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(dist);
        CPPUNIT_ASSERT(meters == 1609.3440000000001);

        Ptr<MgCurveString> curveString = CreateCurveString(0.0);
        dist = curveString->Distance(polygon, measure);
        CPPUNIT_ASSERT(dist == 0.0);

        meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(dist);
        CPPUNIT_ASSERT(meters == 0.0);

        Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        dist = curvePolygon->Distance(lineString, measure);
        CPPUNIT_ASSERT(dist == 135.05924625881784);

        meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(dist);
        CPPUNIT_ASSERT(meters == 217356.78761115094);

        Ptr<MgMultiPoint> multiPoint = CreateMultiPoint();
        dist = multiPoint->Distance(lineString, measure);
        CPPUNIT_ASSERT(dist == 0.0);

        meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(dist);
        CPPUNIT_ASSERT(meters == 0.0);

        Ptr<MgMultiLineString> multiLineString = CreateMultiLineString();
        dist = multiLineString->Distance(multiPoint, measure);
        CPPUNIT_ASSERT(dist == 1.4142135623730951);

        meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(dist);
        CPPUNIT_ASSERT(meters == 2275.9561113237664);

        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        dist = multiPolygon->Distance(multiLineString, measure);
        CPPUNIT_ASSERT(dist == 1.0);

        meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(dist);
        CPPUNIT_ASSERT(meters == 1609.3440000000001);

        Ptr<MgMultiCurveString> multiCurveString = CreateMultiCurveString();
        dist = multiCurveString->Distance(lineString, measure);
        CPPUNIT_ASSERT(dist == 135.05924625881784);

        meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(dist);
        CPPUNIT_ASSERT(meters == 217356.78761115094);

        Ptr<MgMultiCurvePolygon> multiCurvePolygon = CreateMultiCurvePolygon(2, 0.0);
        dist = multiCurvePolygon->Distance(lineString, measure);
        CPPUNIT_ASSERT(dist == 135.05924625881784);

        meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(dist);
        CPPUNIT_ASSERT(meters == 217356.78761115094);

        Ptr<MgMultiGeometry> multiGeometry = CreateMultiGeometry();
        dist = multiGeometry->Distance(lineString, measure);
        CPPUNIT_ASSERT(dist == 0.0);

        meters = coordinateSystem->ConvertCoordinateSystemUnitsToMeters(dist);
        CPPUNIT_ASSERT(meters == 0.0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_Transform()
{
    try
    {
        MgWktReaderWriter readerWriter;
        Ptr<MgCoordinateSystem> coordSysArbitrary = CreateCoordinateSystemArbitrary();
        Ptr<MgCoordinateSystem> coordSysGeographic = CreateCoordinateSystemGeographic();
        Ptr<MgCoordinateSystem> coordSysProjected = CreateCoordinateSystemProjected();

        Ptr<MgTransform> transform = new MgCoordinateSystemTransform(coordSysArbitrary, coordSysArbitrary);
        Ptr<MgPoint> point = CreatePoint();
        Ptr<MgGeometricEntity> geom = point->Transform(transform);
        STRING found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POINT XYZ (5 3 2)") == 0);

        Ptr<MgLineString> lineString = CreateLineString();
        geom = lineString->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"LINESTRING (0 1, 2 3, 4 5)") == 0);

        Ptr<MgPolygon> polygon = CreatePolygon();
        geom = polygon->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POLYGON (0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3))"));

        Ptr<MgCurveString> curveString = CreateCurveString(0.0);
        geom = curveString->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"CURVESTRING (0 0 (CIRCULARARCSEGMENT (0 1, 1 2), LINESTRINGSEGMENT (3 0, 3 2)))") == 0);

        Ptr<MgCurveRing> curveRing = CreateCurveRing(0.0);
        geom = curveRing->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"(0 0 (CIRCULARARCSEGMENT (0 1, 1 2), LINESTRINGSEGMENT (0 0)))") == 0);

        Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        geom = curvePolygon->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"CURVEPOLYGON ((100 100 (CIRCULARARCSEGMENT (100 101, 101 102), LINESTRINGSEGMENT (100 100))),"
                                               L" (200 200 (CIRCULARARCSEGMENT (200 201, 201 202), LINESTRINGSEGMENT (200 200))),"
                                               L" (300 300 (CIRCULARARCSEGMENT (300 301, 301 302), LINESTRINGSEGMENT (300 300))))") == 0);

        Ptr<MgMultiPoint> multiPoint = CreateMultiPoint();
        geom = multiPoint->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOINT XYZ (1 2 3, 4 5 6, 7 8 9)") == 0);

        Ptr<MgMultiLineString> multiLineString = CreateMultiLineString();
        geom = multiLineString->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTILINESTRING XYZ ((0 1 2, 3 4 5, 6 7 8), (9 10 11, 12 13 14, 15 16 17))") == 0);

        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        geom = multiPolygon->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTIPOLYGON (((0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1),"
                                               L" (3 3, 4 3, 4 4, 3 3)), ((0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1),"
                                               L" (3 3, 4 3, 4 4, 3 3)))") == 0);

        Ptr<MgMultiCurveString> multiCurveString = CreateMultiCurveString();
        geom = multiCurveString->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTICURVESTRING ((100 100 (CIRCULARARCSEGMENT (100 101, 101 102), LINESTRINGSEGMENT (103 100, 103 102))),"
                                               L" (200 200 (CIRCULARARCSEGMENT (200 201, 201 202), LINESTRINGSEGMENT (203 200, 203 202))),"
                                               L" (300 300 (CIRCULARARCSEGMENT (300 301, 301 302), LINESTRINGSEGMENT (303 300, 303 302))))") == 0);

        Ptr<MgMultiCurvePolygon> multiCurvePolygon = CreateMultiCurvePolygon(3, 0.0);
        geom = multiCurvePolygon->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"MULTICURVEPOLYGON (((100 100 (CIRCULARARCSEGMENT (100 101, 101 102), LINESTRINGSEGMENT (100 100))),"
                                               L" (200 200 (CIRCULARARCSEGMENT (200 201, 201 202), LINESTRINGSEGMENT (200 200))),"
                                               L" (300 300 (CIRCULARARCSEGMENT (300 301, 301 302), LINESTRINGSEGMENT (300 300)))),"
                                               L" ((101 101 (CIRCULARARCSEGMENT (101 102, 102 103), LINESTRINGSEGMENT (101 101))),"
                                               L" (201 201 (CIRCULARARCSEGMENT (201 202, 202 203), LINESTRINGSEGMENT (201 201))),"
                                               L" (301 301 (CIRCULARARCSEGMENT (301 302, 302 303), LINESTRINGSEGMENT (301 301)))),"
                                               L" ((102 102 (CIRCULARARCSEGMENT (102 103, 103 104), LINESTRINGSEGMENT (102 102))),"
                                               L" (202 202 (CIRCULARARCSEGMENT (202 203, 203 204), LINESTRINGSEGMENT (202 202))),"
                                               L" (302 302 (CIRCULARARCSEGMENT (302 303, 303 304), LINESTRINGSEGMENT (302 302)))))") == 0);

        Ptr<MgMultiGeometry> multiGeometry = CreateMultiGeometry();
        geom = multiGeometry->Transform(transform);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"GEOMETRYCOLLECTION (CURVEPOLYGON ((100 100 (CIRCULARARCSEGMENT (100 101, 101 102),"
                                               L" LINESTRINGSEGMENT (100 100))), (200 200 (CIRCULARARCSEGMENT (200 201, 201 202),"
                                               L" LINESTRINGSEGMENT (200 200))), (300 300 (CIRCULARARCSEGMENT (300 301, 301 302),"
                                               L" LINESTRINGSEGMENT (300 300)))), CURVESTRING (100 100 (CIRCULARARCSEGMENT (100 101, 101 102),"
                                               L" LINESTRINGSEGMENT (103 100, 103 102))), LINESTRING (0 1, 2 3, 4 5), POINT XYZ (5 3 2),"
                                               L" POLYGON ((0 0, 5 0, 5 5, 0 5, 0 0), (1 1, 2 1, 2 2, 1 1), (3 3, 4 3, 4 4, 3 3)))") == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_Touches()
{
    try
    {
        //TEST 1
        Ptr<MgPoint> point = CreatePoint();
        Ptr<MgPolygon> polygon = CreatePolygon();
        bool touches = point->Touches(polygon);
        CPPUNIT_ASSERT(touches);
        touches = polygon->Touches(point);
        CPPUNIT_ASSERT(touches);

        //TEST 2
        MgGeometryFactory factory;
        Ptr<MgCoordinate> coord = factory.CreateCoordinateXY(100.0, 100.0);
        point = new MgPoint(coord);
        touches = point->Touches(polygon);
        CPPUNIT_ASSERT(!touches);
        touches = polygon->Touches(point);
        CPPUNIT_ASSERT(!touches);

        //TEST 3
        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        point = CreatePoint();
        touches = point->Touches(multiPolygon);
        CPPUNIT_ASSERT(!touches);
        touches = multiPolygon->Touches(point);
        CPPUNIT_ASSERT(!touches);

        //TEST 4
        Ptr<MgCoordinateCollection> collection = new MgCoordinateCollection();
        Ptr<MgCoordinate> coord1 = new MgCoordinateXY( 60.0, 120.0);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXY( 60.0,  40.0);
        Ptr<MgCoordinate> coord3 = new MgCoordinateXY(160.0,  40.0);
        Ptr<MgCoordinate> coord4 = new MgCoordinateXY(160.0, 120.0);
        Ptr<MgCoordinate> coord5 = new MgCoordinateXY( 60.0, 120.0);
        collection->Add(coord1);
        collection->Add(coord2);
        collection->Add(coord3);
        collection->Add(coord4);
        collection->Add(coord5);
        Ptr<MgLinearRing> linearRing1 = factory.CreateLinearRing(collection);

        collection = new MgCoordinateCollection();
        Ptr<MgCoordinate> coord6 = new MgCoordinateXY( 70.0, 100.0);
        Ptr<MgCoordinate> coord7 = new MgCoordinateXY(100.0,  70.0);
        Ptr<MgCoordinate> coord8 = new MgCoordinateXY( 60.0, 120.0);
        collection->Add(coord1);
        collection->Add(coord5);
        collection->Add(coord6);
        collection->Add(coord7);
        collection->Add(coord8);
        Ptr<MgLinearRing> linearRing2 = factory.CreateLinearRing(collection);
        Ptr<MgLinearRingCollection> ringCollection = new MgLinearRingCollection();
        ringCollection->Add(linearRing2);

        polygon = factory.CreatePolygon(linearRing1, ringCollection);
        coord = factory.CreateCoordinateXY(100.0, 100.0);
        point = factory.CreatePoint(coord);
        touches = polygon->Touches(point);
        CPPUNIT_ASSERT(!touches);

        //TEST 5
        coord1 = factory.CreateCoordinateXY( 45.0,  45.0);
        coord2 = factory.CreateCoordinateXY( 45.0, -45.0);
        coord3 = factory.CreateCoordinateXY(-45.0, -45.0);
        coord4 = factory.CreateCoordinateXY(-45.0,  45.0);
        coord5 = factory.CreateCoordinateXY( 45.0,  45.0);
        collection = new MgCoordinateCollection();
        collection->Add(coord1);
        collection->Add(coord2);
        collection->Add(coord3);
        collection->Add(coord4);
        collection->Add(coord5);
        linearRing1 = factory.CreateLinearRing(collection);

        coord6 = factory.CreateCoordinateXY(  0.0,  20.0);
        coord7 = factory.CreateCoordinateXY( 20.0, -20.0);
        coord8 = factory.CreateCoordinateXY(-20.0, -20.0);
        collection = new MgCoordinateCollection();
        collection->Add(coord6);
        collection->Add(coord7);
        collection->Add(coord8);
        collection->Add(coord6);
        linearRing2 = factory.CreateLinearRing(collection);

        ringCollection = new MgLinearRingCollection();
        ringCollection->Add(linearRing2);
        polygon = factory.CreatePolygon(linearRing1, ringCollection);
        coord = factory.CreateCoordinateXY(100.0, 100.0);
        point = factory.CreatePoint(coord);
        touches = point->Touches(polygon);
        CPPUNIT_ASSERT(!touches);

        //TEST 6
        coord1 = factory.CreateCoordinateXY(100.0, 100.0);
        coord2 = factory.CreateCoordinateXY( 45.0,  45.0);
        coord3 = factory.CreateCoordinateXY(-45.0, -45.0);
        Ptr<MgPoint> point1 = factory.CreatePoint(coord1);
        Ptr<MgPoint> point2 = factory.CreatePoint(coord2);
        Ptr<MgPoint> point3 = factory.CreatePoint(coord3);

        Ptr<MgCoordinate> start1   = factory.CreateCoordinateXY( 45.0,  45.0);
        Ptr<MgCoordinate> end1     = factory.CreateCoordinateXY( 45.0, -45.0);
        Ptr<MgCoordinate> control1 = factory.CreateCoordinateXY( 45.0,   0.0);
        Ptr<MgCoordinate> start2   = factory.CreateCoordinateXY( 45.0, -45.0);
        Ptr<MgCoordinate> end2     = factory.CreateCoordinateXY(-45.0, -45.0);
        Ptr<MgCoordinate> control2 = factory.CreateCoordinateXY(  0.0, -45.0);

        Ptr<MgCurveSegment> curveSegment1 = factory.CreateArcSegment(start1, end1, control1);
        Ptr<MgCurveSegment> curveSegment2 = factory.CreateArcSegment(start2, end2, control2);

        Ptr<MgCurveSegmentCollection> curveSegments = new MgCurveSegmentCollection();
        curveSegments->Add(curveSegment1);
        curveSegments->Add(curveSegment2);

        Ptr<MgCurveString> curveString = factory.CreateCurveString(curveSegments);
        touches = curveString->Touches(point1);
        touches = curveString->Touches(point3);
        touches = curveString->Touches(point2);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_Envelope()
{
    try
    {
        MgGeometryFactory factory;

        Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXY(10.0,  0.0);
        Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXY( 0.0, 10.0);

        Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
        coords->Add(coord1);
        coords->Add(coord2);

        Ptr<MgLineString> lineString = factory.CreateLineString(coords);

        Ptr<MgEnvelope> env = lineString->Envelope();

        coord1 = env->GetLowerLeftCoordinate();
        coord2 = env->GetUpperRightCoordinate();
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_GetArea()
{
    try
    {
        MgGeometryFactory factory;

        //TEST 1
        Ptr<MgPoint> point = CreatePoint();
        double area = point->GetArea();
        CPPUNIT_ASSERT(area == 0.0);

        //TEST 2
        Ptr<MgLineString> lineString = CreateLineString();
        area = lineString->GetArea();
        CPPUNIT_ASSERT(area == 0.0);

        //TEST 3
        Ptr<MgPolygon> polygon = CreatePolygon();
        area = polygon->GetArea();
        CPPUNIT_ASSERT(area == 24.0);

        //TEST 4 TODO: commented out due to parse exception
        //Ptr<MgCurveString> curveString = CreateCurveString(0.0);
        //area = curveString->GetArea();
        //CPPUNIT_ASSERT(area == 0.0);

        //TEST 5 TODO: commented out due to parse exception
        //Ptr<MgCurveRing> curveRing = CreateCurveRing(0.0);
        //area = curveRing->GetArea();
        //CPPUNIT_ASSERT(area == 0.0);

        //TEST 6 TODO: commented out due to parse exception
        //Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        //area = curveRing->GetArea();
        //CPPUNIT_ASSERT(area == 0.0);

        //TEST 7
        Ptr<MgMultiPoint> multiPoint = CreateMultiPoint();
        area = multiPoint->GetArea();
        CPPUNIT_ASSERT(area == 0.0);

        //TEST 8
        Ptr<MgMultiLineString> multiLineString = CreateMultiLineString();
        area = multiLineString->GetArea();
        CPPUNIT_ASSERT(area == 0.0);

        //TEST 9
        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        area = multiPolygon->GetArea();
        CPPUNIT_ASSERT(area == 48.0);

        //TEST 10 TODO: commented out due to parse exception
        //Ptr<MgMultiCurveString> multiCurveString = CreateMultiCurveString();
        //area = multiCurveString->GetArea();
        //CPPUNIT_ASSERT(area == 0.0);

        //TEST 11 TODO: commented out due to parse exception
        //Ptr<MgMultiCurvePolygon> multiCurvePolygon = CreateMultiCurvePolygon(3, 0.0);
        //area = curveRing->GetArea();
        //CPPUNIT_ASSERT(area == 0.0);

        //TEST 12 TODO: commented out due to parse exception
        //Ptr<MgMultiGeometry> multiGeometry = CreateMultiGeometry();
        //area = multiGeometry->GetArea();
        //CPPUNIT_ASSERT(area == 23.5);

        //TEST 13
        Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXYZ(-45.0, -45.0, 1.0);
        Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXYZ(-45.0,  45.0, 2.0);
        Ptr<MgCoordinate> coord3 = factory.CreateCoordinateXYZ( 45.0,  45.0, 3.0);
        Ptr<MgCoordinate> coord4 = factory.CreateCoordinateXYZ( 45.0, -45.0, 4.0);
        Ptr<MgCoordinate> coord5 = factory.CreateCoordinateXYZ(-45.0, -45.0, 1.0);

        Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
        coords->Add(coord1);
        coords->Add(coord2);
        coords->Add(coord3);
        coords->Add(coord4);
        coords->Add(coord5);

        Ptr<MgLinearRing> linearRing = factory.CreateLinearRing(coords);

        polygon = factory.CreatePolygon(linearRing, NULL);
        area = polygon->GetArea();
        CPPUNIT_ASSERT(area == 8100.0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_GetCentroid()
{
    try
    {
        MgWktReaderWriter readerWriter;
        MgGeometryFactory factory;

        //TEST 1
        Ptr<MgPoint> point = CreatePoint();
        Ptr<MgPoint> centroid = point->GetCentroid();
        STRING found = readerWriter.Write(centroid);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POINT (5 3)") == 0);

        //TEST 2
        Ptr<MgLineString> lineString = CreateLineString();
        centroid = lineString->GetCentroid();
        found = readerWriter.Write(centroid);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POINT (2 3)") == 0);

        //TEST 3
        Ptr<MgPolygon> polygon = CreatePolygon();
        centroid = polygon->GetCentroid();
        found = readerWriter.Write(centroid);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POINT", ::wcslen(L"POINT")));

        //TEST 4
        Ptr<MgCurveString> curveString = CreateCurveString(0.0);
        centroid = curveString->GetCentroid();
        found = readerWriter.Write(centroid);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POINT", ::wcslen(L"POINT")));

        //TEST 5 TODO: commented out due to parse exception
        //Ptr<MgCurveRing> curveRing = CreateCurveRing(0.0);
        //centroid = curveRing->GetCentroid();
        //found = readerWriter.Write(centroid);

        //TEST 6 TODO: commented out due to parse exception
        //Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        //centroid = curveRing->GetCentroid();
        //found = readerWriter.Write(centroid);

        //TEST 7
        Ptr<MgMultiPoint> multiPoint = CreateMultiPoint();
        centroid = multiPoint->GetCentroid();
        found = readerWriter.Write(centroid);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POINT (4 5)") == 0);

        //TEST 8
        Ptr<MgMultiLineString> multiLineString = CreateMultiLineString();
        centroid = multiLineString->GetCentroid();
        found = readerWriter.Write(centroid);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POINT", ::wcslen(L"POINT")));

        //TEST 9
        Ptr<MgMultiPolygon> multiPolygon = CreateMultiPolygon();
        centroid = multiPolygon->GetCentroid();
        found = readerWriter.Write(centroid);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POINT", ::wcslen(L"POINT")));

        //TEST 10
        Ptr<MgMultiCurveString> multiCurveString = CreateMultiCurveString();
        centroid = multiCurveString->GetCentroid();
        found = readerWriter.Write(centroid);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POINT", ::wcslen(L"POINT")));

        //TEST 11 TODO: commented out due to parse exception
        //Ptr<MgMultiCurvePolygon> multiCurvePolygon = CreateMultiCurvePolygon(3, 0.0);
        //centroid = curveRing->GetCentroid();
        //found = readerWriter.Write(centroid);

        //TEST 12
        Ptr<MgMultiGeometry> multiGeometry = CreateMultiGeometry();
        centroid = multiGeometry->GetCentroid();
        found = readerWriter.Write(centroid);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POINT", ::wcslen(L"POINT")));

        //TEST 13
        Ptr<MgCoordinate> coord1 = factory.CreateCoordinateXYZ(-45.0, -45.0, 1.0);
        Ptr<MgCoordinate> coord2 = factory.CreateCoordinateXYZ(-45.0,  45.0, 2.0);
        Ptr<MgCoordinate> coord3 = factory.CreateCoordinateXYZ( 45.0,  45.0, 3.0);
        Ptr<MgCoordinate> coord4 = factory.CreateCoordinateXYZ( 45.0, -45.0, 4.0);
        Ptr<MgCoordinate> coord5 = factory.CreateCoordinateXYZ(-45.0, -45.0, 1.0);

        Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
        coords->Add(coord1);
        coords->Add(coord2);
        coords->Add(coord3);
        coords->Add(coord4);
        coords->Add(coord5);

        Ptr<MgLinearRing> linearRing = factory.CreateLinearRing(coords);

        polygon = factory.CreatePolygon(linearRing, NULL);
        centroid = polygon->GetCentroid();
        found = readerWriter.Write(centroid);
        CPPUNIT_ASSERT(_wcsicmp(found.c_str(), L"POINT (-0 -0)") == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_GetInteriorPoint()
{
    try
    {
        MgGeometryFactory factory;

        // TODO: put this test back in when the parse exception is solved
        //Ptr<MgLinearRing> linearRing = CreateLinearRing();
        //point = linearRing->GetPointInRing();

        Ptr<MgPolygon> polygon = CreatePolygon();
        Ptr<MgPoint> point = polygon->GetPointInRegion();

        // TODO: put this test back in when the parse exception is solved
        //Ptr<MgCurvePolygon> curvePolygon = CreateCurvePolygon(0.0);
        //point = curvePolygon->GetPointInRegion();

        // TODO: put this test back in when the parse exception is solved
        //Ptr<MgCurveRing> curveRing = CreateCurveRing(0.0);
        //point = curveRing->GetPointInRing();
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestGeometry::TestCase_CoordinateIterator()
{
    try
    {
        Ptr<MgGeometricEntity> point = CreatePoint();
        Ptr<MgCoordinateIterator> iterator = point->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 1);

        Ptr<MgGeometricEntity> lineString = CreateLineString();
        iterator = lineString->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 3);

        Ptr<MgGeometricEntity> polygon = CreatePolygon();
        iterator = polygon->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 13);

        Ptr<MgGeometricEntity> curveString = CreateCurveString(0.0);
        iterator = curveString->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 20);

        Ptr<MgGeometricEntity> curveRing = CreateCurveRing(0.0);
        iterator = curveRing->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 19);

        Ptr<MgGeometricEntity> multiPoint = CreateMultiPoint();
        iterator = multiPoint->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 3);

        Ptr<MgGeometricEntity> multiLineString = CreateMultiLineString();
        iterator = multiLineString->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 6);

        Ptr<MgGeometricEntity> multiPolygon = CreateMultiPolygon();
        iterator = multiPolygon->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 26);

        Ptr<MgGeometricEntity> multiCurveString = CreateMultiCurveString();
        iterator = multiCurveString->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 60);

        Ptr<MgGeometricEntity> multiCurvePolygon = CreateMultiCurvePolygon(2, 0.0);
        iterator = multiCurvePolygon->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 114);

        Ptr<MgGeometricEntity> multiGeometry = CreateMultiGeometry();
        iterator = multiGeometry->GetCoordinates();
        CPPUNIT_ASSERT(iterator->GetCount() == 94);
    }
    catch (...)
    {
        throw;
    }
}
