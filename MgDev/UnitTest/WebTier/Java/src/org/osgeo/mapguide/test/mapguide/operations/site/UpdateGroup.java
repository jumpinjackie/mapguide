package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class UpdateGroup extends SiteServiceOperationExecutor
{
    public UpdateGroup(MgSite site, String unitTestVm)
    {
        super("UpdateGroup", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "GROUP", param);
            ReadParameterValue(paramSetId, "NEWGROUP", param);
            ReadParameterValue(paramSetId, "NEWDESCRIPTION", param);

            String group = param.get("GROUP");
            if (group == null)
                group = "";
            String newGroup = param.get("NEWGROUP");
            if (newGroup == null)
                newGroup = "";
            String newDesc = param.get("NEWDESCRIPTION");
            if (newDesc == null)
                newDesc = "";
            _site.updateGroup(group, newGroup, newDesc);
            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}