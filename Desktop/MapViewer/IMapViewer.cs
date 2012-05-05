using System;
using System.Collections.Generic;
using System.Text;
using OSGeo.MapGuide;
using System.Drawing;
using System.ComponentModel;

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
    public interface IMapViewer : IMapViewerComponent, INotifyPropertyChanged
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
        void UpdateSelection();

        /// <summary>
        /// Updates the rendered selection. Call this method if you have manipulated the selection
        /// set outside of the viewer
        /// </summary>
        /// <param name="raise">Indicates if the <see cref="SelectionChanged"/> event should be raised as well</param>
        void UpdateSelection(bool raise);

        /// <summary>
        /// Selects features from all selectable layers that intersects the given geometry
        /// </summary>
        /// <param name="geom"></param>
        void SelectByGeometry(MgGeometry geom);

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
        event EventHandler ViewerInitialized;

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
        /// implemented
        /// </summary>
        bool ConvertTiledGroupsToNonTiled { get; set; }
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
