package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class EnumerateRoles extends SiteServiceOperationExecutor
{
    public EnumerateRoles(MgSite site, String unitTestVm)
    {
        super("EnumerateRoles", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "USER", param);
            ReadParameterValue(paramSetId, "GROUP", param);

            MgStringCollection roles = _site.enumerateRoles(param.get("USER"), param.get("GROUP"));

            return TestResult.StringCollection(roles);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}