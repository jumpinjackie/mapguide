using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide.Viewer;
using OSGeo.MapGuide;

namespace SampleExtension
{
    public partial class HelloMap : MgControlView
    {
        private IMapViewer _viewer;

        public HelloMap(IMapViewer viewer)
        {
            InitializeComponent();
            this.Title = "Hello Map";
            _viewer = viewer;
        }

        private void btnDisplaySpatialReference_Click(object sender, EventArgs e)
        {
            MgMapBase map = _viewer.GetMap();
            MgResourceIdentifier resId = map.MapDefinition;
            string mapSrs = map.GetMapSRS();

            MessageBox.Show("Map: (" + resId.ToString() + ") uses this reference system: " + Environment.NewLine + Environment.NewLine + mapSrs);
        }

        private void btnListLayers_Click(object sender, EventArgs e)
        {
            MgMapBase map = _viewer.GetMap();
            MgResourceIdentifier resId = map.MapDefinition;
            MgLayerCollection layers = map.GetLayers();

            List<string> results = new List<string>();
            for (int i = 0; i < layers.GetCount(); i++)
            {
                MgLayerBase layer = layers.GetItem(i);
                results.Add(layer.Name + " (" + layer.LegendLabel + ")");
            }

            MessageBox.Show("Map (" + resId.ToString() + ") has " + layers.GetCount() + " layers: " + Environment.NewLine + Environment.NewLine + string.Join(Environment.NewLine, results.ToArray()));
        }

        private void btnListLayerGroups_Click(object sender, EventArgs e)
        {
            MgMapBase map = _viewer.GetMap();
            MgResourceIdentifier resId = map.MapDefinition;
            MgLayerGroupCollection groups = map.GetLayerGroups();

            List<string> results = new List<string>();
            for (int i = 0; i < groups.GetCount(); i++)
            {
                MgLayerGroup group = groups.GetItem(i);
                results.Add(group.Name + " (" + group.LegendLabel + ")");
            }

            MessageBox.Show("Map (" + resId.ToString() + ") has " + groups.GetCount() + " layers: " + Environment.NewLine + Environment.NewLine + string.Join(Environment.NewLine, results.ToArray()));
        }
    }
}
