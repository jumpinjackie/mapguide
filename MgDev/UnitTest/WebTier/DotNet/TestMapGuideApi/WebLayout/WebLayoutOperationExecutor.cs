using OSGeo.MapGuide.Test.Common;
using SqliteDotNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSGeo.MapGuide.Test.Operations
{
    public interface IWebLayoutCreator
    {
        MgWebLayout CreateWebLayout(MgResourceIdentifier resId);
    }

    /// <summary>
    /// The base class of all test executors using the MgWebLayout
    /// </summary>
    /// <typeparam name="T">The test executor subclass type</typeparam>
    public abstract class WebLayoutOperationExecutor<T> : PlatformApiTestExecutor
    {
        protected MgWebLayout _wl;
        protected IWebLayoutCreator _creator;

        protected WebLayoutOperationExecutor(IWebLayoutCreator wl, string unitTestVm)
            : base(typeof(T).Name.ToUpper(), ApiTypes.Platform, unitTestVm)
        {
            _creator = wl;
        }

        protected void CreateWebLayoutFromResource(int paramSetId)
        {
            if (_wl != null)
                return;

            _unitTestVm.Execute("Select ParamValue from Params WHERE ParamSet={0} AND ParamName=\"WEBLAYOUT\"", paramSetId);
            string wlId = _unitTestVm.GetString("ParamValue");
            if (string.IsNullOrEmpty(wlId))
            {
                wlId = "Library://UnitTest/layouts/Test.WebLayout";
            }
            MgResourceIdentifier resId = new MgResourceIdentifier(wlId);
            _wl = _creator.CreateWebLayout(resId);
        }

        protected string FormatProperties(MgWebWidget it)
        {
            string name = "";
            switch (it.GetType())
            { 
                case MgWebWidgetType.Separator:
                    name = "";
                    break;
                case MgWebWidgetType.Command:
                    var cmd = ((MgWebCommandWidget)it).GetCommand();
                    name = cmd.GetLabel();
                    break;
                case MgWebWidgetType.Flyout:
                    name = ((MgWebFlyoutWidget)it).GetLabel();
                    break;
            }

            string str = "[" + name + "/" + it.GetType();
            if (it.GetType() == MgWebWidgetType.Separator)
            {
                return str + "]";
            }
            else if (it.GetType() == MgWebWidgetType.Command)
            {
                var cmd = ((MgWebCommandWidget)it).GetCommand();
                return str + "/" + cmd.GetName() + "]";
            }
            else if (it.GetType() == MgWebWidgetType.Flyout)
            {
                var fly = ((MgWebFlyoutWidget)it);
                str = str + "/" + fly.GetIconUrl();
                var coll = fly.GetSubItems();
                for (int i = 0; i < coll.GetCount(); i++)
                {
                    var widget = coll.GetWidget(i);
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
}