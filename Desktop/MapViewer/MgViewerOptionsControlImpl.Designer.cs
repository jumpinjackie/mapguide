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
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(113, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Viewer Selection Color";
            // 
            // lblSelectionColor
            // 
            this.lblSelectionColor.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lblSelectionColor.Location = new System.Drawing.Point(157, 16);
            this.lblSelectionColor.Name = "lblSelectionColor";
            this.lblSelectionColor.Size = new System.Drawing.Size(82, 13);
            this.lblSelectionColor.TabIndex = 1;
            this.lblSelectionColor.Text = "                         ";
            // 
            // btnPickSelectionColor
            // 
            this.btnPickSelectionColor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnPickSelectionColor.Location = new System.Drawing.Point(250, 11);
            this.btnPickSelectionColor.Name = "btnPickSelectionColor";
            this.btnPickSelectionColor.Size = new System.Drawing.Size(27, 23);
            this.btnPickSelectionColor.TabIndex = 2;
            this.btnPickSelectionColor.Text = "...";
            this.btnPickSelectionColor.UseVisualStyleBackColor = true;
            this.btnPickSelectionColor.Click += new System.EventHandler(this.btnPickSelectionColor_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(19, 42);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(79, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Zoom In Factor";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(19, 68);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(87, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "Zoom Out Factor";
            // 
            // chkConvertTiledToNonTiled
            // 
            this.chkConvertTiledToNonTiled.AutoSize = true;
            this.chkConvertTiledToNonTiled.Location = new System.Drawing.Point(22, 92);
            this.chkConvertTiledToNonTiled.Name = "chkConvertTiledToNonTiled";
            this.chkConvertTiledToNonTiled.Size = new System.Drawing.Size(247, 17);
            this.chkConvertTiledToNonTiled.TabIndex = 5;
            this.chkConvertTiledToNonTiled.Text = "Display tiled layers as non-tiled (requires restart)";
            this.chkConvertTiledToNonTiled.UseVisualStyleBackColor = true;
            // 
            // numZoomInFactor
            // 
            this.numZoomInFactor.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.numZoomInFactor.DecimalPlaces = 7;
            this.numZoomInFactor.Location = new System.Drawing.Point(157, 40);
            this.numZoomInFactor.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numZoomInFactor.Name = "numZoomInFactor";
            this.numZoomInFactor.Size = new System.Drawing.Size(120, 20);
            this.numZoomInFactor.TabIndex = 6;
            // 
            // numZoomOutFactor
            // 
            this.numZoomOutFactor.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.numZoomOutFactor.DecimalPlaces = 7;
            this.numZoomOutFactor.Location = new System.Drawing.Point(157, 66);
            this.numZoomOutFactor.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numZoomOutFactor.Name = "numZoomOutFactor";
            this.numZoomOutFactor.Size = new System.Drawing.Size(120, 20);
            this.numZoomOutFactor.TabIndex = 7;
            // 
            // chkShowVertexCoordinates
            // 
            this.chkShowVertexCoordinates.AutoSize = true;
            this.chkShowVertexCoordinates.Location = new System.Drawing.Point(22, 116);
            this.chkShowVertexCoordinates.Name = "chkShowVertexCoordinates";
            this.chkShowVertexCoordinates.Size = new System.Drawing.Size(217, 17);
            this.chkShowVertexCoordinates.TabIndex = 8;
            this.chkShowVertexCoordinates.Text = "Show Vertex Coordinates when digitizing";
            this.chkShowVertexCoordinates.UseVisualStyleBackColor = true;
            // 
            // btnSave
            // 
            this.btnSave.Location = new System.Drawing.Point(22, 149);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(75, 23);
            this.btnSave.TabIndex = 9;
            this.btnSave.Text = "Save Changes";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // MgViewerOptionsControlImpl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
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
            this.Size = new System.Drawing.Size(295, 183);
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
