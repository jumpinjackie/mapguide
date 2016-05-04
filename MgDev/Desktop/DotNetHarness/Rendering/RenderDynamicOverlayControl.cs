using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;
using System.Diagnostics;

namespace DotNetHarness.Rendering
{
    public partial class RenderDynamicOverlayControl : UserControl
    {
        public RenderDynamicOverlayControl()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgdRenderingService renSvc = (MgdRenderingService)fact.CreateService(MgServiceType.RenderingService);
                MgResourceIdentifier resId = new MgResourceIdentifier(textBox1.Text);

                var sw = new Stopwatch();
                sw.Start();
                MgdMap map = new MgdMap(resId);
                sw.Stop();
                Trace.TraceInformation("Runtime map created in {0}ms", sw.ElapsedMilliseconds);
                MgdSelection selection = null;

                if (!string.IsNullOrEmpty(txtSelectLayer.Text) &&
                    !string.IsNullOrEmpty(txtSelectFilter.Text))
                {
                    selection = new MgdSelection(map);

                    var layers = map.GetLayers();
                    if (layers.IndexOf(txtSelectLayer.Text) >= 0)
                    {
                        var layer = layers.GetItem(txtSelectLayer.Text);

                        var query = new MgFeatureQueryOptions();
                        query.SetFilter(txtSelectFilter.Text);

                        var reader = layer.SelectFeatures(query);

                        selection.AddFeatures(layer, reader, 0);

                        Trace.TraceInformation("{0} features in selection set", selection.GetSelectedFeaturesCount(layer, layer.GetFeatureClassName()));
                    }
                }

                double x;
                double y;
                if (double.TryParse(textBox2.Text, out x) &&
                    double.TryParse(textBox3.Text, out y))
                {
                    map.SetViewCenterXY(x, y);
                }

                int w;
                int h;
                if (int.TryParse(txtWidth.Text, out w) &&
                    int.TryParse(txtHeight.Text, out h))
                {
                    map.SetDisplaySize(w, h);
                }

                double scale;
                if (double.TryParse(textBox4.Text, out scale))
                {
                    map.SetViewScale(scale);
                }
                int mode;
                if (int.TryParse(textBox6.Text, out mode))
                { 
                    
                }

                MgColor selColor = new MgColor("0x0000FFFF");
                MgdRenderingOptions renderOpts = new MgdRenderingOptions(textBox5.Text, mode, selColor);

                sw.Start();
                MgByteReader response = renSvc.RenderDynamicOverlay(map, selection, renderOpts);
                sw.Stop();
                Trace.TraceInformation("RenderDynamicOverlay executed in {0}ms", sw.ElapsedMilliseconds);

                new ImageResponseDialog(response).ShowDialog();
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
