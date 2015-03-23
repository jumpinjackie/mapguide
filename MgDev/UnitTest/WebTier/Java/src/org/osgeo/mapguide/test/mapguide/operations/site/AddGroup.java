package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class AddGroup extends SiteServiceOperationExecutor
{
    public AddGroup(MgSite site, String unitTestVm)
    {
        super("AddGroup", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "GROUP", param);
            ReadParameterValue(paramSetId, "DESCRIPTION", param);

            String group = param.get("GROUP");
            if (group == null)
                group = "";
            String desc = param.get("DESCRIPTION");
            if (desc == null)
                desc = "";
            _site.addGroup(group, desc);
            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}