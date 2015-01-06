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
        private static void LoadResource(MgResourceService resSvc, string resIdStr, string path)
        {
            MgResourceIdentifier resId = new MgResourceIdentifier(resIdStr);
            MgByteSource bs = new MgByteSource(path);
            MgByteReader br = bs.GetReader();
            resSvc.SetResource(resId, br, null);
        }

        private static void LoadResourceData(MgResourceService resSvc, string resIdStr, string dataName, string dataType, string path)
        {
            MgResourceIdentifier resId = new MgResourceIdentifier(resIdStr);
            MgByteSource bs = new MgByteSource(path);
            MgByteReader br = bs.GetReader();
            resSvc.SetResourceData(resId, dataName, dataType, br);
        }

        public void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            var resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);
            var renderSvc = (MgRenderingService)factory.CreateService(MgServiceType.RenderingService);
            var root = "../../TestData/TileService/";

            LoadResource(resSvc, "Library://UnitTests/Data/RoadCenterLines.FeatureSource", root + "UT_RoadCenterLines.fs");
            LoadResourceData(resSvc, "Library://UnitTests/Data/RoadCenterLines.FeatureSource", "UT_RoadCenterLines.sdf", MgResourceDataType.File, root + "UT_RoadCenterLines.sdf");
            LoadResource(resSvc, "Library://UnitTests/Layers/RoadCenterLines.LayerDefinition", root + "UT_RoadCenterLines.ldf");

            LoadResource(resSvc, "Library://UnitTests/Data/VotingDistricts.FeatureSource", root + "UT_VotingDistricts.fs");
            LoadResourceData(resSvc, "Library://UnitTests/Data/VotingDistricts.FeatureSource", "UT_VotingDistricts.sdf", MgResourceDataType.File, root + "UT_VotingDistricts.sdf");
            LoadResource(resSvc, "Library://UnitTests/Layers/VotingDistricts.LayerDefinition", root + "UT_VotingDistricts.ldf");

            LoadResource(resSvc, "Library://UnitTests/Data/Parcels.FeatureSource", root + "UT_Parcels.fs");
            LoadResourceData(resSvc, "Library://UnitTests/Data/Parcels.FeatureSource", "UT_Parcels.sdf", MgResourceDataType.File, root + "UT_Parcels.sdf");
            LoadResource(resSvc, "Library://UnitTests/Layers/Parcels.LayerDefinition", root + "UT_Parcels.ldf");

            LoadResource(resSvc, "Library://UnitTests/Maps/BaseMap.MapDefinition", root + "UT_BaseMap.mdf");

            var map = (MgMap)factory.CreateMap(new MgResourceIdentifier("Library://UnitTests/Maps/BaseMap.MapDefinition"));
            var img = renderSvc.RenderTile(map, "BaseLayers", 0, 0);
            CheckValidImage(img);
            img = renderSvc.RenderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Png);
            CheckValidImage(img, 256, 256, MgImageFormats.Png);
            img = renderSvc.RenderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Gif);
            CheckValidImage(img, 256, 256, MgImageFormats.Gif);
            img = renderSvc.RenderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Jpeg);
            CheckValidImage(img, 256, 256, MgImageFormats.Jpeg);
            img = renderSvc.RenderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Png8);
            CheckValidImage(img, 256, 256, MgImageFormats.Png8);
            img = renderSvc.RenderTileXYZ(map, "BaseLayers", 2099, 2985, 13);
            CheckValidImage(img, 256, 256);
            img = renderSvc.RenderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Png);
            CheckValidImage(img, 256, 256, MgImageFormats.Png);
            img = renderSvc.RenderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Png8);
            CheckValidImage(img, 256, 256, MgImageFormats.Png8);
            img = renderSvc.RenderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Gif);
            CheckValidImage(img, 256, 256, MgImageFormats.Gif);
            img = renderSvc.RenderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Jpeg);
            CheckValidImage(img, 256, 256, MgImageFormats.Jpeg);
        }

        private void CheckValidImage(MgByteReader img, int? imageWidth = null, int? imageHeight = null, string format = null)
        {
            try
            {
                MgByteSink sink = new MgByteSink(img);
                sink.ToFile("tile.img");
                using (Image image = Image.FromFile("tile.img"))
                {
                    Assert.IsNotNull(image);
                    if (imageWidth != null)
                        Assert.Equals(image.Width, imageWidth.Value);
                    if (imageHeight != null)
                        Assert.Equals(image.Height, imageHeight.Value);

                    if (format != null)
                    {
                        switch(format)
                        {
                            case MgImageFormats.Gif:
                                Assert.Equals(image.RawFormat, ImageFormat.Gif);
                                break;
                            case MgImageFormats.Jpeg:
                                Assert.Equals(image.RawFormat, ImageFormat.Jpeg);
                                break;
                            case MgImageFormats.Png:
                            case MgImageFormats.Png8:
                                Assert.Equals(image.RawFormat, ImageFormat.Png);
                                break;
                            case MgImageFormats.Tiff:
                                Assert.Equals(image.RawFormat, ImageFormat.Tiff);
                                break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                throw new AssertException("Invalid image found: " + ex.Message);
            }
            finally
            {
                if (File.Exists("tile.img"))
                {
                    try
                    {
                        File.Delete("tile.img");
                    }
                    catch { }
                }
            }
        }
    }

}
