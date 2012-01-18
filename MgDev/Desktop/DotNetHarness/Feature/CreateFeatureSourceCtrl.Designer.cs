namespace DotNetHarness.Feature
{
    partial class CreateFeatureSourceCtrl
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
            this.grpFile = new System.Windows.Forms.GroupBox();
            this.btnCreateFile = new System.Windows.Forms.Button();
            this.txtFileName = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnCreateRdbms = new System.Windows.Forms.Button();
            this.txtDataStore = new System.Windows.Forms.TextBox();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.txtUsername = new System.Windows.Forms.TextBox();
            this.txtService = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.txtResourceId = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtProvider = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.txtCoordinateSystemName = new System.Windows.Forms.TextBox();
            this.txtCoordinateSystemWkt = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.txtSchemaXml = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.txtXYTolerance = new System.Windows.Forms.TextBox();
            this.txtZTolerance = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.btnBrowse = new System.Windows.Forms.Button();
            this.label13 = new System.Windows.Forms.Label();
            this.txtExtents = new System.Windows.Forms.TextBox();
            this.chkFdoMetadata = new System.Windows.Forms.CheckBox();
            this.grpFile.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // grpFile
            // 
            this.grpFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.grpFile.Controls.Add(this.btnCreateFile);
            this.grpFile.Controls.Add(this.txtFileName);
            this.grpFile.Controls.Add(this.label8);
            this.grpFile.Location = new System.Drawing.Point(4, 169);
            this.grpFile.Name = "grpFile";
            this.grpFile.Size = new System.Drawing.Size(627, 66);
            this.grpFile.TabIndex = 0;
            this.grpFile.TabStop = false;
            this.grpFile.Text = "File";
            // 
            // btnCreateFile
            // 
            this.btnCreateFile.Location = new System.Drawing.Point(516, 26);
            this.btnCreateFile.Name = "btnCreateFile";
            this.btnCreateFile.Size = new System.Drawing.Size(75, 23);
            this.btnCreateFile.TabIndex = 2;
            this.btnCreateFile.Text = "Create";
            this.btnCreateFile.UseVisualStyleBackColor = true;
            this.btnCreateFile.Click += new System.EventHandler(this.btnCreateFile_Click);
            // 
            // txtFileName
            // 
            this.txtFileName.Location = new System.Drawing.Point(180, 28);
            this.txtFileName.Name = "txtFileName";
            this.txtFileName.Size = new System.Drawing.Size(241, 20);
            this.txtFileName.TabIndex = 1;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(12, 31);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(54, 13);
            this.label8.TabIndex = 0;
            this.label8.Text = "File Name";
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.chkFdoMetadata);
            this.groupBox1.Controls.Add(this.txtExtents);
            this.groupBox1.Controls.Add(this.label13);
            this.groupBox1.Controls.Add(this.btnCreateRdbms);
            this.groupBox1.Controls.Add(this.txtDataStore);
            this.groupBox1.Controls.Add(this.txtPassword);
            this.groupBox1.Controls.Add(this.txtUsername);
            this.groupBox1.Controls.Add(this.txtService);
            this.groupBox1.Controls.Add(this.label12);
            this.groupBox1.Controls.Add(this.label11);
            this.groupBox1.Controls.Add(this.label10);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Location = new System.Drawing.Point(4, 241);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(627, 186);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "RDBMS";
            // 
            // btnCreateRdbms
            // 
            this.btnCreateRdbms.Location = new System.Drawing.Point(516, 93);
            this.btnCreateRdbms.Name = "btnCreateRdbms";
            this.btnCreateRdbms.Size = new System.Drawing.Size(75, 23);
            this.btnCreateRdbms.TabIndex = 8;
            this.btnCreateRdbms.Text = "Create";
            this.btnCreateRdbms.UseVisualStyleBackColor = true;
            this.btnCreateRdbms.Click += new System.EventHandler(this.btnCreateRdbms_Click);
            // 
            // txtDataStore
            // 
            this.txtDataStore.Location = new System.Drawing.Point(180, 95);
            this.txtDataStore.Name = "txtDataStore";
            this.txtDataStore.Size = new System.Drawing.Size(241, 20);
            this.txtDataStore.TabIndex = 7;
            // 
            // txtPassword
            // 
            this.txtPassword.Location = new System.Drawing.Point(180, 69);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.Size = new System.Drawing.Size(241, 20);
            this.txtPassword.TabIndex = 6;
            // 
            // txtUsername
            // 
            this.txtUsername.Location = new System.Drawing.Point(180, 43);
            this.txtUsername.Name = "txtUsername";
            this.txtUsername.Size = new System.Drawing.Size(241, 20);
            this.txtUsername.TabIndex = 5;
            // 
            // txtService
            // 
            this.txtService.Location = new System.Drawing.Point(180, 17);
            this.txtService.Name = "txtService";
            this.txtService.Size = new System.Drawing.Size(241, 20);
            this.txtService.TabIndex = 4;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(15, 98);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(58, 13);
            this.label12.TabIndex = 3;
            this.label12.Text = "Data Store";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(15, 72);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(53, 13);
            this.label11.TabIndex = 2;
            this.label11.Text = "Password";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(15, 46);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(55, 13);
            this.label10.TabIndex = 1;
            this.label10.Text = "Username";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(15, 20);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(43, 13);
            this.label9.TabIndex = 0;
            this.label9.Text = "Service";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(16, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(67, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Resource ID";
            // 
            // txtResourceId
            // 
            this.txtResourceId.Location = new System.Drawing.Point(184, 13);
            this.txtResourceId.Name = "txtResourceId";
            this.txtResourceId.Size = new System.Drawing.Size(330, 20);
            this.txtResourceId.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(16, 42);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(46, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "Provider";
            // 
            // txtProvider
            // 
            this.txtProvider.Location = new System.Drawing.Point(184, 39);
            this.txtProvider.Name = "txtProvider";
            this.txtProvider.Size = new System.Drawing.Size(330, 20);
            this.txtProvider.TabIndex = 4;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(16, 68);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(126, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Coordinate System Name";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(16, 94);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(123, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Coordinate System WKT";
            // 
            // txtCoordinateSystemName
            // 
            this.txtCoordinateSystemName.Location = new System.Drawing.Point(184, 65);
            this.txtCoordinateSystemName.Name = "txtCoordinateSystemName";
            this.txtCoordinateSystemName.Size = new System.Drawing.Size(330, 20);
            this.txtCoordinateSystemName.TabIndex = 7;
            // 
            // txtCoordinateSystemWkt
            // 
            this.txtCoordinateSystemWkt.Location = new System.Drawing.Point(184, 91);
            this.txtCoordinateSystemWkt.Name = "txtCoordinateSystemWkt";
            this.txtCoordinateSystemWkt.Size = new System.Drawing.Size(330, 20);
            this.txtCoordinateSystemWkt.TabIndex = 8;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(16, 120);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(110, 13);
            this.label5.TabIndex = 9;
            this.label5.Text = "Feature Schema XML";
            // 
            // txtSchemaXml
            // 
            this.txtSchemaXml.Location = new System.Drawing.Point(184, 117);
            this.txtSchemaXml.Name = "txtSchemaXml";
            this.txtSchemaXml.Size = new System.Drawing.Size(330, 20);
            this.txtSchemaXml.TabIndex = 10;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(16, 146);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(72, 13);
            this.label6.TabIndex = 11;
            this.label6.Text = "XY Tolerance";
            // 
            // txtXYTolerance
            // 
            this.txtXYTolerance.Location = new System.Drawing.Point(184, 143);
            this.txtXYTolerance.Name = "txtXYTolerance";
            this.txtXYTolerance.Size = new System.Drawing.Size(105, 20);
            this.txtXYTolerance.TabIndex = 12;
            // 
            // txtZTolerance
            // 
            this.txtZTolerance.Location = new System.Drawing.Point(414, 143);
            this.txtZTolerance.Name = "txtZTolerance";
            this.txtZTolerance.Size = new System.Drawing.Size(100, 20);
            this.txtZTolerance.TabIndex = 14;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(307, 146);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(65, 13);
            this.label7.TabIndex = 13;
            this.label7.Text = "Z Tolerance";
            // 
            // btnBrowse
            // 
            this.btnBrowse.Location = new System.Drawing.Point(520, 115);
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size(75, 23);
            this.btnBrowse.TabIndex = 15;
            this.btnBrowse.Text = "Browse";
            this.btnBrowse.UseVisualStyleBackColor = true;
            this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(15, 135);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(159, 13);
            this.label13.TabIndex = 9;
            this.label13.Text = "Extents (minx, miny, maxx, maxy)";
            // 
            // txtExtents
            // 
            this.txtExtents.Location = new System.Drawing.Point(180, 132);
            this.txtExtents.Name = "txtExtents";
            this.txtExtents.Size = new System.Drawing.Size(330, 20);
            this.txtExtents.TabIndex = 10;
            // 
            // chkFdoMetadata
            // 
            this.chkFdoMetadata.AutoSize = true;
            this.chkFdoMetadata.Location = new System.Drawing.Point(180, 159);
            this.chkFdoMetadata.Name = "chkFdoMetadata";
            this.chkFdoMetadata.Size = new System.Drawing.Size(118, 17);
            this.chkFdoMetadata.TabIndex = 11;
            this.chkFdoMetadata.Text = "Use FDO Metadata";
            this.chkFdoMetadata.UseVisualStyleBackColor = true;
            // 
            // CreateFeatureSourceCtrl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnBrowse);
            this.Controls.Add(this.txtZTolerance);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.txtXYTolerance);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.txtSchemaXml);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.txtCoordinateSystemWkt);
            this.Controls.Add(this.txtCoordinateSystemName);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txtProvider);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtResourceId);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.grpFile);
            this.Name = "CreateFeatureSourceCtrl";
            this.Size = new System.Drawing.Size(634, 440);
            this.grpFile.ResumeLayout(false);
            this.grpFile.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox grpFile;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtResourceId;
        private System.Windows.Forms.TextBox txtProvider;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtCoordinateSystemName;
        private System.Windows.Forms.TextBox txtCoordinateSystemWkt;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txtSchemaXml;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtXYTolerance;
        private System.Windows.Forms.TextBox txtZTolerance;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button btnBrowse;
        private System.Windows.Forms.TextBox txtFileName;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button btnCreateFile;
        private System.Windows.Forms.Button btnCreateRdbms;
        private System.Windows.Forms.TextBox txtDataStore;
        private System.Windows.Forms.TextBox txtPassword;
        private System.Windows.Forms.TextBox txtUsername;
        private System.Windows.Forms.TextBox txtService;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox txtExtents;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.CheckBox chkFdoMetadata;
    }
}
