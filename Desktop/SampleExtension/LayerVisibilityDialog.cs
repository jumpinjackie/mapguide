using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;

namespace SampleExtension
{
    public partial class LayerVisibilityDialog : Form
    {
        public LayerVisibilityDialog(MgLayerCollection layers)
        {
            InitializeComponent();
            for (int i = 0; i < layers.GetCount(); i++)
            {
                MgLayerBase layer = layers.GetItem(i);
                grdLayers.Rows.Add(layer.LegendLabel, layer.GetVisible(), layer.IsVisible());
            }
        }
    }
}
