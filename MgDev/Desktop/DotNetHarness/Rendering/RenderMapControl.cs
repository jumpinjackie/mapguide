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
    public partial class RenderMapControl : UserControl
    {
        private MgGeometryFactory _geomFact;

        public RenderMapControl()
        {
            InitializeComponent();
            _geomFact = new MgGeometryFactory();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgdRenderingService renSvc = (MgdRenderingService)fact.CreateService(MgServiceType.RenderingService);
                MgResourceIdentifier resId = new MgResourceIdentifier(textBox1.Text);

                MgdMap map = new MgdMap(resId);

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

                var sw = new Stopwatch();
                sw.Start();
                MgByteReader response = renSvc.RenderMap(map, null, textBox5.Text);
                sw.Stop();
                Trace.TraceInformation("RenderMap executed in {0}ms", sw.ElapsedMilliseconds);

                new ImageResponseDialog(response).ShowDialog();
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
        }
    }
}
