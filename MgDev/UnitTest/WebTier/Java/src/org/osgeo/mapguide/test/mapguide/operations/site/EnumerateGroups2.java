package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class EnumerateGroups2 extends SiteServiceOperationExecutor
{
    public EnumerateGroups2(MgSite site, String unitTestVm)
    {
        super("EnumerateGroups2", site, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "USER", param);
            ReadParameterValue(paramSetId, "LOGIN", param);
            ReadParameterValue(paramSetId, "PASSWORD", param);

            MgUserInformation userInfo = new MgUserInformation();
            userInfo.setMgUsernamePassword(param.get("LOGIN"), param.get("PASSWORD"));
            userInfo.setLocale("en");

            MgSite site = new MgSite();
            site.open(userInfo);

            MgByteReader reader = site.enumerateGroups(param.get("USER"));
            site.close();

            return TestResult.FromByteReader(reader);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}