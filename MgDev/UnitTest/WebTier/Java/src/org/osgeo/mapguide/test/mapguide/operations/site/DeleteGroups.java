package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class DeleteGroups extends SiteServiceOperationExecutor
{
    public DeleteGroups(MgSite site, String unitTestVm)
    {
        super("DeleteGroups", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "GROUPS", param);

            MgStringCollection groups = CommonUtility.StringToMgStringCollection(param.get("GROUPS"));
            _site.deleteGroups(groups);

            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}