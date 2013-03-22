using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgPrintComponent : MgViewerComponent
    {
        public MgPrintComponent()
        {
            this.Label = this.ToolTipText = Strings.TextPrint;
            this.Icon = Properties.Resources.print;
        }

        protected override MgControlView CreateControlView()
        {
            return new MgPrintControlImpl(this.Viewer);
        }
    }
}
