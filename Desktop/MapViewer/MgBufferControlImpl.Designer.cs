namespace OSGeo.MapGuide.Viewer
{
    partial class MgBufferControlImpl
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MgBufferControlImpl));
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
            resources.ApplyResources(this.groupBox1, "groupBox1");
            this.groupBox1.Controls.Add(this.lstLayers);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.cmbUnits);
            this.groupBox1.Controls.Add(this.numBufferDistance);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.lblSelectFeatures);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.TabStop = false;
            // 
            // lstLayers
            // 
            resources.ApplyResources(this.lstLayers, "lstLayers");
            this.lstLayers.FormattingEnabled = true;
            this.lstLayers.Name = "lstLayers";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // cmbUnits
            // 
            resources.ApplyResources(this.cmbUnits, "cmbUnits");
            this.cmbUnits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbUnits.FormattingEnabled = true;
            this.cmbUnits.Name = "cmbUnits";
            // 
            // numBufferDistance
            // 
            this.numBufferDistance.DecimalPlaces = 7;
            resources.ApplyResources(this.numBufferDistance, "numBufferDistance");
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
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // lblSelectFeatures
            // 
            this.lblSelectFeatures.BackColor = System.Drawing.Color.White;
            this.lblSelectFeatures.ForeColor = System.Drawing.Color.Blue;
            resources.ApplyResources(this.lblSelectFeatures, "lblSelectFeatures");
            this.lblSelectFeatures.Name = "lblSelectFeatures";
            // 
            // groupBox2
            // 
            resources.ApplyResources(this.groupBox2, "groupBox2");
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
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.TabStop = false;
            // 
            // btnFillBackColor
            // 
            resources.ApplyResources(this.btnFillBackColor, "btnFillBackColor");
            this.btnFillBackColor.Name = "btnFillBackColor";
            this.btnFillBackColor.UseVisualStyleBackColor = true;
            this.btnFillBackColor.Click += new System.EventHandler(this.btnFillBackColor_Click);
            // 
            // pnlFillBackColor
            // 
            resources.ApplyResources(this.pnlFillBackColor, "pnlFillBackColor");
            this.pnlFillBackColor.Name = "pnlFillBackColor";
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
            // label5
            // 
            resources.ApplyResources(this.label5, "label5");
            this.label5.Name = "label5";
            // 
            // btnFillColor
            // 
            resources.ApplyResources(this.btnFillColor, "btnFillColor");
            this.btnFillColor.Name = "btnFillColor";
            this.btnFillColor.UseVisualStyleBackColor = true;
            this.btnFillColor.Click += new System.EventHandler(this.btnFillColor_Click);
            // 
            // pnlFillColor
            // 
            resources.ApplyResources(this.pnlFillColor, "pnlFillColor");
            this.pnlFillColor.Name = "pnlFillColor";
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // cmbFillPattern
            // 
            this.cmbFillPattern.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbFillPattern.FormattingEnabled = true;
            resources.ApplyResources(this.cmbFillPattern, "cmbFillPattern");
            this.cmbFillPattern.Name = "cmbFillPattern";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // groupBox3
            // 
            resources.ApplyResources(this.groupBox3, "groupBox3");
            this.groupBox3.Controls.Add(this.numLineThickness);
            this.groupBox3.Controls.Add(this.cmbBorderPattern);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.btnBorderColor);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.pnlBorderColor);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.TabStop = false;
            // 
            // numLineThickness
            // 
            resources.ApplyResources(this.numLineThickness, "numLineThickness");
            this.numLineThickness.Name = "numLineThickness";
            // 
            // cmbBorderPattern
            // 
            this.cmbBorderPattern.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbBorderPattern.FormattingEnabled = true;
            resources.ApplyResources(this.cmbBorderPattern, "cmbBorderPattern");
            this.cmbBorderPattern.Name = "cmbBorderPattern";
            // 
            // label6
            // 
            resources.ApplyResources(this.label6, "label6");
            this.label6.Name = "label6";
            // 
            // label8
            // 
            resources.ApplyResources(this.label8, "label8");
            this.label8.Name = "label8";
            // 
            // btnBorderColor
            // 
            resources.ApplyResources(this.btnBorderColor, "btnBorderColor");
            this.btnBorderColor.Name = "btnBorderColor";
            this.btnBorderColor.UseVisualStyleBackColor = true;
            this.btnBorderColor.Click += new System.EventHandler(this.btnBorderColor_Click);
            // 
            // label7
            // 
            resources.ApplyResources(this.label7, "label7");
            this.label7.Name = "label7";
            // 
            // pnlBorderColor
            // 
            resources.ApplyResources(this.pnlBorderColor, "pnlBorderColor");
            this.pnlBorderColor.Name = "pnlBorderColor";
            // 
            // btnCreate
            // 
            resources.ApplyResources(this.btnCreate, "btnCreate");
            this.btnCreate.Name = "btnCreate";
            this.btnCreate.UseVisualStyleBackColor = true;
            this.btnCreate.Click += new System.EventHandler(this.btnCreate_Click);
            // 
            // groupBox4
            // 
            resources.ApplyResources(this.groupBox4, "groupBox4");
            this.groupBox4.Controls.Add(this.chkMergeBuffers);
            this.groupBox4.Controls.Add(this.txtBufferLayer);
            this.groupBox4.Controls.Add(this.label9);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.TabStop = false;
            // 
            // chkMergeBuffers
            // 
            resources.ApplyResources(this.chkMergeBuffers, "chkMergeBuffers");
            this.chkMergeBuffers.Checked = true;
            this.chkMergeBuffers.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkMergeBuffers.Name = "chkMergeBuffers";
            this.chkMergeBuffers.UseVisualStyleBackColor = true;
            // 
            // txtBufferLayer
            // 
            resources.ApplyResources(this.txtBufferLayer, "txtBufferLayer");
            this.txtBufferLayer.Name = "txtBufferLayer";
            // 
            // label9
            // 
            resources.ApplyResources(this.label9, "label9");
            this.label9.Name = "label9";
            // 
            // MgBufferControlImpl
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.btnCreate);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "MgBufferControlImpl";
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
