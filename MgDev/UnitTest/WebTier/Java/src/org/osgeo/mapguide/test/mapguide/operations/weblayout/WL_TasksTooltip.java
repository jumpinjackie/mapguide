package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_TasksTooltip extends WebLayoutOperationExecutor
{
    public WL_TasksTooltip(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_TasksTooltip", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);
            MgWebTaskPane tp = _wl.getTaskPane();
            MgWebTaskBar tbar = tp.getTaskBar();
            MgWebWidgetCollection tbuttons = tbar.getTaskButtons();
            MgWebTaskBarWidget tasks = (MgWebTaskBarWidget)tbuttons.getWidget(MgWebTaskButtonType.Tasks);
            return TestResult.String(tasks.getTooltip());
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}