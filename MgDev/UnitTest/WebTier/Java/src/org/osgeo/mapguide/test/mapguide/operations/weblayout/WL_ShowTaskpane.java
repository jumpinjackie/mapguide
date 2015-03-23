package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_ShowTaskpane extends WebLayoutOperationExecutor
{
    public WL_ShowTaskpane(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_ShowTaskpane", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            MgWebTaskPane tp = _wl.getTaskPane();
            return TestResult.Boolean(tp.isVisible());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}