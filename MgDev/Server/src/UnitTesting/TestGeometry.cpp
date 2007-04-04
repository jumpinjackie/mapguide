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
        STRING found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        Ptr<MgMultiGeometry> multiGeometry = CreateMultiGeometry();
        geom = multiGeometry->Buffer(5.0, NULL);
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
        STRING found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        Ptr<MgLineString> testLine = CreateLineString();
        geom = testLine->Buffer(5.0, measure);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        Ptr<MgPolygon> testPolygon = CreatePolygon();
        geom = testPolygon->Buffer(5.0, measure);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        Ptr<MgCurveString> testCurveString = CreateCurveString(0.0);
        geom = testCurveString->Buffer(5.0, measure);
        found = geom->ToAwkt(false);
        CPPUNIT_ASSERT(0 == ::wcsncmp(found.c_str(), L"POLYGON", ::wcslen(L"POLYGON")));

        Ptr<MgCurvePolygon> testCurvePolygon = CreateCurvePolygon(0.0, 10);
        geom = testCurvePolygon->Buffer(1.0, measure);
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
