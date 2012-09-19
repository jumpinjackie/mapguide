using System;
using System.Collections.Generic;
using System.Text;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// Manages the main interactions between common map viewer components
    /// </summary>
    public class MapViewerController
    {
        private readonly IMapViewer _viewer;
        private readonly IMapLegend _legend;
        private readonly IMapStatusBar _statBar;
        private readonly IPropertyPane _propPane;

        /// <summary>
        /// Initializes a new instance of the <see cref="MapViewerController"/> class.
        /// </summary>
        /// <param name="viewer">The viewer.</param>
        /// <param name="legend">The legend.</param>
        /// <param name="statBar">The stat bar.</param>
        /// <param name="propPane">The prop pane.</param>
        public MapViewerController(IMapViewer viewer, IMapLegend legend, IMapStatusBar statBar, IPropertyPane propPane)
        {
            if (viewer == null)
                throw new ArgumentNullException("viewer"); //NOXLATE

            _viewer = viewer;
            _legend = legend;
            _statBar = statBar;
            _propPane = propPane;

            WireEvents();
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="MapViewerController"/> class.
        /// </summary>
        /// <param name="viewer">The viewer.</param>
        /// <param name="legend">The legend.</param>
        /// <param name="statBar">The stat bar.</param>
        /// <param name="propPane">The prop pane.</param>
        /// <param name="toolbar">The toolbar.</param>
        public MapViewerController(IMapViewer viewer, IMapLegend legend, IMapStatusBar statBar, IPropertyPane propPane, IDefaultToolbar toolbar)
            : this(viewer, legend, statBar, propPane)
        {
            if (toolbar == null)
                throw new ArgumentNullException("toolbar");

            toolbar.Viewer = viewer;
        }

        private void WireEvents()
        {
            if (_propPane != null)
            {
                _propPane.RequestZoomToCurrentFeature += new EventHandler(OnRequestZoomToCurrentFeature);
            }
            _viewer.MapRefreshed += new EventHandler(OnMapRefreshed);
            _viewer.MapScaleChanged += new EventHandler(OnMapScaleChanged);
            _viewer.SelectionChanged += new EventHandler(OnMapSelectionChanged);
            _viewer.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(OnMapPropertyChanged);
            _viewer.MapLoaded += new EventHandler(OnViewerMapLoaded);
            _viewer.MouseMapPositionChanged += new EventHandler<MapPointEventArgs>(OnMouseMapPositionChanged);
            if (_legend != null)
            {
                _legend.VisibilityChanged += new EventHandler(OnLegendVisibilityChanged);
            }
        }

        void OnRequestZoomToCurrentFeature(object sender, EventArgs e)
        {
            var feat = _propPane.CurrentFeature;
            if (feat.Geometry != null)
            {
                var env = feat.Geometry.Envelope();
                _viewer.ZoomToExtents(env.LowerLeftCoordinate.X, env.LowerLeftCoordinate.Y, env.UpperRightCoordinate.X, env.UpperRightCoordinate.Y);
            }
        }

        void OnMouseMapPositionChanged(object sender, MapPointEventArgs e)
        {
            if (_viewer.CoordinateSystem == null)
                return;

            if (_statBar != null)
                _statBar.SetCursorPositionMessage(string.Format("X: {0:0.0000000}, Y: {1:0.0000000} ({2})", e.X, e.Y, _viewer.CoordinateSystem.Units)); //NOXLATE
        }

        void OnLegendVisibilityChanged(object sender, EventArgs e)
        {
            _viewer.RefreshMap();
        }

        void OnViewerMapLoaded(object sender, EventArgs e)
        {
            if (_legend != null)
            {
                ((MgLegend)_legend).Init(_viewer.GetProvider());
            }
        }

        void OnMapPropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {

        }

        void OnMapSelectionChanged(object sender, EventArgs e)
        {
            if (_propPane != null || _statBar != null)
            {
                var sel = _viewer.GetSelection();
                var sset = new MgSelectionSet(sel, _viewer.GetProvider());
                if (_propPane != null)
                    _propPane.Init(sset);
                if (_statBar != null)
                    _statBar.SetFeatureSelectedMessage(string.Format(Strings.StatFeaturesSelected, sset.TotalCount));
            }
        }

        void OnMapScaleChanged(object sender, EventArgs e)
        {
            var map = _viewer.GetMap();
            double scale = map.ViewScale;

            if (_legend != null)
                _legend.SetScale(scale);

            if (_statBar != null)
                _statBar.SetMapScaleMessage(string.Format("1:{0:0.00000}", scale)); //NOXLATE
        }

        void OnMapRefreshed(object sender, EventArgs e)
        {
            if (_statBar != null)
            {
                var map = _viewer.GetMap();
                _statBar.SetMapScaleMessage(string.Format("1:{0:0.00000}", map.ViewScale)); //NOXLATE0
                var ext = map.MapExtent;
                _statBar.SetMapSizeMessage(string.Format("{0:0.000} x {1:0.000} ({2})", ext.Width, ext.Height, _viewer.CoordinateSystem.Units)); //NOXLATE
            }
        }
    }
}
