using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Text;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A generic component that allows the user to annotate the map with redlines
    /// </summary>
    [ToolboxItem(true)]
    public partial class MgRedlineComponent : MgViewerComponent
    {
        //TODO: Feature parity with the Fusion implementation
        // - Configurable Data Store choice
        // - Configurable Geometry Types
        // - Redline import
        // - Redline export
        // - Configurable default style template

        public MgRedlineComponent()
        {
            this.Icon = Properties.Resources.redline;
            this.Label = this.ToolTipText = Strings.TitleRedline;
        }
        protected override MgControlView CreateControlView()
        {
            return new MgRedlineControlImpl(this.Viewer);
        }
    }
}
