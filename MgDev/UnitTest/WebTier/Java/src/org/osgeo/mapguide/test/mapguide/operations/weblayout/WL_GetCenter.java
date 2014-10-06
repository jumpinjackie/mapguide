package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_GetCenter extends WebLayoutOperationExecutor
{
    public WL_GetCenter(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_GetCenter", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            MgPoint center = _wl.getCenter();
            if (center == null)
            {
                return TestResult.String("");
            }
            else
            {
                MgCoordinate coord = center.getCoordinate();
                return TestResult.String(CommonUtility.DoubleToString(coord.getX()) + "/" + CommonUtility.DoubleToString(coord.getY()));
            }
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}