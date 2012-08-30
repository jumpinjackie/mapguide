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

namespace DotNetHarness.Tile
{
    public partial class GetTileControl : UserControl
    {
        public GetTileControl()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                var fact = new MgdServiceFactory();
                MgdTileService renSvc = (MgdTileService)fact.CreateService(MgServiceType.TileService);

                MgResourceIdentifier mdfId = new MgResourceIdentifier(txtMapDefinition.Text);
                if (rdRuntimeMap.Checked)
                {
                    var sw = new Stopwatch();
                    sw.Start();
                    MgdMap map = new MgdMap(mdfId);
                    sw.Stop();
                    Trace.TraceInformation("Runtime map created in {0}ms", sw.ElapsedMilliseconds);

                    if (rdScaleIndex.Checked)
                    {
                        double scale = map.GetFiniteDisplayScaleAt(Convert.ToInt32(txtScaleIndex.Text));
                        map.SetViewScale(scale);
                    }
                    else
                    {
                        map.SetViewScale(Convert.ToDouble(txtScale.Text));
                    }
                    sw.Reset();
                    sw.Start();
                    MgByteReader response = renSvc.GetTile(map,
                                                           txtBaseGroup.Text,
                                                           Convert.ToInt32(txtCol.Text),
                                                           Convert.ToInt32(txtRow.Text));
                    sw.Stop();
                    Trace.TraceInformation("GetTile executed in {0}ms", sw.ElapsedMilliseconds);
                    new ImageResponseDialog(response).ShowDialog();
                }
                else if (rdMapDefinition.Checked)
                {
                    var sw = new Stopwatch();
                    sw.Start();
                    MgByteReader response = renSvc.GetTile(mdfId,
                                                           txtBaseGroup.Text,
                                                           Convert.ToInt32(txtCol.Text),
                                                           Convert.ToInt32(txtRow.Text),
                                                           Convert.ToInt32(txtScaleIndex.Text));
                    sw.Stop();
                    Trace.TraceInformation("GetTile executed in {0}ms", sw.ElapsedMilliseconds);
                    new ImageResponseDialog(response).ShowDialog();
                }
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
