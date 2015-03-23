package org.osgeo.mapguide.test.platform.operations.feature;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class GetFeatureProviders extends FeatureServiceOperationExecutor
{
    public GetFeatureProviders(MgFeatureService featSvc, String dbPath)
    {
        super("GetFeatureProviders", featSvc, dbPath);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            MgByteReader reader = _featureService.getFeatureProviders();
            return TestResult.FromByteReader(null);
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