using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using OSGeo.MapGuide;

namespace OSGeo.MapGuide
{
    [TestFixture]
    public class PropertiesTest
    {
        [SetUp]
        public void InitMapGuide()
        {
            MapGuideApi.MgInitializeWebTier("d:/mgdev/web/src/webconfig.ini");
        }

        // Foundation

        [Test]
        public void Color()
        {
            short nRed = 22;
            short nGreen = 33;
            short nBlue = 44;
            short nAlpha = 55;
            MgColor color = new MgColor(nRed, nGreen, nBlue, nAlpha);
            Assert.AreEqual(nRed, color.Red);
            Assert.AreEqual(nGreen, color.Green);
            Assert.AreEqual(nBlue, color.Blue);
            Assert.AreEqual(nAlpha, color.Alpha);
            String scolor = nRed.ToString("x") + nGreen.ToString("x") + nBlue.ToString("x") + nAlpha.ToString("x");
            Assert.AreEqual(scolor, color.Color);
        }

        [Test]
        public void DateTime()
        {
            MgDateTime dt = new MgDateTime();
            dt.Year = 2006;
            dt.Month = 9;
            dt.Day = 21;
            dt.Hour = 16;
            dt.Minute = 4;
            dt.Second = 25;
            dt.Microsecond = 99;

            Assert.AreEqual(2006, dt.Year);
            Assert.AreEqual(9, dt.Month);
            Assert.AreEqual(21, dt.Day);
            Assert.AreEqual(16, dt.Hour);
            Assert.AreEqual(4, dt.Minute);
            Assert.AreEqual(25, dt.Second);
            Assert.AreEqual(99, dt.Microsecond);
        }

        [Test]
        public void ByteReader()
        {
            MgByteSource source = new MgByteSource("d:/MgDev/UnitTest/TestData/DrawingService/SpaceShip.dwf");
            source.MimeType = MgMimeType.Dwf;
            MgByteReader reader = source.GetReader();

            Assert.AreEqual(MgMimeType.Dwf, reader.MimeType);
        }

        [Test]
        public void ByteSource()
        {
            MgByteSource source = new MgByteSource("d:/mgdev/web/src/webconfig.ini");
            source.MimeType = "text/plain";

            Assert.AreEqual(MgMimeType.Text, source.MimeType);
        }

        [Test]
        public void PropertyDefinition()
        {
            MgPropertyDefinition propDef = new MgPropertyDefinition("TestProp", MgPropertyType.Int32);
            propDef.Description = "description";
            propDef.QualifiedName = "qualifiedName";

            Assert.AreEqual("TestProp", propDef.Name);
            Assert.AreEqual(MgPropertyType.Int32, propDef.PropertyType);
            Assert.AreEqual("description", propDef.Description);
            Assert.AreEqual("qualifiedName", propDef.QualifiedName);
        }

        [Test]
        public void StringProperty()
        {
            MgStringProperty sp = new MgStringProperty("TestStringName", "TestStringValue");

            Assert.AreEqual("TestStringValue", sp.Value);
            Assert.AreEqual("TestStringName", sp.Name);
            Assert.AreEqual(MgPropertyType.String, sp.PropertyType);
        }

        // PlatformBase

        [Test]
        public void BlobProperty()
        {
            MgByteSource bs = new MgByteSource("d:/mgdev/web/src/webconfig.ini");
            MgByteReader reader = bs.GetReader();
            MgBlobProperty bp = new MgBlobProperty("BlobPropName", reader);

            Assert.AreEqual("BlobPropName", bp.Name);
            Assert.Greater((int)bp.Value.GetLength(), 0);
            Assert.AreEqual(reader.GetLength(), bp.Value.GetLength());
            Assert.AreEqual(MgPropertyType.Blob, bp.PropertyType);

            MgByteSource bs2 = new MgByteSource("d:/MgDev/UnitTest/TestData/DrawingService/SpaceShip.dwf");
            MgByteReader reader2 = bs2.GetReader();
            bp.Value = reader2;
            Assert.AreEqual(reader2.GetLength(), bp.Value.GetLength());
        }

        [Test]
        public void BooleanProperty()
        {
            MgBooleanProperty bp = new MgBooleanProperty("BoolPropName", true);

            Assert.IsTrue(bp.Value);
            Assert.AreEqual("BoolPropName", bp.Name);
            Assert.AreEqual(MgPropertyType.Boolean, bp.PropertyType);

            bp.Value = false;
            Assert.IsFalse(bp.Value);
        }

        [Test]
        public void ByteProperty()
        {
            byte bpv = 123;
            MgByteProperty bp = new MgByteProperty("BytePropName", bpv);

            Assert.AreEqual("BytePropName", bp.Name);
            Assert.AreEqual(bpv, bp.Value);
            Assert.AreEqual(MgPropertyType.Byte, bp.PropertyType);
        }

        [Test]
        public void ClobProperty()
        {
            MgByteSource bs = new MgByteSource("d:/mgdev/web/src/webconfig.ini");
            MgByteReader reader = bs.GetReader();
            MgClobProperty cp = new MgClobProperty("ClobPropName", reader);

            Assert.AreEqual("ClobPropName", cp.Name);
            Assert.Greater((int)cp.Value.GetLength(), 0);
            Assert.AreEqual(reader.GetLength(), cp.Value.GetLength());
            Assert.AreEqual(MgPropertyType.Clob, cp.PropertyType);

            MgByteSource bs2 = new MgByteSource("d:/MgDev/UnitTest/TestData/DrawingService/SpaceShip.dwf");
            MgByteReader reader2 = bs2.GetReader();
            cp.Value = reader2;
            Assert.AreEqual(reader2.GetLength(), cp.Value.GetLength());
        }

        [Test]
        public void DateTimeProperty()
        {
            MgDateTime dt = new MgDateTime(2006, 9, 22, 8, 33, 44, 55);
            MgDateTimeProperty dtp = new MgDateTimeProperty("DateTimePropName", dt);

            Assert.AreEqual("DateTimePropName", dtp.Name);
            Assert.AreEqual(MgPropertyType.DateTime, dtp.PropertyType);
            Assert.AreEqual(dt.Year, dtp.Value.Year);
            Assert.AreEqual(dt.Month, dtp.Value.Month);
            Assert.AreEqual(dt.Day, dtp.Value.Day);
            Assert.AreEqual(dt.Hour, dtp.Value.Hour);
            Assert.AreEqual(dt.Minute, dtp.Value.Minute);
            Assert.AreEqual(dt.Second, dtp.Value.Second);
            Assert.AreEqual(dt.Microsecond, dtp.Value.Microsecond);
        }

        [Test]
        public void DoubleProperty()
        {
            MgDoubleProperty dp = new MgDoubleProperty("DoublePropName", 4.251973);

            Assert.AreEqual("DoublePropName", dp.Name);
            Assert.AreEqual(MgPropertyType.Double, dp.PropertyType);
            Assert.AreEqual(4.251973, dp.Value);
        }

        [Test]
        public void Int16Property()
        {
            MgInt16Property ip = new MgInt16Property("Int16PropName", -32768);

            Assert.AreEqual("Int16PropName", ip.Name);
            Assert.AreEqual(MgPropertyType.Int16, ip.PropertyType);
            Assert.AreEqual(Int16.MinValue, ip.Value);
        }

        [Test]
        public void Int32Property()
        {
            MgInt32Property ip = new MgInt32Property("Int32PropName", -2147483648);

            Assert.AreEqual("Int32PropName", ip.Name);
            Assert.AreEqual(MgPropertyType.Int32, ip.PropertyType);
            Assert.AreEqual(Int32.MinValue, ip.Value);
        }

        [Test]
        public void Int64Property()
        {
            MgInt64Property ip = new MgInt64Property("Int64PropName", -9223372036854775808);

            Assert.AreEqual("Int64PropName", ip.Name);
            Assert.AreEqual(MgPropertyType.Int64, ip.PropertyType);
            Assert.AreEqual(Int64.MinValue, ip.Value);
        }

        [Test]
        public void SingleProperty()
        {
            MgSingleProperty sp = new MgSingleProperty("SinglePropName",  (float)4.251973);

            Assert.AreEqual("SinglePropName", sp.Name);
            Assert.AreEqual(MgPropertyType.Single, sp.PropertyType);
            Assert.AreEqual(4.251973, sp.Value);
        }

        [Test]
        public void LayerBase()
        {
            Assert.Ignore("Resource Service required to construct MgLayer");

            //MgUserInformation user = new MgUserInformation("Administrator", "admin");
            //MgSiteConnection siteConnection = new MgSiteConnection();
            //siteConnection.Open(user);
            //MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);

            //MgResourceIdentifier id = new MgResourceIdentifier("Library://UnitTests/Data/Shuttle.DrawingSource");

            //MgLayerBase lb = new MgLayerBase(id, resourceService);

            //lb.Name = "TestLayerName";
            //Assert.AreEqual("TestLayerName", lb.Name);

            //Assert.AreEqual(MgLayerType.Dynamic, lb.LayerType);

            //MgLayerGroup lg = new MgLayerGroup("TestLayerGroup");
            //lb.Group = lg;
            //Assert.AreEqual(lg.Name, lb.Group.Name);

            //lb.Visible = true;
            //Assert.AreEqual(true, lb.Visible);
            //lb.Visible = false;
            //Assert.AreEqual(false, lb.Visible);

            //lb.Selectable = true;
            //Assert.AreEqual(true, lb.Selectable);
            //lb.Selectable = false;
            //Assert.AreEqual(false, lb.Selectable);

            //Assert.AreEqual(id.ToString(), lb.LayerDefinition.ToString());

            //lb.DisplayInLegend = true;
            //Assert.AreEqual(true, lb.DisplayInLegend);
            //lb.DisplayInLegend = false;
            //Assert.AreEqual(false, lb.DisplayInLegend);

            //Assert.AreEqual(false, lb.ExpandInLegend);

            //lb.LegendLabel = "TestLegendLabel";
            //Assert.AreEqual("TestLegendLabel", lb.LegendLabel);

            //Assert.AreEqual("", lb.FeatureSourceId);
            //Assert.AreEqual("", lb.FeatureClassName);
        }

        [Test]
        public void LayerGroup()
        {
            MgLayerGroup lg = new MgLayerGroup("TestLayerGroupName");

            Assert.AreEqual("TestLayerGroupName", lg.Name);
            Assert.AreEqual(MgLayerGroupType.Normal, lg.LayerGroupType);

            lg.Group = new MgLayerGroup("AnotherTestLayerGroupName");
            Assert.AreEqual("AnotherTestLayerGroupName", lg.Group.Name);

            lg.Visible = true;
            Assert.IsTrue(lg.Visible);
            lg.Visible = false;
            Assert.IsFalse(lg.Visible);

            lg.DisplayInLegend = true;
            Assert.IsTrue(lg.DisplayInLegend);
            lg.DisplayInLegend = false;
            Assert.IsFalse(lg.DisplayInLegend);

            Assert.IsFalse(lg.ExpandInLegend);

            lg.LegendLabel = "TestLegendLabel";
            Assert.AreEqual("TestLegendLabel", lg.LegendLabel);
        }

        [Test]
        public void MapBase()
        {
            MgMapBase mb = new MgMapBase();
            Assert.AreEqual("", mb.Name);
            Assert.AreEqual("", mb.SessionId);
            Assert.IsNull(mb.MapDefinition);
            Assert.AreEqual("", mb.MapSRS);
            Assert.IsNull(mb.MapExtent);
            Assert.IsNull(mb.ViewCenter);
            Assert.AreEqual(1.0, mb.ViewScale);
            Assert.IsNull(mb.DataExtent);
            Assert.AreEqual(96, mb.DisplayDpi);
            Assert.AreEqual(0, mb.DisplayWidth);
            Assert.AreEqual(0, mb.DisplayHeight);
            Assert.AreEqual(0, mb.FiniteDisplayScaleCount);
        }

        [Test]
        public void ClassDefinition()
        {
            MgClassDefinition classDef = new MgClassDefinition();
            classDef.DefaultGeometryPropertyName = "TestDefaultGeometryPropName";
            Assert.AreEqual("TestDefaultGeometryPropName", classDef.DefaultGeometryPropertyName);

            classDef.Name = "TestName";
            Assert.AreEqual("TestName", classDef.Name);

            classDef.Description = "TestDescription";
            Assert.AreEqual("TestDescription", classDef.Description);

            Assert.IsNull(classDef.BaseClassDefinition);
        }

        [Test]
        public void CreateSdfParams()
        {
            MgCreateSdfParams csp = new MgCreateSdfParams();
            csp.SpatialContextName = "TestSpatialContextName";
            Assert.AreEqual("TestSpatialContextName", csp.SpatialContextName);

            csp.SpatialContextDescription = "TestSpatialContextDescription";
            Assert.AreEqual("TestSpatialContextDescription", csp.SpatialContextDescription);

            csp.CoordinateSystemWkt = "TestCoordSysWkt";
            Assert.AreEqual("TestCoordSysWkt", csp.CoordinateSystemWkt);

            csp.XYTolerance = 0.0001;
            Assert.AreEqual(0.0001, csp.XYTolerance);

            csp.ZTolerance = 0.01;
            Assert.AreEqual(0.01, csp.ZTolerance);

            MgFeatureSchema schema = new MgFeatureSchema("SchemaName", "SchemaDescription");
            csp.FeatureSchema = schema;
            Assert.AreEqual(schema.Name, csp.FeatureSchema.Name);
            Assert.AreEqual(schema.Description, csp.FeatureSchema.Description);
        }

        [Test]
        public void DataPropertyDefinition()
        {
            MgDataPropertyDefinition dpd = new MgDataPropertyDefinition("DataPropDefName");

            dpd.DataType = MgPropertyType.Int32;
            Assert.AreEqual(MgPropertyType.Int32, dpd.DataType);

            dpd.Length = 0;
            Assert.AreEqual(0, dpd.Length);

            dpd.Precision = 0;
            Assert.AreEqual(0, dpd.Precision);

            dpd.Scale = 0;
            Assert.AreEqual(0, dpd.Scale);

            dpd.DefaultValue = "123";
            Assert.AreEqual("123", dpd.DefaultValue);

            dpd.Nullable = true;
            Assert.IsTrue(dpd.Nullable);

            dpd.ReadOnly = false;
            Assert.IsFalse(dpd.ReadOnly);
        }

        [Test]
        public void DeleteFeatures()
        {
            MgDeleteFeatures df = new MgDeleteFeatures("dfClassName", "dfFilterText");
            Assert.AreEqual("dfClassName", df.FeatureClassName);
            Assert.AreEqual("dfFilterText", df.FilterText);
        }

        [Test]
        public void FeatureProperty()
        {
            MgFeatureProperty fp = new MgFeatureProperty("FeatureProp", null);
            Assert.AreEqual("FeatureProp", fp.Name);
            Assert.AreEqual(MgPropertyType.Feature, fp.PropertyType);
            Assert.IsNull(fp.Value);
        }

        [Test]
        public void FeatureSchema()
        {
            MgFeatureSchema fs = new MgFeatureSchema();
            fs.Name = "fsName";
            Assert.AreEqual("fsName", fs.Name);

            fs.Description = "fsDescription";
            Assert.AreEqual("fsDescription", fs.Description);
        }

        [Test]
        public void GeometryPropertyDefinition()
        {
            MgGeometricPropertyDefinition gpd = new MgGeometricPropertyDefinition("gdpName");

            gpd.GeometryTypes = MgGeometryType.LineString;
            Assert.AreEqual(MgGeometryType.LineString, gpd.GeometryTypes);

            gpd.ReadOnly = false;
            Assert.IsFalse(gpd.ReadOnly);
            gpd.ReadOnly = true;
            Assert.IsTrue(gpd.ReadOnly);

            gpd.HasElevation = true;
            Assert.IsTrue(gpd.HasElevation);

            gpd.HasMeasure = true;
            Assert.IsTrue(gpd.HasMeasure);

            gpd.SpatialContextAssociation = "spatialContextAssociation";
            Assert.AreEqual("spatialContextAssociation", gpd.SpatialContextAssociation);
        }

        [Test]
        public void GeometryProperty()
        {
            MgByteSource bs = new MgByteSource("d:/mgdev/web/src/webconfig.ini");
            MgByteReader reader = bs.GetReader();

            MgGeometryProperty gp = new MgGeometryProperty("GeomPropName", reader);
            MgByteSource bs2 = new MgByteSource("d:/MgDev/UnitTest/TestData/DrawingService/SpaceShip.dwf");
            MgByteReader reader2 = bs2.GetReader();

            gp.Value = reader2;
            Assert.AreEqual(reader2.GetLength(), gp.Value.GetLength());

            Assert.AreEqual(MgPropertyType.Geometry, gp.PropertyType);
        }

        [Test]
        public void InsertFeatures()
        {
            MgPropertyCollection propColl = new MgPropertyCollection();
            propColl.Add(new MgInt32Property("intProp", 10));
            MgInsertFeatures inf = new MgInsertFeatures("ClassName", propColl);
            Assert.AreEqual("ClassName", inf.FeatureClassName);
        }

        [Test]
        public void LockFeatures()
        {
            
            Assert.Ignore("MgLockFeatures not visible");
        }

        [Test]
        public void LongTransactionReader()
        {
            Assert.Ignore("MgLongTransactionReader constructor not visible");
        }

        [Test]
        public void ObjectPropertyDefinition()
        {
            MgObjectPropertyDefinition opd = new MgObjectPropertyDefinition("odpName");
            MgClassDefinition classDef = new MgClassDefinition();
            classDef.Name = "testClassDef";

            opd.ClassDefinition = classDef;
            Assert.AreEqual(classDef.Name, opd.ClassDefinition.Name);

            opd.ObjectType = MgObjectPropertyType.OrderedCollection;
            Assert.AreEqual(MgObjectPropertyType.OrderedCollection, opd.ObjectType);

            opd.OrderType = MgOrderingOption.Ascending;
            Assert.AreEqual(MgOrderingOption.Ascending, opd.OrderType);

            MgDataPropertyDefinition dpd = new MgDataPropertyDefinition("dpdName");
            opd.IdentityProperty = dpd;
            Assert.AreEqual(dpd.Name, opd.IdentityProperty.Name);
        }

        [Test]
        public void Raster()
        {
            Assert.Ignore("MgRaster constructor not visible");
        }

        [Test]
        public void RasterProperty()
        {
            MgRasterProperty rp = new MgRasterProperty("RasterPropName", null);

            Assert.AreEqual("RasterPropName", rp.Name);
            Assert.AreEqual(MgPropertyType.Raster, rp.PropertyType);
            Assert.IsNull(rp.Value);
        }

        [Test]
        public void RasterPropertyDefinition()
        {
            MgRasterPropertyDefinition rpd = new MgRasterPropertyDefinition("rasterPropDef");

            rpd.ReadOnly = true;
            Assert.IsTrue(rpd.ReadOnly);

            rpd.Nullable = true;
            Assert.IsTrue(rpd.Nullable);

            rpd.DefaultImageXSize = 600;
            Assert.AreEqual(600, rpd.DefaultImageXSize);

            rpd.DefaultImageYSize = 600;
            Assert.AreEqual(600, rpd.DefaultImageYSize);

            Assert.AreEqual("rasterPropDef", rpd.Name);
        }

        [Test]
        public void ResourceIdentifier()
        {
            MgResourceIdentifier resId = new MgResourceIdentifier("Library://UnitTests/Data/Shuttle.DrawingSource");

            resId.RepositoryType = MgRepositoryType.Library;
            Assert.AreEqual(MgRepositoryType.Library, resId.RepositoryType);

            resId.RepositoryName = "resName";
            Assert.AreEqual("resName", resId.RepositoryName);

            resId.Path = "Data";
            Assert.AreEqual("Data", resId.Path);

            resId.Name = "Shuttle";
            Assert.AreEqual("Shuttle", resId.Name);

            resId.ResourceType = MgResourceType.DrawingSource;
            Assert.AreEqual(MgResourceType.DrawingSource, resId.ResourceType);
        }

        [Test]
        public void SpatialContextReader()
        {
            Assert.Ignore("MgSpatialContextReader constructor not visible");
        }

        [Test]
        public void UnlockFeatures()
        {
            Assert.Ignore("MgUnlockFeatures not visible");
        }

        [Test]
        public void UpdateFeatures()
        {
            MgPropertyCollection propColl = new MgPropertyCollection();
            propColl.Add(new MgInt32Property("intProp", 10));
            MgUpdateFeatures uf = new MgUpdateFeatures("ClassName", propColl, "filter");
            Assert.AreEqual("ClassName", uf.FeatureClassName);
            Assert.AreEqual("ClassName", uf.FeatureClassName);

        }

        [Test]
        public void Warnings()
        {
            Assert.Ignore("MgWarnings constructor not visible");
        }

        // Geometry

        [Test]
        public void CoordinateSystem()
        {
            MgCoordinateSystemFactory factory = new MgCoordinateSystemFactory();
            String wkt = "GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.0174532925199433]]";
            MgCoordinateSystem coordSys = factory.Create(wkt);
            Assert.AreEqual(MgCoordinateSystemType.Geographic, coordSys.Type);
            Assert.AreEqual("Degrees", coordSys.Units);
            Assert.AreEqual(-180, coordSys.MinX);
            Assert.AreEqual(-90, coordSys.MinY);
            Assert.AreEqual(180, coordSys.MaxX);
            Assert.AreEqual(90, coordSys.MaxY);
            Assert.AreEqual("", coordSys.Code);
            Assert.AreEqual("", coordSys.Description);
            Assert.AreEqual("Unknown", coordSys.Projection);
            Assert.AreEqual("Unknown", coordSys.ProjectionDescription);
            Assert.AreEqual("WGS_1984", coordSys.Datum);
            Assert.AreEqual("WGS_1984", coordSys.DatumDescription);
            Assert.AreEqual("WGS 84", coordSys.Ellipsoid);
            Assert.AreEqual("WGS 84", coordSys.EllipsoidDescription);
            Assert.AreEqual("", coordSys.Category);

        }

        [Test]
        public void CoordinateSystemMeasure()
        {
            MgCoordinateSystemFactory factory = new MgCoordinateSystemFactory();
            String wkt = "GEOGCS[\"LL84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.0174532925199433]]";
            MgCoordinateSystem coordSys = factory.Create(wkt);
            MgCoordinateSystemMeasure csm = new MgCoordinateSystemMeasure(coordSys);

            Assert.AreEqual(180, csm.Envelope.Height);
            Assert.AreEqual(360, csm.Envelope.Width);
        }

        [Test]
        public void ArcSegment()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinateXY start = gf.CreateCoordinateXY(0, 0) as MgCoordinateXY;
            MgCoordinateXY end = gf.CreateCoordinateXY(10, 10) as MgCoordinateXY;
            MgCoordinateXY control = gf.CreateCoordinateXY(5, 5) as MgCoordinateXY;
            MgArcSegment arcSegment = gf.CreateArcSegment(start, end, control);

            Assert.AreEqual(MgGeometryComponentType.ArcSegment, arcSegment.ComponentType);
            Assert.AreEqual(control.ToString(), arcSegment.ControlCoordinate.ToString());
            Assert.AreEqual(start.ToString(), arcSegment.StartCoordinate.ToString());
            Assert.AreEqual(end.ToString(), arcSegment.EndCoordinate.ToString());
        }

        [Test]
        public void CoordinateXY()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinateXY coord = gf.CreateCoordinateXY(100, 100) as MgCoordinateXY;
            Assert.AreEqual(100, coord.X);
            Assert.AreEqual(100, coord.Y);
            MgCoordinate coord2 = coord;
            Assert.AreEqual(100, coord2.X);
            Assert.AreEqual(100, coord2.Y);
        }

        [Test]
        public void CoordinateXYM()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinateXYM coord = gf.CreateCoordinateXYM(100, 100, 10) as MgCoordinateXYM;
            Assert.AreEqual(100, coord.X);
            Assert.AreEqual(100, coord.Y);
            Assert.AreEqual(10, coord.M);
            MgCoordinate coord2 = coord;
            Assert.AreEqual(100, coord2.X);
            Assert.AreEqual(100, coord2.Y);
            Assert.AreEqual(10, coord2.M);
        }

        [Test]
        public void CoordinateXYZ()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinateXYZ coord = gf.CreateCoordinateXYZ(100, 100, 10) as MgCoordinateXYZ;
            Assert.AreEqual(100, coord.X);
            Assert.AreEqual(100, coord.Y);
            Assert.AreEqual(10, coord.Z);
            MgCoordinate coord2 = coord;
            Assert.AreEqual(100, coord2.X);
            Assert.AreEqual(100, coord2.Y);
            Assert.AreEqual(10, coord2.Z);
        }

        [Test]
        public void CoordinateXYZM()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinateXYZM coord = gf.CreateCoordinateXYZM(100, 100, 10, 5) as MgCoordinateXYZM;
            Assert.AreEqual(100, coord.X);
            Assert.AreEqual(100, coord.Y);
            Assert.AreEqual(10, coord.Z);
            Assert.AreEqual(5, coord.M);
            MgCoordinate coord2 = coord;
            Assert.AreEqual(100, coord2.X);
            Assert.AreEqual(100, coord2.Y);
            Assert.AreEqual(10, coord2.Z);
            Assert.AreEqual(5, coord2.M);
        }

        [Test]
        public void CurvePolygon()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLinearSegment linearSegment = gf.CreateLinearSegment(coordinates);
            MgCurveSegmentCollection curveSegments = new MgCurveSegmentCollection();
            curveSegments.Add(linearSegment);
            MgCurveRing outerRing = gf.CreateCurveRing(curveSegments);
            MgCurvePolygon cp = gf.CreateCurvePolygon(outerRing, null);

            Assert.AreEqual(outerRing.ToString(), cp.ExteriorRing.ToString());
            Assert.AreEqual(0, cp.InteriorRingCount);
            Assert.AreEqual(MgGeometryType.CurvePolygon, cp.GeometryType);
            Assert.AreEqual(2, cp.Dimension);
        }

        [Test]
        public void CurveRing()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLinearSegment linearSegment = gf.CreateLinearSegment(coordinates);
            MgCurveSegmentCollection curveSegments = new MgCurveSegmentCollection();
            curveSegments.Add(linearSegment);
            MgCurveRing outerRing = gf.CreateCurveRing(curveSegments);

            Assert.AreEqual(MgGeometryComponentType.CurveRing, outerRing.ComponentType);
            Assert.AreEqual(1, outerRing.Count);
            Assert.AreEqual(2, outerRing.Dimension);
        }

        [Test]
        public void CurveString()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLinearSegment linearSegment = gf.CreateLinearSegment(coordinates);
            MgCurveSegmentCollection curveSegments = new MgCurveSegmentCollection();
            curveSegments.Add(linearSegment);
            MgCurveString curveString = gf.CreateCurveString(curveSegments);

            Assert.AreEqual(1, curveString.Count);
            Assert.AreEqual(pt1.ToString(), curveString.StartCoordinate.ToString());
            Assert.AreEqual(pt4.ToString(), curveString.EndCoordinate.ToString());
            Assert.AreEqual(MgGeometryType.CurveString, curveString.GeometryType);
            Assert.AreEqual(1, curveString.Dimension);
        }

        [Test]
        public void Envelope()
        {
            MgEnvelope env = new MgEnvelope(-180, -90, 180, 90);

            Assert.AreEqual(-180, env.LowerLeftCoordinate.GetX());
            Assert.AreEqual(-90, env.LowerLeftCoordinate.GetY());
            Assert.AreEqual(180, env.UpperRightCoordinate.GetX());
            Assert.AreEqual(90, env.UpperRightCoordinate.GetY());
            Assert.AreEqual(360, env.Width);
            Assert.AreEqual(180, env.Height);
            Assert.IsNaN(env.Depth);
        }

        [Test]
        public void LinearRing()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLinearRing linearRing = gf.CreateLinearRing(coordinates);

            Assert.AreEqual(MgGeometryComponentType.LinearRing, linearRing.ComponentType);
            Assert.AreEqual(2, linearRing.Dimension);
        }

        [Test]
        public void LinearSegment()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLinearSegment linearSegment = gf.CreateLinearSegment(coordinates);

            Assert.AreEqual(MgGeometryComponentType.LinearSegment, linearSegment.ComponentType);
            Assert.AreEqual(pt1.ToString(), linearSegment.StartCoordinate.ToString());
            Assert.AreEqual(pt4.ToString(), linearSegment.EndCoordinate.ToString());
            Assert.AreEqual(1, linearSegment.Dimension);
        }

        [Test]
        public void LineString()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLineString linestring = gf.CreateLineString(coordinates);

            Assert.AreEqual(pt1.ToString(), linestring.StartCoordinate.ToString());
            Assert.AreEqual(pt4.ToString(), linestring.EndCoordinate.ToString());
            Assert.AreEqual(MgGeometryType.LineString, linestring.GeometryType);
            Assert.AreEqual(1, linestring.Dimension);
        }

        [Test]
        public void MultiCurvePolygon()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLinearSegment linearSegment = gf.CreateLinearSegment(coordinates);
            MgCurveSegmentCollection curveSegments = new MgCurveSegmentCollection();
            MgCurveRing curveRing = gf.CreateCurveRing(curveSegments);
            MgCurvePolygon cp = gf.CreateCurvePolygon(curveRing, null);
            MgCurvePolygonCollection cpc = new MgCurvePolygonCollection();
            cpc.Add(cp);
            MgMultiCurvePolygon mcp = gf.CreateMultiCurvePolygon(cpc);

            Assert.AreEqual(1, mcp.Count);
            Assert.AreEqual(MgGeometryType.MultiCurvePolygon, mcp.GeometryType);
            Assert.AreEqual(2, mcp.Dimension);
        }

        [Test]
        public void MultiCurveString()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLinearSegment linearSegment = gf.CreateLinearSegment(coordinates);
            MgCurveSegmentCollection curveSegments = new MgCurveSegmentCollection();
            curveSegments.Add(linearSegment);
            MgCurveString curveString = gf.CreateCurveString(curveSegments);
            MgCurveStringCollection csc = new MgCurveStringCollection();
            csc.Add(curveString);
            MgMultiCurveString mcs = gf.CreateMultiCurveString(csc);

            Assert.AreEqual(1, mcs.Count);
            Assert.AreEqual(MgGeometryType.MultiCurveString, mcs.GeometryType);
            Assert.AreEqual(1, mcs.Dimension);
        }

        [Test]
        public void MultiGeometry()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLineString ls = gf.CreateLineString(coordinates);
            MgGeometryCollection geometries = new MgGeometryCollection();
            geometries.Add(ls);
            MgMultiGeometry mg = gf.CreateMultiGeometry(geometries);

            Assert.AreEqual(1, mg.Count);
            Assert.AreEqual(MgGeometryType.MultiGeometry, mg.GeometryType);
            Assert.AreEqual(4, mg.Dimension);
        }

        [Test]
        public void MultiLineString()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLineString ls = gf.CreateLineString(coordinates);
            MgLineStringCollection lineStringCollection = new MgLineStringCollection();
            lineStringCollection.Add(ls);
            MgMultiLineString mls = gf.CreateMultiLineString(lineStringCollection);

            Assert.AreEqual(1, mls.Count);
            Assert.AreEqual(MgGeometryType.MultiLineString, mls.GeometryType);
            Assert.AreEqual(1, mls.Dimension);
        }

        [Test]
        public void MultiPoint()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgPoint mgpt1 = gf.CreatePoint(pt1);
            MgPoint mgpt2 = gf.CreatePoint(pt2);
            MgPoint mgpt3 = gf.CreatePoint(pt3);
            MgPoint mgpt4 = gf.CreatePoint(pt4);
            MgPointCollection points = new MgPointCollection();
            points.Add(mgpt1);
            points.Add(mgpt2);
            points.Add(mgpt3);
            points.Add(mgpt4);
            MgMultiPoint mp = gf.CreateMultiPoint(points);

            Assert.AreEqual(4, mp.Count);
            Assert.AreEqual(MgGeometryType.MultiPoint, mp.GeometryType);
            Assert.AreEqual(0, mp.Dimension);
        }

        [Test]
        public void Polygon()
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinate pt1 = gf.CreateCoordinateXY(0, 0);
            MgCoordinate pt2 = gf.CreateCoordinateXY(0, 10);
            MgCoordinate pt3 = gf.CreateCoordinateXY(10, 10);
            MgCoordinate pt4 = gf.CreateCoordinateXY(10, 0);
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(pt1);
            coordinates.Add(pt2);
            coordinates.Add(pt3);
            coordinates.Add(pt4);
            MgLinearRing linearRing = gf.CreateLinearRing(coordinates);
            MgPolygon polygon = gf.CreatePolygon(linearRing, null);

            Assert.AreEqual(linearRing.ToString(), polygon.ExteriorRing.ToString());
            Assert.AreEqual(0, polygon.InteriorRingCount);
            Assert.AreEqual(MgGeometryType.Polygon, polygon.GeometryType);
            Assert.AreEqual(2, polygon.Dimension);
        }
    }

}