package org.osgeo.mapguide.test.platform.operations.maplayer;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.*;

public class RemoveGroup extends MapLayerOperationExecutor
{
    public RemoveGroup(MgResourceService resSvc, String vm, IMapCreator creator)
    {
        super("RemoveGroup", resSvc, vm, creator);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            this.CreateMapFromResource(paramSetId);

            SQLiteStatement stmt = _conn.prepare(String.format("Select ParamValue from Params WHERE ParamSet=%d AND ParamName=\"GROUPNAME\"", paramSetId));
            if (stmt.step()) {
                String groupName = CommonUtility.GetStringFromStatement(stmt, "ParamValue");

                MgLayerGroupCollection groups = _map.getLayerGroups();
                MgLayerGroup group = groups.getItem(groupName);

                groups.remove(group);

                return TestResult.Ok();
            }
            throw new RuntimeException("No group param found for param set: " + paramSetId);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
        catch (Exception ex)
        {
            return TestResult.FromException(ex);
        }
    }
}