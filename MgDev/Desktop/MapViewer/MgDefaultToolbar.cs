using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;
using OSGeo.MapGuide;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A toolbar that contains a default set of viewer commands
    /// </summary>
    [ToolboxItem(true)]
    public class MgDefaultToolbar : ToolStrip, IDefaultToolbar
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="MgDefaultToolbar"/> class.
        /// </summary>
        public MgDefaultToolbar()
            : base()
        {
            this.GripStyle = ToolStripGripStyle.Visible;
            this.ZoomIn = new ToolStripButton(string.Empty, Properties.Resources.zoom_in_fixed, OnZoomIn)
            {
                ToolTipText = Strings.TextZoomIn,
                DisplayStyle = ToolStripItemDisplayStyle.Image
            };
            this.ZoomOut = new ToolStripButton(string.Empty, Properties.Resources.zoom_out_fixed, OnZoomOut)
            {
                ToolTipText = Strings.TextZoomOut,
                DisplayStyle = ToolStripItemDisplayStyle.Image
            };
            this.ZoomExtents = new ToolStripButton(string.Empty, Properties.Resources.zoom_full, OnZoomExtents)
            {
                ToolTipText = Strings.TextZoomExtents,
                DisplayStyle = ToolStripItemDisplayStyle.Image
            };
            this.SelectTool = new ToolStripButton(string.Empty, Properties.Resources.select_features, OnSelect)
            {
                ToolTipText = Strings.TextSelect,
                DisplayStyle = ToolStripItemDisplayStyle.Image
            };
            this.Pan = new ToolStripButton(string.Empty, Properties.Resources.icon_pan, OnPan)
            {
                ToolTipText = Strings.TextPan,
                DisplayStyle = ToolStripItemDisplayStyle.Image
            };
            this.ClearSelection = new ToolStripButton(string.Empty, Properties.Resources.select_clear, OnClearSelection)
            {
                ToolTipText = Strings.TextClearSelection,
                DisplayStyle = ToolStripItemDisplayStyle.Image
            };
            this.RefreshMap = new ToolStripButton(string.Empty, Properties.Resources.view_refresh, OnRefreshMap)
            {
                ToolTipText = Strings.TextRefreshMap,
                DisplayStyle = ToolStripItemDisplayStyle.Image
            };
            this.ToggleTooltips = new ToolStripButton(Strings.TextDisableTooltips, Properties.Resources.ui_tooltip_balloon_bottom, OnToggleTooltips)
            {
                ToolTipText = Strings.TextDisableTooltips,
                DisplayStyle = ToolStripItemDisplayStyle.ImageAndText
            };
            this.Loading = new ToolStripButton(string.Empty, Properties.Resources.icon_loading)
            {
                Alignment = ToolStripItemAlignment.Right,
                ImageScaling = ToolStripItemImageScaling.None,
                Visible = false
            };
            this.SelectPolygon = new ToolStripButton(string.Empty, Properties.Resources.select_polygon, OnSelectPolygon)
            {
                ToolTipText = Strings.TextSelectPolygon,
                DisplayStyle = ToolStripItemDisplayStyle.Image
            };
            this.SelectRadius = new ToolStripButton(string.Empty, Properties.Resources.select_radius, OnSelectRadius)
            {
                ToolTipText = Strings.TextSelectRadius,
                DisplayStyle = ToolStripItemDisplayStyle.Image
            };
            this.CopyMap = new ToolStripButton(string.Empty, Properties.Resources.edit_copy, OnCopyMap)
            {
                ToolTipText = Strings.TextCopyMap,
                DisplayStyle = ToolStripItemDisplayStyle.Image
            };

            this.Items.AddRange(new ToolStripItem[]
            {
                CopyMap,
                new ToolStripSeparator(),
                ZoomIn,
                ZoomOut,
                ZoomExtents,
                new ToolStripSeparator(),
                SelectTool,
                SelectRadius,
                SelectPolygon,
                Pan,
                new ToolStripSeparator(),
                ClearSelection,
                RefreshMap,
                new ToolStripSeparator(),
                ToggleTooltips,
                Loading
            });

            this.Disposed += new EventHandler(OnDisposed);
        }

        /// <summary>
        /// Gets or sets the zoom out mode.
        /// </summary>
        /// <value>
        /// The zoom out mode.
        /// </value>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The behaviour of the zoom out command")] //NOXLATE
        [DefaultValue(ZoomOutMode.ClickToZoom)]
        public ZoomOutMode ZoomOutMode
        {
            get;
            set;
        }

        private MgWktReaderWriter _wktRw;
        private MgGeometryFactory _geomFact;

        void OnDisposed(object sender, EventArgs e)
        {
            if (_wktRw != null)
            {
                _wktRw.Dispose();
                _wktRw = null;
            }

            if (_geomFact != null)
            {
                _geomFact.Dispose();
                _geomFact = null;
            }
        }

        private IMapViewer _viewer;

        /// <summary>
        /// Gets or sets the viewer this toolbar is associated with
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The map viewer component this toolbar will control")] //NOXLATE
        public IMapViewer Viewer
        {
            get { return _viewer; }
            set
            {
                if (_viewer != null)
                {
                    _viewer.PropertyChanged -= OnViewerPropertyChanged;
                    _viewer = null;
                }

                _viewer = value;
                if (_viewer != null)
                    _viewer.PropertyChanged += OnViewerPropertyChanged;

                TooltipStateChanged();
                UpdateButtonCheckedState();

                if (!this.DesignMode)
                {
                    if (_wktRw == null)
                        _wktRw = new MgWktReaderWriter();

                    if (_geomFact == null)
                        _geomFact = new MgGeometryFactory();
                }
            }
        }

        

        internal ToolStripButton Loading { get; private set; }

        internal ToolStripButton ZoomIn { get; private set; }

        void OnZoomIn(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            _viewer.ActiveTool = MapActiveTool.ZoomIn;
        }

        internal ToolStripButton ZoomOut { get; private set; }

        void OnZoomOut(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            if (this.ZoomOutMode == ZoomOutMode.ClickToZoom)
            {
                _viewer.ActiveTool = MapActiveTool.ZoomOut;
            }
            else
            {
                var map = _viewer.GetMap();
                var centerPt = map.ViewCenter;
                var centerCoord = centerPt.Coordinate;

                _viewer.ZoomToView(centerCoord.X, centerCoord.Y, map.ViewScale * _viewer.ZoomOutFactor, true);
            }
        }

        internal ToolStripButton ZoomExtents { get; private set; }

        void OnZoomExtents(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            _viewer.InitialMapView();
        }

        internal ToolStripButton SelectTool { get; private set; }

        void OnSelect(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            _viewer.ActiveTool = MapActiveTool.Select;
        }

        internal ToolStripButton Pan { get; private set; }

        void OnPan(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            _viewer.ActiveTool = MapActiveTool.Pan;
        }

        internal ToolStripButton ClearSelection { get; private set; }

        void OnClearSelection(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            _viewer.ClearSelection();
        }

        internal ToolStripButton SelectRadius { get; private set; }

        void OnSelectRadius(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            _viewer.DigitizeCircle((x, y, r) =>
            {
                MgGeometry geom = _wktRw.Read(Util.MakeWktCircle(x, y, r, true));
                _viewer.SelectByGeometry(geom);
            });
        }

        internal ToolStripButton SelectPolygon { get; private set; }

        void OnSelectPolygon(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            _viewer.DigitizePolygon((coordinates) => {
                MgCoordinateCollection coords = new MgCoordinateCollection();
                for (int i = 0; i < coordinates.GetLength(0); i++)
                {
                    coords.Add(_geomFact.CreateCoordinateXY(coordinates[i, 0], coordinates[i, 1]));
                }
                coords.Add(_geomFact.CreateCoordinateXY(coordinates[0, 0], coordinates[0, 1]));
                MgLinearRing ring = _geomFact.CreateLinearRing(coords);
                MgGeometry poly = _geomFact.CreatePolygon(ring, null);
                _viewer.SelectByGeometry(poly);
            });
        }

        internal ToolStripButton RefreshMap { get; private set; }

        void OnRefreshMap(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            _viewer.RefreshMap();
        }

        internal ToolStripButton ToggleTooltips { get; private set; }

        void OnToggleTooltips(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            _viewer.FeatureTooltipsEnabled = !_viewer.FeatureTooltipsEnabled;
            TooltipStateChanged();
        }

        internal ToolStripButton CopyMap { get; private set; }

        void OnCopyMap(object sender, EventArgs e)
        {
            if (_viewer == null)
                return;

            _viewer.CopyMap();
        }

        private void TooltipStateChanged()
        {
            if (_viewer == null)
                return;

            if (_viewer.FeatureTooltipsEnabled)
            {
                this.ToggleTooltips.Text = Strings.TextDisableTooltips;
                this.ToggleTooltips.ToolTipText = Strings.TooltipDisableTooltips;
            }
            else
            {
                this.ToggleTooltips.Text = Strings.TextEnableTooltips;
                this.ToggleTooltips.ToolTipText = Strings.TooltipEnableTooltips;
            }
        }

        private void UpdateButtonCheckedState()
        {
            var at = (_viewer == null) ? MapActiveTool.None : _viewer.ActiveTool;
            this.Pan.Checked = (at == MapActiveTool.Pan);
            this.SelectTool.Checked = (at == MapActiveTool.Select);
            this.ZoomIn.Checked = (at == MapActiveTool.ZoomIn);
            this.ZoomOut.Checked = (at == MapActiveTool.ZoomOut);
        }

        private void OnViewerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsBusy") //NOXLATE
            {
                var busy = _viewer.IsBusy;
                this.ZoomExtents.Enabled = this.ZoomIn.Enabled
                                         = this.ZoomOut.Enabled
                                         = this.ClearSelection.Enabled
                                         = this.Pan.Enabled
                                         = this.SelectTool.Enabled
                                         = this.SelectPolygon.Enabled
                                         = this.SelectRadius.Enabled
                                         = this.ToggleTooltips.Enabled
                                         = this.CopyMap.Enabled
                                         = this.RefreshMap.Enabled = !busy;
                this.Loading.Visible = busy;
            }
            else if (e.PropertyName == "ActiveTool" || e.PropertyName == "DigitizingType") //NOXLATE
            {
                UpdateButtonCheckedState();
            }
        }
    }

    /// <summary>
    /// Determines the behaviour of the zoom out command in the <see cref="MgDefaultToolbar"/>
    /// </summary>
    public enum ZoomOutMode
    {
        /// <summary>
        /// The user must click on the map to zoom out from that selected point
        /// </summary>
        ClickToZoom,
        /// <summary>
        /// The map automatically zooms out on command invocation
        /// </summary>
        AutoZoom
    }
}
