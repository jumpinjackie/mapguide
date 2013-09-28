using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Windows.Forms;
using System.Globalization;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    [ToolboxItem(true)]
    public class MgCopyMapComponent : MgComponent
    {
        public MgCopyMapComponent()
        {
            this.Label = this.ToolTipText = Strings.TextCopyMap;
            this.Icon = Properties.Resources.edit_copy;
        }

        public override void Invoke()
        {
            this.Viewer.CopyMap();
        }
    }

    [ToolboxItem(true)]
    public class MgZoomInComponent : MgMapActiveToolComponent
    {
        public MgZoomInComponent()
        {
            this.Label = this.ToolTipText = Strings.TextZoomIn;
            this.Icon = Properties.Resources.zoom_in_fixed;
        }

        protected override MapActiveTool Tool
        {
            get
            {
                return MapActiveTool.ZoomIn;
            }
        }
    }

    [ToolboxItem(true)]
    public class MgZoomOutComponent : MgComponent //We derive from MgComponent instead of MgMapActiveToolComponent because we have special invocation logic
    {
        public MgZoomOutComponent()
        {
            this.Label = this.ToolTipText = Strings.TextZoomOut;
            this.Icon = Properties.Resources.zoom_out_fixed;
        }

        public ZoomOutMode ZoomOutMode
        {
            get;
            set;
        }

        protected override void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "ActiveTool") //NOXLATE
            {
                var state = (this.Viewer.ActiveTool == MapActiveTool.ZoomOut);
                foreach (var l in _listeners)
                    l.SetActive(state);
            }
            base.OnViewerPropertyChanged(sender, e);
        }

        public override void Invoke()
        {
            var viewer = this.Viewer;
            if (this.ZoomOutMode == ZoomOutMode.ClickToZoom)
            {
                viewer.ActiveTool = MapActiveTool.ZoomOut;
            }
            else
            {
                var map = viewer.GetMap();
                var centerPt = map.ViewCenter;
                var centerCoord = centerPt.Coordinate;

                viewer.ZoomToView(centerCoord.X, centerCoord.Y, map.ViewScale * viewer.ZoomOutFactor, true);
            }
        }
    }

    [ToolboxItem(true)]
    public class MgZoomNextComponent : MgComponent
    {
        public MgZoomNextComponent()
        {
            this.Label = this.ToolTipText = Strings.TextZoomNext;
            this.Icon = Properties.Resources.view_forward;
        }

        private void Evaluate()
        {
            var history = this.Viewer.ViewHistory;
            var enabled = (history.Count > 1 && this.Viewer.ViewHistoryIndex < history.Count - 1);
            foreach (var l in _listeners)
                l.SetEnabled(enabled);
        }

        protected override void OnMapRefreshed(object sender, EventArgs e)
        {
            Evaluate();
        }

        protected override void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "ViewHistoryIndex" || e.PropertyName == "ViewHistory") //NOXLATE
            {
                Evaluate();
            }
            base.OnViewerPropertyChanged(sender, e);
        }

        public override void Invoke()
        {
            this.Viewer.NextView();
        }
    }

    [ToolboxItem(true)]
    public class MgZoomPreviousComponent : MgComponent
    {
        public MgZoomPreviousComponent()
        {
            this.Label = this.ToolTipText = Strings.TextZoomPrevious;
            this.Icon = Properties.Resources.view_back;
        }

        private void Evaluate()
        {
            var history = this.Viewer.ViewHistory;
            var enabled = history.Count > 1;
            foreach (var l in _listeners)
                l.SetEnabled(enabled);
        }

        protected override void OnMapRefreshed(object sender, EventArgs e)
        {
            Evaluate();
        }

        protected override void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "ViewHistoryIndex" || e.PropertyName == "ViewHistory") //NOXLATE
            {
                Evaluate();
            }
            base.OnViewerPropertyChanged(sender, e);
        }

        public override void Invoke()
        {
            this.Viewer.PreviousView();
        }
    }

    [ToolboxItem(true)]
    public class MgZoomToSelectionComponent : MgComponent
    {
        private MgAgfReaderWriter _agfRw;

        public MgZoomToSelectionComponent()
        {
            this.Label = this.ToolTipText = Strings.TextZoomToSelection;
            this.Icon = Properties.Resources.icon_zoomselect;
            this.Disposed += new EventHandler(OnDisposed);
        }

        void OnDisposed(object sender, EventArgs e)
        {
            if (_agfRw != null)
                _agfRw.Dispose();

            _agfRw = null;
        }

        public override void Invoke()
        {
            var viewer = this.Viewer;
            var sel = viewer.GetSelection();
            var selLayers = sel.GetLayers();

            if (selLayers == null)
            {
                MessageBox.Show(Strings.TextNoSelection);
                return;
            }

            //This doesn't work :(
            //
            //var provider = viewer.GetProvider();
            //var featSvc = (MgFeatureService)provider.CreateService(MgServiceType.FeatureService);
            //var ext = sel.GetExtents(featSvc);
            //var ll = ext.LowerLeftCoordinate;
            //var ur = ext.UpperRightCoordinate;
            //viewer.ZoomToExtents(ll.X, ll.Y, ur.X, ur.Y);
            MgEnvelope env = new MgEnvelope();
            if (_agfRw == null)
                _agfRw = new MgAgfReaderWriter();
            for (var i = 0; i < selLayers.GetCount(); i++)
            {
                var layer = selLayers.GetItem(i);
                var fr = sel.GetSelectedFeatures(layer, layer.FeatureClassName, false);
                try
                {
                    string geomName = layer.GetFeatureGeometryName();
                    while (fr.ReadNext())
                    {
                        if (!fr.IsNull(geomName))
                        {
                            MgByteReader agf = null;
                            MgGeometry geom = null;
                            try 
                            {
                                agf = fr.GetGeometry(geomName);
                                geom = _agfRw.Read(agf);
                                env.ExpandToInclude(geom.Envelope());
                            } 
                            finally 
                            {
                                if (agf != null)
                                {
                                    agf.Dispose();
                                    agf = null;
                                }
                                if (geom != null)
                                {
                                    geom.Dispose();
                                    geom = null;
                                }
                            }
                        }
                    }
                }
                finally
                {
                    fr.Close();
                }
            }
            var ll = env.LowerLeftCoordinate;
            var ur = env.UpperRightCoordinate;
            viewer.ZoomToExtents(ll.X, ll.Y, ur.X, ur.Y);
        }
    }

    [ToolboxItem(true)]
    public class MgInitialViewComponent : MgComponent
    {
        public MgInitialViewComponent()
        {
            this.Label = this.ToolTipText = Strings.TextZoomExtents;
            this.Icon = Properties.Resources.zoom_full;
        }

        public override void Invoke()
        {
            this.Viewer.InitialMapView();
        }
    }

    [ToolboxItem(false)]
    public class MgMapActiveToolComponent : MgComponent
    {
        protected override void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            base.OnViewerPropertyChanged(sender, e);
            if (e.PropertyName == "ActiveTool") //NOXLATE
            {
                ViewerActiveToolChanged(this.Viewer.ActiveTool);
            }
        }

        protected virtual MapActiveTool Tool { get { throw new NotImplementedException(); } }

        public override void Invoke()
        {
            this.Viewer.ActiveTool = this.Tool;
        }

        private void ViewerActiveToolChanged(MapActiveTool activeTool) 
        {
            var state = (this.Tool == activeTool);
            foreach (var l in _listeners)
            {
                l.SetActive(state);
            }
        }
    }

    [ToolboxItem(true)]
    public class MgSelectComponent : MgMapActiveToolComponent
    {
        public MgSelectComponent()
        {
            this.Label = this.ToolTipText = Strings.TextSelect;
            this.Icon = Properties.Resources.select_features;
        }

        protected override MapActiveTool Tool
        {
            get
            {
                return MapActiveTool.Select;
            }
        }
    }

    [ToolboxItem(true)]
    public class MgCircleSelectComponent : MgComponent
    {
        private MgWktReaderWriter _wktRw; //We lazy init this to avoid complications with the WinForms designer

        public MgCircleSelectComponent()
        {
            this.Label = this.ToolTipText = Strings.TextSelectRadius;
            this.Icon = Properties.Resources.select_radius;
            this.Disposed += new EventHandler(OnDisposed);
        }

        void OnDisposed(object sender, EventArgs e)
        {
            if (_wktRw != null)
            {
                _wktRw.Dispose();
                _wktRw = null;
            }
        }

        

        public override void Invoke()
        {
            var viewer = this.Viewer;
            viewer.DigitizeCircle((x, y, r) =>
            {
                if (_wktRw == null)
                    _wktRw = new MgWktReaderWriter();

                MgGeometry geom = _wktRw.Read(Util.MakeWktCircle(x, y, r, true));
                viewer.SelectByGeometry(geom);
            });
        }
    }

    [ToolboxItem(true)]
    public class MgPolygonSelectComponent : MgComponent
    {
        private MgGeometryFactory _geomFact; //We lazy init this to avoid complications with the WinForms designer

        public MgPolygonSelectComponent()
        {
            this.Label = this.ToolTipText = Strings.TextSelectPolygon;
            this.Icon = Properties.Resources.select_polygon;
            this.Disposed += new EventHandler(OnDisposed);
        }

        void OnDisposed(object sender, EventArgs e)
        {
            if (_geomFact != null)
            {
                _geomFact.Dispose();
                _geomFact = null;
            }
        }

        public override void Invoke()
        {
            var viewer = this.Viewer;
            viewer.DigitizePolygon((coordinates) =>
            {
                if (_geomFact == null)
                    _geomFact = new MgGeometryFactory();

                MgCoordinateCollection coords = new MgCoordinateCollection();
                for (int i = 0; i < coordinates.GetLength(0); i++)
                {
                    coords.Add(_geomFact.CreateCoordinateXY(coordinates[i, 0], coordinates[i, 1]));
                }
                coords.Add(_geomFact.CreateCoordinateXY(coordinates[0, 0], coordinates[0, 1]));
                MgLinearRing ring = _geomFact.CreateLinearRing(coords);
                MgGeometry poly = _geomFact.CreatePolygon(ring, null);
                viewer.SelectByGeometry(poly);
            });
        }
    }

    [ToolboxItem(true)]
    public class MgPanComponent : MgMapActiveToolComponent
    {
        public MgPanComponent()
        {
            this.Label = this.ToolTipText = Strings.TextPan;
            this.Icon = Properties.Resources.icon_pan;
        }

        protected override MapActiveTool Tool
        {
            get
            {
                return MapActiveTool.Pan;
            }
        }
    }

    [ToolboxItem(true)]
    public class MgClearSelectionComponent : MgComponent
    {
        public MgClearSelectionComponent()
        {
            this.Label = this.ToolTipText = Strings.TextClearSelection;
            this.Icon = Properties.Resources.select_clear;
        }

        public override void Invoke()
        {
            this.Viewer.ClearSelection();
        }
    }

    [ToolboxItem(true)]
    public class MgRefreshMapComponent : MgComponent
    {
        public MgRefreshMapComponent()
        {
            this.Label = this.ToolTipText = Strings.TextRefreshMap;
            this.Icon = Properties.Resources.view_refresh;
        }

        public override void Invoke()
        {
            this.Viewer.RefreshMap();
        }
    }

    [ToolboxItem(true)]
    public class MgTooltipToggleComponent : MgComponent
    {
        public MgTooltipToggleComponent()
        {
            this.Label = this.ToolTipText = Strings.TextEnableTooltips;
            this.Icon = Properties.Resources.ui_tooltip_balloon_bottom;
        }

        public override void Invoke()
        {
            var viewer = this.Viewer;
            viewer.FeatureTooltipsEnabled = !viewer.FeatureTooltipsEnabled;

            if (viewer.FeatureTooltipsEnabled)
            {
                this.Label = this.ToolTipText = Strings.TextDisableTooltips;
                this.Icon = Properties.Resources.ui_tooltip_balloon_bottom;
            }
            else
            {
                this.Label = this.ToolTipText = Strings.TextEnableTooltips;
                this.Icon = Properties.Resources.ui_tooltip_balloon_bottom;
            }
            //Update subscribers
            foreach (var l in _listeners)
            {
                l.SetText(this.Label);
                l.SetIcon(this.Icon);
            }
        }
    }
}
