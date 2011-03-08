﻿namespace Maestro.Editors.FeatureSource.Providers
{
    partial class FileBasedCtrl
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FileBasedCtrl));
            this.rdManaged = new System.Windows.Forms.RadioButton();
            this.rdUnmanaged = new System.Windows.Forms.RadioButton();
            this.label1 = new System.Windows.Forms.Label();
            this.txtAlias = new System.Windows.Forms.TextBox();
            this.btnBrowseAlias = new System.Windows.Forms.Button();
            this.tt = new System.Windows.Forms.ToolTip(this.components);
            this.unmanagedPanel = new System.Windows.Forms.Panel();
            this.resDataCtrl = new Maestro.Editors.Common.ResourceDataCtrl();
            this.contentPanel.SuspendLayout();
            this.unmanagedPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // contentPanel
            // 
            this.contentPanel.Controls.Add(this.unmanagedPanel);
            this.contentPanel.Controls.Add(this.rdUnmanaged);
            this.contentPanel.Controls.Add(this.rdManaged);
            this.contentPanel.Controls.Add(this.resDataCtrl);
            // 
            // rdManaged
            // 
            resources.ApplyResources(this.rdManaged, "rdManaged");
            this.rdManaged.Checked = true;
            this.rdManaged.Name = "rdManaged";
            this.rdManaged.TabStop = true;
            this.tt.SetToolTip(this.rdManaged, resources.GetString("rdManaged.ToolTip"));
            this.rdManaged.UseVisualStyleBackColor = true;
            this.rdManaged.CheckedChanged += new System.EventHandler(this.rdManaged_CheckedChanged);
            // 
            // rdUnmanaged
            // 
            resources.ApplyResources(this.rdUnmanaged, "rdUnmanaged");
            this.rdUnmanaged.Name = "rdUnmanaged";
            this.tt.SetToolTip(this.rdUnmanaged, resources.GetString("rdUnmanaged.ToolTip"));
            this.rdUnmanaged.UseVisualStyleBackColor = true;
            this.rdUnmanaged.CheckedChanged += new System.EventHandler(this.rdUnmanaged_CheckedChanged);
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // txtAlias
            // 
            resources.ApplyResources(this.txtAlias, "txtAlias");
            this.txtAlias.Name = "txtAlias";
            // 
            // btnBrowseAlias
            // 
            resources.ApplyResources(this.btnBrowseAlias, "btnBrowseAlias");
            this.btnBrowseAlias.Name = "btnBrowseAlias";
            this.btnBrowseAlias.UseVisualStyleBackColor = true;
            this.btnBrowseAlias.Click += new System.EventHandler(this.btnBrowseAlias_Click);
            // 
            // unmanagedPanel
            // 
            resources.ApplyResources(this.unmanagedPanel, "unmanagedPanel");
            this.unmanagedPanel.Controls.Add(this.label1);
            this.unmanagedPanel.Controls.Add(this.btnBrowseAlias);
            this.unmanagedPanel.Controls.Add(this.txtAlias);
            this.unmanagedPanel.Name = "unmanagedPanel";
            // 
            // resDataCtrl
            // 
            resources.ApplyResources(this.resDataCtrl, "resDataCtrl");
            this.resDataCtrl.MarkedFile = "";
            this.resDataCtrl.MarkEnabled = true;
            this.resDataCtrl.Name = "resDataCtrl";
            // 
            // FileBasedCtrl
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Name = "FileBasedCtrl";
            this.contentPanel.ResumeLayout(false);
            this.contentPanel.PerformLayout();
            this.unmanagedPanel.ResumeLayout(false);
            this.unmanagedPanel.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ToolTip tt;
        private System.Windows.Forms.Panel unmanagedPanel;
        protected Maestro.Editors.Common.ResourceDataCtrl resDataCtrl;
        protected System.Windows.Forms.Button btnBrowseAlias;
        protected System.Windows.Forms.TextBox txtAlias;
        protected System.Windows.Forms.RadioButton rdUnmanaged;
        protected System.Windows.Forms.RadioButton rdManaged;
    }
}
