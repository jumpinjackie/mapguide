package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_GetTitle extends WebLayoutOperationExecutor
{
    public WL_GetTitle(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_GetTitle", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            return TestResult.String(_wl.getTitle());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);   
        }
    }
}