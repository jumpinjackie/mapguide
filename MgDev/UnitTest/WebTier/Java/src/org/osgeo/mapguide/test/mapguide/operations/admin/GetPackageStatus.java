package org.osgeo.mapguide.test.mapguide.operations.admin;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class GetPackageStatus extends ServerAdminOperationExecutor
{
    public GetPackageStatus(MgServerAdmin admin, String unitTestVm)
    {
        super("GetPackageStatus", admin, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "PACKAGENAME", param);

            MgPackageStatusInformation status = _serverAdmin.getPackageStatus(param.get("PACKAGENAME"));
            String code = status.getStatusCode();
            return TestResult.String(code);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}