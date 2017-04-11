using OSGeo.MapGuide.Test.Common;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Web.ExternalTests
{
    public class RenderTileTest : IExternalTest
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

            var map = (MgMap)factory.CreateMap(new MgResourceIdentifier("Library://UnitTests/Maps/BaseMap.MapDefinition"));
            var img = renderSvc.RenderTile(map, "BaseLayers", 0, 0);
            Utils.CheckValidImage(img);
            img = renderSvc.RenderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Png);
            Utils.CheckValidImage(img, 256, 256, MgImageFormats.Png);
            img = renderSvc.RenderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Gif);
            Utils.CheckValidImage(img, 256, 256, MgImageFormats.Gif);
            img = renderSvc.RenderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Jpeg);
            Utils.CheckValidImage(img, 256, 256, MgImageFormats.Jpeg);
            img = renderSvc.RenderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Png8);
            Utils.CheckValidImage(img, 256, 256, MgImageFormats.Png8);
            img = renderSvc.RenderTileXYZ(map, "BaseLayers", 2099, 2985, 13);
            Utils.CheckValidImage(img, 256, 256);
            img = renderSvc.RenderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Png);
            Utils.CheckValidImage(img, 256, 256, MgImageFormats.Png);
            img = renderSvc.RenderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Png8);
            Utils.CheckValidImage(img, 256, 256, MgImageFormats.Png8);
            img = renderSvc.RenderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Gif);
            Utils.CheckValidImage(img, 256, 256, MgImageFormats.Gif);
            img = renderSvc.RenderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Jpeg);
            Utils.CheckValidImage(img, 256, 256, MgImageFormats.Jpeg);
        }
    }
}
