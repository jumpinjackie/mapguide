package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_ShowStatusbar extends WebLayoutOperationExecutor
{
    public WL_ShowStatusbar(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_ShowStatusbar", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            MgWebUiPane sb = _wl.getStatusBar();
            return TestResult.Boolean(sb.isVisible());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}