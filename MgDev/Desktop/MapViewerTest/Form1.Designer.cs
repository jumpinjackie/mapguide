namespace MapViewerTest
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.lblCoords = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblScale = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblSelection = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblSize = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblLogo = new System.Windows.Forms.ToolStripStatusLabel();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openMapToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadPackageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.quitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.testToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadCompactMapViewerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.debugToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.showConnectionPoolStatusToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.purgePooledConnectionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.btnDigitizePoint = new System.Windows.Forms.ToolStripButton();
            this.btnDigitizeLine = new System.Windows.Forms.ToolStripButton();
            this.btnDigitizePolyline = new System.Windows.Forms.ToolStripButton();
            this.btnDigitizeCircle = new System.Windows.Forms.ToolStripButton();
            this.btnDigitizeRectangle = new System.Windows.Forms.ToolStripButton();
            this.btnDigitizePolygon = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.btnPlotToDwf = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton2 = new System.Windows.Forms.ToolStripButton();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.legend = new OSGeo.MapGuide.Viewer.MgLegend();
            this.ctxGroup = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.thisIsAGroupContextMenuToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxLayer = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.thisIsALayerContextMenuToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.propertyPane = new OSGeo.MapGuide.Viewer.MgPropertyPane();
            this.viewer = new OSGeo.MapGuide.Viewer.MgMapViewer();
            this.ctxViewer = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.refreshMapToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.initialViewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
            this.toolbar = new OSGeo.MapGuide.Viewer.MgDefaultToolbar();
            this.statusStrip1.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.ctxGroup.SuspendLayout();
            this.ctxLayer.SuspendLayout();
            this.ctxViewer.SuspendLayout();
            this.toolStripContainer1.ContentPanel.SuspendLayout();
            this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
            this.toolStripContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblCoords,
            this.lblScale,
            this.lblSelection,
            this.lblSize,
            this.lblLogo});
            this.statusStrip1.Location = new System.Drawing.Point(0, 539);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(784, 23);
            this.statusStrip1.TabIndex = 0;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // lblCoords
            // 
            this.lblCoords.Name = "lblCoords";
            this.lblCoords.Size = new System.Drawing.Size(0, 18);
            this.lblCoords.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblScale
            // 
            this.lblScale.Name = "lblScale";
            this.lblScale.Size = new System.Drawing.Size(0, 18);
            // 
            // lblSelection
            // 
            this.lblSelection.Name = "lblSelection";
            this.lblSelection.Size = new System.Drawing.Size(632, 18);
            this.lblSelection.Spring = true;
            // 
            // lblSize
            // 
            this.lblSize.Name = "lblSize";
            this.lblSize.Size = new System.Drawing.Size(0, 18);
            // 
            // lblLogo
            // 
            this.lblLogo.Image = global::MapViewerTest.Properties.Resources.PoweredBy_en;
            this.lblLogo.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.lblLogo.Name = "lblLogo";
            this.lblLogo.Size = new System.Drawing.Size(137, 18);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.testToolStripMenuItem,
            this.debugToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(784, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openMapToolStripMenuItem,
            this.loadPackageToolStripMenuItem,
            this.quitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openMapToolStripMenuItem
            // 
            this.openMapToolStripMenuItem.Name = "openMapToolStripMenuItem";
            this.openMapToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.openMapToolStripMenuItem.Text = "Open Map";
            this.openMapToolStripMenuItem.Click += new System.EventHandler(this.openMapToolStripMenuItem_Click);
            // 
            // loadPackageToolStripMenuItem
            // 
            this.loadPackageToolStripMenuItem.Name = "loadPackageToolStripMenuItem";
            this.loadPackageToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.loadPackageToolStripMenuItem.Text = "Load Package";
            this.loadPackageToolStripMenuItem.Click += new System.EventHandler(this.loadPackageToolStripMenuItem_Click);
            // 
            // quitToolStripMenuItem
            // 
            this.quitToolStripMenuItem.Name = "quitToolStripMenuItem";
            this.quitToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.quitToolStripMenuItem.Text = "Quit";
            this.quitToolStripMenuItem.Click += new System.EventHandler(this.quitToolStripMenuItem_Click);
            // 
            // testToolStripMenuItem
            // 
            this.testToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadCompactMapViewerToolStripMenuItem});
            this.testToolStripMenuItem.Name = "testToolStripMenuItem";
            this.testToolStripMenuItem.Size = new System.Drawing.Size(41, 20);
            this.testToolStripMenuItem.Text = "Test";
            // 
            // loadCompactMapViewerToolStripMenuItem
            // 
            this.loadCompactMapViewerToolStripMenuItem.Name = "loadCompactMapViewerToolStripMenuItem";
            this.loadCompactMapViewerToolStripMenuItem.Size = new System.Drawing.Size(217, 22);
            this.loadCompactMapViewerToolStripMenuItem.Text = "Load Compact Map Viewer";
            this.loadCompactMapViewerToolStripMenuItem.Click += new System.EventHandler(this.loadCompactMapViewerToolStripMenuItem_Click);
            // 
            // debugToolStripMenuItem
            // 
            this.debugToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.showConnectionPoolStatusToolStripMenuItem,
            this.purgePooledConnectionsToolStripMenuItem});
            this.debugToolStripMenuItem.Name = "debugToolStripMenuItem";
            this.debugToolStripMenuItem.Size = new System.Drawing.Size(54, 20);
            this.debugToolStripMenuItem.Text = "Debug";
            // 
            // showConnectionPoolStatusToolStripMenuItem
            // 
            this.showConnectionPoolStatusToolStripMenuItem.Name = "showConnectionPoolStatusToolStripMenuItem";
            this.showConnectionPoolStatusToolStripMenuItem.Size = new System.Drawing.Size(230, 22);
            this.showConnectionPoolStatusToolStripMenuItem.Text = "Show Connection Pool Status";
            this.showConnectionPoolStatusToolStripMenuItem.Click += new System.EventHandler(this.showConnectionPoolStatusToolStripMenuItem_Click);
            // 
            // purgePooledConnectionsToolStripMenuItem
            // 
            this.purgePooledConnectionsToolStripMenuItem.Name = "purgePooledConnectionsToolStripMenuItem";
            this.purgePooledConnectionsToolStripMenuItem.Size = new System.Drawing.Size(230, 22);
            this.purgePooledConnectionsToolStripMenuItem.Text = "Purge Pooled Connections";
            this.purgePooledConnectionsToolStripMenuItem.Click += new System.EventHandler(this.purgePooledConnectionsToolStripMenuItem_Click);
            // 
            // toolStrip1
            // 
            this.toolStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnDigitizePoint,
            this.btnDigitizeLine,
            this.btnDigitizePolyline,
            this.btnDigitizeCircle,
            this.btnDigitizeRectangle,
            this.btnDigitizePolygon,
            this.toolStripSeparator1,
            this.btnPlotToDwf,
            this.toolStripButton1,
            this.toolStripButton2});
            this.toolStrip1.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.HorizontalStackWithOverflow;
            this.toolStrip1.Location = new System.Drawing.Point(0, 25);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(784, 25);
            this.toolStrip1.Stretch = true;
            this.toolStrip1.TabIndex = 2;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // btnDigitizePoint
            // 
            this.btnDigitizePoint.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnDigitizePoint.Image = global::MapViewerTest.Properties.Resources.layer_small;
            this.btnDigitizePoint.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDigitizePoint.Name = "btnDigitizePoint";
            this.btnDigitizePoint.Size = new System.Drawing.Size(23, 22);
            this.btnDigitizePoint.Text = "Digitize Point";
            this.btnDigitizePoint.Click += new System.EventHandler(this.btnDigitizePoint_Click);
            // 
            // btnDigitizeLine
            // 
            this.btnDigitizeLine.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnDigitizeLine.Image = global::MapViewerTest.Properties.Resources.layer_shape_line;
            this.btnDigitizeLine.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDigitizeLine.Name = "btnDigitizeLine";
            this.btnDigitizeLine.Size = new System.Drawing.Size(23, 22);
            this.btnDigitizeLine.Text = "Digitize Line";
            this.btnDigitizeLine.Click += new System.EventHandler(this.btnDigitizeLine_Click);
            // 
            // btnDigitizePolyline
            // 
            this.btnDigitizePolyline.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnDigitizePolyline.Image = global::MapViewerTest.Properties.Resources.layer_shape_polyline;
            this.btnDigitizePolyline.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDigitizePolyline.Name = "btnDigitizePolyline";
            this.btnDigitizePolyline.Size = new System.Drawing.Size(23, 22);
            this.btnDigitizePolyline.Text = "Digitize Line String";
            this.btnDigitizePolyline.Click += new System.EventHandler(this.btnDigitizePolyline_Click);
            // 
            // btnDigitizeCircle
            // 
            this.btnDigitizeCircle.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnDigitizeCircle.Image = global::MapViewerTest.Properties.Resources.layer_shape_ellipse;
            this.btnDigitizeCircle.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDigitizeCircle.Name = "btnDigitizeCircle";
            this.btnDigitizeCircle.Size = new System.Drawing.Size(23, 22);
            this.btnDigitizeCircle.Text = "Digitize Circle";
            this.btnDigitizeCircle.Click += new System.EventHandler(this.btnDigitizeCircle_Click);
            // 
            // btnDigitizeRectangle
            // 
            this.btnDigitizeRectangle.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnDigitizeRectangle.Image = global::MapViewerTest.Properties.Resources.layer_shape;
            this.btnDigitizeRectangle.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDigitizeRectangle.Name = "btnDigitizeRectangle";
            this.btnDigitizeRectangle.Size = new System.Drawing.Size(23, 22);
            this.btnDigitizeRectangle.Text = "Digitize Rectangle";
            this.btnDigitizeRectangle.Click += new System.EventHandler(this.btnDigitizeRectangle_Click);
            // 
            // btnDigitizePolygon
            // 
            this.btnDigitizePolygon.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnDigitizePolygon.Image = global::MapViewerTest.Properties.Resources.layer_shape_polygon;
            this.btnDigitizePolygon.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDigitizePolygon.Name = "btnDigitizePolygon";
            this.btnDigitizePolygon.Size = new System.Drawing.Size(23, 22);
            this.btnDigitizePolygon.Text = "Digitize Polygon";
            this.btnDigitizePolygon.Click += new System.EventHandler(this.btnDigitizePolygon_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // btnPlotToDwf
            // 
            this.btnPlotToDwf.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnPlotToDwf.Image = global::MapViewerTest.Properties.Resources.lc_dwf;
            this.btnPlotToDwf.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnPlotToDwf.Name = "btnPlotToDwf";
            this.btnPlotToDwf.Size = new System.Drawing.Size(23, 22);
            this.btnPlotToDwf.Text = "Plot current view to DWF";
            this.btnPlotToDwf.Click += new System.EventHandler(this.btnPlotToDwf_Click);
            // 
            // toolStripButton1
            // 
            this.toolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton1.Image")));
            this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton1.Name = "toolStripButton1";
            this.toolStripButton1.Size = new System.Drawing.Size(72, 22);
            this.toolStripButton1.Text = "Measure";
            this.toolStripButton1.Click += new System.EventHandler(this.toolStripButton1_Click);
            // 
            // toolStripButton2
            // 
            this.toolStripButton2.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton2.Image")));
            this.toolStripButton2.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton2.Name = "toolStripButton2";
            this.toolStripButton2.Size = new System.Drawing.Size(59, 22);
            this.toolStripButton2.Text = "Buffer";
            this.toolStripButton2.Click += new System.EventHandler(this.toolStripButton2_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.viewer);
            this.splitContainer1.Size = new System.Drawing.Size(784, 465);
            this.splitContainer1.SplitterDistance = 208;
            this.splitContainer1.TabIndex = 3;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.legend);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.propertyPane);
            this.splitContainer2.Size = new System.Drawing.Size(208, 465);
            this.splitContainer2.SplitterDistance = 236;
            this.splitContainer2.TabIndex = 0;
            // 
            // legend
            // 
            this.legend.Dock = System.Windows.Forms.DockStyle.Fill;
            this.legend.GroupContextMenu = this.ctxGroup;
            this.legend.LayerContextMenu = this.ctxLayer;
            this.legend.Location = new System.Drawing.Point(0, 0);
            this.legend.Name = "legend";
            this.legend.Size = new System.Drawing.Size(208, 236);
            this.legend.TabIndex = 0;
            this.legend.ThemeCompressionLimit = 25;
            // 
            // ctxGroup
            // 
            this.ctxGroup.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.thisIsAGroupContextMenuToolStripMenuItem});
            this.ctxGroup.Name = "ctxGroup";
            this.ctxGroup.Size = new System.Drawing.Size(228, 26);
            // 
            // thisIsAGroupContextMenuToolStripMenuItem
            // 
            this.thisIsAGroupContextMenuToolStripMenuItem.Name = "thisIsAGroupContextMenuToolStripMenuItem";
            this.thisIsAGroupContextMenuToolStripMenuItem.Size = new System.Drawing.Size(227, 22);
            this.thisIsAGroupContextMenuToolStripMenuItem.Text = "This is a group context menu";
            this.thisIsAGroupContextMenuToolStripMenuItem.Click += new System.EventHandler(this.thisIsAGroupContextMenuToolStripMenuItem_Click);
            // 
            // ctxLayer
            // 
            this.ctxLayer.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.thisIsALayerContextMenuToolStripMenuItem});
            this.ctxLayer.Name = "ctxLayer";
            this.ctxLayer.Size = new System.Drawing.Size(221, 26);
            // 
            // thisIsALayerContextMenuToolStripMenuItem
            // 
            this.thisIsALayerContextMenuToolStripMenuItem.Name = "thisIsALayerContextMenuToolStripMenuItem";
            this.thisIsALayerContextMenuToolStripMenuItem.Size = new System.Drawing.Size(220, 22);
            this.thisIsALayerContextMenuToolStripMenuItem.Text = "This is a layer context menu";
            this.thisIsALayerContextMenuToolStripMenuItem.Click += new System.EventHandler(this.thisIsALayerContextMenuToolStripMenuItem_Click);
            // 
            // propertyPane
            // 
            this.propertyPane.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyPane.Location = new System.Drawing.Point(0, 0);
            this.propertyPane.Name = "propertyPane";
            this.propertyPane.Size = new System.Drawing.Size(208, 225);
            this.propertyPane.TabIndex = 0;
            // 
            // viewer
            // 
            this.viewer.ContextMenuStrip = this.ctxViewer;
            this.viewer.ConvertTiledGroupsToNonTiled = true;
            this.viewer.Cursor = System.Windows.Forms.Cursors.Default;
            this.viewer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.viewer.Location = new System.Drawing.Point(0, 0);
            this.viewer.Name = "viewer";
            this.viewer.SelectionColor = System.Drawing.Color.OrangeRed;
            this.viewer.Size = new System.Drawing.Size(572, 465);
            this.viewer.TabIndex = 1;
            this.viewer.ZoomInFactor = 0.75D;
            this.viewer.ZoomOutFactor = 1.35D;
            // 
            // ctxViewer
            // 
            this.ctxViewer.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.refreshMapToolStripMenuItem,
            this.initialViewToolStripMenuItem});
            this.ctxViewer.Name = "ctxViewer";
            this.ctxViewer.Size = new System.Drawing.Size(141, 48);
            // 
            // refreshMapToolStripMenuItem
            // 
            this.refreshMapToolStripMenuItem.Name = "refreshMapToolStripMenuItem";
            this.refreshMapToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
            this.refreshMapToolStripMenuItem.Text = "Refresh Map";
            this.refreshMapToolStripMenuItem.Click += new System.EventHandler(this.refreshMapToolStripMenuItem_Click);
            // 
            // initialViewToolStripMenuItem
            // 
            this.initialViewToolStripMenuItem.Name = "initialViewToolStripMenuItem";
            this.initialViewToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
            this.initialViewToolStripMenuItem.Text = "Initial View";
            this.initialViewToolStripMenuItem.Click += new System.EventHandler(this.initialViewToolStripMenuItem_Click);
            // 
            // toolStripContainer1
            // 
            // 
            // toolStripContainer1.ContentPanel
            // 
            this.toolStripContainer1.ContentPanel.Controls.Add(this.splitContainer1);
            this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(784, 465);
            this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.toolStripContainer1.Location = new System.Drawing.Point(0, 24);
            this.toolStripContainer1.Name = "toolStripContainer1";
            this.toolStripContainer1.Size = new System.Drawing.Size(784, 515);
            this.toolStripContainer1.TabIndex = 4;
            this.toolStripContainer1.Text = "toolStripContainer1";
            // 
            // toolStripContainer1.TopToolStripPanel
            // 
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.toolbar);
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.toolStrip1);
            // 
            // toolbar
            // 
            this.toolbar.Dock = System.Windows.Forms.DockStyle.None;
            this.toolbar.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.HorizontalStackWithOverflow;
            this.toolbar.Location = new System.Drawing.Point(0, 0);
            this.toolbar.Name = "toolbar";
            this.toolbar.Size = new System.Drawing.Size(784, 25);
            this.toolbar.Stretch = true;
            this.toolbar.TabIndex = 1;
            this.toolbar.Text = "mgDefaultToolbar1";
            this.toolbar.Viewer = null;
            this.toolbar.ZoomOutMode = OSGeo.MapGuide.Viewer.ZoomOutMode.AutoZoom;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(784, 562);
            this.Controls.Add(this.toolStripContainer1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "MapGuide Desktop Map Viewer";
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.ctxGroup.ResumeLayout(false);
            this.ctxLayer.ResumeLayout(false);
            this.ctxViewer.ResumeLayout(false);
            this.toolStripContainer1.ContentPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.PerformLayout();
            this.toolStripContainer1.ResumeLayout(false);
            this.toolStripContainer1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openMapToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem quitToolStripMenuItem;
        private System.Windows.Forms.ToolStripStatusLabel lblCoords;
        private System.Windows.Forms.ToolStripStatusLabel lblScale;
        private System.Windows.Forms.ToolStripStatusLabel lblSelection;
        private System.Windows.Forms.ToolStripStatusLabel lblSize;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.ToolStripMenuItem loadPackageToolStripMenuItem;
        private System.Windows.Forms.ToolStripStatusLabel lblLogo;
        private System.Windows.Forms.ToolStripButton btnDigitizePoint;
        private System.Windows.Forms.ToolStripButton btnDigitizeLine;
        private System.Windows.Forms.ToolStripButton btnDigitizePolyline;
        private System.Windows.Forms.ToolStripButton btnDigitizeCircle;
        private System.Windows.Forms.ToolStripButton btnDigitizeRectangle;
        private System.Windows.Forms.ToolStripButton btnDigitizePolygon;
        private OSGeo.MapGuide.Viewer.MgDefaultToolbar toolbar;
        private System.Windows.Forms.ToolStripContainer toolStripContainer1;
        private OSGeo.MapGuide.Viewer.MgMapViewer viewer;
        private OSGeo.MapGuide.Viewer.MgPropertyPane propertyPane;
        private System.Windows.Forms.ToolStripButton btnPlotToDwf;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem testToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem loadCompactMapViewerToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip ctxGroup;
        private System.Windows.Forms.ContextMenuStrip ctxLayer;
        private System.Windows.Forms.ToolStripMenuItem thisIsAGroupContextMenuToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem thisIsALayerContextMenuToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem debugToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem showConnectionPoolStatusToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem purgePooledConnectionsToolStripMenuItem;
        private OSGeo.MapGuide.Viewer.MgLegend legend;
        private System.Windows.Forms.ContextMenuStrip ctxViewer;
        private System.Windows.Forms.ToolStripMenuItem refreshMapToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem initialViewToolStripMenuItem;
        private System.Windows.Forms.ToolStripButton toolStripButton1;
        private System.Windows.Forms.ToolStripButton toolStripButton2;
    }
}

