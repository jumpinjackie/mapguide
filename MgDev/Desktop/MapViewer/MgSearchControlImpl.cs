using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    public partial class MgSearchControlImpl : MgControlView
    {
        private MgSearchControlImpl()
        {
            InitializeComponent();
        }

        private IMapViewer _viewer;
        private string _filter;
        private MgLayerBase _layer;

        public MgSearchControlImpl(IMapViewer viewer, string title, string layerName, string prompt, string filter)
            : this()
        {
            _viewer = viewer;
            this.Title = title;
            lblPrompt.Text = prompt;
            _filter = filter;

            if (!_filter.Contains("$USER_VARIABLE")) //NOXLATE
                throw new InvalidOperationException(Strings.ErrorSearchFilterMissingPlaceholder);
            
            var map = viewer.GetMap();
            var layers = map.GetLayers();
            var lidx = layers.IndexOf(layerName);
            if (lidx < 0)
                throw new InvalidOperationException(string.Format(Strings.ErrorInvalidSearchLayer, layerName));

            _layer = layers.GetItem(lidx);
        }

        private void btnSearch_Click(object sender, EventArgs e)
        {
            var query = new MgFeatureQueryOptions();
            query.SetFilter(_filter.Replace("$USER_VARIABLE", txtSearch.Text)); //NOXLATE

            var reader = _layer.SelectFeatures(query);
            new MgQueryResultsDialog(_viewer, _layer, reader, -1).Show();
        }
    }
}
