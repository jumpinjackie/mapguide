using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A generic feature query component
    /// </summary>
    [ToolboxItem(true)]
    public class MgQueryComponent : MgViewerComponent
    {
        public MgQueryComponent()
        {
            this.Icon = Properties.Resources.search;
            this.Label = this.ToolTipText = Strings.TitleQuery;
        }

        protected override MgControlView CreateControlView()
        {
            return new MgQueryControlImpl(this.Viewer);
        }
    }
}
