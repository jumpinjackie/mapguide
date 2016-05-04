using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide;

namespace SampleExtension
{
    public class MgProfileComponent : MgComponent
    {
        public MgProfileComponent()
        {
            this.Label = "Profile";
        }

        public override void Invoke()
        {
            var provider = this.Viewer.GetProvider();
            var map = this.Viewer.GetMap();
            var prof = (MgdProfilingService)provider.CreateService(MgServiceType.ProfilingService);
            var opts = new MgdRenderingOptions("PNG", 2, new MgColor(this.Viewer.SelectionColor));
            var result = prof.ProfileRenderDynamicOverlay((MgdMap)map, (MgdSelection)this.Viewer.GetSelection(), opts);
            new XmlResponseDialog(result).ShowDialog(); 
        }
    }
}
