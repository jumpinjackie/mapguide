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
            this.Label = this.ToolTipText = Properties.Resources.TitleQuery;
        }

        protected override MgControlImpl CreateControlImpl()
        {
            return new MgQueryControlImpl(this.Viewer);
        }
    }
}
