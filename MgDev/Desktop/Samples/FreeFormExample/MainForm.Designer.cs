namespace FreeFormExample
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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.mgLegend1 = new OSGeo.MapGuide.Viewer.MgLegend();
            this.mgPropertyPane1 = new OSGeo.MapGuide.Viewer.MgPropertyPane();
            this.mgMapViewer1 = new OSGeo.MapGuide.Viewer.MgMapViewer();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.lblCoords = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblSelected = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblScale = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblSize = new System.Windows.Forms.ToolStripStatusLabel();
            this.btnPan = new System.Windows.Forms.Button();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.btnClearSelection = new System.Windows.Forms.Button();
            this.btnSelectCircle = new System.Windows.Forms.Button();
            this.btnSelectPolygon = new System.Windows.Forms.Button();
            this.btnSelect = new System.Windows.Forms.Button();
            this.btnZoomExtents = new System.Windows.Forms.Button();
            this.btnZoomOut = new System.Windows.Forms.Button();
            this.btnZoomIn = new System.Windows.Forms.Button();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.btnPan);
            this.splitContainer1.Panel2.Controls.Add(this.btnRefresh);
            this.splitContainer1.Panel2.Controls.Add(this.btnClearSelection);
            this.splitContainer1.Panel2.Controls.Add(this.btnSelectCircle);
            this.splitContainer1.Panel2.Controls.Add(this.btnSelectPolygon);
            this.splitContainer1.Panel2.Controls.Add(this.btnSelect);
            this.splitContainer1.Panel2.Controls.Add(this.btnZoomExtents);
            this.splitContainer1.Panel2.Controls.Add(this.btnZoomOut);
            this.splitContainer1.Panel2.Controls.Add(this.btnZoomIn);
            this.splitContainer1.Panel2.Controls.Add(this.mgMapViewer1);
            this.splitContainer1.Size = new System.Drawing.Size(607, 495);
            this.splitContainer1.SplitterDistance = 202;
            this.splitContainer1.TabIndex = 3;
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
            this.splitContainer2.Size = new System.Drawing.Size(202, 495);
            this.splitContainer2.SplitterDistance = 235;
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
            this.mgLegend1.Size = new System.Drawing.Size(202, 235);
            this.mgLegend1.TabIndex = 0;
            this.mgLegend1.ThemeCompressionLimit = 0;
            // 
            // mgPropertyPane1
            // 
            this.mgPropertyPane1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mgPropertyPane1.Location = new System.Drawing.Point(0, 0);
            this.mgPropertyPane1.Name = "mgPropertyPane1";
            this.mgPropertyPane1.Size = new System.Drawing.Size(202, 256);
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
            this.mgMapViewer1.RespectFiniteDisplayScales = false;
            this.mgMapViewer1.SelectionColor = System.Drawing.Color.Blue;
            this.mgMapViewer1.Size = new System.Drawing.Size(401, 495);
            this.mgMapViewer1.TabIndex = 0;
            this.mgMapViewer1.Text = "mgMapViewer1";
            this.mgMapViewer1.TooltipDelayInterval = 1000;
            this.mgMapViewer1.UseRenderMapIfTiledLayersExist = false;
            this.mgMapViewer1.ZoomInFactor = 0.5;
            this.mgMapViewer1.ZoomOutFactor = 2;
            this.mgMapViewer1.SelectionChanged += new System.EventHandler(this.mgMapViewer1_SelectionChanged);
            this.mgMapViewer1.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(this.mgMapViewer1_PropertyChanged);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblCoords,
            this.lblSelected,
            this.lblScale,
            this.lblSize});
            this.statusStrip1.Location = new System.Drawing.Point(0, 495);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(607, 22);
            this.statusStrip1.TabIndex = 4;
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
            this.lblSelected.Size = new System.Drawing.Size(592, 17);
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
            // btnPan
            // 
            this.btnPan.BackColor = System.Drawing.Color.Transparent;
            this.btnPan.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnPan.Image = global::FreeFormExample.Properties.Resources.icon_pan;
            this.btnPan.Location = new System.Drawing.Point(13, 44);
            this.btnPan.Name = "btnPan";
            this.btnPan.Size = new System.Drawing.Size(26, 26);
            this.btnPan.TabIndex = 12;
            this.btnPan.UseVisualStyleBackColor = false;
            this.btnPan.Click += new System.EventHandler(this.btnPan_Click);
            // 
            // btnRefresh
            // 
            this.btnRefresh.BackColor = System.Drawing.Color.Transparent;
            this.btnRefresh.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnRefresh.Image = global::FreeFormExample.Properties.Resources.view_refresh;
            this.btnRefresh.Location = new System.Drawing.Point(77, 44);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(26, 26);
            this.btnRefresh.TabIndex = 11;
            this.btnRefresh.UseVisualStyleBackColor = false;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // btnClearSelection
            // 
            this.btnClearSelection.BackColor = System.Drawing.Color.Transparent;
            this.btnClearSelection.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnClearSelection.Image = global::FreeFormExample.Properties.Resources.select_clear;
            this.btnClearSelection.Location = new System.Drawing.Point(45, 44);
            this.btnClearSelection.Name = "btnClearSelection";
            this.btnClearSelection.Size = new System.Drawing.Size(26, 26);
            this.btnClearSelection.TabIndex = 10;
            this.btnClearSelection.UseVisualStyleBackColor = false;
            this.btnClearSelection.Click += new System.EventHandler(this.btnClearSelection_Click);
            // 
            // btnSelectCircle
            // 
            this.btnSelectCircle.BackColor = System.Drawing.Color.Transparent;
            this.btnSelectCircle.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSelectCircle.Image = global::FreeFormExample.Properties.Resources.select_radius;
            this.btnSelectCircle.Location = new System.Drawing.Point(77, 76);
            this.btnSelectCircle.Name = "btnSelectCircle";
            this.btnSelectCircle.Size = new System.Drawing.Size(26, 26);
            this.btnSelectCircle.TabIndex = 9;
            this.btnSelectCircle.UseVisualStyleBackColor = false;
            this.btnSelectCircle.Click += new System.EventHandler(this.btnSelectCircle_Click);
            // 
            // btnSelectPolygon
            // 
            this.btnSelectPolygon.BackColor = System.Drawing.Color.Transparent;
            this.btnSelectPolygon.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSelectPolygon.Image = global::FreeFormExample.Properties.Resources.select_polygon;
            this.btnSelectPolygon.Location = new System.Drawing.Point(45, 76);
            this.btnSelectPolygon.Name = "btnSelectPolygon";
            this.btnSelectPolygon.Size = new System.Drawing.Size(26, 26);
            this.btnSelectPolygon.TabIndex = 8;
            this.btnSelectPolygon.UseVisualStyleBackColor = false;
            this.btnSelectPolygon.Click += new System.EventHandler(this.btnSelectPolygon_Click);
            // 
            // btnSelect
            // 
            this.btnSelect.BackColor = System.Drawing.Color.Transparent;
            this.btnSelect.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSelect.Image = global::FreeFormExample.Properties.Resources.select_features;
            this.btnSelect.Location = new System.Drawing.Point(13, 76);
            this.btnSelect.Name = "btnSelect";
            this.btnSelect.Size = new System.Drawing.Size(26, 26);
            this.btnSelect.TabIndex = 7;
            this.btnSelect.UseVisualStyleBackColor = false;
            this.btnSelect.Click += new System.EventHandler(this.btnSelect_Click);
            // 
            // btnZoomExtents
            // 
            this.btnZoomExtents.BackColor = System.Drawing.Color.Transparent;
            this.btnZoomExtents.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnZoomExtents.Image = global::FreeFormExample.Properties.Resources.zoom_full;
            this.btnZoomExtents.Location = new System.Drawing.Point(77, 12);
            this.btnZoomExtents.Name = "btnZoomExtents";
            this.btnZoomExtents.Size = new System.Drawing.Size(26, 26);
            this.btnZoomExtents.TabIndex = 6;
            this.btnZoomExtents.UseVisualStyleBackColor = false;
            this.btnZoomExtents.Click += new System.EventHandler(this.btnZoomExtents_Click);
            // 
            // btnZoomOut
            // 
            this.btnZoomOut.BackColor = System.Drawing.Color.Transparent;
            this.btnZoomOut.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnZoomOut.Image = global::FreeFormExample.Properties.Resources.zoom_out_fixed;
            this.btnZoomOut.Location = new System.Drawing.Point(45, 12);
            this.btnZoomOut.Name = "btnZoomOut";
            this.btnZoomOut.Size = new System.Drawing.Size(26, 26);
            this.btnZoomOut.TabIndex = 4;
            this.btnZoomOut.UseVisualStyleBackColor = false;
            this.btnZoomOut.Click += new System.EventHandler(this.btnZoomOut_Click);
            // 
            // btnZoomIn
            // 
            this.btnZoomIn.BackColor = System.Drawing.Color.Transparent;
            this.btnZoomIn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnZoomIn.Image = global::FreeFormExample.Properties.Resources.zoom_in_fixed;
            this.btnZoomIn.Location = new System.Drawing.Point(13, 12);
            this.btnZoomIn.Name = "btnZoomIn";
            this.btnZoomIn.Size = new System.Drawing.Size(26, 26);
            this.btnZoomIn.TabIndex = 3;
            this.btnZoomIn.UseVisualStyleBackColor = false;
            this.btnZoomIn.Click += new System.EventHandler(this.btnZoomIn_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(607, 517);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.statusStrip1);
            this.Name = "MainForm";
            this.Text = "mg-desktop free-form map viewer example";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private OSGeo.MapGuide.Viewer.MgLegend mgLegend1;
        private OSGeo.MapGuide.Viewer.MgPropertyPane mgPropertyPane1;
        private OSGeo.MapGuide.Viewer.MgMapViewer mgMapViewer1;
        private System.Windows.Forms.Button btnZoomIn;
        private System.Windows.Forms.Button btnZoomExtents;
        private System.Windows.Forms.Button btnZoomOut;
        private System.Windows.Forms.Button btnSelect;
        private System.Windows.Forms.Button btnSelectPolygon;
        private System.Windows.Forms.Button btnSelectCircle;
        private System.Windows.Forms.Button btnRefresh;
        private System.Windows.Forms.Button btnClearSelection;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel lblCoords;
        private System.Windows.Forms.ToolStripStatusLabel lblSelected;
        private System.Windows.Forms.ToolStripStatusLabel lblScale;
        private System.Windows.Forms.ToolStripStatusLabel lblSize;
        private System.Windows.Forms.Button btnPan;

    }
}

