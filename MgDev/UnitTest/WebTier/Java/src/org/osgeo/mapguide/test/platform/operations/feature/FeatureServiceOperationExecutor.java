package org.osgeo.mapguide.test.platform.operations.feature;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

import com.almworks.sqlite4java.*;

public abstract class FeatureServiceOperationExecutor extends PlatformApiTestExecutor
{
    protected MgFeatureService _featureService;

    protected FeatureServiceOperationExecutor(String opName, MgFeatureService featSvc, String dbPath)
    {
        super(opName, ApiTypes.Platform, dbPath);
        _featureService = featSvc;
    }
}