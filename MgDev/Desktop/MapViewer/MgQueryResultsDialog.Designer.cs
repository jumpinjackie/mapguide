namespace OSGeo.MapGuide.Viewer
{
    partial class MgQueryResultsDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MgQueryResultsDialog));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.btnSelect = new System.Windows.Forms.ToolStripButton();
            this.btnZoom = new System.Windows.Forms.ToolStripButton();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.lblResults = new System.Windows.Forms.ToolStripStatusLabel();
            this.grdResults = new System.Windows.Forms.DataGridView();
            this.bgGridLoader = new System.ComponentModel.BackgroundWorker();
            this.toolStrip1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.grdResults)).BeginInit();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnSelect,
            this.btnZoom});
            resources.ApplyResources(this.toolStrip1, "toolStrip1");
            this.toolStrip1.Name = "toolStrip1";
            // 
            // btnSelect
            // 
            resources.ApplyResources(this.btnSelect, "btnSelect");
            this.btnSelect.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.select_features;
            this.btnSelect.Name = "btnSelect";
            this.btnSelect.Click += new System.EventHandler(this.btnSelect_Click);
            // 
            // btnZoom
            // 
            resources.ApplyResources(this.btnZoom, "btnZoom");
            this.btnZoom.Image = global::OSGeo.MapGuide.Viewer.Properties.Resources.search;
            this.btnZoom.Name = "btnZoom";
            this.btnZoom.Click += new System.EventHandler(this.btnZoom_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblResults});
            resources.ApplyResources(this.statusStrip1, "statusStrip1");
            this.statusStrip1.Name = "statusStrip1";
            // 
            // lblResults
            // 
            this.lblResults.Name = "lblResults";
            resources.ApplyResources(this.lblResults, "lblResults");
            // 
            // grdResults
            // 
            this.grdResults.AllowUserToAddRows = false;
            this.grdResults.AllowUserToDeleteRows = false;
            this.grdResults.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            resources.ApplyResources(this.grdResults, "grdResults");
            this.grdResults.Name = "grdResults";
            this.grdResults.ReadOnly = true;
            this.grdResults.SelectionChanged += new System.EventHandler(this.grdResults_SelectionChanged);
            // 
            // bgGridLoader
            // 
            this.bgGridLoader.WorkerReportsProgress = true;
            this.bgGridLoader.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgGridLoader_DoWork);
            this.bgGridLoader.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgGridLoader_RunWorkerCompleted);
            this.bgGridLoader.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bgGridLoader_ProgressChanged);
            // 
            // MgQueryResultsDialog
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.grdResults);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.toolStrip1);
            this.Name = "MgQueryResultsDialog";
            this.ShowIcon = false;
            this.TopMost = true;
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.grdResults)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.DataGridView grdResults;
        private System.Windows.Forms.ToolStripButton btnSelect;
        private System.Windows.Forms.ToolStripButton btnZoom;
        private System.Windows.Forms.ToolStripStatusLabel lblResults;
        private System.ComponentModel.BackgroundWorker bgGridLoader;
    }
}