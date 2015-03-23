package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class EnumerateRoles2 extends SiteServiceOperationExecutor
{
    public EnumerateRoles2(MgSite site, String unitTestVm)
    {
        super("EnumerateRoles2", site, unitTestVm);
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

            MgStringCollection roles = site.enumerateRoles(param.get("USER"));
            site.close();

            return TestResult.StringCollection(roles);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}