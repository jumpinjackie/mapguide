namespace SampleExtension
{
    partial class CustomOutput
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnRenderMap4 = new System.Windows.Forms.Button();
            this.btnRenderMap3 = new System.Windows.Forms.Button();
            this.btnRenderMap2 = new System.Windows.Forms.Button();
            this.btnRenderMap1 = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.btnMultiPlot = new System.Windows.Forms.Button();
            this.btnSinglePlot = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(14, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(264, 52);
            this.label1.TabIndex = 0;
            this.label1.Text = "This sample demonstrates outputting the current map to various output formats wit" +
                "h various display parameters\r\n";
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.btnRenderMap4);
            this.groupBox1.Controls.Add(this.btnRenderMap3);
            this.groupBox1.Controls.Add(this.btnRenderMap2);
            this.groupBox1.Controls.Add(this.btnRenderMap1);
            this.groupBox1.Location = new System.Drawing.Point(17, 84);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(261, 160);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Render custom views of current map";
            // 
            // btnRenderMap4
            // 
            this.btnRenderMap4.Location = new System.Drawing.Point(24, 116);
            this.btnRenderMap4.Name = "btnRenderMap4";
            this.btnRenderMap4.Size = new System.Drawing.Size(177, 23);
            this.btnRenderMap4.TabIndex = 3;
            this.btnRenderMap4.Text = "2048 x 1920 @ 1:7000 (192 DPI)";
            this.btnRenderMap4.UseVisualStyleBackColor = true;
            this.btnRenderMap4.Click += new System.EventHandler(this.btnRenderMap4_Click);
            // 
            // btnRenderMap3
            // 
            this.btnRenderMap3.Location = new System.Drawing.Point(24, 87);
            this.btnRenderMap3.Name = "btnRenderMap3";
            this.btnRenderMap3.Size = new System.Drawing.Size(177, 23);
            this.btnRenderMap3.TabIndex = 2;
            this.btnRenderMap3.Text = "1920 x 1680 @ 1:6000 (192 DPI)";
            this.btnRenderMap3.UseVisualStyleBackColor = true;
            this.btnRenderMap3.Click += new System.EventHandler(this.btnRenderMap3_Click);
            // 
            // btnRenderMap2
            // 
            this.btnRenderMap2.Location = new System.Drawing.Point(24, 58);
            this.btnRenderMap2.Name = "btnRenderMap2";
            this.btnRenderMap2.Size = new System.Drawing.Size(177, 23);
            this.btnRenderMap2.TabIndex = 1;
            this.btnRenderMap2.Text = "1024 x 768 @ 1:5000 (96 DPI)";
            this.btnRenderMap2.UseVisualStyleBackColor = true;
            this.btnRenderMap2.Click += new System.EventHandler(this.btnRenderMap2_Click);
            // 
            // btnRenderMap1
            // 
            this.btnRenderMap1.Location = new System.Drawing.Point(24, 29);
            this.btnRenderMap1.Name = "btnRenderMap1";
            this.btnRenderMap1.Size = new System.Drawing.Size(177, 23);
            this.btnRenderMap1.TabIndex = 0;
            this.btnRenderMap1.Text = "640 x 480 @ 1:4000 (96 DPI)";
            this.btnRenderMap1.UseVisualStyleBackColor = true;
            this.btnRenderMap1.Click += new System.EventHandler(this.btnRenderMap1_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.btnMultiPlot);
            this.groupBox2.Controls.Add(this.btnSinglePlot);
            this.groupBox2.Location = new System.Drawing.Point(17, 251);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(261, 104);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Plot current view to DWF";
            // 
            // btnMultiPlot
            // 
            this.btnMultiPlot.Location = new System.Drawing.Point(24, 58);
            this.btnMultiPlot.Name = "btnMultiPlot";
            this.btnMultiPlot.Size = new System.Drawing.Size(177, 23);
            this.btnMultiPlot.TabIndex = 1;
            this.btnMultiPlot.Text = "Multi-Page plot";
            this.btnMultiPlot.UseVisualStyleBackColor = true;
            this.btnMultiPlot.Click += new System.EventHandler(this.btnMultiPlot_Click);
            // 
            // btnSinglePlot
            // 
            this.btnSinglePlot.Location = new System.Drawing.Point(24, 29);
            this.btnSinglePlot.Name = "btnSinglePlot";
            this.btnSinglePlot.Size = new System.Drawing.Size(177, 23);
            this.btnSinglePlot.TabIndex = 0;
            this.btnSinglePlot.Text = "Single-Page plot";
            this.btnSinglePlot.UseVisualStyleBackColor = true;
            this.btnSinglePlot.Click += new System.EventHandler(this.btnSinglePlot_Click);
            // 
            // CustomOutput
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label1);
            this.Name = "CustomOutput";
            this.Size = new System.Drawing.Size(295, 436);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnRenderMap4;
        private System.Windows.Forms.Button btnRenderMap3;
        private System.Windows.Forms.Button btnRenderMap2;
        private System.Windows.Forms.Button btnRenderMap1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnMultiPlot;
        private System.Windows.Forms.Button btnSinglePlot;
    }
}
