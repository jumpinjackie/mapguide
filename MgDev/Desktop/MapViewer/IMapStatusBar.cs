using System;
using System.Collections.Generic;
using System.Text;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// Represents a component that can receive viewer status messages
    /// </summary>
    public interface IMapStatusBar : IMapViewerComponent
    {
        /// <summary>
        /// Sets the message for the current cursor position
        /// </summary>
        /// <param name="message"></param>
        void SetCursorPositionMessage(string message);

        /// <summary>
        /// Sets the message for the number of features selected
        /// </summary>
        /// <param name="message"></param>
        void SetFeatureSelectedMessage(string message);

        /// <summary>
        /// Sets the message for the map scale
        /// </summary>
        /// <param name="message"></param>
        void SetMapScaleMessage(string message);

        /// <summary>
        /// Sets the message for the map size
        /// </summary>
        /// <param name="message"></param>
        void SetMapSizeMessage(string message);
    }
}
