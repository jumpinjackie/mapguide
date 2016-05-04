package org.osgeo.mapguide.test.mapguide.operations.admin;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class GetPackageLog extends ServerAdminOperationExecutor
{
    public GetPackageLog(MgServerAdmin admin, String unitTestVm)
    {
        super("GetPackageLog", admin, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "PACKAGENAME", param);

            MgByteReader reader = _serverAdmin.getPackageLog(param.get("PACKAGENAME"));
            return TestResult.FromByteReader(reader);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}