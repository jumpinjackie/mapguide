namespace OSGeo.MapGuide.Viewer.AppLayoutEngine
{
    partial class Shell
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Shell));
            this.appContainer = new System.Windows.Forms.SplitContainer();
            this.infoPaneViewerContainer = new System.Windows.Forms.SplitContainer();
            this.layerPropertiesContainer = new System.Windows.Forms.SplitContainer();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.TAB_LEGEND = new System.Windows.Forms.TabPage();
            this.imgList = new System.Windows.Forms.ImageList(this.components);
            this.tabControl2 = new System.Windows.Forms.TabControl();
            this.TAB_PROPERTIES = new System.Windows.Forms.TabPage();
            this.viewerContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.viewerToolbar = new System.Windows.Forms.ToolStrip();
            this.taskPaneToolbar = new System.Windows.Forms.ToolStrip();
            this.btnInitialTask = new System.Windows.Forms.ToolStripButton();
            this.taskMenu = new System.Windows.Forms.ToolStripDropDownButton();
            this.mainMenu = new System.Windows.Forms.MenuStrip();
            this.appStatusBar = new System.Windows.Forms.StatusStrip();
            this.lblCoordinates = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblSelected = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblMapScale = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblMapSize = new System.Windows.Forms.ToolStripStatusLabel();
            this.lblPoweredBy = new System.Windows.Forms.ToolStripStatusLabel();
            this.legend = new OSGeo.MapGuide.Viewer.MgLegend();
            this.propertyPane = new OSGeo.MapGuide.Viewer.MgPropertyPane();
            this.mapViewer = new OSGeo.MapGuide.Viewer.MgMapViewer();
            this.taskPane = new OSGeo.MapGuide.Viewer.MgTaskPane();
            this.appContainer.Panel1.SuspendLayout();
            this.appContainer.Panel2.SuspendLayout();
            this.appContainer.SuspendLayout();
            this.infoPaneViewerContainer.Panel1.SuspendLayout();
            this.infoPaneViewerContainer.Panel2.SuspendLayout();
            this.infoPaneViewerContainer.SuspendLayout();
            this.layerPropertiesContainer.Panel1.SuspendLayout();
            this.layerPropertiesContainer.Panel2.SuspendLayout();
            this.layerPropertiesContainer.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.TAB_LEGEND.SuspendLayout();
            this.tabControl2.SuspendLayout();
            this.TAB_PROPERTIES.SuspendLayout();
            this.taskPaneToolbar.SuspendLayout();
            this.appStatusBar.SuspendLayout();
            this.SuspendLayout();
            // 
            // appContainer
            // 
            resources.ApplyResources(this.appContainer, "appContainer");
            this.appContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.appContainer.Name = "appContainer";
            // 
            // appContainer.Panel1
            // 
            this.appContainer.Panel1.Controls.Add(this.infoPaneViewerContainer);
            this.appContainer.Panel1.Controls.Add(this.viewerToolbar);
            // 
            // appContainer.Panel2
            // 
            this.appContainer.Panel2.Controls.Add(this.taskPane);
            this.appContainer.Panel2.Controls.Add(this.taskPaneToolbar);
            // 
            // infoPaneViewerContainer
            // 
            resources.ApplyResources(this.infoPaneViewerContainer, "infoPaneViewerContainer");
            this.infoPaneViewerContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.infoPaneViewerContainer.Name = "infoPaneViewerContainer";
            // 
            // infoPaneViewerContainer.Panel1
            // 
            this.infoPaneViewerContainer.Panel1.Controls.Add(this.layerPropertiesContainer);
            // 
            // infoPaneViewerContainer.Panel2
            // 
            this.infoPaneViewerContainer.Panel2.Controls.Add(this.mapViewer);
            // 
            // layerPropertiesContainer
            // 
            resources.ApplyResources(this.layerPropertiesContainer, "layerPropertiesContainer");
            this.layerPropertiesContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.layerPropertiesContainer.Name = "layerPropertiesContainer";
            // 
            // layerPropertiesContainer.Panel1
            // 
            this.layerPropertiesContainer.Panel1.Controls.Add(this.tabControl1);
            // 
            // layerPropertiesContainer.Panel2
            // 
            this.layerPropertiesContainer.Panel2.Controls.Add(this.tabControl2);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.TAB_LEGEND);
            resources.ApplyResources(this.tabControl1, "tabControl1");
            this.tabControl1.ImageList = this.imgList;
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            // 
            // TAB_LEGEND
            // 
            this.TAB_LEGEND.Controls.Add(this.legend);
            resources.ApplyResources(this.TAB_LEGEND, "TAB_LEGEND");
            this.TAB_LEGEND.Name = "TAB_LEGEND";
            this.TAB_LEGEND.UseVisualStyleBackColor = true;
            // 
            // imgList
            // 
            this.imgList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imgList.ImageStream")));
            this.imgList.TransparentColor = System.Drawing.Color.Transparent;
            this.imgList.Images.SetKeyName(0, "legend-layer.png");
            this.imgList.Images.SetKeyName(1, "property.png");
            // 
            // tabControl2
            // 
            this.tabControl2.Controls.Add(this.TAB_PROPERTIES);
            resources.ApplyResources(this.tabControl2, "tabControl2");
            this.tabControl2.ImageList = this.imgList;
            this.tabControl2.Name = "tabControl2";
            this.tabControl2.SelectedIndex = 0;
            // 
            // TAB_PROPERTIES
            // 
            this.TAB_PROPERTIES.Controls.Add(this.propertyPane);
            resources.ApplyResources(this.TAB_PROPERTIES, "TAB_PROPERTIES");
            this.TAB_PROPERTIES.Name = "TAB_PROPERTIES";
            this.TAB_PROPERTIES.UseVisualStyleBackColor = true;
            // 
            // viewerContextMenu
            // 
            this.viewerContextMenu.Name = "viewerContextMenu";
            resources.ApplyResources(this.viewerContextMenu, "viewerContextMenu");
            // 
            // viewerToolbar
            // 
            resources.ApplyResources(this.viewerToolbar, "viewerToolbar");
            this.viewerToolbar.Name = "viewerToolbar";
            // 
            // taskPaneToolbar
            // 
            this.taskPaneToolbar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnInitialTask,
            this.taskMenu});
            resources.ApplyResources(this.taskPaneToolbar, "taskPaneToolbar");
            this.taskPaneToolbar.Name = "taskPaneToolbar";
            // 
            // btnInitialTask
            // 
            this.btnInitialTask.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.icon_home;
            resources.ApplyResources(this.btnInitialTask, "btnInitialTask");
            this.btnInitialTask.Name = "btnInitialTask";
            this.btnInitialTask.Click += new System.EventHandler(this.btnInitialTask_Click);
            // 
            // taskMenu
            // 
            this.taskMenu.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.taskMenu.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.icon_tasks;
            resources.ApplyResources(this.taskMenu, "taskMenu");
            this.taskMenu.Name = "taskMenu";
            // 
            // mainMenu
            // 
            resources.ApplyResources(this.mainMenu, "mainMenu");
            this.mainMenu.Name = "mainMenu";
            // 
            // appStatusBar
            // 
            this.appStatusBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblCoordinates,
            this.lblSelected,
            this.lblMapScale,
            this.lblMapSize,
            this.lblPoweredBy});
            resources.ApplyResources(this.appStatusBar, "appStatusBar");
            this.appStatusBar.Name = "appStatusBar";
            // 
            // lblCoordinates
            // 
            this.lblCoordinates.Name = "lblCoordinates";
            resources.ApplyResources(this.lblCoordinates, "lblCoordinates");
            // 
            // lblSelected
            // 
            this.lblSelected.Name = "lblSelected";
            resources.ApplyResources(this.lblSelected, "lblSelected");
            this.lblSelected.Spring = true;
            // 
            // lblMapScale
            // 
            this.lblMapScale.Name = "lblMapScale";
            resources.ApplyResources(this.lblMapScale, "lblMapScale");
            // 
            // lblMapSize
            // 
            this.lblMapSize.Name = "lblMapSize";
            resources.ApplyResources(this.lblMapSize, "lblMapSize");
            // 
            // lblPoweredBy
            // 
            this.lblPoweredBy.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.PoweredBy_en;
            resources.ApplyResources(this.lblPoweredBy, "lblPoweredBy");
            this.lblPoweredBy.Name = "lblPoweredBy";
            // 
            // legend
            // 
            resources.ApplyResources(this.legend, "legend");
            this.legend.GroupContextMenu = null;
            this.legend.LayerContextMenu = null;
            this.legend.Name = "legend";
            this.legend.ShowTooltips = true;
            this.legend.ThemeCompressionLimit = 0;
            this.legend.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(this.legend_PropertyChanged);
            // 
            // propertyPane
            // 
            resources.ApplyResources(this.propertyPane, "propertyPane");
            this.propertyPane.Name = "propertyPane";
            // 
            // mapViewer
            // 
            this.mapViewer.ContextMenuStrip = this.viewerContextMenu;
            this.mapViewer.Cursor = System.Windows.Forms.Cursors.Default;
            resources.ApplyResources(this.mapViewer, "mapViewer");
            this.mapViewer.MaxScale = 1000000000;
            this.mapViewer.MinScale = 10;
            this.mapViewer.MouseWheelDelayRenderInterval = 800;
            this.mapViewer.Name = "mapViewer";
            this.mapViewer.PointPixelBuffer = 2;
            this.mapViewer.SelectionColor = System.Drawing.Color.Blue;
            this.mapViewer.TooltipDelayInterval = 1000;
            this.mapViewer.ZoomInFactor = 0.5;
            this.mapViewer.ZoomOutFactor = 2;
            // 
            // taskPane
            // 
            resources.ApplyResources(this.taskPane, "taskPane");
            this.taskPane.Name = "taskPane";
            // 
            // Shell
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.appContainer);
            this.Controls.Add(this.mainMenu);
            this.Controls.Add(this.appStatusBar);
            this.MainMenuStrip = this.mainMenu;
            this.Name = "Shell";
            this.appContainer.Panel1.ResumeLayout(false);
            this.appContainer.Panel1.PerformLayout();
            this.appContainer.Panel2.ResumeLayout(false);
            this.appContainer.Panel2.PerformLayout();
            this.appContainer.ResumeLayout(false);
            this.infoPaneViewerContainer.Panel1.ResumeLayout(false);
            this.infoPaneViewerContainer.Panel2.ResumeLayout(false);
            this.infoPaneViewerContainer.ResumeLayout(false);
            this.layerPropertiesContainer.Panel1.ResumeLayout(false);
            this.layerPropertiesContainer.Panel2.ResumeLayout(false);
            this.layerPropertiesContainer.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.TAB_LEGEND.ResumeLayout(false);
            this.tabControl2.ResumeLayout(false);
            this.TAB_PROPERTIES.ResumeLayout(false);
            this.taskPaneToolbar.ResumeLayout(false);
            this.taskPaneToolbar.PerformLayout();
            this.appStatusBar.ResumeLayout(false);
            this.appStatusBar.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.SplitContainer appContainer;
        private System.Windows.Forms.SplitContainer infoPaneViewerContainer;
        private System.Windows.Forms.SplitContainer layerPropertiesContainer;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage TAB_LEGEND;
        private System.Windows.Forms.TabControl tabControl2;
        private System.Windows.Forms.TabPage TAB_PROPERTIES;
        private System.Windows.Forms.ToolStrip viewerToolbar;
        private MgTaskPane taskPane;
        private System.Windows.Forms.ToolStrip taskPaneToolbar;
        private MgMapViewer mapViewer;
        private System.Windows.Forms.ToolStripButton btnInitialTask;
        private System.Windows.Forms.ToolStripDropDownButton taskMenu;
        private System.Windows.Forms.MenuStrip mainMenu;
        private MgLegend legend;
        private MgPropertyPane propertyPane;
        private System.Windows.Forms.StatusStrip appStatusBar;
        private System.Windows.Forms.ToolStripStatusLabel lblCoordinates;
        private System.Windows.Forms.ToolStripStatusLabel lblSelected;
        private System.Windows.Forms.ToolStripStatusLabel lblMapScale;
        private System.Windows.Forms.ToolStripStatusLabel lblMapSize;
        private System.Windows.Forms.ToolStripStatusLabel lblPoweredBy;
        private System.Windows.Forms.ContextMenuStrip viewerContextMenu;
        private System.Windows.Forms.ImageList imgList;
    }
}