using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer.AppLayoutEngine
{
    [ToolboxItem(true)]
    public class MgQuitComponent : MgComponent
    {
        public MgQuitComponent()
        {
            this.Label = Properties.Resources.TextExitApplication;
        }

        public override void Invoke()
        {
            System.Windows.Forms.Application.Exit();
        }
    }
}
