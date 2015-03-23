package org.osgeo.mapguide.test.platform.operations.maplayer;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.*;

public class AddLayerGroup extends MapLayerOperationExecutor
{
    public AddLayerGroup(MgResourceService resSvc, String vm, IMapCreator creator)
    {
        super("AddLayerGroup", resSvc, vm, creator);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            this.CreateMapFromResource(paramSetId);

            SQLiteStatement grpStmt = _conn.prepare(String.format("Select ParamValue from Params WHERE ParamSet=%d AND ParamName=\"GROUPNAME\"", paramSetId));
            String groupName = "";
            if (grpStmt.step()) {
                groupName = CommonUtility.GetStringFromStatement(grpStmt, "ParamValue");
            }

            SQLiteStatement parentStmt = _conn.prepare(String.format("Select ParamValue from Params WHERE ParamSet=%d AND ParamName=\"PARENTGROUPNAME\"", paramSetId));
            String parentGroupName = "";
            if (parentStmt.step()) {
                parentGroupName = CommonUtility.GetStringFromStatement(parentStmt, "ParamValue");
            }

            MgLayerGroupCollection groups = _map.getLayerGroups();
            MgLayerGroup group = new MgLayerGroup(groupName);
            if (!CommonUtility.IsNullOrEmpty(parentGroupName))
            {
                MgLayerGroup parentGroup = groups.getItem(parentGroupName);
                group.setGroup(parentGroup);
            }
            groups.add(group);

            return TestResult.Integer(groups.getCount());
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