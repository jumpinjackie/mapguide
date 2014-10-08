using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Common.ExternalTests
{
    public class PropertiesTestColor : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestDateTime : IExternalTest 
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestByteReader : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgByteSource source = new MgByteSource("../../TestData/DrawingService/SpaceShip.dwf");
            source.MimeType = MgMimeType.Dwf;
            MgByteReader reader = source.GetReader();

            Assert.AreEqual(MgMimeType.Dwf, reader.MimeType);
        }
    }

    public class PropertiesTestByteSource : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            string tmp = Path.GetTempFileName();
            try
            {
                File.WriteAllText(tmp, "Hello World");

                MgByteSource source = new MgByteSource(tmp);
                source.MimeType = "text/plain";

                Assert.AreEqual(MgMimeType.Text, source.MimeType);   
            }
            finally
            {
                try
                {
                    File.Delete(tmp);
                }
                catch { }
            }
        }
    }

    public class PropertiesTestPropertyDefinition : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgPropertyDefinition propDef = new MgPropertyDefinition("TestProp", MgPropertyType.Int32);
            propDef.Description = "description";
            propDef.QualifiedName = "qualifiedName";

            Assert.AreEqual("TestProp", propDef.Name);
            Assert.AreEqual(MgPropertyType.Int32, propDef.PropertyType);
            Assert.AreEqual("description", propDef.Description);
            Assert.AreEqual("qualifiedName", propDef.QualifiedName);
        }
    }

    public class PropertiesTestStringProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgStringProperty sp = new MgStringProperty("TestStringName", "TestStringValue");

            Assert.AreEqual("TestStringValue", sp.Value);
            Assert.AreEqual("TestStringName", sp.Name);
        }
    }

    public class PropertiesTestBlobProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            string tmp = Path.GetTempFileName();
            try
            {
                File.WriteAllText(tmp, "Hello World");

                MgByteSource bs = new MgByteSource(tmp);
                MgByteReader reader = bs.GetReader();
                MgBlobProperty bp = new MgBlobProperty("BlobPropName", reader);

                Assert.AreEqual("BlobPropName", bp.Name);
                Assert.Greater((int)bp.Value.GetLength(), 0);
                Assert.AreEqual(reader.GetLength(), bp.Value.GetLength());
                Assert.AreEqual(MgPropertyType.Blob, bp.PropertyType);

                MgByteSource bs2 = new MgByteSource("../../TestData/DrawingService/SpaceShip.dwf");
                MgByteReader reader2 = bs2.GetReader();
                bp.Value = reader2;
                Assert.AreEqual(reader2.GetLength(), bp.Value.GetLength());
            }
            finally
            {
                try
                {
                    File.Delete(tmp);
                }
                catch { }
            }
        }
    }

    public class PropertiesTestBooleanProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgBooleanProperty bp = new MgBooleanProperty("BoolPropName", true);

            Assert.IsTrue(bp.Value);
            Assert.AreEqual("BoolPropName", bp.Name);
            Assert.AreEqual(MgPropertyType.Boolean, bp.PropertyType);

            bp.Value = false;
            Assert.IsFalse(bp.Value);
        }
    }

    public class PropertiesTestByteProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            byte bpv = 123;
            MgByteProperty bp = new MgByteProperty("BytePropName", bpv);

            Assert.AreEqual("BytePropName", bp.Name);
            Assert.AreEqual(bpv, bp.Value);
            Assert.AreEqual(MgPropertyType.Byte, bp.PropertyType);
        }
    }

    public class PropertiesTestClobProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            string tmp = Path.GetTempFileName();
            try
            {
                File.WriteAllText(tmp, "Hello World");

                MgByteSource bs = new MgByteSource(tmp);
                MgByteReader reader = bs.GetReader();
                MgClobProperty cp = new MgClobProperty("ClobPropName", reader);

                Assert.AreEqual("ClobPropName", cp.Name);
                Assert.Greater((int)cp.Value.GetLength(), 0);
                Assert.AreEqual(reader.GetLength(), cp.Value.GetLength());
                Assert.AreEqual(MgPropertyType.Clob, cp.PropertyType);

                MgByteSource bs2 = new MgByteSource("../../TestData/DrawingService/SpaceShip.dwf");
                MgByteReader reader2 = bs2.GetReader();
                cp.Value = reader2;
                Assert.AreEqual(reader2.GetLength(), cp.Value.GetLength());
            }
            finally
            {
                try
                {
                    File.Delete(tmp);
                }
                catch { }
            }
        }
    }

    public class PropertiesTestDateTimeProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestDoubleProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgDoubleProperty dp = new MgDoubleProperty("DoublePropName", 4.251973);

            Assert.AreEqual("DoublePropName", dp.Name);
            Assert.AreEqual(MgPropertyType.Double, dp.PropertyType);
            Assert.AreEqual(4.251973, dp.Value);
        }
    }

    public class PropertiesTestInt16Property : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgInt16Property ip = new MgInt16Property("Int16PropName", -32768);

            Assert.AreEqual("Int16PropName", ip.Name);
            Assert.AreEqual(MgPropertyType.Int16, ip.PropertyType);
            Assert.AreEqual(Int16.MinValue, ip.Value);
        }
    }

    public class PropertiesTestInt32Property : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgInt32Property ip = new MgInt32Property("Int32PropName", -2147483648);

            Assert.AreEqual("Int32PropName", ip.Name);
            Assert.AreEqual(MgPropertyType.Int32, ip.PropertyType);
            Assert.AreEqual(Int32.MinValue, ip.Value);
        }
    }

    public class PropertiesTestInt64Property : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgInt64Property ip = new MgInt64Property("Int64PropName", -9223372036854775808);

            Assert.AreEqual("Int64PropName", ip.Name);
            Assert.AreEqual(MgPropertyType.Int64, ip.PropertyType);
            Assert.AreEqual(Int64.MinValue, ip.Value);
        }
    }

    public class PropertiesTestSingleProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgSingleProperty sp = new MgSingleProperty("SinglePropName", 4.251973f);

            Assert.AreEqual("SinglePropName", sp.Name);
            Assert.AreEqual(MgPropertyType.Single, sp.PropertyType);
            Assert.AreEqual(4.251973f, sp.Value);
        }
    }

    public class PropertiesTestLayerBase : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            logger.WriteLine("Skipping PropertiesTestLayerBase: Resource Service required to construct MgLayer");
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
    }

    public class PropertiesTestLayerGroup : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestMapBase : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestClassDefinition : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestFileFeatureSourceParams : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgFileFeatureSourceParams csp = new MgFileFeatureSourceParams();
            csp.ProviderName = "OSGeo.SDF";
            Assert.AreEqual("OSGeo.SDF", csp.ProviderName);

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
    }

    public class PropertiesTestDataPropertyDefinition : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestDeleteFeatures : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgDeleteFeatures df = new MgDeleteFeatures("dfClassName", "dfFilterText");
            Assert.AreEqual("dfClassName", df.FeatureClassName);
            Assert.AreEqual("dfFilterText", df.FilterText);
        }
    }

    public class PropertiesTestFeatureProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgFeatureProperty fp = new MgFeatureProperty("FeatureProp", null);
            Assert.AreEqual("FeatureProp", fp.Name);
            Assert.AreEqual(MgPropertyType.Feature, fp.PropertyType);
            Assert.IsNull(fp.Value);
        }
    }

    public class PropertiesTestFeatureSchema : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgFeatureSchema fs = new MgFeatureSchema();
            fs.Name = "fsName";
            Assert.AreEqual("fsName", fs.Name);

            fs.Description = "fsDescription";
            Assert.AreEqual("fsDescription", fs.Description);
        }
    }

    public class PropertiesTestGeometryPropertyDefinition : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestGeometryProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            string tmp = Path.GetTempFileName();
            try
            {
                File.WriteAllText(tmp, "Hello World");

                MgByteSource bs = new MgByteSource(tmp);
                MgByteReader reader = bs.GetReader();

                MgGeometryProperty gp = new MgGeometryProperty("GeomPropName", reader);
                MgByteSource bs2 = new MgByteSource("../../TestData/DrawingService/SpaceShip.dwf");
                MgByteReader reader2 = bs2.GetReader();

                gp.Value = reader2;
                Assert.AreEqual(reader2.GetLength(), gp.Value.GetLength());

                Assert.AreEqual(MgPropertyType.Geometry, gp.PropertyType);
            }
            finally
            {
                try
                {
                    File.Delete(tmp);
                }
                catch { }
            }
        }
    }

    public class PropertiesTestInsertFeatures : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgPropertyCollection propColl = new MgPropertyCollection();
            propColl.Add(new MgInt32Property("intProp", 10));
            MgInsertFeatures inf = new MgInsertFeatures("ClassName", propColl);
            Assert.AreEqual("ClassName", inf.FeatureClassName);
        }
    }

    public class PropertiesTestObjectPropertyDefinition : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestRasterProperty : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgRasterProperty rp = new MgRasterProperty("RasterPropName", null);

            Assert.AreEqual("RasterPropName", rp.Name);
            Assert.AreEqual(MgPropertyType.Raster, rp.PropertyType);
            Assert.IsNull(rp.Value);
        }
    }

    public class PropertiesTestRasterPropertyDefinition : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestResourceIdentifier : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestUpdateFeatures : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgPropertyCollection propColl = new MgPropertyCollection();
            propColl.Add(new MgInt32Property("intProp", 10));
            MgUpdateFeatures uf = new MgUpdateFeatures("ClassName", propColl, "filter");
            Assert.AreEqual("ClassName", uf.FeatureClassName);
            Assert.AreEqual("ClassName", uf.FeatureClassName);
        }
    }

    public class PropertiesTestCoordinateSystem : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
            MgCoordinateSystem coordSys = csFactory.CreateFromCode("LL84");
            Assert.AreEqual(MgCoordinateSystemType.Geographic, coordSys.Type);
            Assert.AreEqual("DEGREE", coordSys.Units);
            Assert.AreEqual(-180, coordSys.MinX);
            Assert.AreEqual(-90, coordSys.MinY);
            Assert.AreEqual(180, coordSys.MaxX);
            Assert.AreEqual(90, coordSys.MaxY);
            Assert.AreEqual("LL84", coordSys.CsCode);
            Assert.AreEqual("WGS84 datum, Latitude-Longitude; Degrees", coordSys.Description);
            Assert.AreEqual("LL", coordSys.Projection);
            Assert.AreEqual("Null Projection, produces/processes Latitude & Longitude", coordSys.ProjectionDescription);
            Assert.AreEqual("WGS84", coordSys.Datum);
            Assert.AreEqual("World Geodetic System of 1984", coordSys.DatumDescription);
            Assert.AreEqual("WGS84", coordSys.Ellipsoid);
            Assert.AreEqual("World Geodetic System of 1984, GEM 10C", coordSys.EllipsoidDescription);
        }
    }

    public class PropertiesTestArcSegment : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestCoordinateXY : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            MgGeometryFactory gf = new MgGeometryFactory();
            MgCoordinateXY coord = gf.CreateCoordinateXY(100, 100) as MgCoordinateXY;
            Assert.AreEqual(100, coord.X);
            Assert.AreEqual(100, coord.Y);
            MgCoordinate coord2 = coord;
            Assert.AreEqual(100, coord2.X);
            Assert.AreEqual(100, coord2.Y);
        }
    }

    public class PropertiesTestCoordinateXYM : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestCoordinateXYZ : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestCoordinateXYZM : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestCurvePolygon : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestCurveRing : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestCurveString : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestEnvelope : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestLinearRing : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestLinearSegment : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestLineString : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestMultiCurvePolygon : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestMultiCurveString : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestMultiGeometry : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestMultiLineString : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestMultiPoint : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
    }

    public class PropertiesTestPolygon : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
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
