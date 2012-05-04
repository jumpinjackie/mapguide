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
    public class MgBufferComponent : MgViewerComponent
    {
        public MgBufferComponent()
        {
            this.Icon = Properties.Resources.buffer;
            this.Label = Properties.Resources.TitleBuffer;
            this.DefaultLayerName = "Buffer 1";
            this.DefaultBufferUnits = MeasurementUnit.Meters;
        }

        [Description("The default name of the buffer layer that is created")]
        public string DefaultLayerName { get; set; }

        [Description("The default buffer unit selection")]
        public MeasurementUnit DefaultBufferUnits { get; set; }

        protected override MgControlImpl CreateControlImpl()
        {
            return new MgBufferControlImpl(this.Viewer, this.DefaultLayerName, this.DefaultBufferUnits);
        }
    }
}
