package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class EnumerateGroups extends SiteServiceOperationExecutor
{
    public EnumerateGroups(MgSite site, String unitTestVm)
    {
        super("EnumerateGroups", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "USER", param);
            ReadParameterValue(paramSetId, "ROLE", param);

            String user = param.get("USER");
            if (user == null)
                user = "";
            String role = param.get("ROLE");
            if (role == null)
                role = "";
            MgByteReader reader = _site.enumerateGroups(user, role);
            return TestResult.FromByteReader(reader);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}