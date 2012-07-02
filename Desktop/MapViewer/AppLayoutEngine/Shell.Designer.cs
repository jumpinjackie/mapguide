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
            this.appContainer = new System.Windows.Forms.SplitContainer();
            this.infoPaneViewerContainer = new System.Windows.Forms.SplitContainer();
            this.layerPropertiesContainer = new System.Windows.Forms.SplitContainer();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.tabControl2 = new System.Windows.Forms.TabControl();
            this.tabPage3 = new System.Windows.Forms.TabPage();
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
            this.tabPage2.SuspendLayout();
            this.tabControl2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.taskPaneToolbar.SuspendLayout();
            this.appStatusBar.SuspendLayout();
            this.SuspendLayout();
            // 
            // appContainer
            // 
            this.appContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.appContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.appContainer.Location = new System.Drawing.Point(0, 24);
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
            this.appContainer.Size = new System.Drawing.Size(881, 528);
            this.appContainer.SplitterDistance = 662;
            this.appContainer.TabIndex = 1;
            // 
            // infoPaneViewerContainer
            // 
            this.infoPaneViewerContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.infoPaneViewerContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.infoPaneViewerContainer.Location = new System.Drawing.Point(0, 25);
            this.infoPaneViewerContainer.Name = "infoPaneViewerContainer";
            // 
            // infoPaneViewerContainer.Panel1
            // 
            this.infoPaneViewerContainer.Panel1.Controls.Add(this.layerPropertiesContainer);
            // 
            // infoPaneViewerContainer.Panel2
            // 
            this.infoPaneViewerContainer.Panel2.Controls.Add(this.mapViewer);
            this.infoPaneViewerContainer.Size = new System.Drawing.Size(662, 503);
            this.infoPaneViewerContainer.SplitterDistance = 220;
            this.infoPaneViewerContainer.TabIndex = 0;
            // 
            // layerPropertiesContainer
            // 
            this.layerPropertiesContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.layerPropertiesContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.layerPropertiesContainer.Location = new System.Drawing.Point(0, 0);
            this.layerPropertiesContainer.Name = "layerPropertiesContainer";
            this.layerPropertiesContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // layerPropertiesContainer.Panel1
            // 
            this.layerPropertiesContainer.Panel1.Controls.Add(this.tabControl1);
            // 
            // layerPropertiesContainer.Panel2
            // 
            this.layerPropertiesContainer.Panel2.Controls.Add(this.tabControl2);
            this.layerPropertiesContainer.Size = new System.Drawing.Size(220, 503);
            this.layerPropertiesContainer.SplitterDistance = 271;
            this.layerPropertiesContainer.TabIndex = 0;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(220, 271);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.legend);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(212, 245);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Layers";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // tabControl2
            // 
            this.tabControl2.Controls.Add(this.tabPage3);
            this.tabControl2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl2.Location = new System.Drawing.Point(0, 0);
            this.tabControl2.Name = "tabControl2";
            this.tabControl2.SelectedIndex = 0;
            this.tabControl2.Size = new System.Drawing.Size(220, 228);
            this.tabControl2.TabIndex = 0;
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.propertyPane);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(212, 202);
            this.tabPage3.TabIndex = 1;
            this.tabPage3.Text = "Properties";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // viewerContextMenu
            // 
            this.viewerContextMenu.Name = "viewerContextMenu";
            this.viewerContextMenu.Size = new System.Drawing.Size(61, 4);
            // 
            // viewerToolbar
            // 
            this.viewerToolbar.Location = new System.Drawing.Point(0, 0);
            this.viewerToolbar.Name = "viewerToolbar";
            this.viewerToolbar.Size = new System.Drawing.Size(662, 25);
            this.viewerToolbar.TabIndex = 1;
            this.viewerToolbar.Text = "toolStrip2";
            // 
            // taskPaneToolbar
            // 
            this.taskPaneToolbar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnInitialTask,
            this.taskMenu});
            this.taskPaneToolbar.Location = new System.Drawing.Point(0, 0);
            this.taskPaneToolbar.Name = "taskPaneToolbar";
            this.taskPaneToolbar.Size = new System.Drawing.Size(215, 25);
            this.taskPaneToolbar.TabIndex = 0;
            this.taskPaneToolbar.Text = "toolStrip3";
            // 
            // btnInitialTask
            // 
            this.btnInitialTask.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.icon_home;
            this.btnInitialTask.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnInitialTask.Name = "btnInitialTask";
            this.btnInitialTask.Size = new System.Drawing.Size(83, 22);
            this.btnInitialTask.Text = "Initial Task";
            this.btnInitialTask.Click += new System.EventHandler(this.btnInitialTask_Click);
            // 
            // taskMenu
            // 
            this.taskMenu.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.taskMenu.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.icon_tasks;
            this.taskMenu.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.taskMenu.Name = "taskMenu";
            this.taskMenu.Size = new System.Drawing.Size(65, 22);
            this.taskMenu.Text = "Tasks";
            // 
            // mainMenu
            // 
            this.mainMenu.Location = new System.Drawing.Point(0, 0);
            this.mainMenu.Name = "mainMenu";
            this.mainMenu.Size = new System.Drawing.Size(881, 24);
            this.mainMenu.TabIndex = 2;
            this.mainMenu.Text = "menuStrip1";
            // 
            // appStatusBar
            // 
            this.appStatusBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblCoordinates,
            this.lblSelected,
            this.lblMapScale,
            this.lblMapSize,
            this.lblPoweredBy});
            this.appStatusBar.Location = new System.Drawing.Point(0, 552);
            this.appStatusBar.Name = "appStatusBar";
            this.appStatusBar.Size = new System.Drawing.Size(881, 23);
            this.appStatusBar.TabIndex = 3;
            this.appStatusBar.Text = "statusStrip1";
            // 
            // lblCoordinates
            // 
            this.lblCoordinates.Name = "lblCoordinates";
            this.lblCoordinates.Size = new System.Drawing.Size(0, 18);
            // 
            // lblSelected
            // 
            this.lblSelected.Name = "lblSelected";
            this.lblSelected.Size = new System.Drawing.Size(729, 18);
            this.lblSelected.Spring = true;
            this.lblSelected.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lblMapScale
            // 
            this.lblMapScale.Name = "lblMapScale";
            this.lblMapScale.Size = new System.Drawing.Size(0, 18);
            // 
            // lblMapSize
            // 
            this.lblMapSize.Name = "lblMapSize";
            this.lblMapSize.Size = new System.Drawing.Size(0, 18);
            // 
            // lblPoweredBy
            // 
            this.lblPoweredBy.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.PoweredBy_en;
            this.lblPoweredBy.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.lblPoweredBy.Name = "lblPoweredBy";
            this.lblPoweredBy.Size = new System.Drawing.Size(137, 18);
            // 
            // legend
            // 
            this.legend.Dock = System.Windows.Forms.DockStyle.Fill;
            this.legend.GroupContextMenu = null;
            this.legend.LayerContextMenu = null;
            this.legend.Location = new System.Drawing.Point(3, 3);
            this.legend.Name = "legend";
            this.legend.Size = new System.Drawing.Size(206, 239);
            this.legend.TabIndex = 0;
            this.legend.ThemeCompressionLimit = 0;
            // 
            // propertyPane
            // 
            this.propertyPane.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyPane.Location = new System.Drawing.Point(3, 3);
            this.propertyPane.Name = "propertyPane";
            this.propertyPane.Size = new System.Drawing.Size(206, 196);
            this.propertyPane.TabIndex = 0;
            // 
            // mapViewer
            // 
            this.mapViewer.ContextMenuStrip = this.viewerContextMenu;
            this.mapViewer.Cursor = System.Windows.Forms.Cursors.Default;
            this.mapViewer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mapViewer.Location = new System.Drawing.Point(0, 0);
            this.mapViewer.Name = "mapViewer";
            this.mapViewer.SelectionColor = System.Drawing.Color.Blue;
            this.mapViewer.Size = new System.Drawing.Size(438, 503);
            this.mapViewer.TabIndex = 0;
            this.mapViewer.Text = "mgMapViewer1";
            this.mapViewer.ZoomInFactor = 0.5;
            this.mapViewer.ZoomOutFactor = 2;
            // 
            // taskPane
            // 
            this.taskPane.Dock = System.Windows.Forms.DockStyle.Fill;
            this.taskPane.Location = new System.Drawing.Point(0, 25);
            this.taskPane.Name = "taskPane";
            this.taskPane.Size = new System.Drawing.Size(215, 503);
            this.taskPane.TabIndex = 1;
            // 
            // Shell
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(881, 575);
            this.Controls.Add(this.appContainer);
            this.Controls.Add(this.mainMenu);
            this.Controls.Add(this.appStatusBar);
            this.MainMenuStrip = this.mainMenu;
            this.Name = "Shell";
            this.Text = "Shell";
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
            this.tabPage2.ResumeLayout(false);
            this.tabControl2.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
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
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TabControl tabControl2;
        private System.Windows.Forms.TabPage tabPage3;
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
    }
}