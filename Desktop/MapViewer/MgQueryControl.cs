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
    public class MgQueryControl : MgViewerComponent
    {
        public MgQueryControl()
        {
            this.Icon = Properties.Resources.search;
            this.Label = Properties.Resources.TitleQuery;
        }

        protected override MgControlImpl CreateControlImpl()
        {
            return new MgQueryControlImpl(this.Viewer);
        }

        protected override void SubscribeViewerEvents(IMapViewer viewer)
        {
            base.SubscribeViewerEvents(viewer);
        }

        protected override void UnsubscribeViewerEvents(IMapViewer viewer)
        {
            base.UnsubscribeViewerEvents(viewer);
        }
    }
}
