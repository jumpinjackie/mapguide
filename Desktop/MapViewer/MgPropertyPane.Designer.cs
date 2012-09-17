namespace OSGeo.MapGuide.Viewer
{
    partial class MgPropertyPane
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MgPropertyPane));
            this.propGrid = new System.Windows.Forms.PropertyGrid();
            this.featureToolStrip = new System.Windows.Forms.ToolStrip();
            this.btnPrevFeature = new System.Windows.Forms.ToolStripButton();
            this.txtPosition = new System.Windows.Forms.ToolStripTextBox();
            this.lblCount = new System.Windows.Forms.ToolStripLabel();
            this.btnNextFeature = new System.Windows.Forms.ToolStripButton();
            this.btnZoomSelectedFeature = new System.Windows.Forms.ToolStripButton();
            this.controlToolStrip = new System.Windows.Forms.Panel();
            this.cmbLayer = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.featureToolStrip.SuspendLayout();
            this.controlToolStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // propGrid
            // 
            this.propGrid.CommandsVisibleIfAvailable = false;
            resources.ApplyResources(this.propGrid, "propGrid");
            this.propGrid.Name = "propGrid";
            this.propGrid.PropertySort = System.Windows.Forms.PropertySort.NoSort;
            // 
            // featureToolStrip
            // 
            this.featureToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnPrevFeature,
            this.txtPosition,
            this.lblCount,
            this.btnNextFeature,
            this.btnZoomSelectedFeature});
            resources.ApplyResources(this.featureToolStrip, "featureToolStrip");
            this.featureToolStrip.Name = "featureToolStrip";
            // 
            // btnPrevFeature
            // 
            this.btnPrevFeature.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnPrevFeature.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.control_180;
            resources.ApplyResources(this.btnPrevFeature, "btnPrevFeature");
            this.btnPrevFeature.Name = "btnPrevFeature";
            this.btnPrevFeature.Click += new System.EventHandler(this.btnPrevFeature_Click);
            // 
            // txtPosition
            // 
            this.txtPosition.Name = "txtPosition";
            this.txtPosition.ReadOnly = true;
            resources.ApplyResources(this.txtPosition, "txtPosition");
            // 
            // lblCount
            // 
            this.lblCount.Name = "lblCount";
            resources.ApplyResources(this.lblCount, "lblCount");
            // 
            // btnNextFeature
            // 
            this.btnNextFeature.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnNextFeature.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.control;
            resources.ApplyResources(this.btnNextFeature, "btnNextFeature");
            this.btnNextFeature.Name = "btnNextFeature";
            this.btnNextFeature.Click += new System.EventHandler(this.btnNextFeature_Click);
            // 
            // btnZoomSelectedFeature
            // 
            this.btnZoomSelectedFeature.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.btnZoomSelectedFeature.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnZoomSelectedFeature.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.icon_zoomselect;
            resources.ApplyResources(this.btnZoomSelectedFeature, "btnZoomSelectedFeature");
            this.btnZoomSelectedFeature.Name = "btnZoomSelectedFeature";
            this.btnZoomSelectedFeature.Click += new System.EventHandler(this.btnZoomCurrent_Click);
            // 
            // controlToolStrip
            // 
            this.controlToolStrip.Controls.Add(this.cmbLayer);
            this.controlToolStrip.Controls.Add(this.label1);
            resources.ApplyResources(this.controlToolStrip, "controlToolStrip");
            this.controlToolStrip.Name = "controlToolStrip";
            // 
            // cmbLayer
            // 
            resources.ApplyResources(this.cmbLayer, "cmbLayer");
            this.cmbLayer.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbLayer.FormattingEnabled = true;
            this.cmbLayer.Name = "cmbLayer";
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // MgPropertyPane
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.propGrid);
            this.Controls.Add(this.featureToolStrip);
            this.Controls.Add(this.controlToolStrip);
            this.Name = "MgPropertyPane";
            this.featureToolStrip.ResumeLayout(false);
            this.featureToolStrip.PerformLayout();
            this.controlToolStrip.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PropertyGrid propGrid;
        private System.Windows.Forms.ToolStrip featureToolStrip;
        private System.Windows.Forms.ToolStripButton btnPrevFeature;
        private System.Windows.Forms.ToolStripTextBox txtPosition;
        private System.Windows.Forms.ToolStripLabel lblCount;
        private System.Windows.Forms.ToolStripButton btnNextFeature;
        private System.Windows.Forms.ToolStripButton btnZoomSelectedFeature;
        private System.Windows.Forms.Panel controlToolStrip;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox cmbLayer;
    }
}
