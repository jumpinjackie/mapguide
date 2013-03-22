using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgThemeComponent : MgViewerComponent
    {
        public MgThemeComponent()
        {
            this.Icon = Properties.Resources.lc_theme;
            this.Label = this.ToolTipText = Strings.TitleTheme;
        }

        protected override MgControlView CreateControlView()
        {
            return new MgThemeControlImpl(this.Viewer);
        }
    }
}
