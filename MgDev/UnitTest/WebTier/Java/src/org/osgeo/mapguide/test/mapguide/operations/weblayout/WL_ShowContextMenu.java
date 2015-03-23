package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_ShowContextMenu extends WebLayoutOperationExecutor
{
    public WL_ShowContextMenu(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_ShowContextMenu", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            MgWebContextMenu ctx = _wl.getContextMenu();
            return TestResult.Boolean(ctx.isVisible());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}