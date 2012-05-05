using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgPrintComponent : MgViewerComponent
    {
        public MgPrintComponent()
        {
            this.Label = this.ToolTipText = Properties.Resources.TextPrint;
            this.Icon = Properties.Resources.print;
        }

        protected override MgControlImpl CreateControlImpl()
        {
            return new MgPrintControlImpl(this.Viewer);
        }
    }
}
