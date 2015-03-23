package org.osgeo.mapguide.test.mapguide.operations.site;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

import com.almworks.sqlite4java.*;

public abstract class SiteServiceOperationExecutor extends PlatformApiTestExecutor
{
    protected MgSite _site;

    protected SiteServiceOperationExecutor(String opName, MgSite site, String unitTestVm)
    {
        super(opName, ApiTypes.Platform, unitTestVm);
        _site = site;
    }
}