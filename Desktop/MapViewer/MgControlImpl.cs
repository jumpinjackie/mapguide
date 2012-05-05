using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// Base class for views show by their associated <see cref="T:OSGeo.MapGuide.Viewer.MgViewerComponent"/> instances
    /// </summary>
    [ToolboxItem(false)]
    public class MgControlImpl : UserControl
    {
        /// <summary>
        /// Gets or set the title to show if this view is shown in a new window
        /// </summary>
        public virtual string Title { get; set; }

        protected MgControlImpl() { this.ModalWindow = false; }

        /// <summary>
        /// Gets whether to show as a modal window if parent component chooses to show this component
        /// in a new window. Only applies if parent component shows this component in a new window
        /// </summary>
        public virtual bool ModalWindow { get; set; }

        public IContentCloser Closer { get; set; }

        /// <summary>
        /// Raises the <see cref="ContentClosed"/> event
        /// </summary>
        protected void Close()
        {
            if (this.Closer != null)
                this.Closer.Close();
        }
    }

    public interface IContentCloser
    {
        void Close();
    }
}
