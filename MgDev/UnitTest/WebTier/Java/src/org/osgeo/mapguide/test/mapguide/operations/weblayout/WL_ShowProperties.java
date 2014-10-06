package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_ShowProperties extends WebLayoutOperationExecutor
{
    public WL_ShowProperties(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_ShowProperties", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            MgWebInformationPane ip = _wl.getInformationPane();
            return TestResult.Boolean(ip.isPropertiesBandVisible());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}