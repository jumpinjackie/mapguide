package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class UpdateUser extends SiteServiceOperationExecutor
{
    public UpdateUser(MgSite site, String unitTestVm)
    {
        super("UpdateUser", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "USERID", param);
            ReadParameterValue(paramSetId, "NEWUSERID", param);
            ReadParameterValue(paramSetId, "NEWUSERNAME", param);
            ReadParameterValue(paramSetId, "NEWPASSWORD", param);
            ReadParameterValue(paramSetId, "NEWDESCRIPTION", param);

            _site.updateUser(param.get("USERID"), param.get("NEWUSERID"), param.get("NEWUSERNAME"), param.get("NEWPASSWORD"), param.get("NEWDESCRIPTION"));
            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}