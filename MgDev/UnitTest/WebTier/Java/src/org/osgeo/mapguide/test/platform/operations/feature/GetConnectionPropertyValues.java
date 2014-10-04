package org.osgeo.mapguide.test.platform.operations.feature;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class GetConnectionPropertyValues extends FeatureServiceOperationExecutor
{
    public GetConnectionPropertyValues(MgFeatureService featSvc, String unitTestVm)
    {
        super("GetConnectionPropertyValues", featSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "PROVIDER", param);
            ReadParameterValue(paramSetId, "PROPERTY", param);
            ReadParameterValue(paramSetId, "CONNECTIONSTRING", param);

            MgStringCollection result = _featureService.getConnectionPropertyValues(param.get("PROVIDER"), param.get("PROPERTY"), param.get("CONNECTIONSTRING"));
            return TestResult.StringCollection(result);
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