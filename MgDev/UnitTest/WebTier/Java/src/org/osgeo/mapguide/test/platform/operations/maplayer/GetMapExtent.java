package org.osgeo.mapguide.test.platform.operations.maplayer;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.*;

public class GetMapExtent extends MapLayerOperationExecutor
{
    public GetMapExtent(MgResourceService resSvc, String vm, IMapCreator creator)
    {
        super("GetMapExtent", resSvc, vm, creator);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            this.CreateMapFromResource(paramSetId);
            MgEnvelope extent = _map.getMapExtent();
            return TestResult.Envelope(extent);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}