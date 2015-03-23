package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class DeleteUsers extends SiteServiceOperationExecutor
{
    public DeleteUsers(MgSite site, String unitTestVm)
    {
        super("DeleteUsers", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "USERS", param);

            MgStringCollection users = CommonUtility.StringToMgStringCollection(param.get("USERS"));

            _site.deleteUsers(users);
            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}