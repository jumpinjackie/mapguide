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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MgThemeControlImpl));
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
            resources.ApplyResources(this.groupBox1, "groupBox1");
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.TabStop = false;
            // 
            // txtThemeName
            // 
            resources.ApplyResources(this.txtThemeName, "txtThemeName");
            this.txtThemeName.Name = "txtThemeName";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
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
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
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
            resources.ApplyResources(this.groupBox2, "groupBox2");
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.TabStop = false;
            // 
            // numRules
            // 
            resources.ApplyResources(this.numRules, "numRules");
            this.numRules.Name = "numRules";
            // 
            // cmbScaleRange
            // 
            resources.ApplyResources(this.cmbScaleRange, "cmbScaleRange");
            this.cmbScaleRange.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbScaleRange.FormattingEnabled = true;
            this.cmbScaleRange.Name = "cmbScaleRange";
            // 
            // label8
            // 
            resources.ApplyResources(this.label8, "label8");
            this.label8.Name = "label8";
            // 
            // txtMax
            // 
            resources.ApplyResources(this.txtMax, "txtMax");
            this.txtMax.Name = "txtMax";
            // 
            // txtMin
            // 
            resources.ApplyResources(this.txtMin, "txtMin");
            this.txtMin.Name = "txtMin";
            // 
            // cmbDistribution
            // 
            resources.ApplyResources(this.cmbDistribution, "cmbDistribution");
            this.cmbDistribution.DisplayMember = "Value";
            this.cmbDistribution.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbDistribution.FormattingEnabled = true;
            this.cmbDistribution.Name = "cmbDistribution";
            this.cmbDistribution.ValueMember = "Key";
            this.cmbDistribution.SelectedIndexChanged += new System.EventHandler(this.cmbDistribution_SelectedIndexChanged);
            // 
            // cmbProperty
            // 
            resources.ApplyResources(this.cmbProperty, "cmbProperty");
            this.cmbProperty.DisplayMember = "Name";
            this.cmbProperty.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbProperty.FormattingEnabled = true;
            this.cmbProperty.Name = "cmbProperty";
            this.cmbProperty.SelectedIndexChanged += new System.EventHandler(this.cmbProperty_SelectedIndexChanged);
            // 
            // label7
            // 
            resources.ApplyResources(this.label7, "label7");
            this.label7.Name = "label7";
            // 
            // label6
            // 
            resources.ApplyResources(this.label6, "label6");
            this.label6.Name = "label6";
            // 
            // label5
            // 
            resources.ApplyResources(this.label5, "label5");
            this.label5.Name = "label5";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
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
            resources.ApplyResources(this.groupBox3, "groupBox3");
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.TabStop = false;
            // 
            // btnToBorderColor
            // 
            resources.ApplyResources(this.btnToBorderColor, "btnToBorderColor");
            this.btnToBorderColor.Name = "btnToBorderColor";
            this.btnToBorderColor.UseVisualStyleBackColor = true;
            this.btnToBorderColor.Click += new System.EventHandler(this.btnToBorderColor_Click);
            // 
            // pnlToBorderColor
            // 
            this.pnlToBorderColor.BackColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.pnlToBorderColor, "pnlToBorderColor");
            this.pnlToBorderColor.Name = "pnlToBorderColor";
            // 
            // btnFromBorderColor
            // 
            resources.ApplyResources(this.btnFromBorderColor, "btnFromBorderColor");
            this.btnFromBorderColor.Name = "btnFromBorderColor";
            this.btnFromBorderColor.UseVisualStyleBackColor = true;
            this.btnFromBorderColor.Click += new System.EventHandler(this.btnFromBorderColor_Click);
            // 
            // btnToFillColor
            // 
            resources.ApplyResources(this.btnToFillColor, "btnToFillColor");
            this.btnToFillColor.Name = "btnToFillColor";
            this.btnToFillColor.UseVisualStyleBackColor = true;
            this.btnToFillColor.Click += new System.EventHandler(this.btnToFillColor_Click);
            // 
            // pnlFromBorderColor
            // 
            this.pnlFromBorderColor.BackColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.pnlFromBorderColor, "pnlFromBorderColor");
            this.pnlFromBorderColor.Name = "pnlFromBorderColor";
            // 
            // btnFromFillColor
            // 
            resources.ApplyResources(this.btnFromFillColor, "btnFromFillColor");
            this.btnFromFillColor.Name = "btnFromFillColor";
            this.btnFromFillColor.UseVisualStyleBackColor = true;
            this.btnFromFillColor.Click += new System.EventHandler(this.btnFromFillColor_Click);
            // 
            // pnlToFillColor
            // 
            this.pnlToFillColor.BackColor = System.Drawing.Color.Blue;
            resources.ApplyResources(this.pnlToFillColor, "pnlToFillColor");
            this.pnlToFillColor.Name = "pnlToFillColor";
            // 
            // pnlFromFillColor
            // 
            this.pnlFromFillColor.BackColor = System.Drawing.Color.Red;
            resources.ApplyResources(this.pnlFromFillColor, "pnlFromFillColor");
            this.pnlFromFillColor.Name = "pnlFromFillColor";
            // 
            // label14
            // 
            resources.ApplyResources(this.label14, "label14");
            this.label14.Name = "label14";
            // 
            // label15
            // 
            resources.ApplyResources(this.label15, "label15");
            this.label15.Name = "label15";
            // 
            // label13
            // 
            resources.ApplyResources(this.label13, "label13");
            this.label13.Name = "label13";
            // 
            // label12
            // 
            resources.ApplyResources(this.label12, "label12");
            this.label12.Name = "label12";
            // 
            // label11
            // 
            resources.ApplyResources(this.label11, "label11");
            this.label11.Name = "label11";
            // 
            // label10
            // 
            resources.ApplyResources(this.label10, "label10");
            this.label10.Name = "label10";
            // 
            // numFillTransparency
            // 
            resources.ApplyResources(this.numFillTransparency, "numFillTransparency");
            this.numFillTransparency.Name = "numFillTransparency";
            // 
            // label9
            // 
            resources.ApplyResources(this.label9, "label9");
            this.label9.Name = "label9";
            // 
            // btnApply
            // 
            resources.ApplyResources(this.btnApply, "btnApply");
            this.btnApply.Name = "btnApply";
            this.btnApply.UseVisualStyleBackColor = true;
            this.btnApply.Click += new System.EventHandler(this.btnApply_Click);
            // 
            // MgThemeControlImpl
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnApply);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "MgThemeControlImpl";
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
