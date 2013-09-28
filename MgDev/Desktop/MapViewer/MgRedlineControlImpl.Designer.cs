namespace OSGeo.MapGuide.Viewer
{
    partial class MgRedlineControlImpl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MgRedlineControlImpl));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.lstRedlineLayers = new System.Windows.Forms.ListView();
            this.layerIcons = new System.Windows.Forms.ImageList(this.components);
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.btnNew = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.btnDeleteRedlineStore = new System.Windows.Forms.ToolStripButton();
            this.btnAddToMap = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.btnRefreshAvailableLayers = new System.Windows.Forms.ToolStripButton();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.lstLayersOnMap = new System.Windows.Forms.ListView();
            this.toolStrip2 = new System.Windows.Forms.ToolStrip();
            this.btnRemoveLayerFromMap = new System.Windows.Forms.ToolStripButton();
            this.btnEditStyle = new System.Windows.Forms.ToolStripButton();
            this.btnEditObjects = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.btnRefreshLayersOnMap = new System.Windows.Forms.ToolStripButton();
            this.mainBody = new System.Windows.Forms.Panel();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.groupBox1.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.toolStrip2.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.lstRedlineLayers);
            this.groupBox1.Controls.Add(this.toolStrip1);
            resources.ApplyResources(this.groupBox1, "groupBox1");
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.TabStop = false;
            // 
            // lstRedlineLayers
            // 
            resources.ApplyResources(this.lstRedlineLayers, "lstRedlineLayers");
            this.lstRedlineLayers.LargeImageList = this.layerIcons;
            this.lstRedlineLayers.MultiSelect = false;
            this.lstRedlineLayers.Name = "lstRedlineLayers";
            this.lstRedlineLayers.SmallImageList = this.layerIcons;
            this.lstRedlineLayers.UseCompatibleStateImageBehavior = false;
            this.lstRedlineLayers.View = System.Windows.Forms.View.List;
            this.lstRedlineLayers.SelectedIndexChanged += new System.EventHandler(this.lstRedlineLayers_SelectedIndexChanged);
            // 
            // layerIcons
            // 
            this.layerIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("layerIcons.ImageStream")));
            this.layerIcons.TransparentColor = System.Drawing.Color.Transparent;
            this.layerIcons.Images.SetKeyName(0, "legend-layer.png");
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnNew,
            this.toolStripSeparator2,
            this.btnDeleteRedlineStore,
            this.btnAddToMap,
            this.toolStripSeparator3,
            this.btnRefreshAvailableLayers});
            resources.ApplyResources(this.toolStrip1, "toolStrip1");
            this.toolStrip1.Name = "toolStrip1";
            // 
            // btnNew
            // 
            this.btnNew.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnNew.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.plus;
            resources.ApplyResources(this.btnNew, "btnNew");
            this.btnNew.Name = "btnNew";
            this.btnNew.Click += new System.EventHandler(this.btnNew_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            resources.ApplyResources(this.toolStripSeparator2, "toolStripSeparator2");
            // 
            // btnDeleteRedlineStore
            // 
            this.btnDeleteRedlineStore.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            resources.ApplyResources(this.btnDeleteRedlineStore, "btnDeleteRedlineStore");
            this.btnDeleteRedlineStore.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.cross_script;
            this.btnDeleteRedlineStore.Name = "btnDeleteRedlineStore";
            this.btnDeleteRedlineStore.Click += new System.EventHandler(this.btnDeleteRedlineStore_Click);
            // 
            // btnAddToMap
            // 
            this.btnAddToMap.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            resources.ApplyResources(this.btnAddToMap, "btnAddToMap");
            this.btnAddToMap.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.map__plus;
            this.btnAddToMap.Name = "btnAddToMap";
            this.btnAddToMap.Click += new System.EventHandler(this.btnAddToMap_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            resources.ApplyResources(this.toolStripSeparator3, "toolStripSeparator3");
            // 
            // btnRefreshAvailableLayers
            // 
            this.btnRefreshAvailableLayers.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnRefreshAvailableLayers.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.view_refresh;
            resources.ApplyResources(this.btnRefreshAvailableLayers, "btnRefreshAvailableLayers");
            this.btnRefreshAvailableLayers.Name = "btnRefreshAvailableLayers";
            this.btnRefreshAvailableLayers.Click += new System.EventHandler(this.btnRefreshAvailableLayers_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.lstLayersOnMap);
            this.groupBox2.Controls.Add(this.toolStrip2);
            resources.ApplyResources(this.groupBox2, "groupBox2");
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.TabStop = false;
            // 
            // lstLayersOnMap
            // 
            resources.ApplyResources(this.lstLayersOnMap, "lstLayersOnMap");
            this.lstLayersOnMap.LargeImageList = this.layerIcons;
            this.lstLayersOnMap.MultiSelect = false;
            this.lstLayersOnMap.Name = "lstLayersOnMap";
            this.lstLayersOnMap.SmallImageList = this.layerIcons;
            this.lstLayersOnMap.UseCompatibleStateImageBehavior = false;
            this.lstLayersOnMap.View = System.Windows.Forms.View.List;
            this.lstLayersOnMap.SelectedIndexChanged += new System.EventHandler(this.lstLayersOnMap_SelectedIndexChanged);
            // 
            // toolStrip2
            // 
            this.toolStrip2.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnRemoveLayerFromMap,
            this.toolStripSeparator4,
            this.btnEditStyle,
            this.btnEditObjects,
            this.toolStripSeparator1,
            this.btnRefreshLayersOnMap});
            resources.ApplyResources(this.toolStrip2, "toolStrip2");
            this.toolStrip2.Name = "toolStrip2";
            // 
            // btnRemoveLayerFromMap
            // 
            this.btnRemoveLayerFromMap.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            resources.ApplyResources(this.btnRemoveLayerFromMap, "btnRemoveLayerFromMap");
            this.btnRemoveLayerFromMap.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.map__minus;
            this.btnRemoveLayerFromMap.Name = "btnRemoveLayerFromMap";
            this.btnRemoveLayerFromMap.Click += new System.EventHandler(this.btnRemoveLayerFromMap_Click);
            // 
            // btnEditStyle
            // 
            this.btnEditStyle.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            resources.ApplyResources(this.btnEditStyle, "btnEditStyle");
            this.btnEditStyle.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.edit_xml;
            this.btnEditStyle.Name = "btnEditStyle";
            this.btnEditStyle.Click += new System.EventHandler(this.btnEditStyle_Click);
            // 
            // btnEditObjects
            // 
            this.btnEditObjects.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            resources.ApplyResources(this.btnEditObjects, "btnEditObjects");
            this.btnEditObjects.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.redline;
            this.btnEditObjects.Name = "btnEditObjects";
            this.btnEditObjects.Click += new System.EventHandler(this.btnEditObjects_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            resources.ApplyResources(this.toolStripSeparator1, "toolStripSeparator1");
            // 
            // btnRefreshLayersOnMap
            // 
            this.btnRefreshLayersOnMap.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnRefreshLayersOnMap.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.view_refresh;
            resources.ApplyResources(this.btnRefreshLayersOnMap, "btnRefreshLayersOnMap");
            this.btnRefreshLayersOnMap.Name = "btnRefreshLayersOnMap";
            this.btnRefreshLayersOnMap.Click += new System.EventHandler(this.btnRefreshLayersOnMap_Click);
            // 
            // mainBody
            // 
            resources.ApplyResources(this.mainBody, "mainBody");
            this.mainBody.Name = "mainBody";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            resources.ApplyResources(this.toolStripSeparator4, "toolStripSeparator4");
            // 
            // MgRedlineControlImpl
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.mainBody);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "MgRedlineControlImpl";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.toolStrip2.ResumeLayout(false);
            this.toolStrip2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ListView lstRedlineLayers;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton btnDeleteRedlineStore;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ListView lstLayersOnMap;
        private System.Windows.Forms.ToolStrip toolStrip2;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton btnAddToMap;
        private System.Windows.Forms.ToolStripButton btnRemoveLayerFromMap;
        private System.Windows.Forms.ToolStripButton btnEditStyle;
        private System.Windows.Forms.ToolStripButton btnEditObjects;
        private System.Windows.Forms.Panel mainBody;
        private System.Windows.Forms.ToolStripButton btnNew;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton btnRefreshAvailableLayers;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton btnRefreshLayersOnMap;
        private System.Windows.Forms.ImageList layerIcons;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;

    }
}
