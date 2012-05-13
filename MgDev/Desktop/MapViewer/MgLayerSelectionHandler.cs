using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer
{
    public delegate void MgLayerSelectionEventHandler(string layerName, MgFeatureReader selectedFeatures);

    [DefaultEvent("SelectionMade")]
    [ToolboxItem(true)]
    public class MgLayerSelectionHandler : Component
    {
        [Category("MapGuide Viewer")]
        [Description("The map layers that this component will listen for selections")]
        public string[] Layers { get; set; }

        private IMapViewer _viewer;

        [Category("MapGuide Viewer")]
        [Description("The map viewer instance")]
        public IMapViewer Viewer
        {
            get { return _viewer; }
            set 
            {
                if (_viewer != null)
                    _viewer.SelectionChanged -= OnViewerSelectionChanged;
                _viewer = value; 
                if (_viewer != null)
                    _viewer.SelectionChanged += OnViewerSelectionChanged;
            }
        }

        void OnViewerSelectionChanged(object sender, EventArgs e)
        {
            if (this.Layers == null || this.Layers.Length == 0)
                return;

            var sel = _viewer.GetSelection();
            var selLayers = sel.GetLayers();
            if (selLayers != null)
            {
                var layers = new Dictionary<string, MgLayerBase>();
                for (int i = 0; i < selLayers.Count; i++)
                {
                    var layerItem = selLayers.GetItem(i);
                    layers.Add(layerItem.Name, layerItem);
                }
                foreach (var layerName in this.Layers)
                {
                    if (layers.ContainsKey(layerName))
                    {
                        var h = this.SelectionMade;
                        if (h != null)
                        {
                            var reader = sel.GetSelectedFeatures(layers[layerName], layers[layerName].FeatureClassName, false);
                            h(layerName, reader);
                        }
                    }
                }
            }
        }

        [Category("MapGuide Viewer")]
        [Description("Raised when a selection on any of the specified layers has been made")]
        public event MgLayerSelectionEventHandler SelectionMade;
    }
}
