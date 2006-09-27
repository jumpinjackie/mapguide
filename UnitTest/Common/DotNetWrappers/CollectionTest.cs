using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Text;
using NUnit.Framework;
using OSGeo.MapGuide;

namespace OSGeo.MapGuide
{  
    [TestFixture]
    public class CollectionTest
    {
        public MgGeometryFactory factory;
        public MgWktReaderWriter wkt;
        public GeomBuild build;

        [SetUp]
        public void InitMapGuide()
        {
            MapGuideApi.MgInitializeWebTier("d:/mgdev/web/src/webconfig.ini");
            factory = new MgGeometryFactory();
            wkt = new MgWktReaderWriter();
            build = new GeomBuild();
        }

        [Test]
        public void IntCollection()
        {
            MgIntCollection coll = new MgIntCollection();
            coll.Add(10);
            coll.Add(20);
            coll.Add(30);
            coll.Add(40);
            coll.Add(50);
            Assert.AreEqual(20,coll.GetItem(1));
            Assert.AreEqual(30, coll[2]);
            Assert.AreEqual(5, coll.Count);

            int j = 0;
            foreach (int i in coll)
            {
                j += i;
            }
            Assert.AreEqual(150, j);
        }

        [Test]
        public void PropertyCollection()
        {
            MgPropertyCollection coll = new MgPropertyCollection();
            MgDoubleProperty dblProp = new MgDoubleProperty("DoubleProp", 1.1111);
            MgInt32Property intProp = new MgInt32Property("IntProp", 1);
            MgDateTime dateTime = new MgDateTime(2006, 9, 21);
            MgDateTimeProperty dateProp = new MgDateTimeProperty("DateProp", dateTime);
            MgSingleProperty single = new MgSingleProperty("SingleProp", (float) 2.2222);
            coll.Add(dblProp);
            coll.Add(intProp);
            coll.Add(dateProp);
            coll[2] = single;

            Assert.AreEqual(1.1111, (coll[0] as MgDoubleProperty).GetValue());
            Assert.AreEqual(MgPropertyType.Double, coll[0].GetPropertyType());
            Assert.AreEqual(MgPropertyType.Int32, coll[1].GetPropertyType());
            Assert.AreEqual(MgPropertyType.Single, coll[2].GetPropertyType());
            Assert.AreEqual((float) 2.2222, (coll[2] as MgSingleProperty).GetValue());
            Assert.AreEqual(3, coll.Count);

            Assert.AreEqual(MgPropertyType.Double, coll[0].GetPropertyType());

            string str = "";
            foreach (MgProperty prop in coll)
            {
                str = str+"["+prop.GetName()+"]";
            }
            Assert.AreEqual("[DoubleProp][IntProp][SingleProp]", str);
        }

        [Test]
        public void BatchPropertyCollection()
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

        [Test]
        public void ClassDefinitionCollection()
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

        [Test]
        public void CoordinateCollection()
        {
            MgCoordinate c1 = factory.CreateCoordinateXY(1.0, 1.0);
            MgCoordinate c2 = factory.CreateCoordinateXY(2.0, 2.0);
            MgCoordinate c3 = factory.CreateCoordinateXY(3.0, 3.0);
            MgCoordinate c4 = factory.CreateCoordinateXY(4.0, 4.0);

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

        [Test]
        public void CurvePolygonCollection()
        {
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
            Assert.AreEqual(geom1.Envelope().GetWidth()*3.0, width);
        }

        [Test]
        public void CurveRingCollection()
        {
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

        [Test]
        public void CurveSegmentCollection()
        {
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

        [Test]
        public void CurveStringCollection()
        {
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

        [Test]
        public void FeatureCommandCollection()
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

        [Test]
        public void FeatureSchemaCollection()
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

        [Test]
        public void GeometryCollection()
        {
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

        [Test]
        public void LayerCollection()
        {
            Assert.Ignore("Resource Service required to construct MgLayer");
        }

        [Test]
        public void LayerGroupCollection()
        {
            Assert.Ignore("MgLayerGroupCollection constructor not visible");
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

        [Test]
        public void LinearRingCollection()
        {
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

        [Test]
        public void LineStringCollection()
        {
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

        [Test]
        public void MapCollection()
        {
            string coordsys = "GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]]";
            MgEnvelope env = new MgEnvelope(10,10,20,20);
            MgMap map1 = new MgMap();
            map1.Create(coordsys, env, "map1");
            MgMap map2 = new MgMap();
            map2.Create(coordsys, env, "map2");
            MgMap map3 = new MgMap();
            map3.Create(coordsys, env, "map3");
            MgMap map4 = new MgMap();
            map4.Create(coordsys, env, "map4");

            MgMapCollection coll = new MgMapCollection();
            coll.Add(map1);
            coll.Insert(1, map2);
            coll.Add(map3);

            Assert.AreEqual(3, coll.Count);
            Assert.AreEqual(coll[2].GetName(), "map3");

            coll[1] = map4;


            string txt = "";
            foreach (MgMap map in coll)
            {
                txt += "[" + map.GetName() + "]";
            }
            Assert.AreEqual("[map1][map4][map3]", txt);
        }

        [Test]
        public void MapPlotCollection()
        {
            string coordsys = "GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]]";
            MgEnvelope env = new MgEnvelope(10,10,20,20);
            MgMap map1 = new MgMap();
            map1.Create(coordsys, env, "map1");

            MgMap map2 = new MgMap();
            map2.Create(coordsys, env, "map2");

            MgMap map3 = new MgMap();
            map3.Create(coordsys, env, "map3");

            MgPlotSpecification spec = new MgPlotSpecification((float)8.0, (float)11.0, "in", (float)0.0, (float)0.0, (float)5.0, (float)5.0);
            MgResourceIdentifier resId = new MgResourceIdentifier("Library://test.WebLayout");
            MgLayout layout1 = new MgLayout(resId, "Title1", "in");
            MgMapPlot plot1 = new MgMapPlot(map1, spec, layout1);

            MgLayout layout2 = new MgLayout(resId, "Title2", "in");
            MgMapPlot plot2 = new MgMapPlot(map2, spec, layout2);

            MgLayout layout3 = new MgLayout(resId, "Title3", "in");
            MgMapPlot plot3 = new MgMapPlot(map3, spec, layout3);

            MgMapPlotCollection coll = new MgMapPlotCollection();
            coll.Add(plot1);
            coll.Add(plot2);
            coll.Remove(plot1);

            Assert.AreEqual(1, coll.Count);

            coll.Insert(0, plot1);
            coll.Insert(2, plot3);
            MgMapPlot tmp = coll[2];
            coll[2] = tmp;

            Assert.AreEqual(3, coll.Count);


            string txt = "";
            foreach (MgMapPlot plot in coll)
            {
                txt += "[" + plot.GetLayout().GetTitle() + "]";
            }
            Assert.AreEqual("[Title1][Title2][Title3]", txt);
        }

        [Test]
        public void PointCollection()
        {
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

        [Test]
        public void PolygonCollection()
        {
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

        [Test]
        public void PropertyDefinitionCollection()
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

        [Test]
        public void ReadOnlyLayerCollection()
        {
            Assert.Ignore("Resource Service required to construct MgLayer");
        }

        [Test]
        public void StringCollection()
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

        [Test]
        public void StringPropertyCollection()
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

