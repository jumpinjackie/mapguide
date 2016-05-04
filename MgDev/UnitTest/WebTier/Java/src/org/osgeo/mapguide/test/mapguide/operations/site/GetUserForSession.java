package org.osgeo.mapguide.test.mapguide.operations.site;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class GetUserForSession extends SiteServiceOperationExecutor
{
    private IMapGuideSession _session;

    public GetUserForSession(MgSite site, String unitTestVm, IMapGuideSession session)
    {
        super("GetUserForSession", site, unitTestVm);
        _session = session;
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            MgSite site = new MgSite();
            MgUserInformation user = new MgUserInformation();
            String sessionId = "";
            if (_session.getSessionID() != null)
                sessionId = _session.getSessionID();
            user.setMgSessionId(sessionId);
            site.open(user);
            String userId = site.getUserForSession();
            site.close();
            return TestResult.String(userId);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}