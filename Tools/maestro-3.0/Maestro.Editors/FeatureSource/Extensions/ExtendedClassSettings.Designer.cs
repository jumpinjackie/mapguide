﻿namespace Maestro.Editors.FeatureSource.Extensions
{
    partial class ExtendedClassSettings
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ExtendedClassSettings));
            this.label1 = new System.Windows.Forms.Label();
            this.txtExtendedName = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.cmbBaseClass = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // txtExtendedName
            // 
            resources.ApplyResources(this.txtExtendedName, "txtExtendedName");
            this.txtExtendedName.Name = "txtExtendedName";
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // cmbBaseClass
            // 
            resources.ApplyResources(this.cmbBaseClass, "cmbBaseClass");
            this.cmbBaseClass.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbBaseClass.FormattingEnabled = true;
            this.cmbBaseClass.Name = "cmbBaseClass";
            // 
            // ExtendedClassSettings
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Controls.Add(this.cmbBaseClass);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtExtendedName);
            this.Controls.Add(this.label1);
            this.Name = "ExtendedClassSettings";
            resources.ApplyResources(this, "$this");
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtExtendedName;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cmbBaseClass;
    }
}
