using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

#pragma warning disable 1591

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

        [Category("MapGuide Component Properties")] //NOXLATE
        [Description("An optional list of layer names that will be displayed in the list of queryable layers. If left empty, all layers from the map will be displayed in the list of queryable layers")] //NOXLATE
        [DefaultValue(MeasurementUnit.Meters)]
        [MgComponentProperty]
        public string[] LayerList
        {
            get;
            set;
        }

        protected override MgControlView CreateControlView()
        {
            if (this.LayerList == null || this.LayerList.Length == 0)
                return new MgQueryControlImpl(this.Viewer);
            else
                return new MgQueryControlImpl(this.Viewer, this.LayerList);
        }
    }
}
