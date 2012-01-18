namespace DotNetHarness.Feature
{
    partial class SelectFeaturesControl
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
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.txtFeatureSource = new System.Windows.Forms.TextBox();
            this.txtClass = new System.Windows.Forms.TextBox();
            this.txtProps = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.txtFilter = new System.Windows.Forms.TextBox();
            this.lstComputed = new System.Windows.Forms.ListBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.txtAlias = new System.Windows.Forms.TextBox();
            this.txtExpr = new System.Windows.Forms.TextBox();
            this.btnAddExpr = new System.Windows.Forms.Button();
            this.btnRemoveExpr = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(26, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(80, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Feature Source";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(26, 49);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(253, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Class (qualify if feature source has multiple schemas)";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(26, 80);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(196, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Property Names (separate with commas)";
            // 
            // txtFeatureSource
            // 
            this.txtFeatureSource.Location = new System.Drawing.Point(144, 15);
            this.txtFeatureSource.Name = "txtFeatureSource";
            this.txtFeatureSource.Size = new System.Drawing.Size(427, 20);
            this.txtFeatureSource.TabIndex = 3;
            // 
            // txtClass
            // 
            this.txtClass.Location = new System.Drawing.Point(294, 46);
            this.txtClass.Name = "txtClass";
            this.txtClass.Size = new System.Drawing.Size(277, 20);
            this.txtClass.TabIndex = 4;
            // 
            // txtProps
            // 
            this.txtProps.Location = new System.Drawing.Point(237, 77);
            this.txtProps.Multiline = true;
            this.txtProps.Name = "txtProps";
            this.txtProps.Size = new System.Drawing.Size(334, 65);
            this.txtProps.TabIndex = 5;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(29, 226);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(29, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Filter";
            // 
            // txtFilter
            // 
            this.txtFilter.Location = new System.Drawing.Point(144, 223);
            this.txtFilter.Name = "txtFilter";
            this.txtFilter.Size = new System.Drawing.Size(427, 20);
            this.txtFilter.TabIndex = 7;
            // 
            // lstComputed
            // 
            this.lstComputed.FormattingEnabled = true;
            this.lstComputed.Location = new System.Drawing.Point(331, 148);
            this.lstComputed.Name = "lstComputed";
            this.lstComputed.Size = new System.Drawing.Size(240, 69);
            this.lstComputed.TabIndex = 8;
            this.lstComputed.SelectedIndexChanged += new System.EventHandler(this.listBox1_SelectedIndexChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(29, 148);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(105, 13);
            this.label5.TabIndex = 9;
            this.label5.Text = "Computed Properties";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(66, 167);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(29, 13);
            this.label6.TabIndex = 10;
            this.label6.Text = "Alias";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(66, 192);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(58, 13);
            this.label7.TabIndex = 11;
            this.label7.Text = "Expression";
            // 
            // txtAlias
            // 
            this.txtAlias.Location = new System.Drawing.Point(135, 164);
            this.txtAlias.Name = "txtAlias";
            this.txtAlias.Size = new System.Drawing.Size(100, 20);
            this.txtAlias.TabIndex = 12;
            // 
            // txtExpr
            // 
            this.txtExpr.Location = new System.Drawing.Point(135, 189);
            this.txtExpr.Name = "txtExpr";
            this.txtExpr.Size = new System.Drawing.Size(100, 20);
            this.txtExpr.TabIndex = 13;
            // 
            // btnAddExpr
            // 
            this.btnAddExpr.Location = new System.Drawing.Point(250, 162);
            this.btnAddExpr.Name = "btnAddExpr";
            this.btnAddExpr.Size = new System.Drawing.Size(75, 23);
            this.btnAddExpr.TabIndex = 14;
            this.btnAddExpr.Text = ">>";
            this.btnAddExpr.UseVisualStyleBackColor = true;
            this.btnAddExpr.Click += new System.EventHandler(this.btnAddExpr_Click);
            // 
            // btnRemoveExpr
            // 
            this.btnRemoveExpr.Enabled = false;
            this.btnRemoveExpr.Location = new System.Drawing.Point(250, 187);
            this.btnRemoveExpr.Name = "btnRemoveExpr";
            this.btnRemoveExpr.Size = new System.Drawing.Size(75, 23);
            this.btnRemoveExpr.TabIndex = 15;
            this.btnRemoveExpr.Text = "<<";
            this.btnRemoveExpr.UseVisualStyleBackColor = true;
            this.btnRemoveExpr.Click += new System.EventHandler(this.btnRemoveExpr_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(496, 249);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 16;
            this.button1.Text = "Go";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // SelectFeaturesControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.button1);
            this.Controls.Add(this.btnRemoveExpr);
            this.Controls.Add(this.btnAddExpr);
            this.Controls.Add(this.txtExpr);
            this.Controls.Add(this.txtAlias);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.lstComputed);
            this.Controls.Add(this.txtFilter);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.txtProps);
            this.Controls.Add(this.txtClass);
            this.Controls.Add(this.txtFeatureSource);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "SelectFeaturesControl";
            this.Size = new System.Drawing.Size(641, 361);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtFeatureSource;
        private System.Windows.Forms.TextBox txtClass;
        private System.Windows.Forms.TextBox txtProps;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtFilter;
        private System.Windows.Forms.ListBox lstComputed;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox txtAlias;
        private System.Windows.Forms.TextBox txtExpr;
        private System.Windows.Forms.Button btnAddExpr;
        private System.Windows.Forms.Button btnRemoveExpr;
        private System.Windows.Forms.Button button1;
    }
}
