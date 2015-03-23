namespace OSGeo.MapGuide.Viewer
{
    partial class MgViewerOptionsControlImpl
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MgViewerOptionsControlImpl));
            this.label1 = new System.Windows.Forms.Label();
            this.lblSelectionColor = new System.Windows.Forms.Label();
            this.btnPickSelectionColor = new System.Windows.Forms.Button();
            this.colorDialog = new System.Windows.Forms.ColorDialog();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.chkConvertTiledToNonTiled = new System.Windows.Forms.CheckBox();
            this.numZoomInFactor = new System.Windows.Forms.NumericUpDown();
            this.numZoomOutFactor = new System.Windows.Forms.NumericUpDown();
            this.chkShowVertexCoordinates = new System.Windows.Forms.CheckBox();
            this.btnSave = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.numZoomInFactor)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numZoomOutFactor)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // lblSelectionColor
            // 
            resources.ApplyResources(this.lblSelectionColor, "lblSelectionColor");
            this.lblSelectionColor.Name = "lblSelectionColor";
            // 
            // btnPickSelectionColor
            // 
            resources.ApplyResources(this.btnPickSelectionColor, "btnPickSelectionColor");
            this.btnPickSelectionColor.Name = "btnPickSelectionColor";
            this.btnPickSelectionColor.UseVisualStyleBackColor = true;
            this.btnPickSelectionColor.Click += new System.EventHandler(this.btnPickSelectionColor_Click);
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // chkConvertTiledToNonTiled
            // 
            resources.ApplyResources(this.chkConvertTiledToNonTiled, "chkConvertTiledToNonTiled");
            this.chkConvertTiledToNonTiled.Name = "chkConvertTiledToNonTiled";
            this.chkConvertTiledToNonTiled.UseVisualStyleBackColor = true;
            // 
            // numZoomInFactor
            // 
            resources.ApplyResources(this.numZoomInFactor, "numZoomInFactor");
            this.numZoomInFactor.DecimalPlaces = 7;
            this.numZoomInFactor.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numZoomInFactor.Name = "numZoomInFactor";
            // 
            // numZoomOutFactor
            // 
            resources.ApplyResources(this.numZoomOutFactor, "numZoomOutFactor");
            this.numZoomOutFactor.DecimalPlaces = 7;
            this.numZoomOutFactor.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numZoomOutFactor.Name = "numZoomOutFactor";
            // 
            // chkShowVertexCoordinates
            // 
            resources.ApplyResources(this.chkShowVertexCoordinates, "chkShowVertexCoordinates");
            this.chkShowVertexCoordinates.Name = "chkShowVertexCoordinates";
            this.chkShowVertexCoordinates.UseVisualStyleBackColor = true;
            // 
            // btnSave
            // 
            resources.ApplyResources(this.btnSave, "btnSave");
            this.btnSave.Name = "btnSave";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // MgViewerOptionsControlImpl
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.chkShowVertexCoordinates);
            this.Controls.Add(this.numZoomOutFactor);
            this.Controls.Add(this.numZoomInFactor);
            this.Controls.Add(this.chkConvertTiledToNonTiled);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.btnPickSelectionColor);
            this.Controls.Add(this.lblSelectionColor);
            this.Controls.Add(this.label1);
            this.Name = "MgViewerOptionsControlImpl";
            ((System.ComponentModel.ISupportInitialize)(this.numZoomInFactor)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numZoomOutFactor)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lblSelectionColor;
        private System.Windows.Forms.Button btnPickSelectionColor;
        private System.Windows.Forms.ColorDialog colorDialog;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.CheckBox chkConvertTiledToNonTiled;
        private System.Windows.Forms.NumericUpDown numZoomInFactor;
        private System.Windows.Forms.NumericUpDown numZoomOutFactor;
        private System.Windows.Forms.CheckBox chkShowVertexCoordinates;
        private System.Windows.Forms.Button btnSave;
    }
}
