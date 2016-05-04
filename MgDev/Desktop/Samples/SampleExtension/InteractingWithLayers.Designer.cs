namespace SampleExtension
{
    partial class InteractingWithLayers
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(InteractingWithLayers));
            this.label1 = new System.Windows.Forms.Label();
            this.btnLayerVisibility = new System.Windows.Forms.Button();
            this.btnRenameRoadsLayer = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(15, 19);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(180, 189);
            this.label1.TabIndex = 2;
            this.label1.Text = resources.GetString("label1.Text");
            // 
            // btnLayerVisibility
            // 
            this.btnLayerVisibility.Location = new System.Drawing.Point(18, 223);
            this.btnLayerVisibility.Name = "btnLayerVisibility";
            this.btnLayerVisibility.Size = new System.Drawing.Size(177, 23);
            this.btnLayerVisibility.TabIndex = 3;
            this.btnLayerVisibility.Text = "Layer Visibility";
            this.btnLayerVisibility.UseVisualStyleBackColor = true;
            this.btnLayerVisibility.Click += new System.EventHandler(this.btnLayerVisibility_Click);
            // 
            // btnRenameRoadsLayer
            // 
            this.btnRenameRoadsLayer.Location = new System.Drawing.Point(18, 252);
            this.btnRenameRoadsLayer.Name = "btnRenameRoadsLayer";
            this.btnRenameRoadsLayer.Size = new System.Drawing.Size(177, 23);
            this.btnRenameRoadsLayer.TabIndex = 4;
            this.btnRenameRoadsLayer.Text = "Rename Roads Layer to Streets";
            this.btnRenameRoadsLayer.UseVisualStyleBackColor = true;
            this.btnRenameRoadsLayer.Click += new System.EventHandler(this.btnRenameRoadsLayer_Click);
            // 
            // InteractingWithLayers
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnRenameRoadsLayer);
            this.Controls.Add(this.btnLayerVisibility);
            this.Controls.Add(this.label1);
            this.Name = "InteractingWithLayers";
            this.Size = new System.Drawing.Size(216, 362);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnLayerVisibility;
        private System.Windows.Forms.Button btnRenameRoadsLayer;
    }
}
