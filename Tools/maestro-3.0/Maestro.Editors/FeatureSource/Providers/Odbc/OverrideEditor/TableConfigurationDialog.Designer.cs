﻿namespace Maestro.Editors.FeatureSource.Providers.Odbc.OverrideEditor
{
    partial class TableConfigurationDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TableConfigurationDialog));
            this.btnSave = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.grdTables = new System.Windows.Forms.DataGridView();
            this.tableConfigCtrl = new Maestro.Editors.FeatureSource.Providers.Odbc.OverrideEditor.TableConfigCtrl();
            this.COL_OVERRIDE = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.COL_NAME = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.COL_KEY = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.COL_X = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.COL_Y = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.COL_Z = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.grdTables)).BeginInit();
            this.SuspendLayout();
            // 
            // btnSave
            // 
            resources.ApplyResources(this.btnSave, "btnSave");
            this.btnSave.Name = "btnSave";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnCancel
            // 
            resources.ApplyResources(this.btnCancel, "btnCancel");
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // splitContainer1
            // 
            resources.ApplyResources(this.splitContainer1, "splitContainer1");
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.grdTables);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.tableConfigCtrl);
            // 
            // grdTables
            // 
            this.grdTables.AllowUserToAddRows = false;
            this.grdTables.AllowUserToDeleteRows = false;
            this.grdTables.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.grdTables.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.COL_OVERRIDE,
            this.COL_NAME,
            this.COL_KEY,
            this.COL_X,
            this.COL_Y,
            this.COL_Z});
            resources.ApplyResources(this.grdTables, "grdTables");
            this.grdTables.Name = "grdTables";
            this.grdTables.RowHeadersVisible = false;
            this.grdTables.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.grdTables_CellContentClick);
            this.grdTables.CurrentCellDirtyStateChanged += new System.EventHandler(this.grdTables_CurrentCellDirtyStateChanged);
            this.grdTables.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.grdTables_CellContentClick);
            // 
            // tableConfigCtrl
            // 
            resources.ApplyResources(this.tableConfigCtrl, "tableConfigCtrl");
            this.tableConfigCtrl.Name = "tableConfigCtrl";
            // 
            // COL_OVERRIDE
            // 
            this.COL_OVERRIDE.DataPropertyName = "Override";
            resources.ApplyResources(this.COL_OVERRIDE, "COL_OVERRIDE");
            this.COL_OVERRIDE.Name = "COL_OVERRIDE";
            // 
            // COL_NAME
            // 
            this.COL_NAME.DataPropertyName = "TableName";
            resources.ApplyResources(this.COL_NAME, "COL_NAME");
            this.COL_NAME.Name = "COL_NAME";
            this.COL_NAME.ReadOnly = true;
            // 
            // COL_KEY
            // 
            this.COL_KEY.DataPropertyName = "Key";
            resources.ApplyResources(this.COL_KEY, "COL_KEY");
            this.COL_KEY.Name = "COL_KEY";
            this.COL_KEY.ReadOnly = true;
            // 
            // COL_X
            // 
            this.COL_X.DataPropertyName = "X";
            resources.ApplyResources(this.COL_X, "COL_X");
            this.COL_X.Name = "COL_X";
            this.COL_X.ReadOnly = true;
            // 
            // COL_Y
            // 
            this.COL_Y.DataPropertyName = "Y";
            resources.ApplyResources(this.COL_Y, "COL_Y");
            this.COL_Y.Name = "COL_Y";
            this.COL_Y.ReadOnly = true;
            // 
            // COL_Z
            // 
            this.COL_Z.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.COL_Z.DataPropertyName = "Z";
            resources.ApplyResources(this.COL_Z, "COL_Z");
            this.COL_Z.Name = "COL_Z";
            this.COL_Z.ReadOnly = true;
            // 
            // TableConfigurationDialog
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            resources.ApplyResources(this, "$this");
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnSave);
            this.Name = "TableConfigurationDialog";
            this.ShowIcon = false;
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.grdTables)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.DataGridView grdTables;
        private TableConfigCtrl tableConfigCtrl;
        private System.Windows.Forms.DataGridViewCheckBoxColumn COL_OVERRIDE;
        private System.Windows.Forms.DataGridViewTextBoxColumn COL_NAME;
        private System.Windows.Forms.DataGridViewTextBoxColumn COL_KEY;
        private System.Windows.Forms.DataGridViewTextBoxColumn COL_X;
        private System.Windows.Forms.DataGridViewTextBoxColumn COL_Y;
        private System.Windows.Forms.DataGridViewTextBoxColumn COL_Z;
    }
}