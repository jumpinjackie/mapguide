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
            MessageBox.Show("If you see this message. It is because I was automatically invoked by the AppLayout's InvokeOnStartup property", "SampleExtension: MgStartupComponent");
        }
    }
}
