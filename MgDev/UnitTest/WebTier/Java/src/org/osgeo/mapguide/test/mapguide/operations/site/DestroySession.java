package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class DestroySession extends SiteServiceOperationExecutor
{
    public DestroySession(MgSite site, String unitTestVm)
    {
        super("DestroySession", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            //This is what PHP one is giving us
            return TestResult.String("Not Implemented Yet");
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}