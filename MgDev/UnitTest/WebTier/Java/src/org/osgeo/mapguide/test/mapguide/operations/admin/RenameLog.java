package org.osgeo.mapguide.test.mapguide.operations.admin;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class RenameLog extends ServerAdminOperationExecutor
{
    public RenameLog(MgServerAdmin admin, String unitTestVm)
    {
        super("RenameLog", admin, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "OLDFILENAME", param);
            ReadParameterValue(paramSetId, "NEWFILENAME", param);

            _serverAdmin.renameLog(param.get("OLDFILENAME"), param.get("NEWFILENAME"));
            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}