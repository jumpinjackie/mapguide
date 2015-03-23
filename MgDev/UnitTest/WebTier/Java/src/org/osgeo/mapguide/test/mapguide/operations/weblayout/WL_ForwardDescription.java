package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_ForwardDescription extends WebLayoutOperationExecutor
{
    public WL_ForwardDescription(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_ForwardDescription", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            MgWebTaskPane tp = _wl.getTaskPane();
            MgWebTaskBar tbar = tp.getTaskBar();
            MgWebWidgetCollection tbuttons = tbar.getTaskButtons();
            MgWebTaskBarWidget fwd = (MgWebTaskBarWidget)tbuttons.getWidget(MgWebTaskButtonType.Forward);
            return TestResult.String(fwd.getDescription());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}