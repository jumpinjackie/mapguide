﻿namespace Maestro.Editors.Common
{
    partial class SymbolPicker
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SymbolPicker));
            this.label1 = new System.Windows.Forms.Label();
            this.txtSymbolLibrary = new System.Windows.Forms.TextBox();
            this.grpAvailableSymbols = new System.Windows.Forms.GroupBox();
            this.lstSymbols = new System.Windows.Forms.ListView();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.grpAvailableSymbols.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // txtSymbolLibrary
            // 
            resources.ApplyResources(this.txtSymbolLibrary, "txtSymbolLibrary");
            this.txtSymbolLibrary.Name = "txtSymbolLibrary";
            this.txtSymbolLibrary.ReadOnly = true;
            // 
            // grpAvailableSymbols
            // 
            resources.ApplyResources(this.grpAvailableSymbols, "grpAvailableSymbols");
            this.grpAvailableSymbols.Controls.Add(this.lstSymbols);
            this.grpAvailableSymbols.Name = "grpAvailableSymbols";
            this.grpAvailableSymbols.TabStop = false;
            // 
            // lstSymbols
            // 
            resources.ApplyResources(this.lstSymbols, "lstSymbols");
            this.lstSymbols.MultiSelect = false;
            this.lstSymbols.Name = "lstSymbols";
            this.lstSymbols.UseCompatibleStateImageBehavior = false;
            this.lstSymbols.View = System.Windows.Forms.View.Tile;
            this.lstSymbols.SelectedIndexChanged += new System.EventHandler(this.lstSymbols_SelectedIndexChanged);
            // 
            // btnOK
            // 
            resources.ApplyResources(this.btnOK, "btnOK");
            this.btnOK.Name = "btnOK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            resources.ApplyResources(this.btnCancel, "btnCancel");
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // SymbolPicker
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            resources.ApplyResources(this, "$this");
            this.ControlBox = false;
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.grpAvailableSymbols);
            this.Controls.Add(this.txtSymbolLibrary);
            this.Controls.Add(this.label1);
            this.Name = "SymbolPicker";
            this.grpAvailableSymbols.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtSymbolLibrary;
        private System.Windows.Forms.GroupBox grpAvailableSymbols;
        private System.Windows.Forms.ListView lstSymbols;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
    }
}