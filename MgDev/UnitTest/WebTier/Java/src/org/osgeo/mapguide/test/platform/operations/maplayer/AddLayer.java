package org.osgeo.mapguide.test.platform.operations.maplayer;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.*;

public class AddLayer extends MapLayerOperationExecutor
{
    public AddLayer(MgResourceService resSvc, String vm, IMapCreator creator)
    {
        super("AddLayer", resSvc, vm, creator);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            this.CreateMapFromResource(paramSetId);

            SQLiteStatement layerStmt = _conn.prepare(String.format("Select ParamValue from Params WHERE ParamSet=%d AND ParamName=\"LAYERDEFINITION\"", paramSetId));
            String layerDefinition = "";
            if (layerStmt.step()) {
                layerDefinition = CommonUtility.GetStringFromStatement(layerStmt, "ParamValue");   
            }

            SQLiteStatement groupStmt = _conn.prepare(String.format("Select ParamValue from Params WHERE ParamSet=%d AND ParamName=\"GROUPNAME\"", paramSetId));
            String groupName = "";
            if (groupStmt.step()) {
                groupName = CommonUtility.GetStringFromStatement(groupStmt, "ParamValue");
            }

            MgResourceIdentifier ldfId = new MgResourceIdentifier(layerDefinition);
            MgLayerBase layer = _creator.CreateLayer(ldfId);

            if (!CommonUtility.IsNullOrEmpty(groupName))
            {
                MgLayerGroupCollection groups = _map.getLayerGroups();
                int gidx = groups.indexOf(groupName);
                if (gidx >= 0)
                    layer.setGroup(groups.getItem(gidx));
            }

            MgLayerCollection layers = _map.getLayers();
            layers.add(layer);

            return TestResult.Integer(layers.getCount());
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