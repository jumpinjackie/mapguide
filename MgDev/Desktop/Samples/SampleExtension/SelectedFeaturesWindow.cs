using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;

namespace SampleExtension
{
    public partial class SelectedFeaturesWindow : Form
    {
        private MgSelectionSet _set;

        public SelectedFeaturesWindow(MgSelectionSet set)
        {
            InitializeComponent();
            _set = set;
            cmbLayer.DataSource = _set.LayerNames;
            cmbLayer.SelectedIndex = 0;
        }

        private void cmbLayer_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cmbLayer.SelectedItem != null)
            {
                string layerName = cmbLayer.SelectedItem.ToString();
                bool bSetHeader = false;
                dataGridView1.Rows.Clear();
                foreach (var feat in _set.GetFeaturesForLayer(layerName))
                {
                    if (!bSetHeader)
                    {
                        dataGridView1.Columns.Clear();
                        foreach (var name in feat.Properties.Keys)
                        {
                            dataGridView1.Columns.Add(name.ToString(), name.ToString());
                        }
                        bSetHeader = true;
                    }
                    object[] values = new object[feat.Properties.Keys.Count];
                    int i = 0;
                    foreach (var name in feat.Properties.Keys)
                    {
                        values[i] = feat.Properties[name];
                        i++;
                    }
                    dataGridView1.Rows.Add(values);
                }
            }
        }
    }
}
