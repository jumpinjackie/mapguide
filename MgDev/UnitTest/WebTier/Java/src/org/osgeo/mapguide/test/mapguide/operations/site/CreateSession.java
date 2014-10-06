package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class CreateSession extends SiteServiceOperationExecutor
{
    private IMapGuideSession _session;

    public CreateSession(MgSite site, String unitTestVm, IMapGuideSession session)
    {
        super("CreateSession", site, unitTestVm);
        _session = session;
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            MgUserInformation user = new MgUserInformation();
            user.setMgUsernamePassword("Administrator", "admin");
            user.setLocale("en");

            MgSite site = new MgSite();
            site.open(user);

            String session = site.createSession();
            _session.setSessionID(session);
            site.close();

            return TestResult.String(session);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}