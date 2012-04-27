namespace OSGeo.MapGuide.Viewer
{
    partial class MgBufferControl
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.lstLayers = new System.Windows.Forms.ListBox();
            this.label2 = new System.Windows.Forms.Label();
            this.cmbUnits = new System.Windows.Forms.ComboBox();
            this.numBufferDistance = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.lblSelectFeatures = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.btnFillBackColor = new System.Windows.Forms.Button();
            this.pnlFillBackColor = new System.Windows.Forms.Panel();
            this.label10 = new System.Windows.Forms.Label();
            this.numFillTransparency = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.btnFillColor = new System.Windows.Forms.Button();
            this.pnlFillColor = new System.Windows.Forms.Panel();
            this.label4 = new System.Windows.Forms.Label();
            this.cmbFillPattern = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.numLineThickness = new System.Windows.Forms.NumericUpDown();
            this.cmbBorderPattern = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.btnBorderColor = new System.Windows.Forms.Button();
            this.label7 = new System.Windows.Forms.Label();
            this.pnlBorderColor = new System.Windows.Forms.Panel();
            this.btnCreate = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.chkMergeBuffers = new System.Windows.Forms.CheckBox();
            this.txtBufferLayer = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.colorPicker = new System.Windows.Forms.ColorDialog();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numBufferDistance)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numFillTransparency)).BeginInit();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numLineThickness)).BeginInit();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.lstLayers);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.cmbUnits);
            this.groupBox1.Controls.Add(this.numBufferDistance);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.lblSelectFeatures);
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(232, 220);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Buffer Settings";
            // 
            // lstLayers
            // 
            this.lstLayers.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lstLayers.FormattingEnabled = true;
            this.lstLayers.Location = new System.Drawing.Point(9, 126);
            this.lstLayers.Name = "lstLayers";
            this.lstLayers.Size = new System.Drawing.Size(206, 82);
            this.lstLayers.TabIndex = 5;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 101);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(180, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Includes the following layers in buffer";
            // 
            // cmbUnits
            // 
            this.cmbUnits.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbUnits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbUnits.FormattingEnabled = true;
            this.cmbUnits.Location = new System.Drawing.Point(105, 66);
            this.cmbUnits.Name = "cmbUnits";
            this.cmbUnits.Size = new System.Drawing.Size(110, 21);
            this.cmbUnits.TabIndex = 3;
            // 
            // numBufferDistance
            // 
            this.numBufferDistance.Location = new System.Drawing.Point(9, 67);
            this.numBufferDistance.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.numBufferDistance.Minimum = new decimal(new int[] {
            100000,
            0,
            0,
            -2147483648});
            this.numBufferDistance.Name = "numBufferDistance";
            this.numBufferDistance.Size = new System.Drawing.Size(92, 20);
            this.numBufferDistance.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 50);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(126, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Distance around features";
            // 
            // lblSelectFeatures
            // 
            this.lblSelectFeatures.BackColor = System.Drawing.Color.White;
            this.lblSelectFeatures.ForeColor = System.Drawing.Color.Blue;
            this.lblSelectFeatures.Location = new System.Drawing.Point(6, 16);
            this.lblSelectFeatures.Name = "lblSelectFeatures";
            this.lblSelectFeatures.Size = new System.Drawing.Size(209, 23);
            this.lblSelectFeatures.TabIndex = 0;
            this.lblSelectFeatures.Text = "Select Features on the Map first";
            this.lblSelectFeatures.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.btnFillBackColor);
            this.groupBox2.Controls.Add(this.pnlFillBackColor);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.numFillTransparency);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.btnFillColor);
            this.groupBox2.Controls.Add(this.pnlFillColor);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.cmbFillPattern);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Location = new System.Drawing.Point(0, 220);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(232, 144);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Fill Style";
            // 
            // btnFillBackColor
            // 
            this.btnFillBackColor.Location = new System.Drawing.Point(183, 102);
            this.btnFillBackColor.Name = "btnFillBackColor";
            this.btnFillBackColor.Size = new System.Drawing.Size(32, 23);
            this.btnFillBackColor.TabIndex = 9;
            this.btnFillBackColor.Text = "...";
            this.btnFillBackColor.UseVisualStyleBackColor = true;
            this.btnFillBackColor.Click += new System.EventHandler(this.btnFillBackColor_Click);
            // 
            // pnlFillBackColor
            // 
            this.pnlFillBackColor.Location = new System.Drawing.Point(95, 103);
            this.pnlFillBackColor.Name = "pnlFillBackColor";
            this.pnlFillBackColor.Size = new System.Drawing.Size(82, 22);
            this.pnlFillBackColor.TabIndex = 8;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(30, 107);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(59, 13);
            this.label10.TabIndex = 7;
            this.label10.Text = "Back Color";
            // 
            // numFillTransparency
            // 
            this.numFillTransparency.Location = new System.Drawing.Point(95, 77);
            this.numFillTransparency.Name = "numFillTransparency";
            this.numFillTransparency.Size = new System.Drawing.Size(120, 20);
            this.numFillTransparency.TabIndex = 6;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(17, 79);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(72, 13);
            this.label5.TabIndex = 5;
            this.label5.Text = "Transparency";
            // 
            // btnFillColor
            // 
            this.btnFillColor.Location = new System.Drawing.Point(183, 48);
            this.btnFillColor.Name = "btnFillColor";
            this.btnFillColor.Size = new System.Drawing.Size(32, 23);
            this.btnFillColor.TabIndex = 4;
            this.btnFillColor.Text = "...";
            this.btnFillColor.UseVisualStyleBackColor = true;
            this.btnFillColor.Click += new System.EventHandler(this.btnFillColor_Click);
            // 
            // pnlFillColor
            // 
            this.pnlFillColor.Location = new System.Drawing.Point(95, 49);
            this.pnlFillColor.Name = "pnlFillColor";
            this.pnlFillColor.Size = new System.Drawing.Size(82, 22);
            this.pnlFillColor.TabIndex = 3;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(34, 53);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(55, 13);
            this.label4.TabIndex = 2;
            this.label4.Text = "Fore Color";
            // 
            // cmbFillPattern
            // 
            this.cmbFillPattern.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbFillPattern.FormattingEnabled = true;
            this.cmbFillPattern.Location = new System.Drawing.Point(95, 23);
            this.cmbFillPattern.Name = "cmbFillPattern";
            this.cmbFillPattern.Size = new System.Drawing.Size(120, 21);
            this.cmbFillPattern.TabIndex = 1;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(48, 26);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(41, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "Pattern";
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.numLineThickness);
            this.groupBox3.Controls.Add(this.cmbBorderPattern);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.btnBorderColor);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.pnlBorderColor);
            this.groupBox3.Location = new System.Drawing.Point(0, 364);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(232, 100);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Border Style";
            // 
            // numLineThickness
            // 
            this.numLineThickness.Location = new System.Drawing.Point(95, 73);
            this.numLineThickness.Name = "numLineThickness";
            this.numLineThickness.Size = new System.Drawing.Size(120, 20);
            this.numLineThickness.TabIndex = 13;
            // 
            // cmbBorderPattern
            // 
            this.cmbBorderPattern.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbBorderPattern.FormattingEnabled = true;
            this.cmbBorderPattern.Location = new System.Drawing.Point(95, 19);
            this.cmbBorderPattern.Name = "cmbBorderPattern";
            this.cmbBorderPattern.Size = new System.Drawing.Size(120, 21);
            this.cmbBorderPattern.TabIndex = 8;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(17, 75);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(72, 13);
            this.label6.TabIndex = 12;
            this.label6.Text = "Transparency";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(48, 22);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(41, 13);
            this.label8.TabIndex = 7;
            this.label8.Text = "Pattern";
            // 
            // btnBorderColor
            // 
            this.btnBorderColor.Location = new System.Drawing.Point(183, 44);
            this.btnBorderColor.Name = "btnBorderColor";
            this.btnBorderColor.Size = new System.Drawing.Size(32, 23);
            this.btnBorderColor.TabIndex = 11;
            this.btnBorderColor.Text = "...";
            this.btnBorderColor.UseVisualStyleBackColor = true;
            this.btnBorderColor.Click += new System.EventHandler(this.btnBorderColor_Click);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(58, 49);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(31, 13);
            this.label7.TabIndex = 9;
            this.label7.Text = "Color";
            // 
            // pnlBorderColor
            // 
            this.pnlBorderColor.Location = new System.Drawing.Point(95, 45);
            this.pnlBorderColor.Name = "pnlBorderColor";
            this.pnlBorderColor.Size = new System.Drawing.Size(82, 22);
            this.pnlBorderColor.TabIndex = 10;
            // 
            // btnCreate
            // 
            this.btnCreate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnCreate.Location = new System.Drawing.Point(9, 567);
            this.btnCreate.Name = "btnCreate";
            this.btnCreate.Size = new System.Drawing.Size(98, 23);
            this.btnCreate.TabIndex = 3;
            this.btnCreate.Text = "Create Buffer";
            this.btnCreate.UseVisualStyleBackColor = true;
            this.btnCreate.Click += new System.EventHandler(this.btnCreate_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox4.Controls.Add(this.chkMergeBuffers);
            this.groupBox4.Controls.Add(this.txtBufferLayer);
            this.groupBox4.Controls.Add(this.label9);
            this.groupBox4.Location = new System.Drawing.Point(0, 464);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(232, 97);
            this.groupBox4.TabIndex = 4;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Other";
            // 
            // chkMergeBuffers
            // 
            this.chkMergeBuffers.AutoSize = true;
            this.chkMergeBuffers.Checked = true;
            this.chkMergeBuffers.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkMergeBuffers.Location = new System.Drawing.Point(12, 69);
            this.chkMergeBuffers.Name = "chkMergeBuffers";
            this.chkMergeBuffers.Size = new System.Drawing.Size(117, 17);
            this.chkMergeBuffers.TabIndex = 2;
            this.chkMergeBuffers.Text = "Merge Buffer Areas";
            this.chkMergeBuffers.UseVisualStyleBackColor = true;
            // 
            // txtBufferLayer
            // 
            this.txtBufferLayer.Location = new System.Drawing.Point(12, 43);
            this.txtBufferLayer.Name = "txtBufferLayer";
            this.txtBufferLayer.Size = new System.Drawing.Size(203, 20);
            this.txtBufferLayer.TabIndex = 1;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(9, 26);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(98, 13);
            this.label9.TabIndex = 0;
            this.label9.Text = "Buffer Layer Name:";
            // 
            // MgBufferControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.btnCreate);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "MgBufferControl";
            this.Size = new System.Drawing.Size(232, 601);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numBufferDistance)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numFillTransparency)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numLineThickness)).EndInit();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label lblSelectFeatures;
        private System.Windows.Forms.ListBox lstLayers;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cmbUnits;
        private System.Windows.Forms.NumericUpDown numBufferDistance;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.NumericUpDown numFillTransparency;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button btnFillColor;
        private System.Windows.Forms.Panel pnlFillColor;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox cmbFillPattern;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.NumericUpDown numLineThickness;
        private System.Windows.Forms.ComboBox cmbBorderPattern;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button btnBorderColor;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Panel pnlBorderColor;
        private System.Windows.Forms.Button btnCreate;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.CheckBox chkMergeBuffers;
        private System.Windows.Forms.TextBox txtBufferLayer;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ColorDialog colorPicker;
        private System.Windows.Forms.Button btnFillBackColor;
        private System.Windows.Forms.Panel pnlFillBackColor;
        private System.Windows.Forms.Label label10;
    }
}
