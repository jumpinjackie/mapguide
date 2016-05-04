using OSGeo.MapGuide.Test.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Web.ExternalTests
{
    public abstract class ExternalTestBase : IExternalTest
    {
        protected static void LoadResource(MgResourceService resSvc, string resIdStr, string path)
        {
            MgResourceIdentifier resId = new MgResourceIdentifier(resIdStr);
            MgByteSource bs = new MgByteSource(path);
            MgByteReader br = bs.GetReader();
            resSvc.SetResource(resId, br, null);
        }

        protected static void LoadResourceData(MgResourceService resSvc, string resIdStr, string dataName, string dataType, string path)
        {
            MgResourceIdentifier resId = new MgResourceIdentifier(resIdStr);
            MgByteSource bs = new MgByteSource(path);
            MgByteReader br = bs.GetReader();
            resSvc.SetResourceData(resId, dataName, dataType, br);
        }

        public abstract void Execute(IPlatformFactory factory, ITestLogger logger);
    }

    public class LinkedTileSetMapTest : ExternalTestBase
    {
        public override void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            try
            {
                MgResourceService resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);

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

                byte[] tsd = Properties.Resources.UT_XYZ;
                MgByteSource sourceTSD = new MgByteSource(tsd, tsd.Length);
                MgByteReader contentTSD = sourceTSD.GetReader();
                MgResourceIdentifier resTSD = new MgResourceIdentifier("Library://UnitTests/TileSets/Test.TileSetDefinition");

                resSvc.SetResource(resTSD, contentTSD, null);

                string mdf = Encoding.UTF8.GetString(Properties.Resources.UT_BaseMap);
                mdf = string.Format(mdf, resTSD.ToString());
                byte[] mdfBytes = Encoding.UTF8.GetBytes(mdf);
                MgByteSource sourceMDF = new MgByteSource(mdfBytes, mdfBytes.Length);
                MgByteReader contentMDF = sourceMDF.GetReader();
                MgResourceIdentifier resMDF = new MgResourceIdentifier("Library://UnitTests/Maps/LinkedTileSet.MapDefinition");

                resSvc.SetResource(resMDF, contentMDF, null);

                //This should throw because making a MgMap from a Map Defintion that links to a XYZ tile set
                //is not supported
                MgMapBase map = factory.CreateMap(resMDF);

                Assert.Fail("Expected MgUnsupportedTileProviderException to be thrown");
            }
            catch (MgUnsupportedTileProviderException)
            {

            }
        }
    }

    public class TestTicket2538 : ExternalTestBase
    {
        public override void Execute(IPlatformFactory factory, ITestLogger logger)
        {
            //For a map definition that links to a default provider tile set and both have the same coordinate
            //system, it should be using the map definition's extents and we should not have null extents or view
            //center

            MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
            string csWkt = csFactory.ConvertCoordinateSystemCodeToWkt("LL84");

            MgResourceService resSvc = (MgResourceService)factory.CreateService(MgServiceType.ResourceService);

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

            string tsd = Encoding.UTF8.GetString(Properties.Resources.UT_BaseMapTileSet);
            tsd = string.Format(tsd, csWkt, -87.5, 43.5, -86.5, 44.5);
            byte[] tsdBytes = Encoding.UTF8.GetBytes(tsd);
            MgByteSource sourceTSD = new MgByteSource(tsdBytes, tsdBytes.Length);
            MgByteReader contentTSD = sourceTSD.GetReader();
            MgResourceIdentifier resTSD = new MgResourceIdentifier("Library://UnitTests/TileSets/Test.TileSetDefinition");

            resSvc.SetResource(resTSD, contentTSD, null);

            string mdf = Encoding.UTF8.GetString(Properties.Resources.UT_LinkedTileSet);
            mdf = string.Format(mdf, csWkt, -87.0, 43.0, -86.0, 44.0, resTSD.ToString());
            byte[] mdfBytes = Encoding.UTF8.GetBytes(mdf);
            MgByteSource sourceMDF = new MgByteSource(mdfBytes, mdfBytes.Length);
            MgByteReader contentMDF = sourceMDF.GetReader();
            MgResourceIdentifier resMDF = new MgResourceIdentifier("Library://UnitTests/Maps/LinkedTileSet.MapDefinition");

            resSvc.SetResource(resMDF, contentMDF, null);

            MgMapBase map = factory.CreateMap(resMDF);
            MgEnvelope extent = map.GetMapExtent();
            Assert.IsNotNull(extent);
            Assert.IsNotNull(map.MapExtent);
            MgPoint center = map.GetViewCenter();
            Assert.IsNotNull(center);
            Assert.IsNotNull(map.ViewCenter);

            MgCoordinate ll = extent.GetLowerLeftCoordinate();
            MgCoordinate ur = extent.GetUpperRightCoordinate();

            Assert.IsNotNull(ll);
            Assert.IsNotNull(ur);

            Assert.AreEqual(-87.0, ll.X);
            Assert.AreEqual(43.0, ll.Y);
            Assert.AreEqual(-86.0, ur.X);
            Assert.AreEqual(44.0, ur.Y);
        }
    }
}
