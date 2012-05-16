using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide.Viewer;
using System.Windows.Forms;
using OSGeo.MapGuide;

namespace SampleExtension
{
    public class MgPlotToDwfComponent : MgComponent
    {
        public MgPlotToDwfComponent()
        {
            this.Label = "Plot to DWF";
        }

        public override void Invoke()
        {
            var diag = new ResourceIdDialog();
            if (diag.ShowDialog() == DialogResult.OK)
            {
                var layoutId = diag.ResourceID;
                using (var save = new SaveFileDialog())
                {
                    save.Filter = "DWF Files (*.dwf)|*.dwf";
                    if (save.ShowDialog() == DialogResult.OK)
                    {
                        var renderSvc = (MgRenderingService)this.Viewer.GetProvider().CreateService(MgServiceType.RenderingService);
                        var map = (MgdMap)this.Viewer.GetMap();
                        var dwfVer = new MgDwfVersion("6.01", "1.2");

                        var layout = new MgLayout(layoutId, "TestPlot", MgPageUnitsType.Inches);
                        var plotSpec = new MgPlotSpecification(8.5f, 11.0f, MgPageUnitsType.Inches, 0.5f, 0.5f, 0.5f, 0.5f);

                        var result = renderSvc.GeneratePlot(map, plotSpec, layout, dwfVer);
                        var sink = new MgByteSink(result);
                        sink.ToFile(save.FileName);

                        MessageBox.Show("Saved to " + save.FileName);
                    }
                }
            }
        }
    }
}
