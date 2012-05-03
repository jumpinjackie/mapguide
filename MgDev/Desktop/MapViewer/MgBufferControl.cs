using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A generic component that generates buffers from selected features on the map
    /// </summary>
    [ToolboxItem(true)]
    public class MgBufferControl : MgViewerComponent
    {
        public MgBufferControl()
        {
            this.Icon = Properties.Resources.buffer;
            this.Label = Properties.Resources.TitleBuffer;
        }

        protected override MgControlImpl CreateControlImpl()
        {
            return new MgBufferControlImpl(this.Viewer);
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
