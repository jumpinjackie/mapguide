using OSGeo.MapGuide.Test.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Web.ExternalTests
{
    public class QueryFeatureStatelessTest : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var renderSvc = (MgRenderingService)factory.CreateService(MgServiceType.RenderingService);
            var root = "../../TestData/TileService/";

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/RoadCenterLines.FeatureSource", root + "UT_RoadCenterLines.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/RoadCenterLines.FeatureSource", "UT_RoadCenterLines.sdf", MgResourceDataType.File, root + "UT_RoadCenterLines.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/RoadCenterLines.LayerDefinition", root + "UT_RoadCenterLines.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/VotingDistricts.FeatureSource", root + "UT_VotingDistricts.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/VotingDistricts.FeatureSource", "UT_VotingDistricts.sdf", MgResourceDataType.File, root + "UT_VotingDistricts.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/VotingDistricts.LayerDefinition", root + "UT_VotingDistricts.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/Parcels.FeatureSource", root + "UT_Parcels.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/Parcels.FeatureSource", "UT_Parcels.sdf", MgResourceDataType.File, root + "UT_Parcels.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/Parcels.LayerDefinition", root + "UT_Parcels.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Maps/BaseMap.MapDefinition", root + "UT_BaseMap.mdf");

            var mdfId = new MgResourceIdentifier("Library://UnitTests/Maps/BaseMap.MapDefinition");

            //This is a sample request from the AJAX viewer. It selected 7 features. It should do the same here.
            var dpi = 96;
            var width = 773;
            var height = 696;
            var scale = 4345.697945713148;
            var x = -87.72117757411266;
            var y = 43.7527161153258;
            var wkt = "POLYGON((-87.72250482778884 43.75497812742761, -87.72134799968197 43.75497812742761, -87.72134799968197 43.75358373640595, -87.72250482778884 43.75358373640595, -87.72250482778884 43.75497812742761))";
            var map = (MgMap)factory.CreateMap(mdfId, mdfId.Name, width, height, x, y, scale, dpi);

            var wktRw = new MgWktReaderWriter();
            var geom = wktRw.Read(wkt);
            var layerNames = new MgStringCollection();
            layerNames.Add("Parcels");

            var result = renderSvc.QueryFeatures(map, layerNames, geom, MgFeatureSpatialOperations.Intersects, -1);
            var sel = result.GetSelection();
            sel.SetMap(map);

            var total = 0;
            var selLayers = sel.GetLayers();

            foreach (var sl in selLayers)
            {
                total += sel.GetSelectedFeaturesCount(sl, sl.FeatureClassName);
            }

            Assert.AreEqual(7, total);
        }
    }

    public class QueryFeaturePropertiesStatelessTest : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var renderSvc = (MgRenderingService)factory.CreateService(MgServiceType.RenderingService);
            var root = "../../TestData/TileService/";

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/RoadCenterLines.FeatureSource", root + "UT_RoadCenterLines.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/RoadCenterLines.FeatureSource", "UT_RoadCenterLines.sdf", MgResourceDataType.File, root + "UT_RoadCenterLines.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/RoadCenterLines.LayerDefinition", root + "UT_RoadCenterLines.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/VotingDistricts.FeatureSource", root + "UT_VotingDistricts.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/VotingDistricts.FeatureSource", "UT_VotingDistricts.sdf", MgResourceDataType.File, root + "UT_VotingDistricts.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/VotingDistricts.LayerDefinition", root + "UT_VotingDistricts.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/Parcels.FeatureSource", root + "UT_Parcels.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/Parcels.FeatureSource", "UT_Parcels.sdf", MgResourceDataType.File, root + "UT_Parcels.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/Parcels.LayerDefinition", root + "UT_Parcels.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Maps/BaseMap.MapDefinition", root + "UT_BaseMap.mdf");

            var mdfId = new MgResourceIdentifier("Library://UnitTests/Maps/BaseMap.MapDefinition");

            //This is a sample request from the AJAX viewer. It selected 7 features. It should do the same here.
            var dpi = 96;
            var width = 773;
            var height = 696;
            var scale = 4345.697945713148;
            var x = -87.72117757411266;
            var y = 43.7527161153258;
            var wkt = "POLYGON((-87.72250482778884 43.75497812742761, -87.72134799968197 43.75497812742761, -87.72134799968197 43.75358373640595, -87.72250482778884 43.75358373640595, -87.72250482778884 43.75497812742761))";
            var map = (MgMap)factory.CreateMap(mdfId, mdfId.Name, width, height, x, y, scale, dpi);
            
            var wktRw = new MgWktReaderWriter();
            var geom = wktRw.Read(wkt);
            var layerNames = new MgStringCollection();
            layerNames.Add("Parcels");

            var result = renderSvc.QueryFeatureProperties(map, layerNames, geom, MgFeatureSpatialOperations.Intersects, -1);
            Assert.AreEqual(7, result.Count);
        }
    }

    public class RenderDynamicOverlayStatelessTest : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var renderSvc = (MgRenderingService)factory.CreateService(MgServiceType.RenderingService);
            var root = "../../TestData/TileService/";

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/RoadCenterLines.FeatureSource", root + "UT_RoadCenterLines.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/RoadCenterLines.FeatureSource", "UT_RoadCenterLines.sdf", MgResourceDataType.File, root + "UT_RoadCenterLines.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/RoadCenterLines.LayerDefinition", root + "UT_RoadCenterLines.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/VotingDistricts.FeatureSource", root + "UT_VotingDistricts.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/VotingDistricts.FeatureSource", "UT_VotingDistricts.sdf", MgResourceDataType.File, root + "UT_VotingDistricts.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/VotingDistricts.LayerDefinition", root + "UT_VotingDistricts.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/Parcels.FeatureSource", root + "UT_Parcels.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/Parcels.FeatureSource", "UT_Parcels.sdf", MgResourceDataType.File, root + "UT_Parcels.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/Parcels.LayerDefinition", root + "UT_Parcels.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Maps/BaseMap.MapDefinition", root + "UT_BaseMap.mdf");

            var mdfId = new MgResourceIdentifier("Library://UnitTests/Maps/BaseMap.MapDefinition");

            //This is a sample request from the AJAX viewer. It selected 7 features. It should do the same here.
            var dpi = 96;
            var width = 773;
            var height = 696;
            var scale = 4345.697945713148;
            var x = -87.72117757411266;
            var y = 43.7527161153258;
            var wkt = "POLYGON((-87.72250482778884 43.75497812742761, -87.72134799968197 43.75497812742761, -87.72134799968197 43.75358373640595, -87.72250482778884 43.75358373640595, -87.72250482778884 43.75497812742761))";
            var map = (MgMap)factory.CreateMap(mdfId, mdfId.Name, width, height, x, y, scale, dpi);

            var selColor = new MgColor(0, 0, 255);
            var renderOpts = new MgRenderingOptions(MgImageFormats.Png, 1 | 2 | 4 | 8, selColor);
            var result = renderSvc.RenderDynamicOverlay(map, null, renderOpts);

            Utils.CheckValidImage(result, width, height, MgImageFormats.Png);
        }
    }

    public class RenderMapLegendStatelessTest : IExternalTest
    {
        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var renderSvc = (MgRenderingService)factory.CreateService(MgServiceType.RenderingService);
            var root = "../../TestData/TileService/";

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/RoadCenterLines.FeatureSource", root + "UT_RoadCenterLines.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/RoadCenterLines.FeatureSource", "UT_RoadCenterLines.sdf", MgResourceDataType.File, root + "UT_RoadCenterLines.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/RoadCenterLines.LayerDefinition", root + "UT_RoadCenterLines.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/VotingDistricts.FeatureSource", root + "UT_VotingDistricts.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/VotingDistricts.FeatureSource", "UT_VotingDistricts.sdf", MgResourceDataType.File, root + "UT_VotingDistricts.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/VotingDistricts.LayerDefinition", root + "UT_VotingDistricts.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Data/Parcels.FeatureSource", root + "UT_Parcels.fs");
            Utils.LoadResourceData(resSvc, "Library://UnitTests/Data/Parcels.FeatureSource", "UT_Parcels.sdf", MgResourceDataType.File, root + "UT_Parcels.sdf");
            Utils.LoadResource(resSvc, "Library://UnitTests/Layers/Parcels.LayerDefinition", root + "UT_Parcels.ldf");

            Utils.LoadResource(resSvc, "Library://UnitTests/Maps/BaseMap.MapDefinition", root + "UT_BaseMap.mdf");

            var mdfId = new MgResourceIdentifier("Library://UnitTests/Maps/BaseMap.MapDefinition");

            var dpi = 96;
            var width = 773;
            var height = 696;
            var scale = 4345.697945713148;
            var x = -87.72117757411266;
            var y = 43.7527161153258;
            var wkt = "POLYGON((-87.72250482778884 43.75497812742761, -87.72134799968197 43.75497812742761, -87.72134799968197 43.75358373640595, -87.72250482778884 43.75358373640595, -87.72250482778884 43.75497812742761))";
            var map = (MgMap)factory.CreateMap(mdfId, mdfId.Name, width, height, x, y, scale, dpi);

            var bgColor = new MgColor(255, 255, 255);
            var result = renderSvc.RenderMapLegend(map, 200, 600, bgColor, MgImageFormats.Png);
            Utils.CheckValidImage(result, 200, 600, MgImageFormats.Png);
        }
    }
}
