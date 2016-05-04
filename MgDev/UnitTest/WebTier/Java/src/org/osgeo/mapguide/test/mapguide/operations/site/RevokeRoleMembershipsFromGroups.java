package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class RevokeRoleMembershipsFromGroups extends SiteServiceOperationExecutor
{
    public RevokeRoleMembershipsFromGroups(MgSite site, String unitTestVm)
    {
        super("RevokeRoleMembershipsFromGroups", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "ROLES", param);
            ReadParameterValue(paramSetId, "GROUPS", param);

            MgStringCollection roles = CommonUtility.StringToMgStringCollection(param.get("ROLES"));
            MgStringCollection groups = CommonUtility.StringToMgStringCollection(param.get("GROUPS"));

            _site.revokeRoleMembershipsFromGroups(roles, groups);

            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}