package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class UpdateServer extends SiteServiceOperationExecutor
{
    public UpdateServer(MgSite site, String unitTestVm)
    {
        super("UpdateServer", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "OLDNAME", param);
            ReadParameterValue(paramSetId, "NEWNAME", param);
            ReadParameterValue(paramSetId, "NEWDESCRIPTION", param);
            ReadParameterValue(paramSetId, "NEWADDRESS", param);

            _site.updateServer(param.get("OLDNAME"), param.get("NEWNAME"), param.get("NEWDESCRIPTION"), param.get("NEWADDRESS"));
            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}