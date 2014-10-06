package org.osgeo.mapguide.test.mapguide.operations.admin;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class LoadPackage extends ServerAdminOperationExecutor
{
    public LoadPackage(MgServerAdmin admin, String unitTestVm)
    {
        super("LoadPackage", admin, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "PACKAGENAME", param);

            _serverAdmin.loadPackage(param.get("PACKAGENAME"));
            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}