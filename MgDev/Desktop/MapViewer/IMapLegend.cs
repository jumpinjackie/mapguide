using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A legend control that displays and controls the visiblity of the layers in a runtime map
    /// </summary>
    public interface IMapLegend : IMapViewerComponent
    {
        /// <summary>
        /// Refreshes this component
        /// </summary>
        void RefreshLegend();

        /// <summary>
        /// Sets the applicable scale
        /// </summary>
        /// <param name="scale"></param>
        void SetScale(double scale);

        /// <summary>
        /// Raised when a layer's visibility has changed
        /// </summary>
        event EventHandler VisibilityChanged;

        /// <summary>
        /// Gets or sets the context menu that is attached to group nodes
        /// </summary>
        ContextMenuStrip GroupContextMenu { get; set; }

        /// <summary>
        /// Gets or sets the context menu that is attached to layer nodes
        /// </summary>
        ContextMenuStrip LayerContextMenu { get; set; }

        /// <summary>
        /// Gets or sets the theme compression limit (the number of rules a theme must exceed in order to be
        /// compressed)
        /// </summary>
        int ThemeCompressionLimit { get; set; }

        /// <summary>
        /// Gets whether to show tooltips over nodes in the legend control
        /// </summary>
        bool ShowTooltips { get; set; }

        /// <summary>
        /// Gets the selected layer
        /// </summary>
        /// <returns></returns>
        MgLayerBase GetSelectedLayer();

        /// <summary>
        /// Gets the selected group
        /// </summary>
        /// <returns></returns>
        MgLayerGroup GetSelectedGroup();
    }
}
