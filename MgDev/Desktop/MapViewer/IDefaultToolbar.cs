using System;
using System.Collections.Generic;
using System.Text;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// A toolbar that contains a default set of viewer commands
    /// </summary>
    public interface IDefaultToolbar
    {
        /// <summary>
        /// Gets or sets the viewer this toolbar is associated with
        /// </summary>
        IMapViewer Viewer { get; set; }
    }
}
