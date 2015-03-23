package org.osgeo.mapguide.test.platform.operations.maplayer;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.*;

public abstract class MapLayerOperationExecutor extends PlatformApiTestExecutor
{
    protected MgMapBase _map;
    protected MgResourceService _resourceService;
    protected IMapCreator _creator;

    protected MapLayerOperationExecutor(String opName, MgResourceService resSvc, String unitTestVm, IMapCreator creator)
    {
        super(opName, ApiTypes.Platform, unitTestVm);
        _resourceService = resSvc;
        _creator = creator;
    }

    protected void CreateMapFromResource(int paramSetId)
    {
        if (_map != null)
            return;

        try {
            SQLiteStatement stmt = _conn.prepare(String.format("Select ParamValue from Params WHERE ParamSet=%d AND ParamName=\"MAPDEFINITION\"", paramSetId));
            String mapName = null;
            if (stmt.step()) {
                mapName = CommonUtility.GetStringFromStatement(stmt, "ParamValue");
                if (CommonUtility.IsNullOrEmpty(mapName))
                {
                    mapName = "Library://maplayertest/World.MapDefinition";
                }
            } else {
                mapName = "Library://maplayertest/World.MapDefinition";
            }
            MgResourceIdentifier mdfId = new MgResourceIdentifier(mapName);
            _map = _creator.CreateMap(mdfId);
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
    }

    protected MgLayerBase FindLayer(String name)
    {
        if (_map != null)
        {
            MgLayerCollection layers = _map.getLayers();
            int lidx = layers.indexOf(name);
            if (lidx >= 0)
                return layers.getItem(lidx);
        }
        return null;
    }
}