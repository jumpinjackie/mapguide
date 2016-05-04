package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_GetInitialTaskUrl extends WebLayoutOperationExecutor
{
    public WL_GetInitialTaskUrl(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_GetInitialTaskUrl", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            MgWebTaskPane tp = _wl.getTaskPane();
            return TestResult.String(tp.getInitialTaskUrl());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}