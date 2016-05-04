package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class AddUser extends SiteServiceOperationExecutor
{
    public AddUser(MgSite site, String unitTestVm)
    {
        super("AddUser", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "USERID", param);
            ReadParameterValue(paramSetId, "USERNAME", param);
            ReadParameterValue(paramSetId, "PASSWORD", param);
            ReadParameterValue(paramSetId, "DESCRIPTION", param);

            _site.addUser(param.get("USERID"), param.get("USERNAME"), param.get("PASSWORD"), param.get("DESCRIPTION"));
            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}