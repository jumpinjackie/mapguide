package org.osgeo.mapguide.test.platform.operations.maplayer;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.*;

public class GetMapName extends MapLayerOperationExecutor
{
    public GetMapName(MgResourceService resSvc, String vm, IMapCreator creator)
    {
        super("GetMapName", resSvc, vm, creator);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            this.CreateMapFromResource(paramSetId);
            return TestResult.String(_map.getName());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}