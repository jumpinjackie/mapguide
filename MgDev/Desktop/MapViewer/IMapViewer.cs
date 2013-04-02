using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide;
using System.Drawing;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Windows.Forms;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A callback for circle digitization
    /// </summary>
    /// <param name="x">The X coordinate of the circle's center</param>
    /// <param name="y">The Y coordinate of the circle's center</param>
    /// <param name="radius">The circle's radius</param>
    public delegate void CircleDigitizationCallback(double x, double y, double radius);

    /// <summary>
    /// A callback for line digitization
    /// </summary>
    /// <param name="x1">The X coordinate of the line's first point</param>
    /// <param name="y1">The Y coordinate of the line's first point</param>
    /// <param name="x2">The X coordinate of the line's second point</param>
    /// <param name="y2">The Y coordinate of the line's second point</param>
    public delegate void LineDigitizationCallback(double x1, double y1, double x2, double y2);

    /// <summary>
    /// A callback for point digitization
    /// </summary>
    /// <param name="x">The X coordinate of the point</param>
    /// <param name="y">The Y coordinate of the point</param>
    public delegate void PointDigitizationCallback(double x, double y);

    /// <summary>
    /// A callback for polygon digitization
    /// </summary>
    /// <param name="coordinates">A n by 2 array of polygon coordinates, where n is the number of vertices</param>
    public delegate void PolygonDigitizationCallback(double[,] coordinates);

    /// <summary>
    /// A callback for line string digitization
    /// </summary>
    /// <param name="coordinates">A n by 2 array of line string coordinates, where n is the number of vertices</param>
    public delegate void LineStringDigitizationCallback(double[,] coordinates);

    /// <summary>
    /// A callback for rectangle digitization
    /// </summary>
    /// <param name="llx">The X coordinate of the rectangle's lower left point</param>
    /// <param name="lly">The Y coordinate of the rectangle's lower left point</param>
    /// <param name="urx">The X coordinate of the rectangle's upper right point</param>
    /// <param name="ury">The Y coordinate of the rectangle's upper right point</param>
    public delegate void RectangleDigitizationCallback(double llx, double lly, double urx, double ury);

    /// <summary>
    /// A map viewer component
    /// </summary>
    public interface IMapViewer : IMapViewerComponent, INotifyPropertyChanged, IMapComponent
    {
        /// <summary>
        /// Clears the current selection
        /// </summary>
        void ClearSelection();

        /// <summary>
        /// Starts the digitization process for a circle
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        void DigitizeCircle(CircleDigitizationCallback callback);

        /// <summary>
        /// Starts the digitization process for a line
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        void DigitizeLine(LineDigitizationCallback callback);

        /// <summary>
        /// Starts the digitization process for a point
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        void DigitizePoint(PointDigitizationCallback callback);

        /// <summary>
        /// Starts the digitization process for a polygon
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        void DigitizePolygon(PolygonDigitizationCallback callback);

        /// <summary>
        /// Starts the digitization process for a line string (polyline)
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        void DigitizeLineString(LineStringDigitizationCallback callback);

        /// <summary>
        /// Starts the digitization process for a line string (polyline)
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="segmentDigitized">The callback to be invoked when a new segment of the current line string is digitized</param>
        void DigitizeLineString(LineStringDigitizationCallback callback, LineDigitizationCallback segmentDigitized);

        /// <summary>
        /// Starts the digitization process for a rectangle
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        void DigitizeRectangle(RectangleDigitizationCallback callback);

        /// <summary>
        /// Starts the digitization process for a circle
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        void DigitizeCircle(CircleDigitizationCallback callback, string customPrompt);

        /// <summary>
        /// Starts the digitization process for a line
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        void DigitizeLine(LineDigitizationCallback callback, string customPrompt);

        /// <summary>
        /// Starts the digitization process for a point
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        void DigitizePoint(PointDigitizationCallback callback, string customPrompt);

        /// <summary>
        /// Starts the digitization process for a polygon
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        void DigitizePolygon(PolygonDigitizationCallback callback, string customPrompt);

        /// <summary>
        /// Starts the digitization process for a line string (polyline)
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        void DigitizeLineString(LineStringDigitizationCallback callback, string customPrompt);

        /// <summary>
        /// Starts the digitization process for a line string (polyline)
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="segmentDigitized">The callback to be invoked when a new segment of the current line string is digitized</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        void DigitizeLineString(LineStringDigitizationCallback callback, LineDigitizationCallback segmentDigitized, string customPrompt);

        /// <summary>
        /// Starts the digitization process for a rectangle
        /// </summary>
        /// <param name="callback">The callback to be invoked when the digitization process completes</param>
        /// <param name="customPrompt">The custom prompt to use for the tracking tooltip</param>
        void DigitizeRectangle(RectangleDigitizationCallback callback, string customPrompt);

        /// <summary>
        /// Gets the current runtime map
        /// </summary>
        /// <returns></returns>
        MgMapBase GetMap();

        /// <summary>
        /// Gets the selection set of the runtime map
        /// </summary>
        /// <returns></returns>
        MgSelectionBase GetSelection();

        /// <summary>
        /// Gets the coordinate system of the runtime map
        /// </summary>
        MgCoordinateSystem CoordinateSystem { get; }

        /// <summary>
        /// Gets or sets the color used to render selected features
        /// </summary>
        Color SelectionColor { get; set; } 

        /// <summary>
        /// Gets or sets the active tool
        /// </summary>
        MapActiveTool ActiveTool { get; set; }

        /// <summary>
        /// Gets the map viewer provider for this control
        /// </summary>
        /// <returns></returns>
        MgMapViewerProvider GetProvider();

        /// <summary>
        /// Gets or sets the minimum allowed zoom scale for this viewer
        /// </summary>
        int MinScale { get; set; }

        /// <summary>
        /// Gets or sets the maximum allowed zoom scale for this viewer
        /// </summary>
        int MaxScale { get; set; }

        /// <summary>
        /// The amount of time (in ms) to wait to re-render after a mouse wheel scroll
        /// </summary>
        int MouseWheelDelayRenderInterval { get; set; }

        /// <summary>
        /// Gets or sets the factor by which to multiply the scale to zoom in
        /// </summary>
        double ZoomInFactor { get; set; }
        
        /// <summary>
        /// Gets or sets the factor by which to multiply the scale to zoom out
        /// </summary>
        double ZoomOutFactor { get; set; }

        /// <summary>
        /// Gets or sets whether feature tooltips are enabled. If set to true, tooltip queries are
        /// executed at the current mouse position if the active tool is Pan or Select
        /// </summary>
        bool FeatureTooltipsEnabled { get; set; }

        /// <summary>
        /// Gets whether the viewer has any active rendering operations
        /// </summary>
        bool IsBusy { get; }

        /// <summary>
        /// Gets the type of object being currently digitized. If the digitization type is None, then
        /// the viewer is not currently digitizing
        /// </summary>
        MapDigitizationType DigitizingType { get; }

        /// <summary>
        /// Gets the currently rendered image
        /// </summary>
        /// <returns></returns>
        System.Drawing.Image GetCurrentImage();

        /// <summary>
        /// Copies the image of the current map to the clipboard
        /// </summary>
        void CopyMap();

        /// <summary>
        /// Refreshes the current map view
        /// </summary>
        void RefreshMap();

        /// <summary>
        /// Raised when the map has been refreshed
        /// </summary>
        event EventHandler MapRefreshed;

        /// <summary>
        /// Pans the view left by a pre-defined distance
        /// </summary>
        /// <param name="refresh"></param>
        void PanLeft(bool refresh);
        
        /// <summary>
        /// Pans the view up by a pre-defined distance
        /// </summary>
        /// <param name="refresh"></param>
        void PanUp(bool refresh);

        /// <summary>
        /// Pans the view right by a pre-defined distance
        /// </summary>
        /// <param name="refresh"></param>
        void PanRight(bool refresh);

        /// <summary>
        /// Pans the view down by a pre-defined distance
        /// </summary>
        /// <param name="refresh"></param>
        void PanDown(bool refresh);

        /// <summary>
        /// Updates the rendered selection. Call this method if you have manipulated the selection
        /// set outside of the viewer
        /// </summary>
        /// <remarks>
        /// If you have modified the selection as a result of calling <see cref="M:OSGeo.MapGuide.Viewer.IMapViewer.SelectByGeometry"/>, calling
        /// this method is not necessary as it will have automatically do this.
        /// </remarks>
        void UpdateSelection();

        /// <summary>
        /// Updates the rendered selection. Call this method if you have manipulated the selection
        /// set outside of the viewer
        /// </summary>
        /// <param name="raise">Indicates if the <see cref="SelectionChanged"/> event should be raised as well</param>
        /// <remarks>
        /// If you have modified the selection as a result of calling <see cref="M:OSGeo.MapGuide.Viewer.IMapViewer.SelectByGeometry"/>, calling
        /// this method is not necessary as it will have automatically do this.
        /// </remarks>
        void UpdateSelection(bool raise);

        /// <summary>
        /// Selects features from all selectable layers that intersects the given geometry
        /// </summary>
        /// <param name="geom">The geometry to perform intersection tests against</param>
        /// <remarks>
        /// This method will automatically trigger selection updates. Calling <see cref="M:OSGeo.MapGuide.Viewer.IMapViewer.UpdateSelection"/> is not necessary if
        /// you are calling this method
        /// </remarks>
        void SelectByGeometry(MgGeometry geom);

        /// <summary>
        /// Selects features from all selectable layers that intersects the given geometry up to the specified number
        /// </summary>
        /// <param name="geom">The geometry to perform intersection tests against</param>
        /// <param name="maxFeatures">The maximum number of features to select. Specify -1 for all features</param>
        /// <remarks>
        /// This method will automatically trigger selection updates. Calling <see cref="M:OSGeo.MapGuide.Viewer.IMapViewer.UpdateSelection"/> is not necessary if
        /// you are calling this method
        /// </remarks>
        void SelectByGeometry(MgGeometry geom, int maxFeatures);

        /// <summary>
        /// Selects features from all selectable layers that intersects the given geometry up to the specified number
        /// </summary>
        /// <param name="geom">The geometry to perform intersection tests against</param>
        /// <param name="maxFeatures">The maximum number of features to select. Specify -1 for all features</param>
        /// <param name="selectionHandler">An optional handler method that is invoked upon change of selection</param>
        /// <remarks>
        /// If a selection handler is passed to this method two things will happen:
        /// <list type="number">
        ///     <item>The <see cref="E:OSGeo.MapGuide.Viewer.IMapViewer.SelectionChanged"/> event is not raised upon change of selection. Existing subscribers will not be notified of any selection change caused by this method</item>
        ///     <item>The handler will be called with a <see cref="T:OSGeo.MapGuide.MgSelectionBase"/> object passed to it. If this query results in nothing selected, null is passed to this handler</item>
        /// </list>
        /// The handler is responsible for the selection given to it. You may call <see cref="M:OSGeo.MapGuide.MgSelectionBase.Dispose()"/> on this selection if desired when your handler is done with it
        /// </remarks>
        void SelectByGeometry(MgGeometry geom, int maxFeatures, Action<MgSelectionBase> selectionHandler);

        /// <summary>
        /// Zooms to the initial map view
        /// </summary>
        void InitialMapView();

        /// <summary>
        /// Zooms to the specified map view
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="scale"></param>
        /// <param name="refresh"></param>
        void ZoomToView(double x, double y, double scale, bool refresh);

        /// <summary>
        /// Raised when the scale of the current runtime map has changed
        /// </summary>
        event EventHandler MapScaleChanged;

        /// <summary>
        /// Raised when the selection has changed. Note that programmatic selection modifications
        /// will not raise this event.
        /// </summary>
        event EventHandler SelectionChanged;

        /// <summary>
        /// Raised when the viewer has been initialized
        /// </summary>
        event EventHandler MapLoaded;

        /// <summary>
        /// Raised when the map cursor position has changed
        /// </summary>
        event EventHandler<MapPointEventArgs> MouseMapPositionChanged;

        /// <summary>
        /// Zooms to the view defined by the specified extent
        /// </summary>
        /// <param name="llx"></param>
        /// <param name="lly"></param>
        /// <param name="urx"></param>
        /// <param name="ury"></param>
        void ZoomToExtents(double llx, double lly, double urx, double ury);

        /// <summary>
        /// Gets or sets whether to show vertex coordinates when digitizing
        /// </summary>
        bool ShowVertexCoordinatesWhenDigitizing { get; set; }

        /// <summary>
        /// Gets or sets whether to convert tiled layers to non-tiled layers. This is a workaround
        /// setting for tiled maps to be displayed as viewer support for tiled layers is still not
        /// implemented.
        /// 
        /// If <see cref="P:OSGeo.MapGuide.Viewer.IMapViewer.UseRenderMapIfTiledLayersExist"/> is set
        /// to true, this property has no effect.
        /// </summary>
        /// <remarks>
        /// As of 2.5, images from RenderDynamicOverlay will include any base layers by default. Using this property
        /// as a workaround is no longer required. This property remains for compatibility purposes.
        /// </remarks>
        bool ConvertTiledGroupsToNonTiled { get; set; }

        /// <summary>
        /// Gets whether to use the RenderMap API instead of RenderDynamicOverlay if the map has tiled
        /// layers. RenderMap includes tiled layers as part of the output image, but will not take advantage
        /// of any tile caching mechanisms. Setting this property to true nullifies any effect of the 
        /// <see cref="P:OSGeo.MapGuide.Viewer.IMapViewer.ConvertTiledGroupsToNonTiled"/> property
        /// </summary>
        /// <remarks>
        /// As of 2.5, images from RenderDynamicOverlay will include any base layers by default. Setting this property to true is no longer required
        /// unless you still require map images to be rendered with a pre-filled background (which RenderMap() does). This property remains
        /// for compatibility purposes.
        /// </remarks>
        bool UseRenderMapIfTiledLayersExist { get; set; }

        /// <summary>
        /// Gets whether to respect the list of finite display scales in a map being viewed if there are any defined.
        /// If true, all zooms will "snap" to the nearest finite display scale
        /// </summary>
        bool RespectFiniteDisplayScales { get; set; }

        /// <summary>
        /// Gets whether this viewer has a map loaded into it
        /// </summary>
        bool HasLoadedMap { get; }

        /// <summary>
        /// Gets or sets the amount of pixels to buffer out by when doing point-based selections with the Select tool
        /// </summary>
        int PointPixelBuffer { get; set; }

        /// <summary>
        /// Navigates to the previous view in the history stack
        /// </summary>
        void PreviousView();

        /// <summary>
        /// Navigates to the next view in the history stack
        /// </summary>
        void NextView();

        /// <summary>
        /// Gets the current index in the view history stack
        /// </summary>
        int ViewHistoryIndex { get; }

        /// <summary>
        /// Gets the view history stack. The first item being the earliest and the last item being the most recent.
        /// </summary>
        ReadOnlyCollection<MgMapViewHistoryEntry> ViewHistory { get; }

        /// <summary>
        /// Cancels the active digitization process. Does nothing if <see cref="DigitizingType"/> is MapDigitizationType.None
        /// </summary>
        void CancelDigitization();

        /// <summary>
        /// Raised before map rendering begins in the control's Paint method. This allows you to do custom rendering before the
        /// map image is rendered. Depending on whatever map/layer transparency settings, content rendered by your handler may
        /// be obscured by the map image that is rendered afterwards. If you need to do custom rendering on top of a rendered map
        /// image, consider doing the rendering on the <see cref="E:OSGeo.MapGuide.Viewer.IMapViewer.PostMapRender"/> event
        /// instead.
        /// </summary>
        /// <remarks>
        /// The <see cref="T:System.Drawing.Graphics"/> object attached to the <see cref="T:System.Windows.Forms.PaintEventArgs"/>
        /// that is passed to your handler will already have any scale/translate transforms applied as a result of user panning or
        /// transitional zooming.
        /// 
        /// Also note that any such custom rendrered content will not appear in any custom rendering or plotting output through
        /// MapGuide's APIs as it has no knowledge of the your custom rendered content here.
        /// </remarks>
        event PaintEventHandler PreMapRender;

        /// <summary>
        /// Raised after map render has completed in the control's Paint method. This allows you to do custom rendering after the
        /// map image is rendered
        /// </summary>
        /// <remarks>
        /// The <see cref="T:System.Drawing.Graphics"/> object attached to the <see cref="T:System.Windows.Forms.PaintEventArgs"/>
        /// that is passed to your handler will already have any scale/translate transforms applied as a result of user panning or
        /// transitional zooming.
        /// 
        /// Also note that any such custom rendrered content will not appear in any custom rendering or plotting output through
        /// MapGuide's APIs as it has no knowledge of the your custom rendered content here.
        /// </remarks>
        event PaintEventHandler PostMapRender;

        /// <summary>
        /// Gets the width of this control
        /// </summary>
        int ControlWidth { get; }

        /// <summary>
        /// Gets the height of this control
        /// </summary>
        int ControlHeight { get; }

        /// <summary>
        /// Converts the given coordinate in screen units to map units
        /// </summary>
        /// <param name="x">The x coordinate in screen space</param>
        /// <param name="y">The y coordinate in screen space</param>
        /// <returns></returns>
        PointF ScreenToMapUnits(double x, double y);

        /// <summary>
        /// Converts the given coordinate in map-space to coordinates in screen space. This is the inverse of
        /// <see cref="M:OSGeo.MapGuide.Viewer.IMapViewer.ScreenToMapUnits" />
        /// </summary>
        /// <param name="x">The x coordinate in map space</param>
        /// <param name="y">The y coordinate in map space</param>
        /// <returns></returns>
        Point MapToScreenUnits(double x, double y);
    }

    /// <summary>
    /// Defines a map view location already visited
    /// </summary>
    public class MgMapViewHistoryEntry
    {
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="scale"></param>
        public MgMapViewHistoryEntry(double x, double y, double scale)
        {
            this.X = x;
            this.Y = y;
            this.Scale = scale;
        }

        /// <summary>
        /// The view location X coordinate
        /// </summary>
        public double X { get; private set; }

        /// <summary>
        /// The view location Y coordinate
        /// </summary>
        public double Y { get; private set; }

        /// <summary>
        /// The view scale
        /// </summary>
        public double Scale { get; private set; }
    }

    /// <summary>
    /// Contains data of a MouseMapPositionChanged event
    /// </summary>
    public class MapPointEventArgs : EventArgs 
    {
        /// <summary>
        /// Gets the X coordinate 
        /// </summary>
        public readonly double X;

        /// <summary>
        /// Gets the Y coordinate
        /// </summary>
        public readonly double Y;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        public MapPointEventArgs(double x, double y)
        {
            this.X = x; 
            this.Y = y;
        }
    }

    /// <summary>
    /// The type of digitization in progress
    /// </summary>
    public enum MapDigitizationType
    {
        /// <summary>
        /// No digitization in progress
        /// </summary>
        None,
        /// <summary>
        /// A point digitization is in progress
        /// </summary>
        Point,
        /// <summary>
        /// A line digitization is in progress
        /// </summary>
        Line,
        /// <summary>
        /// A line string digitization is in progress
        /// </summary>
        LineString,
        /// <summary>
        /// A rectangle digitization is in progress
        /// </summary>
        Rectangle,
        /// <summary>
        /// A polygon digitization is in progress
        /// </summary>
        Polygon,
        /// <summary>
        /// A circle digitization is in progress
        /// </summary>
        Circle
    }

    /// <summary>
    /// The active viewer tool
    /// </summary>
    public enum MapActiveTool
    {
        /// <summary>
        /// Zoom In command
        /// </summary>
        ZoomIn,
        /// <summary>
        /// Zoom Out command
        /// </summary>
        ZoomOut,
        /// <summary>
        /// Pan command
        /// </summary>
        Pan,
        /// <summary>
        /// Select command
        /// </summary>
        Select,
        /// <summary>
        /// No active command
        /// </summary>
        None
    }
}
