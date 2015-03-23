namespace DesktopViewerTest
{
    partial class MainForm
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
            this.mgDefaultToolbar1 = new OSGeo.MapGuide.Viewer.MgDefaultToolbar();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.lblCoords = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblSelected = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblScale = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblSize = new System.Windows.Forms.ToolStripStatusLabel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.mgLegend1 = new OSGeo.MapGuide.Viewer.MgLegend();
            this.mgPropertyPane1 = new OSGeo.MapGuide.Viewer.MgPropertyPane();
            this.mgMapViewer1 = new OSGeo.MapGuide.Viewer.MgMapViewer();
            this.statusStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // mgDefaultToolbar1
            // 
            this.mgDefaultToolbar1.Location = new System.Drawing.Point(0, 0);
            this.mgDefaultToolbar1.Name = "mgDefaultToolbar1";
            this.mgDefaultToolbar1.Size = new System.Drawing.Size(613, 25);
            this.mgDefaultToolbar1.TabIndex = 0;
            this.mgDefaultToolbar1.Text = "mgDefaultToolbar1";
            this.mgDefaultToolbar1.Viewer = null;
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblCoords,
            this.lblSelected,
            this.lblScale,
            this.lblSize});
            this.statusStrip1.Location = new System.Drawing.Point(0, 484);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(613, 22);
            this.statusStrip1.TabIndex = 1;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // lblCoords
            // 
            this.lblCoords.Name = "lblCoords";
            this.lblCoords.Size = new System.Drawing.Size(0, 17);
            // 
            // lblSelected
            // 
            this.lblSelected.Name = "lblSelected";
            this.lblSelected.Size = new System.Drawing.Size(598, 17);
            this.lblSelected.Spring = true;
            this.lblSelected.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblScale
            // 
            this.lblScale.Name = "lblScale";
            this.lblScale.Size = new System.Drawing.Size(0, 17);
            this.lblScale.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblSize
            // 
            this.lblSize.Name = "lblSize";
            this.lblSize.Size = new System.Drawing.Size(0, 17);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 25);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.mgMapViewer1);
            this.splitContainer1.Size = new System.Drawing.Size(613, 459);
            this.splitContainer1.SplitterDistance = 204;
            this.splitContainer1.TabIndex = 2;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.mgLegend1);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.mgPropertyPane1);
            this.splitContainer2.Size = new System.Drawing.Size(204, 459);
            this.splitContainer2.SplitterDistance = 219;
            this.splitContainer2.TabIndex = 0;
            // 
            // mgLegend1
            // 
            this.mgLegend1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mgLegend1.GroupContextMenu = null;
            this.mgLegend1.LayerContextMenu = null;
            this.mgLegend1.Location = new System.Drawing.Point(0, 0);
            this.mgLegend1.Name = "mgLegend1";
            this.mgLegend1.ShowTooltips = true;
            this.mgLegend1.Size = new System.Drawing.Size(204, 219);
            this.mgLegend1.TabIndex = 0;
            this.mgLegend1.ThemeCompressionLimit = 0;
            // 
            // mgPropertyPane1
            // 
            this.mgPropertyPane1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mgPropertyPane1.Location = new System.Drawing.Point(0, 0);
            this.mgPropertyPane1.Name = "mgPropertyPane1";
            this.mgPropertyPane1.Size = new System.Drawing.Size(204, 236);
            this.mgPropertyPane1.TabIndex = 0;
            // 
            // mgMapViewer1
            // 
            this.mgMapViewer1.Cursor = System.Windows.Forms.Cursors.Default;
            this.mgMapViewer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mgMapViewer1.Location = new System.Drawing.Point(0, 0);
            this.mgMapViewer1.MaxScale = 1000000000;
            this.mgMapViewer1.MinScale = 10;
            this.mgMapViewer1.MouseWheelDelayRenderInterval = 800;
            this.mgMapViewer1.Name = "mgMapViewer1";
            this.mgMapViewer1.PointPixelBuffer = 2;
            this.mgMapViewer1.SelectionColor = System.Drawing.Color.Blue;
            this.mgMapViewer1.Size = new System.Drawing.Size(405, 459);
            this.mgMapViewer1.TabIndex = 0;
            this.mgMapViewer1.Text = "mgMapViewer1";
            this.mgMapViewer1.TooltipDelayInterval = 1000;
            this.mgMapViewer1.ZoomInFactor = 0.5D;
            this.mgMapViewer1.ZoomOutFactor = 2D;
            this.mgMapViewer1.SelectionChanged += new System.EventHandler(this.mgMapViewer1_SelectionChanged);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(613, 506);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.mgDefaultToolbar1);
            this.Name = "MainForm";
            this.Text = "mg-desktop Sample";
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private OSGeo.MapGuide.Viewer.MgDefaultToolbar mgDefaultToolbar1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel lblCoords;
        private System.Windows.Forms.ToolStripStatusLabel lblSelected;
        private System.Windows.Forms.ToolStripStatusLabel lblScale;
        private System.Windows.Forms.ToolStripStatusLabel lblSize;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private OSGeo.MapGuide.Viewer.MgMapViewer mgMapViewer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private OSGeo.MapGuide.Viewer.MgLegend mgLegend1;
        private OSGeo.MapGuide.Viewer.MgPropertyPane mgPropertyPane1;
    }
}

