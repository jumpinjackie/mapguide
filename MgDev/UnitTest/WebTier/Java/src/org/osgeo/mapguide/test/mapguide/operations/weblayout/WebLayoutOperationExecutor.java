package org.osgeo.mapguide.test.mapguide.operations.weblayout;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;

import com.almworks.sqlite4java.*;

public abstract class WebLayoutOperationExecutor extends PlatformApiTestExecutor
{
    protected MgWebLayout _wl;
    protected IWebLayoutCreator _creator;

    protected WebLayoutOperationExecutor(String name, IWebLayoutCreator wl, String unitTestVm)
    {
        super(name, ApiTypes.Platform, unitTestVm);
        _creator = wl;
    }

    protected void CreateWebLayoutFromResource(int paramSetId)
    {
        if (_wl != null)
            return;

        try {
            SQLiteStatement stmt = _conn.prepare(String.format("Select ParamValue from Params WHERE ParamSet=%d AND ParamName=\"WEBLAYOUT\"", paramSetId));
            String wlId = null;
            if (stmt.step()) {
                wlId = CommonUtility.GetStringFromStatement(stmt, "ParamValue");
                if (CommonUtility.IsNullOrEmpty(wlId))
                {
                    wlId = "Library://UnitTest/layouts/Test.WebLayout";
                }
            } else {
                wlId = "Library://UnitTest/layouts/Test.WebLayout";
            }
            MgResourceIdentifier resId = new MgResourceIdentifier(wlId);
            _wl = _creator.CreateWebLayout(resId);
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
    }

    protected String FormatProperties(MgWebWidget it)
    {
        String name = "";
        switch (it.getType())
        { 
            case MgWebWidgetType.Separator:
                name = "";
                break;
            case MgWebWidgetType.Command:
                MgWebCommand cmd = ((MgWebCommandWidget)it).getCommand();
                name = cmd.getLabel();
                break;
            case MgWebWidgetType.Flyout:
                name = ((MgWebFlyoutWidget)it).getLabel();
                break;
        }

        String str = "[" + name + "/" + it.getType();
        if (it.getType() == MgWebWidgetType.Separator)
        {
            return str + "]";
        }
        else if (it.getType() == MgWebWidgetType.Command)
        {
            MgWebCommand cmd = ((MgWebCommandWidget)it).getCommand();
            return str + "/" + cmd.getName() + "]";
        }
        else if (it.getType() == MgWebWidgetType.Flyout)
        {
            MgWebFlyoutWidget fly = ((MgWebFlyoutWidget)it);
            str = str + "/" + fly.getIconUrl();
            MgWebWidgetCollection coll = fly.getSubItems();
            for (int i = 0; i < coll.getCount(); i++)
            {
                MgWebWidget widget = coll.getWidget(i);
                str = str + "/" + FormatProperties(widget);
            }
            str = str + "]";
        }
        else
        {
            return "[** error **]";
        }

        return str;
    }
}