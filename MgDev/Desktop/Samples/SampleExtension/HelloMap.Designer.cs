namespace SampleExtension
{
    partial class HelloMap
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
            this.btnDisplaySpatialReference = new System.Windows.Forms.Button();
            this.btnListLayers = new System.Windows.Forms.Button();
            this.btnListLayerGroups = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(19, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(203, 64);
            this.label1.TabIndex = 0;
            this.label1.Text = "This sample shows the most basic interaction with a map. It obtains a reference t" +
                "o the current map and queries information about it";
            // 
            // btnDisplaySpatialReference
            // 
            this.btnDisplaySpatialReference.Location = new System.Drawing.Point(22, 103);
            this.btnDisplaySpatialReference.Name = "btnDisplaySpatialReference";
            this.btnDisplaySpatialReference.Size = new System.Drawing.Size(186, 23);
            this.btnDisplaySpatialReference.TabIndex = 1;
            this.btnDisplaySpatialReference.Text = "Display Spatial Reference";
            this.btnDisplaySpatialReference.UseVisualStyleBackColor = true;
            this.btnDisplaySpatialReference.Click += new System.EventHandler(this.btnDisplaySpatialReference_Click);
            // 
            // btnListLayers
            // 
            this.btnListLayers.Location = new System.Drawing.Point(22, 132);
            this.btnListLayers.Name = "btnListLayers";
            this.btnListLayers.Size = new System.Drawing.Size(186, 23);
            this.btnListLayers.TabIndex = 2;
            this.btnListLayers.Text = "List Layers";
            this.btnListLayers.UseVisualStyleBackColor = true;
            this.btnListLayers.Click += new System.EventHandler(this.btnListLayers_Click);
            // 
            // btnListLayerGroups
            // 
            this.btnListLayerGroups.Location = new System.Drawing.Point(22, 162);
            this.btnListLayerGroups.Name = "btnListLayerGroups";
            this.btnListLayerGroups.Size = new System.Drawing.Size(186, 23);
            this.btnListLayerGroups.TabIndex = 3;
            this.btnListLayerGroups.Text = "List Layer Groups";
            this.btnListLayerGroups.UseVisualStyleBackColor = true;
            this.btnListLayerGroups.Click += new System.EventHandler(this.btnListLayerGroups_Click);
            // 
            // HelloMap
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnListLayerGroups);
            this.Controls.Add(this.btnListLayers);
            this.Controls.Add(this.btnDisplaySpatialReference);
            this.Controls.Add(this.label1);
            this.Name = "HelloMap";
            this.Size = new System.Drawing.Size(242, 402);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnDisplaySpatialReference;
        private System.Windows.Forms.Button btnListLayers;
        private System.Windows.Forms.Button btnListLayerGroups;
    }
}
