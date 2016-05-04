package org.osgeo.mapguide.test.platform.operations.maplayer;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.*;

public class GetLayers extends MapLayerOperationExecutor
{
    public GetLayers(MgResourceService resSvc, String vm, IMapCreator creator)
    {
        super("GetLayers", resSvc, vm, creator);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            this.CreateMapFromResource(paramSetId);
            MgLayerCollection layers = _map.getLayers();
            String[] names = new String[layers.getCount()];
            for (int i = 0; i < layers.getCount(); i++)
            {
                MgLayerBase layer = layers.getItem(i);
                names[i] = layer.getName();
            }

            return TestResult.String(CommonUtility.JoinStringArray("/", names));
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}