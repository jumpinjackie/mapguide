package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class EnumerateServers extends SiteServiceOperationExecutor
{
    public EnumerateServers(MgSite site, String unitTestVm)
    {
        super("EnumerateServers", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            MgByteReader reader = _site.enumerateServers();
            return TestResult.FromByteReader(reader);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}