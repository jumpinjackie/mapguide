﻿namespace InstantSetup
{
    partial class Form1
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnConfigure = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.TAB_APACHE = new System.Windows.Forms.TabPage();
            this.TAB_IIS7 = new System.Windows.Forms.TabPage();
            this.label1 = new System.Windows.Forms.Label();
            this.txtServiceName = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtSourceDirectory = new System.Windows.Forms.TextBox();
            this.btnSourceDir = new System.Windows.Forms.Button();
            this.chkInstallServices = new System.Windows.Forms.CheckBox();
            this.folderBrowser = new System.Windows.Forms.FolderBrowserDialog();
            this.button1 = new System.Windows.Forms.Button();
            this.txtBatchOutput = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.chkWriteMentorDictPath = new System.Windows.Forms.CheckBox();
            this.chk64Bit = new System.Windows.Forms.CheckBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label7 = new System.Windows.Forms.Label();
            this.chkProviders = new System.Windows.Forms.CheckedListBox();
            this.label6 = new System.Windows.Forms.Label();
            this.btnFdoRegUtilPath = new System.Windows.Forms.Button();
            this.btnFdoPath = new System.Windows.Forms.Button();
            this.txtFdoRegUtilPath = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.txtFdoDir = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.tabControl1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnConfigure
            // 
            this.btnConfigure.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnConfigure.Location = new System.Drawing.Point(830, 563);
            this.btnConfigure.Name = "btnConfigure";
            this.btnConfigure.Size = new System.Drawing.Size(75, 23);
            this.btnConfigure.TabIndex = 0;
            this.btnConfigure.Text = "Configure";
            this.btnConfigure.UseVisualStyleBackColor = true;
            this.btnConfigure.Click += new System.EventHandler(this.btnConfigure_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.TAB_APACHE);
            this.tabControl1.Controls.Add(this.TAB_IIS7);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(3, 16);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(580, 428);
            this.tabControl1.TabIndex = 1;
            // 
            // TAB_APACHE
            // 
            this.TAB_APACHE.Location = new System.Drawing.Point(4, 22);
            this.TAB_APACHE.Name = "TAB_APACHE";
            this.TAB_APACHE.Padding = new System.Windows.Forms.Padding(3);
            this.TAB_APACHE.Size = new System.Drawing.Size(572, 402);
            this.TAB_APACHE.TabIndex = 1;
            this.TAB_APACHE.Text = "Apache";
            this.TAB_APACHE.UseVisualStyleBackColor = true;
            // 
            // TAB_IIS7
            // 
            this.TAB_IIS7.Location = new System.Drawing.Point(4, 22);
            this.TAB_IIS7.Name = "TAB_IIS7";
            this.TAB_IIS7.Padding = new System.Windows.Forms.Padding(3);
            this.TAB_IIS7.Size = new System.Drawing.Size(466, 397);
            this.TAB_IIS7.TabIndex = 2;
            this.TAB_IIS7.Text = "IIS7";
            this.TAB_IIS7.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(14, 64);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(126, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "MapGuide Service Name";
            // 
            // txtServiceName
            // 
            this.txtServiceName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtServiceName.Location = new System.Drawing.Point(156, 61);
            this.txtServiceName.Name = "txtServiceName";
            this.txtServiceName.Size = new System.Drawing.Size(745, 20);
            this.txtServiceName.TabIndex = 3;
            this.txtServiceName.TextChanged += new System.EventHandler(this.txtServiceName_TextChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(14, 12);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(86, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Source Directory";
            // 
            // txtSourceDirectory
            // 
            this.txtSourceDirectory.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtSourceDirectory.Location = new System.Drawing.Point(156, 9);
            this.txtSourceDirectory.Name = "txtSourceDirectory";
            this.txtSourceDirectory.Size = new System.Drawing.Size(710, 20);
            this.txtSourceDirectory.TabIndex = 5;
            this.txtSourceDirectory.TextChanged += new System.EventHandler(this.txtSourceDirectory_TextChanged);
            // 
            // btnSourceDir
            // 
            this.btnSourceDir.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnSourceDir.Location = new System.Drawing.Point(872, 7);
            this.btnSourceDir.Name = "btnSourceDir";
            this.btnSourceDir.Size = new System.Drawing.Size(29, 23);
            this.btnSourceDir.TabIndex = 6;
            this.btnSourceDir.Text = "...";
            this.btnSourceDir.UseVisualStyleBackColor = true;
            this.btnSourceDir.Click += new System.EventHandler(this.btnSourceDir_Click);
            // 
            // chkInstallServices
            // 
            this.chkInstallServices.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.chkInstallServices.AutoSize = true;
            this.chkInstallServices.Location = new System.Drawing.Point(13, 567);
            this.chkInstallServices.Name = "chkInstallServices";
            this.chkInstallServices.Size = new System.Drawing.Size(97, 17);
            this.chkInstallServices.TabIndex = 7;
            this.chkInstallServices.Text = "Install Services";
            this.chkInstallServices.UseVisualStyleBackColor = true;
            this.chkInstallServices.Visible = false;
            this.chkInstallServices.CheckedChanged += new System.EventHandler(this.chkInstallServices_CheckedChanged);
            // 
            // button1
            // 
            this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button1.Location = new System.Drawing.Point(872, 33);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(29, 23);
            this.button1.TabIndex = 10;
            this.button1.Text = "...";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // txtBatchOutput
            // 
            this.txtBatchOutput.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtBatchOutput.Location = new System.Drawing.Point(156, 35);
            this.txtBatchOutput.Name = "txtBatchOutput";
            this.txtBatchOutput.Size = new System.Drawing.Size(710, 20);
            this.txtBatchOutput.TabIndex = 9;
            this.txtBatchOutput.TextChanged += new System.EventHandler(this.txtBatchOutput_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(14, 38);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(134, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Batch File Output Directory";
            // 
            // chkWriteMentorDictPath
            // 
            this.chkWriteMentorDictPath.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.chkWriteMentorDictPath.AutoSize = true;
            this.chkWriteMentorDictPath.Location = new System.Drawing.Point(116, 567);
            this.chkWriteMentorDictPath.Name = "chkWriteMentorDictPath";
            this.chkWriteMentorDictPath.Size = new System.Drawing.Size(271, 17);
            this.chkWriteMentorDictPath.TabIndex = 11;
            this.chkWriteMentorDictPath.Text = "Write MENTOR_DICTIONARY_PATH to batch files";
            this.chkWriteMentorDictPath.UseVisualStyleBackColor = true;
            this.chkWriteMentorDictPath.CheckedChanged += new System.EventHandler(this.chkWriteMentorDictPath_CheckedChanged);
            // 
            // chk64Bit
            // 
            this.chk64Bit.AutoSize = true;
            this.chk64Bit.Location = new System.Drawing.Point(156, 87);
            this.chk64Bit.Name = "chk64Bit";
            this.chk64Bit.Size = new System.Drawing.Size(201, 17);
            this.chk64Bit.TabIndex = 12;
            this.chk64Bit.Text = "This is a 64-bit instance of MapGuide";
            this.chk64Bit.UseVisualStyleBackColor = true;
            this.chk64Bit.CheckedChanged += new System.EventHandler(this.chk64Bit_CheckedChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.tabControl1);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(586, 447);
            this.groupBox1.TabIndex = 13;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Web";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.chkProviders);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.btnFdoRegUtilPath);
            this.groupBox2.Controls.Add(this.btnFdoPath);
            this.groupBox2.Controls.Add(this.txtFdoRegUtilPath);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.txtFdoDir);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox2.Location = new System.Drawing.Point(0, 0);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(294, 447);
            this.groupBox2.TabIndex = 14;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Server";
            // 
            // label7
            // 
            this.label7.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label7.Location = new System.Drawing.Point(19, 386);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(257, 45);
            this.label7.TabIndex = 8;
            this.label7.Text = "NOTE: Some providers (eg. MySQL) require 3rd party external dependencies in order" +
                " to sucessfully register";
            // 
            // chkProviders
            // 
            this.chkProviders.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.chkProviders.CheckOnClick = true;
            this.chkProviders.FormattingEnabled = true;
            this.chkProviders.HorizontalScrollbar = true;
            this.chkProviders.Location = new System.Drawing.Point(20, 137);
            this.chkProviders.Name = "chkProviders";
            this.chkProviders.ScrollAlwaysVisible = true;
            this.chkProviders.Size = new System.Drawing.Size(256, 244);
            this.chkProviders.TabIndex = 7;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(16, 121);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(113, 13);
            this.label6.TabIndex = 6;
            this.label6.Text = "Provider dlls to register";
            // 
            // btnFdoRegUtilPath
            // 
            this.btnFdoRegUtilPath.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnFdoRegUtilPath.Location = new System.Drawing.Point(249, 86);
            this.btnFdoRegUtilPath.Name = "btnFdoRegUtilPath";
            this.btnFdoRegUtilPath.Size = new System.Drawing.Size(27, 23);
            this.btnFdoRegUtilPath.TabIndex = 5;
            this.btnFdoRegUtilPath.Text = "...";
            this.btnFdoRegUtilPath.UseVisualStyleBackColor = true;
            this.btnFdoRegUtilPath.Click += new System.EventHandler(this.btnFdoRegUtilPath_Click);
            // 
            // btnFdoPath
            // 
            this.btnFdoPath.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnFdoPath.Location = new System.Drawing.Point(249, 41);
            this.btnFdoPath.Name = "btnFdoPath";
            this.btnFdoPath.Size = new System.Drawing.Size(27, 23);
            this.btnFdoPath.TabIndex = 4;
            this.btnFdoPath.Text = "...";
            this.btnFdoPath.UseVisualStyleBackColor = true;
            this.btnFdoPath.Click += new System.EventHandler(this.btnFdoPath_Click);
            // 
            // txtFdoRegUtilPath
            // 
            this.txtFdoRegUtilPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFdoRegUtilPath.Location = new System.Drawing.Point(20, 88);
            this.txtFdoRegUtilPath.Name = "txtFdoRegUtilPath";
            this.txtFdoRegUtilPath.Size = new System.Drawing.Size(223, 20);
            this.txtFdoRegUtilPath.TabIndex = 3;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(16, 72);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(85, 13);
            this.label5.TabIndex = 2;
            this.label5.Text = "FdoRegUtil Path";
            // 
            // txtFdoDir
            // 
            this.txtFdoDir.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFdoDir.Location = new System.Drawing.Point(20, 43);
            this.txtFdoDir.Name = "txtFdoDir";
            this.txtFdoDir.Size = new System.Drawing.Size(223, 20);
            this.txtFdoDir.TabIndex = 1;
            this.txtFdoDir.TextChanged += new System.EventHandler(this.txtFdoDir_TextChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(16, 27);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(54, 13);
            this.label4.TabIndex = 0;
            this.label4.Text = "FDO Path";
            // 
            // splitContainer1
            // 
            this.splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(17, 110);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.groupBox2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.groupBox1);
            this.splitContainer1.Size = new System.Drawing.Size(884, 447);
            this.splitContainer1.SplitterDistance = 294;
            this.splitContainer1.TabIndex = 15;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(917, 598);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.chk64Bit);
            this.Controls.Add(this.chkWriteMentorDictPath);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.txtBatchOutput);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.chkInstallServices);
            this.Controls.Add(this.btnSourceDir);
            this.Controls.Add(this.txtSourceDirectory);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtServiceName);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnConfigure);
            this.Name = "Form1";
            this.Text = "MapGuide Instant Setup";
            this.tabControl1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnConfigure;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage TAB_APACHE;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtServiceName;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtSourceDirectory;
        private System.Windows.Forms.Button btnSourceDir;
        private System.Windows.Forms.CheckBox chkInstallServices;
        private System.Windows.Forms.FolderBrowserDialog folderBrowser;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox txtBatchOutput;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.CheckBox chkWriteMentorDictPath;
        private System.Windows.Forms.TabPage TAB_IIS7;
        private System.Windows.Forms.CheckBox chk64Bit;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox txtFdoDir;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.CheckedListBox chkProviders;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btnFdoRegUtilPath;
        private System.Windows.Forms.Button btnFdoPath;
        private System.Windows.Forms.TextBox txtFdoRegUtilPath;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.SplitContainer splitContainer1;
    }
}

