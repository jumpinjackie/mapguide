namespace SampleExtension
{
    partial class LayerVisibilityDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LayerVisibilityDialog));
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.COL_IS_VISIBLE = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.COL_GET_VISIBLE = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.COL_LAYER = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.grdLayers = new System.Windows.Forms.DataGridView();
            ((System.ComponentModel.ISupportInitialize)(this.grdLayers)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(25, 227);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(354, 105);
            this.label1.TabIndex = 1;
            this.label1.Text = resources.GetString("label1.Text");
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(22, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(102, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Layers in draw order";
            // 
            // COL_IS_VISIBLE
            // 
            this.COL_IS_VISIBLE.HeaderText = "IsVisible()";
            this.COL_IS_VISIBLE.Name = "COL_IS_VISIBLE";
            this.COL_IS_VISIBLE.ReadOnly = true;
            // 
            // COL_GET_VISIBLE
            // 
            this.COL_GET_VISIBLE.HeaderText = "GetVisible()";
            this.COL_GET_VISIBLE.Name = "COL_GET_VISIBLE";
            this.COL_GET_VISIBLE.ReadOnly = true;
            // 
            // COL_LAYER
            // 
            this.COL_LAYER.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.COL_LAYER.HeaderText = "Layer";
            this.COL_LAYER.Name = "COL_LAYER";
            this.COL_LAYER.ReadOnly = true;
            // 
            // grdLayers
            // 
            this.grdLayers.AllowUserToAddRows = false;
            this.grdLayers.AllowUserToDeleteRows = false;
            this.grdLayers.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.grdLayers.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.grdLayers.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.COL_LAYER,
            this.COL_GET_VISIBLE,
            this.COL_IS_VISIBLE});
            this.grdLayers.Location = new System.Drawing.Point(25, 39);
            this.grdLayers.Name = "grdLayers";
            this.grdLayers.ReadOnly = true;
            this.grdLayers.Size = new System.Drawing.Size(354, 185);
            this.grdLayers.TabIndex = 0;
            // 
            // LayerVisibilityDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(413, 341);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.grdLayers);
            this.Name = "LayerVisibilityDialog";
            this.Text = "Layer Visibility";
            ((System.ComponentModel.ISupportInitialize)(this.grdLayers)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.DataGridViewTextBoxColumn COL_IS_VISIBLE;
        private System.Windows.Forms.DataGridViewTextBoxColumn COL_GET_VISIBLE;
        private System.Windows.Forms.DataGridViewTextBoxColumn COL_LAYER;
        private System.Windows.Forms.DataGridView grdLayers;
    }
}