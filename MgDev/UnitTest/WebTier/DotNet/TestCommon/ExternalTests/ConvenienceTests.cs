using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OSGeo.MapGuide;

namespace OSGeo.MapGuide.Test.Common.ExternalTests
{
    public class FeatureServiceTestUtil
    {
        public static string CreateTestDataStore(MgResourceIdentifier fsId, string provider, MgFeatureService featSvc)
        {
            string scName = "Default";
            var csFactory = new MgCoordinateSystemFactory();
            string wkt = csFactory.ConvertCoordinateSystemCodeToWkt("LL84");
            var schema = new MgFeatureSchema("Default", "Default Schema");
            var cls = new MgClassDefinition();
            cls.SetName("Test");
            var classes = schema.GetClasses();
            classes.Add(cls);

            var clsProps = cls.GetProperties();
            var idProps = cls.GetIdentityProperties();

            var id = new MgDataPropertyDefinition("ID");
            id.SetAutoGeneration(true);
            id.SetDataType(MgPropertyType.Int32);

            var name = new MgDataPropertyDefinition("Name");
            name.SetDataType(MgPropertyType.String);
            name.SetLength(255);
            name.SetNullable(false);

            var geom = new MgGeometricPropertyDefinition("Geometry");
            geom.SetGeometryTypes(MgFeatureGeometricType.Point);
            geom.SetSpatialContextAssociation(scName);

            clsProps.Add(id);
            clsProps.Add(name);
            clsProps.Add(geom);

            idProps.Add(id);

            cls.SetDefaultGeometryPropertyName("Geometry");

            var fsp = new MgFileFeatureSourceParams(provider, scName, wkt, schema);
            featSvc.CreateFeatureSource(fsId, fsp);

            var schemas = featSvc.DescribeSchema(fsId, "");
            classes = schemas[0].GetClasses();
            return schemas[0].Name + ":" + classes[0].Name;
        }
    }

    public class FeatureServiceInsertFeatures : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/FeatureServiceInsertFeatures.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);
                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var props = new MgPropertyCollection();
                var nameP = new MgStringProperty("Name", "Test");
                var geom = wktRw.Read("POINT (1 1)");
                var agf = agfRw.Write(geom);
                var geomP = new MgGeometryProperty("Geometry", agf);
                props.Add(nameP);
                props.Add(geomP);

                var fr = featSvc.InsertFeatures(fsId, className, props);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(1, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class FeatureServiceInsertFeaturesWithTransaction : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/FeatureServiceInsertFeaturesWithTransaction.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SQLite", featSvc);
                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var props = new MgPropertyCollection();
                var nameP = new MgStringProperty("Name", "Test");
                var geom = wktRw.Read("POINT (1 1)");
                var agf = agfRw.Write(geom);
                var geomP = new MgGeometryProperty("Geometry", agf);
                props.Add(nameP);
                props.Add(geomP);
                var trans = featSvc.BeginTransaction(fsId);
                var fr = featSvc.InsertFeatures(fsId, className, props, trans);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(1, count);
                trans.Rollback();

                fr = featSvc.SelectFeatures(fsId, className, null);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(0, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class FeatureServiceInsertFeaturesBatched : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/FeatureServiceInsertFeaturesBatched.FeatureSource");
            try 
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);
                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var fr = featSvc.InsertFeatures(fsId, className, batchProps);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class FeatureServiceInsertFeaturesBatchedWithTransaction : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/FeatureServiceInsertFeaturesBatchedWithTransaction.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SQLite", featSvc);
                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var trans = featSvc.BeginTransaction(fsId);
                var fr = featSvc.InsertFeatures(fsId, className, batchProps, trans);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);
                trans.Rollback();

                fr = featSvc.SelectFeatures(fsId, className, null);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(0, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class FeatureServiceUpdateFeatures : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/FeatureServiceUpdateFeatures.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);
                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var fr = featSvc.InsertFeatures(fsId, className, batchProps);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);

                var updateProps = new MgPropertyCollection();
                var newName = new MgStringProperty("Name", "Test3Updated");
                updateProps.Add(newName);

                int updated = featSvc.UpdateMatchingFeatures(fsId, className, updateProps, "Name = 'Test3'");
                Assert.AreEqual(1, updated);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class FeatureServiceUpdateFeaturesWithTransaction : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/FeatureServiceUpdateFeaturesWithTransaction.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SQLite", featSvc);
                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var fr = featSvc.InsertFeatures(fsId, className, batchProps);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);

                var updateProps = new MgPropertyCollection();
                var newName = new MgStringProperty("Name", "Test3Updated");
                updateProps.Add(newName);

                var trans = featSvc.BeginTransaction(fsId);
                int updated = featSvc.UpdateMatchingFeatures(fsId, className, updateProps, "Name = 'Test3'", trans);
                Assert.AreEqual(1, updated);
                trans.Rollback();

                var query = new MgFeatureQueryOptions();
                query.SetFilter("Name = 'Test3Updated'");
                fr = featSvc.SelectFeatures(fsId, className, query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(0, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class FeatureServiceDeleteFeatures : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/FeatureServiceDeleteFeatures.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);
                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var fr = featSvc.InsertFeatures(fsId, className, batchProps);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);

                int deleted = featSvc.DeleteFeatures(fsId, className, "Name = 'Test3'");
                Assert.AreEqual(1, deleted);

                var query = new MgFeatureQueryOptions();
                fr = featSvc.SelectFeatures(fsId, className, query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(4, count);

                query.SetFilter("Name = 'Test3'");
                fr = featSvc.SelectFeatures(fsId, className, query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(0, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class FeatureServiceDeleteFeaturesWithTransaction : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/FeatureServiceDeleteFeaturesWithTransaction.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SQLite", featSvc);
                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var fr = featSvc.InsertFeatures(fsId, className, batchProps);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);

                var trans = featSvc.BeginTransaction(fsId);
                int deleted = featSvc.DeleteFeatures(fsId, className, "Name = 'Test3'", trans);
                Assert.AreEqual(1, deleted);

                var query = new MgFeatureQueryOptions();
                fr = featSvc.SelectFeatures(fsId, className, query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(4, count);
                trans.Rollback();

                query.SetFilter("Name = 'Test3'");
                fr = featSvc.SelectFeatures(fsId, className, query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(1, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class LayerInsertFeatures : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/LayerInsertFeatures.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);

                MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
                MgMapBase map = factory.CreateMap(csFactory.ConvertCoordinateSystemCodeToWkt("LL84"), new MgEnvelope(-180, -90, 180, 90), "TestMap");
                MgLayerCollection layers = map.GetLayers();

                string layerXml = string.Format(Properties.Resources.TestLayer, fsId.ToString(), className, "Geometry");
                byte[] bytes = Encoding.UTF8.GetBytes(layerXml);
                MgByteSource bs = new MgByteSource(bytes, bytes.Length);
                MgByteReader br = bs.GetReader();

                var ldfId = new MgResourceIdentifier("Library://UnitTests/Layers/TestLayer.LayerDefinition");
                resSvc.SetResource(ldfId, br, null);

                MgLayerBase layer = factory.CreateLayer(ldfId);
                layers.Insert(0, layer);

                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var props = new MgPropertyCollection();
                var nameP = new MgStringProperty("Name", "Test");
                var geom = wktRw.Read("POINT (1 1)");
                var agf = agfRw.Write(geom);
                var geomP = new MgGeometryProperty("Geometry", agf);
                props.Add(nameP);
                props.Add(geomP);

                var fr = layer.InsertFeatures(props, null);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(1, count);

                var query = new MgFeatureQueryOptions();
                fr = layer.SelectFeatures(query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(1, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class LayerInsertFeaturesWithTransaction : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/LayerInsertFeaturesWithTransaction.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SQLite", featSvc);

                MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
                MgMapBase map = factory.CreateMap(csFactory.ConvertCoordinateSystemCodeToWkt("LL84"), new MgEnvelope(-180, -90, 180, 90), "TestMap");
                MgLayerCollection layers = map.GetLayers();

                string layerXml = string.Format(Properties.Resources.TestLayer, fsId.ToString(), className, "Geometry");
                byte[] bytes = Encoding.UTF8.GetBytes(layerXml);
                MgByteSource bs = new MgByteSource(bytes, bytes.Length);
                MgByteReader br = bs.GetReader();

                var ldfId = new MgResourceIdentifier("Library://UnitTests/Layers/TestLayer.LayerDefinition");
                resSvc.SetResource(ldfId, br, null);

                MgLayerBase layer = factory.CreateLayer(ldfId);
                layers.Insert(0, layer);

                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var props = new MgPropertyCollection();
                var nameP = new MgStringProperty("Name", "Test");
                var geom = wktRw.Read("POINT (1 1)");
                var agf = agfRw.Write(geom);
                var geomP = new MgGeometryProperty("Geometry", agf);
                props.Add(nameP);
                props.Add(geomP);

                var trans = layer.BeginTransaction();
                var fr = layer.InsertFeatures(props, trans);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(1, count);
                trans.Rollback();

                var query = new MgFeatureQueryOptions();
                fr = layer.SelectFeatures(query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(0, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class LayerInsertFeaturesBatched : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/LayerInsertFeaturesBatched.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);

                MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
                MgMapBase map = factory.CreateMap(csFactory.ConvertCoordinateSystemCodeToWkt("LL84"), new MgEnvelope(-180, -90, 180, 90), "TestMap");
                MgLayerCollection layers = map.GetLayers();

                string layerXml = string.Format(Properties.Resources.TestLayer, fsId.ToString(), className, "Geometry");
                byte[] bytes = Encoding.UTF8.GetBytes(layerXml);
                MgByteSource bs = new MgByteSource(bytes, bytes.Length);
                MgByteReader br = bs.GetReader();

                var ldfId = new MgResourceIdentifier("Library://UnitTests/Layers/TestLayer.LayerDefinition");
                resSvc.SetResource(ldfId, br, null);

                MgLayerBase layer = factory.CreateLayer(ldfId);
                layers.Insert(0, layer);

                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var fr = layer.InsertFeatures(batchProps, null);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);

                var query = new MgFeatureQueryOptions();
                fr = layer.SelectFeatures(query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class LayerInsertFeaturesBatchedWithTransaction : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/LayerInsertFeaturesBatchedWithTransaction.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SQLite", featSvc);

                MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
                MgMapBase map = factory.CreateMap(csFactory.ConvertCoordinateSystemCodeToWkt("LL84"), new MgEnvelope(-180, -90, 180, 90), "TestMap");
                MgLayerCollection layers = map.GetLayers();

                string layerXml = string.Format(Properties.Resources.TestLayer, fsId.ToString(), className, "Geometry");
                byte[] bytes = Encoding.UTF8.GetBytes(layerXml);
                MgByteSource bs = new MgByteSource(bytes, bytes.Length);
                MgByteReader br = bs.GetReader();

                var ldfId = new MgResourceIdentifier("Library://UnitTests/Layers/TestLayer.LayerDefinition");
                resSvc.SetResource(ldfId, br, null);

                MgLayerBase layer = factory.CreateLayer(ldfId);
                layers.Insert(0, layer);

                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var trans = layer.BeginTransaction();
                var fr = layer.InsertFeatures(batchProps, trans);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);
                trans.Rollback();

                var query = new MgFeatureQueryOptions();
                fr = layer.SelectFeatures(query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(0, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class LayerUpdateFeatures : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/LayerUpdateFeatures.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);

                MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
                MgMapBase map = factory.CreateMap(csFactory.ConvertCoordinateSystemCodeToWkt("LL84"), new MgEnvelope(-180, -90, 180, 90), "TestMap");
                MgLayerCollection layers = map.GetLayers();

                string layerXml = string.Format(Properties.Resources.TestLayer, fsId.ToString(), className, "Geometry");
                byte[] bytes = Encoding.UTF8.GetBytes(layerXml);
                MgByteSource bs = new MgByteSource(bytes, bytes.Length);
                MgByteReader br = bs.GetReader();

                var ldfId = new MgResourceIdentifier("Library://UnitTests/Layers/TestLayer.LayerDefinition");
                resSvc.SetResource(ldfId, br, null);

                MgLayerBase layer = factory.CreateLayer(ldfId);
                layers.Insert(0, layer);

                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var fr = layer.InsertFeatures(batchProps, null);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);

                var updateProps = new MgPropertyCollection();
                var newName = new MgStringProperty("Name", "Test3Updated");
                updateProps.Add(newName);

                int updated = layer.UpdateMatchingFeatures(updateProps, "Name = 'Test3'", null);
                Assert.AreEqual(1, updated);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class LayerUpdateFeaturesWithTransaction : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/LayerUpdateFeaturesWithTransaction.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SQLite", featSvc);

                MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
                MgMapBase map = factory.CreateMap(csFactory.ConvertCoordinateSystemCodeToWkt("LL84"), new MgEnvelope(-180, -90, 180, 90), "TestMap");
                MgLayerCollection layers = map.GetLayers();

                string layerXml = string.Format(Properties.Resources.TestLayer, fsId.ToString(), className, "Geometry");
                byte[] bytes = Encoding.UTF8.GetBytes(layerXml);
                MgByteSource bs = new MgByteSource(bytes, bytes.Length);
                MgByteReader br = bs.GetReader();

                var ldfId = new MgResourceIdentifier("Library://UnitTests/Layers/TestLayer.LayerDefinition");
                resSvc.SetResource(ldfId, br, null);

                MgLayerBase layer = factory.CreateLayer(ldfId);
                layers.Insert(0, layer);

                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var fr = layer.InsertFeatures(batchProps, null);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);

                var updateProps = new MgPropertyCollection();
                var newName = new MgStringProperty("Name", "Test3Updated");
                updateProps.Add(newName);

                var trans = layer.BeginTransaction();
                int updated = layer.UpdateMatchingFeatures(updateProps, "Name = 'Test3'", trans);
                Assert.AreEqual(1, updated);

                trans.Rollback();
                var query = new MgFeatureQueryOptions();
                query.SetFilter("Name = 'Test3Updated'");
                fr = layer.SelectFeatures(query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(0, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class LayerDeleteFeatures : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/LayerDeleteFeatures.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);

                MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
                MgMapBase map = factory.CreateMap(csFactory.ConvertCoordinateSystemCodeToWkt("LL84"), new MgEnvelope(-180, -90, 180, 90), "TestMap");
                MgLayerCollection layers = map.GetLayers();

                string layerXml = string.Format(Properties.Resources.TestLayer, fsId.ToString(), className, "Geometry");
                byte[] bytes = Encoding.UTF8.GetBytes(layerXml);
                MgByteSource bs = new MgByteSource(bytes, bytes.Length);
                MgByteReader br = bs.GetReader();

                var ldfId = new MgResourceIdentifier("Library://UnitTests/Layers/TestLayer.LayerDefinition");
                resSvc.SetResource(ldfId, br, null);

                MgLayerBase layer = factory.CreateLayer(ldfId);
                layers.Insert(0, layer);

                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var fr = layer.InsertFeatures(batchProps, null);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);

                int deleted = layer.DeleteFeatures("Name = 'Test3'", null);
                Assert.AreEqual(1, deleted);

                var query = new MgFeatureQueryOptions();
                fr = layer.SelectFeatures(query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(4, count);

                query.SetFilter("Name = 'Test3'");
                fr = layer.SelectFeatures(query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(0, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class LayerDeleteFeaturesWithTransaction : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/LayerDeleteFeaturesWithTransaction.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SQLite", featSvc);

                MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
                MgMapBase map = factory.CreateMap(csFactory.ConvertCoordinateSystemCodeToWkt("LL84"), new MgEnvelope(-180, -90, 180, 90), "TestMap");
                MgLayerCollection layers = map.GetLayers();

                string layerXml = string.Format(Properties.Resources.TestLayer, fsId.ToString(), className, "Geometry");
                byte[] bytes = Encoding.UTF8.GetBytes(layerXml);
                MgByteSource bs = new MgByteSource(bytes, bytes.Length);
                MgByteReader br = bs.GetReader();

                var ldfId = new MgResourceIdentifier("Library://UnitTests/Layers/TestLayer.LayerDefinition");
                resSvc.SetResource(ldfId, br, null);

                MgLayerBase layer = factory.CreateLayer(ldfId);
                layers.Insert(0, layer);

                var agfRw = new MgAgfReaderWriter();
                var wktRw = new MgWktReaderWriter();

                var batchProps = new MgBatchPropertyCollection();
                for (int i = 0; i < 5; i++)
                {
                    var props = new MgPropertyCollection();
                    var nameP = new MgStringProperty("Name", "Test" + (i + 1));
                    var geom = wktRw.Read("POINT (1 1)");
                    var agf = agfRw.Write(geom);
                    var geomP = new MgGeometryProperty("Geometry", agf);
                    props.Add(nameP);
                    props.Add(geomP);
                    batchProps.Add(props);
                }

                var fr = layer.InsertFeatures(batchProps, null);
                int count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);

                var trans = layer.BeginTransaction();
                int deleted = layer.DeleteFeatures("Name = 'Test3'", trans);
                Assert.AreEqual(1, deleted);
                trans.Rollback();

                var query = new MgFeatureQueryOptions();
                fr = layer.SelectFeatures(query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(5, count);

                query.SetFilter("Name = 'Test3'");
                fr = layer.SelectFeatures(query);
                count = 0;
                while (fr.ReadNext())
                {
                    count++;
                }
                fr.Close();
                Assert.AreEqual(1, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class SelectFeaturesWithTransform : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/TransformTest.FeatureSource");
            try
            {
                //Create our test data store
                MgFeatureSchema fs = new MgFeatureSchema("Default", "");
                MgClassDefinition cls = new MgClassDefinition();
                cls.SetName("Test");

                MgDataPropertyDefinition id = new MgDataPropertyDefinition("ID");
                id.SetAutoGeneration(true);
                id.SetDataType(MgPropertyType.Int32);

                MgGeometricPropertyDefinition geom = new MgGeometricPropertyDefinition("Geometry");
                geom.SetGeometryTypes(MgFeatureGeometricType.Point);
                geom.SetSpatialContextAssociation("Default");

                MgPropertyDefinitionCollection props = cls.GetProperties();
                MgPropertyDefinitionCollection idProps = cls.GetIdentityProperties();

                props.Add(id);
                props.Add(geom);
                idProps.Add(id);

                cls.SetDefaultGeometryPropertyName("Geometry");
                MgClassDefinitionCollection classes = fs.GetClasses();
                classes.Add(cls);

                //We'll use a transform guaranteed to work. ArbitraryXY (unitA to unitB)
                //We just check that the transformed values are not the original, that way
                //we know that CS-Map did its job
                MgCoordinateSystemFactory csFact = new MgCoordinateSystemFactory();
                string srcWkt = csFact.ConvertCoordinateSystemCodeToWkt("XY-M");
                string dstWkt = csFact.ConvertCoordinateSystemCodeToWkt("XY-IN");

                MgGeometryFactory geomFact = new MgGeometryFactory();
                MgAgfReaderWriter agfRw = new MgAgfReaderWriter();
                MgFileFeatureSourceParams create = new MgFileFeatureSourceParams("OSGeo.SDF", "Default", srcWkt, fs);
                featSvc.CreateFeatureSource(fsId, create);

                //Populate data store with test points

                MgCoordinate coord1 = geomFact.CreateCoordinateXY(-37.1020, 144.0020);
                MgPoint pt1 = geomFact.CreatePoint(coord1);
                MgByteReader agf1 = agfRw.Write(pt1);

                MgPropertyCollection propVals = new MgPropertyCollection();
                MgGeometryProperty pGeom = new MgGeometryProperty("Geometry", agf1);
                propVals.Add(pGeom);

                MgFeatureReader fr = featSvc.InsertFeatures(fsId, "Default:Test", propVals);
                fr.Close();

                MgCoordinate coord2 = geomFact.CreateCoordinateXY(-37.2020, 144.2020);
                MgPoint pt2 = geomFact.CreatePoint(coord2);
                MgByteReader agf2 = agfRw.Write(pt2);

                pGeom.SetValue(agf2);
                fr = featSvc.InsertFeatures(fsId, "Default:Test", propVals);
                fr.Close();

                //Now select from this data store
                MgFeatureQueryOptions query = new MgFeatureQueryOptions();
                MgReader reader = featSvc.SelectFeatures(fsId, "Default:Test", query, dstWkt);

                Assert.IsTrue(reader.ReadNext());
                Assert.IsTrue(!reader.IsNull("Geometry"));

                MgByteReader txAgf1 = reader.GetGeometry("Geometry");
                MgGeometry txGeom1 = agfRw.Read(txAgf1);
                MgPoint txPt1 = (MgPoint)txGeom1;
                Assert.IsTrue(txPt1 != null);
                MgCoordinate txCoord1 = txPt1.GetCoordinate();

                //TODO: Maybe we should really check that it matches the expected transformed result
                Assert.IsTrue(txCoord1.GetX() != -37.1020);
                Assert.IsTrue(txCoord1.GetY() != 144.0020);

                Assert.IsTrue(reader.ReadNext());
                Assert.IsTrue(!reader.IsNull("Geometry"));

                MgByteReader txAgf2 = reader.GetGeometry("Geometry");
                MgGeometry txGeom2 = agfRw.Read(txAgf2);
                MgPoint txPt2 = (MgPoint)txGeom2;
                Assert.IsTrue(txPt2 != null);
                MgCoordinate txCoord2 = txPt2.GetCoordinate();

                //TODO: Maybe we should really check that it matches the expected transformed result
                Assert.IsTrue(txCoord2.GetX() != -37.2020);
                Assert.IsTrue(txCoord2.GetY() != 144.2020);

                reader.Close();
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class LayerSelectFeaturesWithTransform : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/TransformTest.FeatureSource");
            try
            {
                //Create our test data store
                MgFeatureSchema fs = new MgFeatureSchema("Default", "");
                MgClassDefinition cls = new MgClassDefinition();
                cls.SetName("Test");

                MgDataPropertyDefinition id = new MgDataPropertyDefinition("ID");
                id.SetAutoGeneration(true);
                id.SetDataType(MgPropertyType.Int32);

                MgGeometricPropertyDefinition geom = new MgGeometricPropertyDefinition("Geometry");
                geom.SetGeometryTypes(MgFeatureGeometricType.Point);
                geom.SetSpatialContextAssociation("Default");

                MgPropertyDefinitionCollection props = cls.GetProperties();
                MgPropertyDefinitionCollection idProps = cls.GetIdentityProperties();

                props.Add(id);
                props.Add(geom);
                idProps.Add(id);

                cls.SetDefaultGeometryPropertyName("Geometry");
                MgClassDefinitionCollection classes = fs.GetClasses();
                classes.Add(cls);

                //We'll use a transform guaranteed to work. ArbitraryXY (unitA to unitB)
                //We just check that the transformed values are not the original, that way
                //we know that CS-Map did its job
                MgCoordinateSystemFactory csFact = new MgCoordinateSystemFactory();
                string srcWkt = csFact.ConvertCoordinateSystemCodeToWkt("XY-M");
                string dstWkt = csFact.ConvertCoordinateSystemCodeToWkt("XY-IN");

                MgGeometryFactory geomFact = new MgGeometryFactory();
                MgAgfReaderWriter agfRw = new MgAgfReaderWriter();
                MgFileFeatureSourceParams create = new MgFileFeatureSourceParams("OSGeo.SDF", "Default", srcWkt, fs);

                featSvc.CreateFeatureSource(fsId, create);

                MgMapBase map = factory.CreateMap(srcWkt, new MgEnvelope(-180, -90, 180, 90), "TestMap");
                MgLayerCollection layers = map.GetLayers();

                string layerXml = string.Format(Properties.Resources.TestLayer, fsId.ToString(), "Default:Test", "Geometry");
                byte[] bytes = Encoding.UTF8.GetBytes(layerXml);
                MgByteSource bs = new MgByteSource(bytes, bytes.Length);
                MgByteReader br = bs.GetReader();

                var ldfId = new MgResourceIdentifier("Library://UnitTests/Layers/TestLayer.LayerDefinition");
                resSvc.SetResource(ldfId, br, null);

                MgLayerBase layer = factory.CreateLayer(ldfId);
                layers.Insert(0, layer);

                //Populate data store with test points

                MgCoordinate coord1 = geomFact.CreateCoordinateXY(-37.1020, 144.0020);
                MgPoint pt1 = geomFact.CreatePoint(coord1);
                MgByteReader agf1 = agfRw.Write(pt1);

                MgPropertyCollection propVals = new MgPropertyCollection();
                MgGeometryProperty pGeom = new MgGeometryProperty("Geometry", agf1);
                propVals.Add(pGeom);

                MgFeatureReader fr = layer.InsertFeatures(propVals, null);
                fr.Close();

                MgCoordinate coord2 = geomFact.CreateCoordinateXY(-37.2020, 144.2020);
                MgPoint pt2 = geomFact.CreatePoint(coord2);
                MgByteReader agf2 = agfRw.Write(pt2);

                pGeom.SetValue(agf2);
                fr = layer.InsertFeatures(propVals, null);
                fr.Close();

                //Now select from this data store
                MgFeatureQueryOptions query = new MgFeatureQueryOptions();
                MgReader reader = layer.SelectFeatures(query, dstWkt);

                Assert.IsTrue(reader.ReadNext());
                Assert.IsTrue(!reader.IsNull("Geometry"));

                MgByteReader txAgf1 = reader.GetGeometry("Geometry");
                MgGeometry txGeom1 = agfRw.Read(txAgf1);
                MgPoint txPt1 = (MgPoint)txGeom1;
                Assert.IsTrue(txPt1 != null);
                MgCoordinate txCoord1 = txPt1.GetCoordinate();

                //TODO: Maybe we should really check that it matches the expected transformed result
                Assert.IsTrue(txCoord1.GetX() != -37.1020);
                Assert.IsTrue(txCoord1.GetY() != 144.0020);

                Assert.IsTrue(reader.ReadNext());
                Assert.IsTrue(!reader.IsNull("Geometry"));

                MgByteReader txAgf2 = reader.GetGeometry("Geometry");
                MgGeometry txGeom2 = agfRw.Read(txAgf2);
                MgPoint txPt2 = (MgPoint)txGeom2;
                Assert.IsTrue(txPt2 != null);
                MgCoordinate txCoord2 = txPt2.GetCoordinate();

                //TODO: Maybe we should really check that it matches the expected transformed result
                Assert.IsTrue(txCoord2.GetX() != -37.2020);
                Assert.IsTrue(txCoord2.GetY() != 144.2020);

                reader.Close();
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }

    public class LayerGetSpatialContexts : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var featSvc = (MgFeatureService)factory.CreateService(MgServiceType.FeatureService);
            var fsId = new MgResourceIdentifier("Library://UnitTests/Data/GetSpatialContexts.FeatureSource");
            try
            {
                string className = FeatureServiceTestUtil.CreateTestDataStore(fsId, "OSGeo.SDF", featSvc);

                MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
                MgMapBase map = factory.CreateMap(csFactory.ConvertCoordinateSystemCodeToWkt("LL84"), new MgEnvelope(-180, -90, 180, 90), "TestMap");
                MgLayerCollection layers = map.GetLayers();

                string layerXml = string.Format(Properties.Resources.TestLayer, fsId.ToString(), className, "Geometry");
                byte[] bytes = Encoding.UTF8.GetBytes(layerXml);
                MgByteSource bs = new MgByteSource(bytes, bytes.Length);
                MgByteReader br = bs.GetReader();

                var ldfId = new MgResourceIdentifier("Library://UnitTests/Layers/TestLayer.LayerDefinition");
                resSvc.SetResource(ldfId, br, null);

                MgLayerBase layer = factory.CreateLayer(ldfId);
                layers.Insert(0, layer);

                MgSpatialContextReader scReader = layer.GetSpatialContexts(false);
                int count = 0;
                try
                {
                    while (scReader.ReadNext())
                    {
                        Assert.AreEqual("Default", scReader.GetName());
                        Assert.AreEqual(csFactory.ConvertCoordinateSystemCodeToWkt("LL84"), scReader.GetCoordinateSystemWkt());
                        count++;
                    }
                }
                finally
                {
                    scReader.Close();
                }
                Assert.AreEqual(1, count);
            }
            finally
            {
                resSvc.DeleteResource(fsId);
            }
        }
    }
}
