namespace DotNetHarness
{
    partial class Form1
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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.label2 = new System.Windows.Forms.Label();
            this.lstOperations = new System.Windows.Forms.ListBox();
            this.lstApiCategories = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.label2);
            this.splitContainer1.Panel1.Controls.Add(this.lstOperations);
            this.splitContainer1.Panel1.Controls.Add(this.lstApiCategories);
            this.splitContainer1.Panel1.Controls.Add(this.label1);
            this.splitContainer1.Size = new System.Drawing.Size(918, 591);
            this.splitContainer1.SplitterDistance = 189;
            this.splitContainer1.TabIndex = 0;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 132);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(58, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Operations";
            // 
            // lstOperations
            // 
            this.lstOperations.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lstOperations.FormattingEnabled = true;
            this.lstOperations.Location = new System.Drawing.Point(12, 148);
            this.lstOperations.Name = "lstOperations";
            this.lstOperations.Size = new System.Drawing.Size(161, 407);
            this.lstOperations.TabIndex = 2;
            this.lstOperations.SelectedIndexChanged += new System.EventHandler(this.lstOperations_SelectedIndexChanged);
            // 
            // lstApiCategories
            // 
            this.lstApiCategories.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lstApiCategories.FormattingEnabled = true;
            this.lstApiCategories.Items.AddRange(new object[] {
            "Resource",
            "Feature",
            "Rendering",
            "Tile"});
            this.lstApiCategories.Location = new System.Drawing.Point(12, 25);
            this.lstApiCategories.Name = "lstApiCategories";
            this.lstApiCategories.Size = new System.Drawing.Size(161, 95);
            this.lstApiCategories.TabIndex = 1;
            this.lstApiCategories.SelectedIndexChanged += new System.EventHandler(this.lstApiCategories_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(57, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Categories";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(918, 591);
            this.Controls.Add(this.splitContainer1);
            this.Name = "Form1";
            this.Text = "MapGuide Desktop API Test Harness";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ListBox lstOperations;
        private System.Windows.Forms.ListBox lstApiCategories;
        private System.Windows.Forms.Label label1;
    }
}

