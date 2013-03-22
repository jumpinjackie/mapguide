using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// Defines a method for handling viewer selections
    /// </summary>
    /// <param name="selectedFeatures"></param>
    public delegate void MgLayerSelectionEventHandler(MgSelectionSet selectedFeatures);

    [DefaultEvent("SelectionMade")] //NOXLATE
    [ToolboxItem(true)]
    public class MgLayerSelectionHandler : Component
    {
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The map layers that this component will listen for selections")] //NOXLATE
        public string[] Layers { get; set; }

        private IMapViewer _viewer;

        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The map viewer instance")] //NOXLATE
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
                var selectionSet = new MgSelectionSet(sel, _viewer.GetProvider());
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
                            h(selectionSet);
                        }
                    }
                }
            }
        }

        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Raised when a selection on any of the specified layers has been made")] //NOXLATE
        public event MgLayerSelectionEventHandler SelectionMade;
    }
}
