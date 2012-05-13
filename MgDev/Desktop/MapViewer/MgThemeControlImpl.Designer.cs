namespace OSGeo.MapGuide.Viewer
{
    partial class MgThemeControlImpl
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
            this.txtThemeName = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.cmbLayer = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.numRules = new System.Windows.Forms.NumericUpDown();
            this.cmbScaleRange = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            this.txtMax = new System.Windows.Forms.TextBox();
            this.txtMin = new System.Windows.Forms.TextBox();
            this.cmbDistribution = new System.Windows.Forms.ComboBox();
            this.cmbProperty = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.btnToBorderColor = new System.Windows.Forms.Button();
            this.pnlToBorderColor = new System.Windows.Forms.Panel();
            this.btnFromBorderColor = new System.Windows.Forms.Button();
            this.btnToFillColor = new System.Windows.Forms.Button();
            this.pnlFromBorderColor = new System.Windows.Forms.Panel();
            this.btnFromFillColor = new System.Windows.Forms.Button();
            this.pnlToFillColor = new System.Windows.Forms.Panel();
            this.pnlFromFillColor = new System.Windows.Forms.Panel();
            this.label14 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.numFillTransparency = new System.Windows.Forms.NumericUpDown();
            this.label9 = new System.Windows.Forms.Label();
            this.btnApply = new System.Windows.Forms.Button();
            this.colorDialog = new System.Windows.Forms.ColorDialog();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numRules)).BeginInit();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numFillTransparency)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.txtThemeName);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.cmbLayer);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(269, 112);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "General";
            // 
            // txtThemeName
            // 
            this.txtThemeName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtThemeName.Location = new System.Drawing.Point(15, 79);
            this.txtThemeName.Name = "txtThemeName";
            this.txtThemeName.Size = new System.Drawing.Size(237, 20);
            this.txtThemeName.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 63);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(71, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Theme Name";
            // 
            // cmbLayer
            // 
            this.cmbLayer.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbLayer.DisplayMember = "Name";
            this.cmbLayer.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbLayer.FormattingEnabled = true;
            this.cmbLayer.Location = new System.Drawing.Point(15, 35);
            this.cmbLayer.Name = "cmbLayer";
            this.cmbLayer.Size = new System.Drawing.Size(237, 21);
            this.cmbLayer.TabIndex = 1;
            this.cmbLayer.SelectedIndexChanged += new System.EventHandler(this.cmbLayer_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 19);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(57, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Map Layer";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.numRules);
            this.groupBox2.Controls.Add(this.cmbScaleRange);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.txtMax);
            this.groupBox2.Controls.Add(this.txtMin);
            this.groupBox2.Controls.Add(this.cmbDistribution);
            this.groupBox2.Controls.Add(this.cmbProperty);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox2.Location = new System.Drawing.Point(0, 112);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(269, 276);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Theme Conditions";
            // 
            // numRules
            // 
            this.numRules.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.numRules.Location = new System.Drawing.Point(110, 198);
            this.numRules.Name = "numRules";
            this.numRules.Size = new System.Drawing.Size(141, 20);
            this.numRules.TabIndex = 12;
            // 
            // cmbScaleRange
            // 
            this.cmbScaleRange.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbScaleRange.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbScaleRange.FormattingEnabled = true;
            this.cmbScaleRange.Location = new System.Drawing.Point(15, 241);
            this.cmbScaleRange.Name = "cmbScaleRange";
            this.cmbScaleRange.Size = new System.Drawing.Size(237, 21);
            this.cmbScaleRange.TabIndex = 11;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(12, 225);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(69, 13);
            this.label8.TabIndex = 10;
            this.label8.Text = "Scale Range";
            // 
            // txtMax
            // 
            this.txtMax.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtMax.Location = new System.Drawing.Point(15, 123);
            this.txtMax.Name = "txtMax";
            this.txtMax.Size = new System.Drawing.Size(237, 20);
            this.txtMax.TabIndex = 9;
            // 
            // txtMin
            // 
            this.txtMin.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtMin.Location = new System.Drawing.Point(15, 79);
            this.txtMin.Name = "txtMin";
            this.txtMin.Size = new System.Drawing.Size(237, 20);
            this.txtMin.TabIndex = 8;
            // 
            // cmbDistribution
            // 
            this.cmbDistribution.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbDistribution.DisplayMember = "Value";
            this.cmbDistribution.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbDistribution.FormattingEnabled = true;
            this.cmbDistribution.Location = new System.Drawing.Point(15, 167);
            this.cmbDistribution.Name = "cmbDistribution";
            this.cmbDistribution.Size = new System.Drawing.Size(237, 21);
            this.cmbDistribution.TabIndex = 7;
            this.cmbDistribution.ValueMember = "Key";
            this.cmbDistribution.SelectedIndexChanged += new System.EventHandler(this.cmbDistribution_SelectedIndexChanged);
            // 
            // cmbProperty
            // 
            this.cmbProperty.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbProperty.DisplayMember = "Name";
            this.cmbProperty.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbProperty.FormattingEnabled = true;
            this.cmbProperty.Location = new System.Drawing.Point(15, 36);
            this.cmbProperty.Name = "cmbProperty";
            this.cmbProperty.Size = new System.Drawing.Size(237, 21);
            this.cmbProperty.TabIndex = 6;
            this.cmbProperty.SelectedIndexChanged += new System.EventHandler(this.cmbProperty_SelectedIndexChanged);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(12, 200);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(86, 13);
            this.label7.TabIndex = 4;
            this.label7.Text = "Number of Rules";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 151);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(59, 13);
            this.label6.TabIndex = 3;
            this.label6.Text = "Distribution";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 107);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(27, 13);
            this.label5.TabIndex = 2;
            this.label5.Text = "Max";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 63);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(24, 13);
            this.label4.TabIndex = 1;
            this.label4.Text = "Min";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 20);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(46, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "Property";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.btnToBorderColor);
            this.groupBox3.Controls.Add(this.pnlToBorderColor);
            this.groupBox3.Controls.Add(this.btnFromBorderColor);
            this.groupBox3.Controls.Add(this.btnToFillColor);
            this.groupBox3.Controls.Add(this.pnlFromBorderColor);
            this.groupBox3.Controls.Add(this.btnFromFillColor);
            this.groupBox3.Controls.Add(this.pnlToFillColor);
            this.groupBox3.Controls.Add(this.pnlFromFillColor);
            this.groupBox3.Controls.Add(this.label14);
            this.groupBox3.Controls.Add(this.label15);
            this.groupBox3.Controls.Add(this.label13);
            this.groupBox3.Controls.Add(this.label12);
            this.groupBox3.Controls.Add(this.label11);
            this.groupBox3.Controls.Add(this.label10);
            this.groupBox3.Controls.Add(this.numFillTransparency);
            this.groupBox3.Controls.Add(this.label9);
            this.groupBox3.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox3.Location = new System.Drawing.Point(0, 388);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(269, 162);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Style Ramp";
            // 
            // btnToBorderColor
            // 
            this.btnToBorderColor.Location = new System.Drawing.Point(222, 123);
            this.btnToBorderColor.Name = "btnToBorderColor";
            this.btnToBorderColor.Size = new System.Drawing.Size(29, 23);
            this.btnToBorderColor.TabIndex = 13;
            this.btnToBorderColor.Text = "...";
            this.btnToBorderColor.UseVisualStyleBackColor = true;
            this.btnToBorderColor.Click += new System.EventHandler(this.btnToBorderColor_Click);
            // 
            // pnlToBorderColor
            // 
            this.pnlToBorderColor.BackColor = System.Drawing.Color.Black;
            this.pnlToBorderColor.Location = new System.Drawing.Point(178, 123);
            this.pnlToBorderColor.Name = "pnlToBorderColor";
            this.pnlToBorderColor.Size = new System.Drawing.Size(38, 23);
            this.pnlToBorderColor.TabIndex = 12;
            // 
            // btnFromBorderColor
            // 
            this.btnFromBorderColor.Location = new System.Drawing.Point(110, 123);
            this.btnFromBorderColor.Name = "btnFromBorderColor";
            this.btnFromBorderColor.Size = new System.Drawing.Size(29, 23);
            this.btnFromBorderColor.TabIndex = 11;
            this.btnFromBorderColor.Text = "...";
            this.btnFromBorderColor.UseVisualStyleBackColor = true;
            this.btnFromBorderColor.Click += new System.EventHandler(this.btnFromBorderColor_Click);
            // 
            // btnToFillColor
            // 
            this.btnToFillColor.Location = new System.Drawing.Point(222, 71);
            this.btnToFillColor.Name = "btnToFillColor";
            this.btnToFillColor.Size = new System.Drawing.Size(29, 23);
            this.btnToFillColor.TabIndex = 11;
            this.btnToFillColor.Text = "...";
            this.btnToFillColor.UseVisualStyleBackColor = true;
            this.btnToFillColor.Click += new System.EventHandler(this.btnToFillColor_Click);
            // 
            // pnlFromBorderColor
            // 
            this.pnlFromBorderColor.BackColor = System.Drawing.Color.Black;
            this.pnlFromBorderColor.Location = new System.Drawing.Point(66, 123);
            this.pnlFromBorderColor.Name = "pnlFromBorderColor";
            this.pnlFromBorderColor.Size = new System.Drawing.Size(38, 23);
            this.pnlFromBorderColor.TabIndex = 10;
            // 
            // btnFromFillColor
            // 
            this.btnFromFillColor.Location = new System.Drawing.Point(110, 71);
            this.btnFromFillColor.Name = "btnFromFillColor";
            this.btnFromFillColor.Size = new System.Drawing.Size(29, 23);
            this.btnFromFillColor.TabIndex = 9;
            this.btnFromFillColor.Text = "...";
            this.btnFromFillColor.UseVisualStyleBackColor = true;
            this.btnFromFillColor.Click += new System.EventHandler(this.btnFromFillColor_Click);
            // 
            // pnlToFillColor
            // 
            this.pnlToFillColor.BackColor = System.Drawing.Color.Blue;
            this.pnlToFillColor.Location = new System.Drawing.Point(178, 71);
            this.pnlToFillColor.Name = "pnlToFillColor";
            this.pnlToFillColor.Size = new System.Drawing.Size(38, 23);
            this.pnlToFillColor.TabIndex = 10;
            // 
            // pnlFromFillColor
            // 
            this.pnlFromFillColor.BackColor = System.Drawing.Color.Red;
            this.pnlFromFillColor.Location = new System.Drawing.Point(66, 71);
            this.pnlFromFillColor.Name = "pnlFromFillColor";
            this.pnlFromFillColor.Size = new System.Drawing.Size(38, 23);
            this.pnlFromFillColor.TabIndex = 8;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(152, 128);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(20, 13);
            this.label14.TabIndex = 7;
            this.label14.Text = "To";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(30, 128);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(30, 13);
            this.label15.TabIndex = 6;
            this.label15.Text = "From";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(152, 76);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(20, 13);
            this.label13.TabIndex = 5;
            this.label13.Text = "To";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(30, 76);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(30, 13);
            this.label12.TabIndex = 4;
            this.label12.Text = "From";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(12, 107);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(65, 13);
            this.label11.TabIndex = 3;
            this.label11.Text = "Border Color";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(12, 52);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(46, 13);
            this.label10.TabIndex = 2;
            this.label10.Text = "Fill Color";
            // 
            // numFillTransparency
            // 
            this.numFillTransparency.Location = new System.Drawing.Point(123, 25);
            this.numFillTransparency.Name = "numFillTransparency";
            this.numFillTransparency.Size = new System.Drawing.Size(65, 20);
            this.numFillTransparency.TabIndex = 1;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(12, 27);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(104, 13);
            this.label9.TabIndex = 0;
            this.label9.Text = "Fill Transparency (%)";
            // 
            // btnApply
            // 
            this.btnApply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnApply.Location = new System.Drawing.Point(177, 556);
            this.btnApply.Name = "btnApply";
            this.btnApply.Size = new System.Drawing.Size(75, 23);
            this.btnApply.TabIndex = 3;
            this.btnApply.Text = "Apply";
            this.btnApply.UseVisualStyleBackColor = true;
            this.btnApply.Click += new System.EventHandler(this.btnApply_Click);
            // 
            // MgThemeControlImpl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnApply);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "MgThemeControlImpl";
            this.Size = new System.Drawing.Size(269, 593);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numRules)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numFillTransparency)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cmbLayer;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtThemeName;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ComboBox cmbScaleRange;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txtMax;
        private System.Windows.Forms.TextBox txtMin;
        private System.Windows.Forms.ComboBox cmbDistribution;
        private System.Windows.Forms.ComboBox cmbProperty;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button btnToBorderColor;
        private System.Windows.Forms.Panel pnlToBorderColor;
        private System.Windows.Forms.Button btnFromBorderColor;
        private System.Windows.Forms.Button btnToFillColor;
        private System.Windows.Forms.Panel pnlFromBorderColor;
        private System.Windows.Forms.Button btnFromFillColor;
        private System.Windows.Forms.Panel pnlToFillColor;
        private System.Windows.Forms.Panel pnlFromFillColor;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.NumericUpDown numFillTransparency;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button btnApply;
        private System.Windows.Forms.NumericUpDown numRules;
        private System.Windows.Forms.ColorDialog colorDialog;
    }
}
