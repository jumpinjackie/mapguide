package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

public class WL_TestUiItem extends WebLayoutOperationExecutor
{
    public WL_TestUiItem(IWebLayoutCreator layout, String unitTestVm)
    {
        super("WL_TestUiItem", layout, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            CreateWebLayoutFromResource(paramSetId);

            MgWebWidgetCollection coll = null;
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "CONTAINER", param);
            ReadParameterValue(paramSetId, "INDEX", param);

            switch (param.get("CONTAINER"))
            {
                case "toolbar":
                    MgWebToolBar tb = _wl.getToolBar();
                    coll = tb.getWidgets();
                    break;
                case "tasklist":
                    MgWebTaskPane tp = _wl.getTaskPane();
                    MgWebTaskBar tbar = tp.getTaskBar();
                    coll = tbar.getTaskList();
                    break;
                case "contextmenu":
                    coll = _wl.getContextMenu();
                    break;
            }

            MgWebWidget widget = coll.getWidget(Integer.parseInt(param.get("INDEX")));
            if (widget == null)
                return TestResult.String("Null widget");
            else
                return TestResult.String(FormatProperties(widget));
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}