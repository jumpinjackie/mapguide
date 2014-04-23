using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using OSGeo.MapGuide;
using System.Drawing;
using System.ComponentModel;
using System.IO;
using System.Drawing.Drawing2D;
using System.Diagnostics;
using System.Threading;
using System.Xml;
using System.Collections.Specialized;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Reflection;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A map viewer component for displaying/rendering a <see cref="T:OSGeo.MapGuide.MgMapBase"/> instance
    /// </summary>
    /// <remarks>
    /// <para>
    /// For any non-modal <see cref="T:System.Windows.Forms.Form"/> instances displayed over this control, if you call <see cref="T:System.Windows.Forms.Form.Show()"/> on the
    /// form without specifying the owner, this control will automatically be sent to the back of the draw order by this control. To avoid this behavior always call 
    /// <see cref="T:System.Windows.Forms.Form.Show(System.Windows.Forms.IWin32Window)"/>. You can choose to pass an actual owner or a null owner, but this particular overload
    /// must be used.
    /// </para>
    /// <para>
    /// If you are using <see cref="T:OSGeo.MapGuide.Viewer.MgViewerComponent"/> instances with <see cref="T:OSGeo.MapGuide.Viewer.MgViewerTarget"/> set to NewWindow, 
    /// be sure to set the <see cref="P:OSGeo.MapGuide.Viewer.MgViewerComponent.OwnerParent"/> property to your top-level form. This form will be automatically set as 
    /// the owner form for any new window displayed by the respective component. This property is optional. All forms displayed by this component will not exhibit the
    /// "sent to back of draw order" display problem.
    /// </para>
    /// <para>
    /// For applications using the AppLayout engine, such components do not need to have the OwnerParent set. As long as the Target is set to NewWindow, the AppLayout
    /// engine will automatically set the OwnerParent of all defined <see cref="T:OSGeo.MapGuide.Viewer.MgViewerComponent"/> instances to the main 
    /// AppLayout main window.
    /// </para>
    /// </remarks>
    [ToolboxItem(true)]
    public class MgMapViewer : Control, IMapViewer
    {
        private BackgroundWorker renderWorker;

        private MgResourceService _resSvc;
        private MgMapBase _map;
        private MgSelectionBase _selection;
        private MgMapViewerProvider _provider;
        private MgViewerRenderingOptions _overlayRenderOpts;
        private MgViewerRenderingOptions _selectionRenderOpts;
        private MgWktReaderWriter _wktRW;
        private MgAgfReaderWriter _agfRW;
        private MgGeometryFactory _geomFact;
        private MgCoordinateSystemFactory _csFact;
        private MgMeasure _mapMeasure;

        private Color _mapBgColor;

        private double _orgX1;
        private double _orgX2;
        private double _orgY1;
        private double _orgY2;

        private double _extX1;
        private double _extX2;
        private double _extY1;
        private double _extY2;

        private Image _selectionImage;
        private Image _mapImage;

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        internal Image Image
        {
            get { return _mapImage; }
            set
            {
                _mapImage = value;
                //Invalidate();
            }
        }

#if VIEWER_DEBUG
        private MgdLayer _debugLayer;

        private void CreateDebugFeatureSource()
        {
            var id = new MgDataPropertyDefinition("ID"); //NOXLATE
            id.DataType = MgPropertyType.Int32;
            id.Nullable = false;
            id.SetAutoGeneration(true);

            var geom = new MgGeometricPropertyDefinition("Geometry"); //NOXLATE
            geom.GeometryTypes = MgFeatureGeometricType.Point;
            geom.SpatialContextAssociation = "MapCs"; //NOXLATE

            var cls = new MgClassDefinition();
            cls.Name = "Debug"; //NOXLATE
            var props = cls.GetProperties();
            props.Add(id);
            props.Add(geom);

            var idProps = cls.GetIdentityProperties();
            idProps.Add(id);

            cls.DefaultGeometryPropertyName = "Geometry"; //NOXLATE

            var schema = new MgFeatureSchema("Default", "Default schema"); //NOXLATE
            var classes = schema.GetClasses();
            classes.Add(cls);

            //We can make anything up here, there's no real concept of sessions
            var sessionId = Guid.NewGuid().ToString();

            var debugFsId = new MgResourceIdentifier("Session:" + sessionId + "//Debug" + Guid.NewGuid().ToString() + ".FeatureSource"); //NOXLATE
            var createSdf = new MgCreateSdfParams("MapCs", _map.GetMapSRS(), schema); //NOXLATE
            var featureSvc = (MgdFeatureService)fact.CreateService(MgServiceType.FeatureService);
            var resSvc = (MgResourceService)fact.CreateService(MgServiceType.ResourceService);
            featureSvc.CreateFeatureSource(debugFsId, createSdf);

            byte[] bytes = Encoding.UTF8.GetBytes(string.Format(Debug.DebugLayer, debugFsId.ToString(), "Default:Debug", "Geometry")); //NOXLATE
            var source = new MgByteSource(bytes, bytes.Length);

            var debugLayerId = new MgResourceIdentifier("Session:" + sessionId + "//" + debugFsId.Name + ".LayerDefinition"); //NOXLATE
            var breader = source.GetReader();
            resSvc.SetResource(debugLayerId, breader, null);
            
            _debugLayer = new MgdLayer(debugLayerId, resSvc);
            _debugLayer.SetLegendLabel("Debug Layer"); //NOXLATE
            _debugLayer.SetVisible(true);
            _debugLayer.SetDisplayInLegend(true);

            var mapLayers = _map.GetLayers();
            mapLayers.Insert(0, _debugLayer);

            UpdateCenterDebugPoint();
        }

        private MgPropertyCollection _debugCenter;

        private void UpdateCenterDebugPoint()
        {
            if (_debugCenter == null)
                _debugCenter = new MgPropertyCollection();

            var center = _wktRW.Read("POINT (" + _map.ViewCenter.Coordinate.X.ToString(CultureInfo.InvariantCulture) + " " + _map.ViewCenter.Coordinate.Y.ToString(CultureInfo.InvariantCulture) + ")"); //NOXLATE
            var agf = _agfRW.Write(center);
            if (!_debugCenter.Contains("Geometry")) //NOXLATE
            {
                MgGeometryProperty geom = new MgGeometryProperty("Geometry", agf); //NOXLATE
                _debugCenter.Add(geom);
            }
            else
            {
                MgGeometryProperty geom = (MgGeometryProperty)_debugCenter.GetItem("Geometry"); //NOXLATE
                geom.SetValue(agf);
            }

            int deleted = _debugLayer.DeleteFeatures(string.Empty);
            Trace.TraceInformation("Deleted {0} debug points", deleted); //NOXLATE
            var reader = _debugLayer.InsertFeatures(_debugCenter);
            int inserted = 0;
            while (reader.ReadNext())
            {
                inserted++;
            }
            reader.Close();
            Trace.TraceInformation("Added {0} debug points", inserted); //NOXLATE
            _debugLayer.ForceRefresh();
        }
#endif

        private MgCoordinateSystem _mapCs;

        private int _viewHistoryIndex;
        private List<MgMapViewHistoryEntry> _viewHistory;

        /// <summary>
        /// Initializes a new instance of the <see cref="MgMapViewer"/> class.
        /// </summary>
        public MgMapViewer()
        {
            _viewHistory = new List<MgMapViewHistoryEntry>();
            _viewHistoryIndex = -1;
            this.ShowVertexCoordinatesWhenDigitizing = false;
            this.FeatureTooltipsEnabled = false;
            this.TooltipsEnabled = false;
            this.ZoomInFactor = 0.5;
            this.ZoomOutFactor = 2.0;
            this.SelectionColor = Color.Blue;
            this.PointPixelBuffer = 2;

            this.MinScale = 10;
            this.MaxScale = 1000000000;

            this.DigitizingFillTransparency = 100;
            this.DigitizingOutline = Brushes.Red;
            this.DigitzingFillColor = Color.White;
            this.TooltipFillColor = Color.LightYellow;
            this.TooltipFillTransparency = 200;
            this.MouseWheelDelayRenderInterval = 800;
            this.TooltipDelayInterval = 1000;

            this.ActiveTool = MapActiveTool.None;
            this.DoubleBuffered = true;
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);

            _mapBgColor = Color.Transparent;
            /*
            _defaultDigitizationInstructions = Strings.GeneralDigitizationInstructions;
            _defaultMultiSegmentDigitizationInstructions = Strings.MultiSegmentDigitzationInstructions;
            _defaultPointDigitizationPrompt = Strings.PointDigitizationPrompt;
            _defaultLineDigitizationPrompt = Strings.LineDigitizationPrompt;
            _defaultCircleDigitizationPrompt = Strings.CircleDigitizationPrompt;
            _defaultLineStringDigitizationPrompt = Strings.LineStringDigitizationPrompt;
            _defaultPolygonDigitizationPrompt = Strings.PolygonDigitizationPrompt;
            _defaultRectangleDigitizationPrompt = Strings.RectangleDigitizationPrompt;
            */
            renderWorker = new BackgroundWorker();

            renderWorker.DoWork += renderWorker_DoWork;
            renderWorker.RunWorkerCompleted += renderWorker_RunWorkerCompleted;

            base.MouseUp += OnMapMouseUp;
            base.MouseMove += OnMapMouseMove;
            base.MouseDown += OnMapMouseDown;
            base.MouseClick += OnMapMouseClick;
            base.MouseDoubleClick += OnMapMouseDoubleClick;
            base.MouseHover += OnMapMouseHover;
            base.MouseEnter += OnMouseEnter;
            base.MouseWheel += OnMapMouseWheel;
            base.MouseLeave += OnMapMouseLeave;
        }

        /// <summary>
        /// Raises the <see cref="E:System.Windows.Forms.Control.KeyUp"/> event.
        /// </summary>
        /// <param name="e">A <see cref="T:System.Windows.Forms.KeyEventArgs"/> that contains the event data.</param>
        protected override void OnKeyUp(KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
            {
                CancelDigitization();
            }
            else if (e.KeyCode == Keys.Z && e.Modifiers == Keys.Control)
            {
                if (this.DigitizingType == MapDigitizationType.LineString ||
                    this.DigitizingType == MapDigitizationType.Polygon)
                {
                    if (dPath.Count > 2) //Slice off the last recorded point
                    {
                        dPath.RemoveAt(dPath.Count - 1);
                        Invalidate();
                    }
                }
            }
        }

        public void CancelDigitization()
        {
            if (this.DigitizingType != MapDigitizationType.None)
            {
                dPath.Clear();
                dPtStart.X = 0;
                dPtStart.Y = 0;
                this.DigitizingType = MapDigitizationType.None;
                //Trace.TraceInformation("Digitization cancelled");
                this.Invalidate();
            }
        }

        void OnMouseEnter(object sender, EventArgs e)
        {
            this.Focus();   
        }

        void OnMapMouseHover(object sender, EventArgs e)
        {
            HandleMouseHover(e);
        }

        private void HandleMouseHover(EventArgs e)
        {
            
        }

        /// <summary>
        /// Releases the unmanaged resources used by the <see cref="T:System.Windows.Forms.Control"/> and its child controls and optionally releases the managed resources.
        /// </summary>
        /// <param name="disposing">true to release both managed and unmanaged resources; false to release only unmanaged resources.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                base.MouseUp -= OnMapMouseUp;
                base.MouseMove -= OnMapMouseMove;
                base.MouseDown -= OnMapMouseDown;
                base.MouseClick -= OnMapMouseClick;
                base.MouseDoubleClick -= OnMapMouseDoubleClick;
                base.MouseHover -= OnMapMouseHover;
                base.MouseEnter -= OnMouseEnter;
                base.MouseLeave -= OnMapMouseLeave;

                if (renderWorker != null)
                {
                    renderWorker.DoWork -= renderWorker_DoWork;
                    renderWorker.RunWorkerCompleted -= renderWorker_RunWorkerCompleted;
                }

                if (_csFact != null)
                {
                    _csFact.Dispose();
                    _csFact = null;
                }

                if (_resSvc != null)
                {
                    _resSvc.Dispose();
                    _resSvc = null;
                }

                if (_selection != null)
                {
                    _selection.Dispose();
                    _selection = null;
                }

                if (_mapCs != null)
                {
                    _mapCs.Dispose();
                    _mapCs = null;
                }

                if (_agfRW != null)
                {
                    _agfRW.Dispose();
                    _agfRW = null;
                }

                if (_wktRW != null)
                {
                    _wktRW.Dispose();
                    _wktRW = null;
                }

                if (_geomFact != null)
                {
                    _geomFact.Dispose();
                    _geomFact = null;
                }

                if (_mapMeasure != null)
                {
                    _mapMeasure.Dispose();
                    _mapMeasure = null;
                }
            }
            base.Dispose(disposing);
        }

        /// <summary>
        /// Gets or sets the minimum allowed zoom scale for this viewer
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The minimum allowed zoom scale for this viewer")] //NOXLATE
        [MgComponentProperty]
        public int MinScale { get; set; }

        /// <summary>
        /// Gets or sets the maximum allowed zoom scale for this viewer
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The maximum allowed zoom scale for this viewer")] //NOXLATE
        [MgComponentProperty]
        public int MaxScale { get; set; }

        /// <summary>
        /// The amount of time (in ms) to wait to re-render after a mouse wheel scroll
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The amount of time (in ms) to wait to re-render after a mouse wheel scroll")] //NOXLATE
        [MgComponentProperty]
        public int MouseWheelDelayRenderInterval { get; set; }

        /// <summary>
        /// The amount of time (in ms) to wait to re-render after a mouse wheel scroll
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The amount of time (in ms) to wait to fire off a tooltip request after the mouse pointer becomes stationary")] //NOXLATE
        [MgComponentProperty]
        public int TooltipDelayInterval { get; set; }

        private Color _selColor;

        /// <summary>
        /// Gets or sets the color used to render selected features
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The color to use for active selections")] //NOXLATE
        [MgComponentProperty]
        public Color SelectionColor
        {
            get { return _selColor; }
            set 
            { 
                _selColor = value;
                OnPropertyChanged("SelectionColor"); //NOXLATE
            }
        }

        private Color _tooltipFillColor;

        /// <summary>
        /// Gets or sets the color of the tooltip fill.
        /// </summary>
        /// <value>
        /// The color of the tooltip fill.
        /// </value>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The color background for feature tooltips")] //NOXLATE
        internal Color TooltipFillColor
        {
            get { return _tooltipFillColor; }
            set
            {
                if (!value.Equals(_tooltipFillColor))
                {
                    _tooltipFillColor = value;
                    OnPropertyChanged("TooltipFillColor"); //NOXLATE
                }
            }
        }

        private int _tooltipFillTransparency;

        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The color background transparency for feature tooltips")] //NOXLATE
        [DefaultValue(200)]
        internal int TooltipFillTransparency
        {
            get { return _tooltipFillTransparency; }
            set
            {
                if (!value.Equals(_tooltipFillTransparency))
                {
                    _tooltipFillTransparency = value;
                    OnPropertyChanged("TooltipFillTransparency"); //NOXLATE
                }
            }
        }

        private void UpdateSelectionRenderingOptions()
        {
            var value = this.SelectionColor;
            if (_selectionRenderOpts != null)
            {
                var color = _selectionRenderOpts.Color;
                if (color != null && (color.Red != value.R || 
                                      color.Green != value.G ||
                                      color.Blue != value.B))
                {
                    _selectionRenderOpts = null;

                    _selectionRenderOpts = CreateSelectionRenderingOptions(value.R, value.G, value.B);
                    //Trace.TraceInformation("Selection color updated to ({0}, {1}, {2})", value.R, value.G, value.B);
                }
            }
            
        }

        /// <summary>
        /// Gets the coordinate system of the runtime map
        /// </summary>
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public MgCoordinateSystem CoordinateSystem { get { return _mapCs; } }

        private bool _showVertexCoords;

        /// <summary>
        /// Gets or sets a value indicating whether [show vertex coordinates when digitizing].
        /// </summary>
        /// <value>
        /// 	<c>true</c> if [show vertex coordinates when digitizing]; otherwise, <c>false</c>.
        /// </value>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Indicates whether coordinate values are shown when digitizing geometry")] //NOXLATE
        [DefaultValue(false)]
        [MgComponentProperty]
        public bool ShowVertexCoordinatesWhenDigitizing
        {
            get { return _showVertexCoords; }
            set
            {
                if (!value.Equals(_showVertexCoords))
                {
                    _showVertexCoords = value;
                    OnPropertyChanged("ShowVertexCoordinatesWhenDigitizing"); //NOXLATE
                }
            }
        }
        /*
        private string _defaultDigitizationInstructions;
        private string _defaultMultiSegmentDigitizationInstructions;

        private string _defaultPointDigitizationPrompt;
        private string _defaultLineDigitizationPrompt;
        private string _defaultCircleDigitizationPrompt;
        private string _defaultLineStringDigitizationPrompt;
        private string _defaultPolygonDigitizationPrompt;
        private string _defaultRectangleDigitizationPrompt;
        */
        private string _pointCustomDigitizationPrompt;
        private string _lineCustomDigitizationPrompt;
        private string _circleCustomDigitizationPrompt;
        private string _lineStringCustomDigitizationPrompt;
        private string _polygonCustomDigitizationPrompt;
        private string _rectangleCustomDigitizationPrompt;

        /// <summary>
        /// Raises the <see cref="E:System.Windows.Forms.Control.Paint"/> event.
        /// </summary>
        /// <param name="e">A <see cref="T:System.Windows.Forms.PaintEventArgs"/> that contains the event data.</param>
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            //Trace.TraceInformation("OnPaint(e)");
            ApplyPaintTranslateTransform(e);

            if (mouseWheelSx.HasValue && mouseWheelSy.HasValue && mouseWheelSx.Value != 0.0 && mouseWheelSy.Value != 0.0)
            {
                e.Graphics.ScaleTransform(mouseWheelSx.Value, mouseWheelSy.Value);
            }

            var pre = this.PreMapRender;
            if (pre != null)
                pre(this, e);

            if (_mapImage != null)
            {
                //Trace.TraceInformation("Render buffered map image");
                e.Graphics.DrawImage(_mapImage, new PointF(0, 0));
            }

            //Thread.Sleep(100);
            if (_selectionImage != null)
            {
                //Trace.TraceInformation("Render buffered map selection");
                e.Graphics.DrawImage(_selectionImage, new PointF(0, 0));
            }

            //TODO: We could add support here for map-space persistent digitizations

            if (isDragging && (this.ActiveTool == MapActiveTool.Select || this.ActiveTool == MapActiveTool.ZoomIn))
            {
                DrawDragRectangle(e);
            }
            else
            {
                if (this.DigitizingType != MapDigitizationType.None)
                {
                    if (this.DigitizingType == MapDigitizationType.Point)
                    {
                        string str = (_pointCustomDigitizationPrompt ?? Strings.PointDigitizationPrompt) + Environment.NewLine + Strings.GeneralDigitizationInstructions;
                        DrawTrackingTooltip(e, str);
                    }
                    else
                    {
                        if (!dPtStart.IsEmpty)
                        {
                            switch (this.DigitizingType)
                            {
                                case MapDigitizationType.Circle:
                                    DrawTracingCircle(e);
                                    break;
                                case MapDigitizationType.Line:
                                    DrawTracingLine(e);
                                    break;
                                case MapDigitizationType.Rectangle:
                                    DrawTracingRectangle(e);
                                    break;
                            }
                        }
                        else if (dPath.Count > 0)
                        {
                            switch (this.DigitizingType)
                            {
                                case MapDigitizationType.LineString:
                                    DrawTracingLineString(e);
                                    break;
                                case MapDigitizationType.Polygon:
                                    DrawTracingPolygon(e);
                                    break;
                            }
                        }
                    }
                }
                else //None
                {
                    if (this.ActiveTool != MapActiveTool.None)
                    {
                        if (!string.IsNullOrEmpty(_activeTooltipText))
                            DrawTrackingTooltip(e, _activeTooltipText);
                    }
                }
            }

            var post = this.PostMapRender;
            if (post != null)
                post(this, e);
        }

        private void ApplyPaintTranslateTransform(PaintEventArgs e)
        {
            if (!translate.IsEmpty)
            {
                if (mouseWheelTx.HasValue && mouseWheelTy.HasValue)
                    e.Graphics.TranslateTransform(translate.X + mouseWheelTx.Value, translate.Y + mouseWheelTy.Value);
                else
                    e.Graphics.TranslateTransform(translate.X, translate.Y);
            }
            else
            {
                if (mouseWheelTx.HasValue && mouseWheelTy.HasValue)
                    e.Graphics.TranslateTransform(mouseWheelTx.Value, mouseWheelTy.Value);
            }
        }

        private Brush _digitizingOutline;

        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The outline color for geometries being digitized")] //NOXLATE
        internal Brush DigitizingOutline
        {
            get { return _digitizingOutline; }
            set
            {
                _digitizingOutline = value;
                OnPropertyChanged("DigitizingOutline"); //NOXLATE
            }
        }

        private int _digitizingFillTransparency;

        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The fill color transparency for geometries being digitized")] //NOXLATE
        [DefaultValue(100)]
        internal int DigitizingFillTransparency
        {
            get { return _digitizingFillTransparency; }
            set
            {
                if (!value.Equals(_digitizingFillTransparency))
                {
                    _digitizingFillTransparency = value;
                    OnPropertyChanged("DigitizingFillTransparency"); //NOXLATE
                }
            }
        }

        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The amount of pixels to buffer out by when doing point-based selections with the Select tool")] //NOXLATE
        [MgComponentProperty]
        public int PointPixelBuffer { get; set; }

        private Color _digitizingFillColor;

        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The fill color for geometries being digitized")] //NOXLATE
        internal Color DigitzingFillColor
        {
            get { return _digitizingFillColor; }
            set
            {
                _digitizingFillColor = value;
                OnPropertyChanged("DigitzingFillColor"); //NOXLATE
            }
        }

        private Pen CreateOutlinePen()
        {
            return new Pen(this.DigitizingOutline, 2.0f);
        }

        private Brush CreateFillBrush()
        {
            return new SolidBrush(Color.FromArgb(this.DigitizingFillTransparency, this.DigitzingFillColor));
        }

        private static double GetDistanceBetween(PointF a, PointF b)
        {
            return (Math.Sqrt(Math.Pow(Math.Abs(a.X - b.X), 2) + Math.Pow(Math.Abs(a.Y - b.Y), 2)));
        }

        private void DrawVertexCoordinates(PaintEventArgs e, double devX, double devY, bool mapSpace)
        {
            if (!this.ShowVertexCoordinatesWhenDigitizing)
                return;

            string text = string.Empty;
            if (mapSpace)
            {
                var mapPt = ScreenToMapUnits(devX, devY);
                text = string.Format("X: {0}, Y: {1}", mapPt.X, mapPt.Y); //NOXLATE
            }
            else
            {
                text = string.Format("X: {0}, Y: {1}", devX, devY); //NOXLATE
            }

            var f = Control.DefaultFont;
            SizeF size = e.Graphics.MeasureString(text, Font);
            var vertex = new PointF((float)devX, (float)devY);

            //Offset so that the "box" for this string is centered on the vertex itself
            vertex.X -= (size.Width / 2);

            //Fill the surrounding box
            e.Graphics.FillRectangle(new SolidBrush(Color.FromArgb(200, Color.WhiteSmoke)), vertex.X, vertex.Y, size.Width, size.Height);
            e.Graphics.DrawRectangle(Pens.Red, vertex.X, vertex.Y, size.Width, size.Height);

            //Draw the string
            e.Graphics.DrawString(text, f, Brushes.Black, vertex);
        }

        private void DrawTrackingTooltip(PaintEventArgs e, string text)
        {
            if (string.IsNullOrEmpty(text)) //Nothing to draw
                return;

            var f = Control.DefaultFont;
            
            
            int height = 0;
            int width = 0;
            string[] tokens = text.Split(new string[] { "\\n", "\\r\\n", "\n", Environment.NewLine }, StringSplitOptions.None); //NOXLATE
            foreach(string t in tokens)
            {
                var size = e.Graphics.MeasureString(t, f);
                height += (int)size.Height;

                width = Math.Max(width, (int)size.Width);
            }

            e.Graphics.FillRectangle(new SolidBrush(Color.FromArgb(this.TooltipFillTransparency, this.TooltipFillColor)), new Rectangle(_mouseX, _mouseY, width + 10, height + 4));
            float y = 2.0f;
            float heightPerLine = height / tokens.Length;
            foreach (string t in tokens)
            {
                e.Graphics.DrawString(t, f, Brushes.Black, new PointF(_mouseX + 5.0f, _mouseY + y));
                y += heightPerLine;
            }
        }

        private void DrawTracingCircle(PaintEventArgs e)
        {
            var pt2 = new Point(dPtStart.X, dPtStart.Y);
            var diameter = (float)GetDistanceBetween(dPtStart, new PointF(_mouseX, _mouseY)) * 2.0f;
            //Trace.TraceInformation("Diameter ({0}, {1} -> {2}, {3}): {4}", dPtStart.X, dPtStart.Y, _mouseX, _mouseY, diameter); //NOXLATE
            pt2.Offset((int)-(diameter / 2), (int)-(diameter / 2));
            e.Graphics.DrawEllipse(CreateOutlinePen(), pt2.X, pt2.Y, diameter, diameter);
            e.Graphics.FillEllipse(CreateFillBrush(), pt2.X, pt2.Y, diameter, diameter);

            string str = (_circleCustomDigitizationPrompt ?? Strings.CircleDigitizationPrompt) + Environment.NewLine + Strings.GeneralDigitizationInstructions;
            DrawTrackingTooltip(e, str);
        }

        private void DrawTracingLine(PaintEventArgs e)
        {
            e.Graphics.DrawLine(CreateOutlinePen(), dPtStart, new Point(_mouseX, _mouseY));
            DrawVertexCoordinates(e, dPtStart.X, dPtStart.Y, true);
            DrawVertexCoordinates(e, _mouseX, _mouseY, true);
            string str = (_lineCustomDigitizationPrompt ?? Strings.LineDigitizationPrompt) + Environment.NewLine + Strings.GeneralDigitizationInstructions;
            DrawTrackingTooltip(e, str);
        }

        private void DrawTracingLineString(PaintEventArgs e)
        {
            //Not enough points to constitute a line string or polygon
            if (dPath.Count < 2)
                return;
            
            e.Graphics.DrawLines(CreateOutlinePen(), dPath.ToArray());
            foreach (var pt in dPath)
            {
                DrawVertexCoordinates(e, pt.X, pt.Y, true);
            }
            string str = (_lineStringCustomDigitizationPrompt ?? Strings.LineStringDigitizationPrompt) + Environment.NewLine + Strings.MultiSegmentDigitzationInstructions;
            DrawTrackingTooltip(e, str);
        }

        private void DrawTracingPolygon(PaintEventArgs e)
        {
            //Not enough points to constitute a line string or polygon
            if (dPath.Count < 2)
                return;

            e.Graphics.DrawPolygon(CreateOutlinePen(), dPath.ToArray());
            e.Graphics.FillPolygon(CreateFillBrush(), dPath.ToArray());
            foreach (var pt in dPath)
            {
                DrawVertexCoordinates(e, pt.X, pt.Y, true);
            }
            string str = (_polygonCustomDigitizationPrompt ?? Strings.PolygonDigitizationPrompt) + Environment.NewLine + Strings.MultiSegmentDigitzationInstructions;
            DrawTrackingTooltip(e, str);
        }

        private void DrawTracingRectangle(PaintEventArgs e)
        {
            var rect = GetRectangle(dragStart, new Point(_mouseX, _mouseY));
            if (rect.HasValue)
            {
                var r = rect.Value;
                //Trace.TraceInformation("Draw rangle ({0} {1}, {2} {3})", r.Left, r.Top, r.Right, r.Bottom); //NOXLATE
                e.Graphics.DrawRectangle(CreateOutlinePen(), r);
                //Trace.TraceInformation("Fill rangle ({0} {1}, {2} {3})", r.Left, r.Top, r.Right, r.Bottom); //NOXLATE
                e.Graphics.FillRectangle(CreateFillBrush(), r);

                DrawVertexCoordinates(e, r.Left, r.Top, true);
                DrawVertexCoordinates(e, r.Left, r.Bottom, true);
                DrawVertexCoordinates(e, r.Right, r.Top, true);
                DrawVertexCoordinates(e, r.Right, r.Bottom, true);
                string str = (_rectangleCustomDigitizationPrompt ?? Strings.RectangleDigitizationPrompt) + Environment.NewLine + Strings.GeneralDigitizationInstructions;
                DrawTrackingTooltip(e, str);
            }
        }

        private void DrawDragRectangle(PaintEventArgs e)
        {
            var rect = GetRectangle(dragStart, new Point(_mouseX, _mouseY));
            if (rect.HasValue)
            {
                var r = rect.Value;
                //Trace.TraceInformation("Draw rangle ({0} {1}, {2} {3})", r.Left, r.Top, r.Right, r.Bottom); //NOXLATE
                e.Graphics.DrawRectangle(CreateOutlinePen(), r);
                //Trace.TraceInformation("Fill rangle ({0} {1}, {2} {3})", r.Left, r.Top, r.Right, r.Bottom); //NOXLATE
                e.Graphics.FillRectangle(CreateFillBrush(), r);
            }
        }

        private bool _featTooltipsEnabled;

        /// <summary>
        /// Gets or sets whether feature tooltips are enabled. If set to true, tooltip queries are
        /// executed at the current mouse position if the active tool is Pan or Select
        /// </summary>
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [MgComponentProperty]
        public bool FeatureTooltipsEnabled
        {
            get { return _featTooltipsEnabled; }
            set
            {
                if (value.Equals(_featTooltipsEnabled))
                    return;

                _featTooltipsEnabled = value;
                if (!value)
                {
                    _activeTooltipText = null;
                    Invalidate();
                }

                OnPropertyChanged("FeatureTooltipsEnabled"); //NOXLATE
            }
        }

        /// <summary>
        /// Internally determines whether a tooltip query can be executed. Must be true along
        /// with <see cref="FeatureTooltipsEnabled"/> in order for a tooltip query to be executed
        /// </summary>
        internal bool TooltipsEnabled
        {
            get;
            set;
        }

        #region Digitization

        /*
         * Digitization behaviour with respect to mouse and paint events
         * 
         * Point:
         *  MouseClick -> Invoke Callback
         * 
         * Rectangle:
         *  MouseClick -> set start, temp end
         *  MouseMove -> update temp end
         *  OnPaint -> Draw rectangle from start/temp end
         *  MouseClick -> set end -> Invoke Callback
         * 
         * Line:
         *  MouseClick -> set start, temp end
         *  MouseMove -> update temp end
         *  OnPaint -> Draw line from start/temp end
         *  MouseClick -> set end -> Invoke Callback
         * 
         * LineString:
         *  MouseClick -> append point to path
         *  MouseMove -> update temp end
         *  OnPaint -> Draw line with points in path + temp end
         *  MouseDoubleClick -> append point to path -> Invoke Callback
         * 
         * Polygon:
         *  MouseClick -> append point to path
         *  MouseMove -> update temp end
         *  OnPaint -> Draw polygon fill with points in path + temp end
         *  MouseDoubleClick -> append point to path -> Invoke Callback
         * 
         * Circle:
         *  MouseClick -> set start, temp end
         *  MouseMove -> update temp end
         *  OnPaint -> Draw circle from start with radius = (dist from start to temp end)
         *  MouseClick -> set end -> Invoke Callback
         */

        private Point dPtStart; //Rectangle, Line, Circle
        private Point dPtEnd; //Rectangle, Line, Circle
        private List<Point> dPath = new List<Point>(); //LineString, Polygon

        private Delegate _digitzationCallback;

        private bool _digitizationYetToStart = true;

        /// <summary>
        /// Starts the digitization process for a circle
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        public void DigitizeCircle(CircleDigitizationCallback callback, string customPrompt)
        {
            this.DigitizingType = MapDigitizationType.Circle;
            _digitzationCallback = callback;
            _digitizationYetToStart = true;
            _circleCustomDigitizationPrompt = customPrompt;
        }

        /// <summary>
        /// Starts the digitization process for a line
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        public void DigitizeLine(LineDigitizationCallback callback, string customPrompt)
        {
            this.DigitizingType = MapDigitizationType.Line;
            _digitzationCallback = callback;
            _digitizationYetToStart = true;
            _lineCustomDigitizationPrompt = customPrompt;
        }

        /// <summary>
        /// Starts the digitization process for a point
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        public void DigitizePoint(PointDigitizationCallback callback, string customPrompt)
        {
            this.DigitizingType = MapDigitizationType.Point;
            _digitzationCallback = callback;
            _digitizationYetToStart = true;
            _pointCustomDigitizationPrompt = customPrompt;
        }

        /// <summary>
        /// Starts the digitization process for a polygon
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        public void DigitizePolygon(PolygonDigitizationCallback callback, string customPrompt)
        {
            this.DigitizingType = MapDigitizationType.Polygon;
            _digitzationCallback = callback;
            _digitizationYetToStart = true;
            _polygonCustomDigitizationPrompt = customPrompt;
        }

        /// <summary>
        /// Starts the digitization process for a line string (polyline)
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        public void DigitizeLineString(LineStringDigitizationCallback callback, string customPrompt)
        {
            this.DigitizingType = MapDigitizationType.LineString;
            _digitzationCallback = callback;
            _digitizationYetToStart = true;
            _lineStringCustomDigitizationPrompt = customPrompt;
        }

        private LineDigitizationCallback _segmentCallback;

        /// <summary>
        /// Starts the digitization process for a line string (polyline)
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="segmentCallback">The callback to be invoked when a new segment of the current line string is digitized</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        public void DigitizeLineString(LineStringDigitizationCallback callback, LineDigitizationCallback segmentCallback, string customPrompt)
        {
            this.DigitizingType = MapDigitizationType.LineString;
            _digitzationCallback = callback;
            _segmentCallback = segmentCallback;
            _digitizationYetToStart = true;
            _lineStringCustomDigitizationPrompt = customPrompt;
        }

        /// <summary>
        /// Starts the digitization process for a rectangle
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        public void DigitizeRectangle(RectangleDigitizationCallback callback, string customPrompt)
        {
            this.DigitizingType = MapDigitizationType.Rectangle;
            _digitzationCallback = callback;
            _digitizationYetToStart = true;
            _rectangleCustomDigitizationPrompt = customPrompt;
        }

        public void DigitizeCircle(CircleDigitizationCallback callback)
        {
            DigitizeCircle(callback, null);
        }

        public void DigitizeLine(LineDigitizationCallback callback)
        {
            DigitizeLine(callback, null);
        }

        public void DigitizePoint(PointDigitizationCallback callback)
        {
            DigitizePoint(callback, null);
        }

        public void DigitizePolygon(PolygonDigitizationCallback callback)
        {
            DigitizePolygon(callback, null);
        }

        public void DigitizeLineString(LineStringDigitizationCallback callback)
        {
            DigitizeLineString(callback, (string)null);
        }

        public void DigitizeLineString(LineStringDigitizationCallback callback, LineDigitizationCallback segmentDigitized)
        {
            DigitizeLineString(callback, segmentDigitized, null);
        }

        public void DigitizeRectangle(RectangleDigitizationCallback callback)
        {
            DigitizeRectangle(callback, null);
        }

        private void ResetDigitzationState()
        {
            _digitzationCallback = null;
            _segmentCallback = null;
            dPath.Clear();
            dPtEnd.X = dPtStart.Y = 0;
            dPtStart.X = dPtStart.Y = 0;
            this.DigitizingType = MapDigitizationType.None;
            _circleCustomDigitizationPrompt = null;
            _lineCustomDigitizationPrompt = null;
            _lineStringCustomDigitizationPrompt = null;
            _polygonCustomDigitizationPrompt = null;
            _pointCustomDigitizationPrompt = null;
            _rectangleCustomDigitizationPrompt = null;
            Invalidate();
        }

        private void OnCircleDigitized(Point ptStart, Point ptEnd)
        {
            var mapPt = ScreenToMapUnits(ptStart.X, ptStart.Y);
            var mapEnd = ScreenToMapUnits(ptEnd.X, ptEnd.Y);

            var radius = Math.Sqrt(Math.Pow(mapEnd.X - mapPt.X, 2) + Math.Pow(mapEnd.Y - mapPt.Y, 2));

            var cb = (CircleDigitizationCallback)_digitzationCallback;
            ResetDigitzationState();
            cb(mapPt.X, mapPt.Y, radius);
        }

        private void OnPolygonDigitized(List<Point> path)
        {
            double[,] coords = new double[path.Count, 2];
            for (int i = 0; i < path.Count; i++)
            {
                var pt = ScreenToMapUnits(path[i].X, path[i].Y);
                coords[i, 0] = pt.X;
                coords[i, 1] = pt.Y;
            }

            var cb = (PolygonDigitizationCallback)_digitzationCallback;
            ResetDigitzationState();
            cb(coords);
        }

        private void OnLineStringSegmentDigitized(Point p1, Point p2)
        {
            if (_segmentCallback != null)
            {
                var ptx1 = ScreenToMapUnits(p1.X, p1.Y);
                var ptx2 = ScreenToMapUnits(p2.X, p2.Y);
                _segmentCallback.Invoke(ptx1.X, ptx1.Y, ptx2.X, ptx2.Y);
            }
        }

        private void OnLineStringDigitized(List<Point> path)
        {
            double[,] coords = new double[path.Count, 2];
            for (int i = 0; i < path.Count; i++)
            {
                var pt = ScreenToMapUnits(path[i].X, path[i].Y);
                coords[i, 0] = pt.X;
                coords[i, 1] = pt.Y;
            }

            var cb = (LineStringDigitizationCallback)_digitzationCallback;
            ResetDigitzationState();
            cb(coords);
        }

        private void OnLineDigitized(Point start, Point end)
        {
            var mapStart = ScreenToMapUnits(start.X, start.Y);
            var mapEnd = ScreenToMapUnits(end.X, end.Y);

            var cb = (LineDigitizationCallback)_digitzationCallback;
            ResetDigitzationState();
            cb(mapStart.X, mapStart.Y, mapEnd.X, mapEnd.Y);
        }

        private void OnRectangleDigitized(Rectangle rect)
        {
            var mapLL = ScreenToMapUnits(rect.Left, rect.Bottom);
            var mapUR = ScreenToMapUnits(rect.Right, rect.Top);

            var cb = (RectangleDigitizationCallback)_digitzationCallback;
            ResetDigitzationState();
            cb(mapLL.X, mapLL.Y, mapUR.X, mapUR.Y);
        }

        private void OnPointDigitizationCompleted(Point p)
        {
            var mapPt = ScreenToMapUnits(p.X, p.Y);
            var cb = (PointDigitizationCallback)_digitzationCallback;
            ResetDigitzationState();
            cb(mapPt.X, mapPt.Y);
        }

        #endregion

        /// <summary>
        /// Renders the selected feature(s)
        /// </summary>
        const int BEHAVIOR_RENDER_SELECTION = 1;
        /// <summary>
        /// Renders the dynamic layers on the map
        /// </summary>
        const int BEHAVIOR_RENDER_LAYERS = 2;
        /// <summary>
        /// Renders the selected feature(s) even if they are outside the current scale
        /// </summary>
        const int BEHAVIOR_KEEP_SELECTION = 4;
        /// <summary>
        /// Renders the base layers on the map
        /// </summary>
        const int BEHAVIOR_RENDER_BASE_LAYERS = 8; //New in 2.5

        static MgViewerRenderingOptions CreateMapRenderingOptions(short red, short green, short blue)
        {
            return new MgViewerRenderingOptions("PNG", BEHAVIOR_RENDER_LAYERS | BEHAVIOR_RENDER_BASE_LAYERS, new MgColor(red, green, blue)); //NOXLATE
        }

        static MgViewerRenderingOptions CreateSelectionRenderingOptions(short red, short green, short blue)
        {
            return new MgViewerRenderingOptions("PNG", (BEHAVIOR_RENDER_SELECTION | BEHAVIOR_KEEP_SELECTION), new MgColor(red, green, blue)); //NOXLATE
        }

        public void LoadMap(MgMapBase map)
        {
            if (_provider == null)
                throw new InvalidOperationException(Strings.ErrorViewerNotInitialized);

            _provider.LoadMap(map);
        }

        /// <summary>
        /// Initializes this viewer with the specified viewer provider, if the provider contains a map
        /// it will load that as well. Otherwise a call to <see cref="LoadMap"/> is required afterwards
        /// </summary>
        /// <param name="provider">The provider.</param>
        public void Init(MgMapViewerProvider provider)
        {
            if (_agfRW == null)
                _agfRW = new MgAgfReaderWriter();

            if (_wktRW == null)
                _wktRW = new MgWktReaderWriter();

            if (_geomFact == null)
                _geomFact = new MgGeometryFactory();

            _provider = provider;

            if (_resSvc != null) //Forward looking, dispose the existing one as it may be of a different implementation
            {
                _resSvc.Dispose();
                _resSvc = null;
            }
            _resSvc = (MgResourceService)_provider.CreateService(MgServiceType.ResourceService);

            _overlayRenderOpts = CreateMapRenderingOptions(0, 0, 255);
            _selectionRenderOpts = CreateSelectionRenderingOptions(0, 0, 255);

            if (_provider != null)
            {
                _provider.MapLoaded -= OnMapSetOnProvider;
                _provider = null;
            }

            _provider = provider;
            _provider.MapLoaded += OnMapSetOnProvider;
            var map = _provider.GetMap();
            if (map != null)
                OnMapSetOnProvider(this, EventArgs.Empty);
        }

        private void OnMapSetOnProvider(object sender, EventArgs e)
        {
            _hasTiledLayers = null;
            _map = _provider.GetMap();
            _mapCs = _provider.GetMapCoordinateSystem();
            _mapMeasure = _mapCs.GetMeasure();
            var bgColor = _map.GetBackgroundColor();
            if (bgColor.Length == 8 || bgColor.Length == 6)
            {
                _mapBgColor = ColorTranslator.FromHtml("#" + bgColor); //NOXLATE
                this.BackColor = _mapBgColor;
            }
            _provider.SetDisplaySize(this.Width, this.Height);
            _selection = _provider.CreateSelectionForMap();

            var env = _map.GetMapExtent();
            var ll = env.LowerLeftCoordinate;
            var ur = env.UpperRightCoordinate;

            _extX1 = _orgX1 = ll.X;
            _extY2 = _orgY2 = ll.Y;
            _extX2 = _orgX2 = ur.X;
            _extY1 = _orgY1 = ur.Y;

            if ((_orgX1 - _orgX2) == 0 || (_orgY1 - _orgY2) == 0)
            {
                _extX1 = _orgX1 = -.1;
                _extY2 = _orgX2 = .1;
                _extX2 = _orgY1 = -.1;
                _extY1 = _orgY2 = .1;
            }

            if (this.ConvertTiledGroupsToNonTiled)
            {
                var groups = _map.GetLayerGroups();
                for (int i = 0; i < groups.GetCount(); i++)
                {
                    var group = groups.GetItem(i);
                    _provider.MakeGroupNormal(group);
                }
            }

#if VIEWER_DEBUG
            CreateDebugFeatureSource();
#endif
            this.Focus();

            //Reset history stack
            _viewHistory.Clear();
            OnPropertyChanged("ViewHistory"); //NOXLATE
            _viewHistoryIndex = -1;
            OnPropertyChanged("ViewHistoryIndex"); //NOXLATE
            
            var handler = this.MapLoaded;
            if (handler != null)
                handler(this, EventArgs.Empty);

            InitialMapView();
        }

        internal double MetersPerUnit
        {
            get
            {
                return _provider.GetMetersPerUnit(); 
            }
        }

        private double CalculateScale(double mcsW, double mcsH, int devW, int devH)
        {
            var mpu = this.MetersPerUnit;
            var mpp = GetMetersPerPixel(_map.DisplayDpi);
            if (devH * mcsW > devW * mcsH)
                return mcsW * mpu / (devW * mpp); //width-limited
            else
                return mcsH * mpu / (devH * mpp); //height-limited
        }

        /// <summary>
        /// Gets or sets a value indicating whether tiled groups are converted to normal groups. Must be set before
        /// a map is loaded via the <see cref="Init"/> method
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("If true, the map being viewed will have all its tiled groups converted to non-tiled groups. Tiled groups are not supported by this viewer and are not rendered")] //NOXLATE
        [DefaultValue(false)]
        [MgComponentProperty]
        public bool ConvertTiledGroupsToNonTiled
        {
            get;
            set;
        }

        /// <summary>
        /// Gets whether to use the RenderMap API instead of RenderDynamicOverlay if the map has tiled
        /// layers. RenderMap includes tiled layers as part of the output image, but will not take advantage
        /// of any tile caching mechanisms. Setting this property to true nullifies any effect of the 
        /// <see cref="P:OSGeo.MapGuide.Viewer.IMapViewer.ConvertTiledGroupsToNonTiled"/> property
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("If true, the viewer will use the RenderMap API instead of RenderDynamicOverlay allowing tiled layers to be rendered to the final image. Setting this property to true nullifies the ConvertTiledGroupsToNonTiled property")] //NOXLATE
        [DefaultValue(true)]
        [MgComponentProperty]
        public bool UseRenderMapIfTiledLayersExist { get; set; }

        /// <summary>
        /// Gets whether to respect the list of finite display scales in a map being viewed if there are any defined.
        /// If true, all zooms will "snap" to the nearest finite display scale. Otherwise, the viewer will disregard
        /// this list when zooming in or out.
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("If true, all zooms will snap to the nearest finite display scale defined in the map being viewed")] //NOXLATE
        [DefaultValue(true)]
        [MgComponentProperty]
        public bool RespectFiniteDisplayScales { get; set; }

        /// <summary>
        /// Raised when the viewer has been initialized
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Raised when the viewer has been initialized with a runtime map")] //NOXLATE
        public event EventHandler MapLoaded;

        private System.Timers.Timer _delayedResizeTimer;

        void OnDelayResizeTimerElapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            var action = new MethodInvoker(() =>
            {
                ResetMouseWheelPaintTransforms();
                if (_map != null)
                {
                    //Trace.TraceInformation("Performing delayed resize to (" + this.Width + ", " + this.Height + ")"); //NOXLATE
                    _provider.SetDisplaySize(this.Width, this.Height);
                    UpdateExtents();
                    RefreshMap(false);
                }
                _delayedResizeTimer.Stop();
                //Trace.TraceInformation("Delayed resize timer stopped"); //NOXLATE
            });
            if (this.InvokeRequired)
                this.Invoke(action);
            else
                action();
        }

        void OnControlResized(object sender, EventArgs e)
        {
            if (_delayedResizeTimer == null)
            {
                _delayedResizeTimer = new System.Timers.Timer();
                _delayedResizeTimer.Elapsed += OnDelayResizeTimerElapsed;
                //Trace.TraceInformation("Delay resize timer initialized"); //NOXLATE
            }
            if (_delayedResizeTimer.Enabled)
            {
                //Trace.TraceInformation("Stopped delayed resize"); //NOXLATE
                _delayedResizeTimer.Stop();
            }

            _delayedResizeTimer.Interval = 500;
            _delayedResizeTimer.Start();
            //Trace.TraceInformation("Delayed resize re-scheduled"); //NOXLATE
        }

        /// <summary>
        /// Clears the current selection
        /// </summary>
        public void ClearSelection()
        {
            _provider.ClearSelection(_selection);

            if (_selectionImage != null)
            {
                _selectionImage.Dispose();
                _selectionImage = null;
            }

            var handler = this.SelectionChanged;
            if (handler != null)
                handler(this, EventArgs.Empty);

            this.Refresh();
        }

        /// <summary>
        /// Gets the current runtime map
        /// </summary>
        /// <returns></returns>
        public MgMapBase GetMap()
        {
            return _map;
        }

        /// <summary>
        /// Gets the map viewer provider for this control
        /// </summary>
        /// <returns></returns>
        public MgMapViewerProvider GetProvider()
        {
            return _provider;
        }

        /// <summary>
        /// Gets the selection set of the runtime map
        /// </summary>
        /// <returns></returns>
        public MgSelectionBase GetSelection()
        {
            return _selection;
        }

        private bool HasSelection()
        {
            var layers = _selection.GetLayers();
            return layers != null;
        }

        private static int GetSelectionTotal(MgSelectionBase sel)
        {
            int total = 0;
            var layers = sel.GetLayers();
            if (layers != null)
            {
                for (int i = 0; i < layers.GetCount(); i++)
                {
                    var layer = layers.GetItem(i);
                    total += sel.GetSelectedFeaturesCount(layer, layer.FeatureClassName);
                }
            }
            return total;
        }

        private MapDigitizationType _digitizingType = MapDigitizationType.None;

        /// <summary>
        /// Gets the type of object being currently digitized. If the digitization type is None, then
        /// the viewer is not currently digitizing
        /// </summary>
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public MapDigitizationType DigitizingType
        {
            get { return _digitizingType; }
            private set
            {
                if (_digitizingType.Equals(value))
                    return;

                if (value != MapDigitizationType.None)
                {
                    this.ActiveTool = MapActiveTool.None;
                    this.Cursor = Cursors.Cross;
                }
                else
                {
                    this.Cursor = Cursors.Default;
                }

                _digitizingType = value;

                OnPropertyChanged("DigitizingType"); //NOXLATE
            }
        }

        class RenderWorkArgs
        {
            public RenderWorkArgs() { this.UseRenderMap = false; }

            public bool UseRenderMap { get; set; }

            public MgViewerRenderingOptions SelectionRenderingOptions { get; set; }

            public MgViewerRenderingOptions MapRenderingOptions { get; set; }

            public bool RaiseEvents { get; set; }

            public bool InvalidateRegardless { get; set; }
        }

        class RenderResult
        {
            public Image Image { get; set; }

            public Image SelectionImage { get; set; }

            public bool RaiseEvents { get; set; }

            public bool InvalidateRegardless { get; set; }
        }

        /// <summary>
        /// Refreshes the current map view
        /// </summary>
        public void RefreshMap()
        {
            RefreshMap(true);
        }

        /// <summary>
        /// Updates the rendered selection. Call this method if you have manipulated the selection
        /// set outside of the viewer. This does not raise the <see cref="SelectionChanged"/> event
        /// </summary>
        /// <remarks>
        /// If you have modified the selection as a result of calling <see cref="M:OSGeo.MapGuide.Viewer.IMapViewer.SelectByGeometry"/>, calling
        /// this method is not necessary as it will have automatically do this.
        /// </remarks>
        public void UpdateSelection()
        {
            UpdateSelection(false);
        }

        /// <summary>
        /// Updates the rendered selection. Call this method if you have manipulated the selection
        /// set outside of the viewer
        /// </summary>
        /// <param name="raise">Indicates if the <see cref="SelectionChanged"/> event should be raised as well</param>
        /// <remarks>
        /// If you have modified the selection as a result of calling <see cref="M:OSGeo.MapGuide.Viewer.IMapViewer.SelectByGeometry"/>, calling
        /// this method is not necessary as it will have automatically do this.
        /// </remarks>
        public void UpdateSelection(bool raise)
        {
            RenderSelection();
            if (raise)
            {
                var handler = this.SelectionChanged;
                if (handler != null)
                    handler(this, EventArgs.Empty);
            }
        }

        internal void RenderSelection()
        {
            RenderSelection(false);
        }

        internal void RenderSelection(bool invalidateRegardless)
        {
            //This is our refresh action
            RefreshAction action = new RefreshAction(() => 
            {
                if (HasSelection())
                {
                    this.IsBusy = true;
                    UpdateSelectionRenderingOptions();
                    renderWorker.RunWorkerAsync(new RenderWorkArgs()
                    {
                        SelectionRenderingOptions = _selectionRenderOpts,
                        RaiseEvents = false,
                        InvalidateRegardless = invalidateRegardless
                    });
                }
                else
                {
                    if (invalidateRegardless)
                        this.Invalidate();
                }
            });

            //If an existing rendering operation is in progress queue it if 
            //there isn't one queued. Because there is no point in doing the
            //same thing more than once
            if (this.IsBusy)
            {
                if (_queuedRefresh == null) //No refresh operations currently queued
                    _queuedRefresh = action;
            }
            else //Otherwise execute it immediately
            {
                action();
            }
        }

        delegate void RefreshAction();

        RefreshAction _queuedRefresh = null;

        internal void RefreshMap(bool raiseEvents)
        {
            //This is our refresh action
            RefreshAction action = new RefreshAction(() => 
            {
                var args = new RenderWorkArgs()
                {
                    UseRenderMap = this.UseRenderMapIfTiledLayersExist && this.HasTiledLayers,
                    MapRenderingOptions = _overlayRenderOpts,
                    RaiseEvents = raiseEvents
                };
                if (HasSelection())
                {
                    UpdateSelectionRenderingOptions();
                    args.SelectionRenderingOptions = _selectionRenderOpts;
                }
                this.IsBusy = true;
                renderWorker.RunWorkerAsync(args);
            });

            //If an existing rendering operation is in progress queue it if 
            //there isn't one queued. Because there is no point in doing the
            //same thing more than once
            if (this.IsBusy)
            {
                if (_queuedRefresh == null) //No refresh operations currently queued
                    _queuedRefresh = action;
            }
            else //Otherwise execute it immediately
            {
                action();   
            }
        }

        private bool? _hasTiledLayers;

        internal bool HasTiledLayers
        {
            get
            {
                if (!_hasTiledLayers.HasValue)
                {
                    if (_map != null)
                    {
                        var groups = _map.GetLayerGroups();
                        for (int i = 0; i < groups.Count; i++)
                        {
                            if (groups[i].LayerGroupType == MgLayerGroupType.BaseMap)
                            {
                                _hasTiledLayers = true;
                                break;
                            }
                        }
                        if (!_hasTiledLayers.HasValue)
                            _hasTiledLayers = false;
                    }
                    else
                    {
                        _hasTiledLayers = false;
                    }
                }
                return _hasTiledLayers.Value;
            }
        }

        /// <summary>
        /// Raised when the map has been refreshed
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Raised after the viewer has refreshed")] //NOXLATE
        public event EventHandler MapRefreshed;

        private bool _busy = false;

#if TRACE
        private Stopwatch _renderSw = new Stopwatch();
#endif

        /// <summary>
        /// Indicates whether a rendering operation is in progress
        /// </summary>
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public bool IsBusy
        {
            get { return _busy; }
            private set
            {
                if (_busy.Equals(value))
                    return;

                _busy = value;
#if TRACE
                //Trace.TraceInformation("IsBusy = " + _busy);
                if (value)
                {
                    _renderSw.Reset();
                    _renderSw.Start();
                }
                else
                {
                    _renderSw.Stop();
                    //Trace.TraceInformation("Rendering operation took {0}ms", _renderSw.ElapsedMilliseconds);
                }
#endif
                OnPropertyChanged("IsBusy"); //NOXLATE
            }
        }

        /// <summary>
        /// Pans the view left by a pre-defined distance
        /// </summary>
        /// <param name="refresh"></param>
        public void PanLeft(bool refresh)
        {
            PanTo(_extX1 + (_extX2 - _extX1) / 3, _extY2 + (_extY1 - _extY2) / 2, refresh);
        }

        /// <summary>
        /// Pans the view up by a pre-defined distance
        /// </summary>
        /// <param name="refresh"></param>
        public void PanUp(bool refresh)
        {
            PanTo(_extX1 + (_extX2 - _extX1) / 2, _extY1 - (_extY1 - _extY2) / 3, refresh);
        }

        /// <summary>
        /// Pans the view right by a pre-defined distance
        /// </summary>
        /// <param name="refresh"></param>
        public void PanRight(bool refresh)
        {
            PanTo(_extX2 - (_extX2 - _extX1) / 3, _extY2 + (_extY1 - _extY2) / 2, refresh);
        }

        /// <summary>
        /// Pans the view down by a pre-defined distance
        /// </summary>
        /// <param name="refresh"></param>
        public void PanDown(bool refresh)
        {
            PanTo(_extX1 + (_extX2 - _extX1) / 2, _extY2 + (_extY1 - _extY2) / 3, refresh);
        }

        /// <summary>
        /// Zooms the extents.
        /// </summary>
        public void ZoomExtents()
        {
            var scale = CalculateScale((_orgX2 - _orgX1), (_orgY1 - _orgY2), this.Width, this.Height);
            ZoomToView(_orgX1 + ((_orgX2 - _orgX1) / 2), _orgY2 + ((_orgY1 - _orgY2) / 2), scale, true);
        }

        /// <summary>
        /// Zooms to the view defined by the specified extent
        /// </summary>
        /// <param name="llx"></param>
        /// <param name="lly"></param>
        /// <param name="urx"></param>
        /// <param name="ury"></param>
        public void ZoomToExtents(double llx, double lly, double urx, double ury)
        {
            var scale = CalculateScale((urx - llx), (ury - lly), this.Width, this.Height);
            ZoomToView(llx + ((urx - llx) / 2), ury + ((lly - ury) / 2), scale, true);
        }

        /// <summary>
        /// Zooms to scale.
        /// </summary>
        /// <param name="scale">The scale.</param>
        public void ZoomToScale(double scale)
        {
            ZoomToView(_extX1 + (_extX2 - _extX1) / 2, _extY2 + (_extY1 - _extY2) / 2, scale, true);
        }

        /// <summary>
        /// Zooms to the specified map view
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="scale"></param>
        /// <param name="refresh"></param>
        public void ZoomToView(double x, double y, double scale, bool refresh)
        {
            ZoomToView(x, y, scale, refresh, true);
        }

        internal void PanTo(double x, double y, bool refresh)
        {
            ZoomToView(x, y, _map.ViewScale, refresh);
        }

        private void UpdateExtents()
        {
            //Update current extents
            double mpu = this.MetersPerUnit;
            double scale = _map.ViewScale;
            double mpp = GetMetersPerPixel(_map.DisplayDpi);
            var pt = _map.ViewCenter;
            var coord = pt.Coordinate;

            var mcsWidth = _map.DisplayWidth * mpp * scale / mpu;
            var mcsHeight = _map.DisplayHeight * mpp * scale / mpu;

            _extX1 = coord.X - mcsWidth / 2;
            _extY1 = coord.Y + mcsHeight / 2;
            _extX2 = coord.X + mcsWidth / 2;
            _extY2 = coord.Y - mcsHeight / 2;
        }

        private bool PruneHistoryEntriesFromCurrentView()
        {
            if (_viewHistoryIndex < _viewHistory.Count - 1)
            {
                int removed = 0;
                for (int i = _viewHistory.Count - 1; i > _viewHistoryIndex; i--)
                {
                    _viewHistory.RemoveAt(i);
                    removed++;
                }
                return removed > 0;
            }
            return false;
        }

        internal void ZoomToView(double x, double y, double scale, bool refresh, bool raiseEvents)
        {
            ZoomToView(x, y, scale, refresh, raiseEvents, true);
        }

        /// <summary>
        /// Navigates to the previous view in the history stack
        /// </summary>
        public void PreviousView()
        {
            var newIndex = _viewHistoryIndex - 1;
            if (newIndex < 0)
                return;

            var view = _viewHistory[newIndex];
            ZoomToView(view.X, view.Y, view.Scale, true, true, false);
            _viewHistoryIndex = newIndex;
            OnPropertyChanged("ViewHistoryIndex"); //NOXLATE
        }

        /// <summary>
        /// Navigates to the next view in the history stack
        /// </summary>
        public void NextView()
        {
            //Cannot advance from current view
            if (_viewHistoryIndex == _viewHistory.Count - 1)
                return;

            var newIndex = _viewHistoryIndex + 1;
            if (newIndex > _viewHistory.Count - 1)
                return;

            var view = _viewHistory[newIndex];
            ZoomToView(view.X, view.Y, view.Scale, true, true, false);
            _viewHistoryIndex = newIndex;
            OnPropertyChanged("ViewHistoryIndex"); //NOXLATE
        }

        /// <summary>
        /// Gets the current index in the view history stack
        /// </summary>
        public int ViewHistoryIndex
        {
            get { return _viewHistoryIndex; }
        }

        /// <summary>
        /// Gets the view history stack. The first item being the earliest and the last item being the most recent.
        /// </summary>
        public ReadOnlyCollection<MgMapViewHistoryEntry> ViewHistory
        {
            get { return _viewHistory.AsReadOnly(); }
        }

        internal void ZoomToView(double x, double y, double scale, bool refresh, bool raiseEvents, bool addToHistoryStack)
        {
            var newScale = NormalizeScale(scale);
            if (_map.FiniteDisplayScaleCount > 0 && this.RespectFiniteDisplayScales)
                newScale = GetNearestFiniteScale(scale);
            if (addToHistoryStack)
            {
                //If not current view, then any entries from the current view index are no longer needed
                if (ViewHistoryIndex < _viewHistory.Count - 1)
                    PruneHistoryEntriesFromCurrentView();

                _viewHistory.Add(new MgMapViewHistoryEntry(x, y, newScale));
                OnPropertyChanged("ViewHistory"); //NOXLATE
                _viewHistoryIndex = _viewHistory.Count - 1;
                OnPropertyChanged("ViewHistoryIndex"); //NOXLATE
            }

            _provider.SetViewCenterXY(x, y);
#if VIEWER_DEBUG
            UpdateCenterDebugPoint();
            //var mapExt = _map.MapExtent;
            //var dataExt = _map.DataExtent;
            Trace.TraceInformation("Map Extent is ({0},{1} {2},{3})", mapExt.LowerLeftCoordinate.X, mapExt.LowerLeftCoordinate.Y, mapExt.UpperRightCoordinate.X, mapExt.UpperRightCoordinate.Y);
            Trace.TraceInformation("Data Extent is ({0},{1} {2},{3})", dataExt.LowerLeftCoordinate.X, dataExt.LowerLeftCoordinate.Y, dataExt.UpperRightCoordinate.X, dataExt.UpperRightCoordinate.Y);

            Trace.TraceInformation("Center is (" + x + ", " + y + ")");
#endif
            var oldScale = _map.ViewScale;
            _provider.SetViewScale(newScale);

            if (oldScale != _map.ViewScale)
            {
                var handler = this.MapScaleChanged;
                if (handler != null)
                    handler(this, EventArgs.Empty);
            }

            UpdateExtents();

#if VIEWER_DEBUG
            Trace.TraceInformation("Current extents is ({0},{1} {2},{3})", _extX1, _extY1, _extX2, _extY2);
#endif

            //Then refresh
            if (refresh)
                RefreshMap(raiseEvents);
        }

        private double GetNearestFiniteScale(double scale)
        {
            return _map.GetFiniteDisplayScaleAt(GetFiniteScaleIndex(scale));
        }

        private int GetFiniteScaleIndex(double reqScale)
        {
            var index = 0;
            var scaleCount = _map.GetFiniteDisplayScaleCount();
            if (scaleCount > 0)
            {
                var bestDiff = Math.Abs(_map.GetFiniteDisplayScaleAt(0) - reqScale);
                for (var i = 1; i < scaleCount; i++)
                {
                    var scaleDiff = Math.Abs(_map.GetFiniteDisplayScaleAt(i) - reqScale);
                    if (scaleDiff < bestDiff)
                    {
                        index = i;
                        bestDiff = scaleDiff;
                        if (bestDiff == 0)
                        {
                            //perfect match
                            break;
                        }
                    }
                }
            }
            return index;
        }

        /// <summary>
        /// Raised when the scale of the current runtime map has changed
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Raised when the zoom scale of the map has changed")] //NOXLATE
        public event EventHandler MapScaleChanged;

        /// <summary>
        /// Raised when the selection has changed. Note that programmatic selection modifications
        /// will not raise this event.
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Raised when active viewer selection has changed")] //NOXLATE
        public event EventHandler SelectionChanged;

        private void renderWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            var args = (RenderWorkArgs)e.Argument;
            var res = new RenderResult() { RaiseEvents = args.RaiseEvents, InvalidateRegardless = args.InvalidateRegardless };
            if (args.MapRenderingOptions != null)
            {
                MgByteReader br = null;
                if (args.UseRenderMap)
                    br = _provider.RenderMap(null, args.MapRenderingOptions.Format);
                else
                    br = _provider.RenderDynamicOverlay(null, args.MapRenderingOptions);
                byte[] b = new byte[br.GetLength()];
                br.Read(b, b.Length);
                using (var ms = new MemoryStream(b))
                {
                    res.Image = Image.FromStream(ms);
                }
            }
            if (args.SelectionRenderingOptions != null)
            {
                var br = _provider.RenderDynamicOverlay(_selection, args.SelectionRenderingOptions);
                byte[] b = new byte[br.GetLength()];
                br.Read(b, b.Length);
                using (var ms = new MemoryStream(b))
                {
                    res.SelectionImage = Image.FromStream(ms);
                }
            }

            e.Result = res;
        }

        private void renderWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            this.IsBusy = AreWorkersBusy();
            if (e.Error != null)
            {
                MessageBox.Show(e.Error.Message, Strings.TitleError); //NOXLATE
            }
            else
            {
                var res = (RenderResult)e.Result;
                //reset translation
                translate = new System.Drawing.Point();

                bool bInvalidate = false;
                //set the image
                if (res.Image != null)
                {
                    if (this.Image != null)
                    {
                        this.Image.Dispose();
                        this.Image = null;
                    }
                    //Trace.TraceInformation("Set map image"); //NOXLATE
                    this.Image = res.Image;
                    bInvalidate = true;
                }
                if (res.SelectionImage != null)
                {
                    if (_selectionImage != null)
                    {
                        _selectionImage.Dispose();
                        _selectionImage = null;
                    }
                    //Trace.TraceInformation("Set selection image"); //NOXLATE
                    _selectionImage = res.SelectionImage;
                    bInvalidate = true;
                }

                //If there is a queued refresh action, execute it now
                if (_queuedRefresh != null)
                {
                    //Trace.TraceInformation("Executing queued rendering operation"); //NOXLATE
                    _queuedRefresh();
                    _queuedRefresh = null;
                }
                else 
                {
                    if (bInvalidate || res.InvalidateRegardless)
                        Invalidate(true);

                    /*
                    var center = _map.ViewCenter;
                    var ext = _map.DataExtent;
                    System.Diagnostics.Trace.TraceInformation(
                        "**POST-RENDER**{2}Map Center: {0}, {1}{2}Lower left: {3}, {4}{2}Upper Right: {5}, {6}", //NOXLATE
                        center.Coordinate.X,
                        center.Coordinate.Y,
                        Environment.NewLine,
                        ext.LowerLeftCoordinate.X,
                        ext.LowerLeftCoordinate.Y,
                        ext.UpperRightCoordinate.X,
                        ext.UpperRightCoordinate.Y);
                    */
                    if (res.RaiseEvents)
                    {
                        var handler = this.MapRefreshed;
                        if (handler != null)
                            handler(this, EventArgs.Empty);
                    }
                }
            }
        }

        private bool AreWorkersBusy()
        {
            return renderWorker.IsBusy;
        }

        /// <summary>
        /// Zooms to the initial map view
        /// </summary>
        public void InitialMapView()
        {
            InitialMapView(true);
        }

        private void InitialMapView(bool refreshMap)
        {
            var scale = CalculateScale((_orgX2 - _orgX1), (_orgY1 - _orgY2), this.Width, this.Height);
            ZoomToView(_orgX1 + ((_orgX2 - _orgX1) / 2), _orgY2 + ((_orgY1 - _orgY2) / 2), scale, refreshMap);
        }

        private static Rectangle? GetRectangle(Point dPtStart, Point dPtEnd)
        {
            int? left = null;
            int? right = null;
            int? top = null;
            int? bottom = null;

            if (dPtEnd.X < dPtStart.X)
            {
                if (dPtEnd.Y < dPtStart.Y)
                {
                    left = dPtEnd.X;
                    bottom = dPtStart.Y;
                    top = dPtEnd.Y;
                    right = dPtStart.X;
                }
                else if (dPtEnd.Y > dPtStart.Y)
                {
                    left = dPtEnd.X;
                    bottom = dPtEnd.Y;
                    top = dPtStart.Y;
                    right = dPtStart.X;
                }
                else
                {
                    //Equal
                }
            }
            else
            {
                if (dPtEnd.X > dPtStart.X)
                {
                    if (dPtEnd.Y < dPtStart.Y)
                    {
                        left = dPtStart.X;
                        bottom = dPtStart.Y;
                        top = dPtEnd.Y;
                        right = dPtEnd.X;
                    }
                    else if (dPtEnd.Y > dPtStart.Y)
                    {
                        left = dPtStart.X;
                        bottom = dPtEnd.Y;
                        top = dPtStart.Y;
                        right = dPtEnd.X;
                    }
                    else
                    {
                        //Equal
                    }
                }
                //else equal
            }
            if (left.HasValue && right.HasValue && top.HasValue && bottom.HasValue)
            {
                return new Rectangle(left.Value, top.Value, (right.Value - left.Value), (bottom.Value - top.Value));
            }
            return null;
        }

        private double _zoomInFactor;
        private double _zoomOutFactor;

        /// <summary>
        /// Gets or sets the factor by which to multiply the scale to zoom in
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The zoom in factor")] //NOXLATE
        [MgComponentProperty]
        public double ZoomInFactor
        {
            get { return _zoomInFactor; }
            set
            {
                if (value.Equals(_zoomInFactor))
                    return;
                _zoomInFactor = value;
                OnPropertyChanged("ZoomInFactor"); //NOXLATE
            }
        }

        /// <summary>
        /// Gets or sets the factor by which to multiply the scale to zoom out
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("The zoom out factor")] //NOXLATE
        [MgComponentProperty]
        public double ZoomOutFactor
        {
            get { return _zoomOutFactor; }
            set
            {
                if (value.Equals(_zoomOutFactor))
                    return;
                _zoomOutFactor = value;
                OnPropertyChanged("ZoomOutFactor"); //NOXLATE
            }
        }

        private int? _lastTooltipX;
        private int? _lastTooltipY;

        private string QueryFirstVisibleTooltip(int x, int y)
        {
            //No intialized map
            if (_map == null)
                return string.Empty;

            //No change in position
            if (_lastTooltipX == x && _lastTooltipY == y && !string.IsNullOrEmpty(_activeTooltipText))
                return _activeTooltipText;

            if (_lastTooltipX.HasValue && _lastTooltipY.HasValue)
            {
                //Not considered a significant change
                if (Math.Abs(x - _lastTooltipX.Value) < MOUSE_MOVE_TOLERANCE ||
                    Math.Abs(y - _lastTooltipY.Value) < MOUSE_MOVE_TOLERANCE)
                    return _activeTooltipText;
            }

            _lastTooltipX = x;
            _lastTooltipY = y;

            var mapPt1 = ScreenToMapUnits(x - this.PointPixelBuffer, y - this.PointPixelBuffer);
            var mapPt2 = ScreenToMapUnits(x + this.PointPixelBuffer, y + this.PointPixelBuffer);
            var ringCoords = new MgCoordinateCollection();
            ringCoords.Add(_geomFact.CreateCoordinateXY(mapPt2.X, mapPt2.Y));
            ringCoords.Add(_geomFact.CreateCoordinateXY(mapPt1.X, mapPt2.Y));
            ringCoords.Add(_geomFact.CreateCoordinateXY(mapPt1.X, mapPt1.Y));
            ringCoords.Add(_geomFact.CreateCoordinateXY(mapPt2.X, mapPt1.Y));
            ringCoords.Add(_geomFact.CreateCoordinateXY(mapPt2.X, mapPt2.Y)); //Close it
            var poly = _geomFact.CreatePolygon(_geomFact.CreateLinearRing(ringCoords), new MgLinearRingCollection());

            MgTooltipQueryResult tr = _provider.QueryMapFeatures(MgQueryRequestType.Tooltip, null, poly, MgFeatureSpatialOperations.Intersects, "", 1, 5 /* has tooltips */) as MgTooltipQueryResult;
            if (tr != null)
            {
                _activeTooltipText = tr.Tooltip;
                return _activeTooltipText;
            }
            return string.Empty;
        }

        private static bool IsRasterClass(MgClassDefinition cls)
        {
            var props = cls.GetProperties();
            for (int i = 0; i < props.GetCount(); i++)
            {
                var p = props.GetItem(i);
                if (p.PropertyType == MgFeaturePropertyType.RasterProperty)
                    return true;
            }
            return false;
        }

        private static bool IsRasterLayer(MgLayerBase layer)
        {
            var cls = layer.GetClassDefinition();

            return IsRasterClass(cls);
        }

        public System.Drawing.Image GetCurrentImage()
        {
            var bmp = new Bitmap(this.Width, this.Height);
            this.DrawToBitmap(bmp, this.ClientRectangle);
            return bmp;
        }

        /// <summary>
        /// Copies the image of the current map to the clipboard
        /// </summary>
        public void CopyMap()
        {
            Clipboard.SetImage(this.GetCurrentImage());
        }

        /// <summary>
        /// Selects features from all selectable layers that intersects the given geometry
        /// </summary>
        /// <param name="geom"></param>
        public void SelectByGeometry(MgGeometry geom)
        {
            SelectByGeometry(geom, -1);
        }

        /// <summary>
        /// Selects features from all selectable layers that intersects the given geometry
        /// </summary>
        /// <param name="geom"></param>
        /// <param name="maxFeatures"></param>
        public void SelectByGeometry(MgGeometry geom, int maxFeatures)
        {
            SelectByGeometry(geom, maxFeatures, null);
        }

        /// <summary>
        /// Selects features from all selectable layers that intersects the given geometry
        /// </summary>
        /// <param name="geom"></param>
        /// <param name="maxFeatures"></param>
        /// <param name="selectionHandler"></param>
        public void SelectByGeometry(MgGeometry geom, int maxFeatures, Action<MgSelectionBase> selectionHandler)
        {
            //Don't select if dragging. This is the cause of the failure to render
            //multiple selections, which required a manual refresh afterwards
            if (isDragging)
                return;
#if TRACE
            var sw = new Stopwatch();
            sw.Start();
#endif
            MgSelectionBase newSel = null;
            //We will be either wiping or updating the existing selection set
            MgSelectionQueryResult sel = _provider.QueryMapFeatures(MgQueryRequestType.Selection, null, geom, MgFeatureSpatialOperations.Intersects, "", maxFeatures, (1 | 2)) as MgSelectionQueryResult; //1=Visible, 2=Selectable, 4=HasTooltips
            if (sel != null)
            {
                newSel = sel.Selection;
                string newXml = newSel.ToXml();
                _selection.FromXml(newXml);

                //If specified, we need to retain this for passing to the given selection handler
                if (selectionHandler == null)
                    newSel.Dispose();
            }
            else
            {
                _selection.FromXml(string.Empty);
            }
#if TRACE
            sw.Stop();
            //Trace.TraceInformation("Selection processing completed in {0}ms", sw.ElapsedMilliseconds);
#endif
            //This selection may result in nothing, so we invalidate the selection image beforehand
            if (_selectionImage != null)
            {
                _selectionImage.Dispose();
                _selectionImage = null;
            }
            RenderSelection(true); //This is either async or queued up. Either way do this before firing off selection changed
            if (selectionHandler == null)
            {
                var handler = this.SelectionChanged;
                if (handler != null)
                    handler(this, EventArgs.Empty);
            }
            else
            {
                selectionHandler(newSel);
            }
        }

        protected override void OnResize(EventArgs e)
        {
            OnControlResized(this, e);
            base.OnResize(e);
        }

        #region Mouse handlers

        private void OnMapMouseLeave(object sender, EventArgs e)
        {
            //Need to do this otherwise a tooltip query is made at the viewer boundary
            if (delayTooltipTimer != null && delayTooltipTimer.Enabled)
                delayTooltipTimer.Stop();
        }

        private void OnMapMouseDown(object sender, MouseEventArgs e)
        {
            if (IsBusy) return;
            HandleMouseDown(e);
        }

        private void OnMapMouseMove(object sender, MouseEventArgs e)
        {
            if (IsBusy) return;
            HandleMouseMove(e);
        }
        
        private void OnMapMouseUp(object sender, MouseEventArgs e)
        {
            if (IsBusy) return;
            HandleMouseUp(e);
        }

        private void OnMapMouseWheel(object sender, MouseEventArgs e)
        {
            if (IsBusy) return;
            this.Focus();
            HandleMouseWheel(e);
        }

        private void OnMapMouseClick(object sender, MouseEventArgs e)
        {
            if (IsBusy) return;
            this.Focus();
            HandleMouseClick(e);
        }

        private void OnMapMouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (IsBusy) return;
            this.Focus();
            HandleMouseDoubleClick(e);
        }

        private void HandleMouseDoubleClick(MouseEventArgs e)
        {
            //Not enough points to constitute a line string or polygon
            if (dPath.Count < 2)
                return;

            if (this.DigitizingType == MapDigitizationType.LineString)
            {
                //Fix the last one, can't edit last one because points are value types
                dPath.RemoveAt(dPath.Count - 1);
                dPath.Add(new Point(e.X, e.Y));
                OnLineStringDigitized(dPath);
            }
            else if (this.DigitizingType == MapDigitizationType.Polygon)
            {
                //Fix the last one, can't edit last one because points are value types
                dPath.RemoveAt(dPath.Count - 1);
                dPath.Add(new Point(e.X, e.Y));
                OnPolygonDigitized(dPath);
            }
        }

        private double? delayRenderScale;
        private PointF? delayRenderViewCenter;
        private float? mouseWheelSx = null;
        private float? mouseWheelSy = null;
        private float? mouseWheelTx = null;
        private float? mouseWheelTy = null;
        private int? mouseWheelDelta = null;
        private System.Timers.Timer delayRenderTimer = null;

        private void HandleMouseWheel(MouseEventArgs e)
        {
            if (!this.HasLoadedMap)
                return;

            if (delayRenderTimer == null)
            {
                delayRenderTimer = new System.Timers.Timer();
                delayRenderTimer.Enabled = false;
                delayRenderTimer.Elapsed += new System.Timers.ElapsedEventHandler(OnDelayRender);
                delayRenderTimer.Interval = this.MouseWheelDelayRenderInterval;
            }

            delayRenderTimer.Stop();
            delayRenderTimer.Start();
            //Trace.TraceInformation("Postponed delay render"); //NOXLATE
            //Trace.TraceInformation("Mouse delta: " + e.Delta + " (" + (e.Delta > 0 ? "Zoom in" : "Zoom out") + ")"); //NOXLATE
            //Negative delta = zoom out, Positive delta = zoom in
            //deltas are in units of 120, so treat each multiple of 120 as a "zoom unit"

            if (!mouseWheelSx.HasValue && !mouseWheelSy.HasValue)
            {
                mouseWheelSx = 1.0f;
                mouseWheelSy = 1.0f;
            }

            if (!mouseWheelDelta.HasValue)
                mouseWheelDelta = 0;

            if (e.Delta > 0) //Zoom In
            {
                mouseWheelDelta++;
                mouseWheelSx -= 0.1f;
                mouseWheelSy -= 0.1f;
                Invalidate();
            }
            else if (e.Delta < 0) //Zoom Out
            {
                mouseWheelDelta--;
                mouseWheelSx += 0.1f;
                mouseWheelSy += 0.1f;
                Invalidate();
            }

            //Trace.TraceInformation("Delta units is: " + mouseWheelDelta); //NOXLATE

            //Completely ripped the number crunching here from the AJAX viewer with no sense of shame whatsoever :)
            delayRenderScale = GetNewScale(_map.ViewScale, mouseWheelDelta.Value);
            double zoomChange = _map.ViewScale / delayRenderScale.Value;

            //Determine the center of the new, zoomed map, in current screen device coords
            double screenZoomCenterX = e.X - (e.X - this.Width / 2) / zoomChange;
            double screenZoomCenterY = e.Y - (e.Y - this.Height / 2) / zoomChange;
            delayRenderViewCenter = ScreenToMapUnits(screenZoomCenterX, screenZoomCenterY);

            var mpu = this.MetersPerUnit;
            var mpp = GetMetersPerPixel(_map.DisplayDpi);
            var w = (_extX2 - _extX1) * this.MetersPerUnit / (delayRenderScale * mpp);
            if (w > 20000)
            {
                w = 20000;
            }
            var h = w * ((double)this.Height / (double)this.Width);
            var xClickOffset = screenZoomCenterX - this.Width / 2;
            var yClickOffset = screenZoomCenterY - this.Height / 2;

            //Set the paint transforms. Will be reset once the delayed render is fired away
            mouseWheelTx = (float)((double)this.Width / 2 - w / 2 - xClickOffset * zoomChange);
            mouseWheelTy = (float)((double)this.Height / 2 - h / 2 - yClickOffset * zoomChange);
            mouseWheelSx = (float)(w / (double)this.Width);
            mouseWheelSy = (float)(h / (double)this.Height);

            //Trace.TraceInformation("Paint transform (tx: " + mouseWheelTx + ", ty: " + mouseWheelTy + ", sx: " + mouseWheelSx + ", sy: " + mouseWheelSy + ")"); //NOXLATE
        }

        static double GetMetersPerPixel(int dpi)
        {
            return 0.0254 / dpi;
        }

        double GetNewScale(double currentScale, int wheelZoomDelta)
        {
            var newScale = currentScale;
            /*
            //handle finite zoom scales for tiled map
            if (finscale)
            {
                var newScaleIndex = sci - wheelDelta;
                if (newScaleIndex < 0)
                {
                    newScaleIndex = 0;
                }
                if (newScaleIndex > scales.length - 1)
                {
                    newScaleIndex = scales.length - 1;
                }
                newScale = scales[newScaleIndex];
            }
            //no finite zoom scales (untiled map)
            else */
            {
                var zoomChange = Math.Pow(1.5, wheelZoomDelta);
                newScale = zoomChange > 0 ? currentScale / zoomChange : this.MaxScale;
                newScale = NormalizeScale(newScale);
            }
            return newScale;
        }

        double NormalizeScale(double scale)
        {
            if (scale < this.MinScale)
                return this.MinScale;
            if (scale > this.MaxScale)
                return this.MaxScale;
            return scale;
        }

        void OnDelayRender(object sender, System.Timers.ElapsedEventArgs e)
        {
            //Trace.TraceInformation("Delay rendering"); //NOXLATE
            //Trace.TraceInformation("Set new map coordinates to (" + delayRenderViewCenter.Value.X + ", " + delayRenderViewCenter.Value.Y + " at " + delayRenderScale.Value + ")"); //NOXLATE
            ResetMouseWheelPaintTransforms();
            MethodInvoker action = () => { ZoomToView(delayRenderViewCenter.Value.X, delayRenderViewCenter.Value.Y, delayRenderScale.Value, true); };
            if (this.InvokeRequired)
                this.Invoke(action);
            else
                action();
        }

        private void ResetMouseWheelPaintTransforms()
        {
            if (delayRenderTimer != null)
                delayRenderTimer.Stop();
            mouseWheelSx = null;
            mouseWheelSy = null;
            mouseWheelTx = null;
            mouseWheelTy = null;
            mouseWheelDelta = 0;
            //Trace.TraceInformation("Mouse wheel paint transform reset");
        }

        private void HandleMouseClick(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                return;

            if (this.DigitizingType != MapDigitizationType.None)
            {
                //Points are easy, one click and you're done
                if (this.DigitizingType == MapDigitizationType.Point)
                {
                    OnPointDigitizationCompleted(new Point(e.X, e.Y));
                }
                else
                {
                    //Check first click in digitization
                    if (_digitizationYetToStart)
                    {
                        if (this.DigitizingType == MapDigitizationType.LineString ||
                            this.DigitizingType == MapDigitizationType.Polygon)
                        {
                            dPath.Add(new Point(e.X, e.Y));
                            dPath.Add(new Point(e.X, e.Y)); //This is a transient one
                        }
                        else
                        {
                            dPtStart.X = e.X;
                            dPtStart.Y = e.Y;
                        }
                        _digitizationYetToStart = false;
                    }
                    else
                    {
                        if (this.DigitizingType == MapDigitizationType.LineString ||
                            this.DigitizingType == MapDigitizationType.Polygon)
                        {
                            var pt = dPath[dPath.Count - 1];
                            pt.X = e.X;
                            pt.Y = e.Y;
                            dPath.Add(new Point(e.X, e.Y)); //This is a transient one
                            OnLineStringSegmentDigitized(dPath[dPath.Count - 3], dPath[dPath.Count - 2]);
                        }
                        else
                        {
                            //Fortunately, these are all 2-click affairs meaning this is
                            //the second click
                            switch (this.DigitizingType)
                            {
                                case MapDigitizationType.Circle:
                                    {
                                        dPtEnd.X = e.X;
                                        dPtEnd.Y = e.Y;
                                        OnCircleDigitized(dPtStart, dPtEnd);
                                    }
                                    break;
                                case MapDigitizationType.Line:
                                    {
                                        dPtEnd.X = e.X;
                                        dPtEnd.Y = e.Y;
                                        OnLineDigitized(dPtStart, dPtEnd);
                                    }
                                    break;
                                case MapDigitizationType.Rectangle:
                                    {
                                        dPtEnd.X = e.X;
                                        dPtEnd.Y = e.Y;
                                        var rect = GetRectangle(dPtStart, dPtEnd);
                                        if (rect.HasValue)
                                            OnRectangleDigitized(rect.Value);
                                    }
                                    break;
                            }
                        }
                    }
                }
            }
            else
            {
                if (this.ActiveTool == MapActiveTool.Select)
                {
                    var mapPt1 = ScreenToMapUnits(e.X - this.PointPixelBuffer, e.Y - this.PointPixelBuffer);
                    var mapPt2 = ScreenToMapUnits(e.X + this.PointPixelBuffer, e.Y + this.PointPixelBuffer);

                    var coord1 = _geomFact.CreateCoordinateXY(mapPt1.X, mapPt1.Y);
                    var coord2 = _geomFact.CreateCoordinateXY(mapPt2.X, mapPt2.Y);

                    var dist = _mapMeasure.GetDistance(coord1, coord2);

                    MgGeometry geom = _wktRW.Read(Util.MakeWktPolygon(mapPt1.X, mapPt1.Y, mapPt2.X, mapPt2.Y));

                    SelectByGeometry(geom, 1);
                }
                else if (this.ActiveTool == MapActiveTool.ZoomIn)
                {
                    if (!isDragging)
                    {
                        var mapPt = ScreenToMapUnits(e.X, e.Y);
                        var scale = _map.ViewScale;
                        ZoomToView(mapPt.X, mapPt.Y, scale * ZoomInFactor, true);
                    }
                }
                else if (this.ActiveTool == MapActiveTool.ZoomOut)
                {
                    if (!isDragging)
                    {
                        var mapPt = ScreenToMapUnits(e.X, e.Y);
                        var scale = _map.ViewScale;
                        ZoomToView(mapPt.X, mapPt.Y, scale * ZoomOutFactor, true);
                    }
                }
            }
        }

        private void HandleMouseDown(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                dragStart = e.Location;
                //Trace.TraceInformation("Drag started at (" + dragStart.X + ", " + dragStart.Y + ")"); //NOXLATE

                switch (this.ActiveTool)
                {
                    case MapActiveTool.Pan:
                        //Trace.TraceInformation("START PANNING"); //NOXLATE
                        break;
                    case MapActiveTool.Select:
                        //Trace.TraceInformation("START SELECT"); //NOXLATE
                        break;
                    case MapActiveTool.ZoomIn:
                        //Trace.TraceInformation("START ZOOM"); //NOXLATE
                        break;
                }
            }
        }

        private System.Drawing.Point translate;

        private System.Drawing.Point dragStart;
        bool isDragging = false;
        
        private int _mouseX;
        private int _mouseY;

        private string _activeTooltipText;

        private int _mouseDx;
        private int _mouseDy;

        /// <summary>
        /// A mouse is considered to have moved if the differerence in either X or Y directions is greater
        /// than this number
        /// </summary>
        const int MOUSE_MOVE_TOLERANCE = 2;

        private void HandleMouseMove(MouseEventArgs e)
        {
            if (_mouseX == e.X &&
                _mouseY == e.Y)
            {
                return;
            }

            //Record displacement
            _mouseDx = e.X - _mouseX;
            _mouseDy = e.Y - _mouseY;

            _mouseX = e.X;
            _mouseY = e.Y;

            var mapPt = ScreenToMapUnits(e.X, e.Y);
            OnMouseMapPositionChanged(mapPt.X, mapPt.Y);

            if (this.ActiveTool == MapActiveTool.Pan || this.ActiveTool == MapActiveTool.Select || this.ActiveTool == MapActiveTool.ZoomIn)
            {
                if (e.Location != dragStart && !isDragging && e.Button == MouseButtons.Left)
                {
                    isDragging = true;
                }

                if (this.ActiveTool == MapActiveTool.Pan)
                {
                    if (isDragging)
                    {
                        translate = new System.Drawing.Point(e.X - dragStart.X, e.Y - dragStart.Y);
                    }
                }

                this.TooltipsEnabled = !isDragging && this.FeatureTooltipsEnabled;

                //Only query for tooltips if not digitizing
                if (this.DigitizingType == MapDigitizationType.None &&
                   (this.ActiveTool == MapActiveTool.Select || this.ActiveTool == MapActiveTool.Pan) &&
                    this.TooltipsEnabled)
                {
                    if (delayTooltipTimer == null)
                    {
                        delayTooltipTimer = new System.Timers.Timer();
                        delayTooltipTimer.Enabled = false;
                        delayTooltipTimer.Elapsed += new System.Timers.ElapsedEventHandler(OnDelayTooltipTimerElapsed);
                        delayTooltipTimer.Interval = this.TooltipDelayInterval;
                    }

                    _delayTooltipQueryPoint = new Point(e.X, e.Y);
                    delayTooltipTimer.Start();

                    if (Math.Abs(e.X - _lastTooltipQueryX) > 2 || Math.Abs(e.Y - _lastTooltipQueryY) > 2)
                    {
                        _activeTooltipText = null;
                        Invalidate();
                    }
                }
                else
                {
                    _activeTooltipText = null;
                }

                if (e.Button == MouseButtons.Left || !string.IsNullOrEmpty(_activeTooltipText))
                    Invalidate();
            }
            else if (this.DigitizingType != MapDigitizationType.None)
            {
                if (dPath.Count >= 2)
                {
                    //Fix the last one, can't edit last one because points are value types
                    dPath.RemoveAt(dPath.Count - 1);
                    dPath.Add(new Point(e.X, e.Y));
                    //Trace.TraceInformation("Updating last point of a {0} point path", dPath.Count); //NOXLATE
                }
                Invalidate();
            }
        }

        void OnDelayTooltipTimerElapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            delayTooltipTimer.Stop();
            if (_delayTooltipQueryPoint.HasValue)
            {
                _activeTooltipText = QueryFirstVisibleTooltip(_delayTooltipQueryPoint.Value.X, _delayTooltipQueryPoint.Value.Y);
                _lastTooltipQueryX = _delayTooltipQueryPoint.Value.X;
                _lastTooltipQueryY = _delayTooltipQueryPoint.Value.Y;
                _delayTooltipQueryPoint = null;
                Invalidate();
            }
        }

        private int _lastTooltipQueryX;
        private int _lastTooltipQueryY;
        private Point? _delayTooltipQueryPoint = null;
        private System.Timers.Timer delayTooltipTimer = null;

        private void HandleMouseUp(MouseEventArgs e)
        {
            if (isDragging)
            {
                isDragging = false;

                if (this.ActiveTool == MapActiveTool.Pan)
                {
                    //FIXME: This is not perfect. The view will be slightly off of where you released the mouse button

                    //System.Diagnostics.Trace.TraceInformation("Dragged screen distance (" + translate.X + ", " + translate.Y + ")"); //NOXLATE

                    int dx = e.X - dragStart.X;
                    int dy = e.Y - dragStart.Y;

                    var centerScreen = new Point(this.Location.X + (this.Width / 2), this.Location.Y + (this.Height / 2));

                    centerScreen.X -= translate.X;
                    centerScreen.Y -= translate.Y;

                    var pt = _map.ViewCenter.Coordinate;
                    var coord = ScreenToMapUnits(centerScreen.X, centerScreen.Y);

                    double mdx = coord.X - pt.X;
                    double mdy = coord.Y - pt.Y;

                    ZoomToView(coord.X, coord.Y, _map.ViewScale, true);
                    //Trace.TraceInformation("END PANNING"); //NOXLATE
                }
                else if (this.ActiveTool == MapActiveTool.Select)
                {
                    var mapPt = ScreenToMapUnits(e.X, e.Y);
                    var mapDragPt = ScreenToMapUnits(dragStart.X, dragStart.Y);
                    var ringCoords = new MgCoordinateCollection();
                    ringCoords.Add(_geomFact.CreateCoordinateXY(mapDragPt.X, mapDragPt.Y));
                    ringCoords.Add(_geomFact.CreateCoordinateXY(mapPt.X, mapDragPt.Y));
                    ringCoords.Add(_geomFact.CreateCoordinateXY(mapPt.X, mapPt.Y));
                    ringCoords.Add(_geomFact.CreateCoordinateXY(mapDragPt.X, mapPt.Y));
                    ringCoords.Add(_geomFact.CreateCoordinateXY(mapDragPt.X, mapDragPt.Y)); //Close it
                    var poly = _geomFact.CreatePolygon(_geomFact.CreateLinearRing(ringCoords), new MgLinearRingCollection());

                    SelectByGeometry(poly);
                }
                else if (this.ActiveTool == MapActiveTool.ZoomIn)
                {
                    var mapPt = ScreenToMapUnits(e.X, e.Y);
                    var mapDragPt = ScreenToMapUnits(dragStart.X, dragStart.Y);

                    PointF ll;
                    PointF ur;

                    if (mapPt.X <= mapDragPt.X && mapPt.Y <= mapDragPt.Y)
                    {
                        ll = mapPt;
                        ur = mapDragPt;
                    }
                    else
                    {
                        ll = mapDragPt;
                        ur = mapPt;
                    }

                    ZoomToExtents(ll.X, ll.Y, ur.X, ur.Y);
                }
            }
        }

        private void OnMouseMapPositionChanged(double x, double y)
        {
            var handler = this.MouseMapPositionChanged;
            if (handler != null)
                handler(this, new MapPointEventArgs(x, y));
        }

        /// <summary>
        /// Raised when the map cursor position has changed
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Raised when the map position as indicated by the current mouse pointer has changed")] //NOXLATE
        public event EventHandler<MapPointEventArgs> MouseMapPositionChanged;

        #endregion

        private MapActiveTool _tool;

        /// <summary>
        /// Gets or sets the active tool
        /// </summary>
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [MgComponentProperty]
        public MapActiveTool ActiveTool
        {
            get
            {
                return _tool;
            }
            set
            {
                if (_tool.Equals(value))
                    return;

                _tool = value;
                switch (value)
                {
                    case MapActiveTool.Pan:
                        using (var ms = new MemoryStream(Properties.Resources.grab))
                        {
                            this.Cursor = new Cursor(ms);
                        }
                        break;
                    case MapActiveTool.ZoomIn:
                        using (var ms = new MemoryStream(Properties.Resources.zoomin))
                        {
                            this.Cursor = new Cursor(ms);
                        }
                        break;
                    case MapActiveTool.ZoomOut:
                        using (var ms = new MemoryStream(Properties.Resources.zoomout))
                        {
                            this.Cursor = new Cursor(ms);
                        }
                        break;
                    case MapActiveTool.None:
                    case MapActiveTool.Select:
                        {
                            this.Cursor = Cursors.Default;
                        }
                        break;
                }

                //Clear to prevent stray tooltips from being rendered
                if (value != MapActiveTool.Select &&
                    value != MapActiveTool.Pan)
                {
                    _activeTooltipText = null;
                }

                if (value != MapActiveTool.None)
                    this.DigitizingType = MapDigitizationType.None;

                OnPropertyChanged("ActiveTool"); //NOXLATE
            }
        }

        /// <summary>
        /// Screens to map units.
        /// </summary>
        /// <param name="x">The x.</param>
        /// <param name="y">The y.</param>
        /// <returns></returns>
        public PointF ScreenToMapUnits(double x, double y)
        {
            return ScreenToMapUnits(x, y, false);
        }

        private PointF ScreenToMapUnits(double sx, double sy, bool allowOutsideWindow)
        {
            if (!allowOutsideWindow)
            {
                if (sx > this.Width - 1) sx = this.Width - 1;
                else if (sx < 0) sx = 0;

                if (sy > this.Height - 1) sy = this.Height - 1;
                else if (sy < 0) sy = 0;
            }

            var mx = _extX1 + ((_extX2 - _extX1) * (sx / this.Width));
            var my = _extY1 - ((_extY1 - _extY2) * (sy / this.Height));
            return new PointF((float)mx, (float)my);
        }

        /// <summary>
        /// Converts the specified map coordinates to screen coordinates
        /// </summary>
        /// <param name="mx"></param>
        /// <param name="my"></param>
        /// <returns></returns>
        public Point MapToScreenUnits(double mx, double my)
        {
            //Equation derived from high-school mathematical inversion of the 
            //ScreenToMapUnits formula :) Tests thus far indicate this formula
            //is correct.
            var sx = this.Width * ((mx - _extX1) / (_extX2 - _extX1));
            var sy = this.Height * ((my - _extY1) / (_extY2 - _extY1));
            return new Point(Convert.ToInt32(Math.Round(sx)), Convert.ToInt32(Math.Round(sy)));
        }

        /// <summary>
        /// Occurs when a property value changes.
        /// </summary>
        [Category("MapGuide Viewer")] //NOXLATE
        [Description("Raised when a public property of this component has changed")] //NOXLATE
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Called when a public property has changed
        /// </summary>
        /// <param name="name">The name.</param>
        protected void OnPropertyChanged(string name)
        {
            var handler = this.PropertyChanged;
            if (handler != null)
                handler(this, new PropertyChangedEventArgs(name));
        }

        public bool HasLoadedMap { get { return _map != null; } }

        int IMapViewer.ControlHeight { get { return this.Height; } }

        int IMapViewer.ControlWidth { get { return this.Width; } }

        public event PaintEventHandler PreMapRender;

        public event PaintEventHandler PostMapRender;

        private Dictionary<string, PropertyInfo> _properties;

        public IEnumerable<PropertyInfo> ComponentProperties
        {
            get
            {
                CheckAndInitProperties();
                return _properties.Values;
            }
        }

        private void CheckAndInitProperties()
        {
            if (_properties == null)
            {
                _properties = new Dictionary<string, PropertyInfo>();
                var props = this.GetType().GetProperties();
                foreach (var p in props)
                {
                    var attributes = p.GetCustomAttributes(true);
                    foreach (var att in attributes)
                    {
                        var compAttr = att as MgComponentPropertyAttribute;
                        if (compAttr != null)
                        {
                            _properties[p.Name] = p;
                            break;
                        }
                    }
                }
            }
        }

        public void SetPropertyValue(string propertyName, object value)
        {
            CheckAndInitProperties();
            if (!_properties.ContainsKey(propertyName))
                throw new InvalidOperationException(string.Format(Strings.ErrorInvalidComponentProperty, propertyName));

            var prop = _properties[propertyName];
            prop.SetValue(this, Convert.ChangeType(value, prop.PropertyType), null);
        }

        public object GetPropertyValue(string propertyName)
        {
            CheckAndInitProperties();
            if (!_properties.ContainsKey(propertyName))
                throw new InvalidOperationException(string.Format(Strings.ErrorInvalidComponentProperty, propertyName));

            return _properties[propertyName].GetValue(propertyName, null);
        }
    }
}
