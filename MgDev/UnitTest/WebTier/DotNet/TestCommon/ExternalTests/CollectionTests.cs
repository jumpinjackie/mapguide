using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common.ExternalTests
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
            MgCoordinate oCoord3 = factory.CreateCoordinateXY(5.0, offset + 5.0);
            MgCoordinate oCoord4 = factory.CreateCoordinateXY(0.0, offset + 5.0);
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

            MgLinearRing extRing = factory.CreateLinearRing(outerRingCoord);
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
            MgCoordinate startPos = factory.CreateCoordinateXY(offset + 0.0, offset + 0.0);
            MgCoordinate midPos = factory.CreateCoordinateXY(offset + 0.0, offset + 1.0);
            MgCoordinate endPos = factory.CreateCoordinateXY(offset + 1.0, offset + 2.0);
            MgArcSegment arcSeg = factory.CreateArcSegment(startPos, endPos, midPos);

            // Linear Segment
            MgCoordinateCollection points = new MgCoordinateCollection();
            MgCoordinate pt1 = factory.CreateCoordinateXY(offset + 1.0, offset + 2.0);
            MgCoordinate pt2 = factory.CreateCoordinateXY(offset + 3.0, offset + 0.0);
            MgCoordinate pt3 = factory.CreateCoordinateXY(offset + 3.0, offset + 2.0);
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
            MgCoordinate startPos = factory.CreateCoordinateXY(offset + 0.0, offset + 0.0);
            MgCoordinate midPos = factory.CreateCoordinateXY(offset + 0.0, offset + 1.0);
            MgCoordinate endPos = factory.CreateCoordinateXY(offset + 1.0, offset + 2.0);
            MgArcSegment arcSeg = factory.CreateArcSegment(startPos, endPos, midPos);

            // Linear Segment
            MgCoordinateCollection points = new MgCoordinateCollection();
            MgCoordinate fromPt = factory.CreateCoordinateXY(offset + 1.0, offset + 2.0);
            MgCoordinate toPt = factory.CreateCoordinateXY(offset + 0.0, offset + 0.0);
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

            MgCoordinate coord4 = factory.CreateCoordinateXYZ(9.0, 10.0, 11.0);
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
            for (int i = 0; i < numCurvePolys; i++)
            {
                MgCurvePolygon curvePoly = CreateCurvePolygon(i + offset, 1);
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

    public class CollectionTestsIntCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgIntCollection coll = new MgIntCollection();
            coll.Add(10);
            coll.Add(20);
            coll.Add(30);
            coll.Add(40);
            coll.Add(50);
            Assert.AreEqual(20, coll.GetItem(1));
            Assert.AreEqual(30, coll[2]);
            Assert.AreEqual(5, coll.Count);

            int j = 0;
            foreach (int i in coll)
            {
                j += i;
            }
            Assert.AreEqual(150, j);
        }
    }

    public class CollectionTestsPropertyCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgPropertyCollection coll = new MgPropertyCollection();
            MgDoubleProperty dblProp = new MgDoubleProperty("DoubleProp", 1.1111);
            MgInt32Property intProp = new MgInt32Property("IntProp", 1);
            MgDateTime dateTime = new MgDateTime(2006, 9, 21);
            MgDateTimeProperty dateProp = new MgDateTimeProperty("DateProp", dateTime);
            MgSingleProperty single = new MgSingleProperty("SingleProp", (float)2.2222);
            coll.Add(dblProp);
            coll.Add(intProp);
            coll.Add(dateProp);
            coll[2] = single;

            Assert.AreEqual(1.1111, (coll[0] as MgDoubleProperty).GetValue());
            Assert.AreEqual(MgPropertyType.Double, coll[0].GetPropertyType());
            Assert.AreEqual(MgPropertyType.Int32, coll[1].GetPropertyType());
            Assert.AreEqual(MgPropertyType.Single, coll[2].GetPropertyType());
            Assert.AreEqual((float)2.2222, (coll[2] as MgSingleProperty).GetValue());
            Assert.AreEqual(3, coll.Count);

            Assert.AreEqual(MgPropertyType.Double, coll[0].GetPropertyType());

            string str = "";
            foreach (MgProperty prop in coll)
            {
                str = str + "[" + prop.GetName() + "]";
            }
            Assert.AreEqual("[DoubleProp][IntProp][SingleProp]", str);
        }
    }

    public class CollectionTestsBatchPropertyCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgPropertyCollection coll1 = new MgPropertyCollection();
            MgDoubleProperty dblProp = new MgDoubleProperty("DoubleProp", 1.1111);
            coll1.Add(dblProp);

            MgPropertyCollection coll2 = new MgPropertyCollection();
            MgInt32Property intProp = new MgInt32Property("IntProp", 1);
            coll2.Add(intProp);

            MgPropertyCollection coll3 = new MgPropertyCollection();
            MgSingleProperty single = new MgSingleProperty("SingleProp", (float)2.2222);
            coll3.Add(single);


            MgBatchPropertyCollection coll = new MgBatchPropertyCollection();
            coll.Add(coll1);
            coll.Add(coll2);

            Assert.AreEqual(2, coll.Count);
            Assert.AreEqual(MgPropertyType.Double, coll[0][0].GetPropertyType());
            Assert.AreEqual(MgPropertyType.Int32, coll[1][0].GetPropertyType());

            coll[1] = coll3;

            string str = "";
            foreach (MgPropertyCollection c in coll)
            {
                str = str + "[" + c[0].GetName() + "]";
            }
            Assert.AreEqual("[DoubleProp][SingleProp]", str);
        }
    }
     
    public class CollectionTestsClassDefinitionCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgClassDefinition class1 = new MgClassDefinition();
            class1.SetName("class1");

            MgClassDefinition class2 = new MgClassDefinition();
            class2.SetName("class2");

            MgClassDefinition class3 = new MgClassDefinition();
            class3.SetName("class3");

            MgClassDefinitionCollection coll = new MgClassDefinitionCollection();
            coll.Insert(0, class3);
            coll.Insert(0, class2);
            coll.Insert(0, class1);

            Assert.AreEqual(3, coll.Count);
            Assert.AreEqual("class2", coll[1].GetName());

            MgClassDefinition tmp = coll[0];
            coll[0] = coll[1];
            coll[1] = coll[2];
            coll[2] = tmp;

            string str = "";
            foreach (MgClassDefinition def in coll)
            {
                str = str + "[" + def.GetName() + "]";
            }
            Assert.AreEqual("[class2][class3][class1]", str);
        }
    }

    public class CollectionTestsCoordinateCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate c1 = gf.CreateCoordinateXY(1.0, 1.0);
            MgCoordinate c2 = gf.CreateCoordinateXY(2.0, 2.0);
            MgCoordinate c3 = gf.CreateCoordinateXY(3.0, 3.0);
            MgCoordinate c4 = gf.CreateCoordinateXY(4.0, 4.0);

            MgCoordinateCollection coll = new MgCoordinateCollection();
            coll.Add(c1);
            coll.Insert(1, c2);
            coll.Add(c3);
            coll.Add(c4);

            Assert.AreEqual(4, coll.Count);
            Assert.AreEqual(1.0, coll[0].GetX());
            coll[3] = coll[2];
            Assert.AreEqual(3.0, coll[3].GetX());

            double sum = 0.0;
            foreach (MgCoordinate coord in coll)
            {
                sum += coord.GetX();
            }
            Assert.AreEqual(9.0, sum);
        }
    }


    public class CollectionTestsCurvePolygonCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var build = new GeomBuild();
            MgCurvePolygon geom1 = build.CreateCurvePolygon(2.0, 5);
            MgCurvePolygon geom2 = build.CreateCurvePolygon(12.0, 5);
            MgCurvePolygon geom3 = build.CreateCurvePolygon(2.0, 5);

            MgCurvePolygonCollection coll = new MgCurvePolygonCollection();
            coll.Add(geom1);
            coll.Add(geom2);
            coll.Add(geom3);

            Assert.AreEqual(3, coll.Count);
            Assert.IsTrue(geom1.Equals(coll[0]));
            Assert.IsTrue(coll[0].Equals(coll[2]));
            Assert.IsFalse(coll[0].Equals(coll[1]));
            coll[0] = coll[1];
            Assert.IsTrue(coll[0].Equals(coll[1]));

            double width = 0.0;
            foreach (MgCurvePolygon geom in coll)
            {
                width += geom.Envelope().GetWidth();
            }
            Assert.AreEqual(geom1.Envelope().GetWidth() * 3.0, width);
        }
    }

    public class CollectionTestsCurveRingCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var build = new GeomBuild();
            MgCurveRing geom1 = build.CreateCurveRing(2.0);
            MgCurveRing geom2 = build.CreateCurveRing(12.0);
            MgCurveRing geom3 = build.CreateCurveRing(2.0);

            MgCurveRingCollection coll = new MgCurveRingCollection();
            coll.Add(geom1);
            coll.Add(geom2);
            coll.Add(geom3);

            Assert.AreEqual(3, coll.Count);
            Assert.IsTrue(geom1.Envelope().Contains(coll[0].Envelope()));
            Assert.IsTrue(coll[0].Envelope().Contains(coll[2].Envelope()));
            Assert.IsFalse(coll[0].Envelope().Contains(coll[1].Envelope()));
            coll[0] = coll[1];
            Assert.IsTrue(coll[0].Envelope().Contains(coll[1].Envelope()));

            double width = 0.0;
            foreach (MgCurveRing geom in coll)
            {
                width += geom.Envelope().GetWidth();
            }
            Assert.AreEqual(geom1.Envelope().GetWidth() * 3.0, width);
        }
    }

    public class CollectionTestsCurveSegmentCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var build = new GeomBuild();
            MgArcSegment geom1 = build.CreateArcSegment(2.0);
            MgArcSegment geom2 = build.CreateArcSegment(12.0);
            MgArcSegment geom3 = build.CreateArcSegment(2.0);

            MgCurveSegmentCollection coll = new MgCurveSegmentCollection();
            coll.Add(geom1);
            coll.Add(geom2);
            coll.Add(geom3);

            Assert.AreEqual(3, coll.Count);
            Assert.IsTrue(geom1.Envelope().Contains(coll[0].Envelope()));
            Assert.IsTrue(coll[0].Envelope().Contains(coll[2].Envelope()));
            Assert.IsFalse(coll[0].Envelope().Contains(coll[1].Envelope()));
            coll[0] = coll[1];
            Assert.IsTrue(coll[0].Envelope().Contains(coll[1].Envelope()));

            double width = 0.0;
            foreach (MgCurveSegment geom in coll)
            {
                width += geom.Envelope().GetWidth();
            }
            Assert.AreEqual(geom1.Envelope().GetWidth() * 3.0, width);
        }
    }

    public class CollectionTestsCurveStringCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var build = new GeomBuild();
            MgCurveString geom1 = build.CreateCurveString(2.0);
            MgCurveString geom2 = build.CreateCurveString(12.0);
            MgCurveString geom3 = build.CreateCurveString(2.0);

            MgCurveStringCollection coll = new MgCurveStringCollection();
            coll.Add(geom1);
            coll.Add(geom2);
            coll.Add(geom3);

            Assert.AreEqual(3, coll.Count);
            Assert.IsTrue(geom1.Equals(coll[0]));
            Assert.IsTrue(coll[0].Equals(coll[2]));
            Assert.IsFalse(coll[0].Equals(coll[1]));
            coll[0] = coll[1];
            Assert.IsTrue(coll[0].Equals(coll[1]));

            double width = 0.0;
            foreach (MgCurveString geom in coll)
            {
                width += geom.Envelope().GetWidth();
            }
            Assert.AreEqual(geom1.Envelope().GetWidth() * 3.0, width);
        }
    }

    public class CollectionTestsFeatureCommandCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgPropertyCollection propVals = new MgPropertyCollection();
            MgInt32Property prop = new MgInt32Property("prop", 1);
            propVals.Add(prop);
            MgUpdateFeatures update = new MgUpdateFeatures("class2", propVals, "where cat < dog");
            MgInsertFeatures insert = new MgInsertFeatures("class3", propVals);
            MgDeleteFeatures del = new MgDeleteFeatures("class1", "where cat > dog");

            MgFeatureCommandCollection coll = new MgFeatureCommandCollection();
            coll.Add(update);
            coll.Add(insert);
            coll.Add(del);

            Assert.AreEqual(3, coll.Count);
            Assert.AreEqual(MgFeatureCommandType.DeleteFeatures, coll[2].GetCommandType());
            coll[0] = coll[1];

            string txt = "";
            foreach (MgFeatureCommand cmd in coll)
            {
                txt += "[" + cmd.GetCommandType() + "]";
            }
            Assert.AreEqual("[0][0][2]", txt);
        }
    }

    public class CollectionTestsFeatureSchemaCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgFeatureSchema schema1 = new MgFeatureSchema("schema1", "desc1");
            MgFeatureSchema schema2 = new MgFeatureSchema("schema2", "desc2");
            MgFeatureSchema schema3 = new MgFeatureSchema("schema3", "desc3");

            MgFeatureSchemaCollection coll = new MgFeatureSchemaCollection();
            coll.Add(schema1);
            coll.Add(schema2);
            coll.Add(schema3);

            Assert.AreEqual(3, coll.Count);
            Assert.AreEqual("schema3", coll[2].GetName());
            coll[0] = coll[2];

            string txt = "";
            foreach (MgFeatureSchema schema in coll)
            {
                txt += "[" + schema.GetName() + "]";
            }
            Assert.AreEqual("[schema3][schema2][schema3]", txt);
        }
    }

    public class CollectionTestsGeometryCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var wkt = new MgWktReaderWriter();
            MgPoint geom1 = wkt.Read("POINT XY (1.0 1.0)") as MgPoint;
            MgPoint geom2 = wkt.Read("POINT XY (2.0 2.0)") as MgPoint;
            MgPoint geom3 = wkt.Read("POINT XY (1.0 1.0)") as MgPoint;

            MgGeometryCollection coll = new MgGeometryCollection();
            coll.Add(geom1);
            coll.Add(geom2);
            coll.Add(geom3);

            Assert.AreEqual(3, coll.Count);
            Assert.IsTrue(geom1.Equals(coll[0]));
            Assert.IsTrue(coll[0].Equals(coll[2]));
            Assert.IsFalse(coll[0].Equals(coll[1]));
            coll[0] = coll[1];
            Assert.IsTrue(coll[0].Equals(coll[1]));

            double x = 0.0;
            foreach (MgGeometry geom in coll)
            {
                x += (geom as MgPoint).GetCoordinate().GetX();
            }
            Assert.AreEqual(5.0, x);
        }
    }

    public class CollectionTestsLayerCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            logger.WriteLine("Skipping CollectionTestsLayerCollection: Resource Service required to construct MgLayer");
        }
    }

    public class CollectionTestsLayerGroupCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            logger.WriteLine("Skipping CollectionTestsLayerCollection: MgLayerGroupCollection constructor not visible");
            /*
            MgLayerGroup group1 = new MgLayerGroup("group1");
            MgLayerGroup group2 = new MgLayerGroup("group2");
            MgLayerGroup group3 = new MgLayerGroup("group3");
            MgLayerGroup group4 = new MgLayerGroup("group4");

            MgLayerGroupCollection coll = new MgLayerGroupCollection();
            coll.Add(group1);
            coll.Insert(1, group2);
            coll.Add(group3);
            coll.Insert(3, group4);

            Assert.AreEqual(4, coll.Count);
            Assert.AreEqual(coll[2].GetName(), "group3");

            coll[1] = coll[2];

            string txt = "";
            foreach (MgLayerGroup group in coll)
            {
                txt += "[" + group.GetName() + "]";
            }
            Assert.AreEqual("[group1][group3][group3][group4]", txt);
            */
        }
    }

    public class CollectionTestsLinearRingCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var build = new GeomBuild();
            MgLinearRing geom1 = build.CreateLinearRing(1.0);
            MgLinearRing geom2 = build.CreateLinearRing(5.0);
            MgLinearRing geom3 = build.CreateLinearRing(1.0);

            MgLinearRingCollection coll = new MgLinearRingCollection();
            coll.Add(geom1);
            coll.Add(geom2);
            coll.Add(geom3);

            Assert.AreEqual(3, coll.Count);
            Assert.IsTrue(geom1.Envelope().Contains(coll[0].Envelope()));
            Assert.IsTrue(coll[0].Envelope().Contains(coll[2].Envelope()));
            Assert.IsFalse(coll[0].Envelope().Contains(coll[1].Envelope()));
            coll[0] = coll[1];
            Assert.IsTrue(coll[0].Envelope().Contains(coll[1].Envelope()));

            double width = 0.0;
            foreach (MgLinearRing geom in coll)
            {
                width += geom.Envelope().GetWidth();
            }
            Assert.AreEqual(geom1.Envelope().GetWidth() * 3.0, width);
        }
    }

    public class CollectionTestsLineStringCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var build = new GeomBuild();
            MgLineString geom1 = build.CreateLineString(5.0);
            MgLineString geom2 = build.CreateLineString(11.0);
            MgLineString geom3 = build.CreateLineString(5.0);

            MgLineStringCollection coll = new MgLineStringCollection();
            coll.Add(geom1);
            coll.Add(geom2);
            coll.Add(geom3);

            Assert.AreEqual(3, coll.Count);
            Assert.IsTrue(geom1.Equals(coll[0]));
            Assert.IsTrue(coll[0].Equals(coll[2]));
            Assert.IsFalse(coll[0].Equals(coll[1]));
            coll[0] = coll[1];
            Assert.IsTrue(coll[0].Equals(coll[1]));

            double width = 0.0;
            foreach (MgLineString geom in coll)
            {
                width += geom.Envelope().GetWidth();
            }
            Assert.AreEqual(geom1.Envelope().GetWidth() * 3.0, width);
        }
    }

    public class CollectionTestsMapCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            string coordsys = "GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]]";
            MgEnvelope env = new MgEnvelope(10, 10, 20, 20);



            MgMapBase map1 = factory.CreateMap(coordsys, env, "map1");
            MgMapBase map2 = factory.CreateMap(coordsys, env, "map2");
            MgMapBase map3 = factory.CreateMap(coordsys, env, "map3");
            MgMapBase map4 = factory.CreateMap(coordsys, env, "map4");

            MgMapCollection coll = new MgMapCollection();
            coll.Add(map1);
            coll.Insert(1, map2);
            coll.Add(map3);

            Assert.AreEqual(3, coll.Count);
            Assert.AreEqual(coll[2].GetName(), "map3");

            coll[1] = map4;


            string txt = "";
            foreach (MgMapBase map in coll)
            {
                txt += "[" + map.GetName() + "]";
            }
            Assert.AreEqual("[map1][map4][map3]", txt);
        }
    }

    public class CollectionTestsPointCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var wkt = new MgWktReaderWriter();
            MgPoint geom1 = wkt.Read("POINT XY (1.0 1.0)") as MgPoint;
            MgPoint geom2 = wkt.Read("POINT XY (2.0 2.0)") as MgPoint;
            MgPoint geom3 = wkt.Read("POINT XY (1.0 1.0)") as MgPoint;

            MgPointCollection coll = new MgPointCollection();
            coll.Add(geom1);
            coll.Add(geom2);
            coll.Add(geom3);

            Assert.AreEqual(3, coll.Count);
            Assert.IsTrue(geom1.Equals(coll[0]));
            Assert.IsTrue(coll[0].Equals(coll[2]));
            Assert.IsFalse(coll[0].Equals(coll[1]));
            coll[0] = coll[1];
            Assert.IsTrue(coll[0].Equals(coll[1]));

            double x = 0.0;
            foreach (MgPoint geom in coll)
            {
                x += geom.GetCoordinate().GetX();
            }
            Assert.AreEqual(5.0, x);
        }
    }

    public class CollectionTestsPolygonCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var build = new GeomBuild();
            MgPolygon geom1 = build.CreatePolygon(2.0);
            MgPolygon geom2 = build.CreatePolygon(12.0);
            MgPolygon geom3 = build.CreatePolygon(2.0);

            MgPolygonCollection coll = new MgPolygonCollection();
            coll.Add(geom1);
            coll.Add(geom2);
            coll.Add(geom3);

            Assert.AreEqual(3, coll.Count);
            Assert.IsTrue(geom1.Equals(coll[0]));
            Assert.IsTrue(coll[0].Equals(coll[2]));
            Assert.IsFalse(coll[0].Equals(coll[1]));
            coll[0] = coll[1];
            Assert.IsTrue(coll[0].Equals(coll[1]));

            double width = 0.0;
            foreach (MgPolygon geom in coll)
            {
                width += geom.Envelope().GetWidth();
            }
            Assert.AreEqual(geom1.Envelope().GetWidth() * 3.0, width);
        }
    }

    public class CollectionTestsPropertyDefinitionCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgDataPropertyDefinition def1 = new MgDataPropertyDefinition("def1");
            MgDataPropertyDefinition def2 = new MgDataPropertyDefinition("def2");
            MgDataPropertyDefinition def3 = new MgDataPropertyDefinition("def3");
            MgDataPropertyDefinition def4 = new MgDataPropertyDefinition("def4");

            MgPropertyDefinitionCollection coll = new MgPropertyDefinitionCollection();
            coll.Add(def1);
            coll.Add(def2);
            coll.Insert(2, def4);
            coll.Insert(2, def3);

            Assert.AreEqual(4, coll.Count);
            Assert.AreEqual("def1", coll[0].GetName());

            MgPropertyDefinition tmp = coll[0];
            coll.Remove(def1);
            Assert.AreEqual(3, coll.Count);
            coll.Insert(0, tmp);


            string txt = "";
            foreach (MgPropertyDefinition def in coll)
            {
                txt += "[" + def.GetName() + "]";
            }
            Assert.AreEqual("[def1][def2][def3][def4]", txt);
        }
    }

    public class CollectionTestsReadOnlyLayerCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            logger.WriteLine("Skipping CollectionTestsReadOnlyLayerCollection: Resource Service required to construct MgLayer");
        }
    }

    public class CollectionTestsStringCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            StringCollection strColl = new StringCollection();
            strColl.Add("string1");
            strColl.Add("string2");
            strColl.Add("string3");
            strColl.Add("string3");

            MgStringCollection coll1 = new MgStringCollection();
            coll1.Add("Hello");

            MgStringCollection coll2 = new MgStringCollection(strColl);
            Assert.AreEqual(4, coll2.GetCount());
            StringCollection coll3 = coll2;
            Assert.AreEqual(4, coll3.Count);
            for (int i = 0; i < 4; i++)
            {
                Assert.AreEqual(strColl[i], coll2.GetItem(i));
                Assert.AreEqual(strColl[i], coll3[i]);
            }

        }
    }

    public class CollectionTestsStringPropertyCollection : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgStringProperty prop1 = new MgStringProperty("prop1", "val1");
            MgStringProperty prop2 = new MgStringProperty("prop2", "val2");
            MgStringProperty prop3 = new MgStringProperty("prop3", "val3");
            MgStringProperty prop4 = new MgStringProperty("prop4", "val4");

            MgStringPropertyCollection coll = new MgStringPropertyCollection();
            coll.Add(prop1);
            coll.Add(prop2);
            coll.Remove(prop3);
            coll.Remove(coll[1]);
            Assert.AreEqual(1, coll.Count);
            coll.Add(prop3);
            coll[1] = prop2;
            coll.Insert(2, prop3);
            Assert.AreEqual(2, coll.IndexOf(prop3));
            coll.Add(prop4);

            string txt = "";
            foreach (MgStringProperty prop in coll)
            {
                txt += "[" + prop.GetName() + "]";
            }
            Assert.AreEqual("[prop1][prop2][prop3][prop4]", txt);
        }
    }
}
