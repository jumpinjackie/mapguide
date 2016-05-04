using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer.AppLayoutEngine
{
    /// <summary>
    /// A component that exits the application when invoked
    /// </summary>
    [ToolboxItem(true)]
    public class MgQuitComponent : MgComponent
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public MgQuitComponent()
        {
            this.Label = Strings.TextExitApplication;
        }

        /// <summary>
        /// Invokes this component
        /// </summary>
        public override void Invoke()
        {
            System.Windows.Forms.Application.Exit();
        }
    }
}
