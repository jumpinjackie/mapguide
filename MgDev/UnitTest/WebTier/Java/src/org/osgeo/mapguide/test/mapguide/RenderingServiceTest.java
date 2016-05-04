package org.osgeo.mapguide.test.mapguide;

import java.io.*;
import java.nio.file.*;
import java.util.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class RenderingServiceTest implements IExternalTest
{
    private static void LoadResource(MgResourceService resSvc, String resIdStr, String path)
    {
        MgResourceIdentifier resId = new MgResourceIdentifier(resIdStr);
        MgByteSource bs = new MgByteSource(path);
        MgByteReader br = bs.getReader();
        resSvc.setResource(resId, br, null);
    }

    private static void LoadResourceData(MgResourceService resSvc, String resIdStr, String dataName, String dataType, String path)
    {
        MgResourceIdentifier resId = new MgResourceIdentifier(resIdStr);
        MgByteSource bs = new MgByteSource(path);
        MgByteReader br = bs.getReader();
        resSvc.setResourceData(resId, dataName, dataType, br);
    }

    public void Execute(IPlatformFactory factory, ITestLogger logger)
    {
        MgResourceService resSvc = (MgResourceService)factory.createService(MgServiceType.ResourceService);
        MgRenderingService renderSvc = (MgRenderingService)factory.createService(MgServiceType.RenderingService);
        String root = "../../TestData/TileService/";

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

        MgMap map = (MgMap)factory.createMap();
        map.create(new MgResourceIdentifier("Library://UnitTests/Maps/BaseMap.MapDefinition"), "BaseMap");
        MgByteReader img = renderSvc.renderTile(map, "BaseLayers", 0, 0);
        CheckValidImage(img);
        img = renderSvc.renderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Png);
        CheckValidImage(img); //, 256, 256, MgImageFormats.Png);
        img = renderSvc.renderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Gif);
        CheckValidImage(img); //, 256, 256, MgImageFormats.Gif);
        img = renderSvc.renderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Jpeg);
        CheckValidImage(img); //, 256, 256, MgImageFormats.Jpeg);
        img = renderSvc.renderTile(map, "BaseLayers", 0, 0, 256, 256, 96, MgImageFormats.Png8);
        CheckValidImage(img); //, 256, 256, MgImageFormats.Png8);
        img = renderSvc.renderTileXYZ(map, "BaseLayers", 2099, 2985, 13);
        CheckValidImage(img); //, 256, 256);
        img = renderSvc.renderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Png);
        CheckValidImage(img); //, 256, 256, MgImageFormats.Png);
        img = renderSvc.renderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Png8);
        CheckValidImage(img); //, 256, 256, MgImageFormats.Png8);
        img = renderSvc.renderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Gif);
        CheckValidImage(img); //, 256, 256, MgImageFormats.Gif);
        img = renderSvc.renderTileXYZ(map, "BaseLayers", 2099, 2985, 13, 96, MgImageFormats.Jpeg);
        CheckValidImage(img); //, 256, 256, MgImageFormats.Jpeg);
    }

    private void CheckValidImage(MgByteReader img)
    {
        try
        {
            MgByteSink sink = new MgByteSink(img);
            sink.toFile("tile.img");
            File f = new File("tile.img");
            Assert.IsTrue(f.exists());
            Assert.IsTrue(f.length() > 0);
        }
        catch (Exception ex)
        {
            throw new AssertException("Invalid image found: " + ex.getMessage());
        }
    }
}