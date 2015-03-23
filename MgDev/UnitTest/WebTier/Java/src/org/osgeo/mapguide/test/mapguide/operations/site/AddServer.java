package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class AddServer extends SiteServiceOperationExecutor
{
    public AddServer(MgSite site, String unitTestVm)
    {
        super("AddServer", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "NAME", param);
            ReadParameterValue(paramSetId, "DESCRIPTION", param);
            ReadParameterValue(paramSetId, "ADDRESS", param);

            _site.addServer(param.get("NAME"), param.get("DESCRIPTION"), param.get("ADDRESS"));
            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}