package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_GetScale extends WebLayoutOperationExecutor
{
    public WL_GetScale(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_GetScale", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            return TestResult.Double(_wl.getScale());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}