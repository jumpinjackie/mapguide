using OSGeo.MapGuide.Test.Common;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Operations
{
    public class WL_GetTitle : WebLayoutOperationExecutor<WL_GetTitle>
    {
        public WL_GetTitle(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                return new TestResult(_wl.GetTitle(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);   
            }
        }
    }

    public class WL_GetMapDefinition : WebLayoutOperationExecutor<WL_GetMapDefinition>
    {
        public WL_GetMapDefinition(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                return new TestResult(_wl.GetMapDefinition(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_GetScale : WebLayoutOperationExecutor<WL_GetScale>
    {
        public WL_GetScale(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                return new TestResult(_wl.GetScale().ToString(CultureInfo.InvariantCulture), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_GetCenter : WebLayoutOperationExecutor<WL_GetCenter>
    {
        public WL_GetCenter(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var center = _wl.GetCenter();
                if (center == null)
                {
                    return new TestResult("", "text/plain");
                }
                else
                {
                    var coord = center.Coordinate;
                    return new TestResult(coord.GetX() + "/" + coord.GetY(), "text/plain");
                }
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_ShowToolbar : WebLayoutOperationExecutor<WL_ShowToolbar>
    {
        public WL_ShowToolbar(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tb = _wl.GetToolBar();
                return new TestResult(CommonUtility.BooleanToString(tb.IsVisible()), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_ShowStatusbar : WebLayoutOperationExecutor<WL_ShowStatusbar>
    {
        public WL_ShowStatusbar(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var sb = _wl.GetStatusBar();
                return new TestResult(CommonUtility.BooleanToString(sb.IsVisible()), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_ShowTaskpane : WebLayoutOperationExecutor<WL_ShowTaskpane>
    {
        public WL_ShowTaskpane(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                return new TestResult(CommonUtility.BooleanToString(tp.IsVisible()), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_ShowTaskbar : WebLayoutOperationExecutor<WL_ShowTaskbar>
    {
        public WL_ShowTaskbar(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                var tb = tp.GetTaskBar();
                return new TestResult(CommonUtility.BooleanToString(tb.IsVisible()), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_ShowLegend : WebLayoutOperationExecutor<WL_ShowLegend>
    {
        public WL_ShowLegend(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var ip = _wl.GetInformationPane();
                return new TestResult(CommonUtility.BooleanToString(ip.IsLegendBandVisible()), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_ShowProperties : WebLayoutOperationExecutor<WL_ShowProperties>
    {
        public WL_ShowProperties(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var ip = _wl.GetInformationPane();
                return new TestResult(CommonUtility.BooleanToString(ip.IsPropertiesBandVisible()), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_GetTaskPaneWidth : WebLayoutOperationExecutor<WL_GetTaskPaneWidth>
    {
        public WL_GetTaskPaneWidth(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                return new TestResult(tp.GetWidth().ToString(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_GetInformationPaneWidth : WebLayoutOperationExecutor<WL_GetInformationPaneWidth>
    {
        public WL_GetInformationPaneWidth(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var ip = _wl.GetInformationPane();
                return new TestResult(ip.GetWidth().ToString(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_GetInitialTaskUrl : WebLayoutOperationExecutor<WL_GetInitialTaskUrl>
    {
        public WL_GetInitialTaskUrl(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                return new TestResult(tp.GetInitialTaskUrl(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_ShowContextMenu : WebLayoutOperationExecutor<WL_ShowContextMenu>
    {
        public WL_ShowContextMenu(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var ctx = _wl.GetContextMenu();
                return new TestResult(CommonUtility.BooleanToString(ctx.IsVisible()), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_TestUiItem : WebLayoutOperationExecutor<WL_TestUiItem>
    {
        public WL_TestUiItem(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);

                MgWebWidgetCollection coll = null;
                var param = new NameValueCollection();
                _unitTestVm.ReadParameterValue(paramSetId, "CONTAINER", param);
                _unitTestVm.ReadParameterValue(paramSetId, "INDEX", param);

                switch (param["CONTAINER"])
                {
                    case "toolbar":
                        var tb = _wl.GetToolBar();
                        coll = tb.GetWidgets();
                        break;
                    case "tasklist":
                        var tp = _wl.GetTaskPane();
                        var tbar = tp.GetTaskBar();
                        coll = tbar.GetTaskList();
                        break;
                    case "contextmenu":
                        coll = _wl.GetContextMenu();
                        break;
                }

                var widget = coll.GetWidget(Convert.ToInt32(param["INDEX"]));
                if (widget == null)
                    return new TestResult("Null widget", "text/plain");
                else
                    return new TestResult(FormatProperties(widget), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_HomeTooltip : WebLayoutOperationExecutor<WL_HomeTooltip>
    {
        public WL_HomeTooltip(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                var tbar = tp.GetTaskBar();
                var tbuttons = tbar.GetTaskButtons();
                var home = (MgWebTaskBarWidget)tbuttons.GetWidget(MgWebTaskButtonType.Home);
                return new TestResult(home.GetTooltip(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_HomeDescription : WebLayoutOperationExecutor<WL_HomeDescription>
    {
        public WL_HomeDescription(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                var tbar = tp.GetTaskBar();
                var tbuttons = tbar.GetTaskButtons();
                var home = (MgWebTaskBarWidget)tbuttons.GetWidget(MgWebTaskButtonType.Home);
                return new TestResult(home.GetDescription(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_BackTooltip : WebLayoutOperationExecutor<WL_BackTooltip>
    {
        public WL_BackTooltip(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                var tbar = tp.GetTaskBar();
                var tbuttons = tbar.GetTaskButtons();
                var back = (MgWebTaskBarWidget)tbuttons.GetWidget(MgWebTaskButtonType.Back);
                return new TestResult(back.GetTooltip(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_BackDescription : WebLayoutOperationExecutor<WL_BackDescription>
    {
        public WL_BackDescription(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                var tbar = tp.GetTaskBar();
                var tbuttons = tbar.GetTaskButtons();
                var back = (MgWebTaskBarWidget)tbuttons.GetWidget(MgWebTaskButtonType.Back);
                return new TestResult(back.GetDescription(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_ForwardTooltip : WebLayoutOperationExecutor<WL_ForwardTooltip>
    {
        public WL_ForwardTooltip(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                var tbar = tp.GetTaskBar();
                var tbuttons = tbar.GetTaskButtons();
                var fwd = (MgWebTaskBarWidget)tbuttons.GetWidget(MgWebTaskButtonType.Forward);
                return new TestResult(fwd.GetTooltip(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_ForwardDescription : WebLayoutOperationExecutor<WL_ForwardDescription>
    {
        public WL_ForwardDescription(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                var tbar = tp.GetTaskBar();
                var tbuttons = tbar.GetTaskButtons();
                var fwd = (MgWebTaskBarWidget)tbuttons.GetWidget(MgWebTaskButtonType.Forward);
                return new TestResult(fwd.GetDescription(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_TasksName : WebLayoutOperationExecutor<WL_TasksName>
    {
        public WL_TasksName(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                var tbar = tp.GetTaskBar();
                var tbuttons = tbar.GetTaskButtons();
                var tasks = (MgWebTaskBarWidget)tbuttons.GetWidget(MgWebTaskButtonType.Tasks);
                return new TestResult(tasks.GetName(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_TasksTooltip : WebLayoutOperationExecutor<WL_TasksTooltip>
    {
        public WL_TasksTooltip(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                var tbar = tp.GetTaskBar();
                var tbuttons = tbar.GetTaskButtons();
                var tasks = (MgWebTaskBarWidget)tbuttons.GetWidget(MgWebTaskButtonType.Tasks);
                return new TestResult(tasks.GetTooltip(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }

    public class WL_TasksDescription : WebLayoutOperationExecutor<WL_TasksDescription>
    {
        public WL_TasksDescription(IWebLayoutCreator layout, string unitTestVm)
            : base(layout, unitTestVm)
        {

        }

        public override TestResult Execute(int paramSetId)
        {
            try
            {
                CreateWebLayoutFromResource(paramSetId);
                var tp = _wl.GetTaskPane();
                var tbar = tp.GetTaskBar();
                var tbuttons = tbar.GetTaskButtons();
                var tasks = (MgWebTaskBarWidget)tbuttons.GetWidget(MgWebTaskButtonType.Tasks);
                return new TestResult(tasks.GetDescription(), "text/plain");
            }
            catch (MgException ex)
            {
                return TestResult.FromMgException(ex);
            }
        }
    }
}
