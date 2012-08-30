using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide.Viewer;
using System.Windows.Forms;

namespace SampleExtension
{
    public class MgStartupComponent : MgComponent
    {
        public override void Invoke()
        {
            MessageBox.Show(Layers.StartupText, "mg-desktop API Samples");
        }
    }
}
