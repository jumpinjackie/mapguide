using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;

#pragma warning disable 1591

namespace OSGeo.MapGuide.Viewer
{
    /// <summary>
    /// Base class for views show by their associated <see cref="T:OSGeo.MapGuide.Viewer.MgViewerComponent"/> instances
    /// </summary>
    [ToolboxItem(false)]
    public class MgControlView : UserControl
    {
        /// <summary>
        /// Gets or set the title to show if this view is shown in a new window
        /// </summary>
        public virtual string Title { get; set; }

        protected MgControlView() { this.ModalWindow = false; }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                SubCleanup();
            }
            base.Dispose(disposing);
        }

        protected virtual void SubCleanup() { }

        /// <summary>
        /// Gets whether to show as a modal window if parent component chooses to show this component
        /// in a new window. Only applies if parent component shows this component in a new window
        /// </summary>
        public virtual bool ModalWindow { get; set; }

        /// <summary>
        /// Gets or sets the content closer
        /// </summary>
        public IContentCloser Closer { get; set; }

        /// <summary>
        /// 
        /// </summary>
        protected void Close()
        {
            if (this.Closer != null)
                this.Closer.Close();
        }

        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // MgControlView
            // 
            this.Name = "MgControlView";
            this.ResumeLayout(false);

        }
    }

    /// <summary>
    /// An interface for closing component UI content
    /// </summary>
    public interface IContentCloser
    {
        /// <summary>
        /// Closes the UI content
        /// </summary>
        void Close();
    }
}
