using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide;
using OSGeo.MapGuide.Viewer.AppLayoutEngine;

namespace SampleExtension
{
    public partial class InteractingWithLayers : MgControlView
    {
        private IMapViewer _viewer;

        public InteractingWithLayers(IMapViewer viewer)
        {
            InitializeComponent();
            _viewer = viewer;
            this.Title = "Interacting With Layers";

            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            if (layers.IndexOf("Roads") >= 0 && layers.IndexOf("Streets") < 0)
            {
                btnRenameRoadsLayer.Text = "Rename Roads Layer to Streets";
            }
            else if (layers.IndexOf("Roads") < 0 && layers.IndexOf("Streets") >= 0)
            {
                btnRenameRoadsLayer.Text = "Rename Streets Layer to Roads";
            }
        }

        private void btnLayerVisibility_Click(object sender, EventArgs e)
        {
            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();
            new LayerVisibilityDialog(layers).ShowDialog();
        }

        private void btnRenameRoadsLayer_Click(object sender, EventArgs e)
        {
            MgMapBase map = _viewer.GetMap();
            MgLayerCollection layers = map.GetLayers();

            bool bRefresh = false;
            if (layers.IndexOf("Roads") >= 0)
            {
                MgLayerBase roads = layers.GetItem("Roads");
                roads.Name = "Streets";
                roads.LegendLabel = "Streets";

                MessageBox.Show("Layer (Roads) has been changed to (Streets)");
                btnRenameRoadsLayer.Text = "Rename Streets Layer to Roads";
                bRefresh = true;
            }
            else if (layers.IndexOf("Streets") >= 0)
            {
                MgLayerBase streets = layers.GetItem("Streets");
                streets.Name = "Roads";
                streets.LegendLabel = "Roads";

                MessageBox.Show("Layer (Streets) has been changed to (Roads)");
                btnRenameRoadsLayer.Text = "Rename Roads Layer to Streets";
                bRefresh = true;
            }

            if (bRefresh)
            {
                _viewer.RefreshMap();
                IMapLegend legend = Shell.Instance.Legend;
                if (legend != null)
                    legend.RefreshLegend();
            }
        }
    }
}
