package org.osgeo.mapguide.test.platform.operations.maplayer;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.*;

public class GetGroups extends MapLayerOperationExecutor
{
    public GetGroups(MgResourceService resSvc, String vm, IMapCreator creator)
    {
        super("GetGroups", resSvc, vm, creator);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            this.CreateMapFromResource(paramSetId);
            MgLayerGroupCollection groups = _map.getLayerGroups();
            String[] names = new String[groups.getCount()];
            for (int i = 0; i < groups.getCount(); i++)
            {
                MgLayerGroup group = groups.getItem(i);
                names[i] = group.getName();
            }

            return TestResult.String(CommonUtility.JoinStringArray("/", names));
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}