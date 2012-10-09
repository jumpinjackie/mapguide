namespace SampleExtension
{
    partial class HelloViewer
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
            this.label1 = new System.Windows.Forms.Label();
            this.btnZoomToLocation = new System.Windows.Forms.Button();
            this.btnClearSelection = new System.Windows.Forms.Button();
            this.lblViewerBusyState = new System.Windows.Forms.Label();
            this.btnCopyMap = new System.Windows.Forms.Button();
            this.btnInitialView = new System.Windows.Forms.Button();
            this.lblActiveTool = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(18, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(247, 109);
            this.label1.TabIndex = 1;
            this.label1.Text = "This sample demonstrates viewer interaction\r\n\r\nAll MgComponent instances contain " +
                "a reference to the viewer (IMapViewer)";
            // 
            // btnZoomToLocation
            // 
            this.btnZoomToLocation.Location = new System.Drawing.Point(21, 198);
            this.btnZoomToLocation.Name = "btnZoomToLocation";
            this.btnZoomToLocation.Size = new System.Drawing.Size(181, 23);
            this.btnZoomToLocation.TabIndex = 2;
            this.btnZoomToLocation.Text = "Zoom To Location";
            this.btnZoomToLocation.UseVisualStyleBackColor = true;
            this.btnZoomToLocation.Click += new System.EventHandler(this.btnZoomToLocation_Click);
            // 
            // btnClearSelection
            // 
            this.btnClearSelection.Location = new System.Drawing.Point(21, 227);
            this.btnClearSelection.Name = "btnClearSelection";
            this.btnClearSelection.Size = new System.Drawing.Size(181, 23);
            this.btnClearSelection.TabIndex = 4;
            this.btnClearSelection.Text = "Clear Selection";
            this.btnClearSelection.UseVisualStyleBackColor = true;
            this.btnClearSelection.Click += new System.EventHandler(this.btnClearSelection_Click);
            // 
            // lblViewerBusyState
            // 
            this.lblViewerBusyState.AutoSize = true;
            this.lblViewerBusyState.Location = new System.Drawing.Point(18, 138);
            this.lblViewerBusyState.Name = "lblViewerBusyState";
            this.lblViewerBusyState.Size = new System.Drawing.Size(0, 13);
            this.lblViewerBusyState.TabIndex = 5;
            // 
            // btnCopyMap
            // 
            this.btnCopyMap.Location = new System.Drawing.Point(21, 256);
            this.btnCopyMap.Name = "btnCopyMap";
            this.btnCopyMap.Size = new System.Drawing.Size(181, 23);
            this.btnCopyMap.TabIndex = 6;
            this.btnCopyMap.Text = "Copy Map to Clipboard";
            this.btnCopyMap.UseVisualStyleBackColor = true;
            this.btnCopyMap.Click += new System.EventHandler(this.btnCopyMap_Click);
            // 
            // btnInitialView
            // 
            this.btnInitialView.Location = new System.Drawing.Point(21, 285);
            this.btnInitialView.Name = "btnInitialView";
            this.btnInitialView.Size = new System.Drawing.Size(181, 23);
            this.btnInitialView.TabIndex = 7;
            this.btnInitialView.Text = "Initial View";
            this.btnInitialView.UseVisualStyleBackColor = true;
            this.btnInitialView.Click += new System.EventHandler(this.btnInitialView_Click);
            // 
            // lblActiveTool
            // 
            this.lblActiveTool.AutoSize = true;
            this.lblActiveTool.Location = new System.Drawing.Point(18, 167);
            this.lblActiveTool.Name = "lblActiveTool";
            this.lblActiveTool.Size = new System.Drawing.Size(0, 13);
            this.lblActiveTool.TabIndex = 8;
            // 
            // HelloViewer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.lblActiveTool);
            this.Controls.Add(this.btnInitialView);
            this.Controls.Add(this.btnCopyMap);
            this.Controls.Add(this.lblViewerBusyState);
            this.Controls.Add(this.btnClearSelection);
            this.Controls.Add(this.btnZoomToLocation);
            this.Controls.Add(this.label1);
            this.Name = "HelloViewer";
            this.Size = new System.Drawing.Size(283, 330);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnZoomToLocation;
        private System.Windows.Forms.Button btnClearSelection;
        private System.Windows.Forms.Label lblViewerBusyState;
        private System.Windows.Forms.Button btnCopyMap;
        private System.Windows.Forms.Button btnInitialView;
        private System.Windows.Forms.Label lblActiveTool;
    }
}
