package org.osgeo.mapguide.test.platform.operations.feature;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class GetClasses extends FeatureServiceOperationExecutor
{
    public GetClasses(MgFeatureService featSvc, String unitTestVm)
    {
        super("GetClasses", featSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);
            ReadParameterValue(paramSetId, "SCHEMANAME", param);

            MgResourceIdentifier resId = null;
            if (param.containsKey("RESOURCEID"))
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            MgStringCollection result = _featureService.getClasses(resId, param.get("SCHEMANAME"));
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