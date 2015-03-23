package org.osgeo.mapguide.test.mapguide.operations.admin;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class GetLog extends ServerAdminOperationExecutor
{
    public GetLog(MgServerAdmin admin, String unitTestVm)
    {
        super("GetLog", admin, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "LOGTYPE", param);
            ReadParameterValue(paramSetId, "NUMENTRIES", param);

            MgByteReader reader = null;
            if (param.get("NUMENTRIES") == null)
            {
                reader = _serverAdmin.getLog(param.get("LOGTYPE"));
            }
            else
            {
                reader = _serverAdmin.getLog(param.get("LOGTYPE"), Integer.parseInt(param.get("NUMENTRIES")));
            }

            return TestResult.FromByteReader(reader);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}