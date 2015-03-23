using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgGenericInvokeComponent : MgComponent
    {
        public MgGenericInvokeComponent()
        {
            this.Icon = Properties.Resources.invoke_script;
        }

        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Indicates whether this component can be invoked without a loaded map")] //NOXLATE
        [MgComponentProperty]
        public bool CanInvokeWithoutLoadedMap
        {
            get;
            set;
        }

        protected override bool RequiresLoadedMap
        {
            get
            {
                return !this.CanInvokeWithoutLoadedMap;
            }
        }

        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Raised when this component has been invoked")] //NOXLATE
        public event EventHandler Invoked;

        public override void Invoke()
        {
            var h = this.Invoked;
            if (h != null)
                h(this, EventArgs.Empty);
        }
    }
}
