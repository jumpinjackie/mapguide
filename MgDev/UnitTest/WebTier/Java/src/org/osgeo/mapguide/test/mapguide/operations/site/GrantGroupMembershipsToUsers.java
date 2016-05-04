package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class GrantGroupMembershipsToUsers extends SiteServiceOperationExecutor
{
    public GrantGroupMembershipsToUsers(MgSite site, String unitTestVm)
    {
        super("GrantGroupMembershipsToUsers", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "GROUPS", param);
            ReadParameterValue(paramSetId, "USERS", param);

            MgStringCollection groups = CommonUtility.StringToMgStringCollection(param.get("GROUPS"));
            MgStringCollection users = CommonUtility.StringToMgStringCollection(param.get("USERS"));

            _site.grantGroupMembershipsToUsers(groups, users);

            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}