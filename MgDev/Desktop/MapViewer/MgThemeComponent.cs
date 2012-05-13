using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgThemeComponent : MgViewerComponent
    {
        public MgThemeComponent()
        {
            this.Icon = Properties.Resources.lc_theme;
            this.Label = this.ToolTipText = Properties.Resources.TitleTheme;
        }

        protected override MgControlImpl CreateControlImpl()
        {
            return new MgThemeControlImpl(this.Viewer);
        }
    }
}
