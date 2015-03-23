package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_ShowTaskbar extends WebLayoutOperationExecutor
{
    public WL_ShowTaskbar(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_ShowTaskbar", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            MgWebTaskPane tp = _wl.getTaskPane();
            MgWebTaskBar tb = tp.getTaskBar();
            return TestResult.Boolean(tb.isVisible());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}