package org.osgeo.mapguide.test.platform;

import java.io.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class GeomBuild
{
    private MgGeometryFactory factory = new MgGeometryFactory();

    public MgPoint createPoint()
    {
        MgCoordinate coord = factory.createCoordinateXYZ(5.0, 3.0, 2.0);
        return factory.createPoint(coord);
    }

    public MgLineString createLineString(double offset)
    {
        MgCoordinateCollection coordCol = new MgCoordinateCollection();

        MgCoordinate coord1 = factory.createCoordinateXY(offset + 0.0, 1.0);
        MgCoordinate coord2 = factory.createCoordinateXY(offset + 2.0, 3.0);
        MgCoordinate coord3 = factory.createCoordinateXY(offset + 4.0, 5.0);

        coordCol.add(coord1);
        coordCol.add(coord2);
        coordCol.add(coord3);

        return factory.createLineString(coordCol);
    }



    public MgLinearRing createLinearRing(double offset)
    {
        MgCoordinate oCoord1 = factory.createCoordinateXY(0.0, offset);
        MgCoordinate oCoord2 = factory.createCoordinateXY(5.0, offset);
        MgCoordinate oCoord3 = factory.createCoordinateXY(5.0, offset + 5.0);
        MgCoordinate oCoord4 = factory.createCoordinateXY(0.0, offset + 5.0);
        MgCoordinate oCoord5 = factory.createCoordinateXY(0.0, offset);

        MgCoordinateCollection outerRingCoord = new MgCoordinateCollection();
        outerRingCoord.add(oCoord1);
        outerRingCoord.add(oCoord2);
        outerRingCoord.add(oCoord3);
        outerRingCoord.add(oCoord4);
        outerRingCoord.add(oCoord5);

        return factory.createLinearRing(outerRingCoord);
    }


    public MgPolygon createPolygon(double offset)
    {


        // OuterRing
        MgCoordinate oCoord1 = factory.createCoordinateXY(offset + 0.0, 0.0);
        MgCoordinate oCoord2 = factory.createCoordinateXY(offset + 5.0, 0.0);
        MgCoordinate oCoord3 = factory.createCoordinateXY(offset + 5.0, 5.0);
        MgCoordinate oCoord4 = factory.createCoordinateXY(offset + 0.0, 5.0);
        MgCoordinate oCoord5 = factory.createCoordinateXY(offset + 0.0, 0.0);

        MgCoordinateCollection outerRingCoord = new MgCoordinateCollection();
        outerRingCoord.add(oCoord1);
        outerRingCoord.add(oCoord2);
        outerRingCoord.add(oCoord3);
        outerRingCoord.add(oCoord4);
        outerRingCoord.add(oCoord5);

        // Inner Ring1
        MgCoordinate i1Coord1 = factory.createCoordinateXY(offset + 1.0, 1.0);
        MgCoordinate i1Coord2 = factory.createCoordinateXY(offset + 2.0, 1.0);
        MgCoordinate i1Coord3 = factory.createCoordinateXY(offset + 2.0, 2.0);
        MgCoordinate i1Coord4 = factory.createCoordinateXY(offset + 1.0, 1.0);

        MgCoordinateCollection inner1RingCoord = new MgCoordinateCollection();
        inner1RingCoord.add(i1Coord1);
        inner1RingCoord.add(i1Coord2);
        inner1RingCoord.add(i1Coord3);
        inner1RingCoord.add(i1Coord4);

        // Inner Ring2
        MgCoordinate i2Coord1 = factory.createCoordinateXY(offset + 3.0, 3.0);
        MgCoordinate i2Coord2 = factory.createCoordinateXY(offset + 4.0, 3.0);
        MgCoordinate i2Coord3 = factory.createCoordinateXY(offset + 4.0, 4.0);
        MgCoordinate i2Coord4 = factory.createCoordinateXY(offset + 3.0, 3.0);

        MgCoordinateCollection inner2RingCoord = new MgCoordinateCollection();
        inner2RingCoord.add(i2Coord1);
        inner2RingCoord.add(i2Coord2);
        inner2RingCoord.add(i2Coord3);
        inner2RingCoord.add(i2Coord4);

        MgLinearRing extRing = factory.createLinearRing(outerRingCoord);
        MgLinearRing intRing1 = factory.createLinearRing(inner1RingCoord);
        MgLinearRing intRing2 = factory.createLinearRing(inner2RingCoord);

        MgLinearRingCollection intRings = new MgLinearRingCollection();
        intRings.add(intRing1);
        intRings.add(intRing2);

        return factory.createPolygon(extRing, intRings);
    }


    public MgCurveString createCurveString(double offset)
    {


        // create and return a curvestring consisting of
        // one circulararc segment and one linearstring segment

        // arcseg  = (0,0), (0,1), (1,2)
        // lineseg = (1,2), (3,0), (3,2)

        // ArcSegment
        MgCoordinate startPos = factory.createCoordinateXY(offset + 0.0, offset + 0.0);
        MgCoordinate midPos = factory.createCoordinateXY(offset + 0.0, offset + 1.0);
        MgCoordinate endPos = factory.createCoordinateXY(offset + 1.0, offset + 2.0);
        MgArcSegment arcSeg = factory.createArcSegment(startPos, endPos, midPos);

        // Linear Segment
        MgCoordinateCollection points = new MgCoordinateCollection();
        MgCoordinate pt1 = factory.createCoordinateXY(offset + 1.0, offset + 2.0);
        MgCoordinate pt2 = factory.createCoordinateXY(offset + 3.0, offset + 0.0);
        MgCoordinate pt3 = factory.createCoordinateXY(offset + 3.0, offset + 2.0);
        points.add(pt1);
        points.add(pt2);
        points.add(pt3);
        MgLinearSegment lineSeg = factory.createLinearSegment(points);

        // CurveSegment
        MgCurveSegmentCollection curveSegs = new MgCurveSegmentCollection();
        curveSegs.add(arcSeg);
        curveSegs.add(lineSeg);

        return factory.createCurveString(curveSegs);
    }


    public MgCurveRing createCurveRing(double offset)
    {
        // Ring is a closed entity.
        // create and return a ring consisting of
        // one circulararc segment and one linearstring segment

        // arcseg  = (0,0), (0,1), (1,2)
        // lineseg = (1,2), (0,0)



        // ArcSegment
        MgCoordinate startPos = factory.createCoordinateXY(offset + 0.0, offset + 0.0);
        MgCoordinate midPos = factory.createCoordinateXY(offset + 0.0, offset + 1.0);
        MgCoordinate endPos = factory.createCoordinateXY(offset + 1.0, offset + 2.0);
        MgArcSegment arcSeg = factory.createArcSegment(startPos, endPos, midPos);

        // Linear Segment
        MgCoordinateCollection points = new MgCoordinateCollection();
        MgCoordinate fromPt = factory.createCoordinateXY(offset + 1.0, offset + 2.0);
        MgCoordinate toPt = factory.createCoordinateXY(offset + 0.0, offset + 0.0);
        points.add(fromPt);
        points.add(toPt);
        MgLinearSegment lineSeg = factory.createLinearSegment(points);

        // Curve Segment
        MgCurveSegmentCollection curveSegs = new MgCurveSegmentCollection();
        curveSegs.add(arcSeg);
        curveSegs.add(lineSeg);

        return factory.createCurveRing(curveSegs);
    }


    public MgCurvePolygon createCurvePolygon(double offset, int increment)
    {


        MgCurveRing extRing = createCurveRing(offset + increment);

        MgCurveRingCollection intRings = new MgCurveRingCollection();

        MgCurveRing ring1 = createCurveRing(offset + 2 * increment);
        MgCurveRing ring2 = createCurveRing(offset + 3 * increment);

        intRings.add(ring1);
        intRings.add(ring2);

        return factory.createCurvePolygon(extRing, intRings);
    }


    public MgMultiPoint createMultiPoint()
    {


        MgCoordinate coord1 = factory.createCoordinateXYZ(1.0, 2.0, 3.0);
        MgPoint point1 = factory.createPoint(coord1);

        MgCoordinate coord2 = factory.createCoordinateXYZ(4.0, 5.0, 6.0);
        MgPoint point2 = factory.createPoint(coord2);

        MgCoordinate coord3 = factory.createCoordinateXYZ(7.0, 8.0, 9.0);
        MgPoint point3 = factory.createPoint(coord3);

        MgPointCollection pnts = new MgPointCollection();
        pnts.add(point1);
        pnts.add(point2);
        pnts.add(point3);

        return factory.createMultiPoint(pnts);
    }


    public MgMultiLineString createMultiLineString()
    {


        MgCoordinate coord1 = factory.createCoordinateXYZ(0.0, 1.0, 2.0);
        MgCoordinate coord2 = factory.createCoordinateXYZ(3.0, 4.0, 5.0);
        MgCoordinate coord3 = factory.createCoordinateXYZ(6.0, 7.0, 8.0);

        MgCoordinateCollection coordColA = new MgCoordinateCollection();
        coordColA.add(coord1);
        coordColA.add(coord2);
        coordColA.add(coord3);

        MgCoordinate coord4 = factory.createCoordinateXYZ(9.0, 10.0, 11.0);
        MgCoordinate coord5 = factory.createCoordinateXYZ(12.0, 13.0, 14.0);
        MgCoordinate coord6 = factory.createCoordinateXYZ(15.0, 16.0, 17.0);

        MgCoordinateCollection coordColB = new MgCoordinateCollection();
        coordColB.add(coord4);
        coordColB.add(coord5);
        coordColB.add(coord6);

        MgLineString lineString1 = factory.createLineString(coordColA);
        MgLineString lineString2 = factory.createLineString(coordColB);

        MgLineStringCollection lineStrings = new MgLineStringCollection();
        lineStrings.add(lineString1);
        lineStrings.add(lineString2);

        return factory.createMultiLineString(lineStrings);
    }


    public MgMultiPolygon createMultiPolygon()
    {


        MgPolygon polygon1 = createPolygon(0.0);
        MgPolygon polygon2 = createPolygon(0.0);

        MgPolygonCollection polygons = new MgPolygonCollection();
        polygons.add(polygon1);
        polygons.add(polygon2);

        return factory.createMultiPolygon(polygons);
    }


    MgMultiCurveString createMultiCurveString()
    {


        MgCurveString curveString1 = createCurveString(100);
        MgCurveString curveString2 = createCurveString(200);
        MgCurveString curveString3 = createCurveString(300);

        MgCurveStringCollection curveStrings = new MgCurveStringCollection();
        curveStrings.add(curveString1);
        curveStrings.add(curveString2);
        curveStrings.add(curveString3);

        return factory.createMultiCurveString(curveStrings);
    }


    public MgMultiCurvePolygon createMultiCurvePolygon(int numCurvePolys, double offset)
    {


        MgCurvePolygonCollection curvePolys = new MgCurvePolygonCollection();
        for (int i = 0; i < numCurvePolys; i++)
        {
            MgCurvePolygon curvePoly = createCurvePolygon(i + offset, 1);
            curvePolys.add(curvePoly);
        }

        return factory.createMultiCurvePolygon(curvePolys);
    }


    public MgMultiGeometry createMultiGeometry()
    {


        MgGeometryCollection geometries = new MgGeometryCollection();
        MgGeometry geometry = null;

        // CurvePolygon
        geometry = (MgGeometry)createCurvePolygon(0, 1);
        geometries.add(geometry);

        // CurveString
        // Not doing CurveString because of some unfixed defect.
        // It may be the same one that sometimes affects MultiPolygon.
        geometry = (MgGeometry)createCurveString(100);
        geometries.add(geometry);

        // LineString
        geometry = (MgGeometry)createLineString(1.0);
        geometries.add(geometry);

        // Point
        geometry = (MgGeometry)createPoint();
        geometries.add(geometry);

        // Polygon
        geometry = createPolygon(0.0);
        geometries.add(geometry);

        // Make MultiGeometry from the many geometries collected above.
        return factory.createMultiGeometry(geometries);
    }

    public MgArcSegment createArcSegment(double offset)
    {
        MgCoordinate startPos = factory.createCoordinateXY(offset + 0.0, offset + 0.0);
        MgCoordinate midPos = factory.createCoordinateXY(offset + 0.0, offset + 1.0);
        MgCoordinate endPos = factory.createCoordinateXY(offset + 1.0, offset + 2.0);
        return factory.createArcSegment(startPos, endPos, midPos);
    }
}