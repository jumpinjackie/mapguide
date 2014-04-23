namespace DotNetHarness.Tile
{
    partial class GetTileControl
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
            this.txtMapDefinition = new System.Windows.Forms.TextBox();
            this.txtBaseGroup = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtScale = new System.Windows.Forms.TextBox();
            this.txtCol = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.txtRow = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.rdViewScale = new System.Windows.Forms.RadioButton();
            this.rdScaleIndex = new System.Windows.Forms.RadioButton();
            this.txtScaleIndex = new System.Windows.Forms.TextBox();
            this.rdRuntimeMap = new System.Windows.Forms.RadioButton();
            this.rdMapDefinition = new System.Windows.Forms.RadioButton();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(31, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(75, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Map Definition";
            // 
            // txtMapDefinition
            // 
            this.txtMapDefinition.Location = new System.Drawing.Point(135, 22);
            this.txtMapDefinition.Name = "txtMapDefinition";
            this.txtMapDefinition.Size = new System.Drawing.Size(375, 20);
            this.txtMapDefinition.TabIndex = 1;
            this.txtMapDefinition.Text = "Library://Samples/Sheboygan/MapsTiled/Sheboygan.MapDefinition";
            // 
            // txtBaseGroup
            // 
            this.txtBaseGroup.Location = new System.Drawing.Point(135, 48);
            this.txtBaseGroup.Name = "txtBaseGroup";
            this.txtBaseGroup.Size = new System.Drawing.Size(375, 20);
            this.txtBaseGroup.TabIndex = 3;
            this.txtBaseGroup.Text = "Base Layer Group";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(31, 51);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(92, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Base Layer Group";
            // 
            // txtScale
            // 
            this.txtScale.Location = new System.Drawing.Point(135, 74);
            this.txtScale.Name = "txtScale";
            this.txtScale.Size = new System.Drawing.Size(63, 20);
            this.txtScale.TabIndex = 5;
            this.txtScale.Text = "75000";
            // 
            // txtCol
            // 
            this.txtCol.Location = new System.Drawing.Point(335, 100);
            this.txtCol.Name = "txtCol";
            this.txtCol.Size = new System.Drawing.Size(63, 20);
            this.txtCol.TabIndex = 7;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(231, 103);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(42, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Column";
            // 
            // txtRow
            // 
            this.txtRow.Location = new System.Drawing.Point(135, 100);
            this.txtRow.Name = "txtRow";
            this.txtRow.Size = new System.Drawing.Size(63, 20);
            this.txtRow.TabIndex = 9;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(31, 103);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(29, 13);
            this.label5.TabIndex = 8;
            this.label5.Text = "Row";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(435, 98);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 10;
            this.button1.Text = "Go";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // rdViewScale
            // 
            this.rdViewScale.AutoSize = true;
            this.rdViewScale.Location = new System.Drawing.Point(34, 75);
            this.rdViewScale.Name = "rdViewScale";
            this.rdViewScale.Size = new System.Drawing.Size(78, 17);
            this.rdViewScale.TabIndex = 11;
            this.rdViewScale.Text = "View Scale";
            this.rdViewScale.UseVisualStyleBackColor = true;
            // 
            // rdScaleIndex
            // 
            this.rdScaleIndex.AutoSize = true;
            this.rdScaleIndex.Location = new System.Drawing.Point(234, 75);
            this.rdScaleIndex.Name = "rdScaleIndex";
            this.rdScaleIndex.Size = new System.Drawing.Size(81, 17);
            this.rdScaleIndex.TabIndex = 12;
            this.rdScaleIndex.Text = "Scale Index";
            this.rdScaleIndex.UseVisualStyleBackColor = true;
            // 
            // txtScaleIndex
            // 
            this.txtScaleIndex.Location = new System.Drawing.Point(335, 74);
            this.txtScaleIndex.Name = "txtScaleIndex";
            this.txtScaleIndex.Size = new System.Drawing.Size(63, 20);
            this.txtScaleIndex.TabIndex = 13;
            // 
            // rdRuntimeMap
            // 
            this.rdRuntimeMap.AutoSize = true;
            this.rdRuntimeMap.Checked = true;
            this.rdRuntimeMap.Location = new System.Drawing.Point(11, 15);
            this.rdRuntimeMap.Name = "rdRuntimeMap";
            this.rdRuntimeMap.Size = new System.Drawing.Size(148, 17);
            this.rdRuntimeMap.TabIndex = 14;
            this.rdRuntimeMap.TabStop = true;
            this.rdRuntimeMap.Text = "Use runtime map overload";
            this.rdRuntimeMap.UseVisualStyleBackColor = true;
            // 
            // rdMapDefinition
            // 
            this.rdMapDefinition.AutoSize = true;
            this.rdMapDefinition.Location = new System.Drawing.Point(11, 39);
            this.rdMapDefinition.Name = "rdMapDefinition";
            this.rdMapDefinition.Size = new System.Drawing.Size(156, 17);
            this.rdMapDefinition.TabIndex = 15;
            this.rdMapDefinition.Text = "Use map definition overload";
            this.rdMapDefinition.UseVisualStyleBackColor = true;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.rdRuntimeMap);
            this.panel1.Controls.Add(this.rdMapDefinition);
            this.panel1.Location = new System.Drawing.Point(25, 136);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(200, 75);
            this.panel1.TabIndex = 16;
            // 
            // GetTileControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.txtScaleIndex);
            this.Controls.Add(this.rdScaleIndex);
            this.Controls.Add(this.rdViewScale);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.txtRow);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.txtCol);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.txtScale);
            this.Controls.Add(this.txtBaseGroup);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtMapDefinition);
            this.Controls.Add(this.label1);
            this.Name = "GetTileControl";
            this.Size = new System.Drawing.Size(639, 246);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtMapDefinition;
        private System.Windows.Forms.TextBox txtBaseGroup;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtScale;
        private System.Windows.Forms.TextBox txtCol;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtRow;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.RadioButton rdViewScale;
        private System.Windows.Forms.RadioButton rdScaleIndex;
        private System.Windows.Forms.TextBox txtScaleIndex;
        private System.Windows.Forms.RadioButton rdRuntimeMap;
        private System.Windows.Forms.RadioButton rdMapDefinition;
        private System.Windows.Forms.Panel panel1;
    }
}
