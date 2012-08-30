namespace SampleExtension
{
    partial class AnalyzingFeatures
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
            this.btnCreateBuffer = new System.Windows.Forms.Button();
            this.btnFindFeaturesInBuffer = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(17, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(178, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "This samples demonstrates buffering";
            // 
            // btnCreateBuffer
            // 
            this.btnCreateBuffer.Location = new System.Drawing.Point(20, 80);
            this.btnCreateBuffer.Name = "btnCreateBuffer";
            this.btnCreateBuffer.Size = new System.Drawing.Size(175, 23);
            this.btnCreateBuffer.TabIndex = 1;
            this.btnCreateBuffer.Text = "Create Buffer";
            this.btnCreateBuffer.UseVisualStyleBackColor = true;
            this.btnCreateBuffer.Click += new System.EventHandler(this.btnCreateBuffer_Click);
            // 
            // btnFindFeaturesInBuffer
            // 
            this.btnFindFeaturesInBuffer.Location = new System.Drawing.Point(20, 110);
            this.btnFindFeaturesInBuffer.Name = "btnFindFeaturesInBuffer";
            this.btnFindFeaturesInBuffer.Size = new System.Drawing.Size(175, 23);
            this.btnFindFeaturesInBuffer.TabIndex = 2;
            this.btnFindFeaturesInBuffer.Text = "Find Features in Buffer";
            this.btnFindFeaturesInBuffer.UseVisualStyleBackColor = true;
            this.btnFindFeaturesInBuffer.Click += new System.EventHandler(this.btnFindFeaturesInBuffer_Click);
            // 
            // AnalyzingFeatures
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnFindFeaturesInBuffer);
            this.Controls.Add(this.btnCreateBuffer);
            this.Controls.Add(this.label1);
            this.Name = "AnalyzingFeatures";
            this.Size = new System.Drawing.Size(244, 359);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnCreateBuffer;
        private System.Windows.Forms.Button btnFindFeaturesInBuffer;
    }
}
