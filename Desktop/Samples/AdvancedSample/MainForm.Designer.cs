namespace AdvancedSample
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.mgDefaultToolbar1 = new OSGeo.MapGuide.Viewer.MgDefaultToolbar();
            this.btnMeasure = new System.Windows.Forms.ToolStripButton();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.lblCoordinates = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblMessage = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblScale = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblSize = new System.Windows.Forms.ToolStripStatusLabel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.mgLegend1 = new OSGeo.MapGuide.Viewer.MgLegend();
            this.mgPropertyPane1 = new OSGeo.MapGuide.Viewer.MgPropertyPane();
            this.mgMapViewer1 = new OSGeo.MapGuide.Viewer.MgMapViewer();
            this.btnDrawPoint = new System.Windows.Forms.ToolStripButton();
            this.mgDefaultToolbar1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // mgDefaultToolbar1
            // 
            this.mgDefaultToolbar1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnMeasure,
            this.btnDrawPoint});
            this.mgDefaultToolbar1.Location = new System.Drawing.Point(0, 0);
            this.mgDefaultToolbar1.Name = "mgDefaultToolbar1";
            this.mgDefaultToolbar1.Size = new System.Drawing.Size(624, 25);
            this.mgDefaultToolbar1.TabIndex = 2;
            this.mgDefaultToolbar1.Text = "mgDefaultToolbar1";
            this.mgDefaultToolbar1.Viewer = null;
            // 
            // btnMeasure
            // 
            this.btnMeasure.Image = ((System.Drawing.Image)(resources.GetObject("btnMeasure.Image")));
            this.btnMeasure.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnMeasure.Name = "btnMeasure";
            this.btnMeasure.Size = new System.Drawing.Size(72, 22);
            this.btnMeasure.Text = "Measure";
            this.btnMeasure.Click += new System.EventHandler(this.btnMeasure_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblCoordinates,
            this.lblMessage,
            this.lblScale,
            this.lblSize});
            this.statusStrip1.Location = new System.Drawing.Point(0, 420);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(624, 22);
            this.statusStrip1.TabIndex = 3;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // lblCoordinates
            // 
            this.lblCoordinates.Name = "lblCoordinates";
            this.lblCoordinates.Size = new System.Drawing.Size(84, 17);
            this.lblCoordinates.Text = "lblCoordinates";
            // 
            // lblMessage
            // 
            this.lblMessage.Name = "lblMessage";
            this.lblMessage.Size = new System.Drawing.Size(264, 17);
            this.lblMessage.Spring = true;
            this.lblMessage.Text = "lblMessage";
            this.lblMessage.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblScale
            // 
            this.lblScale.Name = "lblScale";
            this.lblScale.Size = new System.Drawing.Size(47, 17);
            this.lblScale.Text = "lblScale";
            // 
            // lblSize
            // 
            this.lblSize.Name = "lblSize";
            this.lblSize.Size = new System.Drawing.Size(40, 17);
            this.lblSize.Text = "lblSize";
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
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
            this.splitContainer1.Size = new System.Drawing.Size(624, 395);
            this.splitContainer1.SplitterDistance = 214;
            this.splitContainer1.TabIndex = 4;
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
            this.splitContainer2.Size = new System.Drawing.Size(214, 395);
            this.splitContainer2.SplitterDistance = 187;
            this.splitContainer2.TabIndex = 0;
            // 
            // mgLegend1
            // 
            this.mgLegend1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mgLegend1.GroupContextMenu = null;
            this.mgLegend1.LayerContextMenu = null;
            this.mgLegend1.Location = new System.Drawing.Point(0, 0);
            this.mgLegend1.Name = "mgLegend1";
            this.mgLegend1.Size = new System.Drawing.Size(214, 187);
            this.mgLegend1.TabIndex = 0;
            this.mgLegend1.ThemeCompressionLimit = 0;
            // 
            // mgPropertyPane1
            // 
            this.mgPropertyPane1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mgPropertyPane1.Location = new System.Drawing.Point(0, 0);
            this.mgPropertyPane1.Name = "mgPropertyPane1";
            this.mgPropertyPane1.Size = new System.Drawing.Size(214, 204);
            this.mgPropertyPane1.TabIndex = 0;
            // 
            // mgMapViewer1
            // 
            this.mgMapViewer1.Cursor = System.Windows.Forms.Cursors.Default;
            this.mgMapViewer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mgMapViewer1.Location = new System.Drawing.Point(0, 0);
            this.mgMapViewer1.Name = "mgMapViewer1";
            this.mgMapViewer1.SelectionColor = System.Drawing.Color.Blue;
            this.mgMapViewer1.ShowVertexCoordinatesWhenDigitizing = true;
            this.mgMapViewer1.Size = new System.Drawing.Size(406, 395);
            this.mgMapViewer1.TabIndex = 0;
            this.mgMapViewer1.Text = "mgMapViewer1";
            this.mgMapViewer1.ZoomInFactor = 0.5;
            this.mgMapViewer1.ZoomOutFactor = 2;
            this.mgMapViewer1.SelectionChanged += new System.EventHandler(this.mgMapViewer1_SelectionChanged);
            // 
            // btnDrawPoint
            // 
            this.btnDrawPoint.Image = ((System.Drawing.Image)(resources.GetObject("btnDrawPoint.Image")));
            this.btnDrawPoint.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDrawPoint.Name = "btnDrawPoint";
            this.btnDrawPoint.Size = new System.Drawing.Size(85, 20);
            this.btnDrawPoint.Text = "Draw Point";
            this.btnDrawPoint.Click += new System.EventHandler(this.btnDrawPoint_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(624, 442);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.mgDefaultToolbar1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.mgDefaultToolbar1.ResumeLayout(false);
            this.mgDefaultToolbar1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private OSGeo.MapGuide.Viewer.MgDefaultToolbar mgDefaultToolbar1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel lblCoordinates;
        private System.Windows.Forms.ToolStripStatusLabel lblMessage;
        private System.Windows.Forms.ToolStripStatusLabel lblSize;
        private System.Windows.Forms.ToolStripStatusLabel lblScale;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private OSGeo.MapGuide.Viewer.MgLegend mgLegend1;
        private OSGeo.MapGuide.Viewer.MgPropertyPane mgPropertyPane1;
        private OSGeo.MapGuide.Viewer.MgMapViewer mgMapViewer1;
        private System.Windows.Forms.ToolStripButton btnMeasure;
        private System.Windows.Forms.ToolStripButton btnDrawPoint;

    }
}

