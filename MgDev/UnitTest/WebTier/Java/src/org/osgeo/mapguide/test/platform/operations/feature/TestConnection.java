package org.osgeo.mapguide.test.platform.operations.feature;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.*;

public class TestConnection extends FeatureServiceOperationExecutor
{
    public TestConnection(MgFeatureService featSvc, String unitTestVm)
    {
        super("TestConnection", featSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);

            boolean bRes = false;
            MgResourceIdentifier resId = null;
            if (!CommonUtility.IsNullOrEmpty(param.get("RESOURCEID")))
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
                bRes = _featureService.testConnection(resId);
            }
            else
            {
                ReadParameterValue(paramSetId, "PROVIDER", param);
                ReadParameterValue(paramSetId, "CONNECTIONSTRING", param);
                String providerName = "";
                if (param.containsKey("PROVIDER"))
                    providerName = param.get("PROVIDER");
                String connString = "";
                if (param.containsKey("CONNECTIONSTRING"))
                    connString = param.get("CONNECTIONSTRING");
                bRes = _featureService.testConnection(providerName, connString);
            }

            return TestResult.Boolean(bRes);
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