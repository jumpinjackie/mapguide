namespace InstantSetup
{
    partial class IISConfigCtrl
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
            this.label1 = new System.Windows.Forms.Label();
            this.txtAppCmd = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtWebSiteName = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.txtVirtualDir = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.numSitePort = new System.Windows.Forms.NumericUpDown();
            this.numClientPort = new System.Windows.Forms.NumericUpDown();
            this.numAdminPort = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.rdDotNet = new System.Windows.Forms.RadioButton();
            this.rdPhp = new System.Windows.Forms.RadioButton();
            this.label7 = new System.Windows.Forms.Label();
            this.txtAppPool = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numSitePort)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numClientPort)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numAdminPort)).BeginInit();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.txtAppPool);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.txtVirtualDir);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.txtWebSiteName);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.txtAppCmd);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(12, 133);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(390, 163);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "IIS General";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(17, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(89, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "appcmd.exe path";
            // 
            // txtAppCmd
            // 
            this.txtAppCmd.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtAppCmd.Location = new System.Drawing.Point(20, 36);
            this.txtAppCmd.Name = "txtAppCmd";
            this.txtAppCmd.Size = new System.Drawing.Size(346, 20);
            this.txtAppCmd.TabIndex = 1;
            this.txtAppCmd.TextChanged += new System.EventHandler(this.txtAppCmd_TextChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(17, 73);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(82, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Web Site Name";
            // 
            // txtWebSiteName
            // 
            this.txtWebSiteName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtWebSiteName.Location = new System.Drawing.Point(144, 70);
            this.txtWebSiteName.Name = "txtWebSiteName";
            this.txtWebSiteName.Size = new System.Drawing.Size(222, 20);
            this.txtWebSiteName.TabIndex = 3;
            this.txtWebSiteName.TextChanged += new System.EventHandler(this.txtWebSiteName_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(17, 99);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(112, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "Virtual Directory Name";
            // 
            // txtVirtualDir
            // 
            this.txtVirtualDir.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtVirtualDir.Location = new System.Drawing.Point(144, 96);
            this.txtVirtualDir.Name = "txtVirtualDir";
            this.txtVirtualDir.Size = new System.Drawing.Size(222, 20);
            this.txtVirtualDir.TabIndex = 5;
            this.txtVirtualDir.TextChanged += new System.EventHandler(this.txtVirtualDir_TextChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.numSitePort);
            this.groupBox2.Controls.Add(this.numClientPort);
            this.groupBox2.Controls.Add(this.numAdminPort);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Location = new System.Drawing.Point(12, 12);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(390, 115);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Port Numbers";
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
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(23, 80);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(87, 13);
            this.label4.TabIndex = 2;
            this.label4.Text = "Site Connections";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(23, 54);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(95, 13);
            this.label5.TabIndex = 1;
            this.label5.Text = "Client Connections";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(23, 26);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(98, 13);
            this.label6.TabIndex = 0;
            this.label6.Text = "Admin Connections";
            // 
            // groupBox4
            // 
            this.groupBox4.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox4.Controls.Add(this.rdDotNet);
            this.groupBox4.Controls.Add(this.rdPhp);
            this.groupBox4.Location = new System.Drawing.Point(12, 302);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(390, 50);
            this.groupBox4.TabIndex = 4;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Preferred Default Viewer";
            // 
            // rdDotNet
            // 
            this.rdDotNet.AutoSize = true;
            this.rdDotNet.Location = new System.Drawing.Point(111, 19);
            this.rdDotNet.Name = "rdDotNet";
            this.rdDotNet.Size = new System.Drawing.Size(45, 17);
            this.rdDotNet.TabIndex = 1;
            this.rdDotNet.TabStop = true;
            this.rdDotNet.Text = ".Net";
            this.rdDotNet.UseVisualStyleBackColor = true;
            this.rdDotNet.CheckedChanged += new System.EventHandler(this.rdDotNet_CheckedChanged);
            // 
            // rdPhp
            // 
            this.rdPhp.AutoSize = true;
            this.rdPhp.Location = new System.Drawing.Point(26, 19);
            this.rdPhp.Name = "rdPhp";
            this.rdPhp.Size = new System.Drawing.Size(47, 17);
            this.rdPhp.TabIndex = 0;
            this.rdPhp.TabStop = true;
            this.rdPhp.Text = "PHP";
            this.rdPhp.UseVisualStyleBackColor = true;
            this.rdPhp.CheckedChanged += new System.EventHandler(this.rdPhp_CheckedChanged);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(17, 125);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(83, 13);
            this.label7.TabIndex = 6;
            this.label7.Text = "Application Pool";
            // 
            // txtAppPool
            // 
            this.txtAppPool.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtAppPool.Location = new System.Drawing.Point(144, 122);
            this.txtAppPool.Name = "txtAppPool";
            this.txtAppPool.Size = new System.Drawing.Size(222, 20);
            this.txtAppPool.TabIndex = 7;
            this.txtAppPool.TextChanged += new System.EventHandler(this.txtAppPool_TextChanged);
            // 
            // IISConfigCtrl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "IISConfigCtrl";
            this.Size = new System.Drawing.Size(422, 371);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numSitePort)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numClientPort)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numAdminPort)).EndInit();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox txtAppCmd;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtVirtualDir;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtWebSiteName;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.NumericUpDown numSitePort;
        private System.Windows.Forms.NumericUpDown numClientPort;
        private System.Windows.Forms.NumericUpDown numAdminPort;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.RadioButton rdDotNet;
        private System.Windows.Forms.RadioButton rdPhp;
        private System.Windows.Forms.TextBox txtAppPool;
        private System.Windows.Forms.Label label7;
    }
}
