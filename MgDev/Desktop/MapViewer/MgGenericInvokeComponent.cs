using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgGenericInvokeComponent : MgComponent
    {
        public MgGenericInvokeComponent()
        {
            this.Icon = Properties.Resources.invoke_script;
        }

        public event EventHandler Invoked;

        public override void Invoke()
        {
            var h = this.Invoked;
            if (h != null)
                h(this, EventArgs.Empty);
        }
    }
}
