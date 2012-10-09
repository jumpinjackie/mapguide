namespace MapViewerTest
{
    partial class CompactViewer
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.toolbar = new OSGeo.MapGuide.Viewer.MgDefaultToolbar();
            this.viewer = new OSGeo.MapGuide.Viewer.MgMapViewer();
            this.SuspendLayout();
            // 
            // toolbar
            // 
            this.toolbar.Location = new System.Drawing.Point(0, 0);
            this.toolbar.Name = "toolbar";
            this.toolbar.Size = new System.Drawing.Size(504, 25);
            this.toolbar.TabIndex = 0;
            this.toolbar.Text = "mgDefaultToolbar1";
            this.toolbar.Viewer = null;
            // 
            // viewer
            // 
            this.viewer.ActiveTool = OSGeo.MapGuide.Viewer.MapActiveTool.None;
            this.viewer.Cursor = System.Windows.Forms.Cursors.Default;
            this.viewer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.viewer.FeatureTooltipsEnabled = false;
            this.viewer.Location = new System.Drawing.Point(0, 25);
            this.viewer.Name = "viewer";
            this.viewer.Size = new System.Drawing.Size(504, 381);
            this.viewer.TabIndex = 1;
            this.viewer.Text = "mgMapViewer1";
            this.viewer.ZoomInFactor = 0.75;
            this.viewer.ZoomOutFactor = 1.35;
            // 
            // CompactViewer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(504, 406);
            this.Controls.Add(this.viewer);
            this.Controls.Add(this.toolbar);
            this.Name = "CompactViewer";
            this.Text = "Compact Map Viewer";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private OSGeo.MapGuide.Viewer.MgDefaultToolbar toolbar;
        private OSGeo.MapGuide.Viewer.MgMapViewer viewer;
    }
}