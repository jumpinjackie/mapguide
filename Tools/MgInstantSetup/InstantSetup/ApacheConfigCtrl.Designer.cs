namespace InstantSetup
{
    partial class ApacheConfigCtrl
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
            this.numSitePort = new System.Windows.Forms.NumericUpDown();
            this.numClientPort = new System.Windows.Forms.NumericUpDown();
            this.numAdminPort = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.txtServiceName = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.numHttpdPort = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.txtVirtualDirectory = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.chkEnableJava = new System.Windows.Forms.CheckBox();
            this.numTomcatPort = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numSitePort)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numClientPort)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numAdminPort)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numHttpdPort)).BeginInit();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numTomcatPort)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.numSitePort);
            this.groupBox1.Controls.Add(this.numClientPort);
            this.groupBox1.Controls.Add(this.numAdminPort);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(4, 4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(440, 115);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Port Numbers";
            // 
            // numSitePort
            // 
            this.numSitePort.Location = new System.Drawing.Point(154, 78);
            this.numSitePort.Maximum = new decimal(new int[] {
            65536,
            0,
            0,
            0});
            this.numSitePort.Name = "numSitePort";
            this.numSitePort.Size = new System.Drawing.Size(120, 20);
            this.numSitePort.TabIndex = 5;
            this.numSitePort.ValueChanged += new System.EventHandler(this.numSitePort_ValueChanged);
            // 
            // numClientPort
            // 
            this.numClientPort.Location = new System.Drawing.Point(154, 52);
            this.numClientPort.Maximum = new decimal(new int[] {
            65536,
            0,
            0,
            0});
            this.numClientPort.Name = "numClientPort";
            this.numClientPort.Size = new System.Drawing.Size(120, 20);
            this.numClientPort.TabIndex = 4;
            this.numClientPort.ValueChanged += new System.EventHandler(this.numClientPort_ValueChanged);
            // 
            // numAdminPort
            // 
            this.numAdminPort.Location = new System.Drawing.Point(154, 26);
            this.numAdminPort.Maximum = new decimal(new int[] {
            65536,
            0,
            0,
            0});
            this.numAdminPort.Name = "numAdminPort";
            this.numAdminPort.Size = new System.Drawing.Size(120, 20);
            this.numAdminPort.TabIndex = 3;
            this.numAdminPort.ValueChanged += new System.EventHandler(this.numAdminPort_ValueChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(23, 80);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(87, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Site Connections";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(23, 54);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(95, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Client Connections";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(23, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(98, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Admin Connections";
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.txtServiceName);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.numHttpdPort);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.txtVirtualDirectory);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Location = new System.Drawing.Point(4, 126);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(440, 114);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "General";
            // 
            // txtServiceName
            // 
            this.txtServiceName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtServiceName.Location = new System.Drawing.Point(154, 77);
            this.txtServiceName.Name = "txtServiceName";
            this.txtServiceName.Size = new System.Drawing.Size(236, 20);
            this.txtServiceName.TabIndex = 11;
            this.txtServiceName.TextChanged += new System.EventHandler(this.txtServiceName_TextChanged);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(23, 80);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(74, 13);
            this.label7.TabIndex = 10;
            this.label7.Text = "Service Name";
            // 
            // numHttpdPort
            // 
            this.numHttpdPort.Location = new System.Drawing.Point(154, 51);
            this.numHttpdPort.Maximum = new decimal(new int[] {
            65536,
            0,
            0,
            0});
            this.numHttpdPort.Name = "numHttpdPort";
            this.numHttpdPort.Size = new System.Drawing.Size(120, 20);
            this.numHttpdPort.TabIndex = 9;
            this.numHttpdPort.ValueChanged += new System.EventHandler(this.numHttpdPort_ValueChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(23, 53);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(93, 13);
            this.label6.TabIndex = 8;
            this.label6.Text = "httpd Port Number";
            // 
            // txtVirtualDirectory
            // 
            this.txtVirtualDirectory.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtVirtualDirectory.Location = new System.Drawing.Point(154, 25);
            this.txtVirtualDirectory.Name = "txtVirtualDirectory";
            this.txtVirtualDirectory.Size = new System.Drawing.Size(236, 20);
            this.txtVirtualDirectory.TabIndex = 1;
            this.txtVirtualDirectory.TextChanged += new System.EventHandler(this.txtVirtualDirectory_TextChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(23, 28);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(112, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "Virtual Directory Name";
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.chkEnableJava);
            this.groupBox3.Controls.Add(this.numTomcatPort);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Location = new System.Drawing.Point(4, 246);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(440, 106);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            // 
            // chkEnableJava
            // 
            this.chkEnableJava.AutoSize = true;
            this.chkEnableJava.Location = new System.Drawing.Point(6, 0);
            this.chkEnableJava.Name = "chkEnableJava";
            this.chkEnableJava.Size = new System.Drawing.Size(85, 17);
            this.chkEnableJava.TabIndex = 0;
            this.chkEnableJava.Text = "Enable Java";
            this.chkEnableJava.UseVisualStyleBackColor = true;
            this.chkEnableJava.CheckedChanged += new System.EventHandler(this.chkEnableJava_CheckedChanged);
            // 
            // numTomcatPort
            // 
            this.numTomcatPort.Location = new System.Drawing.Point(154, 29);
            this.numTomcatPort.Maximum = new decimal(new int[] {
            65536,
            0,
            0,
            0});
            this.numTomcatPort.Name = "numTomcatPort";
            this.numTomcatPort.Size = new System.Drawing.Size(120, 20);
            this.numTomcatPort.TabIndex = 7;
            this.numTomcatPort.ValueChanged += new System.EventHandler(this.numTomcatPort_ValueChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(23, 31);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(105, 13);
            this.label5.TabIndex = 6;
            this.label5.Text = "Tomcat Port Number";
            // 
            // ApacheConfigCtrl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "ApacheConfigCtrl";
            this.Size = new System.Drawing.Size(447, 355);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numSitePort)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numClientPort)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numAdminPort)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numHttpdPort)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numTomcatPort)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numSitePort;
        private System.Windows.Forms.NumericUpDown numClientPort;
        private System.Windows.Forms.NumericUpDown numAdminPort;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox txtVirtualDirectory;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.CheckBox chkEnableJava;
        private System.Windows.Forms.NumericUpDown numTomcatPort;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown numHttpdPort;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtServiceName;
        private System.Windows.Forms.Label label7;
    }
}
