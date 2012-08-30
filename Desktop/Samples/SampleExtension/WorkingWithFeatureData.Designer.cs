namespace SampleExtension
{
    partial class WorkingWithFeatureData
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
            this.btnZoomToDistrict1 = new System.Windows.Forms.Button();
            this.btnQueryDistrict1 = new System.Windows.Forms.Button();
            this.btnSelectDistrict1 = new System.Windows.Forms.Button();
            this.btnListSelected = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(15, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(210, 58);
            this.label1.TabIndex = 0;
            this.label1.Text = "This sample demonstrates basic feature querying and programmatic manipulation of " +
                "the active selection";
            // 
            // btnZoomToDistrict1
            // 
            this.btnZoomToDistrict1.Location = new System.Drawing.Point(27, 95);
            this.btnZoomToDistrict1.Name = "btnZoomToDistrict1";
            this.btnZoomToDistrict1.Size = new System.Drawing.Size(167, 23);
            this.btnZoomToDistrict1.TabIndex = 1;
            this.btnZoomToDistrict1.Text = "Zoom to District 1";
            this.btnZoomToDistrict1.UseVisualStyleBackColor = true;
            this.btnZoomToDistrict1.Click += new System.EventHandler(this.btnZoomToDistrict1_Click);
            // 
            // btnQueryDistrict1
            // 
            this.btnQueryDistrict1.Location = new System.Drawing.Point(27, 124);
            this.btnQueryDistrict1.Name = "btnQueryDistrict1";
            this.btnQueryDistrict1.Size = new System.Drawing.Size(167, 42);
            this.btnQueryDistrict1.TabIndex = 2;
            this.btnQueryDistrict1.Text = "Query: Properties in District 1 owned by Schmitt";
            this.btnQueryDistrict1.UseVisualStyleBackColor = true;
            this.btnQueryDistrict1.Click += new System.EventHandler(this.btnQueryDistrict1_Click);
            // 
            // btnSelectDistrict1
            // 
            this.btnSelectDistrict1.Location = new System.Drawing.Point(27, 172);
            this.btnSelectDistrict1.Name = "btnSelectDistrict1";
            this.btnSelectDistrict1.Size = new System.Drawing.Size(167, 42);
            this.btnSelectDistrict1.TabIndex = 3;
            this.btnSelectDistrict1.Text = "Select: Properties in District 1 owned by Schmitt";
            this.btnSelectDistrict1.UseVisualStyleBackColor = true;
            this.btnSelectDistrict1.Click += new System.EventHandler(this.btnSelectDistrict1_Click);
            // 
            // btnListSelected
            // 
            this.btnListSelected.Location = new System.Drawing.Point(27, 220);
            this.btnListSelected.Name = "btnListSelected";
            this.btnListSelected.Size = new System.Drawing.Size(167, 27);
            this.btnListSelected.TabIndex = 4;
            this.btnListSelected.Text = "List Selected Features";
            this.btnListSelected.UseVisualStyleBackColor = true;
            this.btnListSelected.Click += new System.EventHandler(this.btnListSelected_Click);
            // 
            // WorkingWithFeatureData
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnListSelected);
            this.Controls.Add(this.btnSelectDistrict1);
            this.Controls.Add(this.btnQueryDistrict1);
            this.Controls.Add(this.btnZoomToDistrict1);
            this.Controls.Add(this.label1);
            this.Name = "WorkingWithFeatureData";
            this.Size = new System.Drawing.Size(238, 359);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnZoomToDistrict1;
        private System.Windows.Forms.Button btnQueryDistrict1;
        private System.Windows.Forms.Button btnSelectDistrict1;
        private System.Windows.Forms.Button btnListSelected;
    }
}
