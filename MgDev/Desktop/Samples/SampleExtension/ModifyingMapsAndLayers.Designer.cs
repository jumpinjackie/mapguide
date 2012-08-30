namespace SampleExtension
{
    partial class ModifyingMapsAndLayers
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
            this.btnShowBuildings = new System.Windows.Forms.Button();
            this.btnCreateSquareFootage = new System.Windows.Forms.Button();
            this.btnCreateHydroLine = new System.Windows.Forms.Button();
            this.btnCreatePointsOfInterest = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(15, 23);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(282, 58);
            this.label1.TabIndex = 1;
            this.label1.Text = "This sample demonstrates basic map and layer manipulation";
            // 
            // btnShowBuildings
            // 
            this.btnShowBuildings.Location = new System.Drawing.Point(18, 102);
            this.btnShowBuildings.Name = "btnShowBuildings";
            this.btnShowBuildings.Size = new System.Drawing.Size(217, 23);
            this.btnShowBuildings.TabIndex = 2;
            this.btnShowBuildings.Text = "Show buildings built after 1980";
            this.btnShowBuildings.UseVisualStyleBackColor = true;
            this.btnShowBuildings.Click += new System.EventHandler(this.btnShowBuildings_Click);
            // 
            // btnCreateSquareFootage
            // 
            this.btnCreateSquareFootage.Location = new System.Drawing.Point(18, 131);
            this.btnCreateSquareFootage.Name = "btnCreateSquareFootage";
            this.btnCreateSquareFootage.Size = new System.Drawing.Size(217, 23);
            this.btnCreateSquareFootage.TabIndex = 3;
            this.btnCreateSquareFootage.Text = "Create square footage layer";
            this.btnCreateSquareFootage.UseVisualStyleBackColor = true;
            this.btnCreateSquareFootage.Click += new System.EventHandler(this.btnCreateSquareFootage_Click);
            // 
            // btnCreateHydroLine
            // 
            this.btnCreateHydroLine.Location = new System.Drawing.Point(18, 160);
            this.btnCreateHydroLine.Name = "btnCreateHydroLine";
            this.btnCreateHydroLine.Size = new System.Drawing.Size(217, 23);
            this.btnCreateHydroLine.TabIndex = 4;
            this.btnCreateHydroLine.Text = "Create hydro line layer";
            this.btnCreateHydroLine.UseVisualStyleBackColor = true;
            this.btnCreateHydroLine.Click += new System.EventHandler(this.btnCreateHydroLine_Click);
            // 
            // btnCreatePointsOfInterest
            // 
            this.btnCreatePointsOfInterest.Location = new System.Drawing.Point(18, 189);
            this.btnCreatePointsOfInterest.Name = "btnCreatePointsOfInterest";
            this.btnCreatePointsOfInterest.Size = new System.Drawing.Size(217, 23);
            this.btnCreatePointsOfInterest.TabIndex = 5;
            this.btnCreatePointsOfInterest.Text = "Create points of interest layer";
            this.btnCreatePointsOfInterest.UseVisualStyleBackColor = true;
            this.btnCreatePointsOfInterest.Click += new System.EventHandler(this.btnCreatePointsOfInterest_Click);
            // 
            // ModifyingMapsAndLayers
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnCreatePointsOfInterest);
            this.Controls.Add(this.btnCreateHydroLine);
            this.Controls.Add(this.btnCreateSquareFootage);
            this.Controls.Add(this.btnShowBuildings);
            this.Controls.Add(this.label1);
            this.Name = "ModifyingMapsAndLayers";
            this.Size = new System.Drawing.Size(314, 419);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnShowBuildings;
        private System.Windows.Forms.Button btnCreateSquareFootage;
        private System.Windows.Forms.Button btnCreateHydroLine;
        private System.Windows.Forms.Button btnCreatePointsOfInterest;
    }
}
