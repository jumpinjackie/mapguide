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
            this.propGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propGrid.HelpVisible = false;
            this.propGrid.Location = new System.Drawing.Point(0, 30);
            this.propGrid.Name = "propGrid";
            this.propGrid.PropertySort = System.Windows.Forms.PropertySort.NoSort;
            this.propGrid.Size = new System.Drawing.Size(210, 347);
            this.propGrid.TabIndex = 1;
            // 
            // featureToolStrip
            // 
            this.featureToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnPrevFeature,
            this.txtPosition,
            this.lblCount,
            this.btnNextFeature,
            this.btnZoomSelectedFeature});
            this.featureToolStrip.Location = new System.Drawing.Point(0, 30);
            this.featureToolStrip.Name = "featureToolStrip";
            this.featureToolStrip.Size = new System.Drawing.Size(210, 25);
            this.featureToolStrip.TabIndex = 2;
            this.featureToolStrip.Text = "toolStrip1";
            this.featureToolStrip.Visible = false;
            // 
            // btnPrevFeature
            // 
            this.btnPrevFeature.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnPrevFeature.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.control_180;
            this.btnPrevFeature.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnPrevFeature.Name = "btnPrevFeature";
            this.btnPrevFeature.Size = new System.Drawing.Size(23, 22);
            this.btnPrevFeature.Text = "Previous Feature";
            this.btnPrevFeature.Click += new System.EventHandler(this.btnPrevFeature_Click);
            // 
            // txtPosition
            // 
            this.txtPosition.Name = "txtPosition";
            this.txtPosition.ReadOnly = true;
            this.txtPosition.Size = new System.Drawing.Size(50, 25);
            // 
            // lblCount
            // 
            this.lblCount.Name = "lblCount";
            this.lblCount.Size = new System.Drawing.Size(35, 22);
            this.lblCount.Text = "of {0}";
            // 
            // btnNextFeature
            // 
            this.btnNextFeature.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnNextFeature.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.control;
            this.btnNextFeature.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnNextFeature.Name = "btnNextFeature";
            this.btnNextFeature.Size = new System.Drawing.Size(23, 22);
            this.btnNextFeature.Text = "Next Feature";
            this.btnNextFeature.Click += new System.EventHandler(this.btnNextFeature_Click);
            // 
            // btnZoomSelectedFeature
            // 
            this.btnZoomSelectedFeature.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.btnZoomSelectedFeature.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnZoomSelectedFeature.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.icon_zoomselect;
            this.btnZoomSelectedFeature.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnZoomSelectedFeature.Name = "btnZoomSelectedFeature";
            this.btnZoomSelectedFeature.Size = new System.Drawing.Size(23, 22);
            this.btnZoomSelectedFeature.Text = "Zoom to current feature";
            this.btnZoomSelectedFeature.Click += new System.EventHandler(this.btnZoomCurrent_Click);
            // 
            // controlToolStrip
            // 
            this.controlToolStrip.Controls.Add(this.cmbLayer);
            this.controlToolStrip.Controls.Add(this.label1);
            this.controlToolStrip.Dock = System.Windows.Forms.DockStyle.Top;
            this.controlToolStrip.Location = new System.Drawing.Point(0, 0);
            this.controlToolStrip.Name = "controlToolStrip";
            this.controlToolStrip.Size = new System.Drawing.Size(210, 30);
            this.controlToolStrip.TabIndex = 3;
            this.controlToolStrip.Visible = false;
            // 
            // cmbLayer
            // 
            this.cmbLayer.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbLayer.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbLayer.FormattingEnabled = true;
            this.cmbLayer.Location = new System.Drawing.Point(65, 4);
            this.cmbLayer.Name = "cmbLayer";
            this.cmbLayer.Size = new System.Drawing.Size(142, 21);
            this.cmbLayer.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.label1.Location = new System.Drawing.Point(13, 4);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(46, 23);
            this.label1.TabIndex = 0;
            this.label1.Text = "Layer";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // MgPropertyPane
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.propGrid);
            this.Controls.Add(this.featureToolStrip);
            this.Controls.Add(this.controlToolStrip);
            this.Name = "MgPropertyPane";
            this.Size = new System.Drawing.Size(210, 377);
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
