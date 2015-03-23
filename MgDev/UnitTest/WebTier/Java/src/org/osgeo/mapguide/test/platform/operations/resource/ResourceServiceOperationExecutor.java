package org.osgeo.mapguide.test.platform.operations.resource;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

import com.almworks.sqlite4java.*;

public abstract class ResourceServiceOperationExecutor extends PlatformApiTestExecutor
{
    protected MgResourceService _resourceService;

    protected ResourceServiceOperationExecutor(String opName, MgResourceService resSvc, String unitTestVm)
    {
        super(opName, ApiTypes.Platform, unitTestVm);
        _resourceService = resSvc;
    }
}