using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide;

namespace OSGeo.MapGuide
{
    public class GeomBuild
    {
        private MgGeometryFactory factory = new MgGeometryFactory();

        public MgPoint CreatePoint()
        {
            MgCoordinate coord = factory.CreateCoordinateXYZ(5.0, 3.0, 2.0);
            return factory.CreatePoint(coord);
        }

        public MgLineString CreateLineString(double offset)
        {
            MgCoordinateCollection coordCol = new MgCoordinateCollection();

            MgCoordinate coord1 = factory.CreateCoordinateXY(offset + 0.0, 1.0);
            MgCoordinate coord2 = factory.CreateCoordinateXY(offset + 2.0, 3.0);
            MgCoordinate coord3 = factory.CreateCoordinateXY(offset + 4.0, 5.0);

            coordCol.Add(coord1);
            coordCol.Add(coord2);
            coordCol.Add(coord3);

            return factory.CreateLineString(coordCol);
        }



        public MgLinearRing CreateLinearRing(double offset)
        {
            MgCoordinate oCoord1 = factory.CreateCoordinateXY(0.0, offset);
            MgCoordinate oCoord2 = factory.CreateCoordinateXY(5.0, offset);
            MgCoordinate oCoord3 = factory.CreateCoordinateXY(5.0, offset+5.0);
            MgCoordinate oCoord4 = factory.CreateCoordinateXY(0.0, offset+5.0);
            MgCoordinate oCoord5 = factory.CreateCoordinateXY(0.0, offset);

            MgCoordinateCollection outerRingCoord = new MgCoordinateCollection();
            outerRingCoord.Add(oCoord1);
            outerRingCoord.Add(oCoord2);
            outerRingCoord.Add(oCoord3);
            outerRingCoord.Add(oCoord4);
            outerRingCoord.Add(oCoord5);

            return factory.CreateLinearRing(outerRingCoord);
        }


        public MgPolygon CreatePolygon(double offset)
        {
            

            // OuterRing
            MgCoordinate oCoord1 = factory.CreateCoordinateXY(offset + 0.0, 0.0);
            MgCoordinate oCoord2 = factory.CreateCoordinateXY(offset + 5.0, 0.0);
            MgCoordinate oCoord3 = factory.CreateCoordinateXY(offset + 5.0, 5.0);
            MgCoordinate oCoord4 = factory.CreateCoordinateXY(offset + 0.0, 5.0);
            MgCoordinate oCoord5 = factory.CreateCoordinateXY(offset + 0.0, 0.0);

            MgCoordinateCollection outerRingCoord = new MgCoordinateCollection();
            outerRingCoord.Add(oCoord1);
            outerRingCoord.Add(oCoord2);
            outerRingCoord.Add(oCoord3);
            outerRingCoord.Add(oCoord4);
            outerRingCoord.Add(oCoord5);

            // Inner Ring1
            MgCoordinate i1Coord1 = factory.CreateCoordinateXY(offset + 1.0, 1.0);
            MgCoordinate i1Coord2 = factory.CreateCoordinateXY(offset + 2.0, 1.0);
            MgCoordinate i1Coord3 = factory.CreateCoordinateXY(offset + 2.0, 2.0);
            MgCoordinate i1Coord4 = factory.CreateCoordinateXY(offset + 1.0, 1.0);

            MgCoordinateCollection inner1RingCoord = new MgCoordinateCollection();
            inner1RingCoord.Add(i1Coord1);
            inner1RingCoord.Add(i1Coord2);
            inner1RingCoord.Add(i1Coord3);
            inner1RingCoord.Add(i1Coord4);

            // Inner Ring2
            MgCoordinate i2Coord1 = factory.CreateCoordinateXY(offset + 3.0, 3.0);
            MgCoordinate i2Coord2 = factory.CreateCoordinateXY(offset + 4.0, 3.0);
            MgCoordinate i2Coord3 = factory.CreateCoordinateXY(offset + 4.0, 4.0);
            MgCoordinate i2Coord4 = factory.CreateCoordinateXY(offset + 3.0, 3.0);

            MgCoordinateCollection inner2RingCoord = new MgCoordinateCollection();
            inner2RingCoord.Add(i2Coord1);
            inner2RingCoord.Add(i2Coord2);
            inner2RingCoord.Add(i2Coord3);
            inner2RingCoord.Add(i2Coord4);

            MgLinearRing extRing  = factory.CreateLinearRing(outerRingCoord);
            MgLinearRing intRing1 = factory.CreateLinearRing(inner1RingCoord);
            MgLinearRing intRing2 = factory.CreateLinearRing(inner2RingCoord);

            MgLinearRingCollection intRings = new MgLinearRingCollection();
            intRings.Add(intRing1);
            intRings.Add(intRing2);

            return factory.CreatePolygon(extRing, intRings);
        }


        public MgCurveString CreateCurveString(double offset)
        {
            

            // Create and return a curvestring consisting of
            // one circulararc segment and one linearstring segment

            // arcseg  = (0,0), (0,1), (1,2)
            // lineseg = (1,2), (3,0), (3,2)

            // ArcSegment
            MgCoordinate startPos = factory.CreateCoordinateXY(offset+0.0, offset+0.0);
            MgCoordinate midPos   = factory.CreateCoordinateXY(offset+0.0, offset+1.0);
            MgCoordinate endPos   = factory.CreateCoordinateXY(offset+1.0, offset+2.0);
            MgArcSegment arcSeg   = factory.CreateArcSegment(startPos, endPos, midPos);

            // Linear Segment
            MgCoordinateCollection points = new MgCoordinateCollection();
            MgCoordinate pt1 = factory.CreateCoordinateXY(offset+1.0, offset+2.0);
            MgCoordinate pt2 = factory.CreateCoordinateXY(offset+3.0, offset+0.0);
            MgCoordinate pt3 = factory.CreateCoordinateXY(offset+3.0, offset+2.0);
            points.Add(pt1);
            points.Add(pt2);
            points.Add(pt3);
            MgLinearSegment lineSeg = factory.CreateLinearSegment(points);

            // CurveSegment
            MgCurveSegmentCollection curveSegs = new MgCurveSegmentCollection();
            curveSegs.Add(arcSeg);
            curveSegs.Add(lineSeg);

            return factory.CreateCurveString(curveSegs);
        }


        public MgCurveRing CreateCurveRing(double offset)
        {
            // Ring is a closed entity.
            // Create and return a ring consisting of
            // one circulararc segment and one linearstring segment

            // arcseg  = (0,0), (0,1), (1,2)
            // lineseg = (1,2), (0,0)

            

            // ArcSegment
            MgCoordinate startPos = factory.CreateCoordinateXY(offset+0.0, offset+0.0);
            MgCoordinate midPos   = factory.CreateCoordinateXY(offset+0.0, offset+1.0);
            MgCoordinate endPos   = factory.CreateCoordinateXY(offset+1.0, offset+2.0);
            MgArcSegment arcSeg   = factory.CreateArcSegment(startPos, endPos, midPos);

            // Linear Segment
            MgCoordinateCollection points = new MgCoordinateCollection();
            MgCoordinate fromPt = factory.CreateCoordinateXY(offset+1.0, offset+2.0);
            MgCoordinate   toPt = factory.CreateCoordinateXY(offset+0.0, offset+0.0);
            points.Add(fromPt);
            points.Add(toPt);
            MgLinearSegment lineSeg = factory.CreateLinearSegment(points);

            // Curve Segment
            MgCurveSegmentCollection curveSegs = new MgCurveSegmentCollection();
            curveSegs.Add(arcSeg);
            curveSegs.Add(lineSeg);

            return factory.CreateCurveRing(curveSegs);
        }


        public MgCurvePolygon CreateCurvePolygon(double offset, int increment)
        {
            

            MgCurveRing extRing = CreateCurveRing(offset + increment);

            MgCurveRingCollection intRings = new MgCurveRingCollection();

            MgCurveRing ring1 = CreateCurveRing(offset + 2 * increment);
            MgCurveRing ring2 = CreateCurveRing(offset + 3 * increment);

            intRings.Add(ring1);
            intRings.Add(ring2);

            return factory.CreateCurvePolygon(extRing, intRings);
        }


        public MgMultiPoint CreateMultiPoint()
        {
            

            MgCoordinate coord1 = factory.CreateCoordinateXYZ(1.0, 2.0, 3.0);
            MgPoint point1 = factory.CreatePoint(coord1);

            MgCoordinate coord2 = factory.CreateCoordinateXYZ(4.0, 5.0, 6.0);
            MgPoint point2 = factory.CreatePoint(coord2);

            MgCoordinate coord3 = factory.CreateCoordinateXYZ(7.0, 8.0, 9.0);
            MgPoint point3 = factory.CreatePoint(coord3);

            MgPointCollection pnts = new MgPointCollection();
            pnts.Add(point1);
            pnts.Add(point2);
            pnts.Add(point3);

            return factory.CreateMultiPoint(pnts);
        }


        public MgMultiLineString CreateMultiLineString()
        {
            

            MgCoordinate coord1 = factory.CreateCoordinateXYZ(0.0, 1.0, 2.0);
            MgCoordinate coord2 = factory.CreateCoordinateXYZ(3.0, 4.0, 5.0);
            MgCoordinate coord3 = factory.CreateCoordinateXYZ(6.0, 7.0, 8.0);

            MgCoordinateCollection coordColA = new MgCoordinateCollection();
            coordColA.Add(coord1);
            coordColA.Add(coord2);
            coordColA.Add(coord3);

            MgCoordinate coord4 = factory.CreateCoordinateXYZ( 9.0, 10.0, 11.0);
            MgCoordinate coord5 = factory.CreateCoordinateXYZ(12.0, 13.0, 14.0);
            MgCoordinate coord6 = factory.CreateCoordinateXYZ(15.0, 16.0, 17.0);

            MgCoordinateCollection coordColB = new MgCoordinateCollection();
            coordColB.Add(coord4);
            coordColB.Add(coord5);
            coordColB.Add(coord6);

            MgLineString lineString1 = factory.CreateLineString(coordColA);
            MgLineString lineString2 = factory.CreateLineString(coordColB);

            MgLineStringCollection lineStrings = new MgLineStringCollection();
            lineStrings.Add(lineString1);
            lineStrings.Add(lineString2);

            return factory.CreateMultiLineString(lineStrings);
        }


        public MgMultiPolygon CreateMultiPolygon()
        {
            

            MgPolygon polygon1 = CreatePolygon(0.0);
            MgPolygon polygon2 = CreatePolygon(0.0);

            MgPolygonCollection polygons = new MgPolygonCollection();
            polygons.Add(polygon1);
            polygons.Add(polygon2);

            return factory.CreateMultiPolygon(polygons);
        }


        MgMultiCurveString CreateMultiCurveString()
        {
            

            MgCurveString curveString1 = CreateCurveString(100);
            MgCurveString curveString2 = CreateCurveString(200);
            MgCurveString curveString3 = CreateCurveString(300);

            MgCurveStringCollection curveStrings = new MgCurveStringCollection();
            curveStrings.Add(curveString1);
            curveStrings.Add(curveString2);
            curveStrings.Add(curveString3);

            return factory.CreateMultiCurveString(curveStrings);
        }


        public MgMultiCurvePolygon CreateMultiCurvePolygon(int numCurvePolys, double offset)
        {
            

            MgCurvePolygonCollection curvePolys = new MgCurvePolygonCollection();
            for (int i=0; i < numCurvePolys; i++)
            {
                MgCurvePolygon curvePoly = CreateCurvePolygon(i+offset, 1);
                curvePolys.Add(curvePoly);
            }

            return factory.CreateMultiCurvePolygon(curvePolys);
        }


        public MgMultiGeometry CreateMultiGeometry()
        {
            

            MgGeometryCollection geometries = new MgGeometryCollection();
            MgGeometry geometry = null;

            // CurvePolygon
            geometry = (MgGeometry)CreateCurvePolygon(0, 1);
            geometries.Add(geometry);

            // CurveString
            // Not doing CurveString because of some unfixed defect.
            // It may be the same one that sometimes affects MultiPolygon.
            geometry = (MgGeometry)CreateCurveString(100);
            geometries.Add(geometry);

            // LineString
            geometry = (MgGeometry)CreateLineString(1.0);
            geometries.Add(geometry);

            // Point
            geometry = (MgGeometry)CreatePoint();
            geometries.Add(geometry);

            // Polygon
            geometry = CreatePolygon(0.0);
            geometries.Add(geometry);

            // Make MultiGeometry from the many geometries collected above.
            return factory.CreateMultiGeometry(geometries);
        }

        public MgArcSegment CreateArcSegment(double offset)
        {
            MgCoordinate startPos = factory.CreateCoordinateXY(offset + 0.0, offset + 0.0);
            MgCoordinate midPos = factory.CreateCoordinateXY(offset + 0.0, offset + 1.0);
            MgCoordinate endPos = factory.CreateCoordinateXY(offset + 1.0, offset + 2.0);
            return factory.CreateArcSegment(startPos, endPos, midPos);
        }
    }
}
