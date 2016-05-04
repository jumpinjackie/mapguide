package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class EnumerateUsers extends SiteServiceOperationExecutor
{
    public EnumerateUsers(MgSite site, String unitTestVm)
    {
        super("EnumerateUsers", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "GROUP", param);
            ReadParameterValue(paramSetId, "ROLE", param);
            ReadParameterValue(paramSetId, "INCLUDEGROUPS", param);

            MgByteReader reader = null;
            String group = param.get("GROUP");
            if (group == null)
                group = "";
            if (param.get("ROLE") != null)
            {
                String role = param.get("ROLE");
                if (role == null)
                    role = "";
                String includeGroups = param.get("INCLUDEGROUPS");
                if (includeGroups == null)
                    includeGroups = "1";
                reader = _site.enumerateUsers(group, role, (includeGroups.equals("1")));
            }
            else
            {
                reader = _site.enumerateUsers(group);
            }
            return TestResult.FromByteReader(reader);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}