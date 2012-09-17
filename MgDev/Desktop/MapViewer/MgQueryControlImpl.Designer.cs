namespace OSGeo.MapGuide.Viewer
{
    partial class MgQueryControlImpl
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MgQueryControlImpl));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.lnkRefreshLayers = new System.Windows.Forms.LinkLabel();
            this.label1 = new System.Windows.Forms.Label();
            this.cmbLayer = new System.Windows.Forms.ComboBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.txtValue = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.cmbOperator = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.cmbProperty = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.chkPropFilterEnabled = new System.Windows.Forms.CheckBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.lblSpatialFilterGeomSet = new System.Windows.Forms.Label();
            this.spatialFilterButtonPanel = new System.Windows.Forms.FlowLayoutPanel();
            this.btnRectangle = new System.Windows.Forms.Button();
            this.btnPolygon = new System.Windows.Forms.Button();
            this.btnClear = new System.Windows.Forms.Button();
            this.chkSpatialFilter = new System.Windows.Forms.CheckBox();
            this.btnExecute = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.numResults = new System.Windows.Forms.NumericUpDown();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.spatialFilterButtonPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numResults)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.lnkRefreshLayers);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.cmbLayer);
            resources.ApplyResources(this.groupBox1, "groupBox1");
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.TabStop = false;
            // 
            // lnkRefreshLayers
            // 
            resources.ApplyResources(this.lnkRefreshLayers, "lnkRefreshLayers");
            this.lnkRefreshLayers.Name = "lnkRefreshLayers";
            this.lnkRefreshLayers.TabStop = true;
            this.lnkRefreshLayers.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lnkRefreshLayers_LinkClicked);
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // cmbLayer
            // 
            resources.ApplyResources(this.cmbLayer, "cmbLayer");
            this.cmbLayer.DisplayMember = "Name";
            this.cmbLayer.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbLayer.FormattingEnabled = true;
            this.cmbLayer.Name = "cmbLayer";
            this.cmbLayer.SelectedIndexChanged += new System.EventHandler(this.cmbLayer_SelectedIndexChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.txtValue);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.cmbOperator);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.cmbProperty);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.chkPropFilterEnabled);
            resources.ApplyResources(this.groupBox2, "groupBox2");
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.TabStop = false;
            // 
            // txtValue
            // 
            resources.ApplyResources(this.txtValue, "txtValue");
            this.txtValue.Name = "txtValue";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // cmbOperator
            // 
            resources.ApplyResources(this.cmbOperator, "cmbOperator");
            this.cmbOperator.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbOperator.FormattingEnabled = true;
            this.cmbOperator.Name = "cmbOperator";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // cmbProperty
            // 
            resources.ApplyResources(this.cmbProperty, "cmbProperty");
            this.cmbProperty.DisplayMember = "Name";
            this.cmbProperty.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbProperty.FormattingEnabled = true;
            this.cmbProperty.Name = "cmbProperty";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // chkPropFilterEnabled
            // 
            resources.ApplyResources(this.chkPropFilterEnabled, "chkPropFilterEnabled");
            this.chkPropFilterEnabled.Name = "chkPropFilterEnabled";
            this.chkPropFilterEnabled.UseVisualStyleBackColor = true;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.lblSpatialFilterGeomSet);
            this.groupBox3.Controls.Add(this.spatialFilterButtonPanel);
            this.groupBox3.Controls.Add(this.chkSpatialFilter);
            resources.ApplyResources(this.groupBox3, "groupBox3");
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.TabStop = false;
            // 
            // lblSpatialFilterGeomSet
            // 
            resources.ApplyResources(this.lblSpatialFilterGeomSet, "lblSpatialFilterGeomSet");
            this.lblSpatialFilterGeomSet.ForeColor = System.Drawing.Color.Red;
            this.lblSpatialFilterGeomSet.Name = "lblSpatialFilterGeomSet";
            // 
            // spatialFilterButtonPanel
            // 
            resources.ApplyResources(this.spatialFilterButtonPanel, "spatialFilterButtonPanel");
            this.spatialFilterButtonPanel.Controls.Add(this.btnRectangle);
            this.spatialFilterButtonPanel.Controls.Add(this.btnPolygon);
            this.spatialFilterButtonPanel.Controls.Add(this.btnClear);
            this.spatialFilterButtonPanel.Name = "spatialFilterButtonPanel";
            // 
            // btnRectangle
            // 
            resources.ApplyResources(this.btnRectangle, "btnRectangle");
            this.btnRectangle.Name = "btnRectangle";
            this.btnRectangle.UseVisualStyleBackColor = true;
            this.btnRectangle.Click += new System.EventHandler(this.btnRectangle_Click);
            // 
            // btnPolygon
            // 
            resources.ApplyResources(this.btnPolygon, "btnPolygon");
            this.btnPolygon.Name = "btnPolygon";
            this.btnPolygon.UseVisualStyleBackColor = true;
            this.btnPolygon.Click += new System.EventHandler(this.btnPolygon_Click);
            // 
            // btnClear
            // 
            resources.ApplyResources(this.btnClear, "btnClear");
            this.btnClear.Name = "btnClear";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
            // 
            // chkSpatialFilter
            // 
            resources.ApplyResources(this.chkSpatialFilter, "chkSpatialFilter");
            this.chkSpatialFilter.Name = "chkSpatialFilter";
            this.chkSpatialFilter.UseVisualStyleBackColor = true;
            // 
            // btnExecute
            // 
            resources.ApplyResources(this.btnExecute, "btnExecute");
            this.btnExecute.Name = "btnExecute";
            this.btnExecute.UseVisualStyleBackColor = true;
            this.btnExecute.Click += new System.EventHandler(this.btnExecute_Click);
            // 
            // label5
            // 
            resources.ApplyResources(this.label5, "label5");
            this.label5.Name = "label5";
            // 
            // numResults
            // 
            resources.ApplyResources(this.numResults, "numResults");
            this.numResults.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.numResults.Name = "numResults";
            this.numResults.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // MgQueryControlImpl
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.numResults);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.btnExecute);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "MgQueryControlImpl";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.spatialFilterButtonPanel.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.numResults)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox cmbLayer;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.CheckBox chkPropFilterEnabled;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.CheckBox chkSpatialFilter;
        private System.Windows.Forms.TextBox txtValue;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox cmbOperator;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox cmbProperty;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.FlowLayoutPanel spatialFilterButtonPanel;
        private System.Windows.Forms.Button btnRectangle;
        private System.Windows.Forms.Button btnPolygon;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.Button btnExecute;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown numResults;
        private System.Windows.Forms.LinkLabel lnkRefreshLayers;
        private System.Windows.Forms.Label lblSpatialFilterGeomSet;
    }
}
