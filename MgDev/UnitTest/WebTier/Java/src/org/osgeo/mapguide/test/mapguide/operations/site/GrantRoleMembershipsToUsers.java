package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class GrantRoleMembershipsToUsers extends SiteServiceOperationExecutor
{
    public GrantRoleMembershipsToUsers(MgSite site, String unitTestVm)
    {
        super("GrantRoleMembershipsToUsers", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "ROLES", param);
            ReadParameterValue(paramSetId, "USERS", param);

            MgStringCollection roles = CommonUtility.StringToMgStringCollection(param.get("ROLES"));
            MgStringCollection users = CommonUtility.StringToMgStringCollection(param.get("USERS"));

            _site.grantRoleMembershipsToUsers(roles, users);

            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}