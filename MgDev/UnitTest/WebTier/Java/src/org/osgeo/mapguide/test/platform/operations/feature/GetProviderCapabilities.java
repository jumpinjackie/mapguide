package org.osgeo.mapguide.test.platform.operations.feature;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class GetProviderCapabilities extends FeatureServiceOperationExecutor
{
    public GetProviderCapabilities(MgFeatureService featSvc, String dbPath)
    {
        super("GetProviderCapabilities", featSvc, dbPath);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "PROVIDER", param);

            String p1 = "";
            if (param.containsKey("PROVIDER"))
                p1 = param.get("PROVIDER");
            MgByteReader reader = _featureService.getCapabilities(p1);
            return TestResult.FromByteReader(reader);
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