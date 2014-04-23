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
    public partial class RenderTileControl : UserControl
    {
        public RenderTileControl()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgdRenderingService renSvc = (MgdRenderingService)fact.CreateService(MgServiceType.RenderingService);

                MgResourceIdentifier mdfId = new MgResourceIdentifier(txtMapDefinition.Text);
                var sw = new Stopwatch();
                sw.Start();
                MgdMap map = new MgdMap(mdfId);
                sw.Stop();
                Trace.TraceInformation("Runtime map created in {0}ms", sw.ElapsedMilliseconds);

                map.SetViewScale(Convert.ToDouble(txtScale.Text));
                sw.Reset();
                sw.Start();
                MgByteReader response = renSvc.RenderTile(map, txtBaseGroup.Text, Convert.ToInt32(txtCol.Text), Convert.ToInt32(txtRow.Text));
                sw.Stop();
                Trace.TraceInformation("RenderTile executed in {0}ms", sw.ElapsedMilliseconds);
                new ImageResponseDialog(response).ShowDialog();
            }
            catch (MgException ex)
            {
                MessageBox.Show(ex.ToString(), "Error from MapGuide");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "Error");
            }
        }
    }
}
