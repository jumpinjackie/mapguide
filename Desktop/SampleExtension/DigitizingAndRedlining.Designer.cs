namespace SampleExtension
{
    partial class DigitizingAndRedlining
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
            this.btnPoint = new System.Windows.Forms.Button();
            this.btnLine = new System.Windows.Forms.Button();
            this.btnLineString = new System.Windows.Forms.Button();
            this.btnPolygon = new System.Windows.Forms.Button();
            this.btnClearRedlines = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(18, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(201, 50);
            this.label1.TabIndex = 0;
            this.label1.Text = "This sample demonstrates digitizing and redlining";
            // 
            // btnPoint
            // 
            this.btnPoint.Location = new System.Drawing.Point(21, 89);
            this.btnPoint.Name = "btnPoint";
            this.btnPoint.Size = new System.Drawing.Size(80, 23);
            this.btnPoint.TabIndex = 1;
            this.btnPoint.Text = "Point";
            this.btnPoint.UseVisualStyleBackColor = true;
            this.btnPoint.Click += new System.EventHandler(this.btnPoint_Click);
            // 
            // btnLine
            // 
            this.btnLine.Location = new System.Drawing.Point(21, 118);
            this.btnLine.Name = "btnLine";
            this.btnLine.Size = new System.Drawing.Size(80, 23);
            this.btnLine.TabIndex = 2;
            this.btnLine.Text = "Line";
            this.btnLine.UseVisualStyleBackColor = true;
            this.btnLine.Click += new System.EventHandler(this.btnLine_Click);
            // 
            // btnLineString
            // 
            this.btnLineString.Location = new System.Drawing.Point(107, 89);
            this.btnLineString.Name = "btnLineString";
            this.btnLineString.Size = new System.Drawing.Size(80, 23);
            this.btnLineString.TabIndex = 3;
            this.btnLineString.Text = "LineString";
            this.btnLineString.UseVisualStyleBackColor = true;
            this.btnLineString.Click += new System.EventHandler(this.btnLineString_Click);
            // 
            // btnPolygon
            // 
            this.btnPolygon.Location = new System.Drawing.Point(107, 118);
            this.btnPolygon.Name = "btnPolygon";
            this.btnPolygon.Size = new System.Drawing.Size(80, 23);
            this.btnPolygon.TabIndex = 4;
            this.btnPolygon.Text = "Polygon";
            this.btnPolygon.UseVisualStyleBackColor = true;
            this.btnPolygon.Click += new System.EventHandler(this.btnPolygon_Click);
            // 
            // btnClearRedlines
            // 
            this.btnClearRedlines.Location = new System.Drawing.Point(21, 178);
            this.btnClearRedlines.Name = "btnClearRedlines";
            this.btnClearRedlines.Size = new System.Drawing.Size(166, 23);
            this.btnClearRedlines.TabIndex = 5;
            this.btnClearRedlines.Text = "Clear Redlines";
            this.btnClearRedlines.UseVisualStyleBackColor = true;
            this.btnClearRedlines.Click += new System.EventHandler(this.btnClearRedlines_Click);
            // 
            // DigitizingAndRedlining
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnClearRedlines);
            this.Controls.Add(this.btnPolygon);
            this.Controls.Add(this.btnLineString);
            this.Controls.Add(this.btnLine);
            this.Controls.Add(this.btnPoint);
            this.Controls.Add(this.label1);
            this.Name = "DigitizingAndRedlining";
            this.Size = new System.Drawing.Size(236, 472);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnPoint;
        private System.Windows.Forms.Button btnLine;
        private System.Windows.Forms.Button btnLineString;
        private System.Windows.Forms.Button btnPolygon;
        private System.Windows.Forms.Button btnClearRedlines;
    }
}
