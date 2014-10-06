package org.osgeo.mapguide.test.mapguide.operations.admin;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class ClearLog extends ServerAdminOperationExecutor
{
    public ClearLog(MgServerAdmin admin, String unitTestVm)
    {
        super("ClearLog", admin, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "LOGTYPE", param);

            boolean cleared = _serverAdmin.clearLog(param.get("LOGTYPE"));
            return TestResult.Boolean(cleared);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}